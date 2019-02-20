#include <stdio.h>
#include <SDL2/SDL.h>
#include "jamtracker.h"

SDL_Thread *thread;

void extract_channel_data(
	ChannelItem *p, 
	uint8_t *effect, 
	uint8_t *evalue,
	uint8_t *instru,
	uint8_t *pitch,
        uint16_t *period);

typedef struct {
} RowParams;

void set_note_ex2(
		SamplerStatus *smpst,
		uint16_t period[8],
		uint8_t note[8],
		uint8_t smp[8],
		uint8_t volume[8],
		uint32_t offset[8],
		uint8_t tick);

int generate_audio(SamplerStatus *smpst, size_t ns);
void apply_fx(SamplerStatus *smpst, int16_t perinc[8], int8_t volinc[8]);

void adjust_period(SamplerStatus *smpst, uint8_t ch, int8_t value);
void adjust_volume(SamplerStatus *smpst, uint8_t ch, int8_t value);

SDL_atomic_t song_p;

void set_song_position(int p)
{
	SDL_AtomicSet(&song_p, p);
}

int get_song_position()
{
	return SDL_AtomicGet(&song_p);
}


SDL_atomic_t song_command;

void set_song_command(int p)
{
	SDL_AtomicSet(&song_command, p);
}

int get_song_command()
{
	int v = SDL_AtomicGet(&song_command);
	SDL_AtomicCAS(&song_command, v, 0);

	return v;
}


SDL_atomic_t channel_avg[8];

void set_channel_avg(float avg[8])
{
	int j;
	for(j=0;j<8;j++)
		SDL_AtomicSet(&channel_avg[j], (int)(avg[j]/512.0));
}

void get_channel_avg(int avg[8])
{
	int j;
	for(j=0;j<8;j++)
		avg[j] = SDL_AtomicGet(&channel_avg[j]);
}

float average[8];

extern float freq_tab[12];
extern float oct_tab[12]; 

extern Pipe audiostream;

void PipeInit(Pipe *pipe)
{
	pipe->rp=0;
	pipe->wp=0;
	pipe->len=0;
	pipe->notfull = SDL_TRUE; 
	pipe->lock = SDL_CreateMutex();
	pipe->cond = SDL_CreateCond();
	printf("Pipe Init...\n");
}

void PipeSend(Pipe *pipe, uint8_t *src, size_t len)
{
	size_t i=0;
	SDL_LockMutex(pipe->lock);
//	printf("Data To Write: %lu\n", len);
	while(i<len)
	{
		size_t space = PIPESIZE - pipe->len;
	        if(space==0)
			pipe->notfull = SDL_FALSE;	
		if(pipe->notfull==SDL_FALSE) {
//			printf("<FULL>\n");
			SDL_CondWait(pipe->cond, pipe->lock);
		}
		space = PIPESIZE - pipe->len;
//		printf("Writing %lu (%lu)\n", len-i, pipe->len);
		while(space && i < len) {
			pipe->buffer[pipe->wp]=src[i];
			space--;
			pipe->wp = (pipe->wp+1)%PIPESIZE;
			pipe->len++;
			i++;
		}
//		printf("After: %lu (%lu)\n", len-i, pipe->len);
	}
	SDL_UnlockMutex(pipe->lock);
}

size_t PipeReceive(Pipe *pipe, uint8_t *dest, size_t len)
{
	size_t rd = 0;
	SDL_LockMutex(pipe->lock);
//	printf(">>>>>>>>>>> Reading: %lu\n", len);
	while(len>0 && pipe->len>0) {
		dest[rd] = pipe->buffer[pipe->rp];
		pipe->rp = (pipe->rp+1) % PIPESIZE;
		len--;
		pipe->len--;
		rd++;
	}

	if(pipe->len<PIPESIZE && pipe->notfull == SDL_FALSE) {
		pipe->notfull=SDL_TRUE;
		SDL_CondSignal(pipe->cond);
	}

	SDL_UnlockMutex(pipe->lock);
	return rd;
}


static void push_event(uint8_t seq, uint8_t pat, uint8_t pos)
{
	if (SequencerEvent != ((Uint32)-1)) {
 		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
 		event.type = SequencerEvent;
		event.user.code = 0x10000000 | (seq<<16) | (pat<<8) | pos;
		event.user.data1 = NULL;
		event.user.data2 = NULL;
		SDL_PushEvent(&event);
	}
}

SamplerStatus eSamplerStatus;

uint8_t instru[8] = {0,0,0,0, 0,0,0,0};
uint8_t effect[8] = {0,0,0,0, 0,0,0,0};
uint8_t evalue[8] = {0,0,0,0, 0,0,0,0};
uint8_t pitch[8]  = {0,0,0,0, 0,0,0,0};
uint8_t volume[8] = {0,0,0,0, 0,0,0,0};
uint32_t offset[8] = {1,1,1,1, 1,1,1,1};
uint16_t period[8] = {0,0,0,0, 0,0,0,0};
int8_t   volinc[8] = {0,0,0,0, 0,0,0,0};
int16_t  perinc[8] = {0,0,0,0, 0,0,0,0};
int16_t  slide[8]   = {0,0,0,0, 0,0,0,0};
uint16_t slideto[8] = {0,0,0,0, 0,0,0,0};

uint8_t note_delay[8] = {0,0,0,0, 0,0,0,0};
uint8_t note_cut[8]   = {0,0,0,0, 0,0,0,0};

uint8_t note_vib[8]  = {0,0,0,0, 0,0,0,0};
uint8_t note_arp[8]  = {0,0,0,0, 0,0,0,0};

uint8_t vibrato_ptr[8] = {0,0,0,0, 0,0,0,0};

int16_t loop_point[8] = {-1,-1,-1,-1, -1,-1,-1,-1};
int8_t loop_count[8] = {-1,-1,-1,-1, -1,-1,-1,-1};
int8_t row_delay = -1;

SDL_bool sequence_started = SDL_FALSE;

SDL_mutex *lock_wait_start;
SDL_cond *cond_wait_start;

int start_point = 0;

void sequence_wait_start()
{
	printf("Wait for start...\n");
	SDL_LockMutex(lock_wait_start);
	while (!sequence_started) {
        	SDL_CondWait(cond_wait_start, lock_wait_start);
	}
	SDL_UnlockMutex(lock_wait_start);
}

void sequence_start()
{
	printf("Start Received\n");
	SDL_LockMutex(lock_wait_start);
	if (!sequence_started) {
		printf("Can start!\n");
		start_point = 0;
		sequence_started = SDL_TRUE;
		SDL_CondSignal(cond_wait_start);
	}
	SDL_UnlockMutex(lock_wait_start);
}

void sequence_resume(int point)
{
	printf("Resume Received\n");
	SDL_LockMutex(lock_wait_start);
	if (!sequence_started) {
		printf("Can Resume\n");
		start_point = point;
		sequence_started = SDL_TRUE;
		SDL_CondSignal(cond_wait_start);
	}
	SDL_UnlockMutex(lock_wait_start);
}



int sequence_status()
{
	int st = 0;
	SDL_LockMutex(lock_wait_start);
	st = sequence_started ? 1: 0;
	SDL_UnlockMutex(lock_wait_start);
	return st;
}

void sequence_ended()
{
	SDL_LockMutex(lock_wait_start);
	if (sequence_started) {
		sequence_started = SDL_FALSE;
	}
	SDL_UnlockMutex(lock_wait_start);
}

static int play_sequence(SequencerData *s);

/* Very simple thread - counts 0 to 9 delaying 50ms between increments */
static int SequencerThread(void *ptr)
{
	SequencerData *s = (SequencerData *)ptr;
	while(1)
	{
		set_song_position(-1);
		sequence_wait_start();
		printf(">>>>>>>>>>>>>> GO \n");
		play_sequence(s);
		printf(">>>>>>>>>>>>>>>> EXIT\n");
		sequence_ended();
		//break;
	}
	return 0;
}


static void sequence_reset(SamplerStatus *smpst, SequencerData *s)
{
	int j;
	for (j=0;j<8;j++)
	{
		average[j] = 0.0;
		instru[j] = 0;
		effect[j] = 0;
		evalue[j] = 0;
		pitch[j]  = 0;
		volume[j] = 0;
		offset[j] = 1;
		period[j] = 0;
		volinc[j] = 0;
		perinc[j] = 0;
		slide[j]   = 0;
		slideto[j] = 0;

		note_delay[j] = 0;
		note_cut[j]   = 0;

		note_vib[j]  = 0;
		note_arp[j]  = 0;

		vibrato_ptr[j] = 0;

		loop_point[j] = -1;
		loop_count[j] = -1;
	}
	row_delay = -1;

	memset(smpst, 0, sizeof(SamplerStatus));
	smpst->smpdata = s->smpdata;
}

static int play_sequence(SequencerData *s)
{
    int cnt = 0;

    SamplerStatus *smpst = &eSamplerStatus;
 
	int n=0, o_n=-1;
	uint8_t ticks = 6;
	uint8_t delta = 20;
	unsigned int currentTime = SDL_GetTicks();
	unsigned int lastTime = currentTime;
	int i=0, j;
	int16_t cursor=0;

	set_song_position(0);

	sequence_reset(smpst, s);

	printf("PTR Received: %p %p %d\n", s->seq, s->smpdata, (int)s->npat);

	if (start_point) {
		cursor = ((start_point >> 16) & 0xFF)*64 + (start_point & 0xFF);
		printf("Start Point: %08X %08X\n", start_point, cursor);
	}

	int action  = 0;
	int lastseq = 0;
	while(1) {
		int j;
		int next_step;
		int pat;
		int cmd;
		int skip = 0;

		if (cursor<0)
			cursor=0;


		int pause = 0;

		do {
			cmd = get_song_command();

			switch (cmd) {
			case SONG_BREAK:
				skip  = 1;
				pause = 0;
				break;
			case SONG_RESTART:
				cursor = 0;
				sequence_reset(smpst, s);
				ticks = 6;
				delta = 20;
				pause = 0;
				break;
			case SONG_PAUSE:
				pause = !pause;
				break;
			default:
				if ( cmd>=SONG_EVENT_BASE && cmd <SONG_EVENT_BASE+12 ) {
					action = cmd - SONG_EVENT_BASE+1;
					if (pause) 
						pause=0;
				}
			}

			if (pause) {
				SDL_Delay(10);
			}
		} while(pause);

		if (skip || cursor >= s->nseq*64) {
			cursor=0;
			break;
		}

		n = cursor>>6;

		if (o_n != n) {
			// jump 
		}

		pat = s->seq[n];
	        i = cursor&0x3F;

		if (o_n != n) {
			printf("<<<<<<< Pattern %3d, Pos: %3d >>>>>>>\n", pat, n);
			o_n = n;
		}

    		for (j=0;j<4;j++) {
				int r_perinc = 1;
				int r_slide  = 1;
				int r_vibrato = 1;

				volume[j]=0xFF;
				offset[j]=0x1; // disable
				volinc[j]=0;
				note_delay[j]=0;
				note_cut[j]=0;
				note_arp[j]=0;

				ChannelItem *p = &(s->patterns[pat].item[i*4+j]);
				next_step = i+1;
				
				extract_channel_data(p, &effect[j], &evalue[j], &instru[j], &pitch[j], &period[j]);
					
				if(pitch[j]>0) {
					pitch[j] -= 24;
				}
				
				switch (effect[j]) {
				case 0x0:
					if (evalue[j]!=0) {
						note_arp[j] = evalue[j];
						printf(">>> Arpejo: %02X\n", evalue[j]);
					}
				break;
				case 0x9:
					offset[j]=evalue[j]<<8;
					printf("Offset %05X %05X\n", offset[j], evalue[j]);
				break;
				case 0xF:
					if(evalue[j]>1 && evalue[j]<32) {
						ticks = evalue[j];
						printf("Ticks: %d\n", evalue[j]);
						//delta = 20;
					}
					else if(evalue[j]>=32) {
						delta = 1000/(evalue[j]*0.4);
						//ticks = 4;
						printf("Bpm: %02X\n", evalue[j]);

					}
					break;
				case 0x1:
					if (evalue[j]!=0) {
						perinc[j]=-evalue[j];
						printf("Dec\n");
					}
					r_perinc = 0;
					break;
				case 0x2:
					if (evalue[j]!=0) {
						perinc[j]=evalue[j];
					}
					r_perinc = 0;
					break;
				case 0x3:
					if (evalue[j] !=0 ) {
						slide[j]=evalue[j];
					}
					if (period[j] != 0) {
						slideto[j] = period[j];
					}
					period[j] = 0;
					r_slide = 0;
					break;
				case 0x4:
					if (evalue[j]!=0) {
						note_vib[j]=evalue[j];
					}
					r_vibrato = 0;
					break;
				case 0x5:
					if (period[j] != 0) {
						slideto[j] = period[j];
					}
					period[j] = 0;
					r_slide = 0;
					//fall
				case 0x6:
					if (effect[j]==0x6) {
						r_vibrato=0;
					}
					//fall
				case 0xA:
					if ( (evalue[j]&0xF0) == 0x0) {
						volinc[j]=-(int8_t)(evalue[j]&0x0F);
					}
					else if ((evalue[j]&0x0F) == 0x0) {
						volinc[j]=(evalue[j]>>4) & 0x0F;
					}	
				break;
				case 0xC:
					if (evalue[j]<=64) {
						volume[j]=evalue[j];
					}
				break;
				case 0xB:
					if (64*evalue[j]>cursor) { // add a flag to enable looping
						cursor=64*evalue[j];
						cursor--;
					}
				break;
				case 0xD:
					printf("HH %02X\n", evalue[j]);
					int ncursor = ((evalue[j]&0xF0)>>4)*10 + (evalue[j]&0x0F);
					if(ncursor==0) {
						cursor = ((cursor>>6)+1)<<6;
					} else {
						cursor = (((cursor>>6)+1)<<6) + ncursor;
					}
					cursor--;
					//printf("<<<<<<< Pattern %3d, Pos: %3d >>>>>>>\n", pat, n);
				break;
				case 0xE:
					switch ((evalue[j]>>4)) {
					case 0x1:
						adjust_period(smpst, j, evalue[j]&0xF);
					break;
					case 0x2:
						adjust_period(smpst, j, -(evalue[j]&0xF));
					break;
					case 0x6:
						{
							uint8_t v = evalue[j]&0xF;
							if (v==0) {
								loop_point[j] = cursor;
								printf("Loop @ %d\n", cursor);
							}
							else if (v!=0 && loop_point[j]!=-1) {
								if (loop_count[j] == -1) {
									printf("Set Count = %d\n", v);
									loop_count[j] = v;
								} else {
									printf("Dec count %d\n", loop_count[j]-1);
									loop_count[j]--;
								}
							

								if (loop_count[j]==0) {
									printf("Continue...\n");
									loop_count[j] = -1; 
									loop_point[j] = -1; 
								}

								if (loop_point[j]>=0) {
									printf("Jump to %d\n", loop_point[j]);
									cursor = loop_point[j]-1;
								}
							}

						}	
					break;
					case 0xA:
					      adjust_volume(smpst, j, evalue[j]&0xF);
					break;
					case 0xB:
					      adjust_volume(smpst, j, -(evalue[j]&0xF));
					break;
					case 0xC:
						note_cut[j]=evalue[j]&0xF;
						printf("Note cut %d -> %d\n", j, note_cut[j]);
					break;
					case 0xD:
						note_delay[j]=evalue[j]&0xF;
						printf("Note delay[%d]: %d\n", j, note_delay[j]);
					break;
					case 0xE:
						if (row_delay<0) {
							row_delay=evalue[j]&0xF;
						} else if (row_delay==0) {
							row_delay = -1;
						}
					break;
					default:
						printf("TODO fx 0xE%02x \n", evalue[j]);
					}
				break;
				default:
					printf("Fx: %X: %02X\n", effect[j], evalue[j]);
				}

				if (r_perinc) {
					perinc[j]=0;
				}

				if (r_slide) {
					slide[j]   = 0;
					slideto[j] = 0;
				}

				if (r_vibrato) {
					note_vib[j]=0;
				}
			}

			set_song_position(n*0x10000+pat*0x100+i);
		
			int t;
			size_t ns = (float)(delta * 22.050);

			for(t=0;t<ticks;t++) {
				set_note_ex2(smpst, period, pitch, instru, volume, offset, t);

				generate_audio(smpst, ns);

				set_channel_avg(average);


				apply_fx(smpst, perinc, volinc);
			}

			if (row_delay>0) {
				printf("Repeat row ---\n");
				row_delay--;
			} else {
    				cursor++;
			}
    }

    return cnt;
}


int start_sequencer(SequencerData *data)
{
    	printf("\nSequencer Start\n");
	lock_wait_start = SDL_CreateMutex();
	cond_wait_start = SDL_CreateCond();

    /* Simply create a thread */
    thread = SDL_CreateThread(SequencerThread, "SequencerThread", data);

    if (NULL == thread) {
        printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
	return 0;
    }
    
    return 1;
}

int stop_sequencer()
{
    	int  threadReturnValue;
	SDL_WaitThread(thread, &threadReturnValue);
	printf("\nThread returned value: %d", threadReturnValue);
	return 1;
}


unsigned char nlabel[24] = {
	'C', '-',
	'C', '#',
	'D', '-',
	'D', '#',
	'E', '-',
	'F', '-',
	'F', '#',
	'G', '-',
	'G', '#',
	'A', '-',
	'A', '#',
	'B', '-'
};

void extract_channel_data(
	ChannelItem *p, 
	uint8_t *effect, 
	uint8_t *evalue,
	uint8_t *instru,
	uint8_t *pitch,
        uint16_t *period)
{
	*effect = p->data[2]&0x0F;
	*evalue = p->data[3];
	*instru = ((p->data[0]&0xF0)) | ((p->data[2]&0xF0)>>4);
	*period = ((p->data[0]&0x0F) << 8) | p->data[1];
	
	*pitch = note_period_tab[*period];
}

void dump_channel_item(ChannelItem *p)
{
	uint8_t effect = p->data[2]&0x0F;
	uint8_t evalue = p->data[3];
	uint8_t instru = ((p->data[0]&0xF0)) | ((p->data[2]&0xF0)>>4);
	uint16_t period = ((p->data[0]&0x0F) << 8) | p->data[1];
	
	uint8_t pitch = note_period_tab[period];
	unsigned char name[4]={'-','-','-', '\0'};
	if (pitch>0) {
		name[2] = '0'+pitch/12-1;
		name[0] = nlabel[(pitch%12)*2];
		name[1] = nlabel[(pitch%12)*2+1];
		
	}
	printf("%s %02X %01X%02X", name, instru, effect, evalue);
}

void dump_channel_item_str(char *dest, ChannelItem *p)
{
	uint8_t effect = p->data[2]&0x0F;
	uint8_t evalue = p->data[3];
	uint8_t instru = ((p->data[0]&0xF0)) | ((p->data[2]&0xF0)>>4);
	uint16_t period = ((p->data[0]&0x0F) << 8) | p->data[1];
	
	uint8_t pitch = note_period_tab[period];
	unsigned char name[4]={'-','-','-', '\0'};
	if (pitch>0) {
		name[2] = '0'+pitch/12-1;
		name[0] = nlabel[(pitch%12)*2];
		name[1] = nlabel[(pitch%12)*2+1];
		
	}
	sprintf(dest, "%s%02X%01X%02X", name, instru, effect, evalue);
}

float fine_tune[16] = {
	1.0,
	1.0072464122237,
	1.01454533493752,
	1.02189714865412,
	1.02930223664349,
	1.03676098495299,
	1.04427378242741,
	1.05184102072929,
	0.943874312681693,
	0.95071401503875,
	0.957603280698573,
	0.964542468817287,
	0.971531941153606,
	0.9785720620877,
	0.985663198640187,
	0.992805720491269
	};

float arpeggio_table[16] = {
	1,
	1.0594630943593,
	1.12246204830937,
	1.18920711500272,
	1.25992104989487,
	1.33483985417003,
	1.4142135623731,
	1.49830707687668,
	1.5874010519682,
	1.68179283050743,
	1.78179743628068,
	1.88774862536339,
	2,
	2.11892618871859,
	2.24492409661875,
	2.37841423000544
};

extern int16_t mod_sine[64];

int16_t note_vibrato(SamplerStatus *smpst, uint8_t ch)
{
	int16_t offset = 0;
	int16_t depth  = note_vib[ch] & 0xF;
	uint8_t speed  = note_vib[ch]>>4;
	uint8_t tune = 0;
	uint16_t p = 0;

	printf("Vibrato[%d]: %02X Depth %d Speed: %d\n", ch, note_vib[ch], depth, speed);

	if (instru[ch]!=0 || period[ch]!=0) {
		vibrato_ptr[ch]=0;
	}

	offset = (mod_sine[vibrato_ptr[ch]]*depth)/64;
	vibrato_ptr[ch] = (vibrato_ptr[ch]+speed)&0x3F;


	if (smpst->sample[ch]!=-1) {
		tune = smpst->smpdata[smpst->sample[ch]].tune%16;
	}

	p = smpst->period[ch] ;
	p += offset;

	if (p>4096 || p<113) {
		p=113;
	}

	if (p>856) {
		p=856;
	}

	smpst->phase[ch]  = period_phase[p]*fine_tune[tune];
	
	return offset;
}

void note_arpeggio(SamplerStatus *smpst, uint8_t ch, uint8_t tick)
{
	float mod = 1.0;
	uint8_t tune = 0;

	if (smpst->sample[ch]!=-1) {
		tune = smpst->smpdata[smpst->sample[ch]].tune%16;
	}

	switch (tick%3) {
		case 0:
			mod = 1.0;
		break;
		case 1:
			mod = arpeggio_table[note_arp[ch]>>4];
		break;
		case 2:
			mod = arpeggio_table[note_arp[ch]&0xF];
		break;
	}

	uint32_t period = smpst->period[ch];
	smpst->phase[ch]  = period_phase[period]*fine_tune[tune]*mod;
}

void set_note_ex2(SamplerStatus *smpst,
	       	  uint16_t period[8], 
		  uint8_t note[8], 
		  uint8_t smp[8], 
		  uint8_t volume[8], 
		  uint32_t offset[8], 
		  uint8_t tick)
{
	int i;

	for (i=0;i<8;i++) {
		
		int retrigger = 0;

		int16_t period_offset = 0;

		if (note_vib[i]!=0) {
			period_offset = note_vibrato(smpst, i);
		}

		if (tick != 0 && note_arp[i]!=0) {
			note_arpeggio(smpst, i, tick);
		}

		if (tick != 0 && tick == note_cut[i]) {
			//smpst->period[i] = 0;
			//smpst->phase[i] = 0;
			smpst->volume[i] = 0;
			continue;
		}

		if (tick != note_delay[i]) {
			continue;
		}

		if(smp[i]!=0) {
			smpst->sample[i]  = smp[i]-1;
			
			if (volume[i]==0xFF) {
				smpst->volume[i]  = smpst->smpdata[smp[i]-1].vol;
			} 
			else {
				smpst->volume[i]  = volume[i];
			}
			retrigger = 1;
		} else {
			if( volume[i]!=0xFF) {
				smpst->volume[i]  = volume[i];
			}
		}

		if (period[i]!=0) { 
			uint8_t tune = 0;
			uint16_t p = period[i];

			if (smpst->sample[i]!=-1) {
				tune = smpst->smpdata[smpst->sample[i]].tune%16;
			}

			smpst->period[i] = p;
			p += period_offset;

			if (p>4096 || p<113) {
				p=113;
			}

			if (p>856) {
				p=856;
			}

			smpst->phase[i]  = period_phase[p]*fine_tune[tune];
			retrigger = 1;
		}

		if (retrigger) {
			smpst->pointer[i] = (offset[i]!=1) ? offset[i]<<16 : 0;
		}
	}
}

void apply_fx(SamplerStatus *smpst, int16_t perinc[8], int8_t volinc[8])
{
	int i;
	for(i=0;i<8;i++)
	{
		int calc_phase = 0;
		if (slide[i]!=0 && slideto[i]!=0) {
			if( smpst->period[i] > slideto[i]) {
				printf("Slide Up   [%d]: %d ==>(%d)==> %d\n",i, smpst->period[i], slide[i], slideto[i]);

				smpst->period[i] -= slide[i];
				if (smpst->period[i] < slideto[i]) {
					smpst->period[i] = slideto[i];
				}
			}
			else if (smpst->period[i] < slideto[i]) {
				printf("Slide Down [%d]: %d ==>(%d)==> %d\n",i, smpst->period[i], slide[i], slideto[i]);
				smpst->period[i] += slide[i];
				if (smpst->period[i] > slideto[i]) {
					smpst->period[i] = slideto[i];
				}
			}

			if (smpst->period[i]>4096 || smpst->period[i]<113) {
				smpst->period[i]=113;
			}

			if (smpst->period[i]>856) {
				smpst->period[i]=856;
			}

			calc_phase = 1;
		}

		if (perinc[i]!=0) {
			smpst->period[i]+=perinc[i];

			if (smpst->period[i]>4096 || smpst->period[i]<113) {
				smpst->period[i]=113;
			}

			if (smpst->period[i]>856) {
				smpst->period[i]=856;
			}

			printf("Period[%i] = (%d) %d\n", i, perinc[i], smpst->period[i]);
			calc_phase = 1;
		}

		if (calc_phase) {
			uint8_t tune = 0;
			if (smpst->sample[i]!=-1) {
				tune = smpst->smpdata[ smpst->sample[i] ].tune%16;
			}
			smpst->phase[i] = period_phase[ smpst->period[i] ] * fine_tune[tune];
		}
		if (volinc[i]!=0) {
			smpst->volume[i]+=volinc[i];
	//		printf("Volume (%d) %d\n", volinc[i], smpst->volume[i]);
			if(smpst->volume[i]>127) {
				smpst->volume[i] = 0;
			} else if(smpst->volume[i]>64) {
				smpst->volume[i] = 64;
			}
		}
	}
}

void adjust_period(SamplerStatus *smpst, uint8_t ch, int8_t value)
{
	uint8_t tune = 0;
	smpst->period[ch]+=value;

	if (smpst->period[ch]>4096 || smpst->period[ch]<113) {
		smpst->period[ch]=113;
	}

	if (smpst->period[ch]>856) {
		smpst->period[ch]=856;
	}

	printf("Period[%i] = (%d) %d\n", ch, value, smpst->period[ch]);

	if (smpst->sample[ch]!=-1) {
		tune = smpst->smpdata[ smpst->sample[ch] ].tune%16;
	}
	smpst->phase[ch] = period_phase[ smpst->period[ch] ] * fine_tune[tune];
}

void adjust_volume(SamplerStatus *smpst, uint8_t ch, int8_t value)
{
	smpst->volume[ch]+=value;
	printf("Volume (%d) %d\n", value, smpst->volume[ch]);
	if(smpst->volume[ch]>127) {
		smpst->volume[ch] = 0;
	} else if(smpst->volume[ch]>64) {
		smpst->volume[ch] = 64;
	}
}



#define AUDBUFFERSZ 1024

Uint8 audbuffer[AUDBUFFERSZ];

int generate_audio(SamplerStatus *smpst, size_t ns)
{
	float chmix[8];
	size_t cnt=0;
	int j;

	for (j=0;j<8; j++) {
		average[j]=0;
	}

	while(ns)
	{
		size_t i;
		size_t len = AUDBUFFERSZ;
		if (ns*2<len) {
			len = ns*2;
		}
		ns -= len/2;

		for(i=0;i<len;i++)
		{
			int s;
			int chleft = 0, chright = 0;
			for (s = 0; s<8;s++) {
				chmix[s]=0.0;
				int c = smpst->sample[s];
				uint32_t bp = smpst->pointer[s] >> 16;
				uint32_t ph = smpst->phase[s];
				uint32_t le, ls;
				float vol = 1.0;
				int a = 0;

				if (ph == 0 || c < 0 || smpst->smpdata[c].len < 1 || bp >= smpst->smpdata[c].len) {
					continue;
				}

		/*
		if (bp >= smpst->smpdata[c].len)
		{
			bp %= smpst->smpdata[c].len;
		}
		*/

				vol = smpst->volume[s];
				chmix[s] = ((char)(smpst->smpdata[c].data[bp])) * (vol/64.0);

			
				smpst->pointer[s]+=smpst->phase[s];
				ls = smpst->smpdata[c].loop_start << 16;
				le = smpst->smpdata[c].loop_end   << 16;

				if(le > ls && smpst->pointer[s] >= le ) {
					smpst->pointer[s] = ls + (smpst->pointer[s]-le);
				}

			}

			average[0] += chmix[0] * chmix[0];
			average[1] += chmix[1] * chmix[1];
			average[2] += chmix[2] * chmix[2];
			average[3] += chmix[3] * chmix[3];
			cnt++;

			chleft  = chmix[0]*0.5 + chmix[3]*0.5;
			chright = chmix[1]*0.5 + chmix[2]*0.5;
			if (chleft < -128) {
				chleft = -128;
			}
			if (chleft > 127) {
				chleft = 127;
			}
			if (chright < -128) {
				chright = -128;
			}
			if (chright > 127) {
				chright = 127;
			}

			audbuffer[i]    = chleft+128;
			audbuffer[++i]	= chright+128;

		}
		if(len) {
			PipeSend(&audiostream, audbuffer, len);
		}
	}

	if (cnt > 0) {
		for (j=0;j<8; j++) {
			average[j]=average[j]/(float)cnt;
		}
	}

	return 1;
}

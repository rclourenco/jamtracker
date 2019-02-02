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

void set_note_ex2(uint16_t period[8], uint8_t note[8], uint8_t smp[8], uint8_t volume[8], uint32_t offset[8]);
int generate_audio(size_t ns);

extern struct _SamplerStatus SamplerGlobalStatus;
extern struct _Sample Samples[31];
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

/* Very simple thread - counts 0 to 9 delaying 50ms between increments */
static int SequencerThread(void *ptr)
{
    int cnt;
    SequencerData *s=(SequencerData *)ptr;

	uint8_t instru[8] = {0,0,0,0, 0,0,0,0};
	uint8_t effect[8] = {0,0,0,0, 0,0,0,0};
	uint8_t evalue[8] = {0,0,0,0, 0,0,0,0};
	uint8_t pitch[8]  = {0,0,0,0, 0,0,0,0};
	uint8_t volume[8] = {0,0,0,0, 0,0,0,0};
	uint32_t offset[8] = {1,1,1,1, 1,1,1,1};
	uint16_t period[8] = {0,0,0,0, 0,0,0,0};
 
	int n=0, o_n=-1;
	uint8_t ticks = 6;
	uint8_t delta = 20;
	unsigned int currentTime = SDL_GetTicks();
	unsigned int lastTime = currentTime;
	int i=0, j;
	uint16_t cursor=0;
	while(1) {
		int j;
		int next_step;
		int pat;

		if (cursor > s->nseq*64)
    			cursor=0;

		n = cursor>>6;
		pat = s->seq[n];
	        i = cursor&0x3F;

		if (o_n != n) {
			printf("<<<<<<< Pattern %3d, Pos: %3d >>>>>>>\n", pat, n);
			o_n = n;
		}

    		for (j=0;j<4;j++) {
				volume[j]=0xFF;
				offset[j]=0x1; // disable
				ChannelItem *p = &(s->patterns[pat].item[i*4+j]);
				next_step = i+1;
				
				extract_channel_data(p, &effect[j], &evalue[j], &instru[j], &pitch[j], &period[j]);
					
				if(pitch[j]>0 && instru[j]>0) {
					pitch[j] -= 24;
					instru[j]--;
				}
				
				switch (effect[j]) {
				case 0x0:
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
				case 0xC:
					if (evalue[j]<=64) {
						volume[j]=evalue[j];
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
				default:
					printf("Fx: %X: %02X\n", effect[j], evalue[j]);
				}
			}

			set_note_ex2(period, pitch, instru, volume, offset);
		
			push_event(n, pat, i);
		
			currentTime = SDL_GetTicks();
			int delay = delta*ticks; // - SDL_TICKS_PASSED(lastTime, currentTime);
			lastTime = currentTime;
			size_t ns = (float)(delay * 22.050);
			generate_audio(ns);
        	//printf("\nDelay: %d", delay);
			//if (delay>0) {
			//	SDL_Delay(delay);
        		//SDL_Delay(delta*ticks);
    			//}
			//

			//i = next_step;
    	cursor++;
    }

    return cnt;
}


int start_sequencer(SequencerData *data)
{

    printf("\nSequencer Start\n");

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

void set_note_ex2(uint16_t period[8], uint8_t note[8], uint8_t smp[8], uint8_t volume[8], uint32_t offset[8])
{
	uint32_t p[8];
	int i;
	uint8_t tune;

	for (i=0;i<8;i++) {
		if (note[i]==0) {
			p[i]=0;
			continue;
		}

		tune = Samples[smp[i]].tune%16;

		/*
		uint8_t octave = (note[i]/12);
		float phase;

		if (octave > 5)
		{
			octave = 5;
		}

		if (octave < 0) {
			octave = 0;
		}

#define FK_old 261.63
#define FK 311.00


		phase = (freq_tab[note[i]%12]*oct_tab[octave]/FK);

		p[i] = phase*0x10000;	
		*/

		p[i] = period_phase[period[i]]*fine_tune[tune];
	}
/*
	for(i=0;i<4;i++) {
		printf(" # %08X - %02X", p[i], smp[i]);
	}
	printf("\n");
*/
	for (i=0;i<8;i++) {
		
		if (p[i]==0) {
			if( volume[i]!=0xFF) {
				SamplerGlobalStatus.volume[i]  = volume[i];
			}
			if (offset[i]!=1) {
				SamplerGlobalStatus.pointer[i] = offset[i]<<16;
			}
			continue;
		}

		SamplerGlobalStatus.phase[i]   = p[i];
		SamplerGlobalStatus.sample[i]  = smp[i];
		SamplerGlobalStatus.pointer[i] = (offset[i]!=1) ? offset[i]<<16 : 0;

		if (volume[i]==0xFF) {
			SamplerGlobalStatus.volume[i]  = Samples[smp[i]].vol;
		} 
		else {
			SamplerGlobalStatus.volume[i]  = volume[i];
		}	
	}
}

#define AUDBUFFERSZ 1024

Uint8 audbuffer[AUDBUFFERSZ];

int generate_audio(size_t ns)
{
	while(ns)
	{
		size_t i;
		size_t len = AUDBUFFERSZ;
		if (ns<len) {
			len = ns;
		}
		ns -= len;

		for(i=0;i<len;i++) 
		{
			int s;
			int total = 0;
			for (s = 0; s<8;s++) {
				int c = SamplerGlobalStatus.sample[s];
				uint32_t bp = SamplerGlobalStatus.pointer[s] >> 16;
				uint32_t ph = SamplerGlobalStatus.phase[s];
				uint32_t le, ls;
				float vol = 1.0;
				int a = 0;

				if (c < 0 || Samples[c].len < 1 || bp >= Samples[c].len) {
					continue;
				}

		/*
		if (bp >= Samples[c].len)
		{
			bp %= Samples[c].len;
		}
		*/

				vol = SamplerGlobalStatus.volume[s];
				total += ((char)Samples[c].data[bp]) * (vol/64.0);

			
				SamplerGlobalStatus.pointer[s]+=SamplerGlobalStatus.phase[s];
				ls = Samples[c].loop_start << 16;
				le = Samples[c].loop_end   << 16;

				if(le > ls && SamplerGlobalStatus.pointer[s] >= le ) {
					SamplerGlobalStatus.pointer[s] = ls + (SamplerGlobalStatus.pointer[s]-le);
				}

			}

			if (total < -128) {
				total = -128;
			}
			if (total > 127) {
				total = 127;
			}

			audbuffer[i] = total+128;	
		}
		if(len) {
			PipeSend(&audiostream, audbuffer, len);
		}
	}
	return 1;
}

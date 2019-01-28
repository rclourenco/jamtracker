#include <stdio.h>
#include <SDL2/SDL.h>
#include "jamtracker.h"

SDL_Thread *thread;

void extract_channel_data(
	ChannelItem *p, 
	uint8_t *effect, 
	uint8_t *evalue,
	uint8_t *instru,
	uint8_t *pitch );

void set_note_ex2(uint8_t note[8], uint8_t smp[8]);
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
	printf("Data To Write: %lu\n", len);
	while(i<len)
	{
		size_t space = PIPESIZE - pipe->len;
	        if(space==0)
			pipe->notfull = SDL_FALSE;	
		if(pipe->notfull==SDL_FALSE) {
			printf("<FULL>\n");
			SDL_CondWait(pipe->cond, pipe->lock);
		}
		space = PIPESIZE - pipe->len;
		printf("Writing %lu (%lu)\n", len-i, pipe->len);
		while(space && i < len) {
			pipe->buffer[pipe->wp]=src[i];
			space--;
			pipe->wp = (pipe->wp+1)%PIPESIZE;
			pipe->len++;
			i++;
		}
		printf("After: %lu (%lu)\n", len-i, pipe->len);
	}
	SDL_UnlockMutex(pipe->lock);

}

size_t PipeReceive(Pipe *pipe, uint8_t *dest, size_t len)
{
	size_t rd = 0;
	SDL_LockMutex(pipe->lock);
	printf(">>>>>>>>>>> Reading: %lu\n", len);
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

	int n=0;
	uint8_t ticks = 6;
	uint8_t delta = 20;
	unsigned int currentTime = SDL_GetTicks();
	unsigned int lastTime = currentTime;
	while(1) {
		int i,j;
		int pat = s->seq[n];
		printf("<<<<<<< Pattern %3d >>>>>>>\n", pat);
    	for (i = 0; i < 64; ++i) {
    		for (j=0;j<4;j++) {
				ChannelItem *p = &(s->patterns[pat].item[i*4+j]);
				
				extract_channel_data(p, &effect[j], &evalue[j], &instru[j], &pitch[j]);
					
				if(pitch[j]>0 && instru[j]>0) {
					pitch[j] -= 24;
					instru[j]--;
				}
				
				switch (effect[j]) {
				case 0xF:
					if(evalue[j]>1 && evalue[j]<32) {
						ticks = evalue[j];
						delta = 20;
					}
					else if(evalue[j]>=32) {
						delta = 1000/(evalue[j]*0.4);
						//ticks = 4;
					}
					break;
				case 0xD:
				break;
				}
			}
			
			set_note_ex2(pitch, instru);
			push_event(n, pat, i);
		
			currentTime = SDL_GetTicks();
			int delay = delta*ticks - SDL_TICKS_PASSED(lastTime, currentTime);
			lastTime = currentTime;
			size_t ns = (delay * 22.050);
			generate_audio(ns);
        	//printf("\nDelay: %d", delay);
			//if (delay>0) {
			//	SDL_Delay(delay);
        		//SDL_Delay(delta*ticks);
    			//}
		}
    	n++;
    	if(n>s->nseq)
    		n=0;
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
	uint8_t *pitch )
{
	uint16_t period;

	*effect = p->data[2]&0x0F;
	*evalue = p->data[3];
	*instru = ((p->data[0]&0xF0)) | ((p->data[2]&0xF0)>>4);
	period = ((p->data[0]&0x0F) << 8) | p->data[1];
	
	*pitch = note_period_tab[period];
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

void set_note_ex2(uint8_t note[8], uint8_t smp[8])
{
	uint32_t p[8];
	int i;

	for (i=0;i<8;i++) {
		if (note[i]==0) {
			p[i]=0;
			continue;
		}

		uint8_t octave = (note[i]/12);
		float phase;

		if (octave > 5)
		{
			octave = 5;
		}

		if (octave < 0) {
			octave = 0;
		}

		phase = (freq_tab[note[i]%12]*oct_tab[octave]/261.63);

		p[i] = phase*0x10000;
			
	}

	/*
	for(i=0;i<4;i++) {
		printf(" # %08X - %02X", p[i], smp[i]);
	}
	*/

	for (i=0;i<8;i++) {
		
		if (p[i]==0) {
			continue;
		}
		SamplerGlobalStatus.phase[i]   = p[i];
		SamplerGlobalStatus.sample[i]  = smp[i];
		SamplerGlobalStatus.pointer[i] = 0;
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

				vol = Samples[c].vol;
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
			//SDL_QueueAudio(1, audbuffer, len);
		}
	}
	return 1;
}

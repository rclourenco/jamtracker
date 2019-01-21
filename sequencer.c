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
			
			set_note_ex(pitch, instru);
			push_event(n, pat, i);
		
			currentTime = SDL_GetTicks();
			int delay = delta*ticks - SDL_TICKS_PASSED(lastTime, currentTime);
			lastTime = currentTime;
        	//printf("\nDelay: %d", delay);
			if (delay>0) {
				SDL_Delay(delay);
        		//SDL_Delay(delta*ticks);
    		}
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


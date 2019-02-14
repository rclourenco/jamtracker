#include <SDL2/SDL.h>
#include <stdio.h>
#include <strings.h>
#include "jamtracker.h"
#include "graphlib.c"

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 500;
const int SCREEN_BPP = 0;

SequencerData seqdta;

//The event structure
SDL_Event event;

Uint32 SequencerEvent;

Pipe audiostream;
//The font
//TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 0, 0, 0 };

int textx = 0;
int texty = 0;
unsigned char textbuffer[80*25];

void textwrite(char *str)
{
	int scroll_start = 3;
	int scroll_len = 19-scroll_start;
	while(*str) {
		if(*str=='\n') {
			textx=0;
			texty++;
		} else {
			textbuffer[texty*80+textx]=*str;
			textx++;
			if (textx>=80) {
				textx=0;
				texty++;
			}
		}

		if(texty>=20) {
			memmove(&textbuffer[scroll_start*80], &textbuffer[(scroll_start+1)*80], 80*scroll_len*sizeof(unsigned char));
			memset(&textbuffer[19*80], 0, 80*sizeof(unsigned char));
			texty=19;
			textx=0;
		}

		str++;
	}
}

SDL_mutex *mutex;

void apply_surface()
{
	#define SHAPE_W 16
	#define SHAPE_H 16
	#define SHAPE_SIZE 16
	int i,j;
	SDL_Rect SrcR;
	SDL_Rect DestR;
	TextColor=14;
    fillscreen(0x1);	
//	SDL_RenderClear(renderer);
	for (i=0;i<80;i++) {
		for(j=0;j<25;j++) {
			unsigned char vo = textbuffer[j*80+i];
			unsigned char v = vo;
			if (v>='A' && v<='Z')
				v = v - 'A' + 33;
			else if(v>='a' && v<= 'z')
				v = v - 'a' + 33;
			else if(v>='0' && v<= '9')
				v = v - '0' + 16;
			else {
				switch(v) {
				case '-': v=13; break;
				case '#': v=11; break;
				default:
					v = 0;
				}
			}
			int cx = v%10;
			int cy = (v/10);

			SrcR.x = cx*32;
 			SrcR.y = cy*25;
			SrcR.w = 32;
			SrcR.h = 25;

			DestR.x = i*8;
			DestR.y = j*8;
			DestR.w = 32;
			DestR.h = 25;
			pchar(DestR.x, DestR.y, vo);
			//SDL_RenderCopy(renderer, background, &SrcR, &DestR);
		}
	}
	graph_refresh();
	//rotate_palette();
//	SDL_RenderPresent(renderer);

}

void update_status(uint8_t seq, uint8_t pat, uint8_t pos)
{
	char data[128];
	int j;

	sprintf(data, " %02X", pos);
	textwrite(data);
	for (j=0;j<4;j++) {
		ChannelItem *p = &(seqdta.patterns[pat].item[pos*4+j]);

		dump_channel_item_str(data, p);
		textwrite(" ");
		textwrite(data);
		//textwrite(" ");
//		if(j%2) {
//			textwrite("\n");
//		} else {
//			textwrite(" ");
//		}
	}
	textwrite("\n");
//	sprintf(data, "%d %d %d\n", seq, pat, pos);
//	printf(">>> %s", data);
//	textwrite(data);
	apply_surface();
}


int main_loop_old();
int main_loop_new();
int load_files_new();
int init_new();

void cleanup_new();

int main( int argc, char *args[])
{
	MusicModule *mm;
	if( argc > 1 )
	{
		mm = musmod_load(args[1]);

		printf("XXXXXXXXXX\n");
	
		if(mm==NULL) {
			return 0;
		}
	}
	
	/*
	debug dump pattern
	*/
	int cp;
	for(cp=0; cp < mm->lps; cp++) {
		int i;
		printf("------- Pattern %d --------\n", cp);
		for (i=0;i<64;i++) {
			int j=0;
			for (j=0;j<4;j++) {
				ChannelItem *p = &(mm->patterns[cp].item[i*4+j]);
				dump_channel_item(p);
				printf(" - ");
			}
			printf("\n");
		}
		printf("--------------------------\n");
		break;
	}
	
	seqdta.patterns = mm->patterns;
	seqdta.npat = mm->lps; 
	seqdta.seq  = mm->ms.seq;
	seqdta.nseq = mm->ms.np;
	seqdta.smpdata = mm->samples;

	printf("INIT now\n");
    //Initialize
    if( init_new() == 0 )
    {
        return 1;
    }
    
    SequencerEvent = SDL_RegisterEvents(1);

    //Load the files
    if( load_files_new() == 0 )
    {
        return 1;
    }

    printf("Ready!\n");
    textwrite("   # JAMTRACKER #\n\n");
	int i;

	for(i=19;i>=0; i--) {
		if(mm->name[i]==32) {
			mm->name[i]=0;
		} else if (mm->name[i]>0) {
			break;
		}
	}
	int ns = strlen(mm->name);
	char tmp[128];
	if (ns<20) {
		sprintf(tmp, "%*s\n", ns+(20-ns)/2, mm->name);
	}
	else {
		sprintf(tmp, "%s\n", mm->name);
	}
	textwrite(tmp);
    apply_surface();

    main_loop_new();

    cleanup_new();

    return 0;
}

float freq_tab[12]={
	130.81, 138.59, 146.83, 
	155.86, 164.81, 174.61,
	185.00, 196.00, 207.65,
	220.00, 233.08, 246.94
};

float oct_tab[12]={0.5, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0};

int main_loop_new()
{
	int quit = 0;

 	SDL_PauseAudio(0); /* start audio playing. */
	int smp = 0;
	start_sequencer(&seqdta);	
	while(!quit)
	{
	        //While there's events to handle
		while( SDL_WaitEvent( &event ) )
        	{
			if(event.type == SequencerEvent) {
				if (event.user.code == 0x10FFFFFF) {
					quit=1;
					break;
				} else {
					update_status(
								(event.user.code>>16) & 0xFF,
								(event.user.code>>8) & 0xFF,
								event.user.code & 0xFF
							);
				}
			}
	            //If a key was pressed
		        if(event.type == SDL_KEYUP)
			{
				//set_note_off();
			}

        	    	if( event.type == SDL_KEYDOWN && event.key.repeat == 0)
            		{

			int sl = -1;
			switch(event.key.keysym.sym)
			{
			case SDLK_SPACE:
			break;
			case SDLK_UP: if(smp<30) smp++; break;
			case SDLK_DOWN: if(smp>0) smp--; break;
			case SDLK_q:sl=12; break;
			case SDLK_2:sl=13; break;
			case SDLK_w:sl=14; break;
			case SDLK_3:sl=15; break;
	    		case SDLK_e:sl=16; break;
			case SDLK_r:sl=17; break;
			case SDLK_5:sl=18; break;
			case SDLK_t:sl=19; break;
			case SDLK_6:sl=20; break;
			case SDLK_y:sl=21; break;
			case SDLK_7:sl=22; break;
			case SDLK_u:sl=23; break;
			case SDLK_i:sl=24; break;
			case SDLK_9:sl=25; break;
			case SDLK_o:sl=26; break;
			case SDLK_0:sl=27; break;
			case SDLK_p:sl=28; break;

			case SDLK_z:sl=0; break;
			case SDLK_s:sl=1; break;
			case SDLK_x:sl=2; break;
			case SDLK_d:sl=3; break;
			case SDLK_c:sl=4; break;

			case SDLK_v:sl=5; break;
			case SDLK_g:sl=6; break;
			case SDLK_b:sl=7; break;
			case SDLK_h:sl=8; break;
			case SDLK_n:sl=9; break;
			case SDLK_j:sl=10; break;
			case SDLK_m:sl=11; break;
		/*	case SDLK_x:sl=12; break;
			case SDLK_c:sl=13; break;
			case SDLK_v:sl=14; break;
			case SDLK_b:sl=15; break;
			case SDLK_n:sl=16; break;
			case SDLK_m:sl=17; break;*/
			default: break;
			}

			if(sl!=-1) {
				//set_note(sl, smp);
			}
			}

			//If the user has Xed out the window
 			if( event.type == SDL_QUIT )
 			{
 				//Quit the program
 				quit = 1;
				break;
  			}
		}
	}
	SDL_PauseAudio(1);
	SDL_CloseAudio();
	return 1;
}


void cleanup_new()
{
	modo3h();
}

int load_files_new()
{
    return 1;
}

void audio_callback2(void *userdata, Uint8 *stream, int len)
{
	size_t i;
	size_t wrt;

        wrt = PipeReceive(&audiostream, stream, len);

	for(i=wrt; i<len; i++) {
		stream[i]=128;
	}
}

struct _SamplerStatus statuscpy;

int init_new()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return 0;
    }

	mutex = SDL_CreateMutex();
	if (!mutex) {
  		fprintf(stderr, "Couldn't create mutex\n");
  		return 0;
	}

	set_window_name("Jamtrack I");
	modo13h();


	int i, count = SDL_GetNumAudioDevices(0);

	for (i = 0; i < count; ++i) {
    		SDL_Log("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
	}


	SDL_AudioSpec want, have;

	memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.freq = 22050;
	want.format = AUDIO_U8;
	want.channels = 2;
	want.samples = 1024;
	//want.callback = audio_callback; /* you wrote this function elsewhere -- see SDL_AudioSpec for details */
	want.callback = audio_callback2;
//	SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

if (SDL_OpenAudio(&want, &have) < 0) {
    printf("Failed to open audio: %s\n", SDL_GetError());
    return 0;
} else {
    if (have.format != want.format || have.channels != want.channels) {
        printf("We didn't get AUDIO_U8 audio format.\n");
	return 0;
    }
}

    PipeInit(&audiostream);
    return 1;
}



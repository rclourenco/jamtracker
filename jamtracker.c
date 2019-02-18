#include <SDL2/SDL.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "jamtracker.h"
#include "graphlib.c"

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 500;
const int SCREEN_BPP = 0;

SequencerData seqdta;

//The event structure
SDL_Event event;
SDL_AudioDeviceID adev;

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
	int scroll_start = 5;
	int scroll_len = 22-scroll_start;
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

		if(texty>=23) {
			memmove(&textbuffer[scroll_start*80], &textbuffer[(scroll_start+1)*80], 80*scroll_len*sizeof(unsigned char));
			memset(&textbuffer[22*80], 0, 80*sizeof(unsigned char));
			texty=22;
			textx=0;
		}

		str++;
	}
}

SDL_mutex *mutex;

void apply_surface(int seq, int pat, int pos, int avg[8])
{
	#define SHAPE_W 16
	#define SHAPE_H 16
	#define SHAPE_SIZE 16
	int yi = 68;
	int nr = 13;
	int nr2 = 6;

	int i,j;
	SDL_Rect SrcR;
	SDL_Rect DestR;
	TextBackGround=255;
	TextColor=14;
	FillColor=17;
	FillArea(1, yi-4, 318, yi+107, 17);
    	//fillscreen(0x0);
	//writest(100, 20, "JamTracker");
	FillArea(5,  0, 30,50, 0);
	FillArea(290,0,315,50, 0);

//	printf("[%5d %5d %5d %5d]\n", avg[0], avg[1],  avg[2], avg[3]);

	FillArea(5,   50, 15,  50-avg[0], 2);
	FillArea(20,  50, 30,  50-avg[1], 2);
	FillArea(290, 50, 300, 50-avg[2], 2);
	FillArea(305, 50, 315, 50-avg[3], 2);

	char buffer[128];

	if (seq>=0 && pat>=0 && pos>=0) {
    		for(i=0;i<nr;i++)
		{
			int row = pos - nr2 + i;
			if (row<0 || row > 63) {
				continue;
			}

			TextColor =  (row == pos) ? 40 : 7;

			sprintf(buffer, "%02X", row);
			writest(8, yi+i*8, buffer);


			for (j=0;j<4;j++) {
				ChannelItem *p = &(seqdta.patterns[pat].item[row*4+j]);
				dump_channel_item_str(buffer, p);
				writest(32+j*72, yi+i*8, buffer);
			}
		}
	}

	Color = 2;
	DrawRect(2, yi-3, 317, yi+106, LINE);
	DrawLine(27, yi-2, 27, yi+105);
	DrawLine(99, yi-2, 99, yi+105);
	DrawLine(171, yi-2, 171, yi+105);
	DrawLine(244, yi-2, 244, yi+105);

	TextBackGround=0;
	TextColor=32;

	writest(0, 176, "\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC2\
\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC2\
\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF");


	if(seq == -1) {
		sprintf(buffer, "%c Position: -- %c Pattern: -- %c Row: -- %c", 179, 179, 179, 179);
	} else {
		sprintf(buffer, "%c Position: %02X %c Pattern: %02X %c Row: %02X %c", 179, seq, 179, pat, 179, pos, 179);
	}

	writest(0, 184, buffer);

	writest(0, 192, "\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC1\
\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC1\
\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9");

	graph_refresh();
}

void show_song(MusicModule *mm)
{
	fillscreen(0);
	FillArea(40, 30, 279, 50, 17);
	Color=32;
	DrawLine(40, 30, 279, 30);
	DrawLine(40, 30, 40, 50);
	Color=1;
	DrawLine(40, 50, 279, 50);
	DrawLine(279, 30, 279, 50);

	TextBackGround=255;
	TextColor=19;
	writest(120,10, "JamTracker");
	TextColor=40;
	writest(121,11, "JamTracker");

	TextColor=46;

	if (mm) {
		int l = strlen(mm->name);
		TextColor=20;
		writest(39+(15-l/2)*8, 35, mm->name);
		TextColor=46;
		writest(40+(15-l/2)*8, 36, mm->name);
	}
	graph_refresh();
}
void update_status(uint8_t seq, uint8_t pat, uint8_t pos, int avg[8])
{
	apply_surface(seq, pat, pos, avg);
}


int main_loop_old();
int main_loop_new(MusicModuleList *mml);

int load_files_new();
int init_new(int audio_device);

void cleanup_new();

int avg[8];

int main( int argc, char *args[])
{
	MusicModule *mm = NULL;
	MusicModuleList *mml = NULL;

	int audio_device = 0;

	if (argc > 2) {
		audio_device = atoi(args[2]);

		printf("Audio Device: %d\n", audio_device);
	}

    if( init_new(audio_device) == 0 )
    {
        return 1;
    }


	if( argc > 1 )
	{
		mml = musmod_open_list(args[1]);

		if (!mml)
			return 2;

		printf("__________________\n");

		mm = musmod_list_next(mml);


		printf("XXXXXXXXXX\n");
	
		if(mm==NULL) {
			return 2;
		}

	}
	
	if (mm == NULL) {
		return 1;
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
    
    SequencerEvent = SDL_RegisterEvents(1);

    //Load the files
    if( load_files_new() == 0 )
    {
        return 1;
    }

    printf("Ready!\n");
    textwrite("             # JAMTRACKER #\n\n");
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
//	if (ns<20) {
//		sprintf(tmp, "Name: %*s\n\n", ns+(20-ns)/2, mm->name);
//	}
//	else {
		sprintf(tmp, " Name: %s\n\n", mm->name);
//	}
	textwrite(tmp);
	textwrite("\n");
	show_song(mm);
    apply_surface(-1,-1,-1, avg);

    main_loop_new(mml);

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

int main_loop_new(MusicModuleList *mml)
{
	int quit = 0;

 	SDL_PauseAudioDevice(adev, 0); /* start audio playing. */
	int smp = 0;
	start_sequencer(&seqdta);
	int osp = -1;

	int on_break = 0;
	int stoped = 0;

	while(!quit)
	{
		int sp = get_song_position();
		if (sp!=-1) {
			get_channel_avg(avg);
			update_status( (sp>>16) &0xFF, (sp>>8) & 0xFF, sp & 0xFF, avg);
			osp = sp;
		}

		if (sp==-1 && sequence_status()==0) {
			switch(on_break) {
			case 1:
				printf("Load next >>>>>>>>>>>>>>>>>>>>>>\n");
				MusicModule *mm = musmod_list_next(mml);
				if (mm) {
					seqdta.patterns = mm->patterns;
					seqdta.npat = mm->lps; 
					seqdta.seq  = mm->ms.seq;
					seqdta.nseq = mm->ms.np;
					seqdta.smpdata = mm->samples;
					show_song(mm);
					printf("PTR Set %p %p %d\n", seqdta.seq, seqdta.smpdata, (int)seqdta.npat);
					stoped = 0;
					sequence_start();
				} else {
					quit = 1;
				}
				break;
			case 2:
				// restart
				stoped = 0;
				sequence_start();
				break;
			case 3:
				stoped = 1;
				break;
			}

			on_break = stoped ? 0 : 1;

			if (quit)
				break;
		}
	        //While there's events to handle
		if( SDL_WaitEventTimeout( &event, 40 ) )
        	{
			if(event.type == SequencerEvent) {
				if (event.user.code == 0x10FFFFFF) {
					quit=1;
					break;
				} else {
	/*				update_status(
								(event.user.code>>16) & 0xFF,
								(event.user.code>>8) & 0xFF,
								event.user.code & 0xFF
							);
	*/
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
				stoped = 0;
				sequence_start();
				break;
			case SDLK_0:
				set_fullscreen(0);
				break;
			case SDLK_1:
				set_fullscreen(1);
				break;
			case SDLK_2:
				set_fullscreen(2);
				break;
			case SDLK_ESCAPE:
				quit=1;
			break;
			case SDLK_x:
				on_break = 1;
				if (!stoped)
					set_song_command(SONG_BREAK);
			break;
			case SDLK_r:
				on_break = 2;
				if (!stoped)
					set_song_command(SONG_BREAK);
			break;
			case SDLK_s:
				on_break = 3;
				if (!stoped)
					set_song_command(SONG_BREAK);
			break;
			case SDLK_c:
				stoped = 0;
				sequence_resume(osp);
			break;
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
	SDL_PauseAudioDevice(adev, 1);
	SDL_CloseAudioDevice(adev);
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

int init_new(int audio_device)
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

	int i, count = SDL_GetNumAudioDevices(0);

	char *device_str = NULL;
	char devname[256];

	for (i = 0; i < count; ++i) {
			const char *tmp = SDL_GetAudioDeviceName(i, 0);

			if (i==audio_device) {
				strncpy(devname, tmp, 255);
				devname[255] = '\0'; 
				device_str = devname;

	    		SDL_Log("[x] Audio device %d: %s", i, tmp);
			}
			else {
				SDL_Log("[ ] Audio device %d: %s", i, tmp);
			}
	}


	set_window_name("Jamtrack I");
	modo13h();


	SDL_AudioSpec want, have;

	memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.freq = 22050;
	want.format = AUDIO_U8;
	want.channels = 2;
	want.samples = 1024;
	//want.callback = audio_callback; /* you wrote this function elsewhere -- see SDL_AudioSpec for details */
	want.callback = audio_callback2;

//
	adev = SDL_OpenAudioDevice(device_str, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

if (adev == 0) {
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



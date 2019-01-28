/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include <SDL2/SDL.h>
//#include <SDL/SDL_image.h>
//#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <strings.h>
#include "jamtracker.h"

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 500;
const int SCREEN_BPP = 0;

struct _Sample Samples[31];

size_t nSamples = 0;

struct {
	unsigned char np;
	unsigned char flag;
	unsigned char seq[128];
	unsigned char id[4];
} ModSeq;

ModPattern Pattern[127];

uint8_t LoadedPatterns = 0;

struct _SamplerStatus SamplerGlobalStatus;

SequencerData seqdta;
char ModName[21];

SDL_Window *window = NULL;
//The surfaces
SDL_Texture *background = NULL;
SDL_Renderer *renderer = NULL;

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

//The music that will be played

//The sound effects that will be used

//Mix_Chunk test[31];

SDL_mutex *mutex;

SDL_Texture *load_image(const char *filename )
{
	SDL_Surface *surf = SDL_LoadBMP( filename );
    
	if( surf == NULL )
	{
		printf("No image loaded\n");
		return 0;
	}
    
 	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	
	return texture;
}

void apply_surface()
{
	#define SHAPE_W 16
	#define SHAPE_H 16
	#define SHAPE_SIZE 16
	int i,j;
	SDL_Rect SrcR;
	SDL_Rect DestR;

	
	SDL_RenderClear(renderer);
	for (i=0;i<80;i++) {
		for(j=0;j<25;j++) {
			unsigned char v = textbuffer[j*80+i];
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

			DestR.x = i*32;
			DestR.y = j*25;
			DestR.w = 32;
			DestR.h = 25;

			SDL_RenderCopy(renderer, background, &SrcR, &DestR);
		}
	}
	SDL_RenderPresent(renderer);

}

void update_status(uint8_t seq, uint8_t pat, uint8_t pos)
{
	char data[128];
	int j;
	for (j=0;j<4;j++) {
		ChannelItem *p = &(Pattern[pat].item[pos*4+j]);

		dump_channel_item_str(data, p);
		textwrite(" ");
		textwrite(data);
		if(j%2) {
			textwrite("\n");
		} else {
			textwrite(" ");
		}
	}
	textwrite("\n");
//	sprintf(data, "%d %d %d\n", seq, pat, pos);
//	printf(">>> %s", data);
//	textwrite(data);
	apply_surface();
}

int load_mod(char *modfile)
{
	char modtype[5];
	int smpn = 15;
	int i = 0;
	int max = 0;

	FILE *fp;
	fp = fopen(modfile, "rb");
	if( !fp )
	{
		return 0;
	}

	fseek(fp, 1080, SEEK_SET);
	if(fread(modtype, 4, 1, fp)!=1)
	{
		goto failure;
	}
	fseek(fp, 0, SEEK_SET);

	if(fread(ModName, 20, 1, fp)!=1) {
	
		goto failure;
	}

	modtype[4]='\0';
	ModName[20]='\0';

	if (modtype[0]=='M' && modtype[1]=='.' && modtype[2]=='K' && modtype[3]=='.') {
		smpn=31;
	} else {
		printf("Not supported\n");
		goto failure;
	}

	printf("ModType: %s\n", modtype);
	printf("Modname: %s\n", ModName);
	for(i=0;i<smpn;i++) {
		char smpname[23];
		unsigned char smpdef[30];
		unsigned slen = 0, ls = 0, llen = 0;
		if( fread(smpdef, 30, 1, fp) != 1 ) {
			goto failure;
		}
		strncpy(smpname, (const char *)smpdef, 22);
		smpname[22]='\0';
		slen = smpdef[22]*256+smpdef[23];
		slen <<= 1;
		ls = smpdef[26]*256+smpdef[27];
		llen = smpdef[28]*256+smpdef[29];
		ls *= 2;
		llen *= 2;
		printf("Sample %2d: %-22s S:%5X T: %d V: %2d [%6X => %6X]\n", 
				i, smpname, slen, smpdef[24], smpdef[25],ls,llen);
		Samples[i].len = slen;
		Samples[i].data = NULL;
		Samples[i].vol  = smpdef[25];
		Samples[i].tune = smpdef[24]&0xF;
		if (Samples[i].vol == 0) {
			Samples[i].vol = 32;
		}
		Samples[i].loop_start = ls;
		if (llen>2) {
			Samples[i].loop_end = ls+llen;
		} else {
			Samples[i].loop_end = ls;
		}
	}
	nSamples = smpn;
	if (fread(&ModSeq, sizeof(ModSeq), 1, fp)!=1) {
		goto failure;
	}

	printf("Number of patterns: %u\n", ModSeq.np);
	printf("Flag: %u\n", ModSeq.flag);
	printf("ID: %c%c%c%c\n", ModSeq.id[0], ModSeq.id[1], ModSeq.id[2], ModSeq.id[3]);
	printf("L: %ld\n", ftell(fp));
	
	for(i=0;i<=ModSeq.np;i++) {
		if (ModSeq.seq[i]>max) 
			max = ModSeq.seq[i];
	}
	max++;
	printf("Max %d\n", max);
	if (max>127) {
		fprintf(stderr, "Unsupported number o patterns\n");
		goto failure;
	}
	
	for (i=0; i<max; i++) {
		if (fread(&Pattern[i], sizeof(ModPattern), 1, fp)!=1) {
			goto failure;
		}
	}
	LoadedPatterns=max;
	
	printf("L: %ld\n", ftell(fp));
	for(i=0;i<nSamples;i++) {
		printf("S: %u\n", (unsigned int)Samples[i].len);
		if (Samples[i].len == 0)
			continue;
		Samples[i].data = (unsigned char *)malloc(Samples[i].len);
		if (Samples[i].data == NULL) {
			goto failure;
		}
		if (fread(Samples[i].data, Samples[i].len, 1, fp)!=1)
			goto failure;
	}

	printf("L: %ld\n", ftell(fp));

	fclose(fp);
	return 1;
failure:
	fclose(fp);
	return 0;
}

int main_loop_old();
int main_loop_new();
int load_files_new();
int init_new();

void cleanup_new();

int main( int argc, char *args[])
{
	if( argc > 1 )
	{
		load_mod(args[1]);
	}
	
	/*
	debug dump pattern
	*/
	if (LoadedPatterns>0) {
		int i;
		printf("------- Pattern 1 --------\n");
		for (i=0;i<64;i++) {
			int j=0;
			for (j=0;j<4;j++) {
				ChannelItem *p = &(Pattern[0].item[i*4+j]);
				dump_channel_item(p);
				printf(" - ");
			}
			printf("\n");
		}
		printf("--------------------------\n");
	}
	
	seqdta.patterns = Pattern;
	seqdta.npat = LoadedPatterns;
	seqdta.seq  = ModSeq.seq;
	seqdta.nseq = ModSeq.np;

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
		if(ModName[i]==32) {
			ModName[i]=0;
		} else if (ModName[i]>0) {
			break;
		}
	}
	int ns = strlen(ModName);
	char tmp[128];
	if (ns<20) {
		sprintf(tmp, "%*s\n", ns+(20-ns)/2, ModName);
	}
	else {
		sprintf(tmp, "%s\n", ModName);
	}
	textwrite(tmp);
    apply_surface();

    main_loop_new();

    cleanup_new();

    return 0;
}

void set_freq(int f)
{
	float phase = (f/130.81)*0x10000;

	if (SDL_LockMutex(mutex) == 0) {
		SamplerGlobalStatus.phase[0]=phase;
  		/* Do stuff while mutex is locked */
  		SDL_UnlockMutex(mutex);
	} else {
  		fprintf(stderr, "Couldn't lock mutex\n");
	}
}

void set_sample(int c)
{
	if (SDL_LockMutex(mutex) == 0) {
		SamplerGlobalStatus.phase[0] = 1 << 16;
		SamplerGlobalStatus.sample[0]=c;
		SamplerGlobalStatus.pointer[0]=0;
  		/* Do stuff while mutex is locked */
  		SDL_UnlockMutex(mutex);
	} else {
  		fprintf(stderr, "Couldn't lock mutex\n");
	}
}

float freq_tab[12]={
	130.81, 138.59, 146.83, 
	155.86, 164.81, 174.61,
	185.00, 196.00, 207.65,
	220.00, 233.08, 246.94
};

float oct_tab[12]={0.5, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0};

void set_note(uint8_t note, uint8_t smp)
{
	uint8_t octave = (note/12);
	float phase;
	uint32_t p;

	if (octave > 5)
	{
		octave = 5;
	}

	phase = (freq_tab[note%12]*oct_tab[octave]/261.63);

	p = phase*0x10000;

	printf("%d %d %lf %08X\n", octave, note%12, phase, p);

	if (SDL_LockMutex(mutex) == 0) {
		SamplerGlobalStatus.phase[0] = p;
		SamplerGlobalStatus.sample[0]=smp;
		SamplerGlobalStatus.pointer[0]=0;
  		/* Do stuff while mutex is locked */
  		SDL_UnlockMutex(mutex);
	} else {
  		fprintf(stderr, "Couldn't lock mutex\n");
	}
}

void set_note_ex(uint8_t note[8], uint8_t smp[8])
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

	if (SDL_LockMutex(mutex) == 0) {
		for (i=0;i<8;i++) {
		
			if (p[i]==0) {
				continue;
			}
			SamplerGlobalStatus.phase[i]   = p[i];
			SamplerGlobalStatus.sample[i]  = smp[i];
			SamplerGlobalStatus.pointer[i] = 0;
		}
  		/* Do stuff while mutex is locked */
  		SDL_UnlockMutex(mutex);
	} else {
  		fprintf(stderr, "Couldn't lock mutex\n");
	}
}



void set_note_off()
{
	if (SDL_LockMutex(mutex) == 0) {
		SamplerGlobalStatus.phase[0]   = 0;
		SamplerGlobalStatus.sample[0]  = -1;
		SamplerGlobalStatus.pointer[0] = 0;
  		/* Do stuff while mutex is locked */
  		SDL_UnlockMutex(mutex);
	} else {
  		fprintf(stderr, "Couldn't lock mutex\n");
	}
}



void get_sampler_status(struct _SamplerStatus *x)
{
	if (SDL_LockMutex(mutex) == 0) {
		memcpy(x, &SamplerGlobalStatus, sizeof(struct _SamplerStatus) );
  		/* Do stuff while mutex is locked */
  		SDL_UnlockMutex(mutex);
	} else {
  		fprintf(stderr, "Couldn't lock mutex\n");
	}
}

void update_sampler_pointers(struct _SamplerStatus *x)
{
	if (SDL_LockMutex(mutex) == 0) {
		memcpy(&SamplerGlobalStatus, x, sizeof(struct _SamplerStatus) );
		//memcpy(SamplerGlobalStatus.pointer, x->pointer, sizeof(int)*8 );
  		/* Do stuff while mutex is locked */
  		SDL_UnlockMutex(mutex);
	} else {
  		fprintf(stderr, "Couldn't lock mutex\n");
	}
}

int main_loop_new()
{
	int quit = 0;
	set_sample(-1);
 	SDL_PauseAudio(0); /* start audio playing. */
	int smp = 0;
	start_sequencer(&seqdta);	
	while(!quit)
	{
	        //While there's events to handle
		while( SDL_WaitEvent( &event ) )
        	{
			if(event.type == SequencerEvent) {
				//printf(">>> %X\n", event.user.code);
				update_status(
								(event.user.code>>16) & 0xFF,
								(event.user.code>>8) & 0xFF,
								event.user.code & 0xFF
							);
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
    //Free the surfaces
    SDL_DestroyTexture( background );

    //Close the font
    //TTF_CloseFont( font );

    //Quit SDL_ttf
    //TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

int load_files_new()
{
    //Load the background texture
     background = load_image( "data/font_x.bmp" );

 	if( background == NULL )
	{
		printf("No background loaded\n");
		return 0;
	}
    

    //If there was an error in loading the font
  //  if( font == NULL )
  //  {
   //     return 0;
   // }

    //If everything loaded fine
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

void audio_callback(void*  userdata, Uint8* stream, int len)
{

	int i = 0;
	get_sampler_status(&statuscpy);

	for(i=0;i<len;i++) 
	{
		int s;
		int total = 0;
		for (s = 0; s<8;s++) {
			int c = statuscpy.sample[s];
			uint32_t bp = statuscpy.pointer[s] >> 16;
			uint32_t ph = statuscpy.phase[s];
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

			
			statuscpy.pointer[s]+=statuscpy.phase[s];
			ls = Samples[c].loop_start << 16;
			le = Samples[c].loop_end   << 16;

			if(le > ls && statuscpy.pointer[s] >= le ) {
				statuscpy.pointer[s] = ls + (statuscpy.pointer[s]-le);
			}

		}

		if (total < -128) {
			total = -128;
		}
		if (total > 127) {
				total = 127;
		}

		stream[i] = total+128;
		
		
	}

	update_sampler_pointers(&statuscpy);
}


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

    window = SDL_CreateWindow("JamTracker I",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if( window == NULL) {
		return 0;
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
 
    //If there was an error in setting up the screen
    if( renderer == NULL )
    {
        return 0;
    }

    //Initialize SDL_ttf
//    if( TTF_Init() == -1 )
//    {
//        return 0;
//    }

	int i, count = SDL_GetNumAudioDevices(0);

	for (i = 0; i < count; ++i) {
    		SDL_Log("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
	}


	SDL_AudioSpec want, have;

	memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.freq = 22050;
	want.format = AUDIO_U8;
	want.channels = 1;
	want.samples = 1024;
	//want.callback = audio_callback; /* you wrote this function elsewhere -- see SDL_AudioSpec for details */
	want.callback = audio_callback2;
//	SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

if (SDL_OpenAudio(&want, &have) < 0) {
    printf("Failed to open audio: %s\n", SDL_GetError());
    return 0;
} else {
    if (have.format != want.format) {
        printf("We didn't get AUDIO_U8 audio format.\n");
	return 0;
    }
}

    PipeInit(&audiostream);
    return 1;
}



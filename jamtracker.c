/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <strings.h>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

struct {
	size_t len;
	size_t loop_start;
	size_t loop_end;
	uint8_t vol;
	unsigned char *data;
} Samples[31];

size_t nSamples = 0;

struct {
	unsigned char np;
	unsigned char flag;
	unsigned char seq[128];
	unsigned char id[4];
} ModSeq;

struct _SamplerStatus {
	uint32_t phase[8];
	int sample[8];
	uint32_t pointer[8];
} SamplerGlobalStatus;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The font
TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 0, 0, 0 };

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *scratch = NULL;
Mix_Chunk *high = NULL;
Mix_Chunk *med = NULL;
Mix_Chunk *low = NULL;

Mix_Chunk test[31];

SDL_mutex *mutex;

SDL_Surface *load_image(const char *filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

int load_mod(char *modfile)
{
	char modtype[5];
	char modname[21];
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

	if(fread(modname, 20, 1, fp)!=1)
	{
		goto failure;
	}

	modtype[4]='\0';
	modname[20]='\0';

	if (modtype[0]=='M' && modtype[1]=='.' && modtype[2]=='K' && modtype[3]=='.') {
		smpn=31;
	} else {
		printf("Not supported\n");
		goto failure;
	}

	printf("ModType: %s\n", modtype);
	printf("Modname: %s\n", modname);
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
	fseek(fp, max*64*4*4, SEEK_CUR);
	printf("L: %ld\n", ftell(fp));
	for(i=0;i<nSamples;i++) {
		printf("S: %u\n", Samples[i].len);
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

int main( int argc, char* args[] )
{
	if( argc > 1 )
	{
		load_mod(args[1]);
	}

    //Initialize
    if( init_new() == 0 )
    {
        return 1;
    }

    //Load the files
    if( load_files_new() == 0 )
    {
        return 1;
    }

    printf("Ready!\n");

    //Apply the background
    apply_surface( 0, 0, background, screen, NULL );

    //Render the text
    message = TTF_RenderText_Solid( font, "Up/Down to select the sound", textColor );

    //If there was an error in rendering the text
    if( message == NULL )
    {
        return 1;
    }

    //Show the message on the screen
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, 100, message, screen, NULL );

    //Free the message
    SDL_FreeSurface( message );

    //Render the text
    message = TTF_RenderText_Solid( font, "Letters to play the notes", textColor );

    //If there was an error in rendering the text
    if( message == NULL )
    {
        return 1;
    }

    //Show the message on the screen
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, 200, message, screen, NULL );

    //Free the message
    SDL_FreeSurface( message );

    //Render the text
    message = TTF_RenderText_Solid( font, "zSxDcvGbHnJm - q2w3er5t6y7ui9o0p", textColor );

    //If there was an error in rendering the text
    if( message == NULL )
    {
        return 1;
    }

    //Show the message on the screen
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, 300, message, screen, NULL );

    //Free the message
    SDL_FreeSurface( message );

    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }

    main_loop_new();
    //Free surfaces, fonts and sounds
    //then quit SDL_ttf and SDL
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

float oct_tab[12]={1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0};

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
		memcpy(SamplerGlobalStatus.pointer, x->pointer, sizeof(int)*8 );
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
	while(!quit)
	{
	        //While there's events to handle
		while( SDL_WaitEvent( &event ) )
        	{
	            //If a key was pressed
		        if(event.type == SDL_KEYUP)
			{
				set_note_off();
			}

        	    	if( event.type == SDL_KEYDOWN )
            		{

			int sl = -1;
			switch(event.key.keysym.sym)
			{
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
			}

			if(sl!=-1) {
				set_note(sl, smp);
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
	SDL_CloseAudio();
}


void cleanup_new()
{
    //Free the surfaces
    SDL_FreeSurface( background );

    //Close the font
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

int load_files_new()
{
	int i=0;
    //Load the background image
    background = load_image( "data/background.png" );

    //Open the font
    font = TTF_OpenFont( "data/lazy.ttf", 30 );

    //If there was a problem in loading the background
    if( background == NULL )
    {
        return 0;
    }

    //If there was an error in loading the font
    if( font == NULL )
    {
        return 0;
    }

    //If everything loaded fine
    return 1;
}

struct _SamplerStatus statuscpy;

void audio_callback(void*  userdata, Uint8* stream, int len)
{

	int i = 0;
	get_sampler_status(&statuscpy);

	for(i=0;i<len;i++) 
	{
		int c = statuscpy.sample[0];
		uint32_t bp = statuscpy.pointer[0] >> 16;
		uint32_t ph = statuscpy.phase[0];
		uint32_t le, ls;
		float vol = 1.0;
		int a = 0;

		if (c < 0 || Samples[c].len < 1 || bp >= Samples[c].len) {
			stream[i] = 128;
			continue;
		}

		/*
		if (bp >= Samples[c].len)
		{
			bp %= Samples[c].len;
		}
		*/

		vol = Samples[c].vol;
		a = ((char)Samples[c].data[bp]) * (vol/64.0);

		if (a < -128) {
			a = -128;
		}
		if (a > 127) {
			a = 127;
		}

		stream[i] = a+128;
		statuscpy.pointer[0]+=statuscpy.phase[0];
		ls = Samples[c].loop_start << 16;
		le = Samples[c].loop_end   << 16;

		if(le > ls && statuscpy.pointer[0] >= le ) {
			statuscpy.pointer[0] = ls + (statuscpy.pointer[0]-le);
		}
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


    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return 0;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return 0;
    }

	SDL_AudioSpec want, have;

	memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.freq = 22050;
	want.format = AUDIO_U8;
	want.channels = 1;
	want.samples = 1024;
	want.callback = audio_callback; /* you wrote this function elsewhere -- see SDL_AudioSpec for details */

if (SDL_OpenAudio(&want, &have) < 0) {
    printf("Failed to open audio: %s\n", SDL_GetError());
    return 1;
} else {
    if (have.format != want.format) {
        printf("We didn't get AUDIO_U8 audio format.\n");
	return 0;
    }
}

    //Initialize SDL_mixer
    /*
    if( Mix_OpenAudio( 22050, AUDIO_S8, 1, 1024 ) == -1 )
    {
        return false;
    } else {
int numtimesopened, frequency, channels;
Uint16 format;
numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);
if(!numtimesopened) {
    printf("Mix_QuerySpec: %s\n",Mix_GetError());
}
else {
    char *format_str="Unknown";
    switch(format) {
        case AUDIO_U8: format_str="U8"; break;
        case AUDIO_S8: format_str="S8"; break;
        case AUDIO_U16LSB: format_str="U16LSB"; break;
        case AUDIO_S16LSB: format_str="S16LSB"; break;
        case AUDIO_U16MSB: format_str="U16MSB"; break;
        case AUDIO_S16MSB: format_str="S16MSB"; break;
    }
    printf("opened=%d times  frequency=%dHz  format=%s  channels=%d",
            numtimesopened, frequency, format_str, channels);
}

    }
    */
    //Set the window caption
    SDL_WM_SetCaption( "Jam Tracker", NULL );

    //If everything initialized fine
    return 1;
}



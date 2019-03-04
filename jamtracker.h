#ifndef _jamtracker_h 
	#define _jamtracker_h
#include "musmod.h"

typedef struct _SamplerStatus {
	uint32_t phase[8];
	uint16_t period[8];
	int      sample[8];
	uint32_t pointer[8];
	uint8_t  volume[8];
	Sample   *smpdata;
} SamplerStatus;

typedef struct {
	ModPattern *patterns;
	size_t npat;
	uint8_t *seq;
	size_t nseq;
	Sample *smpdata;
	ModMeta *meta;
} SequencerData;

extern uint8_t note_period_tab[2048];
extern uint32_t period_phase[2048];

extern Uint32 SequencerEvent;

int start_sequencer(SequencerData *data);
int stop_sequencer();
void sequence_start();
void sequence_resume(int point);
int sequence_status();

void dump_channel_item(ChannelItem *p);
void set_note(uint8_t note, uint8_t smp);
void dump_channel_item_str(char *dest, ChannelItem *p);
int get_song_position();
int get_song_status();
void set_note_ex(uint8_t note[8], uint8_t smp[8]);
void get_channel_avg(int avg[8]);

#define PAUSE_FLAG   0x100

#define SEQUENCER_PLAYING 0
#define SEQUENCER_STOPPED 1
#define SEQUENCER_PAUSED  2

#define SONG_BREAK   0xFFFF
#define SONG_RESTART 0xFFFE
#define SONG_PAUSE   0xFFFD
#define SONG_EVENT_BASE 0x0100
#define SONG_EVENT(X)   (0x0100+(X))

void set_song_command(int cmd);

#define PIPESIZE 4096

typedef struct {
	volatile uint8_t buffer[PIPESIZE];
	volatile size_t rp;
	volatile size_t wp;
	volatile size_t len;
	volatile SDL_bool notfull;
	SDL_mutex *lock;
	SDL_cond *cond;
}Pipe;

void PipeInit(Pipe *pipe);
void PipeSend(Pipe *pipe, uint8_t *src, size_t len);
size_t PipeReceive(Pipe *pipe, uint8_t *dest, size_t len);

#endif

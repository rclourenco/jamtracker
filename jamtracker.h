#ifndef _jamtracker_h 
	#define _jamtracker_h

struct _Sample {
	size_t len;
	size_t loop_start;
	size_t loop_end;
	uint8_t vol;
	uint8_t tune;
	unsigned char *data;
};

struct _SamplerStatus {
	uint32_t phase[8];
	int      sample[8];
	uint32_t pointer[8];
	uint8_t  volume[8];
};

typedef struct {
	uint8_t data[4];
} ChannelItem;

typedef struct {
	ChannelItem item[4*64];
} ModPattern;

typedef struct {
	ModPattern *patterns;
	size_t npat;
	uint8_t *seq;
	size_t nseq;
} SequencerData;

extern uint8_t note_period_tab[2048];
extern uint32_t period_phase[2048];

extern Uint32 SequencerEvent;

int start_sequencer(SequencerData *data);
int stop_sequencer();
void dump_channel_item(ChannelItem *p);
void set_note(uint8_t note, uint8_t smp);
void dump_channel_item_str(char *dest, ChannelItem *p);

void set_note_ex(uint8_t note[8], uint8_t smp[8]);

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

#ifndef _jamtracker_h 
	#define _jamtracker_h

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
extern Uint32 SequencerEvent;

int start_sequencer(SequencerData *data);
int stop_sequencer();
void dump_channel_item(ChannelItem *p);
void set_note(uint8_t note, uint8_t smp);
void dump_channel_item_str(char *dest, ChannelItem *p);

void set_note_ex(uint8_t note[8], uint8_t smp[8]);

#endif

#ifndef _MUSMOD_H
	#define _MUSMOD_H

typedef struct _Sample {
	size_t len;
	size_t loop_start;
	size_t loop_end;
	uint8_t vol;
	uint8_t tune;
	unsigned char *data;
} Sample;

typedef struct {
	unsigned char np;
	unsigned char flag;
	unsigned char seq[128];
	unsigned char id[4];
} ModSequence;

typedef struct {
	uint8_t data[4];
} ChannelItem;

typedef struct {
	ChannelItem item[4*64];
} ModPattern;

typedef struct {
	ModSequence ms;
	char name[21];
	ModPattern patterns[127];
	Sample samples[31];
	uint8_t lps;
	uint8_t nsamples;
} MusicModule;

MusicModule *musmod_load(char *modfile);

#endif

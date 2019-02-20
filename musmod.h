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
	int  active;
	char global[12];
	char perseq[128][13];
} ModMeta;

typedef struct {
	ModSequence ms;
	char name[21];
	ModPattern patterns[127];
	Sample samples[31];
	uint8_t lps;
	uint8_t nsamples;
	ModMeta meta;
} MusicModule;

MusicModule *musmod_load(char *modfile);
void musmod_free(MusicModule *mm);

typedef struct {
	char **list;
	size_t len;
	size_t pos;
	MusicModule *cur;
} MusicModuleList;

MusicModuleList *musmod_open_list(const char *list);
MusicModule *musmod_list_next(MusicModuleList *mml);
MusicModule *musmod_list_prev(MusicModuleList *mml);
int musmod_list_islast(MusicModuleList *mml);
int musmod_list_isfirst(MusicModuleList *mml);


#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "musmod.h"

MusicModule *musmod_load(char *modfile)
{
	char modtype[5];
	int smpn = 15;
	int i = 0;
	int max = 0;

	MusicModule *mm = NULL;

	FILE *fp;
	fp = fopen(modfile, "rb");
	if( !fp )
	{
		return 0;
	}

	mm = (MusicModule *)malloc(sizeof(MusicModule));
	if (!mm) {
		goto failure;
	}

	memset(mm, 0, sizeof(MusicModule));

	fseek(fp, 1080, SEEK_SET);
	if(fread(modtype, 4, 1, fp)!=1)
	{
		goto failure;
	}
	fseek(fp, 0, SEEK_SET);

	if(fread(mm->name, 20, 1, fp)!=1) {
	
		goto failure;
	}

	modtype[4]='\0';
	mm->name[20]='\0';

	if (!strcmp(modtype, "M.K.")) {
		smpn=31;
	}
	else if (!strcmp(modtype, "M!K!")) {
		smpn=31;
	}
	else if (modtype[0]=='M' && modtype[1]=='.' && modtype[2]=='K' && modtype[3]=='.') {
		smpn=31;
	} else {
		printf("Not supported\n");
		goto failure;
	}

	printf("ModType: %s\n", modtype);
	printf("Modname: %s\n", mm->name);

	for(i=0;i<smpn;i++) {
		char smpname[23];
		unsigned char smpdef[30];
		uint32_t slen = 0, ls = 0, llen = 0;
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
		mm->samples[i].len = slen;
		mm->samples[i].data = NULL;
		mm->samples[i].vol  = smpdef[25];
		mm->samples[i].tune = smpdef[24]&0xF;
		if (mm->samples[i].vol == 0) {
			mm->samples[i].vol = 32;
		}

		mm->samples[i].loop_start = ls;
		if (llen>2) {
			mm->samples[i].loop_end = ls+llen;
		} else {
			mm->samples[i].loop_end = ls;
		}
	}

	mm->nsamples = smpn;
	if (fread(&mm->ms, sizeof(ModSequence), 1, fp)!=1) {
		goto failure;
	}

	printf("Number of patterns: %u\n", mm->ms.np);
	printf("Flag: %u\n", mm->ms.flag);
	printf("ID: %c%c%c%c\n", mm->ms.id[0], mm->ms.id[1], mm->ms.id[2], mm->ms.id[3]);

	printf("L: %ld\n", ftell(fp));
	
	for(i=0;i<=mm->ms.np;i++) {
		if (mm->ms.seq[i]>max) 
			max = mm->ms.seq[i];
	}
	max++;
	printf("Max %d\n", max);
	if (max>127) {
		fprintf(stderr, "Unsupported number o patterns\n");
		goto failure;
	}
	
	for (i=0; i<max; i++) {
		if (fread(&mm->patterns[i], sizeof(ModPattern), 1, fp)!=1) {
			goto failure;
		}
	}
	mm->lps=max;
	
	printf("L: %ld\n", ftell(fp));

	for(i=0;i<mm->nsamples;i++) {
		printf("S: %u\n", (unsigned int)mm->samples[i].len);
		if (mm->samples[i].len == 0)
			continue;
		mm->samples[i].data = (unsigned char *)malloc(mm->samples[i].len);
		if (mm->samples[i].data == NULL) {
			goto failure;
		}
		if (fread(mm->samples[i].data, mm->samples[i].len, 1, fp)!=1)
			goto failure;
	}

	printf("L: %ld\n", ftell(fp));

	fclose(fp);
	return mm;
failure:
	if(mm) {
		for(i=0;i<mm->nsamples;i++)
		{
			if (mm->samples[i].data)
				free(mm->samples[i].data);
		}

		free(mm);
	}
	fclose(fp);
	return NULL;
}

void musmod_free(MusicModule *mm)
{
	int i;
	if(mm) {
		for(i=0;i<mm->nsamples;i++)
		{
			if (mm->samples[i].data)
				free(mm->samples[i].data);
		}
		free(mm);
	}
}

int readline(char *buffer, size_t len, FILE *fp)
{
	int ch;
	int c=0;
	while( (ch=fgetc(fp))!=EOF && ch!='\n' ) {
		if (c<len) {
			*buffer = ch;
			buffer++;
		}
		c++;
	}
	*buffer='\0';
	return c;
}

MusicModuleList *musmod_open_list(const char *list)
{
	FILE *fp = NULL;
	char *s = NULL;
	char buffer[256];
	int lines = 0;

	if (!list)
		return NULL;

	MusicModuleList *mml = (MusicModuleList *)malloc( sizeof(MusicModuleList));
	if (!mml)
		return NULL;
	mml->len = 0;
	mml->pos = 0;
	mml->list = NULL;
	mml->cur = NULL;

	s = strstr(list, ".pls");

	if (!s || strcmp(s, ".pls")) {

		mml->list = (char **)malloc(sizeof(char *));
		if (!mml->list)
			goto failure;

		mml->len = 1;
		mml->list[0] = strdup(list);
		if (!mml->list[0])
			goto failure;

		return mml;
	}

	fp = fopen(list, "rt");
	if (!fp)
		goto failure;

	while(!feof(fp)) {
		if (readline(buffer, 255, fp))
			lines++;
	}
	rewind(fp);
	printf("Lines %d\n", lines);
	if (lines == 0)
		goto failure;

	mml->list = (char **) malloc(sizeof(char *)*lines);
	if (!mml->list)
			goto failure;

	mml->len = 0;

	while(!feof(fp)) {
		if (readline(buffer, 255, fp)) {
			size_t p = mml->len;

			if (p>lines)                //should never happen
				goto failure; 

			mml->list[p] = strdup(buffer);
			if (!mml->list[p])
				goto failure;
			mml->len++;
		}
	}

	int i;
	for(i=0;i<mml->len;i++)
	{
		printf("> %s\n", mml->list[i]);
	}

	fclose(fp);
	return mml;

failure:
	if (fp)
		fclose(fp);
	if (mml) {
		int i;
		if (mml->list) {
			for(i=0;i<mml->len; i++)
			{
				if (mml->list[i])
					free(mml->list[i]);
			}
			free(mml->list);
		}
		free(mml);
	}

	return NULL;
}

MusicModule *musmod_list_next(MusicModuleList *mml)
{
	MusicModule *new = NULL;
	if (!mml)
		return NULL;

	while (mml->pos < mml->len) {
		new = musmod_load(mml->list[mml->pos]);
		mml->pos++;

		if (new!=NULL) {
			if (mml->cur) {
				musmod_free(mml->cur);
			}
			mml->cur = new;
			break;
		}
	}

	return mml->cur;
}

MusicModule *musmod_list_prev(MusicModuleList *mml)
{
	MusicModule *new = NULL;
	if (!mml)
		return NULL;

	if (mml->len==0)
		return mml->cur;

	while (mml->pos>1) {
		mml->pos-=2;
		new = musmod_load(mml->list[mml->pos]);
		mml->pos++;

		if (new!=NULL) {
			if (mml->cur) {
				musmod_free(mml->cur);
			}
			mml->cur = new;
			break;
		}
	}

	return mml->cur;
}

int musmod_list_islast(MusicModuleList *mml)
{
	if (!mml)
		return 1;

	return mml->pos >= mml->len;
}

int musmod_list_isfirst(MusicModuleList *mml)
{
	if (!mml)
		return 1;

	return mml->pos < 2;
}



void musmod_list_close(MusicModuleList *mml)
{
	if (mml) {
		int i;

		if (mml->cur) {
			musmod_free(mml->cur);
		}

		if (mml->list) {
			for(i=0;i<mml->len; i++)
			{
				if (mml->list[i])
					free(mml->list[i]);
			}
			free(mml->list);
		}
		free(mml);
	}

}

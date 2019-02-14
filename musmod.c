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
		free(mm);
	}
	fclose(fp);
	return NULL;
}



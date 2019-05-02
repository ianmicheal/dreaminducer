/* Things missing from this module: play_music() stop_music() */

#include <kos.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "image.h"
#include "xmlparse.h"
#include "debug.h"
#include <time.h>
#include "gmtime.h"

#define MUSIC_NONE	0
#define MUSIC_MODPLUG	1
#define MUSIC_MP3	2

typedef struct {   
	int type;
	char *filename;
} musent_t;
static musent_t musent[64];
static int nummus;

static void music_enqueue(int type, char *filename)
{
	musent[nummus].type=type;
	musent[nummus].filename=malloc(strlen(filename)+1);
	strcpy(musent[nummus].filename,filename);
	nummus++;
}

static void musicent_start(void *userData, const char *name, const char **attr)
{
	int i;

	if(!stricmp(name,"modplug")) {
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"src")) {
				music_enqueue(MUSIC_MODPLUG, (char *)attr[i+1]);
			}
		}		
	}
	if(!stricmp(name,"mp3")) {
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"src")) {
				music_enqueue(MUSIC_MP3, (char *)attr[i+1]);
			}
		}		
	}
}

static void musicent_end(void *userData, const char *name)
{
}

void load_music(char *filename) {
	int fd=0;
	int len=0;
	int done=1;
	uint8 *buf=NULL;

	memset(musent,0,sizeof(musent_t)*64);
	nummus=0;

	XML_Parser p = XML_ParserCreate(NULL);
	if(!p) {
		printf("Couldn't allocate memory for parser\r\n");
		goto errout;
	}

	XML_SetElementHandler(p, musicent_start, musicent_end);

	fd=fs_open(filename, O_RDONLY);
	if(fd==0) { printf("load_music: file not found\r\n"); goto errout; }
	len=fs_total(fd);
	buf=malloc(len); 
	if(!buf) {
		printf("Couldn't allocate memory for buffer\r\n");
		goto errout;
	}

	fs_read(fd, buf, len);
	fs_close(fd); fd=0;   

	if (! XML_Parse(p, buf, len, done)) {
		fprintf(stderr, "Parse error at line %d:\n%s\n",
		XML_GetCurrentLineNumber(p),
		XML_ErrorString(XML_GetErrorCode(p)));
		goto errout;
	}

	return;

errout:
        if(fd) fs_close(fd);
        if(p) free(p);
        return;
}


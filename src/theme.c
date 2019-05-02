#include <kos.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "image.h"
#include "xmlparse.h"
#include "list.h"
#include "debug.h"
#include <time.h>
#include "gmtime.h"
#include "tmalloc.h"

time_t rtc_unix_secs();

#define TE_TYPE_NONE	0
#define TE_TYPE_IMAGE	1
#define TE_TYPE_LIST	2
#define TE_TYPE_SCREEN	3
#define TE_TYPE_DESC	4
#define TE_TYPE_TIME	5

typedef struct {
	int	type;
	float	x1,x2,y1,y2,z1;
	float	u1,u2,v1,v2;
	float	a,r,g,b;
	uint32	txraddy;
	uint8	txrmode;
	uint16	tw,th;
	int	font;
} tentry_t;
static tentry_t *tentry=NULL;

char	*clockdelim=NULL;
int	clockampm=1;

static int te_numentries=0;
static int te_curentry=0;

int has_screenshot=0;

static void te_start(void *userData, const char *name, const char **attr)
{
	int i;

	if(!stricmp(name,"author")) {
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"value"))
				printf("Author: %s\r\n", attr[i+1]);
		}
	} else
	if(!stricmp(name,"title")) {
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"value"))
				printf("Title: %s\r\n", attr[i+1]);
		}
	} else
	if(!stricmp(name,"font")) {
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"src")) {
				InducerImage *timg;

				timg = libgail_load_image(attr[i+1],TA_ARGB4444);
				if(timg!=NULL) {
					fonttxr = libgail_image_to_txr(timg, &fonttw, &fontth);
					libgail_free_image(timg);
				}
			}
		}
	} else
	if(!stricmp(name,"layout")) {
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"elements")) {
				has_screenshot=0;
				te_numentries=atoi(attr[i+1]);
				if(tentry) free(tentry);
				tentry=malloc(sizeof(tentry_t)*te_numentries);
				te_curentry=0;
			}
		}
	} else
	if(tentry && (te_curentry < te_numentries)) {
		tentry[te_curentry].a = tentry[te_curentry].r = 
			tentry[te_curentry].g = tentry[te_curentry].b = 1.0f;
		tentry[te_curentry].font = 24;
		if(!stricmp(name,"image")) {
			uint8	 *tfn=NULL;
			tentry[te_curentry].type = TE_TYPE_IMAGE;
			tentry[te_curentry].txrmode = TA_ARGB4444;
			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"x"))
					tentry[te_curentry].x1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"y"))
					tentry[te_curentry].y1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"z"))
					tentry[te_curentry].z1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"w"))
					tentry[te_curentry].x2=tentry[te_curentry].x1+atoi(attr[i+1]);
				if(!stricmp(attr[i],"h"))
					tentry[te_curentry].y2=tentry[te_curentry].y1+atoi(attr[i+1]);
				if(!stricmp(attr[i],"a"))
					tentry[te_curentry].a=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"r"))
					tentry[te_curentry].r=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"g"))
					tentry[te_curentry].g=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"b"))
					tentry[te_curentry].b=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"mode")) {
					if(!stricmp(attr[i+1],"argb4444"))
						tentry[te_curentry].txrmode=TA_ARGB4444;
					if(!stricmp(attr[i+1],"argb1555"))
						tentry[te_curentry].txrmode=TA_ARGB1555;
					if(!stricmp(attr[i+1],"rgb565"))
						tentry[te_curentry].txrmode=TA_RGB565;
//					if(!stricmp(attr[i+1],"yuv422"))
//						tentry[te_curentry].txrmode=TA_YUV422;
				}
				if(!stricmp(attr[i],"src"))
					tfn=(char *)attr[i+1];
			}
			if(tfn) {
				InducerImage *timg;

				timg = libgail_load_image(tfn,tentry[te_curentry].txrmode);
				if(timg!=NULL) {
					tentry[te_curentry].txraddy = libgail_image_to_txr(timg, &(tentry[te_curentry].tw), &(tentry[te_curentry].th));
					tentry[te_curentry].u1=tentry[te_curentry].v1=0.0f;
					tentry[te_curentry].u2=(timg->width*1.0f)/(tentry[te_curentry].tw*1.0f);
					tentry[te_curentry].v2=(timg->height*1.0f)/(tentry[te_curentry].th*1.0f);
					libgail_free_image(timg);
				}
			}
			te_curentry++;
		}

		if(!stricmp(name,"list")) {
			tentry[te_curentry].type = TE_TYPE_LIST;
			for (i = 0; attr[i]; i += 2)
				if(!stricmp(attr[i],"font"))
					tentry[te_curentry].font=atoi(attr[i+1]);

			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"x"))
					tentry[te_curentry].x1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"y"))
					tentry[te_curentry].y1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"z"))
					tentry[te_curentry].z1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"w")) {
					tentry[te_curentry].x2=tentry[te_curentry].x1+atoi(attr[i+1]);
					menu_list.w=atoi(attr[i+1])/(tentry[te_curentry].font/2);
				}
				if(!stricmp(attr[i],"h")) {
					tentry[te_curentry].y2=tentry[te_curentry].y1+atoi(attr[i+1]);
					menu_list.h=atoi(attr[i+1])/tentry[te_curentry].font;
				}
				if(!stricmp(attr[i],"a"))
					tentry[te_curentry].a=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"r"))
					tentry[te_curentry].r=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"g"))
					tentry[te_curentry].g=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"b"))
					tentry[te_curentry].b=atoi(attr[i+1])/256.0f;
			}
			te_curentry++;
		}

		if(!stricmp(name,"desc")) {
			tentry[te_curentry].type = TE_TYPE_DESC;
			for (i = 0; attr[i]; i += 2)
				if(!stricmp(attr[i],"font"))
					tentry[te_curentry].font=atoi(attr[i+1]);

			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"x"))
					tentry[te_curentry].x1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"y"))
					tentry[te_curentry].y1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"z"))
					tentry[te_curentry].z1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"w")) {
					tentry[te_curentry].x2=tentry[te_curentry].x1+atoi(attr[i+1]);
					menu_desc.w=atoi(attr[i+1])/(tentry[te_curentry].font/2);
				}
				if(!stricmp(attr[i],"h")) {
					tentry[te_curentry].y2=tentry[te_curentry].y1+atoi(attr[i+1]);
					menu_desc.h=atoi(attr[i+1])/tentry[te_curentry].font;
				}
				if(!stricmp(attr[i],"a"))
					tentry[te_curentry].a=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"r"))
					tentry[te_curentry].r=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"g"))
					tentry[te_curentry].g=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"b"))
					tentry[te_curentry].b=atoi(attr[i+1])/256.0f;
			}
			te_curentry++;
		}

		if(!stricmp(name,"time")) {
			tentry[te_curentry].type = TE_TYPE_TIME;
			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"x"))
					tentry[te_curentry].x1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"y"))
					tentry[te_curentry].y1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"z"))
					tentry[te_curentry].z1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"a"))
					tentry[te_curentry].a=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"r"))
					tentry[te_curentry].r=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"g"))
					tentry[te_curentry].g=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"b"))
					tentry[te_curentry].b=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"font"))
					tentry[te_curentry].font=atoi(attr[i+1]);
				if(!stricmp(attr[i],"ampm"))
					clockampm=atoi(attr[i+1]);
			}
			te_curentry++;
		}

		if(!stricmp(name,"screenshot")) {
			has_screenshot=1;
			tentry[te_curentry].type = TE_TYPE_SCREEN;
			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"x"))
					tentry[te_curentry].x1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"y"))
					tentry[te_curentry].y1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"z"))
					tentry[te_curentry].z1=atoi(attr[i+1]);
				if(!stricmp(attr[i],"w"))
					tentry[te_curentry].x2=tentry[te_curentry].x1+atoi(attr[i+1]);
				if(!stricmp(attr[i],"h"))
					tentry[te_curentry].y2=tentry[te_curentry].y1+atoi(attr[i+1]);
				if(!stricmp(attr[i],"a"))
					tentry[te_curentry].a=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"r"))
					tentry[te_curentry].r=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"g"))
					tentry[te_curentry].g=atoi(attr[i+1])/256.0f;
				if(!stricmp(attr[i],"b"))
					tentry[te_curentry].b=atoi(attr[i+1])/256.0f;
			}
			te_curentry++;
		}
	}
}

static void te_end(void *userData, const char *name)
{
}

void load_theme(char *filename) {
	int fd=0;
	int len=0;
	int done=1;
	uint8 *buf=NULL;

	printf("load_theme: %s \r\n", filename);

	XML_Parser p = XML_ParserCreate(NULL);
	if(!p) {
		printf("Couldn't allocate memory for parser\r\n");
		goto errout;
	}

	XML_SetElementHandler(p, te_start, te_end);

	fd=fs_open(filename, O_RDONLY);
	if(fd==0) { printf("load_theme: file not found\r\n"); goto errout; }
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
	}

errout:
	if(buf) free(buf);
        if(fd) fs_close(fd);
        if(p) free(p);
        return;
}

void unload_theme() {
	int i;

	if(fonttxr) tfree(fonttxr);

	for(i=0; i<te_numentries; i++) {
		if(tentry[i].txraddy) tfree(tentry[i].txraddy);
	}

	if(tentry) { free(tentry); tentry=NULL; }
	te_numentries=0;
}

void theme_render() {
	int i;

	for(i=0; i<te_numentries; i++) {
		switch(tentry[i].type) {
			case TE_TYPE_IMAGE:
				libgail_send_header(tentry[i].txrmode,tentry[i].tw,tentry[i].th,tentry[i].txraddy,0);
				libgail_render_poly(
					tentry[i].x1,tentry[i].y1,tentry[i].z1,
					tentry[i].x2,tentry[i].y2,
					tentry[i].u1,tentry[i].v1,
					tentry[i].u2,tentry[i].v2, 
					tentry[i].a,tentry[i].r,
					tentry[i].g,tentry[i].b); 
				break;
			case TE_TYPE_SCREEN:
				scrn_render(tentry[i].x1,tentry[i].y1,tentry[i].z1,tentry[i].x2,tentry[i].y2,tentry[i].a,tentry[i].r,tentry[i].g,tentry[i].b);
				break;
			case TE_TYPE_LIST:
				list_render(tentry[i].x1,tentry[i].y1,tentry[i].z1,tentry[i].x2,tentry[i].y2,tentry[i].a,tentry[i].r,tentry[i].g,tentry[i].b,tentry[i].font);
				break;
			case TE_TYPE_DESC:
				desc_render(tentry[i].x1,tentry[i].y1,tentry[i].z1,tentry[i].x2,tentry[i].y2,tentry[i].a,tentry[i].r,tentry[i].g,tentry[i].b,tentry[i].font);
				break;
			case TE_TYPE_TIME:
				{
					static char timestring[24];
					static uint32 lc; 

					if(lc==0) lc=jiffies-50;
					if(jiffies-lc>33) {
						time_t 	t=rtc_unix_secs();
						struct tm *gm=gmtime(&t);
						int hour=gm->tm_hour;
						char ampm[4];

						if(clockampm) {
							if(((hour>=0) && (hour<=11)) || (hour==24)) {
								strcpy(ampm, " am");
								if((hour==0) || (hour==24)) hour=12;
							} else {
								strcpy(ampm, " pm");
								if(hour>12) hour-=12;
							}
						} else {
							strcpy(ampm," ");
						}

						sprintf(timestring,"%02d:%02d:%02d%s",hour,gm->tm_min,gm->tm_sec,ampm);
						lc=jiffies;
					}
					draw_string(tentry[i].font,tentry[i].x1,tentry[i].x1+16*tentry[i].font,tentry[i].y1,tentry[i].z1,tentry[i].a,tentry[i].r,tentry[i].g,tentry[i].b,timestring);
				}
				break;
		}
	}
}

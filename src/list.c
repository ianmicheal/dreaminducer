#include <kos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "xmlparse.h"
#include "list.h"
#include "theme.h"
#include "image.h"
#include "tmalloc.h"
#include "intro.h"
#include "divx.h"

char *currentlist;

#define LE_TYPE_NONE	0
#define LE_TYPE_BIN	1
#define LE_TYPE_LINK	2
#define LE_TYPE_MOVIE	3

static uint32	scrntxr;
static uint16	scrntw,scrnth;
static uint16	scrniw,scrnih;
static uint8	scrnfmt;

extern int has_screenshot;

typedef struct {   
	uint8	type;
        char    *title;
	char	*screenshot;
	char	*description;
	char	*target;
	char	*preroll;
	char	*postroll;
} entry_t;
static entry_t *mentry=NULL;

uint32 fonttxr;
uint16 fonttw, fontth;

textbox_t menu_list;
textbox_t menu_desc;

static int list_numentries=0;
static int list_curentry=0;
static int list_top=0;

void load_scrn();

static void list_start(void *userData, const char *name, const char **attr)
{
	int i;

	if(!stricmp(name,"list")) {
		scrnfmt=TA_ARGB4444;
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"entries")) {
				list_numentries=atoi(attr[i+1]);
				if(mentry) {
					free(mentry);
				}
				mentry=malloc(sizeof(entry_t)*list_numentries);
				memset(mentry,0,sizeof(entry_t)*list_numentries);
				list_curentry=0;
				list_top=0;
			}
			if(!stricmp(attr[i],"theme")) {
				load_theme((char *)attr[i+1]);
			}
			if(!stricmp(attr[i],"mode")) {
				if(!stricmp(attr[i+1],"argb4444"))
					scrnfmt=TA_ARGB4444;
				if(!stricmp(attr[i+1],"argb1555"))
					scrnfmt=TA_ARGB1555;
				if(!stricmp(attr[i+1],"rgb565"))
					scrnfmt=TA_RGB565;
				if(!stricmp(attr[i+1],"yuv422"))
					scrnfmt=TA_YUV422;
			}      

		}
		return;
	}
	if(mentry && (list_curentry < list_numentries)) {
		if(!stricmp(name,"item")) {
			mentry[list_curentry].type=LE_TYPE_BIN;
			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"title")) {
					mentry[list_curentry].title=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].title,attr[i+1]);
				}
				if(!stricmp(attr[i],"screenshot")) {
					mentry[list_curentry].screenshot=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].screenshot,attr[i+1]);
				}
				if(!stricmp(attr[i],"description")) {
					mentry[list_curentry].description=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].description,attr[i+1]);
				}
				if(!stricmp(attr[i],"target")) {
					mentry[list_curentry].target=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].target,attr[i+1]);
				}
				if(!stricmp(attr[i],"preroll")) {
					mentry[list_curentry].preroll=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].preroll,attr[i+1]);
				}
			}
			list_curentry++;
		}
		if(!stricmp(name,"movie")) {
			mentry[list_curentry].type=LE_TYPE_MOVIE;
			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"title")) {
					mentry[list_curentry].title=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].title,attr[i+1]);
				}
				if(!stricmp(attr[i],"screenshot")) {
					mentry[list_curentry].screenshot=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].screenshot,attr[i+1]);
				}
				if(!stricmp(attr[i],"description")) {
					mentry[list_curentry].description=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].description,attr[i+1]);
				}
				if(!stricmp(attr[i],"target")) {
					mentry[list_curentry].target=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].target,attr[i+1]);
				}
				if(!stricmp(attr[i],"preroll")) {
					mentry[list_curentry].preroll=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].preroll,attr[i+1]);
				}
				if(!stricmp(attr[i],"postroll")) {
					mentry[list_curentry].postroll=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].postroll,attr[i+1]);
				}
			}
			list_curentry++;
		}
		if(!stricmp(name,"link")) {
			mentry[list_curentry].type=LE_TYPE_LINK;
			for (i = 0; attr[i]; i += 2) {
				if(!stricmp(attr[i],"title")) {
					mentry[list_curentry].title=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].title,attr[i+1]);
				}
				if(!stricmp(attr[i],"screenshot")) {
					mentry[list_curentry].screenshot=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].screenshot,attr[i+1]);
				}
				if(!stricmp(attr[i],"description")) {
					mentry[list_curentry].description=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].description,attr[i+1]);
				}
				if(!stricmp(attr[i],"target")) {
					mentry[list_curentry].target=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].target,attr[i+1]);
				}
				if(!stricmp(attr[i],"preroll")) {
					mentry[list_curentry].preroll=malloc(strlen(attr[i+1])+1);
					strcpy(mentry[list_curentry].preroll,attr[i+1]);
				}
			}
			list_curentry++;
		}
	}
}

static void list_end(void *userData, const char *name)
{
/* We don't do anything special for the end of each tag */
}

void load_list(char *filename) {
	int fd=0;
	int len=0;
	int done=1;
	uint8 *buf=NULL;

	XML_Parser p = XML_ParserCreate(NULL);
	if(!p) {
		printf("Couldn't allocate memory for parser\r\n");
		goto errout;
	}

	XML_SetElementHandler(p, list_start, list_end);

	/* take note of the name */
	currentlist=malloc(strlen(filename)+1);
	strcpy(currentlist,filename);

	/* read the file */
	fd=fs_open(filename, O_RDONLY);
	if(fd==0) { printf("load_list: file not found\r\n"); goto errout; }
	len=fs_total(fd);
	buf=malloc(len);
	if(!buf) {
		printf("Couldn't allocate memory for buffer\r\n");
		goto errout;
	}

	fs_read(fd, buf, len);
	fs_close(fd); fd=0;

	if (!XML_Parse(p, buf, len, done)) {
		fprintf(stderr, "Parse error at line %d:\n%s\n",
		XML_GetCurrentLineNumber(p),
		XML_ErrorString(XML_GetErrorCode(p)));
		goto errout;
	}

	free(p);
	list_curentry=0;
	load_scrn();

	return;

errout:
	list_numentries=0;
	if(currentlist) { free(currentlist); currentlist=NULL; }
	if(mentry) { free(mentry); mentry=NULL; }
	if(fd) fs_close(fd);
	if(p) { free(p); p=NULL; }
	return;
}

void unload_list() {
	int i;

	unload_theme();
	if(scrntxr) tfree(scrntxr);
	scrniw=scrnih=scrntw=scrnth=0;

	for(i=0; i<list_numentries; i++) {
		if(mentry[i].title) free(mentry[i].title);
		if(mentry[i].screenshot) free(mentry[i].screenshot);
		if(mentry[i].description) free(mentry[i].description);
		if(mentry[i].target) free(mentry[i].target);
		if(mentry[i].preroll) free(mentry[i].preroll);
		if(mentry[i].postroll) free(mentry[i].postroll);
	}

	list_numentries=0;
	if(currentlist) { free(currentlist); currentlist=NULL; }
	if(mentry) { free(mentry); mentry=NULL; }

	return;
}

void load_scrn() {
	InducerImage *timg=NULL;

	if(scrntxr) tfree(scrntxr);
	scrntxr=scrntw=scrnth=0;

	if(!has_screenshot) return;

	if(mentry[list_curentry].screenshot)
		timg = libgail_load_image(mentry[list_curentry].screenshot, scrnfmt);
	if(timg) {
		scrniw=timg->width;
		scrnih=timg->height;
		scrntxr = libgail_image_to_txr(timg, &scrntw, &scrnth);
		libgail_free_image(timg);
	}
}

void scroll_up(int i) {
	if(list_curentry>=i) {
		list_curentry-=i;
		if(list_top>list_curentry)
			list_top=list_curentry;

		load_scrn();
	} else if(list_curentry>0) {
		list_curentry=0;
		if(list_top>list_curentry)
			list_top=0;

		load_scrn();
	}
}

void scroll_down(int i) {
	if(list_curentry+i<list_numentries) {
		list_curentry+=i;
		if(list_curentry>list_top+menu_list.h-1)
			list_top+=i;

		load_scrn();
	} else if(list_curentry<list_numentries) {
		list_curentry=list_numentries-1;
		if((list_curentry>=list_top+menu_list.h) && (menu_list.h<list_numentries))
			list_top=(list_curentry-menu_list.h)+1;

		load_scrn();
	}
}

#define BINLOADBASE 0x8cc0b800
void (*binload)() = (void (*)())BINLOADBASE;

void flush_icache_range(unsigned long start, unsigned long end);

void do_command() {
	switch(mentry[list_curentry].type) {
		case LE_TYPE_BIN: {
			char *pre, *tg;
			int lce;
			char *cl;

			lce=list_curentry;
			cl=malloc(strlen(currentlist)+1);
			strcpy(cl, currentlist);

			pre=tg=NULL;

			if(mentry[list_curentry].preroll) {
				pre=malloc(strlen(mentry[list_curentry].preroll)+1);
				strcpy(pre, mentry[list_curentry].preroll);
			}
			if(mentry[list_curentry].target) {
				tg=malloc(strlen(mentry[list_curentry].target)+1);
				strcpy(tg, mentry[list_curentry].target);
			}

			unload_list();

			if(pre) { load_intro(pre); free(pre); }

			if(tg) {
				file_t fd;
				size_t len,tr,r,i;
				uint8 *string=tg+4;
				uint8 *outplace=(uint8 *)(BINLOADBASE);

				fd=fs_open("/cd/binload.bin", O_RDONLY);
				if(!fd) {
					printf("Unable to open /cd/binload.bin\r\n");
					return;
				}

				tr=len=fs_total(fd);

				printf("Loading /cd/binload.bin (%i bytes) at 0x%x\r\n", len, BINLOADBASE);

				while(tr) {
					r = fs_read(fd, outplace, tr);
					tr -= r;
					outplace += r;
				}

				fs_close(fd);

				outplace=(uint8 *)(BINLOADBASE+0x17b4);

				printf("String at %p : %s \r\n", outplace,outplace);

				for(i=0; i<strlen(string); i++) {
					int c = string[i];
					outplace[i] = toupper(c);
				}
				outplace[i]=0;

				printf("String at %p : %s \r\n", outplace,outplace);
				printf("Killing KOS and starting binloader...\r\n");

				irq_disable();
				hardware_shutdown();
				irq_shutdown();
				arch_dtors();

				flush_icache_range(BINLOADBASE, len);

				binload();
			}

			load_list(cl);
			list_curentry=lce;
			load_scrn();
			free(cl);
			return;
		}
		break;

		case LE_TYPE_LINK: {
			char *pre, *tg;
			int lce;
			char *cl;

			pre=tg=NULL;

			if(mentry[list_curentry].preroll) {
				pre=malloc(strlen(mentry[list_curentry].preroll)+1);
				strcpy(pre, mentry[list_curentry].preroll);
			}
			if(mentry[list_curentry].target) {
				tg=malloc(strlen(mentry[list_curentry].target)+1);
				strcpy(tg, mentry[list_curentry].target);
			}

			lce=list_curentry;
			cl=malloc(strlen(currentlist)+1);
			strcpy(cl, currentlist);

			unload_list();

			if(pre) { load_intro(pre); free(pre); }
			if(tg) {
				load_list(tg);
 				free(tg);
			} else {
				load_list(cl);
				list_curentry=lce;
				load_scrn();
			}
			free(cl);

			return;
		}
		break;

		case LE_TYPE_MOVIE: {
			char *pre, *tg, *post;
			int lce;
			char *cl;

			pre=tg=post=NULL;

			if(mentry[list_curentry].preroll) {
				pre=malloc(strlen(mentry[list_curentry].preroll)+1);
				strcpy(pre, mentry[list_curentry].preroll);
			}
			if(mentry[list_curentry].target) {
				tg=malloc(strlen(mentry[list_curentry].target)+1);
				strcpy(tg, mentry[list_curentry].target);
			}
			if(mentry[list_curentry].postroll) {
				post=malloc(strlen(mentry[list_curentry].postroll)+1);
				strcpy(post, mentry[list_curentry].postroll);
			}

			lce=list_curentry;
			cl=malloc(strlen(currentlist)+1);
			strcpy(cl, currentlist);

			unload_list();

			if(pre) { load_intro(pre); free(pre); }
			if(tg) { play_divx(tg); free(tg); }
			if(post) { load_intro(post); free(post); }

			load_list(cl);
			list_curentry=lce;
			load_scrn();
			free(cl);
			return;
		}
		break;

		default:
			printf("do_command: ERROR! reached end of do_command, something is wrong here\r\n");
	}
}

void draw_char(int font, float x1, float y1, float z1, float a, float r,
		float g, float b, int c) {
	vertex_ot_t	vert;
	int		ix, iy;
	float		u1, v1, u2, v2;

	if((c<1)||(c>127)) return;
  
	ix = (c % 16) * 12;
	iy = (c / 16) * 24;
	u1 = ix * 1.0f / 256.0f;
	v1 = iy * 1.0f / 256.0f;
	u2 = (ix+12) * 1.0f / 256.0f;
	v2 = (iy+24) * 1.0f / 256.0f;

	vert.flags = TA_VERTEX_NORMAL;
	vert.x = x1; vert.y = y1 + font;
	vert.z = z1; vert.u = u1; vert.v = v2;
	vert.dummy1 = vert.dummy2 = 0;
	vert.a = a; vert.r = r; vert.g = g; vert.b = b;
	vert.oa = vert.or = vert.og = vert.ob = 0.0f;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.y = y1; vert.v = v1;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.x = x1 + font/2;
	vert.y = y1 + font;  
	vert.u = u2; vert.v = v2;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.flags = TA_VERTEX_EOL;
	vert.y = y1; vert.v = v1;
	ta_commit_vertex(&vert, sizeof(vert));
}

void draw_string(int font, float x1, float x2, float y, float z, float a, float r,
		float g, float b, char *str) {
	poly_hdr_t poly;
	float x=x1;
	ta_poly_hdr_txr(&poly, TA_TRANSLUCENT, TA_ARGB4444, fonttw, fontth,
		fonttxr, TA_BILINEAR_FILTER);
	ta_commit_poly_hdr(&poly);

	if(((uint32)(str)<0x8c010000) ||
			((uint32)(str)>0x8cffffff)) {
		return;
	}

	while ((*str) && (x + font/2 < x2)) {
		if(*str=='#') {
			x=x1; y+=font;
			str++;
		} else {
			draw_char(font, x, y, z, a, r, g, b, *str++);
			x+=font/2;
		}
	}
}

void scrn_render(float x1, float y1, float z1,
		float x2, float y2, float a, float r, float g, float b) {
	poly_hdr_t poly;

	if((!scrntw)&&(!scrnth)) return;

	ta_poly_hdr_txr(&poly, TA_TRANSLUCENT, scrnfmt, scrntw, scrnth, scrntxr, 0);
	ta_commit_poly_hdr(&poly);

	libgail_render_poly(x1, y1, z1, x2, y2, 0.0f, 0.0f, (scrniw*1.0f)/(scrntw*1.0f), (scrnih*1.0f)/(scrnth*1.0f), 1.0f, 1.0f, 1.0f, 1.0f);
}

void list_render(float x1, float y1, float z1,
		float x2, float y2, float a, float r, float g, float b, int font) {
	int i;

	for(i=list_top; i<list_top+menu_list.h; i++) {
		if(i<list_numentries) {
			if(i==list_curentry) {
				draw_string(font,x1,x2,y1+font*(i-list_top),z1,a,r,g,b, mentry[i].title);
			} else {
				draw_string(font,x1,x2,y1+font*(i-list_top),z1,a*0.5f,r,g,b, mentry[i].title);
			}
		}
	}
}

void desc_render(float x1, float y1, float z1,
		float x2, float y2, float a, float r, float g, float b, int font) {
	draw_string(font,x1,x2,y1,z1,a,r,g,b, mentry[list_curentry].description);
}

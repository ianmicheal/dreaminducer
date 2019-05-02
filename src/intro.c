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
#include <tmalloc.h>
#include "intro.h"
#include "divx.h"

time_t rtc_unix_secs();

static void display_txr(float fade, uint16 tw, uint16 th, uint16 w, uint16 h, uint32 taddy)
{
	vertex_ot_t     v;
	poly_hdr_t      hdr;   
	float x1,x2,y1,y2;
	float u1,u2,v1,v2;   

	if (fade < 0.0f) fade = 0.0f;

	ta_poly_hdr_txr(&hdr, TA_OPAQUE, TA_RGB565, tw, th, taddy, TA_NO_FILTER);
	ta_commit_poly_hdr(&hdr);

	x1=320.0f-w/2;
	x2=640.0f-x1;
	y1=240.0f-h/2;
	y2=480-y1;

	u1=v1=0.0f;
	u2=(w*1.0f)/(tw*1.0f);
	v2=(h*1.0f)/(th*1.0f);

	v.flags = TA_VERTEX_NORMAL;
	v.a = 1.0f; v.r = v.g = v.b = fade;
	v.oa = v.or = v.og = v.ob = 0.0f;

/*  - -  */	v.x = x1; v.y = y2; v.z = 1.0f;
/*  + -  */	v.u = u1; v.v = v2;
		ta_commit_vertex(&v, sizeof(v));

/*  + -  */	v.y = y1;
/*  - -  */	v.v = v1;
		ta_commit_vertex(&v, sizeof(v));

/*  - +  */	v.x = x2; v.y = y2;
/*  - -  */	v.u = u2; v.v = v2;
		ta_commit_vertex(&v, sizeof(v));

/*  - -  */	v.y = y1;
/*  - +  */	v.v = v1;
		v.flags = TA_VERTEX_EOL;
		ta_commit_vertex(&v, sizeof(v));
}

static void img_blank() {
        poly_hdr_t      hdr;

        ta_poly_hdr_col(&hdr, TA_TRANSLUCENT);
        ta_commit_poly_hdr(&hdr);
}

static void introent_start(void *userData, const char *name, const char **attr)
{
	int i;

	if(!stricmp(name,"movie")) {
		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"src")) {
				play_divx((char *)attr[i+1]);
			}
		}		
	} else if(!stricmp(name,"image")) {
		InducerImage *timg;
		uint16 w=0,h=0,tw,th;
		uint32 taddy=0xffffffff;
		uint16 delay=90;
		int nofadein=0;
		int nofadeout=0;

		for (i = 0; attr[i]; i += 2) {
			if(!stricmp(attr[i],"src")) {
				timg = libgail_load_image(attr[i+1],TA_RGB565);
				w=timg->width; h=timg->height;
				taddy= libgail_image_to_txr(timg, &tw, &th);
				libgail_free_image(timg);

				/* We free the texture here incase someone puts
				 * multiple SRC attributes in one IMAGE tag
				 */
				tfree(taddy);
			}

			if(!stricmp(attr[i],"delay"))
				delay=atoi(attr[i+1])+1;
			if(!stricmp(attr[i],"nofadein"))
				nofadein=atoi(attr[i+1]);
			if(!stricmp(attr[i],"nofadeout"))
				nofadeout=atoi(attr[i+1]);
		}
		
		if(taddy != 0xffffffff) {
			float f;

			if(nofadein==0)
				for(f=0.0f; f<1.0f; f+=0.025f) {
					ta_begin_render();
					display_txr(f,tw,th,w,h,taddy);
					ta_commit_eol();
					img_blank();
					ta_commit_eol();  
					ta_finish_frame();
				}
			for(; delay>0; delay--) {
				ta_begin_render();
				display_txr(1.0f,tw,th,w,h,taddy);
				ta_commit_eol();
				img_blank();
				ta_commit_eol();  
				ta_finish_frame();
			}
			if(nofadeout==0)
				for(f=1.0f; f>0.0f; f-=0.025f) {
					ta_begin_render();
					display_txr(f,tw,th,w,h,taddy);
					ta_commit_eol();
					img_blank();
					ta_commit_eol();  
					ta_finish_frame();
				}
		}
	}
}

static void introent_end(void *userData, const char *name)
{
}

void load_intro(char *filename) {
	int fd=0;
	int len=0;
	int done=1;
	uint8 *buf=NULL;

	XML_Parser p = XML_ParserCreate(NULL);
	if(!p) {
		printf("Couldn't allocate memory for parser\r\n");
		goto errout;
	}

	XML_SetElementHandler(p, introent_start, introent_end);

	fd=fs_open(filename, O_RDONLY);
	if(fd==0) { printf("load_intro: file not found\r\n"); goto errout; }
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


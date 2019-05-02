/* Swarm Screensaver for DreamInducer

   swarm.c      
   (c)2001 David Kuder

*/

#include <kos.h>
#include <kos/thread.h>
#include <dc/ta.h>
#include <string.h>
#include <time.h>
#include "list.h"
#include <stdlib.h>
#include "image.h"
#include "tmalloc.h"

time_t rtc_unix_secs();

char beetexsrc[]={
 0x0, 0x0, 0x2, 0x4, 0x4, 0x2, 0x0, 0x0,
 0x0, 0x4, 0x6, 0xf, 0xf, 0x6, 0x4, 0x0,
 0x2, 0x6, 0xf, 0x6, 0x6, 0xf, 0x6, 0x2,
 0x4, 0xf, 0x6, 0x1, 0x1, 0x6, 0xf, 0x4,
 0x4, 0xf, 0x6, 0x1, 0x1, 0x6, 0xf, 0x4,
 0x2, 0x6, 0xf, 0x6, 0x6, 0xf, 0x6, 0x2,
 0x0, 0x4, 0x6, 0xf, 0xf, 0x6, 0x4, 0x0,
 0x0, 0x0, 0x2, 0x4, 0x4, 0x2, 0x0, 0x0
};

uint32 nextregroup=0;

uint32 beetxr=0;
int BEES=96;
int WASPS=2;
#define BEESPREAD	768
#define BEESPEED	16
#define WASPSPEED	12

struct {
	/* Coords */
	float x;
	float y;

	/* Vectors */
	float vx;
	float vy;

	/* Current target */
	float tx; /* Target X for wasps */
	float ty; /* Target Y for wasps */

	/* Insect type, 0=Wasp, 1=Bee */
	uint8 it;

	uint8 hive;

	/* Age in frames, dies after 3200 */
	int age;
} insect[128];

enum {IT_WASP, IT_BEE};

void init_insect(int i);

float swarm_rand() { 
        return rand()/(1.0f*RAND_MAX);
}

void swarm_render(int mode, float x1, float y1, float x2, float y2, float z1,
		float a, float r, float g, float b) {
	poly_hdr_t      poly;
	vertex_ot_t vert;

	ta_poly_hdr_txr(&poly, mode, TA_ARGB4444, 8, 8, beetxr, 1);
	ta_commit_poly_hdr(&poly);

	vert.flags = TA_VERTEX_NORMAL;
	vert.a = a; vert.r = r; vert.g = g; vert.b = b;

	vert.x = x1;
	vert.y = y2;
	vert.u = 0;
	vert.v = 1;
	vert.z = z1;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.y = y1;
	vert.v = 0;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.x = x2;
	vert.y = y2;
	vert.u = 1;
	vert.v = 1;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.flags = TA_VERTEX_EOL;
	vert.y = y1;
	vert.v = 0;
	ta_commit_vertex(&vert, sizeof(vert));
}

void render_insect(int i) {
	register float a,r,g,b;

	if((insect[i].x-8.0f<0) ||
	   (insect[i].y-8.0f<0) ||
	   (insect[i].x+8.0f>640) ||
	   (insect[i].y+8.0f>480)) return;

	if(insect[i].it==IT_BEE) {
		register int age=insect[i].age;
		if(age>2400) {
			age-=2400;
			a=(800-age)/800.0f;
			r=(1.0f*insect[i].hive)/(1.0f*WASPS);
			g=0.0f;
			b=1.0f;
		} else if(age>800) {
			age-=800;
			a=age/1600.0f;
			r=(1.0f*insect[i].hive)/(1.0f*WASPS);
			g=0.75f-(a*0.75f);
			b=a*0.25f+0.75f;
			a=1.0f;
		} else {
			a=age/800.0f;
			r=(1.0f*insect[i].hive)/(1.0f*WASPS);
			g=0.75f;
			b=0.75f;
		}
	} else
		a=r=g=b=1.0f; 

	swarm_render(TA_TRANSLUCENT, insect[i].x-(8.0f*a), insect[i].y-(8.0f*a),
		insect[i].x+(8.0f*a), insect[i].y+(8.0f*a), 128.0f, a, r, g, b);
}

void calc_insect(int i) {
	register float tx, ty, ms;

	if((i < WASPS)&&(insect[i].it!=IT_WASP)) init_insect(i);
	if((i >= WASPS)&&(insect[i].it!=IT_BEE)) init_insect(i);

	if(i < WASPS) {
		ms=WASPSPEED;
		tx=insect[i].tx;
		ty=insect[i].ty;

		// If we get within 128 pixels, find a new target
		if( (((int)(insect[i].tx)>>6)==((int)(insect[i].x)>>6)) && 
		    (((int)(insect[i].ty)>>6)==((int)(insect[i].y)>>6)) ) {
			insect[i].tx=swarm_rand()*480+80;
			insect[i].ty=swarm_rand()*360+60;
		}
	} else {
		int v=(int)insect[i].tx;
		if(v>=WASPS) v=0;

		ms=BEESPEED;
		tx=insect[v].x+(swarm_rand()*BEESPREAD)-BEESPREAD/2;
		ty=insect[v].y+(swarm_rand()*BEESPREAD)-BEESPREAD/2;
		insect[i].age+=(((int)insect[v].x)&3)+(((int)insect[v].y)&3);
		if(insect[i].age>3200) init_insect(i);
	}

	if((tx<insect[i].x) && (insect[i].vx>(-ms)))
		insect[i].vx -= ms/40;
	if((ty<insect[i].y) && (insect[i].vy>(-ms)))
		insect[i].vy -= ms/40;

	if((tx>insect[i].x) && (insect[i].vx<ms))
		insect[i].vx += ms/40;
	if((ty>insect[i].y) && (insect[i].vy<ms))
		insect[i].vy += ms/40;

	if((tx<insect[i].x) && (insect[i].vx<0))
		insect[i].vx *= 0.95f;
	if((ty<insect[i].y) && (insect[i].vy<0))
		insect[i].vy *= 0.95f;

	if((tx>insect[i].x) && (insect[i].vx>0))
		insect[i].vx *= 0.95f;
	if((ty>insect[i].y) && (insect[i].vy>0))
		insect[i].vy *= 0.95f;


	insect[i].x+=insect[i].vx;
	insect[i].y+=insect[i].vy;
}

void init_insect(int i) {
	insect[i].x=swarm_rand()*(int)640;
	insect[i].y=swarm_rand()*(int)480;

	insect[i].vx=0.0f;
	insect[i].vy=0.0f;

	if(i < WASPS) {
		insect[i].it=IT_WASP;
		insect[i].tx=swarm_rand()*640;
		insect[i].ty=swarm_rand()*480;
	} else {
		insect[i].it=IT_BEE;
		insect[i].tx=swarm_rand()*WASPS;
		insect[i].hive=insect[i].tx;
		insect[i].age=0;
	}
	nextregroup=jiffies+(swarm_rand()*300);
}

void swarm_regroup() {
	register int i;
	for(i=0; i<BEES+WASPS; i++)
		if(insect[i].it==IT_BEE)
			insect[i].tx=swarm_rand()*WASPS;
	nextregroup=jiffies+(swarm_rand()*300);
}

void swarm_process() {
	int i;
	for(i=0; i<BEES+WASPS; i++)
		calc_insect(i);
	if(jiffies>nextregroup)
		swarm_regroup();
}

void swarm_paint() {
	int i;

	for(i=0; i<BEES+WASPS; i++) {
		render_insect(i);
	}
}

void swarm_init() {
	int i;

	BEES=(int)(swarm_rand() * 62.0f)+62;
	WASPS=(int)(swarm_rand() * 2.0f)+2;

	for(i=0; i<BEES+WASPS; i++)
		init_insect(i);

	for(i=WASPS; i<BEES+WASPS; i++)
		insect[i].age=(swarm_rand()*2560.0f);

	if(beetxr==0) {
		uint16 *beeptr;
		register int x,y,c;

		beetxr=tmalloc(128);
		beeptr=ta_txr_map(beetxr);

		for(x=0; x<8; x++)
			for(y=0; y<8; y++) {
				c = beetexsrc[y*8+x];
				beeptr[y*8+x]=(c<<12) | 0xfff;
			}
	}

}

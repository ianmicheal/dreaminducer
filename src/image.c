/* DreamInducer Menuing System

   image.c
   (c)2002 David Kuder
*/

#include <kos.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "image.h"
#include "tmalloc.h"

/* Load an image with the appropriate handler, chosen by file extension.
 */
InducerImage *libgail_load_image(const char *fn, uint8 format) {
	InducerImage *img;
	char *ext;

	ext=strrchr(fn,'.')+1;

	if(!strncasecmp(ext,"png",3)) {
		img = libgail_load_png(fn, format);
		if (img != NULL) {
			return img;
		}
	}

	printf("libgail_load_image: unknown image file %s\r\n", fn);

	return NULL;
}

/* Find a pixel with color [r,g,b] and change it's alpha value to [a]
 * This function is a real mess, but serves its purpose.
 */
void libgail_colorkey(InducerImage *img, uint8 a, uint8 r, uint8 g, uint8 b) {
	register uint16 width, height, color, mask, alpha, i;
	register uint16 *pixel_data=(uint16 *) img->data;

	width=img->width;
	height=img->height;

	switch(img->format) {
		case TA_ARGB4444:
			color = ((r >> 4) << 8) | ((g >> 4) << 4) |
					((b >> 4) << 0);
			alpha = ((a >> 4) << 12);
			mask = 0x0fff;
			break;
		case TA_ARGB1555:
			color = ((r >> 3) << 10) | ((g >> 3) << 5) |
					((b >> 3) << 0);
			alpha = ((a >> 7) << 15);
			mask = 0x7fff;
			break;
		default:
			return ; 
	}

	for(i = 0; i < height*width; i++) 
		if((pixel_data[i] & mask)==color)
			pixel_data[i]=(pixel_data[i] & mask) | alpha;

	return;
}

/* Load an image into texture ram and twiddle if asked.
 * NOTE: This function is capable of loading textures the graphics processor
 *	 might not be able to handle. (24/32 bit textures)
 */
uint32 libgail_image_to_txr(InducerImage *img, uint16 *twidth, uint16 *theight) {
	register uint16 tw, th, x, y;
	uint32 taddy;
	uint16 *texture;

	for(tw = 2; tw < img->width; tw <<= 1) {};
	for(th = 2; th < img->height; th <<= 1) {};
	*twidth=tw;
	*theight=th;

	taddy = tmalloc(tw*th*2);
	texture = ta_txr_map(taddy);

	for(y=0; y<th; y++) for(x=0; x<tw; x++)
		texture[y*tw+x]=img->data[y*img->width+x];

	return taddy;
}

void libgail_free_image(InducerImage *img) {
	if(!img) return;
	if(img->data) free(img->data);
	free(img);
}

void libgail_render_poly(float x1, float y1, float z1, float x2, float y2,
			float u1, float v1, float u2, float v2,
			float a, float r, float g, float b) {
	vertex_ot_t vert;

	vert.a = a; vert.r = r; vert.g = g; vert.b = b;
	vert.oa = vert.or = vert.ob = 0.0f;
	vert.flags = TA_VERTEX_NORMAL;
	vert.x = x1;
	vert.y = y1;
	vert.u = u1;
	vert.v = v1;
	vert.z = z1;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.a = a; vert.r = r; vert.g = g; vert.b = b;
	vert.oa = vert.or = vert.ob = 0.0f;
	vert.flags = TA_VERTEX_NORMAL;
	vert.x = x2;
	vert.y = y1;
	vert.u = u2;
	vert.v = v1;
	vert.z = z1;
	ta_commit_vertex(&vert, sizeof(vert));

	vert.a = a; vert.r = r; vert.g = g; vert.b = b;
	vert.oa = vert.or = vert.ob = 0.0f;
	vert.flags = TA_VERTEX_NORMAL;
	vert.x = x1;
	vert.y = y2;
	vert.u = u1;
	vert.v = v2;
	vert.z = z1;
	ta_commit_vertex(&vert, sizeof(vert));
	
	vert.a = a; vert.r = r; vert.g = g; vert.b = b;
	vert.oa = vert.or = vert.ob = 0.0f;
	vert.flags = TA_VERTEX_EOL;
	vert.x = x2;
	vert.y = y2;
	vert.u = u2;
	vert.v = v2;
	vert.z = z1;
	ta_commit_vertex(&vert, sizeof(vert));
}


/* DreamInducer Menuing System

   image.h
   (c)2002 David Kuder

   based on pcx.h
   (c)2000-2001 Dan Potter
*/

#ifndef __IMAGE_H
#define __IMAGE_H

#include <unistd.h>

typedef struct {
	int	width, height;
	uint16	*data;
	uint8	format;
} InducerImage;

void libgail_colorkey(InducerImage *img, uint8 a, uint8 r, uint8 g, uint8 b);

InducerImage *libgail_load_image(const char *fn, uint8 txrfmt);
InducerImage *libgail_load_png(const char *fn, uint8 txrfmt);

void libgail_free_image(InducerImage *img);

uint32 libgail_image_to_txr(InducerImage *img, uint16 *twidth, uint16 *theight);

#define libgail_send_header(format,tw,th,txraddy,filter) { \
	poly_hdr_t poly; \
\
	ta_poly_hdr_txr(&poly, TA_TRANSLUCENT, format, tw, th, txraddy, filter); \
	ta_commit_poly_hdr(&poly); \
}

void libgail_render_poly(float x1, float y1, float z1, float x2, float y2,
		float u1, float v1, float u2, float v2,
		float a, float r, float g, float b);

void libgail_dither_real(uint8 *datain, uint8 *dataout, int xs, int ys, int channels);
#define libgail_dither_bw(in, out, x, y) libgail_dither_real(in, out, x, y, 1)
#define libgail_dither_rgb(in, out, x, y) libgail_dither_real(in, out, x, y, 3)
#define libgail_dither_argb(in, out, x, y) libgail_dither_real(in, out, x, y, 4)

#endif /* __IMAGE_H */

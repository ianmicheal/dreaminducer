/* Grendel's Advanced Imaging Library

   loadpng.c
   (c)2002 David Kuder
*/

#include <kos.h>
#include <png/png.h>
#include <png/readpng.h>
#include "image.h"

InducerImage *libgail_load_png(const char *fn, uint8 format) {
	InducerImage    *img;

	FILE *infile;
	uint8 *buffer;
	uint32 row_stride;
	int channels;
	uint32 width, height, num_pixels;
	const double display_exponent = 1.0;

	if ((infile = fopen(fn, "r")) == 0) {
		printf("libgail_load_png: can't open %s\n", fn);
		return NULL;
	}

	if (readpng_init(infile, &width, &height))
		return NULL;

	buffer = readpng_get_image(display_exponent, &channels, &row_stride);

	img = (InducerImage*)malloc(sizeof(InducerImage));
	memset(img, 0, sizeof(InducerImage));

	img->width = width;
	img->height = height;
	num_pixels = width*height;
	img->data = (uint16 *)malloc(num_pixels * 2);
	img->format = format;

//	printf("File %s loaded into image %p, size %lix%li, with %i channels\r\n", fn, img, width, height, channels); 

	if (channels == 3) {
		register uint8 r,g,b;
		register uint16 x,y;
		for(y=0;y<height;y++) {
			for(x=0;x<width;x++) {
				r=buffer[(y*width+x)*3];
				g=buffer[(y*width+x)*3+1];
				b=buffer[(y*width+x)*3+2];
				switch(img->format) {
					case TA_ARGB4444:
						img->data[y*width+x]=(0xf<<12) | ((r>>4)<<8) | ((g>>4)<<4) | (b>>4);
						break;
					case TA_ARGB1555:
						img->data[y*width+x]=(1<<15) | ((r>>3)<<10) | ((g>>3)<<5) | (b>>3);
						break;
					case TA_RGB565:
						img->data[y*width+x]=((r>>3)<<11) | ((g>>2)<<5) | (b>>3);
						break;
				}
			}
		}
	} else if (channels == 4) {
		register uint8 a,r,g,b;
		register uint16 x,y;
		for(y=0;y<height;y++) {
			for(x=0;x<width;x++) {
				a=buffer[(y*width+x)*4+3];
				r=buffer[(y*width+x)*4];
				g=buffer[(y*width+x)*4+1];
				b=buffer[(y*width+x)*4+2];
				switch(img->format) {
					case TA_ARGB4444:
						img->data[y*width+x]=((a>>4)<<12) | ((r>>4)<<8) | ((g>>4)<<4) | (b>>4);
						break;
					case TA_ARGB1555:
						img->data[y*width+x]=((a>>7)<<15) | ((r>>3)<<10) | ((g>>3)<<5) | (b>>3);
						break;
					case TA_RGB565:
						img->data[y*width+x]=((r>>3)<<11) | ((g>>2)<<2) | (b>>3);
						break;
				}
			}
		}
	}

	readpng_cleanup(1);

	fclose(infile);

	return img;
}


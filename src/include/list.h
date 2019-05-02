/* DreamInducer Menuing System

   list.h
   (c)2002 David Kuder
*/

#ifndef __LIST_H
#define __LIST_H

#include <unistd.h>

void load_list(char *);
void scroll_up(int);
void scroll_down(int);
void do_command();
void scrn_render(float, float, float, float, float, float, float, float, float);
void list_render(float, float, float, float, float, float, float, float, float, int);
void desc_render(float, float, float, float, float, float, float, float, float, int);
void draw_string(int font, float x1, float x2, float y, float z, float a, float r, float g, float b, char *str);

uint32 fonttxr;
uint16 fonttw, fontth;

typedef struct {  
        int w,h;
} textbox_t;
textbox_t menu_list;
textbox_t menu_desc;

#endif	/* __LIST_H */

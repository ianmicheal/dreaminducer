#include <kos.h>
#include <dc/ta.h>
#include "theme.h"
#include "list.h"
#include "maple.h"
#include "swarm.h"
#include "tmalloc.h"

void load_intro(char *filename);

int screensaver=0;

void draw_blank(int mode) {
	poly_hdr_t poly;

	ta_poly_hdr_col(&poly, mode);
	ta_commit_poly_hdr(&poly);
}

int main(int argc, char **argv) {
	dbgio_disable();
	dbglog_set_level(DBG_DEAD);
	kos_init_all(ALL_ENABLE, NULL);

	/* We don't want any textures at 0x0 */
	ta_txr_allocate(128);

	/* We allocate 3.5mb for textures */
	txr_heap_init(ta_txr_allocate(3584*1024), 3584*1024);

	load_intro("/cd/intro.dxi");
	load_list("/cd/mainmenu.dxl");

	while(1) {
		ta_begin_render();

				draw_blank(TA_OPAQUE);
			ta_commit_eol();

				draw_blank(TA_TRANSLUCENT);

				if(screensaver)
					swarm_paint();
				if(!screensaver)
					theme_render();
			ta_commit_eol();

		ta_finish_frame();

		if(screensaver)
			swarm_process();

		do_maple();
	}

	kos_shutdown_all();
	return 0;
}


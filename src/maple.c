#include <kos.h>
#include <dc/maple.h>
#include <stdlib.h>
#include <kos/thread.h>
#include "list.h"
#include "swarm.h"

static int32 startheld=0;
static int32 now=0;

extern int screensaver;

void do_maple() {
	cont_cond_t cond;
	static uint8 mcont;

	now++;

	if(startheld==0) startheld=now;

	if(now-startheld>120*60) {
		if(screensaver==0) swarm_init();
		screensaver=1;
	} else {
		screensaver=0;
	}

	if (mcont<=0) {
		maple_rescan_bus(1);
		mcont = maple_first_controller();
		cont_get_cond(mcont, &cond);
		cont_get_cond(mcont, &cond);
	} else if (cont_get_cond(mcont, &cond) < 0) {
		maple_rescan_bus(1);
		mcont = maple_first_controller();
	}

	if (mcont && cond.joyy) {
		if ((cond.joyy < 112) && (now-startheld > 15)) {
			scroll_up(1);
			startheld=now;
		}
		else if ((cond.joyy > 144) && (now-startheld > 15)) {
			scroll_down(1);
			startheld=now;
		}
	}

	if (mcont && cond.ltrig) {
			scroll_up(menu_list.h);
			startheld=now;
	}

	if (mcont && cond.rtrig) {
			scroll_down(menu_list.h);
			startheld=now;
	}

	if ((mcont && !(cond.buttons & CONT_DPAD_UP)) && (now-startheld > 15)) {
		scroll_up(1);
		startheld=now;
	} else if ((mcont && !(cond.buttons & CONT_DPAD_DOWN)) && (now-startheld > 15)) {
		scroll_down(1);
		startheld=now;
	}

	if ((mcont && !(cond.buttons & CONT_A)) && (now-startheld > 15)) {
		do_command();
		startheld=now;
	}

	if ((mcont && !(cond.buttons & CONT_START)) && (now-startheld > 15)) {
		if(now-startheld > 15) exit(0);
		startheld=now;
	}

	if (mcont && !(cond.buttons & CONT_X) && !(cond.buttons & CONT_Y)) {
		startheld-=10;
	} else if (mcont && !(cond.buttons & CONT_Y)) {
		startheld=now-10;
	}
}

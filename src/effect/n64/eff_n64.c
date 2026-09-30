#include "eff/eff_n64.h"

#include <string.h>

//.sbss
static eff_n64_work work;

//.sdata
static eff_n64_work* wp = &work;

void effInit64(void) {
	memset(wp, 0, sizeof(eff_n64_work));
	wp->effTextureLoaded = FALSE;
}

void tri1(u16 x, u16 y, u16 z) {
	GXWGFifo.u16 = x;
	GXWGFifo.u16 = x;
	GXWGFifo.u16 = x;
	GXWGFifo.u16 = y;
	GXWGFifo.u16 = y;
	GXWGFifo.u16 = y;
	GXWGFifo.u16 = z;
	GXWGFifo.u16 = z;
	GXWGFifo.u16 = z;
}

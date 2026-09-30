#include "eff/eff_n64.h"

#include "manager/dvdmgr.h"
#include <string.h>

//.sbss
static eff_n64_work work;

//.sdata
static eff_n64_work* wp = &work;

static void _callback_tpl(s32 error, DVDFileInfo* info) {
	DVDEntry* entry = info->cb.userData;

	UnpackTexPalette(wp->effTexture);
	DVDMgrClose(entry);
	wp->effTextureLoaded = TRUE;
}

void effInit64(void) {
	memset(wp, 0, sizeof(eff_n64_work));
	wp->effTextureLoaded = FALSE;
}

u32 effTblRandN64(u32 rand, s32 index) {
	s32 rand2 = _rand_table[index % 128];

	if (rand2 < 0) {
		rand2 = -rand2;
	}
	return rand2 % (s32)(rand + 1);
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

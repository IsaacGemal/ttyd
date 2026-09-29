#include "eff/eff_n64.h"

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

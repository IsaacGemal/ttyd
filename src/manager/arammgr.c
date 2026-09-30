#include "manager/arammgr.h"
#include <dolphin/os/OSCache.h>

//.bss
static ARAMWork work;

//.sdata
static ARAMWork* wp;

// NONMATCHING (1.96%): empty placeholder, not decompiled yet.
void aramMgrInit(void) {

}

void aramCallbackGarbage(u32 request) {
	// wp->field_0x5C is an unnamed tail: word flag at 0x5C, buffer at 0x60, size at 0x64.
	*(s32*)&wp->field_0x5C[0] = 0;
	DCInvalidateRange(*(void**)&wp->field_0x5C[4], *(u32*)&wp->field_0x5C[8]);
	ICInvalidateRange(*(void**)&wp->field_0x5C[4], *(u32*)&wp->field_0x5C[8]);
}

void arqCallbackToAram(u32 request) {
	ARAMWorkNode* work = wp->nodes;

	while (work != NULL) {
		if (request == (u32)work + 4) {
			break;
		}
		work = work->next;
	}
	work->flags |= 4;
}

void* aramMgrToAram(void* data, u32 size) {
	void* work = aramMgrToAramAsync(data, size);

	while (!(*(volatile u16*)work & 4)) {
	}
	return work;
}

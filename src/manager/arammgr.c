#include "manager/arammgr.h"

//.bss
static ARAMWork work;

//.sdata
static ARAMWork* wp;

// NONMATCHING (1.96%): empty placeholder, not decompiled yet.
void aramMgrInit(void) {

}

void arqCallbackToAram(u32 request) {
	ARAMWorkNode* work = ((ARAMWorkNode*)wp)->next;

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

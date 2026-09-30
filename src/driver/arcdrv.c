#include "driver/arcdrv.h"
#include <string.h>

//.bss
static ArchiveEntry work[4];




//void* arcOpen(char* filename, )


//BOOL arcDataCheck()

void arcInit(void) {
	int i;

	for (i = 0; i < 4; i++) {
		memset(&work[i], 0, sizeof(ArchiveEntry));
	}
}

BOOL arcDataCheck(void* data) {
	int i;

	for (i = 0; i < 4; i++) {
		if ((u32)work[i].data <= (u32)data && (u32)work[i].data + work[i].size >= (u32)data) {
			return TRUE;
		}
	}
	return FALSE;
}

void arcDelete(s32 id) {
	memset(&work[id], 0, sizeof(ArchiveEntry));
}


// NONMATCHING (4.76%): empty placeholder, not decompiled yet.
void arcEntry(s32 id, void* data, u32 size) {

}

void* arcOpen(const char* filename, void** addr, u32* length) {
	ARCFileInfo info;
#ifdef MUST_MATCH
	// MUST_MATCH: without a named pointer MWCC rematerializes &info at each call instead of
	// caching it in one register (the target computes it once before the loop).
	ARCFileInfo* infoPtr = &info;
#endif
	int i;

	for (i = 0; i < 4; i++) {
#ifdef MUST_MATCH
		if ((work[i].flags & 1) && ARCOpen(&work[i].handle, filename, infoPtr)) {
			void* startAddr = ARCGetStartAddrInMem(infoPtr);
#else
		if ((work[i].flags & 1) && ARCOpen(&work[i].handle, filename, &info)) {
			void* startAddr = ARCGetStartAddrInMem(&info);
#endif
			if (addr) {
				*addr = startAddr;
			}
			if (length) {
#ifdef MUST_MATCH
				*length = ARCGetLength(infoPtr);
#else
				*length = ARCGetLength(&info);
#endif
			}
#ifdef MUST_MATCH
			ARCClose(infoPtr);
#else
			ARCClose(&info);
#endif
			return startAddr;
		}
	}
	if (addr) {
		*addr = NULL;
	}
	if (length) {
		*length = 0;
	}
	return NULL;
}

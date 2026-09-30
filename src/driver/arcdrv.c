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


// NONMATCHING (87.13%): 3 instructions differ, 3 missing, 1 extra, plus register differences.
void* arcOpen(const char* filename, void** addr, u32* length) {
	ARCFileInfo info;
	void* startAddr;
	int i;

	for (i = 0; i < 4; i++) {
		if ((work[i].flags & 1) && ARCOpen(&work[i].handle, filename, &info)) {
			startAddr = ARCGetStartAddrInMem(&info);
			if (addr) {
				*addr = startAddr;
			}
			if (length) {
				*length = ARCGetLength(&info);
			}
			ARCClose(&info);
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

// NONMATCHING (4.76%): empty placeholder, not decompiled yet.
void arcEntry(s32 id, void* data, u32 size) {

}

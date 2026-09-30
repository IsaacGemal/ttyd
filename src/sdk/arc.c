#include "sdk/arc.h"
#include <dolphin/os.h>
#include <ctype.h>

typedef struct FSTEntry {
	u32 entryInfo; //top byte is 0 file 1 dir, u24 string offset
	u32 entryOffsetOrParent; //file = offset, dir = parent index
	u32 entrySizeOrNext; //file = size, dir = next entry
} FSTEntry;

#define entryIsDir(h, i) (((((FSTEntry*)(h)->FSTStart)[i].entryInfo & 0xff000000) == 0) ? FALSE : TRUE)
#define stringOff(h, i) (((FSTEntry*)(h)->FSTStart)[i].entryInfo & ~0xff000000)
#define parentDir(h, i) (((FSTEntry*)(h)->FSTStart)[i].entryOffsetOrParent)
#define nextDir(h, i) (((FSTEntry*)(h)->FSTStart)[i].entrySizeOrNext)
#define filePosition(h, i) (((FSTEntry*)(h)->FSTStart)[i].entryOffsetOrParent)
#define fileLength(h, i) (((FSTEntry*)(h)->FSTStart)[i].entrySizeOrNext)

static u32 entryToPath(ARCHandle* handle, u32 entry, char* path, u32 maxlen);

BOOL ARCInitHandle(void* arcStart, ARCHandle* handle) {
	FSTEntry* entry;
	ARCHeader* header;

	header = (ARCHeader*)arcStart;
	if (header->magic != 0x55AA382D) {
		OSHalt("ARCInitHandle: bad archive format");
	}
	handle->archiveStartAddr = arcStart;
	entry = (FSTEntry*)((u32)arcStart + header->fstStart);
	handle->FSTStart = (void*)entry; //relative->absolute
	handle->fileStart = (void*)((u32)arcStart + header->fileStart); //relative->absolute
	handle->entryNum = entry->entrySizeOrNext; //entry 0 size is entry count
	handle->FSTStringStart = (char*)&entry[handle->entryNum];
	handle->FSTLength = header->fstSize;
	handle->currDir = 0;
	return TRUE;
}

// NONMATCHING (97.62%): the original tests the entry number with one compare-and-branch pair less.
BOOL ARCOpen(ARCHandle* handle, const char* filename, ARCFileInfo* info) {
	FSTEntry* fst = (FSTEntry*)handle->FSTStart;
	s32 entry;
	char currentDir[128];

	entry = ARCConvertPathToEntrynum(handle, filename);
	if (0 > entry) {
		ARCGetCurrentDir(handle, currentDir, 128);
		return FALSE;
	}
	if (((fst[entry].entryInfo & 0xff000000) == 0) ? FALSE : TRUE) {
		return FALSE;
	}
	info->handle = handle;
	info->startOffset = fst[entry].entryOffsetOrParent;
	info->length = fst[entry].entrySizeOrNext;
	return TRUE;
}

static BOOL isSame(const signed char* path, const signed char* string) {
	while (*string != '\0') {
		if (tolower(*path++) != tolower(*string++)) {
			return FALSE;
		}
	}
	if (*path == '/' || *path == '\0') {
		return TRUE;
	}
	return FALSE;
}

// NONMATCHING (72.65%): the original keeps far fewer values live (three saved registers, isSame and the
// directory scan inlined tighter); ours saves r21-r31.
s32 ARCConvertPathToEntrynum(ARCHandle* handle, const char* path) {
	const signed char* pathPtr = (const signed char*)path;
	const signed char* ptr;
	signed char* stringPtr;
	BOOL isDir;
	u32 length;
	u32 dirLookAt;
	u32 i;
	FSTEntry* fst = (FSTEntry*)handle->FSTStart;

	dirLookAt = handle->currDir;
	while (1) {
		if (*pathPtr == '\0') {
			return (s32)dirLookAt;
		} else if (*pathPtr == '/') {
			dirLookAt = 0;
			pathPtr++;
			continue;
		} else if (*pathPtr == '.') {
			if (*(pathPtr + 1) == '.') {
				if (*(pathPtr + 2) == '/') {
					dirLookAt = fst[dirLookAt].entryOffsetOrParent;
					pathPtr += 3;
					continue;
				} else if (*(pathPtr + 2) == '\0') {
					return (s32)fst[dirLookAt].entryOffsetOrParent;
				}
			} else if (*(pathPtr + 1) == '/') {
				pathPtr += 2;
				continue;
			} else if (*(pathPtr + 1) == '\0') {
				return (s32)dirLookAt;
			}
		}

		for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++)
			;

		isDir = (*ptr == '\0') ? FALSE : TRUE;
		length = (u32)(ptr - pathPtr);

		ptr = pathPtr;

		for (i = dirLookAt + 1; i < fst[dirLookAt].entrySizeOrNext;
		     i = ((fst[i].entryInfo & 0xff000000) == 0 ? FALSE : TRUE) ? fst[i].entrySizeOrNext : (i + 1)) {
			if ((((fst[i].entryInfo & 0xff000000) == 0 ? FALSE : TRUE) == FALSE) && (isDir == TRUE)) {
				continue;
			}

			stringPtr = (signed char*)handle->FSTStringStart + (fst[i].entryInfo & ~0xff000000);

			if (isSame(ptr, stringPtr) == TRUE) {
				goto next_hier;
			}
		}

		return -1;

	next_hier:
		if (!isDir) {
			return (s32)i;
		}

		dirLookAt = i;
		pathPtr += length + 1;
	}
}

u32 myStrncpy(char* dest, signed char* src, u32 maxlen) {
	u32 i = maxlen;

	while ((i > 0) && (*src != 0)) {
		*dest++ = *src++;
		i--;
	}

	return (maxlen - i);
}

static u32 entryToPath(ARCHandle* handle, u32 entry, char* path, u32 maxlen) {
	FSTEntry* fst = (FSTEntry*)handle->FSTStart;
	char* name;
	u32 loc;

	if (entry == 0) {
		return 0;
	}

	name = handle->FSTStringStart + (fst[entry].entryInfo & ~0xff000000);

	loc = entryToPath(handle, fst[entry].entryOffsetOrParent, path, maxlen);

	if (loc == maxlen) {
		return loc;
	}

	*(path + loc++) = '/';

	loc += myStrncpy(path + loc, (signed char*)name, maxlen - loc);

	return loc;
}

BOOL ARCConvertEntrynumToPath(ARCHandle* handle, s32 entrynum, char* path, u32 maxlen) {
	FSTEntry* fst = (FSTEntry*)handle->FSTStart;
	u32 loc;

	loc = entryToPath(handle, (u32)entrynum, path, maxlen);

	if (loc == maxlen) {
		path[maxlen - 1] = '\0';
		return FALSE;
	}

	if (((fst[entrynum].entryInfo & 0xff000000) == 0) ? FALSE : TRUE) {
		if (loc == maxlen - 1) {
			path[loc] = '\0';
			return FALSE;
		}

		path[loc++] = '/';
	}

	path[loc] = '\0';
	return TRUE;
}

BOOL ARCGetCurrentDir(ARCHandle* handle, char* path, u32 maxlen) {
	return ARCConvertEntrynumToPath(handle, (s32)handle->currDir, path, maxlen);
}

void* ARCGetStartAddrInMem(ARCFileInfo* info) {
	return (void*)((u32)info->handle->archiveStartAddr + info->startOffset);
}

u32 ARCGetLength(ARCFileInfo* info) {
	return info->length;
}

BOOL ARCClose(ARCFileInfo* info) {
	return TRUE;
}

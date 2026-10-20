#pragma once

#include <dolphin/gx.h>
#include "manager/filemgr.h"

typedef enum HEAP_TYPE {
	HEAP_DEFAULT = 0,
	HEAP_MALLOC = 1, //for memInit/Clear to do its allocation
	HEAP_EXT = 2, //TODO: proper name for EXT
	HEAP_EFFECT = 3,
	HEAP_BATTLE = 4,
	HEAP_UNK5 = 5
} HEAP_TYPE;

typedef struct smartEntry {
	void* address; //0x0
	u32 size; //0x4
	FileEntry* file; //0x8
	u16 field_0xC; //0xC
	u8 type; //0xE, total guess, no way to know
	u8 field_0xF; //0xF
	u32 freeAfter; //0x10, free bytes between this entry and the next one in the heap
	struct smartEntry* next; //0x14
	struct smartEntry* prev; //0x18
} smartEntry;

typedef struct SmartWork {
	void* ptr; //0x0
	smartEntry work[0x800]; //0x4-0xE004
	u32 bytesLeft; //0xE004
	smartEntry* usedHead; //0xE008
	smartEntry* usedTail; //0xE00C
	smartEntry* head; //0xE010
	smartEntry* tail; //0xE014
	u32 waitsync; //0xE018
} SmartWork;

void memInit(void);
void memClear(HEAP_TYPE heap);
void* __memAlloc(HEAP_TYPE heap, u32 size);
void __memFree(HEAP_TYPE heap, void* ptr);
void* _mapAlloc(u32 size);
void* _mapAllocTail(u32 size);
void _mapFree(void* alloc);
void smartInit(void);
void L_smartReInit(void);
void smartAutoFree(u32 a1);
void smartFree(smartEntry* entry);
smartEntry* smartAlloc(u32 size, u8 type);
GXTexObj* smartTexObj(GXTexObj* obj, void** image_ptr);
void smartGarbage(void);

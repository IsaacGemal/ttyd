#include "memory.h"
#include "manager/dvdmgr.h"
#include "system.h"
#include <dolphin/os.h>
#include <string.h>

//HEAP_MALLOC block is a series of these nodes
typedef struct MapAllocEntry MapAllocEntry;
struct MapAllocEntry {
	MapAllocEntry* next; //0x0
	u32 size; //0x4
	u16 inuse; //0x8
	u16 currId; //0xA
	u8 pad_0xC[0x20 - 0xC]; //0xC
};

//.bss
SmartWork smartWork;

//.sdata
//flag, size / 0x400 (>> 10)
s32 size_table[6][2] = {
	{1, 0x1C84}, //0x721000, 7300 KiB/7.13 MiB
	{1, 0xD90}, //0x364000, 3472 KiB/3.39 MiB
	{1, 0xE0}, //0x38000, 224 KiB
	{1, 0x80}, //0x20000, 128 KiB
	{1, 0x100}, //0x40000, 256 KiB
	{0, 0x64}
};
static SmartWork* wp = &smartWork;

// the 4 bytes after wp in the target's .sdata (gap_07_8040F50C_sdata)
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") static u32 gap_after_wp = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

//.sbss
// declared last to first: the compiler lays the uninitialised globals out in reverse
OSHeapHandle heapHandle[6];
void* heapStart[6];
void* heapEnd[6];
void* mapalloc_base_ptr;
u32 mapalloc_size;
BOOL g_bFirstSmartAlloc;
u16 count;

// Map-allocator bookkeeping for the malloc heap, kept in an inlined helper: with the
// helper's `entry` declared before `size` MWCC colors them r23/r22, as the original does.
static inline void mapInitEntry(void) {
    MapAllocEntry* entry;
    u32 size = ((u32)heapEnd[1] - (u32)heapStart[1]) - 0x20;
    entry = OSAllocFromHeap(heapHandle[1], size);
    if (entry) {
        memset(entry, 0, size);
        DCFlushRange(entry, size);
    }
    entry->next = 0;
    entry->size = size - 0x20;
    entry->inuse = 0;
    mapalloc_base_ptr = entry;
    mapalloc_size = size;
    count = 0;
}

void memInit(void) {
    void* arenaLo;
    void* arenaHi;
    u32 temp_r31;
    int i;
    int size;
    u32 temp;

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    arenaLo = OSInitAlloc(arenaLo, arenaHi, 6);
    OSSetArenaLo(arenaLo);
    arenaLo = (void*)OSRoundUp32B(arenaLo);
    arenaHi = (void*)OSRoundDown32B(arenaHi);
    temp = (u32)arenaLo;

    for (i = 0; i < 6; i++) {
        if (size_table[i][0] == 1) {
            heapStart[i] = (void*)temp;
            size = size_table[i][1] << 10;
            heapEnd[i] = (void*)(temp + size);
            temp += size;
        }
    }

    temp_r31 = (u32)arenaHi - temp;
    for (i = 0; i < 6; i++) {
        if (size_table[i][0] == 0) {
            size = (size_table[i][1] * (u64)temp_r31) / 100Ull;
            heapStart[i] = (void*)temp;
            size -= (size & 0x1F);
            heapEnd[i] = (void*)(temp + size);
            temp += size;
        }
    }
    for (i = 0; i < 6; i++) {
        heapHandle[i] = OSCreateHeap(heapStart[i], heapEnd[i]);
    }
    OSSetArenaLo((void* ) arenaHi);
    for (i = 0; i < 6; i++) {
        OSDestroyHeap(heapHandle[i]);
        OSCreateHeap(heapStart[i], heapEnd[i]);
        if (i == 1) {
            mapInitEntry();
        }
    }
}

void memClear(HEAP_TYPE heap) {
	OSDestroyHeap(heapHandle[heap]);
	OSCreateHeap(heapStart[heap], heapEnd[heap]);
	if (heap == HEAP_MALLOC) {
		mapInitEntry();
	}
}

void* __memAlloc(HEAP_TYPE heap, u32 size) {
	void* ptr;

	ptr = OSAllocFromHeap(heapHandle[heap], size);
	if (ptr) {
		memset(ptr, 0, size);
		DCFlushRange(ptr, size);
	}
	return ptr;
}

void __memFree(HEAP_TYPE heap, void* ptr) {
	OSFreeToHeap(heapHandle[heap], ptr);
}

void* _mapAlloc(u32 size) {
	MapAllocEntry* nextptr; //shorthand to make sure regalloc is correct
	MapAllocEntry* entry; //for our loop
	MapAllocEntry* tailEntry; //make sure we don't lose when inserting, might need better name
	MapAllocEntry* currEntry; //one we're allocating
	void* usableData; //memory caller can actually use
	u32 usableSize; //size minus 0x20 header
	u32 maxSize; //used to update latest tail
	u32 fullSize; //size including header

	usableSize = OSRoundUp32B(size);
	if (!usableSize) {
		return NULL;
	}
	entry = mapalloc_base_ptr;
	fullSize = usableSize + 0x20;
	maxSize = 0;
	tailEntry = 0;
	while (1) {
		if (!entry->inuse && entry->size >= fullSize) {
			if (maxSize > entry->size || !maxSize) {
				tailEntry = entry->next;
				currEntry = entry;
				maxSize = entry->size;
			}
		}
		if (!entry->next) break;
		entry = entry->next;
	}
	if (maxSize) {
		usableData = (void*)((u32)currEntry + 0x20);

		currEntry->next = (void*)((u32)currEntry + fullSize);
		currEntry->size = usableSize;
		currEntry->inuse = 1;
		currEntry->currId = count++;

		nextptr = currEntry->next;
		nextptr->next = tailEntry;
		nextptr->size = maxSize - fullSize;
		nextptr->inuse = 0;
		nextptr->currId = 0;

		memset(usableData, 0, usableSize);
		DCFlushRange(usableData, usableSize);
		return usableData;
	}
	else {
		return NULL;
	}
}

void* _mapAllocTail(u32 size) {
	MapAllocEntry* nextptr; //shorthand to make sure regalloc is correct
	MapAllocEntry* entry; //for our loop
	MapAllocEntry* tailEntry; //make sure we don't lose when inserting, might need better name
	MapAllocEntry* currEntry; //one we're allocating
	//void* usableData; //memory caller can actually use
	u32 maxSize; //used to update latest tail
	u32 fullSize; //size including header
	u32 usableSize; //size minus 0x20 header

	usableSize = OSRoundUp32B(size);
	if (!usableSize) {
		return NULL;
	}
	entry = mapalloc_base_ptr;
	fullSize = usableSize + 0x20;
	maxSize = 0;
	tailEntry = 0;
	while (1) {
		if (!entry->inuse && entry->size >= fullSize) {
			tailEntry = entry->next;
			currEntry = entry;
			maxSize = entry->size;
		}
		if (!entry->next) break;
		entry = entry->next;
	}
	if (maxSize) {
		memset(currEntry, 0, fullSize);
		DCFlushRange(currEntry, fullSize);

		currEntry->next = (MapAllocEntry*)(((u32)currEntry + maxSize) - fullSize);
		currEntry->size = maxSize - fullSize;
		currEntry->inuse = 0;
		currEntry->currId = 0;

		nextptr = currEntry->next;
		nextptr->next = tailEntry;
		tailEntry = (void*)((u32)nextptr + 0x20);
		nextptr->size = usableSize;
		nextptr->inuse = 1;
		nextptr->currId = count++;

		memset(tailEntry, 0, usableSize);
		DCFlushRange(tailEntry, usableSize);
		return tailEntry;
	}
	else {
		return NULL;
	}
}

void _mapFree(void* alloc) {
	MapAllocEntry* loopEntry;
	MapAllocEntry* nextEntry;
	u32 newSize;
	MapAllocEntry* entry;

	if (!alloc) return;
	entry = (MapAllocEntry*)((u32)alloc - 0x20);
	if (!entry->inuse) return;
	nextEntry = entry->next;
	newSize = entry->size;
	if (nextEntry && !nextEntry->inuse) {
		newSize += nextEntry->size + 0x20;
		nextEntry = nextEntry->next;
	}
	loopEntry = mapalloc_base_ptr;
	while (1) {
		if (loopEntry->next == entry) {
			if (!loopEntry->inuse) {
				newSize += loopEntry->size + 0x20;
				entry = loopEntry;
			}
			break;
		}
		if ((u32)loopEntry > (u32)entry || !loopEntry->next) break;
		loopEntry = loopEntry->next;
	}
	entry->next = nextEntry;
	entry->size = newSize;
	entry->inuse = 0;
}

// NONMATCHING (75.04%): 13 instructions differ, 24 missing, 75 extra, plus register differences.
void smartInit(void) {
	u32 size;
	void* ptr;
	smartEntry* entry;
	int i;

	size = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	ptr = OSAllocFromHeap(heapHandle[5], size);
	if (ptr) {
		memset(ptr, 0, size);
		DCFlushRange(ptr, size);
	}
	wp->ptr = ptr;
	wp->usedHead = 0;
	wp->usedTail = 0;
	wp->bytesLeft = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	memset(wp->ptr, 0, (u32)heapEnd[5] - (u32)heapStart[5] - 32);
	memset(wp->work, 0, sizeof(wp->work));
	entry = wp->work;
	for (i = 0x800; i != 0; i--) {
		entry->next = entry + 1;
		entry->prev = entry - 1;
		entry++;
	}
	wp->head = &wp->work[0];
	wp->head->prev = NULL;
	wp->tail = &wp->work[0x7FF];
	wp->tail->next = NULL;
	wp->waitsync = 0;
	g_bFirstSmartAlloc = FALSE;
}

// NONMATCHING (59.39%): 1 instruction differs, 9 missing, 24 extra, plus register differences.
void L_smartReInit(void) {
	smartEntry* entry;
	int i;

	wp->usedHead = 0;
	wp->usedTail = 0;
	wp->bytesLeft = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	memset(wp->ptr, 0, (u32)heapEnd[5] - (u32)heapStart[5] - 32);
	memset(wp->work, 0, sizeof(wp->work));
	entry = wp->work;
	for (i = 0x800; i != 0; i--) {
		entry->next = entry + 1;
		entry->prev = entry - 1;
		entry++;
	}
	wp->head = &wp->work[0];
	wp->head->prev = NULL;
	wp->tail = &wp->work[0x7FF];
	wp->tail->next = NULL;
	g_bFirstSmartAlloc = 0;
}



// dont_inline keeps the smartAutoFree/smartGarbage calls in smartAlloc as real `bl`s.
#pragma dont_inline on
void smartAutoFree(u32 a1) {
	smartEntry* entry;
	smartEntry* next;
	u16 type;

	type = a1;
	entry = wp->usedHead;
	while (entry) {
		next = entry->next;
		if (entry->type == type) {
			smartFree(entry);
		}
		entry = next;
	}
	if ((s32)a1 == 3) {
		entry = wp->usedHead;
		while (entry) {
			next = entry->next;
			if ((s32)entry->type == 4) {
				smartFree(entry);
			}
			entry = next;
		}
	}
}
#pragma dont_inline reset

void smartFree(smartEntry* entry) {
	smartEntry* prev;
	s32 size;

	if (entry->type == 4) {
		entry->type = 3;
		return;
	}
	if (!entry->prev) {
		wp->usedHead = entry->next;
		if (wp->usedHead) {
			wp->usedHead->prev = NULL;
		}
	}
	else {
		entry->prev->next = entry->next;
	}
	if (!entry->next) {
		wp->usedTail = entry->prev;
		if (wp->usedTail) {
			wp->usedTail->next = NULL;
		}
	}
	else {
		entry->next->prev = entry->prev;
	}
	prev = entry->prev;
	size = entry->size + entry->freeAfter;
	if (prev) {
		prev->freeAfter += size;
	}
	else {
		wp->bytesLeft += size;
	}
	if (!wp->head) {
		wp->head = entry;
		entry->prev = NULL;
	}
	else {
		wp->tail->next = entry;
		entry->prev = wp->tail;
	}
	entry->field_0xC = 0;
	entry->next = NULL;
	entry->size = 0;
	entry->freeAfter = 0;
	entry->file = 0;
	wp->tail = entry;
	wp->waitsync++;
}

// 100%: the heap-tail expression needs explicit grouping for mwcc's reassociation.
smartEntry* smartAlloc(u32 size, u8 type) {
	smartEntry* entry1, * entry2;
	u32 size2;
	int i;

	size2 = size;
#ifdef MUST_MATCH
	if (g_bFirstSmartAlloc == 0) { // `!g_bFirstSmartAlloc` scores 97.47% instead of 97.76%
#else
	if (!g_bFirstSmartAlloc) {
#endif
		g_bFirstSmartAlloc = TRUE;
		smartAutoFree(3);
	}

#ifdef MUST_MATCH
	if ((s32)wp->waitsync != 0) {
#else
	if (wp->waitsync) {
#endif
		sysWaitDrawSync();
		wp->waitsync = 0;
	}

	entry2 = wp->head;

	if (!entry2->prev) {
		wp->head = entry2->next;
		if (wp->head) {
			wp->head->prev = NULL;
		}
	}
	else {
		entry2->prev->next = entry2->next;
	}

	if (!entry2->next) {
		wp->tail = entry2->prev;
		if (wp->tail) {
			wp->tail->next = NULL;
		}
	}
	else {
		entry2->next->prev = entry2->prev;
	}

	if (size2 & 31) {
		size2 += 32 - (size2 & 31);
	}

	entry2->field_0xC = 1;
	entry2->type = type;
	entry2->size = size2;
	entry2->file = 0;
	if (wp->bytesLeft >= size2) {
		entry2->address = wp->ptr;
		entry2->freeAfter = wp->bytesLeft - size2;
		entry2->next = wp->usedHead;
		entry2->prev = NULL;
		wp->bytesLeft = 0;
		if (wp->usedHead) {
			wp->usedHead->prev = entry2;
		}
		wp->usedHead = entry2;
		if (!entry2->next) {
#ifdef MUST_MATCH
			// Grouping that makes mwcc pool heapEnd-heapStart before adding wp->ptr (-32 first).
			entry2->freeAfter = ((u32)heapEnd[5] - (u32)heapStart[5]) - 32 + (u32)wp->ptr - (u32)entry2->address - entry2->size;
#else
			entry2->freeAfter = (u32)wp->ptr + (u32)heapEnd[5] - (u32)heapStart[5] - 32 - (u32)entry2->address - entry2->size;
#endif
			wp->usedTail = entry2;
		}
		return entry2;
	}
	else {
		for (entry1 = wp->usedHead; entry1; entry1 = entry1->next) {
			if (entry1->freeAfter >= size2) {
				entry2->address = (void*)((u32)entry1->address + entry1->size);
				entry2->freeAfter = entry1->freeAfter - size2;
				entry2->next = entry1->next;
				entry2->prev = entry1;
				entry1->freeAfter = 0;
				if (entry1->next) {
					entry1->next->prev = entry2;
				}
				else {
					wp->usedTail = entry2;
				}
				entry1->next = entry2;
				return entry2;
			}
		}
		for (i = 0; i < 3; i++) {
			switch (i) {
				case 1:
					_fileGarbage(1);
					break;
				case 2:
					_fileGarbage(0);
					break;
#ifdef MUST_MATCH
				// redundant with default; makes mwcc pick 1 as the search-tree pivot.
				case 0:
#endif
				default: break;
			}
			smartGarbage();
			entry1 = wp->usedTail;
			if (entry1->freeAfter >= size2) {
				entry2->address = (void*)((u32)entry1->address + entry1->size);
				entry2->freeAfter = entry1->freeAfter - size2;
				entry2->next = entry1->next;
				entry2->prev = entry1;
				entry1->freeAfter = 0;
				if (entry1->next) {
					entry1->next->prev = entry2;
				}
				else {
					wp->usedTail = entry2;
				}
				entry1->next = entry2;
				return entry2;
			}
		}
		return NULL;
	}
}

void smartGarbage(void) {
	void* src;
	void* space;
	smartEntry* cur;
	smartEntry* prev;

	sysWaitDrawSync();
	if (wp->usedHead != 0) {
		wp->bytesLeft = 0;
	}
	else {
		wp->bytesLeft = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	}
	prev = NULL;
	space = wp->ptr;
	for (cur = wp->usedHead; cur; prev = cur, cur = cur->next) {
		src = cur->address;
		if (src != space) {
			if (cur->file) {
				if (cur->file->state == 3 && cur->file->entry != 0
				    && OSGetCurrentThread() != &dvdmgr_thread) {
					if (prev) {
						prev->freeAfter = (u32)src - (u32)space;
					}
					else {
						wp->bytesLeft = (u32)src - (u32)space;
					}
					space = (void*)((u32)cur->address + cur->size);
					cur->freeAfter = 0;
					continue;
				}
				fileGarbageMoveMem(space, cur->file);
			}
			else {
				memmove(space, src, cur->size);
				cur->address = space;
			}
		}
		cur->freeAfter = 0;
		space = (void*)((u32)cur->address + cur->size);
	}
	if (wp->usedTail) {
		wp->usedTail->freeAfter = (u32)wp->ptr + ((u32)heapEnd[5] - (u32)heapStart[5] - 32) - (u32)wp->usedTail->address
		                              - wp->usedTail->size;
	}
	DCFlushRange(wp->ptr, (u32)heapEnd[5] - (u32)heapStart[5] - 32);
}

GXTexObj* smartTexObj(GXTexObj* obj, void** image_ptr) {
	if (image_ptr) {
		GXInitTexObjData(obj, *image_ptr);
	}
	return obj;
}

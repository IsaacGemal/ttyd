#include "manager/arammgr.h"
#include <dolphin/os/OSCache.h>
#include <dolphin/ar.h>
#include <dolphin/arq.h>
#include "memory.h"
#include <string.h>

// The owner value every ARQPostRequest of this manager passes.
#define ARAM_OWNER 0x19751128

//.bss
static ARAMWork work;

//.sdata
static ARAMWork* wp = &work;

void aramMgrInit(void) {
	memset(wp, 0, sizeof(ARAMWork));
	if (!ARCheckInit()) {
		ARInit(wp->stackIndex, 8);
	}
	ARQInit();
	wp->aramSize = 0x3FC000;
	wp->nodeCount = 0x40;
	wp->aramStart = ARAlloc(wp->aramSize);
	wp->aramCursor = wp->aramStart;
	wp->nodeArray = __memAlloc(HEAP_DEFAULT, wp->nodeCount * sizeof(ARAMWorkNode));
	memset(wp->nodeArray, 0, wp->nodeCount * sizeof(ARAMWorkNode));
	wp->nodes = NULL;
	wp->tail = NULL;
}

void aramCallbackGarbage(u32 request) {
	wp->garbageBusy = 0;
	DCInvalidateRange(wp->garbageBuf, wp->garbageChunk);
	ICInvalidateRange(wp->garbageBuf, wp->garbageChunk);
}

void aramMgrGarbage(void) {
	u32 offset;
	ARAMWorkNode* prev;
	u32 cursor;
	ARAMWorkNode* first;
	ARAMWorkNode* cur;
	s32 i;
	s32 count;
	ARAMWorkNode* head;
	ARAMWorkNode* node;

	first = wp->nodes;
	for (node = first; node != NULL; node = node->next) {
		if (node->flags & 2) {
			while (!(node->flags & 4)) {
			}
		}
		if (node->flags & 8) {
			while (!(node->flags & 0x10)) {
			}
		}
	}
	prev = NULL;
	head = NULL;
	for (node = first; node != NULL;) {
		if (node->flags & 0x20) {
			node->flags = 0;
			if (prev != NULL) {
				prev->next = node->next;
			}
			node = node->next;
		} else {
			if (prev == NULL) {
				head = node;
			}
			prev = node;
			node = node->next;
		}
	}
	wp->nodes = head;
	wp->tail = prev;
	cursor = wp->aramStart;
	for (cur = wp->nodes; cur != NULL; cur = cur->next) {
		if (cur->aramAddr != cursor) {
			wp->garbageChunk = 0x8000;
			wp->garbageBuf = __memAlloc(HEAP_DEFAULT, wp->garbageChunk);
			count = (wp->garbageChunk - 1 + cur->size) / wp->garbageChunk;
			for (i = 0, offset = 0; i < count; i++) {
				wp->garbageBusy = 1;
				ARQPostRequest(&wp->request, ARAM_OWNER, 1, 0, cur->aramAddr + offset, (u32)wp->garbageBuf,
				               wp->garbageChunk, aramCallbackGarbage);
				while (wp->garbageBusy) {
				}
				wp->garbageBusy = 1;
				ARQPostRequest(&wp->request, ARAM_OWNER, 0, 0, (u32)wp->garbageBuf, cursor + offset,
				               wp->garbageChunk, aramCallbackGarbage);
				while (wp->garbageBusy) {
				}
				offset += 0x8000;
			}
			__memFree(HEAP_DEFAULT, wp->garbageBuf);
			wp->garbageBuf = NULL;
		}
		cur->aramAddr = cursor;
		cursor = cur->aramAddr + cur->size;
	}
	wp->aramCursor = cursor;
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

void* aramMgrToAramAsync(void* data, u32 size) {
	ARAMWorkNode* node = NULL;
	ARAMWorkNode* scan;
	u32 aram;
	s32 i;

	scan = wp->nodeArray;
	for (i = wp->nodeCount; i > 0; i--) {
		if (!(scan->flags & 1)) {
			node = scan;
			break;
		}
		scan++;
	}
	memset(node, 0, sizeof(ARAMWorkNode));
	size = (size + 0x1F) & ~0x1F;
	aram = wp->aramCursor;
	wp->aramCursor += size;
	if (wp->aramCursor >= wp->aramStart + wp->aramSize) {
		aramMgrGarbage();
		aram = wp->aramCursor;
		wp->aramCursor += size;
	}
	DCStoreRange(data, size);
	ARQPostRequest(&node->request, ARAM_OWNER, 0, 0, (u32)data, aram, size, arqCallbackToAram);
	node->flags |= 1;
	node->flags |= 2;
	node->data = data;
	node->size = size;
	node->aramAddr = aram;
	node->next = NULL;
	if (wp->nodes == NULL) {
		wp->nodes = node;
		wp->tail = node;
	} else {
		wp->tail->next = node;
		wp->tail = node;
	}
	return node;
}

void* aramMgrToAram(void* data, u32 size) {
	ARAMWorkNode* work = aramMgrToAramAsync(data, size);

	while (!(work->flags & 4)) {
	}
	return work;
}

void arqCallbackToMram(u32 request) {
	ARAMWorkNode* work = wp->nodes;

	while (work != NULL) {
		if (request == (u32)work + 4) {
			break;
		}
		work = work->next;
	}
	work->flags |= 0x10;
	work->flags |= 0x20;
	DCInvalidateRange(work->mramAddr, work->size);
	ICInvalidateRange(work->mramAddr, work->size);
}

void aramMgrToMram(void* aram, void* mram) {
	ARAMWorkNode* node = aram;

	while (!(node->flags & 4)) {
	}
	ARQPostRequest(&node->request, ARAM_OWNER, 1, 0, node->aramAddr, (u32)mram, node->size, arqCallbackToMram);
	node->flags |= 8;
	node->mramAddr = mram;
	while (!(node->flags & 0x10)) {
	}
}

#pragma once

#include <dolphin/ar.h>
//#include <dolphin/arq.h>

// Per-request block: ARAMWork.nodes is the head of a list of these, and the callback receives &node->request.
typedef struct ARAMWorkNode {
	volatile u16 flags; //0x0, 1 posted to ARAM, 2 waiting for it, 4 reached ARAM, 8 posted back, 0x10 reached MRAM, 0x20 freed
	ARQRequest request; //0x4
	u32 size; //0x24
	u32 aramAddr; //0x28
	void* data; //0x2C
	void* mramAddr; //0x30
	struct ARAMWorkNode* next; //0x34
} ARAMWorkNode;

typedef struct ARAMWork {
	u32 stackIndex[8]; //0x0, ARInit's stack_index_addr
	u32 aramCursor; //0x20
	u32 aramStart; //0x24
	u32 aramSize; //0x28
	ARAMWorkNode* nodeArray; //0x2C
	s32 nodeCount; //0x30
	ARAMWorkNode* nodes; //0x34, head of the live list
	ARAMWorkNode* tail; //0x38
	ARQRequest request; //0x3C, garbage collector's request
	volatile s32 garbageBusy; //0x5C
	void* garbageBuf; //0x60
	u32 garbageChunk; //0x64
} ARAMWork;

void aramMgrInit(void);
void aramMgrGarbage(void);
void* aramMgrToAram(void* data, u32 size);
void aramMgrToMram(void* aram, void* mram);
void* aramMgrToAramAsync(void* data, u32 size);
void arqCallbackToAram(u32 request);
void arqCallbackToMram(u32 request);
void aramCallbackGarbage(u32 request);

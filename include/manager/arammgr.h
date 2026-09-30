#pragma once

#include <dolphin/ar.h>
//#include <dolphin/arq.h>

// Per-request block: ARAMWork.nodes is the head of a list of these, and the callback receives &node->request.
typedef struct ARAMWorkNode {
	u16 flags; //0x0
	ARQRequest request; //0x4
	u8 field_0x24[0x34 - 0x24]; //0x24
	struct ARAMWorkNode* next; //0x34
} ARAMWorkNode;

typedef struct ARAMWork {
	u8 field_0x0[0x34 - 0x0]; //0x0
	ARAMWorkNode* nodes; //0x34
	u8 field_0x38[0x3C - 0x38]; //0x38
	ARQRequest request; //0x3C
	u8 field_0x5C[0x68 - 0x5C]; //0x5C
} ARAMWork;

void aramMgrInit(void);
void* aramMgrToAram(void* data, u32 size);
void aramMgrToMram(void* aram, void* mram);
void* aramMgrToAramAsync(void* data, u32 size);
void arqCallbackToAram(u32 request);

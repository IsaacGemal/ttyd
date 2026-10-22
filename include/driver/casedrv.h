#pragma once

#include <dolphin/types.h>

struct CaseEntry;

typedef struct CaseSetup {
	u16 activeConditionId; //0x0, TODO rename
	u8 pad_2[2]; //0x2
	const char* hitObjName; //0x4
	u32 swFlag; //0x8
	s32 (*activeFunc)(struct CaseEntry* entry); //0xC, copied to CaseEntry.activeFunc
	s32 lwData[16]; //0x10
	u8 field_0x50[0x54 - 0x50]; //0x50
	void* evtCode; //0x54, TODO re-type
	s32 priority; //0x58
} CaseSetup;

typedef struct CaseEntry {
	u16 flags; //0x0
	u8 pad_2[2]; //0x2
	s32 activeConditionId; //0x4, TODO rename
	char hitObjName[0x40]; //0x8
	s32 caseId; //0x48
	u32 swFlag; //0x4C
	s32 (*activeFunc)(struct CaseEntry* entry); //0x50, activeConditionId 17: caseMain sets flags 2 when it returns nonzero
	s32 lwData[16]; //0x54
	s32 field_0x94; //0x94
	void* evtCode; //0x98, TODO re-type
	s32 priority; //0x9C
} CaseEntry;

void caseInit(void);
void caseReInit(void);
s32 caseEntry(CaseSetup* setup);
void caseDelete(s32 caseId);
void caseMain(void);
CaseEntry* caseCheckHitObj(void* obj); // Takes HitEntry
CaseEntry* caseIdToPtr(s32 caseId);
struct HitEntry;
CaseEntry* caseCheckHitObj2(struct HitEntry* hit);

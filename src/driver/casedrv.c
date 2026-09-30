#include "driver/casedrv.h"
#include "driver/hitdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

typedef struct CaseWork {
	s32 count; //0x0
	CaseEntry* entries; //0x4
} CaseWork;

//.sbss
static CaseWork work;
static CaseWork* wp = &work;


void caseInit(void) {
	wp->count = 64;
	wp->entries = __memAlloc(HEAP_DEFAULT, wp->count * sizeof(CaseEntry));
	memset(wp->entries, 0, wp->count * sizeof(CaseEntry));
}

// NONMATCHING (72.86%): 3 instructions differ, 1 missing, 1 extra.
void caseReInit(void) {
	int i;

	for (i = 0; i < wp->count; i++) {
		wp->entries[i].flags &= ~1;
	}
}

// NONMATCHING (81.75%): 3 instructions differ, 3 missing, 6 extra, plus register differences.
s32 caseEntry(CaseSetup* setup) {
	CaseEntry* entry;
	int i;
	if (wp->count > 0) {
		for (i = 0; i < wp->count; i++) {
			entry = &wp->entries[i];
			if (!(entry->flags & 1)) {
				break; //unused entry
			}
		}
	}
	memset(entry, 0, sizeof(CaseEntry));
	entry->caseId = i;
	entry->flags = 0;
	entry->activeConditionId = setup->activeConditionId & 0x7FFF;
	if (setup->activeConditionId & 0x8000) {
		entry->flags |= 4u;
	}
	entry->flags |= 1u;
	strcpy(entry->hitObjName, setup->hitObjName);
	entry->swFlag = setup->swFlag;
	entry->activeFunc = setup->activeFunc;
	entry->evtCode = setup->evtCode;
	entry->priority = setup->priority;
	entry->lwData[16] = 0;
	memcpy(entry->lwData, setup->lwData, sizeof(entry->lwData));
	return i;
}

void caseDelete(s32 caseId) {
	wp->entries[caseId].flags &= ~1u;
}

// NONMATCHING (0.26%): empty placeholder, not decompiled yet.
void caseMain(void) {

}

CaseEntry* caseCheckHitObj(void* obj) {
	CaseEntry* entry;
	int i;
	HitEntry* hit = obj;

	if (gp->inBattle != 0) {
		return NULL;
	}

	for (i = 0, entry = wp->entries; i < wp->count; i++, entry++) {
		if (!(entry->flags & 1)) {
			continue;
		}
		if (entry->activeConditionId == 0x11) {
			continue;
		}
		if ((u32)(entry->activeConditionId - 0x12) <= 1) {
			continue;
		}
		if (strcmp(hitGetName(hit), entry->hitObjName) == 0) {
			return entry;
		}
	}
	return NULL;
}

CaseEntry* caseCheckHitObj2(HitEntry* hit) {
	CaseEntry* entry;
	int i;

	if (gp->inBattle != 0) {
		return NULL;
	}

	for (i = 0, entry = wp->entries; i < wp->count; i++, entry++) {
		if (!(entry->flags & 1)) {
			continue;
		}
		if (entry->activeConditionId == 0x11) {
			continue;
		}
		if ((u32)(entry->activeConditionId - 0x12) <= 1) {
			continue;
		}
		if (entry->activeConditionId == 0xF && strcmp(hitGetName(hit), entry->hitObjName) == 0) {
			return entry;
		}
	}
	return NULL;
}

CaseEntry* caseIdToPtr(s32 caseId) {
	CaseEntry* entry = &wp->entries[caseId];

	if (entry->flags & 1) {
		return entry;
	}
	return NULL;
}

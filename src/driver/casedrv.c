#include "driver/casedrv.h"
#include "driver/hitdrv.h"
#include "driver/npcdrv.h"
#include "driver/swdrv.h"
#include "mario/mariost.h"
#include "mario/mario_party.h"
#include "party.h"
#include "party/party_yoshi.h"
#include "win/win_main.h"
#include "system.h"
#include "manager/evtmgr.h"
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

void caseReInit(void) {
	CaseEntry* entry = wp->entries;
	int i;

	for (i = 0; i < wp->count; i++, entry++) {
		entry->flags &= ~1;
	}
}

s32 caseEntry(CaseSetup* setup) {
	CaseEntry* entry;
	long i;

	for (i = 0, entry = wp->entries; i < wp->count; i++, entry++) {
		if (!(entry->flags & 1)) {
			break; //unused entry
		}
	}
	memset(entry, 0, sizeof(CaseEntry));
	entry->caseId = i;
	entry->flags = 0;
	// The original masks with ~0x8000 (rlwinm 0,17,15), not 0x7FFF (clrlwi);
	// equivalent for this u16 field.
	entry->activeConditionId = setup->activeConditionId & ~0x8000;
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

void caseMain(void) {
	MarioWork* mario = marioGetPtr();
	s32 field;
	CaseEntry* entry;
	u16 val;
	u16 flags;
	s32 i;
	CaseEntry* entry2;
	s32 i2;
	void* hitStandOn;
	void* hit1F8;
	void* hitJumpFrom;
	void* hit1E0;
	void* hitPush;
	void* hit1FC;
	void* hit1F4;

	partyGetPtr(marioGetPartyId());

	hitStandOn = mario->hitobjStandOn;
	hit1F8 = mario->hitobjUnder;
	hitJumpFrom = mario->hitobjJumpFrom;
	hit1E0 = mario->hitobjFront;
	hitPush = mario->hitobjPush;
	hit1FC = mario->hitobjHammer;
	hit1F4 = mario->hitobjHead;
	val = mario->buttonNew;

	if (marioStGetSystemLevel() != 0) {
		return;
	}
	if (winCheck() != 0) {
		return;
	}
	if (gp->inBattle != 0) {
		return;
	}
	if (N_fbatPreventMarioEventChk() != 0) {
		return;
	}
	if (marioCaseEventValidChk() == 0) {
		return;
	}
	if (mario->currMotionId != MARIO_MOTION_STAY && mario->currMotionId != MARIO_MOTION_WALK &&
	    mario->currMotionId != MARIO_MOTION_DASH && yoshiGetStatus() != 1) {
		val = 0;
	}
	field = val & 0x100;

	for (i = 0, entry = wp->entries; i < wp->count; i++, entry++) {
		flags = entry->flags;
		if (!(flags & 1)) {
			continue;
		}
		switch (entry->activeConditionId) {
		case 0:
			if (hitStandOn != NULL && strcmp(hitGetName(hitStandOn), entry->hitObjName) == 0) {
				entry->flags |= 2;
			}
			break;
		case 1:
			if (hitStandOn != NULL) {
				if (strcmp(hitGetName(hitStandOn), entry->hitObjName) != 0) {
					entry->flags |= 2;
				}
			} else {
				entry->flags |= 2;
			}
			break;
		case 2:
			if (hit1F8 != NULL && strcmp(hitGetName(hit1F8), entry->hitObjName) == 0) {
				entry->flags |= 2;
			}
			break;
		case 3:
			if (hit1F8 != NULL) {
				if (strcmp(hitGetName(hit1F8), entry->hitObjName) != 0) {
					entry->flags |= 2;
				}
			} else {
				entry->flags |= 2;
			}
			break;
		case 8:
			if (hit1E0 != NULL && strcmp(hitGetName(hit1E0), entry->hitObjName) == 0) {
				entry->flags |= 2;
			}
			break;
		case 4:
			if (hitJumpFrom != NULL && mario->airTimer == 1 && fbatNpcTalkCheck() == NULL &&
			    strcmp(hitGetName(hitJumpFrom), entry->hitObjName) == 0 && (mario->buttonNew & 0x100) != 0) {
				entry->flags |= 2;
			}
			break;
		case 5:
			if (hitStandOn != NULL && fbatNpcTalkCheck() == NULL &&
			    strcmp(hitGetName(hitStandOn), entry->hitObjName) == 0 && field != 0) {
				entry->flags |= 2;
			}
			break;
		case 6:
			if (hitStandOn != NULL && strcmp(hitGetName(hitStandOn), entry->hitObjName) == 0 &&
			    mario->wStickDir2 > 0x1e) {
				entry->flags |= 2;
			}
			break;
		case 7:
			if (hitStandOn != NULL && strcmp(hitGetName(hitStandOn), entry->hitObjName) == 0 &&
			    (keyGetDirTrg(0) & 0x1000) != 0) {
				entry->flags |= 2;
			}
			break;
		case 9:
			if (hit1E0 != NULL && fbatNpcTalkCheck() == NULL &&
			    strcmp(hitGetName(hit1E0), entry->hitObjName) == 0 && field != 0) {
				entry->flags |= 2;
			}
			break;
		case 10:
			if (hitPush != NULL && strcmp(hitGetName(hitPush), entry->hitObjName) == 0) {
				entry->flags |= 2;
			}
			break;
		case 11:
			if (hitPush == NULL || strcmp(hitGetName(hitPush), entry->hitObjName) != 0) {
				entry->flags |= 2;
			}
			break;
		case 12:
			if (hitPush != NULL && fbatNpcTalkCheck() == NULL &&
			    strcmp(hitGetName(hitPush), entry->hitObjName) == 0 && field != 0) {
				entry->flags |= 2;
			}
			break;
		case 13:
			if (hit1FC != NULL && strcmp(hitGetName(hit1FC), entry->hitObjName) == 0) {
				entry->flags |= 2;
			}
			break;
		case 14:
			if (hit1FC != NULL && mario->currMotionId == MARIO_MOTION_HAMMER_2 &&
			    strcmp(hitGetName(hit1FC), entry->hitObjName) == 0) {
				entry->flags |= 2;
			}
			break;
		case 15: {
			PartyEntry* party = partyGetPtr(marioGetPartyId());
			if (party != NULL && party->currentMemberId == PARTY_MEMBER_BOBBERY) {
				HitEntry* hit = party->frontHit;
				if (hit != NULL && strcmp(hitGetName(hit), entry->hitObjName) == 0) {
					entry->flags |= 2;
				}
			}
			break;
		}
		case 16:
			if (hit1F4 != NULL && strcmp(hitGetName(hit1F4), entry->hitObjName) == 0) {
				entry->flags |= 2;
			}
			break;
		case 18:
			if (swGet(entry->swFlag) != 0) {
				entry->flags |= 2;
			}
			break;
		case 19:
			if (_swGet(entry->swFlag) != 0) {
				entry->flags |= 2;
			}
			break;
		case 17:
			if (entry->activeFunc != NULL && entry->activeFunc(entry) != 0) {
				entry->flags |= 2;
			}
			break;
		}
	}

	entry2 = wp->entries;
	i2 = 0;
	while (i2 < wp->count) {
		if ((entry2->flags & 1) && (entry2->flags & 2)) {
			if (evtCheckID(*(s32*)entry2->field_0x94) == 0) {
				*(s32*)entry2->field_0x94 = 0;
			}
			if (*(s32*)entry2->field_0x94 == 0) {
				EventEntry* evt = evtEntry(entry2->evtCode, entry2->priority, 0x20);
				evt->caseId = entry2->caseId;
				*(s32*)entry2->field_0x94 = evt->eventId;
				memcpy(evt->lwData, entry2->lwData, 0x40);
				if (entry2->priority == 0x14) {
					npcStopForEvent();
				}
			}
			entry2->flags &= ~2;
		}
		i2++;
		entry2++;
	}
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

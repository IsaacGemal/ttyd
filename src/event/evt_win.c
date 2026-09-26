#include "driver/icondrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "mario/mario.h"
#include "party.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "manager/winmgr.h"
#include "driver/msgdrv.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include "data/item_data.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include <dolphin/mtx.h>
#include "manager/fontmgr.h"
#include "driver/camdrv.h"

// evt_unitwin_disp_func is a display callback, not an event command.
void evt_unitwin_disp_func(CameraId cameraId, void* param);

extern void mapObjFlagOff(const char* name, u32 flag);
const char str_802F6894[] = "yubicursor_2";
extern s32 winGhostDiaryChk(void);
extern s32 N_itemPickUpFromFieldCheck(void);

extern int sprintf(char* str, const char* format, ...);
extern ItemData itemDataTable[];
extern f32 float_1000_8041dbe4;
extern f32 float_6_8041dbe8;
extern const f32 float_30_8041dbf0;
extern const f32 float_10_8041dbec;
extern const f32 float_36_8041db64;
extern const f32 float_0_8041db00;
extern const f32 float_1p1_8041db30;
extern const f32 float_0p8_8041db74;
extern const f32 float_124_8041dbf4;
extern const f32 float_38_8041dbf8;
extern u32 dat_8041d9bc;
extern u32 dat_8041d9b4;
extern u32 dat_8041d9b8;
extern const f32 float_108_8041dadc;
extern const f32 float_0p5_8041db60;
extern const f32 float_12_8041dbfc;
const char str_8041DBAC[] = "%d";
f64 double_to_int_802f68a8 = 4503601774854144.0;
f64 double_to_int_mask_802f68b8 = 4503599627370496.0;
const char str_8041DAC0[] = "uwn%d";

// Work structure for the unit window, not yet extracted into a header.
typedef struct UnitWinObj {
	const char* name;      // 0x0
	const char* field_0x4; // 0x4
	void* field_0x8;       // 0x8, event script
} UnitWinObj;              // size 0xC

typedef struct UnitWinWork {
	u16 flags;                  // 0x0
	u16 pad_02;                 // 0x2
	UnitWinObj* objList;        // 0x4
	s32* field_0x8;             // 0x8
	u16 field_0xC[6];           // 0xC
	s32 count;                  // 0x18
	EventEntry* field_0x1C;      // 0x1C
	EventEntry* field_0x20;      // 0x20
	s32 index;                  // 0x24
	s32 field_0x28;             // 0x28
	s32 field_0x2C;             // 0x2C
	s32 field_0x30;             // 0x30
	f32 field_0x34;             // 0x34
	u8 pad_0x38[0xC];           // 0x38
	f32 field_0x44;             // 0x44
	u8 pad_0x48[0x8];           // 0x48
	s32 field_0x50;             // 0x50
	u16 field_0x54;             // 0x54
	u8 pad_0x56[2];             // 0x56
	s32 field_0x58;             // 0x58
	s32 field_0x5C;             // 0x5C
	s32 field_0x60[3];          // 0x60
	u8 pad_0x6C[0x74];          // 0x6C
	s32 field_0xE0;             // 0xE0
	s32 field_0xE4;             // 0xE4
} UnitWinWork;

static UnitWinWork* _wp;
static s16* mario_status_point_table[3];
USER_FUNC(evt_unitwin_main_func);
USER_FUNC(unk_80174e98);
static u32 main_evt[] = { 0x0001005B, (u32)evt_unitwin_main_func, 0x2, 0x1 };
u32 select_evt[] = { 0x0001005B, (u32)unk_80174e98, 0x2, 0x1 };
extern WinMgrDesc unk_80375478[];

USER_FUNC(unitwin_setmariolevel) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	s32 unk;

	pouch->baseMaxHP = evtGetValue(event, args[0]);
	pouch->baseMaxFP = evtGetValue(event, args[1]);
	pouch->maximumBP = evtGetValue(event, args[2]);
	pouchReviseMarioParam();
	pouch->currentHP = pouch->maximumHP;
	pouch->currentFP = pouch->maximumFP;

	if (pouch->availableBP < 0) {
		s32 i;

		for (i = 0; i < 200; i++) {
			if (pouchEquipCheckBadgeIndex(i)) {
				pouchUnEquipBadgeIndex(i);
			}
		}
		pouchReviseMarioParam();
		unk = 1;
	} else {
		unk = 0;
	}

	evtSetValue(event, args[3], unk);
	return EVT_RETURN_DONE;
}

// NONMATCHING (77.77%): the original keeps id*0xe in r30 and defers `pouch+offset`;
// folding the base+offset via `&pouch->partyData[id]` before the second evtGetValue
// changes the scheduled stores.
USER_FUNC(unitwin_setpartylank) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	s32 id = evtGetValue(event, args[0]);
	PouchPartyData* party = &pouch->partyData[id];
	s32 lank = evtGetValue(event, args[1]);

	party->techLevel = lank;
	party->attackLevel = lank;
	party->HPLevel = lank;
	pouchRevisePartyParam();
	party->currentHP = party->maximumHP;
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwin_getpartylank) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	s32 id = evtGetValue(event, args[0]);

	evtSetValue(event, args[1], pouch->partyData[id].techLevel);
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwin_get_work_ptr) {
	evtSetValue(event, event->args[0], (s32)_wp);
	return EVT_RETURN_DONE;
}

// NONMATCHING (96.44%): itemDataTable[].icon_id is declared s16 in item_data.h,
// but the original loads it with lhz (u16); sign extension adds one instruction.
USER_FUNC(set_new_goods_list) {
	s32* args = event->args;
	UnitWinWork* wp = (UnitWinWork*)evtGetValue(event, args[0]);
	s32 item = evtGetValue(event, args[1]);
	char name[32];

	wp->field_0x8[wp->index] = item;
	if (wp->index < wp->count) {
		sprintf(name, str_8041DAC0, wp->index);
		if (item != 0) {
			iconChange(name, (u16)itemDataTable[item].icon_id);
		} else {
			iconDelete(name);
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_unitwin_main_func) {
	UnitWinWork* wp = (UnitWinWork*)event->lwData[0];

	if (wp->field_0x54 & 1) {
		wp->field_0x54 = wp->field_0x54 | 2;
		return EVT_RETURN_DONE;
	}

	if (wp->field_0x28 > 0) {
		wp->field_0x28--;
	} else {
		wp->flags &= ~1;
	}
	dispEntry(8, float_1000_8041dbe4, 0, evt_unitwin_disp_func, wp);
	return EVT_RETURN_BLOCK;
}

// NONMATCHING (98.47%): the original materializes !marioChkCtrl()/!marioChkKey()
// with cntlzw+srwi; writing the same test as `a || b`, `a == 0`, etc. makes the
// compiler emit cmpwi instead.
USER_FUNC(unk_80174e98) {
	UnitWinWork* wp = (UnitWinWork*)event->lwData[0];
	s32 selected = 0;
	MarioWork* mario = marioGetPtr();
	PartyEntry* party = partyGetPtr(0);
	HitEntry* hit = (HitEntry*)mario->hitobjStandOn;
	s32 buttons;
	s32 i;

	if (wp->field_0x54 & 1) {
		wp->field_0x54 = wp->field_0x54 | 2;
		return EVT_RETURN_DONE;
	}
	wp->flags &= ~1;

	buttons = keyGetButtonTrg(0);
	if (!marioChkCtrl() || !marioChkKey()) {
		buttons = 0;
	}

	if (party != NULL && (party->flags & 0x100)) {
		if (wp->field_0xE0 != -1) {
			wp->field_0xE0 = -1;
			winMgrClose(wp->field_0x60[0]);
			winMgrClose(wp->field_0x60[1]);
		}
		return EVT_RETURN_BLOCK;
	}

	if (winGhostDiaryChk() || N_itemPickUpFromFieldCheck()) {
		if (wp->field_0xE0 != -1) {
			wp->field_0xE0 = -1;
			winMgrClose(wp->field_0x60[0]);
			winMgrClose(wp->field_0x60[1]);
		}
		return EVT_RETURN_BLOCK;
	}

	if (!(wp->flags & 2)) {
		if (hit != NULL) {
			for (i = 0; i < wp->count; i++) {
				if (strcmp(wp->objList[i].field_0x4, hitGetName(hit)) == 0) {
					break;
				}
			}
			if (!(wp->field_0xC[i] & 1) && i < wp->count) {
				if (wp->field_0xE0 != i) {
					if (wp->field_0xE0 == -1) {
						winMgrOpen(wp->field_0x60[0]);
						winMgrOpen(wp->field_0x60[1]);
					}
					wp->field_0xE0 = i;
					wp->index = i;
					winMgrHelpInit(winMgrGetPtr(wp->field_0x60[1]));
				}
				selected = 1;
			}
		}
		if (selected == 0 && wp->field_0xE0 != -1) {
			wp->field_0xE0 = -1;
			winMgrClose(wp->field_0x60[0]);
			winMgrClose(wp->field_0x60[1]);
		}
		if (wp->field_0xE0 != -1) {
			wp->flags |= 1;
			if (buttons & 0x100) {
				marioKeyOff();
				wp->field_0x20 = evtEntry(wp->objList[wp->index].field_0x8, 0, 0);
				wp->field_0x20->lwData[0] = (s32)wp;
				wp->flags |= 2;
				winMgrClose(wp->field_0x60[0]);
				winMgrClose(wp->field_0x60[1]);
				wp->field_0xE0 = -1;
				wp->field_0xE4 = 0;
			}
		}
	} else {
		if (evtCheckID(wp->field_0x20->eventId) == 0) {
			wp->flags &= ~2;
			marioKeyOn();
			if (wp->field_0xE4 != 0) {
				wp->field_0xC[wp->index] |= 1;
			}
		}
	}

	for (i = 0; i < wp->count; i++) {
		MapObject* obj = mapGetMapObj(wp->objList[i].name);
		if (obj != NULL) {
			char name[32];

			sprintf(name, str_8041DAC0, i);
			if (wp->field_0xC[i] & 1) {
				iconFlagOn(name, 2);
			} else if (*(u32*)((u8*)obj + 0x0) & 1) {
				iconFlagOn(name, 2);
			} else {
				iconFlagOff(name, 2);
			}
		}
	}
	return EVT_RETURN_BLOCK;
}

USER_FUNC(unk_801751f8) {
	s32* args = event->args;
	UnitWinWork* wp = (UnitWinWork*)evtGetValue(event, args[0]);

	wp->field_0xE4 = evtGetValue(event, args[1]);
	return EVT_RETURN_DONE;
}

USER_FUNC(unk_8017524c) {
	s32 on = evtGetValue(event, event->args[0]);
	UnitWinWork* wp = _wp;

	if (on) {
		wp->flags |= 0x100;
	} else {
		wp->flags &= ~0x100;
	}
	return EVT_RETURN_DONE;
}

// NONMATCHING (89.16%): the loop/return body matches; the epilogue restores r31
// after r30 in the original, the compiler here hoists the r31 restore earlier.
USER_FUNC(unitwincoin_wait) {
	UnitWinWork* wp = _wp;

	pouchGetPtr();
	return (pouchGetCoin() != wp->field_0x2C) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(unitwincoin_disp_onoff) {
	s32 on = evtGetValue(event, event->args[0]);
	UnitWinWork* wp;

	pouchGetPtr();
	wp = _wp;
	wp->field_0x2C = pouchGetCoin();
	wp->field_0x5C = 0;
	if (on) {
		wp->flags |= 0x1000;
	} else {
		wp->flags &= ~0x1000;
	}
	return EVT_RETURN_DONE;
}

// The original keeps the byte offset (index*4) in a register across winMgrAction
// and indexes with lwzx; writing `list[wp->index]` makes the compiler defer the
// shift past the call, so compute the byte offset explicitly.
void unk_801758fc(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	s32 offset = wp->index * 4;
	s32* list = wp->field_0x8;

	if (!winMgrAction(wp->field_0x60[1])) {
		entry->field_0x34 = (s32)msgSearch(itemDataTable[*(s32*)((u8*)list + offset)].description);
		winMgrHelpDraw(entry);
	}
}

void unk_8017597c(WinMgrEntry* entry) {
	winMgrHelpMain(entry);
}

USER_FUNC(evt_unitwin_free) {
	UnitWinWork* wp = _wp;
	s32 i;

	if (wp == NULL) {
		return EVT_RETURN_DONE;
	}
	if (isFirstCall) {
		wp->field_0x54 |= 1;
	}
	if (!(wp->field_0x54 & 2)) {
		return EVT_RETURN_BLOCK;
	}
	for (i = 0; i < wp->count; i++) {
		char name[32];

		mapObjFlagOff(wp->objList[i].name, 0x4000);
		sprintf(name, str_8041DAC0, i);
		iconDelete(name);
	}
	if (wp->flags & 0x80) {
		for (i = 0; i < 3; i++) {
			winMgrDelete(wp->field_0x60[i]);
		}
	}
	iconDelete(str_802F6894);
	_mapFree(_wp);
	_wp = NULL;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_unitwin_alloc) {
	s32 flag = evtGetValue(event, event->args[0]);
	UnitWinWork* wp = _mapAlloc(0xE8);

	memset(wp, 0, 0xE8);
	wp->field_0x50 = 0;
	_wp = wp;
	if (flag) {
		wp->flags |= 0x80;
	} else {
		wp->flags &= ~0x80;
	}
	iconEntry2D(str_802F6894, 0x1F0);
	iconFlagOn(str_802F6894, 2);
	iconFlagOn(str_802F6894, 4);
	if (!(wp->flags & 0x80)) {
		wp->field_0x1C = evtEntry(main_evt, 0, 0);
		wp->field_0x1C->lwData[0] = (s32)wp;
	}
	mario_status_point_table[0] = &pouchGetPtr()->baseMaxHP;
	mario_status_point_table[1] = &pouchGetPtr()->baseMaxFP;
	mario_status_point_table[2] = &pouchGetPtr()->maximumBP;
	return EVT_RETURN_DONE;
}

// NONMATCHING (98.40%): itemDataTable[].icon_id is declared s16 in item_data.h,
// but the original loads it with lhz (u16); sign extension adds one instruction.
USER_FUNC(evt_unitwin_selltable_setup) {
	s32* args = event->args;
	s32 i;
	Vec pos;
	UnitWinWork* wp;
	s32 objList = evtGetValue(event, args[0]);
	s32* itemList = (s32*)evtGetValue(event, args[1]);

	wp = _wp;
	wp->objList = (UnitWinObj*)objList;
	wp->field_0x8 = itemList;
	wp->count = 0;
	for (i = 0; wp->objList[i].name != NULL; i++) {
		char name[32];

		wp->count++;
		mapObjGetPos(wp->objList[i].name, &pos);
		mapObjFlagOn(wp->objList[i].name, 0x4000);
		if (wp->field_0x8[i] != 0) {
			sprintf(name, str_8041DAC0, i);
			iconEntry(name, (u16)itemDataTable[wp->field_0x8[i]].icon_id);
			iconSetPos(name, pos.x, float_6_8041dbe8 + pos.y, pos.z);
			iconFlagOn(name, 0x40);
		} else {
			wp->field_0xC[i] |= 1;
		}
	}
	wp->field_0x1C = evtEntryType(select_evt, 0, 0x20, 0);
	wp->field_0x1C->lwData[0] = (s32)wp;
	for (i = 0; i < 3; i++) {
		wp->field_0x60[i] = winMgrEntry(&unk_80375478[i]);
	}
	wp->field_0xE0 = -1;
	return EVT_RETURN_DONE;
}

void unk_80175710(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	PouchData* pouch = pouchGetPtr();

	if (!(entry->flags & 4)) {
		Mtx mtx;
		Mtx scale;

		PSMTXTrans(mtx, float_10_8041dbec + (float_30_8041dbf0 + (f32)entry->field_0x18), (f32)entry->field_0x1C - float_36_8041db64, float_0_8041db00);
		if ((u32)gp->language == 0) {
			PSMTXScale(scale, float_1p1_8041db30, float_1p1_8041db30, float_1p1_8041db30);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x1F1);
		} else {
			PSMTXScale(scale, float_0p8_8041db74, float_1p1_8041db30, float_1p1_8041db30);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x20A);
		}
		PSMTXTrans(mtx, float_124_8041dbf4 + (f32)entry->field_0x18, (f32)entry->field_0x1C - float_38_8041dbf8, float_0_8041db00);
		iconNumberDispGx(mtx, wp->field_0x30, 0, *(GXColor*)&dat_8041d9bc);
		{
			s32 delta = pouch->mShopPoints - wp->field_0x30;

			if (delta != 0) {
				delta /= 10;
				if (delta == 0) {
					delta = (pouch->mShopPoints > wp->field_0x30) ? 1 : -1;
				}
				wp->field_0x30 += delta;
			}
		}
	}
}

// NONMATCHING (98.05%): itemDataTable[].discount_price is declared s16 but the
// original loads it with lhz (u16); remaining diffs are stack slot and float
// register allocation offsets.
void unk_8017599c(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	GXColor iconColor;
	GXColor titleColor;
	Vec pos;
	char buf[0x40];
	Mtx mtx;
	const char* msg;
	s32 i;

	msg = msgSearch(itemDataTable[wp->field_0x8[wp->index]].name);
	if (winMgrAction(wp->field_0x60[0]) == 0) {
		FontDrawStart();
		FontDrawEdge();
		*(u32*)&titleColor = dat_8041d9b4;
		FontDrawColor(&titleColor);
		FontDrawString(msg, (f32)(-(s32)FontGetMessageWidth(msg) / 2), float_108_8041dadc);
		FontDrawEdgeOff();
	}
	for (i = 0; i < wp->count; i++) {
		if (wp->field_0xC[i] & 1) {
			continue;
		}
		mapObjGetPos(wp->objList[i].name, &pos);
		pos.y += float_30_8041dbf0;
		PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
		PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
		pos.x *= float_0p5_8041db60 * (f32)gp->fbWidth;
		pos.y *= float_0p5_8041db60 * (f32)gp->efbHeight;
		sprintf(buf, str_8041DBAC, itemDataTable[wp->field_0x8[i]].discount_price);
		PSMTXTrans(mtx, float_12_8041dbfc * (f32)(strlen(buf) - 1) + pos.x, pos.y, pos.z);
		*(u32*)&iconColor = dat_8041d9b8;
		iconNumberDispGx(mtx, itemDataTable[wp->field_0x8[i]].discount_price, 0, iconColor);
	}
}

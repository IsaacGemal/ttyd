#include "manager/fontmgr.h"
void FontDrawColor(GXColor* color);
#include "evt/evt_shop.h"
#include "evt/evt_case.h"
#include "mario/mariost.h"
#include "mario/mario_pouch.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/winmgr.h"
#include "driver/camdrv.h"
#include "data/item_data.h"
#include "memory.h"
#include "system.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>
#include <string.h>
#include "driver/icondrv.h"
#include "driver/swdrv.h"
#include "driver/msgdrv.h"
#include "driver/mapdrv.h"
#include "driver/hitdrv.h"
#include "driver/itemdrv.h"
#include "window/win_main.h"
#include "party.h"
#include "window/win_mario.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_pouch.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "matching.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern u32 first_shopping_evt[];

// This file's own script, defined below.
extern u32 buy_evt[];

// The remaining scripts are file-local (their asm `.obj` blocks are `local`).
static u32 sell_evt[196];
static u32 keep_evt[148];
static u32 unkeep_evt[147];
static u32 point_evt[15];

extern GlobalWork* gp;

//.sbss (uninitialised globals are laid out last-declared-first)
u8 gap_08_80418944_sbss[4];
static evt_shop_work* _wp;
char mapname[16];

typedef struct ShopWork {
	u16 flags;              //0x0
	u16 pad_02;             //0x2
	void* field_0x4;        //0x4
	s32* field_0x8;         //0x8
	void* field_0xC;        //0xC
	void* field_0x10;       //0x10
	u16 field_0x14[6];      //0x14
	s32 field_0x20;         //0x20
	EventEntry* field_0x24; //0x24
	EventEntry* field_0x28; //0x28
	s32 field_0x2C;         //0x2C
	s32 field_0x30;         //0x30
	s32 field_0x34;         //0x34
	s32 field_0x38;         //0x38
	u8 field_0x3C[0x50 - 0x3C];
	s32 field_0x50;         //0x50
	s32 field_0x54;         //0x54
	s32 field_0x58;         //0x58
	u8 field_0x5C[0xD0 - 0x5C];
	s32 field_0xD0;         //0xD0
	s32 field_0xD4;         //0xD4
} ShopWork;

typedef struct ShopPointEntry {
	s32 price; //0x0
	s32 item;  //0x4
} ShopPointEntry;

typedef struct ItemDbEntry {
	s32 id;      //0x0
	s16 field_4; //0x4
	s16 field_6; //0x6
} ItemDbEntry;

extern ItemData itemDataTable[];
extern ShopPointEntry shopPointList[];
extern const char str_802ECD50[];
extern const char str_802ECE38[];
extern u32 dat_8041bbec;
extern u32 dat_8041bbe8;
extern u32 dat_8041bbe0;
extern u32 dat_8041bbe4;
extern const char str_8041BC60[];
extern const f32 float_0p5_8041bc5c;
extern const f32 float_12_8041bc64;
extern const f32 float_108_8041bc58;
extern const f32 float_30_8041bc40;
extern const f32 float_10_8041bc3c;
extern const f32 float_36_8041bc44;
extern const f32 float_0_8041bc34;
extern const f32 float_1p1_8041bc48;
extern const f32 float_0p8_8041bc4c;
extern const f32 float_124_8041bc50;
extern const f32 float_38_8041bc54;
extern WinMgrDesc list_desc[];
extern WinMgrDesc window_desc[];
extern const char str_802ECDE8[];
extern const char str_8041BC1C[6];
extern const f32 float_6_8041bc24;
extern u32 shopper_talk_evt[];
extern u32 main_evt[4];
extern u32 evt_shoplist[10];
extern const char str_802ECDF8[];
extern const char str_802ECE10[];
extern const char str_8041BC28[];
extern u32 dat_8041bbf0;
extern u32 dat_8041bbf4;
extern u32 dat_8041bbf8;
extern u32 dat_8041bbfc;
extern u32 dat_8041bc00;
extern u32 dat_8041bc04;
extern u32 dat_8041bc08;
extern u32 dat_8041bc0c;
extern const f32 float_0p6_8041bc2c;
extern const f32 float_40_8041bc30;
extern const f32 float_0p64_8041bc38;

// Data, in the original object's order (build/G8MJ01/asm/event/evt_shop.s).
// Named objects precede the real functions so immediate generation preserves their storage.

// .rodata
const char str_802ECD50[16] = "item_first_hit";
const char str_802ECD60[8] = "mri_07";
const char str_802ECD68[16] = "stg2_shop_04_1";
const char str_802ECD78[8] = "usu_01";
const char str_802ECD80[20] = "usugurashop_04_1";
const char str_802ECD94[12] = "rsh_03_a";
const char str_802ECDA0[12] = "rsh_03_b";
const char str_802ECDAC[12] = "rsh_03_c";
const char str_802ECDB8[16] = "stg6_shop_04_1";
const char str_802ECDC8[24] = "<scrl_auto><once_stop>";
const char str_802ECDE0[8] = "rsh_03";
const char str_802ECDE8[16] = "a_shop_keihin";
const char str_802ECDF8[24] = "msg_shop_point_list5";
const char str_802ECE10[24] = "msg_shop_point_list6";

// .sdata2
__declspec(section ".sdata2") u32 dat_8041bbe0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041bbe4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041bbe8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041bbec = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041bbf0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041bbf4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041bbf8 = 0xF00000FF;
__declspec(section ".sdata2") u32 dat_8041bbfc = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041bc00 = 0xF00000FF;
__declspec(section ".sdata2") u32 dat_8041bc04 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041bc08 = 0xF00000FF;
__declspec(section ".sdata2") u32 dat_8041bc0c = 0x000000FF;
__declspec(section ".sdata2") const char str_8041BC10[8] = "mario";
__declspec(section ".sdata2") const char str_8041BC18[4] = "me";
__declspec(section ".sdata2") const char str_8041BC1C[6] = "shp%d";
__declspec(section ".sdata2") const f32 float_6_8041bc24 = 6.0f;
__declspec(section ".sdata2") const char str_8041BC28[4] = "%s";
__declspec(section ".sdata2") const f32 float_0p6_8041bc2c = 0.6f;
__declspec(section ".sdata2") const f32 float_40_8041bc30 = 40.0f;
__declspec(section ".sdata2") const f32 float_0_8041bc34 = 0.0f;
__declspec(section ".sdata2") const f32 float_0p64_8041bc38 = 0.64f;
__declspec(section ".sdata2") const f32 float_10_8041bc3c = 10.0f;
__declspec(section ".sdata2") const f32 float_30_8041bc40 = 30.0f;
__declspec(section ".sdata2") const f32 float_36_8041bc44 = 36.0f;
__declspec(section ".sdata2") const f32 float_1p1_8041bc48 = 1.1f;
__declspec(section ".sdata2") const f32 float_0p8_8041bc4c = 0.8f;
__declspec(section ".sdata2") const f32 float_124_8041bc50 = 124.0f;
__declspec(section ".sdata2") const f32 float_38_8041bc54 = 38.0f;
__declspec(section ".sdata2") const f32 float_108_8041bc58 = 108.0f;
__declspec(section ".sdata2") const f32 float_0p5_8041bc5c = 0.5f;
__declspec(section ".sdata2") const char str_8041BC60[4] = "%d";
__declspec(section ".sdata2") const f32 float_12_8041bc64 = 12.0f;

// Keep named scalar loads when definitions precede immediate code generation.
#ifdef MUST_MATCH
#define SHOP_F32(value) (*(f32*)&(value))
#else
#define SHOP_F32(value) (value)
#endif

// Emit real functions in retail order; list_disp pools the native unsigned conversion first.
#ifdef MUST_MATCH
#pragma defer_codegen off
#endif

s32 evtShopIsActive(void) {
	s32 active;

	if (strcmp(gp->currentMapName, mapname) == 0 && ((ShopWork*)_wp)->flags != 0) {
		active = 1;
	} else {
		active = 0;
	}
	return active;
}

USER_FUNC(evt_shop_main_func) {
	ShopWork* wp = (ShopWork*)event->lwData[0];
	s32 selected = 0;
	MarioWork* mario = marioGetPtr();
	PartyEntry* party = partyGetPtr(0);
	HitEntry* hit = (HitEntry*)mario->hitobjStandOn;
	s32 buttons;
	s32 i;

	wp->flags &= ~1;
	buttons = keyGetButtonTrg(0);
	{
#ifdef MUST_MATCH
		// The original shares one buttons=0 tail between the two checks; the natural
		// short-circuit below materialises the second compare instead of branching on it.
		BOOL notCtrl = (marioChkCtrl() == 0);

		do {
			if (!notCtrl) {
				notCtrl = (marioChkKey() == 0);
				if (!notCtrl) {
					break;
				}
			}
			buttons = 0;
		} while (0);
#else
		if (!marioChkCtrl() || !marioChkKey()) {
			buttons = 0;
		}
#endif
	}

	if (strncmp(gp->currentMapName, str_802ECDE0, strlen(str_802ECDE0)) == 0) {
		if (evtGetValue(event, 0xF5DE0180) >= 0x148 && evtGetValue(event, 0xF5DE0180) < 0x14C &&
		    evtGetValue(event, 0xF84068F2) == 0) {
			return 0;
		}
	}

	if (party != NULL && (party->flags & 0x100)) {
		if (wp->field_0xD0 != -1) {
			wp->field_0xD0 = -1;
			winMgrClose(wp->field_0x50);
			winMgrClose(wp->field_0x54);
		}
		return 0;
	}

	if (winGhostDiaryChk() || N_itemPickUpFromFieldCheck()) {
		if (wp->field_0xD0 != -1) {
			wp->field_0xD0 = -1;
			winMgrClose(wp->field_0x50);
			winMgrClose(wp->field_0x54);
		}
		return 0;
	}

	if (!(wp->flags & 2)) {
		if (hit != NULL) {
			for (i = 0; i < wp->field_0x20; i++) {
				if (strcmp(((ShopObj*)wp->field_0x4)[i].hitName, hitGetName(hit)) == 0) {
					break;
				}
			}
			if (!(wp->field_0x14[i] & 1) && i < wp->field_0x20) {
				if (wp->field_0xD0 != i) {
					if (wp->field_0xD0 == -1) {
						winMgrOpen(wp->field_0x50);
						winMgrOpen(wp->field_0x54);
					}
					wp->field_0xD0 = i;
					wp->field_0x2C = i;
					winMgrHelpInit(winMgrGetPtr(wp->field_0x54));
				}
				selected = 1;
			}
		}
		if (selected == 0 && wp->field_0xD0 != -1) {
			wp->field_0xD0 = -1;
			winMgrClose(wp->field_0x50);
			winMgrClose(wp->field_0x54);
		}
		if (wp->field_0xD0 != -1) {
			wp->flags |= 1;
			if (buttons & 0x100) {
				marioKeyOff();
				wp->field_0x28 = evtEntry(buy_evt, 0, 0);
				wp->field_0x28->lwData[0] = (s32)wp;
				wp->flags |= 2;
				winMgrClose(wp->field_0x50);
				winMgrClose(wp->field_0x54);
				wp->field_0xD0 = -1;
				wp->field_0xD4 = 0;
			}
		}
	} else {
		if (evtCheckID(wp->field_0x28->eventId) == 0) {
			wp->flags &= ~2;
			marioKeyOn();
			if (wp->field_0xD4 == 0x33) {
				wp->field_0x14[wp->field_0x2C] |= 1;
			}
			if (wp->field_0xD4 == 0x37) {
				wp->field_0x14[wp->field_0x2C] |= 1;
			}
			if (wp->field_0xD4 == 0x38) {
				wp->field_0x14[wp->field_0x2C] |= 1;
			}
			if (wp->field_0xD4 == 0x39) {
				wp->field_0x14[wp->field_0x2C] |= 1;
			}
			if (wp->field_0xD4 == 0x3A) {
				wp->field_0x14[wp->field_0x2C] |= 1;
			}
			if (wp->field_0xD4 == 0x3B) {
				wp->field_0x14[wp->field_0x2C] |= 1;
			}
		}
	}

	for (i = 0; i < wp->field_0x20; i++) {
		MapObject* obj = mapGetMapObj(((ShopObj*)wp->field_0x4)[i].name);
		if (obj != NULL) {
			char name[32];

			sprintf(name, str_8041BC1C, i);
			if (wp->field_0x14[i] & 1) {
				iconFlagOn(name, 2);
			} else if (*(u32*)obj & 1) {
				iconFlagOn(name, 2);
			} else {
				iconFlagOff(name, 2);
			}
		}
	}
	return 0;
}

USER_FUNC(unkeep_pouchcheck_func) {
	pouchGetPtr();
	event->lwData[0] = pouchGetKeepItemCnt();
	event->lwData[1] = pouchGetEmptyHaveItemCnt();
	return EVT_RETURN_DONE;
}

USER_FUNC(keep_pouchcheck_func) {
	pouchGetPtr();
	event->lwData[0] = pouchGetHaveItemCnt();
	event->lwData[1] = pouchGetEmptyKeepItemCnt();
	return EVT_RETURN_DONE;
}

s32 item_data_db_restore(void) {
	ItemDbEntry* p = (ItemDbEntry*)((ShopWork*)_wp)->field_0x10;
	s32 id;

	if (p != NULL) {
		while ((id = p->id) != 0) {
			itemDataTable[id].sell_price = p->field_6;
			p++;
		}
	}
	return EVT_RETURN_DONE;
}

s32 item_data_db_arrange(void) {
	ItemDbEntry* p = (ItemDbEntry*)((ShopWork*)_wp)->field_0x10;
	s32 id;

	if (p != NULL) {
		while ((id = p->id) != 0) {
			p->field_6 = *(u16*)&itemDataTable[id].sell_price;
			itemDataTable[p->id].sell_price = p->field_4;
			p++;
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(sell_pouchcheck_func) {
	pouchGetPtr();
	event->lwData[0] = pouchGetHaveItemCnt();
	return EVT_RETURN_DONE;
}

USER_FUNC(name_price) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)msgSearch(itemDataTable[id].name));
	evtSetValue(event, args[2], *(u16*)&itemDataTable[id].sell_price);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_fook_evt) {
	evtSetValue(event, event->args[0], *(s32*)((u8*)((ShopWork*)_wp)->field_0xC + 0x4));
	return EVT_RETURN_DONE;
}

USER_FUNC(get_ptr) {
	evtSetValue(event, event->args[0], (s32)_wp);
	return EVT_RETURN_DONE;
}

USER_FUNC(set_buy_item_id) {
	((ShopWork*)_wp)->field_0xD4 = evtGetValue(event, event->args[0]);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_buy_evt) {
	evtSetValue(event, event->args[0], *(s32*)((u8*)((ShopWork*)_wp)->field_0xC + 0x8));
	return EVT_RETURN_DONE;
}

USER_FUNC(get_msg) {
	s32* args = event->args;
	void* p = (void*)evtGetValue(event, args[0]);
	s32 idx = evtGetValue(event, args[1]);
	u8* base = *(u8**)((u8*)p + 0xC);
	s32 value;

	base += idx * 4;
	value = *(s32*)(base + 0xC);

	evtSetValue(event, args[2], value);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_value) {
	s32* args = event->args;
	ShopWork* wp = (ShopWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)msgSearch(itemDataTable[wp->field_0x8[wp->field_0x2C * 2]].name));
	evtSetValue(event, args[2], wp->field_0x8[wp->field_0x2C * 2]);
	evtSetValue(event, args[3], wp->field_0x8[wp->field_0x2C * 2 + 1]);
	return EVT_RETURN_DONE;
}

USER_FUNC(disp_off) {
	ShopWork* wp = (ShopWork*)evtGetValue(event, event->args[0]);

	wp->flags &= ~1;
	return EVT_RETURN_DONE;
}

USER_FUNC(shopper_name) {
	evtSetValue(event, event->args[0], *(s32*)((u8*)((ShopWork*)_wp)->field_0xC + 0x0));
	return EVT_RETURN_DONE;
}

USER_FUNC(shop_point_item) {
	s32* args = event->args;
	s32 value = evtGetValue(event, args[0]);
	ShopPointEntry* p;
	s32 price;

	pouchGetPtr();
	p = shopPointList;
	while ((price = p->price) != -1) {
		if (price == value) {
			evtSetValue(event, args[1], p->item);
			evtSetValue(event, args[2], (s32)msgSearch(itemDataTable[p->item].name));
			return EVT_RETURN_DONE;
		}
		p++;
	}
	evtSetValue(event, args[1], -1);
	evtSetValue(event, args[2], -1);
	return EVT_RETURN_DONE;
}

USER_FUNC(exchange_shop_point) {
	PouchData* pouch = pouchGetPtr();
	ShopPointEntry* p;
	s32 i;
	s32 price;

	for (i = 0, p = shopPointList; (price = p->price) != -1; i++, p++) {
		if (price <= pouch->mShopPoints && !(pouch->field_0x5B4 & (1 << i))) {
			pouch->field_0x5B4 |= 1 << i;
			return EVT_RETURN_DONE;
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(chk_shop_point) {
	s32* args = event->args;
	ShopPointEntry* p;
	PouchData* pouch = pouchGetPtr();
	s16 points;
	s32 i;
	s32 price;

	for (i = 0, p = shopPointList; (price = p->price) != -1; i++, p++) {
		points = pouch->mShopPoints;

		if (price > points) {
			continue;
		}
		if (pouch->field_0x5B4 & (1 << i)) {
			continue;
		}
		if (points != 0x12C) {
			evtSetValue(event, args[0], 0);
			evtSetValue(event, args[1], p->item);
			evtSetValue(event, args[2], (s32)msgSearch(itemDataTable[p->item].name));
		}
		else {
			s32 j;

			for (j = 0; shopPointList[j].price != 0x12C; j++) {
			}
			evtSetValue(event, args[0], 0);
			evtSetValue(event, args[1], shopPointList[j].item);
			evtSetValue(event, args[2], (s32)msgSearch(itemDataTable[shopPointList[j].item].name));
		}
		return EVT_RETURN_DONE;
	}
	evtSetValue(event, args[0], -1);
	evtSetValue(event, args[1], -1);
	evtSetValue(event, args[2], -1);
	return EVT_RETURN_DONE;
}

USER_FUNC(reset_shop_point) {
	PouchData* pouch = pouchGetPtr();

	pouch->mShopPoints = 0;
	pouch->field_0x5B4 = 0;
	return EVT_RETURN_DONE;
}

USER_FUNC(add_shop_point) {
#ifdef MUST_MATCH
	// MUST_MATCH: holding the string base as an integer reproduces the original's base+offset
	// addressing (addi r4, r27, off); as a const char* MWCC rematerializes the base per use.
	u32 str = (u32)str_802ECD50;
#else
	const char* str = str_802ECD50;
#endif
	s32* args = event->args;
	s32 v0 = evtGetValue(event, args[0]);
	s32 v1 = evtGetValue(event, args[1]);
	s32 v2 = evtGetValue(event, args[2]);
	s32 v3 = evtGetValue(event, args[3]);
	PouchData* pouch = pouchGetPtr();

#ifdef MUST_MATCH
	if (strcmp(gp->currentMapName, (const char*)(str + 0x10)) == 0 && swGet(0xFA) && v1 < 900000) {
#else
	if (strcmp(gp->currentMapName, str + 0x10) == 0 && swGet(0xFA) && v1 < 900000) {
#endif
		v0 *= 2;
	}
#ifdef MUST_MATCH
	if (strcmp(gp->currentMapName, (const char*)(str + 0x28)) == 0 && swGet(0xFB) && v2 < 900000) {
#else
	if (strcmp(gp->currentMapName, str + 0x28) == 0 && swGet(0xFB) && v2 < 900000) {
#endif
		v0 *= 2;
	}
#ifdef MUST_MATCH
	if (strncmp(gp->currentMapName, (const char*)(str + 0x90), 6) == 0 && swGet(0xFC) && v3 < 900000) {
#else
	if (strncmp(gp->currentMapName, str + 0x90, 6) == 0 && swGet(0xFC) && v3 < 900000) {
#endif
		v0 *= 2;
	}
	pouch->mShopPoints += v0;
	if (pouch->mShopPoints > 0x12C) {
		pouch->mShopPoints = 0x12C;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(get_shop_point) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();

	evtSetValue(event, args[0], pouch->mShopPoints);
	return EVT_RETURN_DONE;
}

USER_FUNC(shop_flag_onoff) {
	s32* args = event->args;
	s32 a = evtGetValue(event, args[0]);
	s32 b = evtGetValue(event, args[1]);
	ShopWork* wp = (ShopWork*)_wp;

	if (a != 0) {
		wp->flags |= b;
	}
	else {
		wp->flags &= ~b;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(point_wait) {
	ShopWork* wp = (ShopWork*)_wp;
	PouchData* pouch = pouchGetPtr();

	if (wp->field_0x38 != pouch->mShopPoints) {
		psndSFXOn((const char*)0x8CA);
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(point_disp_onoff) {
	s32 value = evtGetValue(event, event->args[0]);
	PouchData* pouch = pouchGetPtr();
	ShopWork* wp = (ShopWork*)_wp;

	wp->field_0x38 = pouch->mShopPoints;
	if (value != 0) {
		winMgrOpen(wp->field_0x58);
	}
	else {
		winMgrClose(wp->field_0x58);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_shop_setup) {
	s32* args;
	s32 v0;
	s32 v1;
	s32 v2;
	s32 v3;
	NpcEntry* npc;
	s32 i;
	ShopWork* wp;

	args = event->args;
	v0 = evtGetValue(event, args[0]);
	v1 = evtGetValue(event, args[1]);
	v2 = evtGetValue(event, args[2]);
	v3 = evtGetValue(event, args[3]);

	wp = (ShopWork*)_mapAlloc(0xD8);
	memset(wp, 0, 0xD8);
	_wp = (evt_shop_work*)wp;
	strncpy(mapname, gp->currentMapName, 15);
	wp->field_0x4 = (void*)v0;
	wp->field_0x8 = (s32*)v1;
	wp->field_0xC = (void*)v2;
	wp->field_0x10 = (void*)v3;
	wp->field_0x20 = 6;

	for (i = 0; i < wp->field_0x20; i++) {
		Vec pos;
		char buf[0x20];

		mapObjGetPos(*(const char**)((u8*)wp->field_0x4 + i * 8), &pos);
		mapObjFlagOn(*(const char**)((u8*)wp->field_0x4 + i * 8), 0x4000);
		sprintf(buf, str_8041BC1C, i);
		iconEntry(buf, *(u16*)&itemDataTable[wp->field_0x8[i * 2]].icon_id);
		iconSetPos(buf, pos.x, SHOP_F32(float_6_8041bc24) + pos.y, pos.z);
		iconFlagOn(buf, 0x40);
		wp->field_0x14[i] = 0;
		if (wp->field_0x8[i * 2] == 0x33 && (s32)pouchCheckItem(0x33) != 0) {
			wp->field_0x14[i] |= 1;
		}
		if (wp->field_0x8[i * 2] == 0x37 && (s32)pouchCheckItem(0x37) != 0) {
			wp->field_0x14[i] |= 1;
		}
		if (wp->field_0x8[i * 2] == 0x38 && (s32)pouchCheckItem(0x38) != 0) {
			wp->field_0x14[i] |= 1;
		}
		if (wp->field_0x8[i * 2] == 0x39 && (s32)pouchCheckItem(0x39) != 0) {
			wp->field_0x14[i] |= 1;
		}
		if (wp->field_0x8[i * 2] == 0x3A && (s32)pouchCheckItem(0x3A) != 0) {
			wp->field_0x14[i] |= 1;
		}
		if (wp->field_0x8[i * 2] == 0x3B && (s32)pouchCheckItem(0x3B) != 0) {
			wp->field_0x14[i] |= 1;
		}
	}
	npc = npcNameToPtr(*(const char**)v2);
	npc->talkEvt = shopper_talk_evt;
	wp->field_0x24 = evtEntryType(main_evt, 0, 0x20, 0);
	wp->field_0x24->lwData[0] = (s32)wp;
	for (i = 0; i < 3; i++) {
		(&wp->field_0x50)[i] = winMgrEntry(&window_desc[i]);
	}
	wp->field_0xD0 = -1;
	evtRunCaseEntry(9, 1, (s32)str_802ECDE8, 0, (s32*)evt_shoplist, 0);
	return EVT_RETURN_DONE;
}

USER_FUNC(disp_list) {
	if (isFirstCall) {
		event->userdata[0] = winMgrEntry(&list_desc[0]);
		event->userdata[1] = winMgrEntry(&list_desc[1]);
		event->userdata[2] = winMgrEntry(&list_desc[2]);
		winMgrOpen(event->userdata[0]);
		winMgrOpen(event->userdata[1]);
		winMgrOpen(event->userdata[2]);
		psndSFXOn((const char*)0x2A);
	}
	if (keyGetButtonTrg(0) & 0x200) {
		winMgrCloseAutoDelete(event->userdata[0]);
		winMgrCloseAutoDelete(event->userdata[1]);
		winMgrCloseAutoDelete(event->userdata[2]);
		psndSFXOn((const char*)0x2B);
		return 1;
	}
	return 0;
}

void list_disp(WinMgrEntry* entry) {
	PouchData* pouch;
	GXColor color0;
	GXColor color1;
	GXColor color2;
	GXColor color3;
	GXColor color4;
	GXColor color5;
	GXColor color6;
	GXColor color7;
	s32 i;
	ItemData* item;
	const char* msg;
	f32 s;
	char buf[0x100];
	Mtx mtx;
	Mtx scale;

	if (entry->flags & 4) {
		return;
	}

	pouch = pouchGetPtr();
	i = 0;

	msg = msgSearch(str_802ECDF8);
	FontDrawStart();
	FontDrawEdge();
	*(u32*)&color0 = dat_8041bbf0;
	FontDrawColor(&color0);
	FontDrawMessage((100 - FontGetMessageWidth(msg)) / 2 + (10 + entry->x), entry->y + 13, msg);
	FontDrawEdgeOff();

	msg = msgSearch(str_802ECE10);
	FontDrawStart();
	FontDrawEdge();
	*(u32*)&color1 = dat_8041bbf4;
	FontDrawColor(&color1);
	FontDrawMessage(entry->x + entry->width / 2 - 10 + (100 - FontGetMessageWidth(msg)) / 2, entry->y + 13, msg);
	FontDrawEdgeOff();

	FontDrawStart();
	for (; shopPointList[i].price != -1; i++) {
		item = &itemDataTable[shopPointList[i].item];
		FontDrawStart();
		if (pouch->field_0x5B4 & (1 << i)) {
			*(u32*)&color2 = dat_8041bbf8;
			FontDrawColor(&color2);
		}
		else {
			*(u32*)&color3 = dat_8041bbfc;
			FontDrawColor(&color3);
		}
		sprintf(buf, str_8041BC28, winZenkakuStr(shopPointList[i].price));
		FontDrawMessage(entry->width / 2 - FontGetMessageWidth(buf) + entry->x - 90, entry->y - 20 - i * 26, buf);

		FontDrawStart();
		if (pouch->field_0x5B4 & (1 << i)) {
			*(u32*)&color4 = dat_8041bc00;
			FontDrawColor(&color4);
		}
		else {
			*(u32*)&color5 = dat_8041bc04;
			FontDrawColor(&color5);
		}
		sprintf(buf, str_8041BC28, msgSearch(str_802ECDF8));
		{
			f32 w = FontGetMessageWidth(buf);
			f32 x = (f32)(entry->width / 2) - SHOP_F32(float_0p6_8041bc2c) * w;
			x = (f32)entry->x + x;
			x -= SHOP_F32(float_40_8041bc30);
			PSMTXTrans(mtx, x, entry->y - 28 - i * 26, SHOP_F32(float_0_8041bc34));
		}
		s = MATCH_F32(float_0p64_8041bc38);
		PSMTXScale(scale, s, s, s);
		PSMTXConcat(mtx, scale, mtx);
		FontDrawMessageMtx(mtx, buf);

		msg = msgSearch(item->name);
		FontDrawStart();
		if (pouch->field_0x5B4 & (1 << i)) {
			*(u32*)&color6 = dat_8041bc08;
			FontDrawColor(&color6);
		}
		else {
			*(u32*)&color7 = dat_8041bc0c;
			FontDrawColor(&color7);
		}
		FontDrawMessage(entry->x + entry->width / 2 - 10, entry->y - 20 - i * 26, msg);
	}
}

void title_disp(WinMgrEntry* entry) {
	if (!(entry->flags & 4)) {
		GXColor color;
		const char* msg = msgSearch("msg_shop_point_list4\0\0\0");

		FontDrawStart();
		FontDrawEdge();
		*(u32*)&color = dat_8041bbec;
		FontDrawColor(&color);
		FontDrawMessage(entry->x + (entry->width - FontGetMessageWidth(msg)) / 2, entry->y - 12, msg);
		FontDrawEdgeOff();
	}
}

void point_disp(WinMgrEntry* entry) {
	ShopWork* wp = (ShopWork*)_wp;
	PouchData* pouch = pouchGetPtr();

	if (!(entry->flags & 4)) {
		Mtx mtx;
		Mtx scale;
		s32 delta;

		PSMTXTrans(mtx, SHOP_F32(float_10_8041bc3c) + (SHOP_F32(float_30_8041bc40) + (f32)entry->x), (f32)entry->y - SHOP_F32(float_36_8041bc44), SHOP_F32(float_0_8041bc34));
		if ((u32)gp->language == 0) {
			PSMTXScale(scale, SHOP_F32(float_1p1_8041bc48), SHOP_F32(float_1p1_8041bc48), SHOP_F32(float_1p1_8041bc48));
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x1F1);
		}
		else {
			PSMTXScale(scale, SHOP_F32(float_0p8_8041bc4c), SHOP_F32(float_1p1_8041bc48), SHOP_F32(float_1p1_8041bc48));
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x20A);
		}
		PSMTXTrans(mtx, SHOP_F32(float_124_8041bc50) + (f32)entry->x, (f32)entry->y - SHOP_F32(float_38_8041bc54), SHOP_F32(float_0_8041bc34));
		iconNumberDispGx(mtx, wp->field_0x38, 0, *(GXColor*)&dat_8041bbe8);
		delta = pouch->mShopPoints - wp->field_0x38;
		if (delta != 0) {
			delta /= 10;
			if (delta == 0) {
				delta = (pouch->mShopPoints > wp->field_0x38) ? 1 : -1;
			}
			wp->field_0x38 += delta;
		}
	}
}

void help_disp(WinMgrEntry* entry) {
	ShopWork* wp = (ShopWork*)_wp;
	s32 offset = wp->field_0x2C * 8;
	s32* items = wp->field_0x8;

	if (!winMgrAction(wp->field_0x54)) {
		s32 item = *(s32*)((u8*)items + offset);
		entry->message = msgSearch(itemDataTable[item].description);
		winMgrHelpDraw(entry);
	}
}

void help_main(WinMgrEntry* entry) {
	winMgrHelpMain(entry);
}

void name_disp(WinMgrEntry* entry) {
	ShopWork* wp = (ShopWork*)_wp;
	GXColor titleColor;
	Vec pos;
	Mtx mtx;
	char buf[0x40];
	const char* msg;
	s32 i;

#ifdef MUST_MATCH
	// MUST_MATCH: reading _wp directly here (rather than through wp) makes the compiler
	// keep the pointer in a scratch register and copy it to wp's callee-saved register.
	msg = msgSearch(itemDataTable[((ShopWork*)_wp)->field_0x8[((ShopWork*)_wp)->field_0x2C * 2]].name);
#else
	msg = msgSearch(itemDataTable[wp->field_0x8[wp->field_0x2C * 2]].name);
#endif
	if (winMgrAction(wp->field_0x50) == 0) {
		FontDrawStart();
		FontDrawEdge();
		*(u32*)&titleColor = dat_8041bbe0;
		FontDrawColor(&titleColor);
		FontDrawString(msg, (f32)(-(s32)FontGetMessageWidth(msg) / 2), SHOP_F32(float_108_8041bc58));
		FontDrawEdgeOff();
	}
	for (i = 0; i < wp->field_0x20; i++) {
		if (wp->field_0x14[i] & 1) {
			continue;
		}
		mapObjGetPos(*(const char**)((u8*)wp->field_0x4 + i * 8), &pos);
		pos.y += SHOP_F32(float_30_8041bc40);
		PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
		PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
#ifdef MUST_MATCH
		// MUST_MATCH: the array form makes the scheduler emit the x scale before the y one,
		// which is the original's order; the named-field form emits y first.
		((f32*)&pos)[0] *= (f32)gp->fbWidth * SHOP_F32(float_0p5_8041bc5c);
		((f32*)&pos)[1] *= (f32)gp->efbHeight * SHOP_F32(float_0p5_8041bc5c);
#else
		pos.x *= (f32)gp->fbWidth * SHOP_F32(float_0p5_8041bc5c);
		pos.y *= (f32)gp->efbHeight * SHOP_F32(float_0p5_8041bc5c);
#endif
		sprintf(buf, str_8041BC60, wp->field_0x8[i * 2 + 1]);
		PSMTXTrans(mtx, SHOP_F32(float_12_8041bc64) * (f32)(strlen(buf) - 1) + pos.x, pos.y, pos.z);
		iconNumberDispGx(mtx, *(s32*)((u8*)wp->field_0x8 + i * 8 + 4), 0, *(GXColor*)&dat_8041bbe4);
	}
}

#undef SHOP_F32

// .data
WinMgrDesc window_desc[3] = {
    { {0, 0, 0, 0, 0, 0, 0, 0}, 8, 0xFFFFFF6A, 0x78, 0x12C, 0x32, {0x2B, 0xA4, 0xFF, 0xFF}, NULL, (void*)name_disp },
    { {0, 0, 0, 0, 0, 0, 0, 0}, 8, 0xFFFFFF10, 0xFFFFFF7E, 0x1F4, 0x50, {0xF0, 0xF0, 0xF0, 0xFF}, (void*)help_main, (void*)help_disp },
    { {0, 0, 0, 2, 0, 0, 0, 0}, 8, 0xFFFFFF06, 0xFFFFFF7E, 0x96, 0x2D, {0xF0, 0xF0, 0xF0, 0xFF}, NULL, (void*)point_disp },
};

ShopPointEntry shopPointList[11] = {
    { 0x0A, 0x94 }, { 0x14, 0x83 }, { 0x1E, 0x95 }, { 0x32, 0x81 }, { 0x46, 0x82 }, { 0x64, 0x7E },
    { 0x96, 0x97 }, { 0xC8, 0x96 }, { 0xFA, 0x9C }, { 0x12C, 0x7F }, { -1, -1 },
};

WinMgrDesc list_desc[3] = {
    { {0, 0, 0, 0, 0, 0, 0, 0}, 8, 0xFFFFFEE8, 0xA0, 0xD2, 0x50, {0x2B, 0xA4, 0xFF, 0xFF}, NULL, (void*)title_disp },
    { {0, 0, 0, 0, 0, 0, 0, 3}, 8, 0xFFFFFFC4, 0xA0, 0x154, 0x12C, {0xF0, 0xF0, 0xF0, 0xFF}, NULL, (void*)list_disp },
    { {0, 0, 0, 2, 0, 0, 0, 0}, 8, 0xFFFFFF06, 0xFFFFFF7E, 0x96, 0x2D, {0xF0, 0xF0, 0xF0, 0xFF}, NULL, (void*)point_disp },
};

u32 buy_evt[326] = {
    0x0001005B, (u32)(void*)evt_mario_normalize, 0x00020032, 0xFE363C8A, 0xFE363C80, 0x0002005B, (u32)disp_off, 0xFE363C8A,
    0x00020018, 0xF8405B80, 0x00000000, 0x00020032, 0xF8405B80, 0x00000001, 0x0001005E, (u32)(void*)first_shopping_evt,
    0x00000002, 0x00000021, 0x0002005B, (u32)shopper_name, 0xFE363C89, 0x0003005B, (u32)(void*)evt_set_dir_to_target, 0xFE363C89,
    (u32)(void*)str_8041BC10, 0x0003005B, (u32)(void*)evt_win_coin_on, 0x00000000, 0xFE363C88, 0x0002005B, (u32)point_disp_onoff, 0x00000001,
    0x0005005B, (u32)get_value, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0xFE363C8D, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000000, 0xFE363C8E, 0x0002005B, (u32)(void*)evt_sub_get_language, 0xFE363C80, 0x00020019, 0xFE363C80,
    0x00000000, 0x0007005B, (u32)(void*)evt_msg_print_insert, 0x00000000, 0xFE363C8E, 0x00000000, 0xFE363C89, 0xFE363C8D,
    0xFE363C8B, 0x00000020, 0x0007005B, (u32)(void*)evt_msg_print_insert, 0x00000000, 0xFE363C8E, 0x00000000, 0xFE363C89,
    0xFE363C8B, 0xFE363C8D, 0x00000021, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000018, 0xFE363C8E,
    0x0003005B, (u32)(void*)evt_msg_select, 0x00000000, 0xFE363C8E, 0x00020018, 0xFE363C80, 0x00000001, 0x0004005B,
    (u32)get_msg, 0xFE363C8A, 0x00000016, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E,
    0x0002005B, (u32)(void*)evt_win_coin_off, 0xFE363C88, 0x0002005B, (u32)point_disp_onoff, 0x00000000, 0x00000002, 0x00000021,
    0x0002005B, (u32)(void*)evt_pouch_get_coin, 0xFE363C80, 0x0002001A, 0xFE363C80, 0xFE363C8D, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000001, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x0002005B,
    (u32)(void*)evt_win_coin_off, 0xFE363C88, 0x0002005B, (u32)point_disp_onoff, 0x00000000, 0x00000002, 0x00000021, 0x0003005B,
    (u32)(void*)evt_pouch_add_item, 0xFE363C8C, 0xFE363C80, 0x00020018, 0xFE363C80, 0xFFFFFFFF, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000002, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x0002005B,
    (u32)(void*)evt_win_coin_off, 0xFE363C88, 0x0002005B, (u32)point_disp_onoff, 0x00000000, 0x00000002, 0x00000021, 0x0002005B,
    (u32)set_buy_item_id, 0xFE363C8C, 0x0003005B, (u32)(void*)evt_sub_load_progresstime, 0xF5DE0185, 0xFE363C80, 0x0003005B, (u32)(void*)evt_sub_load_progresstime,
    0xF5DE018E, 0xFE363C81, 0x0003005B, (u32)(void*)evt_sub_load_progresstime, 0xF5DE0197, 0xFE363C82, 0x0005005B, (u32)add_shop_point,
    0x00000001, 0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020037, 0xFE363C8D, 0xFFFFFFFF, 0x0002005B,
    (u32)(void*)evt_pouch_add_coin, 0xFE363C8D, 0x0002005B, (u32)(void*)evt_win_coin_wait, 0xFE363C88, 0x0001005B, (u32)point_wait, 0x0001000A,
    0x000000C8, 0x0002005B, (u32)(void*)evt_win_coin_off, 0xFE363C88, 0x0002005B, (u32)point_disp_onoff, 0x00000000, 0x0001005B,
    (u32)(void*)evt_msg_continue, 0x0002005B, (u32)(void*)evt_sub_get_system_flag, 0xFE363C80, 0x0002001F, 0xFE363C80, 0x00001000, 0x00020018,
    0xF8405C6B, 0x00000000, 0x00020032, 0xF8405C6B, 0x00000001, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000,
    (u32)(void*)str_802ECD50, 0x00000000, 0x00000000, 0x00000021, 0x00000021, 0x00010005, 0x00000000, 0x0004005B,
    (u32)chk_shop_point, 0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020018, 0xFE363C80, 0xFFFFFFFF, 0x00010004,
    0x0000007B, 0x00000002, 0x00000021, 0x00020032, 0xFBD3E280, 0x00000000, 0x0002005B, (u32)get_shop_point,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x0000012C, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x0000001A,
    0xFE363C8E, 0x0006005B, (u32)(void*)evt_msg_print_insert, 0x00000000, 0xFE363C8E, 0x00000000, 0xFE363C89, 0xFE363C82,
    0x00000020, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000022, 0xFE363C8E, 0x0005005B, (u32)(void*)evt_msg_print,
    0x00000000, 0xFE363C8E, 0x00000000, 0xFE363C89, 0x0003005B, (u32)(void*)evt_win_coin_on, 0x00000000, 0xFE363C88,
    0x0002005B, (u32)point_disp_onoff, 0x00000001, 0x0001005B, (u32)reset_shop_point, 0x0002005B, (u32)(void*)evt_win_coin_wait, 0xFE363C88,
    0x0001005B, (u32)point_wait, 0x0001000A, 0x000001F4, 0x0002005B, (u32)(void*)evt_win_coin_off, 0xFE363C88, 0x0002005B,
    (u32)point_disp_onoff, 0x00000000, 0x00020032, 0xFBD3E280, 0x00000001, 0x00000021, 0x0003005B, (u32)(void*)evt_pouch_add_item,
    0xFE363C81, 0xFE363C80, 0x00020018, 0xFE363C80, 0xFFFFFFFF, 0x0004005B, (u32)get_msg, 0xFE363C8A,
    0x0000001B, 0xFE363C8E, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, 0xFE363C89,
    0x00020018, 0xFBD3E280, 0x00000001, 0x00020032, 0xF8405B81, 0x00000001, 0x00000021, 0x00010004,
    0x0000007B, 0x00000002, 0x00000021, 0x0001005B, (u32)exchange_shop_point, 0x00000006, 0x00010003, 0x0000007B,
    0x0002005B, (u32)get_buy_evt, 0xFE363C81, 0x00020019, 0xFE363C81, 0x00000000, 0x00020032, 0xFE363C80,
    0xFE363C8C, 0x0001005E, 0xFE363C81, 0x00000021, 0x00000002, 0x00000001,
};

u32 shopper_talk_evt[352] = {
    0x0002005B, (u32)(void*)evt_mario_key_onoff, 0x00000000, 0x0002005B, (u32)get_ptr, 0xFE363C8A, 0x00020032, 0xFBD3E28A,
    0x00000000, 0x0002005B, (u32)get_fook_evt, 0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000, 0x0001005E,
    0xFE363C80, 0x00020018, 0xFE363C80, 0xFFFFFFFF, 0x0002005B, (u32)(void*)evt_mario_key_onoff, 0x00000001, 0x00000002,
    0x00000021, 0x00020018, 0xFE363C80, 0xFFFFFF9C, 0x00020032, 0xFBD3E28A, 0x00000001, 0x00000021,
    0x00000021, 0x00020018, 0xF8405B80, 0x00000000, 0x00020032, 0xF8405B80, 0x00000001, 0x0001005E,
    (u32)(void*)first_shopping_evt, 0x0002005B, (u32)(void*)evt_mario_key_onoff, 0x00000001, 0x00000002, 0x00000021, 0x00020032, 0xFBD3E280,
    0x00000000, 0x00010005, 0x00000000, 0x0004005B, (u32)chk_shop_point, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020018, 0xF8405B81, 0x00000001, 0x00020032, 0xFE363C80, 0x00000000, 0x0004005B, (u32)shop_point_item,
    0x0000012C, 0xFE363C81, 0xFE363C82, 0x00000021, 0x00020018, 0xFE363C80, 0xFFFFFFFF, 0x00020018,
    0xFBD3E280, 0x00000001, 0x0002005B, (u32)(void*)evt_mario_key_onoff, 0x00000001, 0x00000002, 0x00000021, 0x00020032,
    0xFBD3E28B, 0x00000000, 0x0002005B, (u32)(void*)evt_sub_get_mapname, 0xFE363C80, 0x0002000C, 0xFE363C80, (u32)(void*)str_802ECD60,
    0x00020018, 0xF8405C7A, 0x00000001, 0x0003005B, (u32)(void*)evt_sub_load_progresstime, 0xF5DE0185, 0xFE363C80, 0x0002001A,
    0xFE363C80, 0x000DBBA0, 0x00020032, 0xFE363C8E, (u32)(void*)str_802ECD68, 0x00020032, 0xFBD3E28B, 0x00000001,
    0x00000021, 0x00000021, 0x00000021, 0x0002005B, (u32)(void*)evt_sub_get_mapname, 0xFE363C80, 0x0002000C, 0xFE363C80,
    (u32)(void*)str_802ECD78, 0x00020018, 0xF8405C7B, 0x00000001, 0x0003005B, (u32)(void*)evt_sub_load_progresstime, 0xF5DE018E, 0xFE363C80,
    0x0002001A, 0xFE363C80, 0x000DBBA0, 0x00020032, 0xFE363C8E, (u32)(void*)str_802ECD80, 0x00020032, 0xFBD3E28B,
    0x00000001, 0x00000021, 0x00000021, 0x00000021, 0x00020032, 0xFBD3E28C, 0x00000000, 0x0002005B,
    (u32)(void*)evt_sub_get_mapname, 0xFE363C80, 0x0002000C, 0xFE363C80, (u32)(void*)str_802ECD94, 0x00020032, 0xFBD3E28C, 0x00000001,
    0x00000021, 0x0002000C, 0xFE363C80, (u32)(void*)str_802ECDA0, 0x00020032, 0xFBD3E28C, 0x00000001, 0x00000021,
    0x0002000C, 0xFE363C80, (u32)(void*)str_802ECDAC, 0x00020032, 0xFBD3E28C, 0x00000001, 0x00000021, 0x00020018,
    0xFBD3E28C, 0x00000001, 0x00020018, 0xF8405C7C, 0x00000001, 0x0003005B, (u32)(void*)evt_sub_load_progresstime, 0xF5DE0197,
    0xFE363C80, 0x0002001A, 0xFE363C80, 0x000DBBA0, 0x00020032, 0xFE363C8E, (u32)(void*)str_802ECDB8, 0x00020032,
    0xFBD3E28B, 0x00000001, 0x00000021, 0x00000021, 0x00000021, 0x00020018, 0xFBD3E28B, 0x00000000,
    0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000004, 0xFE363C8E, 0x00000021, 0x00020018, 0xFBD3E28A,
    0x00000000, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, 0x00000000, 0x00000020,
    0x00020032, 0xFBD3E28A, 0x00000000, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000001, (u32)(void*)str_802ECDC8, 0x0003005B,
    (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000021, 0x00000007, 0x00000021, 0x0003005B, (u32)(void*)evt_pouch_add_item,
    0xFE363C81, 0xFE363C80, 0x00020018, 0xFE363C80, 0xFFFFFFFF, 0x0004005B, (u32)get_msg, 0xFE363C8A,
    0x0000001D, 0xFE363C8E, 0x00020018, 0xFBD3E28A, 0x00000000, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000,
    0xFE363C8E, 0x00000000, 0x00000000, 0x00000020, 0x00020032, 0xFBD3E28A, 0x00000000, 0x0003005B,
    (u32)(void*)evt_msg_print_add, 0x00000001, (u32)(void*)str_802ECDC8, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000021,
    0x00000007, 0x00000021, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x0000001C, 0xFE363C8E, 0x00020018,
    0xFBD3E28A, 0x00000000, 0x0006005B, (u32)(void*)evt_msg_print_insert, 0x00000000, 0xFE363C8E, 0x00000000, 0x00000000,
    0xFE363C82, 0x00000020, 0x00020032, 0xFBD3E28A, 0x00000000, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000001,
    (u32)(void*)str_802ECDC8, 0x0004005B, (u32)(void*)evt_msg_print_add_insert, 0x00000000, 0xFE363C8E, 0xFE363C82, 0x00000021, 0x00020018,
    0xF8405B81, 0x00000000, 0x0001005B, (u32)exchange_shop_point, 0x00000021, 0x00020032, 0xFBD3E280, 0x00000001,
    0x00020018, 0xF8405B81, 0x00000001, 0x00020032, 0xF8405B81, 0x00000000, 0x00000021, 0x00000006,
    0x0003005B, (u32)shop_flag_onoff, 0x00000001, 0x00004000, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000017,
    0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_select, 0x00000000, 0xFE363C8E, 0x0003005B, (u32)shop_flag_onoff, 0x00000000,
    0x00004000, 0x00010022, 0xFE363C80, 0x00010024, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A,
    0x00000005, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00010024, 0x00000001,
    0x0001005E, (u32)(void*)sell_evt, 0x00010024, 0x00000002, 0x0001005E, (u32)(void*)keep_evt, 0x00010024, 0x00000003,
    0x0001005E, (u32)(void*)unkeep_evt, 0x00010024, 0x00000004, 0x0001005E, (u32)(void*)point_evt, 0x00010024, 0x00000005,
    0x0001005B, (u32)(void*)evt_msg_continue, 0x00000031, 0x0002005B, (u32)(void*)evt_mario_key_onoff, 0x00000001, 0x00000002, 0x00000001,
};

u32 first_shopping_evt[86] = {
    0x0002005B, (u32)shopper_name, 0xFE363C89, 0x0003005B, (u32)(void*)evt_set_dir_to_target, 0xFE363C89, (u32)(void*)str_8041BC10, 0x0002005B,
    (u32)(void*)evt_mario_set_dir_npc, 0xFE363C89, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x0000001E, 0xFE363C8E, 0x00020018,
    0xFBD3E28A, 0x00000000, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, 0xFE363C89,
    0x00000020, 0x00020032, 0xFBD3E28A, 0x00000000, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000001, (u32)(void*)str_802ECDC8,
    0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000021, 0x00010005, 0x00000000, 0x0004005B,
    (u32)get_msg, 0xFE363C8A, 0x0000001F, 0xFE363C8E, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E,
    0x00000000, 0xFE363C89, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000018, 0xFE363C8E, 0x0003005B,
    (u32)(void*)evt_msg_select, 0x00000000, 0xFE363C8E, 0x00020018, 0xFE363C80, 0x00000000, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000020, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000020,
    0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000021, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000,
    0xFE363C8E, 0x00000002, 0x00000021, 0x00000006, 0x00000002, 0x00000001,
};

static u32 sell_evt[196] = {
    0x0001005B, (u32)sell_pouchcheck_func, 0x00020018, 0xFE363C80, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A,
    0x00000006, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000002, 0x00000021,
    0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000007, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000,
    0xFE363C8E, 0x0003005B, (u32)(void*)evt_win_coin_on, 0x00000000, 0xFE363C88, 0x00010003, 0x00000000, 0x0001005B,
    (u32)item_data_db_arrange, 0x0005005B, (u32)(void*)evt_win_item_select, 0x00000001, 0x00000003, 0xFE363C81, 0xFE363C84, 0x0001005B,
    (u32)item_data_db_restore, 0x0002001C, 0xFE363C81, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000016,
    0xFE363C8E, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, (u32)(void*)str_8041BC18, 0x0002005B,
    (u32)(void*)evt_win_coin_off, 0xFE363C88, 0x00000002, 0x00000021, 0x0001005B, (u32)item_data_db_arrange, 0x0004005B, (u32)name_price,
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x0001005B, (u32)item_data_db_restore, 0x0004005B, (u32)get_msg, 0xFE363C8A,
    0x00000008, 0xFE363C8E, 0x0002005B, (u32)(void*)evt_sub_get_language, 0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0007005B, (u32)(void*)evt_msg_print_insert, 0x00000000, 0xFE363C8E, 0x00000000, (u32)(void*)str_8041BC18, 0xFE363C83, 0xFE363C82,
    0x00000020, 0x0007005B, (u32)(void*)evt_msg_print_insert, 0x00000000, 0xFE363C8E, 0x00000000, (u32)(void*)str_8041BC18, 0xFE363C82,
    0xFE363C83, 0x00000021, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000018, 0xFE363C8E, 0x0003005B,
    (u32)(void*)evt_msg_select, 0x00000000, 0xFE363C8E, 0x00020018, 0xFE363C80, 0x00000001, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000009, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00010004,
    0x00000000, 0x00000021, 0x0004005B, (u32)(void*)N_evt_pouch_remove_item_index, 0xFE363C81, 0xFE363C84, 0xFE363C80, 0x0002005B,
    (u32)(void*)evt_pouch_add_coin, 0xFE363C83, 0x0002005B, (u32)(void*)evt_win_coin_wait, 0xFE363C88, 0x0001000A, 0x000000C8, 0x0002005B,
    (u32)(void*)evt_win_coin_off, 0xFE363C88, 0x0001005B, (u32)sell_pouchcheck_func, 0x00020018, 0xFE363C80, 0x00000000, 0x0004005B,
    (u32)get_msg, 0xFE363C8A, 0x0000000B, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E,
    0x00000002, 0x00000021, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x0000000A, 0xFE363C8E, 0x0003005B,
    (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000018, 0xFE363C8E,
    0x0003005B, (u32)(void*)evt_msg_select, 0x00000000, 0xFE363C8E, 0x00020018, 0xFE363C80, 0x00000001, 0x0004005B,
    (u32)get_msg, 0xFE363C8A, 0x00000016, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E,
    0x00000002, 0x00000021, 0x0001005B, (u32)(void*)evt_msg_continue, 0x0003005B, (u32)(void*)evt_win_coin_on, 0x00000000, 0xFE363C88,
    0x00010004, 0x00000000, 0x00000002, 0x00000001,
};

static u32 keep_evt[148] = {
    0x0001005B, (u32)keep_pouchcheck_func, 0x00020018, 0xFE363C80, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A,
    0x0000000C, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000002, 0x00000021,
    0x00020018, 0xFE363C81, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x0000000D, 0xFE363C8E,
    0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000002, 0x00000021, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x0000000E, 0xFE363C8E, 0x0004005B, (u32)(void*)evt_msg_print_add_insert, 0x00000000, 0xFE363C8E, 0xFE363C81,
    0x00010003, 0x00000000, 0x0005005B, (u32)(void*)evt_win_item_select, 0x00000001, 0x00000004, 0xFE363C81, 0xFE363C84,
    0x0002001C, 0xFE363C81, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000016, 0xFE363C8E,
    0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, (u32)(void*)str_8041BC18, 0x00000002, 0x00000021,
    0x0004005B, (u32)(void*)N_evt_pouch_remove_item_index, 0xFE363C81, 0xFE363C84, 0xFE363C80, 0x0003005B, (u32)(void*)evt_pouch_add_keepitem, 0xFE363C81,
    0xFE363C80, 0x0001005B, (u32)keep_pouchcheck_func, 0x00020032, 0xFBD3E280, 0x00000000, 0x00020018, 0xFE363C80,
    0x00000000, 0x00020032, 0xFBD3E280, 0x00000001, 0x00000021, 0x00020018, 0xFE363C81, 0x00000000,
    0x00020032, 0xFBD3E280, 0x00000001, 0x00000021, 0x00020018, 0xFBD3E280, 0x00000001, 0x0004005B,
    (u32)get_msg, 0xFE363C8A, 0x0000000F, 0xFE363C8E, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E,
    0x00000000, (u32)(void*)str_8041BC18, 0x00000002, 0x00000021, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000010,
    0xFE363C8E, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, (u32)(void*)str_8041BC18, 0x0004005B,
    (u32)get_msg, 0xFE363C8A, 0x00000018, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_select, 0x00000000, 0xFE363C8E,
    0x00020018, 0xFE363C80, 0x00000001, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000016, 0xFE363C8E,
    0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000002, 0x00000021, 0x0001005B, (u32)(void*)evt_msg_continue,
    0x00010004, 0x00000000, 0x00000002, 0x00000001,
};

static u32 unkeep_evt[147] = {
    0x0001005B, (u32)unkeep_pouchcheck_func, 0x00020018, 0xFE363C80, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A,
    0x00000011, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000002, 0x00000021,
    0x00020018, 0xFE363C81, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000012, 0xFE363C8E,
    0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000002, 0x00000021, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000013, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00010003,
    0x00000000, 0x0005005B, (u32)(void*)evt_win_item_select, 0x00000001, 0x00000005, 0xFE363C81, 0xFE363C84, 0x0002001C,
    0xFE363C81, 0x00000000, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000016, 0xFE363C8E, 0x0005005B,
    (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, (u32)(void*)str_8041BC18, 0x00000002, 0x00000021, 0x0004005B,
    (u32)(void*)evt_pouch_remove_keepitem, 0xFE363C81, 0xFE363C84, 0xFE363C80, 0x0003005B, (u32)(void*)evt_pouch_add_item, 0xFE363C81, 0xFE363C80,
    0x0001005B, (u32)unkeep_pouchcheck_func, 0x00020032, 0xFBD3E280, 0x00000000, 0x00020018, 0xFE363C80, 0x00000000,
    0x00020032, 0xFBD3E280, 0x00000001, 0x00000021, 0x00020018, 0xFE363C81, 0x00000000, 0x00020032,
    0xFBD3E280, 0x00000001, 0x00000021, 0x00020018, 0xFBD3E280, 0x00000001, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000014, 0xFE363C8E, 0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000,
    (u32)(void*)str_8041BC18, 0x00000002, 0x00000021, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000015, 0xFE363C8E,
    0x0005005B, (u32)(void*)evt_msg_print, 0x00000000, 0xFE363C8E, 0x00000000, (u32)(void*)str_8041BC18, 0x0004005B, (u32)get_msg,
    0xFE363C8A, 0x00000018, 0xFE363C8E, 0x0003005B, (u32)(void*)evt_msg_select, 0x00000000, 0xFE363C8E, 0x00020018,
    0xFE363C80, 0x00000001, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000016, 0xFE363C8E, 0x0003005B,
    (u32)(void*)evt_msg_print_add, 0x00000000, 0xFE363C8E, 0x00000002, 0x00000021, 0x0001005B, (u32)(void*)evt_msg_continue, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};

static u32 point_evt[15] = {
    0x0002005B, (u32)get_shop_point, 0xFE363C80, 0x0004005B, (u32)get_msg, 0xFE363C8A, 0x00000019, 0xFE363C8E,
    0x0004005B, (u32)(void*)evt_msg_print_add_insert, 0x00000000, 0xFE363C8E, 0xFE363C80, 0x00000002, 0x00000001,
};

// .sdata
u32 evt_shoplist[10] = {
    0x0002005B, (u32)(void*)evt_mario_key_onoff, 0x00000000, 0x0001005B, (u32)disp_list, 0x0002005B, (u32)(void*)evt_mario_key_onoff, 0x00000001,
    0x00000002, 0x00000001,
};

u32 main_evt[4] = {
    0x0001005B, (u32)evt_shop_main_func, 0x00000002, 0x00000001,
};

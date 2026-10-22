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
extern s32 first_shopping_evt[];

// This file's own script, defined below.
extern s32 buy_evt[];

// The remaining scripts are file-local (their asm `.obj` blocks are `local`).
static s32 sell_evt[196];
static s32 keep_evt[148];
static s32 unkeep_evt[147];
static s32 point_evt[15];

extern GlobalWork* gp;

//.sbss (uninitialised globals are laid out last-declared-first)
u8 gap_08_80418944_sbss[4];
static struct ShopWork* _wp;
char mapname[16];

typedef struct ItemDbEntry {
	s32 id;               //0x0
	s16 altSellPrice;     //0x4, sell price written into itemDataTable while the shop is open
	s16 savedSellPrice;   //0x6, the original sell price, restored by item_data_db_restore
} ItemDbEntry;

typedef struct ShopperData {
	const char* name;        //0x0
	s32 fookEvt;             //0x4
	s32 buyEvt;              //0x8
	const char* field_0xC[]; //0xC, message keys get_msg returns by index
} ShopperData;

typedef struct ShopWork {
	u16 flags;                    //0x0, bit 0: shop UI on (disp_off clears it), bit 1: purchase event running
	u16 pad_02;                   //0x2
	ShopObj* objs;                //0x4, one per shop object: map object name and hit name
	s32* items;                   //0x8, item id / price pairs
	ShopperData* shopper;         //0xC
	ItemDbEntry* itemDbOverrides; //0x10, id / sell price table applied by item_data_db_arrange
	u16 acquired[6];              //0x14, bit 0 set once the item is held or bought
	s32 numItems;                 //0x20
	EventEntry* mainEvt;          //0x24
	EventEntry* buyEvt;           //0x28
	s32 selectedItem;             //0x2C, selected item index
	u8 pad_0x30[0x38 - 0x30];     //0x30
	s32 shownPoints;              //0x38, shop points as drawn by point_disp
	u8 pad_0x3C[0x50 - 0x3C];
	s32 nameWin;                  //0x50, window_desc[0]: item name
	s32 helpWin;                  //0x54, window_desc[1]: item description
	s32 pointsWin;                //0x58, window_desc[2]: shop points
	u8 pad_0x5C[0xD0 - 0x5C];
	s32 openIdx;                  //0xD0, item whose windows are open, -1 when none
	s32 buyItemId;                //0xD4
} ShopWork;

typedef struct ShopPointEntry {
	s32 price; //0x0
	s32 item;  //0x4
} ShopPointEntry;

extern ItemData itemDataTable[];
extern ShopPointEntry shopPointList[];
extern const char str_item_first_hit[];
extern const char str_msg_shop_point_list4[];
extern u32 shop_list_title_color;
extern u32 shop_points_color;
extern u32 shop_item_name_color;
extern u32 shop_price_color;
extern const char str_fmt_d[];
extern const f32 float_0p5_8041bc5c;
extern const f32 shop_price_digit_width;
extern const f32 shop_item_name_y;
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
extern const char shop_icon_name_fmt[6];
extern const f32 shop_icon_y_offset;
extern s32 shopper_talk_evt[];
extern s32 main_evt[4];
extern s32 evt_shoplist[10];
extern const char str_fmt_s[];
extern u32 shop_list_points_head_color;
extern u32 shop_list_item_head_color;
extern u32 shop_list_price_exchanged_color;
extern u32 shop_list_price_color;
extern u32 shop_list_unit_exchanged_color;
extern u32 shop_list_unit_color;
extern u32 shop_list_item_exchanged_color;
extern u32 shop_list_item_color;
extern const f32 float_0p6_8041bc2c;
extern const f32 float_40_8041bc30;
extern const f32 float_0p64_8041bc38;

// Data, in the original object's order (build/G8MJ01/asm/event/evt_shop.s).
// Named objects precede the real functions so immediate generation preserves their storage.

// .rodata
const char str_item_first_hit[16] = "item_first_hit";
const char shop_map_mri_07[8] = "mri_07";
const char shop_msg_stg2_shop_04_1[16] = "stg2_shop_04_1";
const char shop_map_usu_01[8] = "usu_01";
const char shop_msg_usugurashop_04_1[20] = "usugurashop_04_1";
const char shop_map_rsh_03_a[12] = "rsh_03_a";
const char shop_map_rsh_03_b[12] = "rsh_03_b";
const char shop_map_rsh_03_c[12] = "rsh_03_c";
const char shop_msg_stg6_shop_04_1[16] = "stg6_shop_04_1";
const char shop_msg_scroll_once_stop[24] = "<scrl_auto><once_stop>";
const char shop_map_rsh_03[8] = "rsh_03";

// .sdata2
__declspec(section ".sdata2") u32 shop_item_name_color = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 shop_price_color = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 shop_points_color = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 shop_list_title_color = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 shop_list_points_head_color = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 shop_list_item_head_color = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 shop_list_price_exchanged_color = 0xF00000FF;
__declspec(section ".sdata2") u32 shop_list_price_color = 0x000000FF;
__declspec(section ".sdata2") u32 shop_list_unit_exchanged_color = 0xF00000FF;
__declspec(section ".sdata2") u32 shop_list_unit_color = 0x000000FF;
__declspec(section ".sdata2") u32 shop_list_item_exchanged_color = 0xF00000FF;
__declspec(section ".sdata2") u32 shop_list_item_color = 0x000000FF;
__declspec(section ".sdata2") const char str_8041BC10[8] = "mario";
__declspec(section ".sdata2") const char str_me[4] = "me";
__declspec(section ".sdata2") const char shop_icon_name_fmt[6] = "shp%d";
__declspec(section ".sdata2") const f32 shop_icon_y_offset = 6.0f;
__declspec(section ".sdata2") const char str_fmt_s[4] = "%s";
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
__declspec(section ".sdata2") const f32 shop_item_name_y = 108.0f;
__declspec(section ".sdata2") const f32 float_0p5_8041bc5c = 0.5f;
__declspec(section ".sdata2") const char str_fmt_d[4] = "%d";
__declspec(section ".sdata2") const f32 shop_price_digit_width = 12.0f;

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

	if (strcmp(gp->currentMapName, mapname) == 0 && _wp->flags != 0) {
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
	HitEntry* hit = mario->hitobjStandOn;
	s32 buttons;
	s32 i;

	wp->flags &= ~1;
	buttons = keyGetButtonTrg(0);
	{
#ifdef MUST_MATCH
		// The comma stores notKey before testing it, as retail does; the plain short-circuit
		// materialises the second compare instead of branching on it.
		BOOL notCtrl = (marioChkCtrl() == 0);
		BOOL notKey;

		if (notCtrl || (notKey = (marioChkKey() == 0), notKey)) {
			buttons = 0;
		}
#else
		if (!marioChkCtrl() || !marioChkKey()) {
			buttons = 0;
		}
#endif
	}

	if (strncmp(gp->currentMapName, shop_map_rsh_03, strlen(shop_map_rsh_03)) == 0) {
		if (evtGetValue(event, 0xF5DE0180) >= 0x148 && evtGetValue(event, 0xF5DE0180) < 0x14C &&
		    evtGetValue(event, 0xF84068F2) == 0) {
			return 0;
		}
	}

	if (party != NULL && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
		if (wp->openIdx != -1) {
			wp->openIdx = -1;
			winMgrClose(wp->nameWin);
			winMgrClose(wp->helpWin);
		}
		return 0;
	}

	if (winGhostDiaryChk() || N_itemPickUpFromFieldCheck()) {
		if (wp->openIdx != -1) {
			wp->openIdx = -1;
			winMgrClose(wp->nameWin);
			winMgrClose(wp->helpWin);
		}
		return 0;
	}

	if (!(wp->flags & 2)) {
		if (hit != NULL) {
			for (i = 0; i < wp->numItems; i++) {
				if (strcmp(wp->objs[i].hitName, hitGetName(hit)) == 0) {
					break;
				}
			}
			if (!(wp->acquired[i] & 1) && i < wp->numItems) {
				if (wp->openIdx != i) {
					if (wp->openIdx == -1) {
						winMgrOpen(wp->nameWin);
						winMgrOpen(wp->helpWin);
					}
					wp->openIdx = i;
					wp->selectedItem = i;
					winMgrHelpInit(winMgrGetPtr(wp->helpWin));
				}
				selected = 1;
			}
		}
		if (selected == 0 && wp->openIdx != -1) {
			wp->openIdx = -1;
			winMgrClose(wp->nameWin);
			winMgrClose(wp->helpWin);
		}
		if (wp->openIdx != -1) {
			wp->flags |= 1;
			if (buttons & 0x100) {
				marioKeyOff();
				wp->buyEvt = evtEntry(buy_evt, 0, 0);
				wp->buyEvt->lwData[0] = (s32)wp;
				wp->flags |= 2;
				winMgrClose(wp->nameWin);
				winMgrClose(wp->helpWin);
				wp->openIdx = -1;
				wp->buyItemId = 0;
			}
		}
	} else {
		if (evtCheckID(wp->buyEvt->eventId) == 0) {
			wp->flags &= ~2;
			marioKeyOn();
			if (wp->buyItemId == ITEM_CONTACT_LENS) {
				wp->acquired[wp->selectedItem] |= 1;
			}
			if (wp->buyItemId == ITEM_SUPER_LUIGI1) {
				wp->acquired[wp->selectedItem] |= 1;
			}
			if (wp->buyItemId == ITEM_SUPER_LUIGI2) {
				wp->acquired[wp->selectedItem] |= 1;
			}
			if (wp->buyItemId == ITEM_SUPER_LUIGI3) {
				wp->acquired[wp->selectedItem] |= 1;
			}
			if (wp->buyItemId == ITEM_SUPER_LUIGI4) {
				wp->acquired[wp->selectedItem] |= 1;
			}
			if (wp->buyItemId == ITEM_SUPER_LUIGI5) {
				wp->acquired[wp->selectedItem] |= 1;
			}
		}
	}

	for (i = 0; i < wp->numItems; i++) {
		MapObject* obj = mapGetMapObj(wp->objs[i].name);
		if (obj != NULL) {
			char name[32];

			sprintf(name, shop_icon_name_fmt, i);
			if (wp->acquired[i] & 1) {
				iconFlagOn(name, 2);
			} else if (obj->flags & 1) {
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
	ItemDbEntry* p = _wp->itemDbOverrides;
	s32 id;

	if (p != NULL) {
		while ((id = p->id) != 0) {
			itemDataTable[id].sell_price = p->savedSellPrice;
			p++;
		}
	}
	return EVT_RETURN_DONE;
}

s32 item_data_db_arrange(void) {
	ItemDbEntry* p = _wp->itemDbOverrides;
	s32 id;

	if (p != NULL) {
		while ((id = p->id) != 0) {
			p->savedSellPrice = itemDataTable[id].sell_price;
			itemDataTable[p->id].sell_price = p->altSellPrice;
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
	evtSetValue(event, args[2], itemDataTable[id].sell_price);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_fook_evt) {
	evtSetValue(event, event->args[0], _wp->shopper->fookEvt);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_ptr) {
	evtSetValue(event, event->args[0], (s32)_wp);
	return EVT_RETURN_DONE;
}

USER_FUNC(set_buy_item_id) {
	_wp->buyItemId = evtGetValue(event, event->args[0]);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_buy_evt) {
	evtSetValue(event, event->args[0], _wp->shopper->buyEvt);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_msg) {
	s32* args = event->args;
	ShopWork* wp = (ShopWork*)evtGetValue(event, args[0]);
	s32 idx = evtGetValue(event, args[1]);
	s32 value;

	value = (s32)wp->shopper->field_0xC[idx];

	evtSetValue(event, args[2], value);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_value) {
	s32* args = event->args;
	ShopWork* wp = (ShopWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)msgSearch(itemDataTable[wp->items[wp->selectedItem * 2]].name));
	evtSetValue(event, args[2], wp->items[wp->selectedItem * 2]);
	evtSetValue(event, args[3], wp->items[wp->selectedItem * 2 + 1]);
	return EVT_RETURN_DONE;
}

USER_FUNC(disp_off) {
	ShopWork* wp = (ShopWork*)evtGetValue(event, event->args[0]);

	wp->flags &= ~1;
	return EVT_RETURN_DONE;
}

USER_FUNC(shopper_name) {
	evtSetValue(event, event->args[0], (s32)_wp->shopper->name);
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
		if (price <= pouch->mShopPoints && !(pouch->shopExchangedMask & (1 << i))) {
			pouch->shopExchangedMask |= 1 << i;
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
		if (pouch->shopExchangedMask & (1 << i)) {
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
	pouch->shopExchangedMask = 0;
	return EVT_RETURN_DONE;
}

USER_FUNC(add_shop_point) {
	const char* const str = str_item_first_hit;
	s32* args = event->args;
	s32 v0 = evtGetValue(event, args[0]);
	s32 v1 = evtGetValue(event, args[1]);
	s32 v2 = evtGetValue(event, args[2]);
	s32 v3 = evtGetValue(event, args[3]);
	PouchData* pouch = pouchGetPtr();

	if (strcmp(gp->currentMapName, str + 0x10) == 0 && swGet(0xFA) && v1 < 900000) {
		v0 *= 2;
	}
	if (strcmp(gp->currentMapName, str + 0x28) == 0 && swGet(0xFB) && v2 < 900000) {
		v0 *= 2;
	}
	if (strncmp(gp->currentMapName, str + 0x90, 6) == 0 && swGet(0xFC) && v3 < 900000) {
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
	ShopWork* wp = _wp;

	if (a != 0) {
		wp->flags |= b;
	}
	else {
		wp->flags &= ~b;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(point_wait) {
	ShopWork* wp = _wp;
	PouchData* pouch = pouchGetPtr();

	if (wp->shownPoints != pouch->mShopPoints) {
		psndSFXOn((const char*)0x8CA);
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(point_disp_onoff) {
	s32 value = evtGetValue(event, event->args[0]);
	PouchData* pouch = pouchGetPtr();
	ShopWork* wp = _wp;

	wp->shownPoints = pouch->mShopPoints;
	if (value != 0) {
		winMgrOpen(wp->pointsWin);
	}
	else {
		winMgrClose(wp->pointsWin);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_shop_setup) {
	s32* args;
	ShopObj* v0;
	s32* v1;
	ShopperData* v2;
	ItemDbEntry* v3;
	NpcEntry* npc;
	s32 i;
	ShopWork* wp;

	args = event->args;
	v0 = (ShopObj*)evtGetValue(event, args[0]);
	v1 = (s32*)evtGetValue(event, args[1]);
	v2 = (ShopperData*)evtGetValue(event, args[2]);
	v3 = (ItemDbEntry*)evtGetValue(event, args[3]);

	wp = _mapAlloc(0xD8);
	memset(wp, 0, 0xD8);
	_wp = wp;
	strncpy(mapname, gp->currentMapName, 15);
	wp->objs = v0;
	wp->items = v1;
	wp->shopper = v2;
	wp->itemDbOverrides = v3;
	wp->numItems = 6;

	for (i = 0; i < wp->numItems; i++) {
		Vec pos;
		char buf[0x20];

		mapObjGetPos(wp->objs[i].name, &pos);
		mapObjFlagOn(wp->objs[i].name, 0x4000);
		sprintf(buf, shop_icon_name_fmt, i);
		iconEntry(buf, itemDataTable[wp->items[i * 2]].icon_id);
		iconSetPos(buf, pos.x, SHOP_F32(shop_icon_y_offset) + pos.y, pos.z);
		iconFlagOn(buf, 0x40);
		wp->acquired[i] = 0;
		if (wp->items[i * 2] == ITEM_CONTACT_LENS && pouchCheckItem(ITEM_CONTACT_LENS) != 0) {
			wp->acquired[i] |= 1;
		}
		if (wp->items[i * 2] == ITEM_SUPER_LUIGI1 && pouchCheckItem(ITEM_SUPER_LUIGI1) != 0) {
			wp->acquired[i] |= 1;
		}
		if (wp->items[i * 2] == ITEM_SUPER_LUIGI2 && pouchCheckItem(ITEM_SUPER_LUIGI2) != 0) {
			wp->acquired[i] |= 1;
		}
		if (wp->items[i * 2] == ITEM_SUPER_LUIGI3 && pouchCheckItem(ITEM_SUPER_LUIGI3) != 0) {
			wp->acquired[i] |= 1;
		}
		if (wp->items[i * 2] == ITEM_SUPER_LUIGI4 && pouchCheckItem(ITEM_SUPER_LUIGI4) != 0) {
			wp->acquired[i] |= 1;
		}
		if (wp->items[i * 2] == ITEM_SUPER_LUIGI5 && pouchCheckItem(ITEM_SUPER_LUIGI5) != 0) {
			wp->acquired[i] |= 1;
		}
	}
	npc = npcNameToPtr(v2->name);
	npc->talkEvt = shopper_talk_evt;
	wp->mainEvt = evtEntryType(main_evt, 0, 0x20, 0);
	wp->mainEvt->lwData[0] = (s32)wp;
	for (i = 0; i < 3; i++) {
		(&wp->nameWin)[i] = winMgrEntry(&window_desc[i]);
	}
	wp->openIdx = -1;
	evtRunCaseEntry(9, 1, (s32)"a_shop_keihin", 0, (s32*)evt_shoplist, 0);
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

	msg = msgSearch("msg_shop_point_list5");
	FontDrawStart();
	FontDrawEdge();
	*(u32*)&color0 = shop_list_points_head_color;
	FontDrawColor(&color0);
	FontDrawMessage((100 - FontGetMessageWidth(msg)) / 2 + (10 + entry->x), entry->y + 13, msg);
	FontDrawEdgeOff();

	msg = msgSearch("msg_shop_point_list6");
	FontDrawStart();
	FontDrawEdge();
	*(u32*)&color1 = shop_list_item_head_color;
	FontDrawColor(&color1);
	FontDrawMessage(entry->x + entry->width / 2 - 10 + (100 - FontGetMessageWidth(msg)) / 2, entry->y + 13, msg);
	FontDrawEdgeOff();

	FontDrawStart();
	for (; shopPointList[i].price != -1; i++) {
		item = &itemDataTable[shopPointList[i].item];
		FontDrawStart();
		if (pouch->shopExchangedMask & (1 << i)) {
			*(u32*)&color2 = shop_list_price_exchanged_color;
			FontDrawColor(&color2);
		}
		else {
			*(u32*)&color3 = shop_list_price_color;
			FontDrawColor(&color3);
		}
		sprintf(buf, str_fmt_s, winZenkakuStr(shopPointList[i].price));
		FontDrawMessage(entry->width / 2 - FontGetMessageWidth(buf) + entry->x - 90, entry->y - 20 - i * 26, buf);

		FontDrawStart();
		if (pouch->shopExchangedMask & (1 << i)) {
			*(u32*)&color4 = shop_list_unit_exchanged_color;
			FontDrawColor(&color4);
		}
		else {
			*(u32*)&color5 = shop_list_unit_color;
			FontDrawColor(&color5);
		}
		sprintf(buf, str_fmt_s, msgSearch("msg_shop_point_list5"));
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
		if (pouch->shopExchangedMask & (1 << i)) {
			*(u32*)&color6 = shop_list_item_exchanged_color;
			FontDrawColor(&color6);
		}
		else {
			*(u32*)&color7 = shop_list_item_color;
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
		*(u32*)&color = shop_list_title_color;
		FontDrawColor(&color);
		FontDrawMessage(entry->x + (entry->width - FontGetMessageWidth(msg)) / 2, entry->y - 12, msg);
		FontDrawEdgeOff();
	}
}

void point_disp(WinMgrEntry* entry) {
	ShopWork* wp = _wp;
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
		iconNumberDispGx(mtx, wp->shownPoints, 0, *(GXColor*)&shop_points_color);
		delta = pouch->mShopPoints - wp->shownPoints;
		if (delta != 0) {
			delta /= 10;
			if (delta == 0) {
				delta = (pouch->mShopPoints > wp->shownPoints) ? 1 : -1;
			}
			wp->shownPoints += delta;
		}
	}
}

void help_disp(WinMgrEntry* entry) {
	ShopWork* wp = _wp;
	s32 offset = wp->selectedItem * 8;
	s32* items = wp->items;

	if (!winMgrAction(wp->helpWin)) {
		s32 item = *(s32*)((u8*)items + offset);
		entry->message = msgSearch(itemDataTable[item].description);
		winMgrHelpDraw(entry);
	}
}

void help_main(WinMgrEntry* entry) {
	winMgrHelpMain(entry);
}

void name_disp(WinMgrEntry* entry) {
	ShopWork* wp = _wp;
	GXColor titleColor;
	Vec pos;
	Mtx mtx;
	char buf[0x40];
	const char* msg;
	s32 i;

	msg = msgSearch(itemDataTable[_wp->items[_wp->selectedItem * 2]].name);
	if (winMgrAction(wp->nameWin) == 0) {
		FontDrawStart();
		FontDrawEdge();
		*(u32*)&titleColor = shop_item_name_color;
		FontDrawColor(&titleColor);
		FontDrawString((f32)(-(s32)FontGetMessageWidth(msg) / 2), SHOP_F32(shop_item_name_y), msg);
		FontDrawEdgeOff();
	}
	for (i = 0; i < wp->numItems; i++) {
		if (wp->acquired[i] & 1) {
			continue;
		}
		mapObjGetPos(wp->objs[i].name, &pos);
		pos.y += SHOP_F32(float_30_8041bc40);
		PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
		PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
		pos.x *= (f32)gp->fbWidth * SHOP_F32(float_0p5_8041bc5c);
		pos.y *= (f32)gp->efbHeight * SHOP_F32(float_0p5_8041bc5c);
		sprintf(buf, str_fmt_d, wp->items[i * 2 + 1]);
		PSMTXTrans(mtx, SHOP_F32(shop_price_digit_width) * (f32)(strlen(buf) - 1) + pos.x, pos.y, pos.z);
		iconNumberDispGx(mtx, wp->items[i * 2 + 1], 0, *(GXColor*)&shop_price_color);
	}
}

#undef SHOP_F32

// .data
WinMgrDesc window_desc[3] = {
    { 0, 0, 8, 0xFFFFFF6A, 0x78, 0x12C, 0x32, {0x2B, 0xA4, 0xFF, 0xFF}, NULL, (void*)name_disp },
    { 0, 0, 8, 0xFFFFFF10, 0xFFFFFF7E, 0x1F4, 0x50, {0xF0, 0xF0, 0xF0, 0xFF}, (void*)help_main, (void*)help_disp },
    { 2, 0, 8, 0xFFFFFF06, 0xFFFFFF7E, 0x96, 0x2D, {0xF0, 0xF0, 0xF0, 0xFF}, NULL, (void*)point_disp },
};

ShopPointEntry shopPointList[11] = {
    { 0x0A, 0x94 }, { 0x14, 0x83 }, { 0x1E, 0x95 }, { 0x32, 0x81 }, { 0x46, 0x82 }, { 0x64, 0x7E },
    { 0x96, 0x97 }, { 0xC8, 0x96 }, { 0xFA, 0x9C }, { 0x12C, 0x7F }, { -1, -1 },
};

WinMgrDesc list_desc[3] = {
    { 0, 0, 8, 0xFFFFFEE8, 0xA0, 0xD2, 0x50, {0x2B, 0xA4, 0xFF, 0xFF}, NULL, (void*)title_disp },
    { 0, 3, 8, 0xFFFFFFC4, 0xA0, 0x154, 0x12C, {0xF0, 0xF0, 0xF0, 0xFF}, NULL, (void*)list_disp },
    { 2, 0, 8, 0xFFFFFF06, 0xFFFFFF7E, 0x96, 0x2D, {0xF0, 0xF0, 0xF0, 0xFF}, NULL, (void*)point_disp },
};

EVT_BEGIN(buy_evt)
    CALL(evt_mario_normalize)
    SET_VALUE(LW(10), LW(0))
    CALL(disp_off, LW(10))
    IF_INT_EQUAL(GSWF(0), 0)
        SET_VALUE(GSWF(0), 1)
        CALL_CHILD(first_shopping_evt)
        RETURN
    END_IF
    CALL(shopper_name, LW(9))
    CALL(evt_set_dir_to_target, LW(9), PTR((char*)str_8041BC10))
    CALL(evt_win_coin_on, 0, LW(8))
    CALL(point_disp_onoff, 1)
    CALL(get_value, LW(10), LW(11), LW(12), LW(13))
    CALL(get_msg, LW(10), 0, LW(14))
    CALL(evt_sub_get_language, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_insert, 0, LW(14), 0, LW(9), LW(13), LW(11))
    ELSE
        CALL(evt_msg_print_insert, 0, LW(14), 0, LW(9), LW(11), LW(13))
    END_IF
    CALL(get_msg, LW(10), 24, LW(14))
    CALL(evt_msg_select, 0, LW(14))
    IF_INT_EQUAL(LW(0), 1)
        CALL(get_msg, LW(10), 22, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        CALL(evt_win_coin_off, LW(8))
        CALL(point_disp_onoff, 0)
        RETURN
    END_IF
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_LESS(LW(0), LW(13))
        CALL(get_msg, LW(10), 1, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        CALL(evt_win_coin_off, LW(8))
        CALL(point_disp_onoff, 0)
        RETURN
    END_IF
    CALL(evt_pouch_add_item, LW(12), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(get_msg, LW(10), 2, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        CALL(evt_win_coin_off, LW(8))
        CALL(point_disp_onoff, 0)
        RETURN
    END_IF
    CALL(set_buy_item_id, LW(12))
    CALL(evt_sub_load_progresstime, GSW(5), LW(0))
    CALL(evt_sub_load_progresstime, GSW(14), LW(1))
    CALL(evt_sub_load_progresstime, GSW(23), LW(2))
    CALL(add_shop_point, 1, LW(0), LW(1), LW(2))
    MULTIPLY(LW(13), -1)
    CALL(evt_pouch_add_coin, LW(13))
    CALL(evt_win_coin_wait, LW(8))
    CALL(point_wait)
    WAIT_MS(200)
    CALL(evt_win_coin_off, LW(8))
    CALL(point_disp_onoff, 0)
    CALL(evt_msg_continue)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_CLEAR(LW(0), 4096)
        IF_INT_EQUAL(GSWF(235), 0)
            SET_VALUE(GSWF(235), 1)
            CALL(evt_msg_print, 0, PTR((char*)str_item_first_hit), 0, 0)
        END_IF
    END_IF
    LOOP(0)
        CALL(chk_shop_point, LW(0), LW(1), LW(2))
        IF_INT_EQUAL(LW(0), -1)
            GOTO(123)
            RETURN
        END_IF
        SET_VALUE(LF(0), 0)
        CALL(get_shop_point, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 300)
            CALL(get_msg, LW(10), 26, LW(14))
            CALL(evt_msg_print_insert, 0, LW(14), 0, LW(9), LW(2))
        ELSE
            CALL(get_msg, LW(10), 34, LW(14))
            CALL(evt_msg_print, 0, LW(14), 0, LW(9))
            CALL(evt_win_coin_on, 0, LW(8))
            CALL(point_disp_onoff, 1)
            CALL(reset_shop_point)
            CALL(evt_win_coin_wait, LW(8))
            CALL(point_wait)
            WAIT_MS(500)
            CALL(evt_win_coin_off, LW(8))
            CALL(point_disp_onoff, 0)
            SET_VALUE(LF(0), 1)
        END_IF
        CALL(evt_pouch_add_item, LW(1), LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(get_msg, LW(10), 27, LW(14))
            CALL(evt_msg_print, 0, LW(14), 0, LW(9))
            IF_INT_EQUAL(LF(0), 1)
                SET_VALUE(GSWF(1), 1)
            END_IF
            GOTO(123)
            RETURN
        END_IF
        CALL(exchange_shop_point)
    END_LOOP
    LABEL(123)
    CALL(get_buy_evt, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SET_VALUE(LW(0), LW(12))
        CALL_CHILD(LW(1))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(shopper_talk_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(get_ptr, LW(10))
    SET_VALUE(LF(10), 0)
    CALL(get_fook_evt, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        IF_INT_EQUAL(LW(0), -100)
            SET_VALUE(LF(10), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(0), 0)
        SET_VALUE(GSWF(0), 1)
        CALL_CHILD(first_shopping_evt)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(chk_shop_point, LW(0), LW(1), LW(2))
        IF_INT_EQUAL(GSWF(1), 1)
            SET_VALUE(LW(0), 0)
            CALL(shop_point_item, 300, LW(1), LW(2))
        END_IF
        IF_INT_EQUAL(LW(0), -1)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_mario_key_onoff, 1)
                RETURN
            END_IF
            SET_VALUE(LF(11), 0)
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), PTR((char*)shop_map_mri_07))
                IF_INT_EQUAL(GSWF(250), 1)
                    CALL(evt_sub_load_progresstime, GSW(5), LW(0))
                    IF_INT_LESS(LW(0), 900000)
                        SET_VALUE(LW(14), PTR((char*)shop_msg_stg2_shop_04_1))
                        SET_VALUE(LF(11), 1)
                    END_IF
                END_IF
            END_IF
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), PTR((char*)shop_map_usu_01))
                IF_INT_EQUAL(GSWF(251), 1)
                    CALL(evt_sub_load_progresstime, GSW(14), LW(0))
                    IF_INT_LESS(LW(0), 900000)
                        SET_VALUE(LW(14), PTR((char*)shop_msg_usugurashop_04_1))
                        SET_VALUE(LF(11), 1)
                    END_IF
                END_IF
            END_IF
            SET_VALUE(LF(12), 0)
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), PTR((char*)shop_map_rsh_03_a))
                SET_VALUE(LF(12), 1)
            END_IF
            IF_STR_EQUAL(LW(0), PTR((char*)shop_map_rsh_03_b))
                SET_VALUE(LF(12), 1)
            END_IF
            IF_STR_EQUAL(LW(0), PTR((char*)shop_map_rsh_03_c))
                SET_VALUE(LF(12), 1)
            END_IF
            IF_INT_EQUAL(LF(12), 1)
                IF_INT_EQUAL(GSWF(252), 1)
                    CALL(evt_sub_load_progresstime, GSW(23), LW(0))
                    IF_INT_LESS(LW(0), 900000)
                        SET_VALUE(LW(14), PTR((char*)shop_msg_stg6_shop_04_1))
                        SET_VALUE(LF(11), 1)
                    END_IF
                END_IF
            END_IF
            IF_INT_EQUAL(LF(11), 0)
                CALL(get_msg, LW(10), 4, LW(14))
            END_IF
            IF_INT_EQUAL(LF(10), 0)
                CALL(evt_msg_print, 0, LW(14), 0, 0)
            ELSE
                SET_VALUE(LF(10), 0)
                CALL(evt_msg_print_add, 1, PTR((char*)shop_msg_scroll_once_stop))
                CALL(evt_msg_print_add, 0, LW(14))
            END_IF
            BREAK_LOOP
        END_IF
        CALL(evt_pouch_add_item, LW(1), LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(get_msg, LW(10), 29, LW(14))
            IF_INT_EQUAL(LF(10), 0)
                CALL(evt_msg_print, 0, LW(14), 0, 0)
            ELSE
                SET_VALUE(LF(10), 0)
                CALL(evt_msg_print_add, 1, PTR((char*)shop_msg_scroll_once_stop))
                CALL(evt_msg_print_add, 0, LW(14))
            END_IF
            BREAK_LOOP
        END_IF
        CALL(get_msg, LW(10), 28, LW(14))
        IF_INT_EQUAL(LF(10), 0)
            CALL(evt_msg_print_insert, 0, LW(14), 0, 0, LW(2))
        ELSE
            SET_VALUE(LF(10), 0)
            CALL(evt_msg_print_add, 1, PTR((char*)shop_msg_scroll_once_stop))
            CALL(evt_msg_print_add_insert, 0, LW(14), LW(2))
        END_IF
        IF_INT_EQUAL(GSWF(1), 0)
            CALL(exchange_shop_point)
        END_IF
        SET_VALUE(LF(0), 1)
        IF_INT_EQUAL(GSWF(1), 1)
            SET_VALUE(GSWF(1), 0)
        END_IF
    END_LOOP
    CALL(shop_flag_onoff, 1, 16384)
    CALL(get_msg, LW(10), 23, LW(14))
    CALL(evt_msg_select, 0, LW(14))
    CALL(shop_flag_onoff, 0, 16384)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(get_msg, LW(10), 5, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        CASE_EQUAL(1)
        CALL_CHILD(sell_evt)
        CASE_EQUAL(2)
        CALL_CHILD(keep_evt)
        CASE_EQUAL(3)
        CALL_CHILD(unkeep_evt)
        CASE_EQUAL(4)
        CALL_CHILD(point_evt)
        CASE_EQUAL(5)
        CALL(evt_msg_continue)
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(first_shopping_evt)
    CALL(shopper_name, LW(9))
    CALL(evt_set_dir_to_target, LW(9), PTR((char*)str_8041BC10))
    CALL(evt_mario_set_dir_npc, LW(9))
    CALL(get_msg, LW(10), 30, LW(14))
    IF_INT_EQUAL(LF(10), 0)
        CALL(evt_msg_print, 0, LW(14), 0, LW(9))
    ELSE
        SET_VALUE(LF(10), 0)
        CALL(evt_msg_print_add, 1, PTR((char*)shop_msg_scroll_once_stop))
        CALL(evt_msg_print_add, 0, LW(14))
    END_IF
    LOOP(0)
        CALL(get_msg, LW(10), 31, LW(14))
        CALL(evt_msg_print, 0, LW(14), 0, LW(9))
        CALL(get_msg, LW(10), 24, LW(14))
        CALL(evt_msg_select, 0, LW(14))
        IF_INT_EQUAL(LW(0), 0)
            CALL(get_msg, LW(10), 32, LW(14))
            CALL(evt_msg_print_add, 0, LW(14))
        ELSE
            CALL(get_msg, LW(10), 33, LW(14))
            CALL(evt_msg_print_add, 0, LW(14))
            RETURN
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(sell_evt)
    CALL(sell_pouchcheck_func)
    IF_INT_EQUAL(LW(0), 0)
        CALL(get_msg, LW(10), 6, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    CALL(get_msg, LW(10), 7, LW(14))
    CALL(evt_msg_print_add, 0, LW(14))
    CALL(evt_win_coin_on, 0, LW(8))
    LABEL(0)
    CALL(item_data_db_arrange)
    CALL(evt_win_item_select, 1, 3, LW(1), LW(4))
    CALL(item_data_db_restore)
    IF_INT_LESS_OR_EQUAL(LW(1), 0)
        CALL(get_msg, LW(10), 22, LW(14))
        CALL(evt_msg_print, 0, LW(14), 0, PTR((char*)str_me))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(item_data_db_arrange)
    CALL(name_price, LW(1), LW(2), LW(3))
    CALL(item_data_db_restore)
    CALL(get_msg, LW(10), 8, LW(14))
    CALL(evt_sub_get_language, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_insert, 0, LW(14), 0, PTR((char*)str_me), LW(3), LW(2))
    ELSE
        CALL(evt_msg_print_insert, 0, LW(14), 0, PTR((char*)str_me), LW(2), LW(3))
    END_IF
    CALL(get_msg, LW(10), 24, LW(14))
    CALL(evt_msg_select, 0, LW(14))
    IF_INT_EQUAL(LW(0), 1)
        CALL(get_msg, LW(10), 9, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        GOTO(0)
    END_IF
    CALL(N_evt_pouch_remove_item_index, LW(1), LW(4), LW(0))
    CALL(evt_pouch_add_coin, LW(3))
    CALL(evt_win_coin_wait, LW(8))
    WAIT_MS(200)
    CALL(evt_win_coin_off, LW(8))
    CALL(sell_pouchcheck_func)
    IF_INT_EQUAL(LW(0), 0)
        CALL(get_msg, LW(10), 11, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    CALL(get_msg, LW(10), 10, LW(14))
    CALL(evt_msg_print_add, 0, LW(14))
    CALL(get_msg, LW(10), 24, LW(14))
    CALL(evt_msg_select, 0, LW(14))
    IF_INT_EQUAL(LW(0), 1)
        CALL(get_msg, LW(10), 22, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_win_coin_on, 0, LW(8))
    GOTO(0)
    RETURN
EVT_END()

static EVT_BEGIN(keep_evt)
    CALL(keep_pouchcheck_func)
    IF_INT_EQUAL(LW(0), 0)
        CALL(get_msg, LW(10), 12, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    IF_INT_EQUAL(LW(1), 0)
        CALL(get_msg, LW(10), 13, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    CALL(get_msg, LW(10), 14, LW(14))
    CALL(evt_msg_print_add_insert, 0, LW(14), LW(1))
    LABEL(0)
    CALL(evt_win_item_select, 1, 4, LW(1), LW(4))
    IF_INT_LESS_OR_EQUAL(LW(1), 0)
        CALL(get_msg, LW(10), 22, LW(14))
        CALL(evt_msg_print, 0, LW(14), 0, PTR((char*)str_me))
        RETURN
    END_IF
    CALL(N_evt_pouch_remove_item_index, LW(1), LW(4), LW(0))
    CALL(evt_pouch_add_keepitem, LW(1), LW(0))
    CALL(keep_pouchcheck_func)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LW(1), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(get_msg, LW(10), 15, LW(14))
        CALL(evt_msg_print, 0, LW(14), 0, PTR((char*)str_me))
        RETURN
    END_IF
    CALL(get_msg, LW(10), 16, LW(14))
    CALL(evt_msg_print, 0, LW(14), 0, PTR((char*)str_me))
    CALL(get_msg, LW(10), 24, LW(14))
    CALL(evt_msg_select, 0, LW(14))
    IF_INT_EQUAL(LW(0), 1)
        CALL(get_msg, LW(10), 22, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    GOTO(0)
    RETURN
EVT_END()

static EVT_BEGIN(unkeep_evt)
    CALL(unkeep_pouchcheck_func)
    IF_INT_EQUAL(LW(0), 0)
        CALL(get_msg, LW(10), 17, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    IF_INT_EQUAL(LW(1), 0)
        CALL(get_msg, LW(10), 18, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    CALL(get_msg, LW(10), 19, LW(14))
    CALL(evt_msg_print_add, 0, LW(14))
    LABEL(0)
    CALL(evt_win_item_select, 1, 5, LW(1), LW(4))
    IF_INT_LESS_OR_EQUAL(LW(1), 0)
        CALL(get_msg, LW(10), 22, LW(14))
        CALL(evt_msg_print, 0, LW(14), 0, PTR((char*)str_me))
        RETURN
    END_IF
    CALL(evt_pouch_remove_keepitem, LW(1), LW(4), LW(0))
    CALL(evt_pouch_add_item, LW(1), LW(0))
    CALL(unkeep_pouchcheck_func)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LW(1), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(get_msg, LW(10), 20, LW(14))
        CALL(evt_msg_print, 0, LW(14), 0, PTR((char*)str_me))
        RETURN
    END_IF
    CALL(get_msg, LW(10), 21, LW(14))
    CALL(evt_msg_print, 0, LW(14), 0, PTR((char*)str_me))
    CALL(get_msg, LW(10), 24, LW(14))
    CALL(evt_msg_select, 0, LW(14))
    IF_INT_EQUAL(LW(0), 1)
        CALL(get_msg, LW(10), 22, LW(14))
        CALL(evt_msg_print_add, 0, LW(14))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    GOTO(0)
    RETURN
EVT_END()

static EVT_BEGIN(point_evt)
    CALL(get_shop_point, LW(0))
    CALL(get_msg, LW(10), 25, LW(14))
    CALL(evt_msg_print_add_insert, 0, LW(14), LW(0))
    RETURN
EVT_END()

// .sdata
EVT_BEGIN(evt_shoplist)
    CALL(evt_mario_key_onoff, 0)
    CALL(disp_list)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(main_evt)
    CALL(evt_shop_main_func)
    RETURN
EVT_END()

#include "manager/fontmgr.h"
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

// this file's own script (its data isn't written as source yet)
extern u32 buy_evt[];

extern GlobalWork* gp;

//.sbss
char mapname[16];
static evt_shop_work* _wp;

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

s32 evtShopIsActive(void) {
	s32 active;

	if (strcmp(gp->currentMapName, mapname) == 0 && ((ShopWork*)_wp)->flags != 0) {
		active = 1;
	} else {
		active = 0;
	}
	return active;
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

USER_FUNC(sell_pouchcheck_func) {
	pouchGetPtr();
	event->lwData[0] = pouchGetHaveItemCnt();
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

USER_FUNC(get_ptr) {
	evtSetValue(event, event->args[0], (s32)_wp);
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

USER_FUNC(get_fook_evt) {
	evtSetValue(event, event->args[0], *(s32*)((u8*)((ShopWork*)_wp)->field_0xC + 0x4));
	return EVT_RETURN_DONE;
}

USER_FUNC(get_buy_evt) {
	evtSetValue(event, event->args[0], *(s32*)((u8*)((ShopWork*)_wp)->field_0xC + 0x8));
	return EVT_RETURN_DONE;
}

USER_FUNC(set_buy_item_id) {
	((ShopWork*)_wp)->field_0xD4 = evtGetValue(event, event->args[0]);
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

USER_FUNC(name_price) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)msgSearch(itemDataTable[id].name));
	evtSetValue(event, args[2], *(u16*)&itemDataTable[id].sell_price);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_shop_point) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();

	evtSetValue(event, args[0], pouch->mShopPoints);
	return EVT_RETURN_DONE;
}

USER_FUNC(reset_shop_point) {
	PouchData* pouch = pouchGetPtr();

	pouch->mShopPoints = 0;
	pouch->field_0x5B4 = 0;
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

void help_main(WinMgrEntry* entry) {
	winMgrHelpMain(entry);
}

void help_disp(WinMgrEntry* entry) {
	ShopWork* wp = (ShopWork*)_wp;
	s32 offset = wp->field_0x2C * 8;
	s32* items = wp->field_0x8;

	if (!winMgrAction(wp->field_0x54)) {
		s32 item = *(s32*)((u8*)items + offset);
		entry->field_0x34 = (s32)msgSearch(itemDataTable[item].description);
		winMgrHelpDraw(entry);
	}
}

void title_disp(WinMgrEntry* entry) {
	if (!(entry->flags & 4)) {
		GXColor color;
		const char* msg = msgSearch(str_802ECE38);

		FontDrawStart();
		FontDrawEdge();
		*(u32*)&color = dat_8041bbec;
		FontDrawColor(&color);
		FontDrawMessage(entry->field_0x18 + (entry->field_0x20 - FontGetMessageWidth(msg)) / 2, entry->field_0x1C - 12, msg);
		FontDrawEdgeOff();
	}
}

// NONMATCHING (94.99%): `i = 0` is emitted just before the loop preheader, not back after
// `bl pouchGetPtr`; two header x-sums keep their last intermediate in r3 where the original
// uses r0; and the int->float block schedules differently (mask lands in f30, not f28).
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
	u32 width;
	char buf[0x100];
	Mtx mtx;
	Mtx scale;

	if (entry->flags & 4) {
		return;
	}

	pouch = pouchGetPtr();

	msg = msgSearch(str_802ECDF8);
	FontDrawStart();
	FontDrawEdge();
	*(u32*)&color0 = dat_8041bbf0;
	FontDrawColor(&color0);
	width = FontGetMessageWidth(msg);
	FontDrawMessage((100 - (s32)width) / 2 + (10 + entry->field_0x18), entry->field_0x1C + 13, msg);
	FontDrawEdgeOff();

	msg = msgSearch(str_802ECE10);
	FontDrawStart();
	FontDrawEdge();
	*(u32*)&color1 = dat_8041bbf4;
	FontDrawColor(&color1);
	width = FontGetMessageWidth(msg);
	FontDrawMessage(entry->field_0x18 + entry->field_0x20 / 2 - 10 + (100 - (s32)width) / 2, entry->field_0x1C + 13, msg);
	FontDrawEdgeOff();

	i = 0;
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
		sprintf(buf, "%s", winZenkakuStr(shopPointList[i].price));
		width = FontGetMessageWidth(buf);
		FontDrawMessage(entry->field_0x20 / 2 - (s32)width + entry->field_0x18 - 90, entry->field_0x1C - 20 - i * 26, buf);

		FontDrawStart();
		if (pouch->field_0x5B4 & (1 << i)) {
			*(u32*)&color4 = dat_8041bc00;
			FontDrawColor(&color4);
		}
		else {
			*(u32*)&color5 = dat_8041bc04;
			FontDrawColor(&color5);
		}
		msg = msgSearch(str_802ECDF8);
		sprintf(buf, "%s", msg);
		width = FontGetMessageWidth(buf);
		PSMTXTrans(mtx, (f32)entry->field_0x18 + (entry->field_0x20 / 2 - 0.6f * width) - 40.0f, entry->field_0x1C - 28 - i * 26, 0.0f);
		PSMTXScale(scale, 0.64f, 0.64f, 0.64f);
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
		FontDrawMessage(entry->field_0x18 + entry->field_0x20 / 2 - 10, entry->field_0x1C - 20 - i * 26, msg);
	}
}

void point_disp(WinMgrEntry* entry) {
	ShopWork* wp = (ShopWork*)_wp;
	PouchData* pouch = pouchGetPtr();

	if (!(entry->flags & 4)) {
		Mtx mtx;
		Mtx scale;
		s32 delta;

		PSMTXTrans(mtx, float_10_8041bc3c + (float_30_8041bc40 + (f32)entry->field_0x18), (f32)entry->field_0x1C - float_36_8041bc44, float_0_8041bc34);
		if ((u32)gp->language == 0) {
			PSMTXScale(scale, float_1p1_8041bc48, float_1p1_8041bc48, float_1p1_8041bc48);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x1F1);
		}
		else {
			PSMTXScale(scale, float_0p8_8041bc4c, float_1p1_8041bc48, float_1p1_8041bc48);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x20A);
		}
		PSMTXTrans(mtx, float_124_8041bc50 + (f32)entry->field_0x18, (f32)entry->field_0x1C - float_38_8041bc54, float_0_8041bc34);
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

// NONMATCHING (99.18%): `_wp` is loaded straight into r29 where the original loads r5 and
// copies to r29; and the two viewport-scale multiplies run y-first (0x172/0x14) where the
// original runs x-first (0x170/0x10), which is also the 0x10/0x14 stack-slot swap.
void name_disp(WinMgrEntry* entry) {
	ShopWork* wp = (ShopWork*)_wp;
	GXColor titleColor;
	Vec pos;
	Mtx mtx;
	char buf[0x40];
	const char* msg;
	s32 i;

	msg = msgSearch(itemDataTable[wp->field_0x8[wp->field_0x2C * 2]].name);
	if (winMgrAction(wp->field_0x50) == 0) {
		FontDrawStart();
		FontDrawEdge();
		*(u32*)&titleColor = dat_8041bbe0;
		FontDrawColor(&titleColor);
		FontDrawString(msg, (f32)(-(s32)FontGetMessageWidth(msg) / 2), float_108_8041bc58);
		FontDrawEdgeOff();
	}
	for (i = 0; i < wp->field_0x20; i++) {
		if (wp->field_0x14[i] & 1) {
			continue;
		}
		mapObjGetPos(*(const char**)((u8*)wp->field_0x4 + i * 8), &pos);
		pos.y += float_30_8041bc40;
		PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
		PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
		pos.y *= (f32)gp->efbHeight * float_0p5_8041bc5c;
		pos.x *= (f32)gp->fbWidth * float_0p5_8041bc5c;
		sprintf(buf, "%d", *(s32*)((u8*)wp->field_0x8 + i * 8 + 4));
		PSMTXTrans(mtx, float_12_8041bc64 * (f32)(strlen(buf) - 1) + pos.x, pos.y, pos.z);
		iconNumberDispGx(mtx, *(s32*)((u8*)wp->field_0x8 + i * 8 + 4), 0, *(GXColor*)&dat_8041bbe4);
	}
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
		iconSetPos(buf, pos.x, float_6_8041bc24 + pos.y, pos.z);
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

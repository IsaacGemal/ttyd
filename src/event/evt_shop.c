#include "manager/fontmgr.h"
#include "evt/evt_shop.h"
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
extern void mapObjGetPos(const char* name, Vec* pos);
extern void FontDrawString(const char* msg, f32 x, f32 y);
extern const f32 float_30_8041bc40;
extern const f32 float_10_8041bc3c;
extern const f32 float_36_8041bc44;
extern const f32 float_0_8041bc34;
extern const f32 float_1p1_8041bc48;
extern const f32 float_0p8_8041bc4c;
extern const f32 float_124_8041bc50;
extern const f32 float_38_8041bc54;
extern const char* msgSearch(const char* tag);
extern WinMgrDesc list_desc[];
extern void FontDrawStart(void);
extern void FontDrawEdge(void);
extern void FontDrawEdgeOff(void);
extern void FontDrawColor(GXColor* color);
extern void FontDrawMessage(s32 x, s32 y, const char* msg);

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
	const char* str = str_802ECD50;
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

void name_disp(WinMgrEntry* entry) {
	ShopWork* wp = (ShopWork*)_wp;
	GXColor iconColor;
	GXColor titleColor;
	Vec pos;
	char buf[0x40];
	Mtx mtx;
	const char* msg;
	s32 i;
	s32 offset;
	s32 price;

	msg = msgSearch(itemDataTable[wp->field_0x8[wp->field_0x2C * 2]].name);
	if (winMgrAction(wp->field_0x50) == 0) {
		*(u32*)&titleColor = dat_8041bbe0;
		FontDrawStart();
		FontDrawEdge();
		FontDrawColor(&titleColor);
		FontDrawString(msg, (f32)(-(s32)FontGetMessageWidth(msg) / 2), float_108_8041bc58);
		FontDrawEdgeOff();
	}
	for (i = 0, offset = 0; i < wp->field_0x20; i++, offset += 8) {
		if (wp->field_0x14[i] & 1) {
			continue;
		}
		mapObjGetPos(*(const char**)((u8*)wp->field_0x4 + offset), &pos);
		pos.y += float_30_8041bc40;
		PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
		PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
		pos.x *= float_0p5_8041bc5c * (f32)gp->fbWidth;
		pos.y *= float_0p5_8041bc5c * (f32)gp->efbHeight;
		price = *(s32*)((u8*)wp->field_0x8 + offset + 4);
		sprintf(buf, "%d", price);
		PSMTXTrans(mtx, float_12_8041bc64 * (f32)(strlen(buf) - 1) + pos.x, pos.y, pos.z);
		*(u32*)&iconColor = dat_8041bbe4;
		iconNumberDispGx(mtx, price, 0, iconColor);
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

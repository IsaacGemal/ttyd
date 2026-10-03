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
#include "driver/itemdrv.h"
#include "evt/evt_win.h"
#include "window/win_main.h"
#include "driver/windowdrv.h"
#include "statuswindow.h"
#include "pmario_sound.h"
#include <dolphin/gx.h>
#include <dolphin/gx/GXTransform.h>
#include <dolphin/gx/GXGet.h>
#include "evt/evt_badgeshop.h"
#include "evt/evt_johoya.h"
#include "evt/evt_lottery.h"
#include "mario/mario_party.h"
#include <stdio.h>

// The original's call sites convert the flag and icon arguments as u16 (clrlwi), not s16 (extsh).
#define iconDispGxU(p, f, i, sc) ((void (*)(Vec*, u16, u16, f32))iconDispGx)(p, f, i, sc)

// evt_unitwin_disp_func is a display callback, not an event command.

typedef struct { u32 w[16]; } UstBlk40;

// .rodata objects, in the original's order (build/G8MJ01/asm/event/evt_win.s). The original reads
// these as offsets from the pool base r30 = &vec3_802f64b8.
const Vec vec3_802f64b8 = { 11.0f, 0.0f, 0.0f };
const Vec vec3_802f64c4 = { 244.0f, 86.0f, 0.0f };
const Vec vec3_802f64d0 = { 11.0f, 0.0f, 0.0f };
const Vec vec3_802f64dc = { 244.0f, 86.0f, 0.0f };
const u8 zero_802f64e8[0x40] = { 0 };
const Vec vec3_802f6528 = { 11.0f, 0.0f, 0.0f };
const u8 zero_802f6534[0x40] = { 0 };
const Vec vec3_802f6574 = { 11.0f, 0.0f, 0.0f };
const Vec vec3_802f6580 = { 166.0f, 0.0f, 0.0f };
const Vec vec3_802f658c = { 186.0f, 0.0f, 0.0f };
const Vec vec3_802f6598 = { 176.0f, 0.0f, 0.0f };
const u8 zero_802f65a4[0x40] = { 0 };
const Vec vec3_802f65e4 = { 11.0f, 0.0f, 0.0f };
const u8 zero_802f65f0[0x40] = { 0 };
const Vec vec3_802f6630 = { 11.0f, 0.0f, 0.0f };
const char str_802F663C[] = "SFX_COUNT_COIN1";
const char str_802F664C[] = "msg_window_title_1";
const char str_802F6660[] = "msg_window_title_2";
const char str_802F6674[] = "msg_window_title_3";
const char str_802F6688[] = "msg_window_title_4";
const char str_802F669C[] = "msg_window_title_5";
const char str_802F66B0[] = "msg_window_title_6";
const char str_802F66C4[] = "msg_window_title_7";
const char str_802F66D8[] = "msg_window_title_8";
const char str_802F66EC[] = "name_party0";
const char str_802F66F8[] = "name_party1";
const char str_802F6704[] = "name_party4";
const char str_802F6710[] = "name_party5";
const char str_802F671C[] = "name_party2";
const char str_802F6728[] = "name_party6";
const char str_802F6734[] = "helperr";
const char str_802F673C[] = "lankup";
const char str_802F6744[] = "lankdown";
const char str_802F6750[] = "%s_%s_lank%d%s";
const char str_802F6760[] = "msg_window_title_9";
const char str_802F6774[] = "msg_window_title_10";
const char str_802F6788[] = "%s_levelup%s";
const char str_802F6798[] = "%s_leveldown%s";
const char str_802F67A8[] = "msg_window_select_6";
const char str_802F67BC[] = "msg_window_select_8";
const char str_802F67D0[] = "msg_window_select_7";
const char str_802F67E4[] = "msg_window_select_10";
const char str_802F67FC[] = "msg_window_select_2";
const char str_802F6810[] = "msg_window_select_3";
const char str_802F6824[] = "msg_window_select_4";
const char str_802F6838[] = "msg_window_select_11";
const char str_802F6850[] = "msg_window_select_12";
const char str_802F6868[] = "msg_window_select_9";
const char str_802F687C[] = "msg_window_select_9_1";
const char str_802F6894[] = "yubicursor_2";
const f64 double_to_int_802f68a8 = 4503601774854144.0;
const char str_802F68B0[] = "%s_%04d";
const f64 double_to_int_mask_802f68b8 = 4503599627370496.0;


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
const char str_8041DAC0[] = "uwn%d";

// Constants and tables evt_unitwin_disp_func reads. They live in this unit's .sdata/.sdata2/.sdata; the
// originals are named float_<value>_<addr>, dat_<addr> and mario_status_*/party_*_table.
extern const f32 float_neg150_8041dac8;
extern const f32 float_120_8041dacc;
extern const f32 float_300_8041dad0;
extern const f32 float_50_8041dad4;
extern const f32 float_20_8041dad8;
extern const f32 float_neg240_8041dae0;
extern const f32 float_neg130_8041dae4;
extern const f32 float_480_8041dae8;
extern const f32 float_70_8041daec;
extern const f32 float_8_8041daf0;
extern const f32 float_neg304_8041daf4;
extern const f32 float_240_8041daf8;
extern const f32 float_60_8041dafc;
extern const f32 float_neg250_8041db04;
extern const f32 float_neg75_8041db08;
extern const f32 float_150_8041db0c;
extern const f32 float_45_8041db10;
extern const f32 float_neg220_8041db14;
extern const f32 float_neg113_8041db18;
extern const f32 float_0p7_8041db1c;
extern const f32 float_neg192_8041db20;
extern const f32 float_neg126_8041db24;
extern const f32 float_neg210_8041db28;
extern const f32 float_neg166_8041db2c;
extern const f32 float_neg168_8041db34;
extern const f32 float_neg172_8041db38;
extern const f32 float_neg24_8041db44;
extern const f32 float_100_8041db48;
extern const f32 float_51_8041db4c;
extern const f32 float_114_8041db50;
extern const f32 float_28_8041db54;
extern const f32 float_81_8041db58;
extern const f32 float_111_8041db5c;
extern const f32 float_244_8041db68;
extern const f32 float_82_8041db6c;
extern const f32 float_0p6_8041db70;
extern const f32 float_2_8041db78;
extern const f32 float_1_8041db80;
extern const f32 float_71_8041db8c;
extern const f32 float_16_8041db98;
extern const f32 float_41_8041db9c;
extern const f32 float_167_8041dba0;
extern const f32 float_85_8041dba4;
extern const f32 float_177_8041dba8;
extern const f32 float_236_8041dbb0;
extern const f32 float_221_8041db94;
extern const f32 float_21_8041dbbc;
extern const f32 float_206_8041dbd0;
extern const f32 float_neg280_8041dbd4;
extern const f32 float_228_8041dbd8;
extern const f32 float_90_8041dbdc;
extern const f32 float_66_8041dbe0;
extern u32 dat_8041d9c0;
extern u32 dat_8041d9c4;
extern u32 dat_8041d9c8;
extern u32 dat_8041d9cc;
extern u32 dat_8041d9d0;
extern u32 dat_8041d9d4;
extern u32 dat_8041d9d8;
extern u32 dat_8041d9dc;
extern u32 dat_8041d9e0;
extern u32 dat_8041d9e4;
extern u32 dat_8041d9e8;
extern u32 dat_8041d9ec;
extern u32 dat_8041d9f0;
extern u32 dat_8041d9f4;
extern u32 dat_8041d9f8;
extern u32 dat_8041d9fc;
extern u32 dat_8041da00;
extern u32 dat_8041da04;
extern u32 dat_8041da08;
extern u32 dat_8041da0c;
extern u32 dat_8041da10;
extern u32 dat_8041da14;
extern u32 dat_8041da18;
extern u32 dat_8041da1c;
extern u32 dat_8041da20;
extern u32 dat_8041da24;
extern u32 dat_8041da28;
extern u32 dat_8041da2c;
extern u32 dat_8041da30;
extern u32 dat_8041da34;
extern u32 dat_8041da38;
extern u32 dat_8041da3c;
extern u32 dat_8041da40;
extern u32 dat_8041da44;
extern u32 dat_8041da48;
extern u32 dat_8041da4c;
extern u32 dat_8041da50;
extern u32 dat_8041da54;
extern u32 dat_8041da58;
extern u32 dat_8041da5c;
extern u32 dat_8041da60;
extern u32 dat_8041da64;
extern u32 dat_8041da68;
extern u32 dat_8041da6c;
extern u32 dat_8041da70;
extern u32 dat_8041da74;
extern u32 dat_8041da78;
extern u32 dat_8041da7c;
extern u32 dat_8041da80;
extern u32 dat_8041da84;
extern u32 dat_8041da88;
extern u32 dat_8041da8c;
extern u32 dat_8041da90;
extern u32 dat_8041da94;
extern u32 dat_8041da98;
extern u32 dat_8041da9c;
extern u32 dat_8041daa0;
extern u32 dat_8041daa4;
extern u32 dat_8041daa8;
extern u32 dat_8041daac;
extern u32 dat_8041dab0;
extern u32 dat_8041dab4;
extern u32 dat_8041dab8;
extern u32 dat_8041dabc;
extern s32 unk_804189fc;
extern s32 party_id_table[8];
extern u32 party_icon_table[7];
extern const char* party_labelname_table[7];
extern u32 mario_status_icon_table[3];
extern const char* mario_status_labelname_table[3];
extern const char* mario_status_name_table[3];
extern s32 mario_status_henka_table[3];
extern s32 badge_bottakuru100_table[];
extern BadgeShopWork* bdsw;

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
	u8 pad_0x38[0x4];           // 0x38
	f32 field_0x3C;             // 0x3C
	f32 field_0x40;             // 0x40
	f32 field_0x44;             // 0x44
	u8 pad_0x48[0x4];           // 0x48
	s32 field_0x4C;             // 0x4C
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

// The original keeps the byte offset (id * sizeof(PouchPartyData)) in r30 and
// recomputes the final `pouch + id * sizeof` in a volatile register after the
// pouchRevisePartyParam call, rather than caching a single element pointer.
USER_FUNC(unitwin_setpartylank) {
	s32 id;
	s32 offset;
	s32 lank;
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();

	id = evtGetValue(event, args[0]);
	offset = id * sizeof(PouchPartyData);
	lank = evtGetValue(event, args[1]);

	((PouchPartyData*)((u8*)pouch + offset))->techLevel = lank;
	((PouchPartyData*)((u8*)pouch + offset))->attackLevel = lank;
	((PouchPartyData*)((u8*)pouch + offset))->HPLevel = lank;
	pouchRevisePartyParam();
	((PouchPartyData*)((u8*)pouch + id * sizeof(PouchPartyData)))->currentHP =
	    ((PouchPartyData*)((u8*)pouch + id * sizeof(PouchPartyData)))->maximumHP;
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

// itemDataTable[].icon_id is declared s16 in the header, but the original loads it with
// `lhz` (u16) and its address build only matches through a local u16 view of the field.
USER_FUNC(set_new_goods_list) {
	s32* args = event->args;
	UnitWinWork* wp = (UnitWinWork*)evtGetValue(event, args[0]);
	s32 item = evtGetValue(event, args[1]);
	char name[32];

	wp->field_0x8[wp->index] = item;
	if (wp->index < wp->count) {
		sprintf(name, str_8041DAC0, wp->index);
		if (item != 0) {
#ifdef MUST_MATCH
			u16* p = (u16*)&itemDataTable[0].icon_id;
			iconChange(name, p[item * (sizeof(ItemData) / sizeof(u16))]);
#else
			iconChange(name, (u16)itemDataTable[item].icon_id);
#endif
		} else {
			iconDelete(name);
		}
	}
	return EVT_RETURN_DONE;
}

// Mario HP/FP/BP status window shared by the level-up / level-down cases. col[] holds the eight
// GXColors each case uses (window1, window2, title1, window3, title2, rowUp, rowDown, help) and
// levelFmt is either "%s_levelup%s" or "%s_leveldown%s".
#define UST_TAIL_A(wp, levelFmt) \
	if ((f32)dispCnt == wp->field_0x34) { \
		if (changed == 0) { \
			sprintf(help2, "help"); \
		} else { \
			sprintf(help2, str_802F6734); \
		} \
		sprintf(help, levelFmt, mario_status_labelname_table[row], help2); \
		numval = mario_status_point_table[row][0] + mario_status_henka_table[row] * 2; \
		if (numval > 0) { \
			arrow = 0x1B5; \
		} else { \
			arrow = 0x162; \
			blink = 0; \
		} \
		{ \
			Vec ic = vec3_802f6580; \
			ic.y = wp->field_0x44 + (f32)(0x38 - dispOff) + (f32)blink; \
			iconDispGxU(&ic, flag | 0x10, arrow, float_0p5_8041db60); \
		} \
		{ \
			Vec ic = vec3_802f658c; \
			ic.y = wp->field_0x44 + (f32)(0x38 - dispOff) + (f32)blink; \
			iconDispGxU(&ic, flag | 0x10, arrow, float_0p5_8041db60); \
		} \
	} else { \
		numval = mario_status_point_table[row][0] - mario_status_henka_table[row]; \
		if (numval > 0) { \
			arrow = 0x1B6; \
		} else { \
			arrow = 0x162; \
			blink = 0; \
		} \
		{ \
			Vec ic = vec3_802f6598; \
			ic.y = wp->field_0x44 + (f32)(0x38 - dispOff) - (f32)blink; \
			iconDispGxU(&ic, flag | 0x10, arrow, float_0p5_8041db60); \
		} \
	} \
	if (numval > 0) { \
		sprintf(num, "%3d", numval); \
	} else { \
		sprintf(num, "  \x81H"); \
	} \
	FontDrawStart(); \
	FontDrawString(num, float_206_8041dbd0, wp->field_0x44 + (f32)(0x50 - dispOff));

#define UST_TAIL_PLUS(wp, levelFmt) \
	if ((f32)dispCnt == wp->field_0x34) { \
		if (changed == 0) { \
			sprintf(help2, "help"); \
		} else { \
			sprintf(help2, str_802F6734); \
		} \
		sprintf(help, levelFmt, mario_status_labelname_table[row], help2); \
		numval = mario_status_point_table[row][0] + mario_status_henka_table[row]; \
		if (numval > 0) { \
			sprintf(num, "%3d", numval); \
		} else { \
			sprintf(num, "  \x81H"); \
		} \
		FontDrawStart(); \
		FontDrawString(num, float_206_8041dbd0, wp->field_0x44 + (f32)(0x50 - dispOff)); \
	}

#define UST_TAIL_MINUS(wp, levelFmt) \
	if ((f32)dispCnt == wp->field_0x34) { \
		if (changed == 0) { \
			sprintf(help2, "help"); \
		} else { \
			sprintf(help2, str_802F6734); \
		} \
		sprintf(help, levelFmt, mario_status_labelname_table[row], help2); \
		numval = mario_status_point_table[row][0] - mario_status_henka_table[row]; \
		if (numval > 0) { \
			sprintf(num, "%3d", numval); \
		} else { \
			sprintf(num, "  \x81H"); \
		} \
		FontDrawStart(); \
		FontDrawString(num, float_206_8041dbd0, wp->field_0x44 + (f32)(0x50 - dispOff)); \
	}

#define UST_GUARD_NONE
#define UST_GUARD_ROW if (row == wp->field_0x4C) { continue; }

#define UST_CHG_A(wp, c5, c6) \
	cnt = 0; \
	for (j = 0; j < wp->field_0x58; j++) { \
		if ((f32)j != wp->field_0x34 && mario_status_point_table[j][0] - mario_status_henka_table[j] <= 0) { \
			cnt = 1; \
		} \
	} \
	if (cnt == 0) { \
		rowColor = *(GXColor*)&c5; \
		changed = 0; \
		flag = 0; \
	} else { \
		rowColor = *(GXColor*)&c6; \
		changed = 1; \
		flag = 0x20; \
	}

#define UST_CHG_B(wp, c5, c6) \
	cnt = 0; \
	for (j = 0; j < wp->field_0x58; j++) { \
		if ((f32)j != wp->field_0x34 && mario_status_point_table[j][0] - mario_status_henka_table[j] <= 0) { \
			cnt += 1; \
		} \
	} \
	if (cnt < 2) { \
		rowColor = *(GXColor*)&c5; \
		changed = 0; \
		flag = 0; \
	} else { \
		rowColor = *(GXColor*)&c6; \
		changed = 1; \
		flag = 0x20; \
	}

#define UST_CHG_C(wp, c5, c6) \
	if (mario_status_point_table[row][0] - mario_status_henka_table[row] > 0) { \
		rowColor = *(GXColor*)&c5; \
		changed = 0; \
		flag = 0; \
	} else { \
		rowColor = *(GXColor*)&c6; \
		changed = 1; \
		flag = 0x20; \
	}

#define UNITWIN_STATUS_DISP(wp, c0, c1, c2, c3, c4, c5, c6, c7, levelFmt, ROWGUARD, CHGBLOCK, ZEROBLK, TAILBLOCK, POSBLK, ROWMAX) do { \
	s32 dispCnt; \
	char help2[0x10]; \
	s32 row; \
	s32 listOff; \
	char help[0x40]; \
	s32 dispOff = 0; \
	char num[0x40]; \
	u32 sc[4]; \
	s32 m8; \
	GXColor color; \
	*(UstBlk40*)help = *(const UstBlk40*)ZEROBLK; \
	m8 = 8 - wp->field_0x58; \
	listOff = ((m8 > 0) ? m8 : 0) * 0x18; \
	windowDispGX_Waku_col(0, *(GXColor*)&c0, float_neg24_8041db44, float_100_8041db48, float_300_8041dad0, (f32)(0xdc - listOff), \
	                      float_20_8041dad8); \
	windowDispGX_Waku_col(0, *(GXColor*)&c1, float_16_8041db98, float_114_8041db50, float_120_8041dacc, float_28_8041db54, \
	                      float_8_8041daf0); \
	FontDrawStart(); \
	FontDrawEdge(); \
	color = *(GXColor*)&c2; \
	FontDrawColor(&color); \
	FontDrawString(msgSearch(str_802F6760), float_21_8041dbbc, float_111_8041db5c); \
	FontDrawEdgeOff(); \
	windowDispGX_Waku_col(0, *(GXColor*)&c3, float_167_8041dba0, float_114_8041db50, float_85_8041dba4, float_28_8041db54, \
	                      float_8_8041daf0); \
	FontDrawStart(); \
	FontDrawEdge(); \
	color = *(GXColor*)&c4; \
	FontDrawColor(&color); \
	FontDrawString(msgSearch(str_802F6774), float_177_8041dba8, float_111_8041db5c); \
	FontDrawEdgeOff(); \
	GXGetScissor(&sc[0], &sc[1], &sc[2], &sc[3]); \
	GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff); \
	dispCnt = 0; \
	for (row = 0; row < ROWMAX; row++) { \
		s32 changed = 0; \
		s32 j; \
		s32 cnt = 0; \
		s32 numval; \
		s32 flag; \
		s32 arrow; \
		s32 blink; \
		GXColor rowColor; \
		ROWGUARD \
		CHGBLOCK(wp, c5, c6) \
		{ \
			Vec pc = POSBLK; \
			pc.y = wp->field_0x44 + (f32)(0x38 - dispOff); \
			iconDispGxU(&pc, flag | 0x10, mario_status_icon_table[row], float_0p5_8041db60); \
		} \
		FontDrawStart(); \
		{ GXColor cc = rowColor; FontDrawColor(&cc); } \
		sprintf(num, "%s%3d", mario_status_name_table[row], mario_status_point_table[row][0]); \
		FontDrawString(num, float_36_8041db64, wp->field_0x44 + (f32)(0x50 - dispOff)); \
		blink = ((gp->retraceCount & 0xF) > 7) ? 0 : 2; \
		TAILBLOCK(wp, levelFmt) \
		dispOff += 0x18; \
		dispCnt++; \
	} \
	GXSetScissor(sc[0], sc[1], sc[2], sc[3]); \
	if ((s8)help[0] != 0) { \
		windowDispGX_Waku_col(0, *(GXColor*)&c7, float_neg240_8041dae0, float_neg130_8041dae4, float_480_8041dae8, float_70_8041daec, \
		                      float_20_8041dad8); \
		FontDrawStart(); \
		FontDrawMessage(-220, -138, msgSearch(help)); \
	} \
} while (0)



// NONMATCHING (96.07%): complete reconstruction; the bl sequence (338 calls), case layout
// and .rodata order all match. Fixed here: the status-macro flag is passed as `flag | 0x10` (the
// target does `ori` + `clrlwi`, not `extsh`); the badge/icon `iconId` is `s32` (the target
// truncates it with `clrlwi` at the iconDispGx call); the common-tail buffer check keeps the
// base pointer in a `char*` variable used by the loop, so `p != NULL` compiles to `cmplwi` on a
// register instead of the folded `addic. r0, r1, 0x660`; and five declaration runs are ordered to
// reproduce MWCC's callee-saved colouring (match-only, see REPORT). What remains is MWCC's
// callee-saved colouring: our first-block sellName/sellDesc get r16/r17 where the target uses
// r17/r18, and the same one-register shift runs through the function, which also makes the frame
// 0x20 larger (0x7a0 vs 0x780). The int->float magic is the compiler's own pool entry, so the named
// double_to_int_802f68a8 / double_to_int_mask_802f68b8 definitions in this file are duplicates:
// every i2f conversion loads 0x408(r30) instead of the target's 0x3f0(r30) (removing those two
// data definitions moves the loads to 0x3f0 but does not change the score). Display callback
// for the shop window.
// NONMATCHING (96.07%): MWCC callee-saved register colouring is shifted one register (r16/r17 vs
// the target's r17/r18) and the frame is 0x20 larger; see the note above.
void evt_unitwin_disp_func(CameraId cameraId, void* param) {
	UnitWinWork* wp = (UnitWinWork*)param;
	PouchData* pouch = pouchGetPtr();

	// --- flags & 1: selected item's name/description, plus the on-map price icons ---
	if (wp->flags & 1) {
		u32 len;
		s32 sellItem = wp->field_0x8[wp->index];
		GXColor color;
		const char* sellDesc = itemDataTable[sellItem].description;
		s32 i;
		const char* sellName = itemDataTable[sellItem].id;

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9c0, float_neg150_8041dac8, float_120_8041dacc, float_300_8041dad0, float_50_8041dad4,
		                      float_20_8041dad8);
		FontDrawStart();
		FontDrawEdge();
		color = *(GXColor*)&dat_8041d9c4;
		FontDrawColor(&color);
		len = strlen(sellName);
		FontDrawString(sellName, (f32)(s32)(((len * 0x18) >> 2) * -1), float_108_8041dadc);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9c8, float_neg240_8041dae0, float_neg130_8041dae4, float_480_8041dae8, float_70_8041daec,
		                      float_20_8041dad8);
		FontDrawStart();
		FontDrawMessage(-220, -138, msgSearch(sellDesc));

		for (i = 0; i < wp->count; i++) {

			if (wp->field_0x8[i] != 0) {
				Mtx mtx;
				f32 projv[7];
				u8 price;
				f32 vp[6];
				f32 sx, sy, sz;
				CameraEntry* cam;
				Vec pos;
				GXColor iconColor;

				mapObjGetPos(wp->objList[i].name, &pos);
				cam = camGetPtr(CAMERA_3D);
				GXSetProjection(cam->projection, cam->projType);
				GXGetProjectionv(projv);
				GXGetViewportv(vp);
				GXProject(pos.x, pos.y, pos.z, cam->view, projv, vp, &sx, &sy, &sz);
				cam = camGetPtr(cameraId);
				GXSetProjection(cam->projection, cam->projType);
				if (wp->field_0x8[i] >= 0xF0 && wp->field_0x8[i] < 0x153) {
					price = (u16)itemDataTable[wp->field_0x8[i]].discount_price;
				} else {
					price = ((ItemDataView*)&itemDataTable[wp->field_0x8[i]])->buy_price;
				}
				PSMTXTrans(mtx, float_8_8041daf0 + (float_neg304_8041daf4 + sx),
				           float_240_8041daf8 - (sy - float_60_8041dafc), float_0_8041db00);
				iconNumberDispGx(mtx, price, 1, *(GXColor*)&dat_8041d9cc);
			}
		}
	}

	// --- flags & 0xD000: coin / super coin / star piece window ---
	if (wp->flags & 0xD000) {
		Mtx scale;
		s32 cur;
		GXColor color;
		Mtx mtx;

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9d0, float_neg250_8041db04, float_neg75_8041db08, float_150_8041db0c, float_45_8041db10,
		                      float_20_8041dad8);
		PSMTXTrans(mtx, float_neg220_8041db14, float_neg113_8041db18, float_0_8041db00);
		PSMTXScale(scale, float_0p7_8041db1c, float_0p7_8041db1c, float_0p7_8041db1c);
		PSMTXConcat(mtx, scale, mtx);
		if (wp->flags & 0x1000) {
			iconDispGx2(mtx, 0x10, 0x18B);
			cur = pouchGetCoin();
		} else if (wp->flags & 0x4000) {
			iconDispGx2(mtx, 0x10, 0x102);
			cur = pouchGetSuperCoin();
		} else {
			iconDispGx2(mtx, 0x10, 0x18D);
			cur = pouchGetStarPiece();
		}
		PSMTXTrans(mtx, float_neg192_8041db20, float_neg113_8041db18, float_0_8041db00);
		iconDispGx2(mtx, 0x10, 0x1D6);
		PSMTXTrans(mtx, float_neg126_8041db24, float_neg113_8041db18, float_0_8041db00);
		color = *(GXColor*)&dat_8041d9d4;
		iconNumberDispGx(mtx, wp->field_0x2C, 0, color);
		{
			s32 delta = cur - wp->field_0x2C;

			if (delta != 0) {
				delta /= 10;
				if (delta == 0) {
					delta = (cur > wp->field_0x2C) ? 1 : -1;
				}
				if (wp->field_0x5C & 1) {
					wp->field_0x2C += delta;
					statusWinForceUpdateCoin();
				}
				wp->field_0x5C = wp->field_0x5C + 1;
				wp->field_0x5C = wp->field_0x5C % 1200;
				if (unk_804189fc == 0) {
					psndSFXOn(str_802F663C);
					unk_804189fc = 7;
				}
			}
		}
		if (unk_804189fc > 0) {
			unk_804189fc--;
		}
	}

	// --- flags & 0x2000: shop-points window ---
	if (wp->flags & 0x2000) {
		Mtx mtx;
		Mtx scale;
		GXColor color;

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9d8, float_neg250_8041db04, float_neg130_8041dae4, float_150_8041db0c, float_45_8041db10,
		                      float_20_8041dad8);
		PSMTXTrans(mtx, float_neg210_8041db28, float_neg166_8041db2c, float_0_8041db00);
		PSMTXScale(scale, float_1p1_8041db30, float_1p1_8041db30, float_1p1_8041db30);
		PSMTXConcat(mtx, scale, mtx);
		iconDispGx2(mtx, 0x10, 0x1F1);
		PSMTXTrans(mtx, float_neg126_8041db24, float_neg168_8041db34, float_0_8041db00);
		color = *(GXColor*)&dat_8041d9dc;
		iconNumberDispGx(mtx, wp->field_0x30, 0, color);
		if ((gp->retraceCount & 3) == 0) {
			s32 delta = pouch->mShineSpriteCount - wp->field_0x30;

			if (delta != 0) {
				delta /= 10;
				if (delta == 0) {
					delta = (pouch->mShineSpriteCount > wp->field_0x30) ? 1 : -1;
				}
				wp->field_0x30 += delta;
			}
		}
	}

	// --- flags & 0x100: lottery number ---
	if (wp->flags & 0x100) {
		if (lotteryGetPtr()->mFlags & 2) {
			char buf[8];
			GXColor color;
			const char* p;
			Mtx mtx;
			s32 k;

			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9e0, float_neg250_8041db04, float_neg130_8041dae4, float_150_8041db0c, float_45_8041db10,
			                      float_20_8041dad8);
			PSMTXTrans(mtx, float_neg220_8041db14, float_neg172_8041db38, float_0_8041db00);
			iconDispGx2(mtx, 0x10, 0x140);
			sprintf(buf, "%04d", *(s16*)&lotteryGetPtr()->field_0x2[0x1E]);
			for (k = 0, p = buf; k < 4; k++, p++) {
				PSMTXTrans(mtx, float_20_8041dad8 * (f32)k + float_neg126_8041db24 - float_60_8041dafc,
				           float_neg168_8041db34, float_0_8041db00);
				color = *(GXColor*)&dat_8041d9e4;
				iconNumberDispGx(mtx, (s8)*p - '0', 0, color);
			}
		}
	}

	switch (wp->field_0x50) {
	case 4: {
		// Stored-item (keep) list.
		const char* selDesc = 0;
		s32 dispOff = 0;
		s32 dispCnt = 0;
		s32 cnt = pouchGetKeepItemCnt();
		GXColor color;
		u32 sc[4];
		s32 k;

		cnt = (8 - cnt > 0) ? (8 - pouchGetKeepItemCnt()) : 0;
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9e8, float_neg24_8041db44, float_100_8041db48, float_300_8041dad0, (f32)(0xdc - cnt * 0x18),
		                      float_20_8041dad8);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9ec, float_51_8041db4c, float_114_8041db50, float_150_8041db0c, float_28_8041db54,
		                      float_8_8041daf0);
		FontDrawStart();
		FontDrawEdge();
		color = *(GXColor*)&dat_8041d9f0;
		FontDrawColor(&color);
		FontDrawString(msgSearch(str_802F664C), float_81_8041db58, float_111_8041db5c);
		FontDrawEdgeOff();
		GXGetScissor(&sc[0], &sc[1], &sc[2], &sc[3]);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - cnt * 0x18);
		for (k = 0; k < 32; k++) {
			s32 it = pouchKeepItem(k);
			ItemData* data = &itemDataTable[it];

			if (it != 0) {
				Vec pos = vec3_802f64b8;

				pos.y = wp->field_0x44 + (f32)(0x38 - dispOff);
				iconDispGxU(&pos, 0x10, ((ItemDataView*)data)->icon_id, float_0p5_8041db60);
				FontDrawStart();
				FontDrawString(data->id, float_36_8041db64, wp->field_0x44 + (f32)(0x50 - dispOff));
				if ((f32)dispCnt == wp->field_0x34) {
					selDesc = data->description;
				}
				dispOff += 0x18;
				dispCnt++;
			}
		}
		GXSetScissor(sc[0], sc[1], sc[2], sc[3]);
		if (selDesc != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9f4, float_neg240_8041dae0, float_neg130_8041dae4, float_480_8041dae8, float_70_8041daec,
			                      float_20_8041dad8);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(selDesc));
		}
		break;
	}

	case 2:
	case 3:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16: {
		// Item / badge / shop list window.
		ItemData* const dataBase = itemDataTable;
		const char* selDesc = 0;
		s32 xofs;
		const char* title;
		s32 listOff;
		s32 cnt;
		s32 n;
		s32 dispCnt;
		s32 dispOff;
		s32 mode50 = wp->field_0x50;
		s32 limit;
		GXColor color;
		u32 sc[4];

		if (mode50 == 2 || mode50 == 3) {
			cnt = pouchGetHaveItemCnt();
			listOff = ((8 - cnt > 0) ? (8 - pouchGetHaveItemCnt()) : 0) * 0x18;
			limit = 0x14;
			title = msgSearch(str_802F664C);
			xofs = 0x1E;
		} else if (mode50 == 12) {
			cnt = pouchGetHaveBadgeCnt();
			listOff = ((8 - cnt > 0) ? (8 - pouchGetHaveBadgeCnt()) : 0) * 0x18;
			limit = 0xC8;
			title = msgSearch(str_802F6660);
			xofs = 0x28;
		} else if (mode50 == 14) {
			limit = 0x152;
			cnt = 8 - wp->field_0x58;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			title = msgSearch(str_802F6674);
			xofs = 0x14;
		} else if (mode50 == 15) {
			limit = wp->field_0x58;
			cnt = 8 - wp->field_0x58;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			title = msgSearch(str_802F6674);
			xofs = 0x14;
		} else if (mode50 == 16) {
			limit = 0x152;
			cnt = 8 - wp->field_0x58;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			title = msgSearch(str_802F6688);
			xofs = 0x1E;
		} else {
			cnt = 8 - wp->field_0x58;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			limit = 0x63;
			title = msgSearch(str_802F6660);
			xofs = 0x28;
		}

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9f8, float_neg24_8041db44, float_100_8041db48, float_300_8041dad0, (f32)(0xdc - listOff),
		                      float_20_8041dad8);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9fc, float_51_8041db4c, float_114_8041db50, float_150_8041db0c, float_28_8041db54,
		                      float_8_8041daf0);
		FontDrawStart();
		FontDrawEdge();
		color = *(GXColor*)&dat_8041da00;
		FontDrawColor(&color);
		FontDrawString(title, (f32)(xofs + 0x33), float_111_8041db5c);
		FontDrawEdgeOff();

		if (wp->field_0x50 != 3) {
			Mtx scale;
			s32 iconId;
			Mtx mtx;

			PSMTXTrans(mtx, float_244_8041db68, float_82_8041db6c, float_0_8041db00);
			PSMTXScale(scale, float_0p6_8041db70, float_0p6_8041db70, float_0p6_8041db70);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGxCol(mtx, 0x10, 0x211, *(GXColor*)&dat_8041da04);
			if (wp->field_0x50 == 0xD) {
				iconId = 0x18D;
			} else if (wp->field_0x50 == 0x10) {
				iconId = 0x13F;
			} else {
				iconId = 0x18B;
			}
			{
				Vec pos = vec3_802f64c4;
				iconDispGxU(&pos, 0x10, iconId, float_0p6_8041db70);
			}
		}

		GXGetScissor(&sc[0], &sc[1], &sc[2], &sc[3]);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);

		dispOff = 0;
		dispCnt = 0;
		for (n = 0; n < limit; n++) {
			s32 item;
			s32 flag = 0;
			GXColor icolor = *(GXColor*)&dat_8041da08;

			switch (wp->field_0x50) {
			case 2:
			case 3:
				item = pouchHaveItem(n);
				break;
			case 12:
				item = pouchHaveBadge(n);
				break;
			case 11: {
				s16 got = badgeShop_get(bdsw->bargain_table, (s16)(n + 0xF0));
				if ((s32)got - badgeShop_ThrowCheck(n + 0xF0) > 0) {
					item = n + 0xF0;
				} else {
					item = 0;
				}
				break;
			}
			case 13:
				if ((s16)badgeShop_get(bdsw->starmaniac_table, (s16)(n + 0xF0)) > 0) {
					item = n + 0xF0;
				} else {
					item = 0;
				}
				break;
			case 14:
				if ((s16)badgeShop_get(bdsw->bottakuru_table, (s16)(n + 1)) > 0) {
					item = n + 1;
				} else {
					item = 0;
				}
				break;
			case 15:
				item = badge_bottakuru100_table[n];
				break;
			case 16:
				if ((s16)badgeShop_get(bdsw->bteresa_table, (s16)(n + 1)) > 0) {
					item = n + 1;
				} else {
					item = 0;
				}
				break;
			default:
				break;
			}

			if (item == 0) {
				continue;
			}
			{
				s32 price;
				ItemData* data = &dataBase[item];
				char buf[16];

				if (wp->field_0x50 == 0xD) {
					if (data->star_piece_price > pouchGetStarPiece()) {
						flag = 0x20;
						icolor = *(GXColor*)&dat_8041da0c;
					}
				}
				{
					Vec pos = vec3_802f64d0;

					pos.y = wp->field_0x44 + (f32)(0x38 - dispOff);
					iconDispGxU(&pos, (u16)(flag | 0x10), ((ItemDataView*)data)->icon_id, float_0p5_8041db60);
				}
				FontDrawStart();
				FontDrawScale(float_0p8_8041db74);
				{
					GXColor cc = icolor;
					FontDrawColor(&cc);
				}
				FontDrawString(data->id, float_36_8041db64, wp->field_0x44 + (f32)(0x50 - dispOff) - float_2_8041db78);
				if (wp->field_0x50 != 3) {
					f32 scale;
					s32 x;

					switch (wp->field_0x50) {
					case 0xE:
						price = ((ItemDataView*)data)->buy_price * 12 / 10;
						sprintf(buf, "%2d", price);
						scale = float_1_8041db80;
						x = 0;
						break;
					case 0xF:
						price = ((ItemDataView*)data)->buy_price * (evtGetValue(NULL, 0xF5DE06A9) + 11) / 10;
						sprintf(buf, "%3d", price);
						scale = float_1_8041db80;
						x = -0xE;
						break;
					case 0x10:
						price = (((ItemDataView*)data)->buy_price * 2 + 2) / 3;
						sprintf(buf, "%4d", price);
						scale = float_0p8_8041db74;
						x = -0x14;
						break;
					case 0xB:
						sprintf(buf, "%2d", ((ItemDataView*)data)->buy_price);
						scale = float_1_8041db80;
						x = 0;
						break;
					case 0xD:
						sprintf(buf, "%2d", data->star_piece_price);
						scale = float_1_8041db80;
						x = 0;
						break;
					default:
						sprintf(buf, "%2d", data->sell_price);
						scale = float_1_8041db80;
						x = 0;
						break;
					}
					FontDrawStart();
					FontDrawScale(scale);
					{
						GXColor cc = icolor;
						FontDrawColor(&cc);
					}
					FontDrawString(buf, (f32)(x + 0xE2), wp->field_0x44 + (f32)(0x50 - dispOff));
				}
				if ((f32)dispCnt == wp->field_0x34) {
					selDesc = data->description;
				}
			}
			dispOff += 0x18;
			dispCnt++;
		}
		GXSetScissor(sc[0], sc[1], sc[2], sc[3]);
		if (selDesc != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da10, float_neg240_8041dae0, float_neg130_8041dae4, float_480_8041dae8, float_70_8041daec,
			                      float_20_8041dad8);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(selDesc));
		}
		break;
	}

	case 17:
	case 18: {
		InformantWork* jdt = &_jdt;
		InformantInfo* info = jdt->info;
		s32 entry;
		s32 dispOff;
		s32 listOff;
		GXColor color;
		const u8* prefix;
		u32 sc[4];
		const char* title;
		s32 mode50 = wp->field_0x50;
		s32 m8 = 8 - wp->field_0x58;

		listOff = ((m8 > 0) ? m8 : 0) * 0x18;
		if (mode50 == 0x11) {
			title = msgSearch(str_802F669C);
			prefix = jdt->unk14.unkD;
		} else {
			title = msgSearch(str_802F66B0);
			prefix = jdt->unk14.unk1A;
		}
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da14, float_neg24_8041db44, float_100_8041db48, float_300_8041dad0, (f32)(0xdc - listOff),
		                      float_20_8041dad8);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da18, float_51_8041db4c, float_114_8041db50, float_150_8041db0c, float_28_8041db54,
		                      float_8_8041daf0);
		FontDrawStart();
		FontDrawEdge();
		color = *(GXColor*)&dat_8041da1c;
		FontDrawColor(&color);
		FontDrawString(title, float_71_8041db8c, float_111_8041db5c);
		FontDrawEdgeOff();
		if (wp->field_0x50 == 0x11) {
			Mtx mtx;
			Mtx scale;

			PSMTXTrans(mtx, float_244_8041db68, float_82_8041db6c, float_0_8041db00);
			PSMTXScale(scale, float_0p6_8041db70, float_0p6_8041db70, float_0p6_8041db70);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGxCol(mtx, 0x10, 0x211, *(GXColor*)&dat_8041da20);
			{
				Vec pos = vec3_802f64dc;

				iconDispGxU(&pos, 0x10, 0x18B, float_0p6_8041db70);
			}
		}
		GXGetScissor(&sc[0], &sc[1], &sc[2], &sc[3]);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);

		dispOff = 0;
		for (entry = 0; entry < wp->field_0x58; entry++) {
			InformantInfo* inf = &info[entry];
			char buf[0x20];
			char saved;
			GXColor tcolor;

			if (johoya_get((u8*)prefix, inf->storyId) != 0) {
				sprintf(buf, "");
				tcolor = *(GXColor*)&dat_8041da24;
			} else {
				sprintf(buf, "%3d", inf->unkA);
				tcolor = *(GXColor*)&dat_8041da28;
			}
			FontDrawStart();
			saved = ((char*)inf->initEndPos)[inf->tagEndPos];
			((char*)inf->initEndPos)[inf->tagEndPos] = 0;
			color = tcolor;
			FontDrawColor(&color);
			FontDrawScale(float_0p8_8041db74);
			FontDrawString((char*)inf->initEndPos, float_1_8041db80, wp->field_0x44 + (f32)(0x50 - dispOff));
			((char*)inf->initEndPos)[inf->tagEndPos] = saved;
			if (wp->field_0x50 == 0x11) {
				FontDrawStart();
				color = tcolor;
				FontDrawColor(&color);
				FontDrawString(buf, float_221_8041db94, wp->field_0x44 + (f32)(0x50 - dispOff));
			}
			dispOff += 0x18;
		}
		GXSetScissor(sc[0], sc[1], sc[2], sc[3]);
		break;
	}

	case 5:
	case 6:
	case 7: {
		// Party member stats window.
		GXColor color;
		s32 dispOff;
		u32 sc[4];
		s32 entry;
		char help[0x40];
		s32 listOff;
		s32 m8;

		*(UstBlk40*)help = *(const UstBlk40*)zero_802f64e8;
		m8 = 8 - wp->field_0x58;
		listOff = ((m8 > 0) ? m8 : 0) * 0x18;
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da2c, float_neg24_8041db44, float_100_8041db48, float_300_8041dad0, (f32)(0xdc - listOff),
		                      float_20_8041dad8);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da30, float_16_8041db98, float_114_8041db50, float_120_8041dacc, float_28_8041db54,
		                      float_8_8041daf0);
		FontDrawStart();
		FontDrawEdge();
		color = *(GXColor*)&dat_8041da34;
		FontDrawColor(&color);
		FontDrawString(msgSearch(str_802F66C4), float_41_8041db9c, float_111_8041db5c);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da38, float_167_8041dba0, float_114_8041db50, float_85_8041dba4, float_28_8041db54,
		                      float_8_8041daf0);
		FontDrawStart();
		FontDrawEdge();
		color = *(GXColor*)&dat_8041da3c;
		FontDrawColor(&color);
		FontDrawString(msgSearch(str_802F66D8), float_177_8041dba8, float_111_8041db5c);
		FontDrawEdgeOff();
		GXGetScissor(&sc[0], &sc[1], &sc[2], &sc[3]);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);

		for (entry = 0, dispOff = 0; party_id_table[entry] != 0; entry++) {
			s32 iconId;
			GXColor nameColor;
			char buf[16];
			s32 flag;
			const char* name;
			s32 lvl;

			if (wp->field_0x50 == 5 || wp->field_0x50 == 6) {
				if (!partyChkJoin(party_id_table[entry])) {
					continue;
				}
				if (evtGetValue(NULL, 0xF8406022) == 0) {
					if (pouch->partyData[party_id_table[entry]].techLevel < 1) {
						nameColor = *(GXColor*)&dat_8041da40;
						lvl = 0;
						flag = 0;
					} else {
						nameColor = *(GXColor*)&dat_8041da44;
						lvl = 1;
						flag = 0x20;
					}
				} else {
					if (pouch->partyData[party_id_table[entry]].techLevel < 2) {
						nameColor = *(GXColor*)&dat_8041da48;
						lvl = 0;
						flag = 0;
					} else {
						nameColor = *(GXColor*)&dat_8041da4c;
						lvl = 2;
						flag = 0x20;
					}
				}
			} else {
				if (!partyChkJoin(party_id_table[entry]) || wp->field_0x4C == party_id_table[entry]) {
					continue;
				}
				if (pouch->partyData[party_id_table[entry]].techLevel > 0) {
					nameColor = *(GXColor*)&dat_8041da50;
					lvl = 0;
					flag = 0;
				} else {
					nameColor = *(GXColor*)&dat_8041da54;
					lvl = 1;
					flag = 0x20;
				}
			}

			{
				Vec pos = vec3_802f6528;

				pos.y = wp->field_0x44 + (f32)(0x38 - dispOff);
				iconId = party_icon_table[entry];
				if (iconId == 0x158) {
					switch (pouchGetPartyColor(4)) {
					case 0: iconId = 0x158; break;
					case 1: iconId = 0x159; break;
					case 2: iconId = 0x15A; break;
					case 3: iconId = 0x15B; break;
					case 4: iconId = 0x15C; break;
					case 5: iconId = 0x15D; break;
					case 6:
					default: iconId = 0x15E; break;
					}
				}
				iconDispGxU(&pos, flag | 0x10, iconId, float_0p5_8041db60);
			}
			FontDrawStart();
			{ GXColor cc = nameColor; FontDrawColor(&cc); }
			switch (entry) {
			case 0: name = msgSearch(str_802F66EC); break;
			case 1: name = msgSearch(str_802F66F8); break;
			case 2: name = msgSearch(str_802F6704); break;
			case 3: name = pouchGetYoshiName(); break;
			case 4: name = msgSearch(str_802F6710); break;
			case 5: name = msgSearch(str_802F671C); break;
			case 6: name = msgSearch(str_802F6728); break;
			}
			FontDrawString(name, float_36_8041db64, wp->field_0x44 + (f32)(0x50 - dispOff));
			sprintf(buf, "%d", pouch->partyData[party_id_table[entry]].techLevel);
			FontDrawStart();
			FontDrawString(buf, float_236_8041dbb0, wp->field_0x44 + (f32)(0x50 - dispOff));
			if ((f32)entry == wp->field_0x34) {
				char help2[0x10];
				char lank[0x10];

				if (lvl == 0) {
					sprintf(help2, "help");
				} else {
					sprintf(help2, str_802F6734);
				}
				if (wp->field_0x50 == 5 || wp->field_0x50 == 6) {
					sprintf(lank, str_802F673C);
				} else {
					sprintf(lank, str_802F6744);
				}
				sprintf(help, str_802F6750, help2, lank, pouch->partyData[party_id_table[entry]].techLevel + 1, party_labelname_table[entry]);
			}
			dispOff += 0x18;
		}
		GXSetScissor(sc[0], sc[1], sc[2], sc[3]);
		if ((s8)help[0] != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da58, float_neg240_8041dae0, float_neg130_8041dae4, float_480_8041dae8, float_70_8041daec,
			                      float_20_8041dad8);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(help));
		}
		break;
	}

	case 8:
		UNITWIN_STATUS_DISP(wp, dat_8041da5c, dat_8041da60, dat_8041da64, dat_8041da68, dat_8041da6c, dat_8041da70, dat_8041da74, dat_8041da78, str_802F6788, UST_GUARD_NONE, UST_CHG_A, zero_802f6534, UST_TAIL_A, vec3_802f6574, wp->field_0x58);
		break;
	case 9:
		UNITWIN_STATUS_DISP(wp, dat_8041da7c, dat_8041da80, dat_8041da84, dat_8041da88, dat_8041da8c, dat_8041da90, dat_8041da94, dat_8041da98, str_802F6788, UST_GUARD_NONE, UST_CHG_B, zero_802f65a4, UST_TAIL_PLUS, vec3_802f65e4, wp->field_0x58);
		break;
	case 10:
		UNITWIN_STATUS_DISP(wp, dat_8041da9c, dat_8041daa0, dat_8041daa4, dat_8041daa8, dat_8041daac, dat_8041dab0, dat_8041dab4, dat_8041dab8, str_802F6798, UST_GUARD_ROW, UST_CHG_C, zero_802f65f0, UST_TAIL_MINUS, vec3_802f6630, 3);
		break;

	default:
		break;
	}

	// --- common tail: title/help window selected by wp->field_0x50 ---
	if (wp->field_0x50 != 0 && wp->field_0x50 != 1) {
		GXColor color;
		s32 i;
		char* p;
		char buf[0x88];
		s32 lines;
		char* second = NULL;

		switch (wp->field_0x50) {
		case 11:
		case 14:
		case 15: strcpy(buf, msgSearch(str_802F67A8)); break;
		case 17: strcpy(buf, msgSearch(str_802F67BC)); break;
		case 18: strcpy(buf, msgSearch(str_802F67D0)); break;
		case 13:
		case 16: strcpy(buf, msgSearch(str_802F67E4)); break;
		case 2:
		case 12: strcpy(buf, msgSearch(str_802F67FC)); break;
		case 3: strcpy(buf, msgSearch(str_802F6810)); break;
		case 4: strcpy(buf, msgSearch(str_802F6824)); break;
		case 5: strcpy(buf, msgSearch(str_802F6838)); break;
		case 6: strcpy(buf, msgSearch(str_802F6838)); break;
		case 7: strcpy(buf, msgSearch(str_802F6850)); break;
		case 8:
		case 9: strcpy(buf, msgSearch(str_802F6868)); break;
		case 10: strcpy(buf, msgSearch(str_802F687C)); break;
		default: break;
		}
		p = buf;
		lines = 1;
		for (i = 0; ((s8*)p)[i] != 0; i++) {
			if (((s8*)p)[i] == '\n') {
				buf[i] = 0;
				second = &buf[i] + 1;
				lines = 2;
				break;
			}
		}
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041dabc, float_neg280_8041dbd4, float_100_8041db48, float_240_8041daf8,
		                      (f32)((lines - 1) * 0x18 + 0x2D), float_20_8041dad8);
		FontDrawStart();
		{
			s32 w1;
			s32 w2;

			if (p != NULL) {
				w1 = FontGetMessageWidth(p);
			} else {
				w1 = 0;
			}
			if (second != NULL) {
				w2 = FontGetMessageWidth(second);
			} else {
				w2 = 0;
			}
			if (w2 > w1) {
				w1 = w2;
			}
			if (w1 <= 0xE4) {
				w1 = (0xE4 - w1) / 2;
			} else {
				FontDrawScale(float_228_8041dbd8 / (f32)w1);
				w1 = 0;
			}
			FontDrawString(p, (f32)(w1 - 0x112), float_90_8041dbdc);
			if (second != NULL) {
				FontDrawString(second, (f32)(w1 - 0x112), float_66_8041dbe0);
			}
		}
		iconFlagOff(str_802F6894, 2);
		iconSetPos(str_802F6894, wp->field_0x3C, wp->field_0x40, float_0_8041db00);
	} else {
		iconFlagOn(str_802F6894, 2);
	}
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
#ifdef MUST_MATCH
	// The original shares one buttons=0 tail between the two checks; the natural
	// short-circuit below materialises the second compare instead of branching on it.
	{
		BOOL notCtrl = !marioChkCtrl();

		do {
			if (!notCtrl) {
				notCtrl = !marioChkKey();
				if (!notCtrl) {
					break;
				}
			}
			buttons = 0;
		} while (0);
	}
#else
	if (!marioChkCtrl() || !marioChkKey()) {
		buttons = 0;
	}
#endif

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

USER_FUNC(unitwincoin_wait) {
	UnitWinWork* wp = _wp;
	s32 coin;

	pouchGetPtr();
	coin = pouchGetCoin();
	return (wp->field_0x2C != coin) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
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
#ifdef MUST_MATCH
			iconEntry(name, *(u16*)&itemDataTable[wp->field_0x8[i]].icon_id);
#else
			iconEntry(name, (u16)itemDataTable[wp->field_0x8[i]].icon_id);
#endif
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

void unk_8017599c(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	Mtx mtx;
	Vec pos;
	char buf[0x40];
	GXColor titleColor;
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
#ifdef MUST_MATCH
		{
			// The named local keeps 0.5f in f2 and the int->float mask in f3; with the
			// literal (or the named float symbol) the two swap (cf. envdrv.c).
			f32 half = 0.5f;
			pos.x *= (f32)gp->fbWidth * half;
			pos.y *= (f32)gp->efbHeight * half;
		}
#else
		pos.x *= (f32)gp->fbWidth * float_0p5_8041db60;
		pos.y *= (f32)gp->efbHeight * float_0p5_8041db60;
#endif
		sprintf(buf, str_8041DBAC, itemDataTable[wp->field_0x8[i]].discount_price);
		PSMTXTrans(mtx, float_12_8041dbfc * (f32)(strlen(buf) - 1) + pos.x, pos.y, pos.z);
		iconNumberDispGx(mtx, itemDataTable[wp->field_0x8[i]].discount_price, 0, *(GXColor*)&dat_8041d9b8);
	}
}

#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
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
void FontDrawColor(GXColor* color);
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

#ifdef MUST_MATCH
// Every retail caller of FontDrawColor copies the colour to the stack first: the original takes it by
// value (fontmgr.h says GXColor*). iconDispGx's call sites convert the flag and icon as u16 (clrlwi), not
// s16 (extsh).
#define FontDrawColorV(c) ((void (*)(GXColor))FontDrawColor)(c)
// The original's iconDispGx takes the scale first: its callers evaluate it before the position.
#define iconDispGxS ((void (*)(f32, Vec*, u16, u16))iconDispGx)
// The original's FontDrawString takes (x, y, str): its callers evaluate the coordinates before the
// string. The registers are the same either way (f1, f2, r3).
#define FontDrawString(s, x, y) ((void (*)(f32, f32, const char*))FontDrawString)(x, y, s)
#else
#define FontDrawColorV(c) do { GXColor fontColor_ = (c); FontDrawColor(&fontColor_); } while (0)
static inline void iconDispGxS(f32 scale, Vec* pos, u16 flags, u16 iconId) { iconDispGx(scale, pos, flags, iconId); }
#endif

// evt_unitwin_disp_func is a display callback, not an event command.



extern ItemData itemDataTable[];
extern const f32 float_0_8041db00[1];
extern const f32 float_1p1_8041db30[1];
extern u32 dat_8041d9bc;
extern u32 dat_8041d9b4;
extern u32 dat_8041d9b8;
extern const f32 float_108_8041dadc[1];
extern char str_8041DAC0[];

// The .sdata2 objects the unit names (floats up to the first pooled one, colour words, a few strings) are defined as one-element
// const arrays and read as name[0] (a plain const f32 would be folded to an anonymous pool entry), in the original's order, in
// the .sdata2 block below; the flush helpers before each function keep the pooled literals in source order after them.
extern const f32 float_neg150_8041dac8[1];
extern const f32 float_120_8041dacc[1];
extern const f32 float_300_8041dad0[1];
extern const f32 float_50_8041dad4[1];
extern const f32 float_20_8041dad8[1];
extern const f32 float_neg240_8041dae0[1];
extern const f32 float_neg130_8041dae4[1];
extern const f32 float_480_8041dae8[1];
extern const f32 float_70_8041daec[1];
extern const f32 float_8_8041daf0[1];
extern const f32 float_neg304_8041daf4[1];
extern const f32 float_240_8041daf8[1];
extern const f32 float_60_8041dafc[1];
extern const f32 float_neg250_8041db04[1];
extern const f32 float_neg75_8041db08[1];
extern const f32 float_150_8041db0c[1];
extern const f32 float_45_8041db10[1];
extern const f32 float_neg220_8041db14[1];
extern const f32 float_neg113_8041db18[1];
extern const f32 float_0p7_8041db1c[1];
extern const f32 float_neg192_8041db20[1];
extern const f32 float_neg126_8041db24[1];
extern const f32 float_neg210_8041db28[1];
extern const f32 float_neg166_8041db2c[1];
extern const f32 float_neg168_8041db34[1];
extern const f32 float_neg172_8041db38[1];
extern const f32 float_neg24_8041db44[1];
extern const f32 float_100_8041db48[1];
extern const f32 float_51_8041db4c[1];
extern const f32 float_114_8041db50[1];
extern const f32 float_28_8041db54[1];
extern const f32 float_81_8041db58[1];
extern const f32 float_111_8041db5c[1];
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
extern s32 badge_bottakuru100_table[];
extern BadgeShopWork* bdsw;

// Work structure for the unit window, not yet extracted into a header.
typedef struct UnitWinObj {
	const char* name;      // 0x0
	const char* hitName;   // 0x4, compared with hitGetName()
	void* evtScript;       // 0x8, event script run when the object is used
} UnitWinObj;             // size 0xC

typedef struct UnitWinWork {
	u16 flags;                  // 0x0, UNITWIN_FLAG_*
	u16 pad_02;                 // 0x2
	UnitWinObj* objList;        // 0x4
	s32* items;                 // 0x8, item id per object (0 = none)
	u16 taken[6];               // 0xC, per object: bit 0 set once the object is done
	s32 count;                  // 0x18
	EventEntry* mainEvt;        // 0x1C
	EventEntry* useEvt;         // 0x20
	s32 index;                  // 0x24
	s32 hideTimer;              // 0x28, frames until the box closes
	s32 dispCoin;               // 0x2C, coin count shown (counts toward the pouch value)
	s32 dispNum;                // 0x30, shop points / shine sprites shown
	f32 cursorRow;              // 0x34, row under the cursor (compared with the list index)
	u8 pad_0x38[0x4];           // 0x38
	f32 cursorX;                // 0x3C
	f32 cursorY;                // 0x40
	f32 listY;                  // 0x44
	u8 pad_0x48[0x4];           // 0x48
	s32 excludeMember;          // 0x4C, party member left out of the list
	s32 mode;                   // 0x50, which list the window shows
	u16 freeState;              // 0x54, bit 0 requests free, bit 1 is set once the event has run
	u8 pad_0x56[2];             // 0x56
	s32 rowCount;               // 0x58
	s32 coinTick;               // 0x5C, animation counter for the coin count (mod 1200)
	s32 winId[3];               // 0x60, winMgr entries
	u8 pad_0x6C[0x74];          // 0x6C
	s32 selObj;                 // 0xE0, selected object index (-1 = none)
	s32 useTaken;               // 0xE4, set by the script when the use was accepted
} UnitWinWork;

static int party_id_table[8] = { PARTY_MEMBER_GOOMBELLA, PARTY_MEMBER_KOOPS, PARTY_MEMBER_FLURRIE, PARTY_MEMBER_YOSHI, PARTY_MEMBER_VIVIAN, PARTY_MEMBER_BOBBERY, PARTY_MEMBER_MS_MOWZ, PARTY_MEMBER_NONE };
static u32 party_icon_table[7] = { 0x155, 0x156, 0x15F, 0x158, 0x160, 0x157, 0x161 };
static const char* party_labelname_table[7] = { "_kur", "_nok", "_win", "_yos", "_viv", "_bom", "_chu" };
static u32 mario_status_icon_table[3] = { 0x19F, 0x1A0, 0x1A1 };
static const char* mario_status_labelname_table[3] = { "_hp", "_fp", "_bp" };
static const char* mario_status_name_table[3] = { "\x82\x67\x82\x6F", "\x82\x65\x82\x6F", "\x82\x61\x82\x6F" };
static s32 mario_status_henka_table[3] = { 5, 5, 3 };
extern const char str_8041DB3C[];
static u32 main_evt[] = { 0x0001005B, (u32)evt_unitwin_main_func, 0x2, 0x1 };
u32 select_evt[] = { 0x0001005B, (u32)unk_80174e98, 0x2, 0x1 };
WinMgrDesc unk_80375478[3] = {
	{ 0, 0, 8, -150, 120, 300, 50, { 0x2B, 0xA4, 0xFF, 0xFF }, NULL, unk_8017599c },
	{ 0, 0, 8, -240, -130, 500, 80, { 0xF0, 0xF0, 0xF0, 0xFF }, unk_8017597c, unk_801758fc },
	{ 2, 0, 8, -250, -130, 150, 45, { 0xF0, 0xF0, 0xF0, 0xFF }, NULL, unk_80175710 },
};
#pragma force_active on
u32 gap_08_80418A0C_sbss;
#pragma force_active reset
static s16* mario_status_point_table[3];
s32 unk_804189fc;
static UnitWinWork* _wp;

// .sdata2 objects, in the original's order.
__declspec(section ".sdata2") u32 dat_8041d9b4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9b8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9bc = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9c0 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041d9c4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9c8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9cc = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9d0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9d4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9d8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9dc = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9e0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9e4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9e8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9ec = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041d9f0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9f4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9f8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041d9fc = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da00 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da04 = 0xF0F0F0FF;
__declspec(section ".sdata2") u32 dat_8041da08 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041da0c = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041da10 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da14 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da18 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da1c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da20 = 0xF0F0F0FF;
__declspec(section ".sdata2") u32 dat_8041da24 = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041da28 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041da2c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da30 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da34 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da38 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da3c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da40 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041da44 = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041da48 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041da4c = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041da50 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041da54 = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041da58 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da5c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da60 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da64 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da68 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da6c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da70 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041da74 = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041da78 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da7c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da80 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da84 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da88 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041da8c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da90 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041da94 = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041da98 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041da9c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041daa0 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041daa4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041daa8 = 0x2BA4FFFF;
__declspec(section ".sdata2") u32 dat_8041daac = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041dab0 = 0x000000FF;
__declspec(section ".sdata2") u32 dat_8041dab4 = 0xA0A0A0FF;
__declspec(section ".sdata2") u32 dat_8041dab8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041dabc = 0xFFFFFFFF;
__declspec(section ".sdata2") char str_8041DAC0[] = "uwn%d";
__declspec(section ".sdata2") const f32 float_neg150_8041dac8[1] = { -150.0f };
__declspec(section ".sdata2") const f32 float_120_8041dacc[1] = { 120.0f };
__declspec(section ".sdata2") const f32 float_300_8041dad0[1] = { 300.0f };
__declspec(section ".sdata2") const f32 float_50_8041dad4[1] = { 50.0f };
__declspec(section ".sdata2") const f32 float_20_8041dad8[1] = { 20.0f };
__declspec(section ".sdata2") const f32 float_108_8041dadc[1] = { 108.0f };
__declspec(section ".sdata2") const f32 float_neg240_8041dae0[1] = { -240.0f };
__declspec(section ".sdata2") const f32 float_neg130_8041dae4[1] = { -130.0f };
__declspec(section ".sdata2") const f32 float_480_8041dae8[1] = { 480.0f };
__declspec(section ".sdata2") const f32 float_70_8041daec[1] = { 70.0f };
__declspec(section ".sdata2") const f32 float_8_8041daf0[1] = { 8.0f };
__declspec(section ".sdata2") const f32 float_neg304_8041daf4[1] = { -304.0f };
__declspec(section ".sdata2") const f32 float_240_8041daf8[1] = { 240.0f };
__declspec(section ".sdata2") const f32 float_60_8041dafc[1] = { 60.0f };
__declspec(section ".sdata2") const f32 float_0_8041db00[1] = { 0.0f };
__declspec(section ".sdata2") const f32 float_neg250_8041db04[1] = { -250.0f };
__declspec(section ".sdata2") const f32 float_neg75_8041db08[1] = { -75.0f };
__declspec(section ".sdata2") const f32 float_150_8041db0c[1] = { 150.0f };
__declspec(section ".sdata2") const f32 float_45_8041db10[1] = { 45.0f };
__declspec(section ".sdata2") const f32 float_neg220_8041db14[1] = { -220.0f };
__declspec(section ".sdata2") const f32 float_neg113_8041db18[1] = { -113.0f };
__declspec(section ".sdata2") const f32 float_0p7_8041db1c[1] = { 0.699999988079071f };
__declspec(section ".sdata2") const f32 float_neg192_8041db20[1] = { -192.0f };
__declspec(section ".sdata2") const f32 float_neg126_8041db24[1] = { -126.0f };
__declspec(section ".sdata2") const f32 float_neg210_8041db28[1] = { -210.0f };
__declspec(section ".sdata2") const f32 float_neg166_8041db2c[1] = { -166.0f };
__declspec(section ".sdata2") const f32 float_1p1_8041db30[1] = { 1.100000023841858f };
__declspec(section ".sdata2") const f32 float_neg168_8041db34[1] = { -168.0f };
__declspec(section ".sdata2") const f32 float_neg172_8041db38[1] = { -172.0f };
__declspec(section ".sdata2") const char str_8041DB3C[] = "%04d";
__declspec(section ".sdata2") const f32 float_neg24_8041db44[1] = { -24.0f };
__declspec(section ".sdata2") const f32 float_100_8041db48[1] = { 100.0f };
__declspec(section ".sdata2") const f32 float_51_8041db4c[1] = { 51.0f };
__declspec(section ".sdata2") const f32 float_114_8041db50[1] = { 114.0f };
__declspec(section ".sdata2") const f32 float_28_8041db54[1] = { 28.0f };
__declspec(section ".sdata2") const f32 float_81_8041db58[1] = { 81.0f };
__declspec(section ".sdata2") const f32 float_111_8041db5c[1] = { 111.0f };

#pragma pool_data on // the original addresses its .rodata through one base register
void unk_8017599c(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	Mtx mtx;
	Vec pos;
	char buf[0x40];
	GXColor titleColor;
	const char* msg;
	s32 i;

	msg = msgSearch(itemDataTable[wp->items[wp->index]].name);
	if (winMgrAction(wp->winId[0]) == 0) {
		FontDrawStart();
		FontDrawEdge();
		*(u32*)&titleColor = dat_8041d9b4;
		FontDrawColor(&titleColor);
		FontDrawString(msg, (f32)(-(s32)FontGetMessageWidth(msg) / 2), float_108_8041dadc[0]);
		FontDrawEdgeOff();
	}
	for (i = 0; i < wp->count; i++) {
		if (wp->taken[i] & 1) {
			continue;
		}
		mapObjGetPos(wp->objList[i].name, &pos);
		pos.y += 30.0f;
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
		pos.x *= (f32)gp->fbWidth * 0.5f;
		pos.y *= (f32)gp->efbHeight * 0.5f;
#endif
		sprintf(buf, "%d", itemDataTable[wp->items[i]].discount_price);
		PSMTXTrans(mtx, 12.0f * (f32)(strlen(buf) - 1) + pos.x, pos.y, pos.z);
		iconNumberDispGx(mtx, itemDataTable[wp->items[i]].discount_price, 0, *(GXColor*)&dat_8041d9b8);
	}
}

void unk_8017597c(WinMgrEntry* entry) {
	winMgrHelpMain(entry);
}

// The original keeps the byte offset (index*4) in a register across winMgrAction
// and indexes with lwzx; writing `list[wp->index]` makes the compiler defer the
// shift past the call, so compute the byte offset explicitly.
void unk_801758fc(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	s32 offset = wp->index * 4;
	s32* list = wp->items;

	if (!winMgrAction(wp->winId[1])) {
		entry->message = msgSearch(itemDataTable[*(s32*)((u8*)list + offset)].description);
		winMgrHelpDraw(entry);
	}
}

void unk_80175710(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	PouchData* pouch = pouchGetPtr();

	if (!(entry->flags & 4)) {
		Mtx mtx;
		Mtx scale;

		PSMTXTrans(mtx, 10.0f + (30.0f + (f32)entry->x), (f32)entry->y - 36.0f, float_0_8041db00[0]);
		if ((u32)gp->language == 0) {
			PSMTXScale(scale, float_1p1_8041db30[0], float_1p1_8041db30[0], float_1p1_8041db30[0]);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x1F1);
		} else {
			PSMTXScale(scale, 0.800000011920929f, float_1p1_8041db30[0], float_1p1_8041db30[0]);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGx2(mtx, 0x10, 0x20A);
		}
		PSMTXTrans(mtx, 124.0f + (f32)entry->x, (f32)entry->y - 38.0f, float_0_8041db00[0]);
		iconNumberDispGx(mtx, wp->dispNum, 0, *(GXColor*)&dat_8041d9bc);
		{
			s32 delta = pouch->mShopPoints - wp->dispNum;

			if (delta != 0) {
				delta /= 10;
				if (delta == 0) {
					delta = (pouch->mShopPoints > wp->dispNum) ? 1 : -1;
				}
				wp->dispNum += delta;
			}
		}
	}
}

USER_FUNC(evt_unitwin_alloc) {
	s32 flag = evtGetValue(event, event->args[0]);
	UnitWinWork* wp = _mapAlloc(0xE8);

	memset(wp, 0, 0xE8);
	wp->mode = 0;
	_wp = wp;
	if (flag) {
		wp->flags |= 0x80;
	} else {
		wp->flags &= ~0x80;
	}
	iconEntry2D("yubicursor_2", 0x1F0);
	iconFlagOn("yubicursor_2", 2);
	iconFlagOn("yubicursor_2", 4);
	if (!(wp->flags & 0x80)) {
		wp->mainEvt = evtEntry(main_evt, 0, 0);
		wp->mainEvt->lwData[0] = (s32)wp;
	}
	mario_status_point_table[0] = &pouchGetPtr()->baseMaxHP;
	mario_status_point_table[1] = &pouchGetPtr()->baseMaxFP;
	mario_status_point_table[2] = &pouchGetPtr()->maximumBP;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_unitwin_free) {
	UnitWinWork* wp = _wp;
	s32 i;

	if (wp == NULL) {
		return EVT_RETURN_DONE;
	}
	if (isFirstCall) {
		wp->freeState |= 1;
	}
	if (!(wp->freeState & 2)) {
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
			winMgrDelete(wp->winId[i]);
		}
	}
	iconDelete("yubicursor_2");
	_mapFree(_wp);
	_wp = NULL;
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
	wp->items = itemList;
	wp->count = 0;
	for (i = 0; wp->objList[i].name != NULL; i++) {
		char name[32];

		wp->count++;
		mapObjGetPos(wp->objList[i].name, &pos);
		mapObjFlagOn(wp->objList[i].name, 0x4000);
		if (wp->items[i] != 0) {
			sprintf(name, str_8041DAC0, i);
#ifdef MUST_MATCH
			iconEntry(name, *(u16*)&itemDataTable[wp->items[i]].icon_id);
#else
			iconEntry(name, (u16)itemDataTable[wp->items[i]].icon_id);
#endif
			iconSetPos(name, pos.x, 6.0f + pos.y, pos.z);
			iconFlagOn(name, 0x40);
		} else {
			wp->taken[i] |= 1;
		}
	}
	wp->mainEvt = evtEntryType(select_evt, 0, 0x20, 0);
	wp->mainEvt->lwData[0] = (s32)wp;
	for (i = 0; i < 3; i++) {
		wp->winId[i] = winMgrEntry(&unk_80375478[i]);
	}
	wp->selObj = -1;
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwincoin_disp_onoff) {
	s32 on = evtGetValue(event, event->args[0]);
	UnitWinWork* wp;

	pouchGetPtr();
	wp = _wp;
	wp->dispCoin = pouchGetCoin();
	wp->coinTick = 0;
	if (on) {
		wp->flags |= 0x1000;
	} else {
		wp->flags &= ~0x1000;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwincoin_wait) {
	UnitWinWork* wp = _wp;
	s32 coin;

	pouchGetPtr();
	coin = pouchGetCoin();
	return (wp->dispCoin != coin) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
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

USER_FUNC(unk_801751f8) {
	s32* args = event->args;
	UnitWinWork* wp = (UnitWinWork*)evtGetValue(event, args[0]);

	wp->useTaken = evtGetValue(event, args[1]);
	return EVT_RETURN_DONE;
}

USER_FUNC(unk_80174e98) {
	UnitWinWork* wp = (UnitWinWork*)event->lwData[0];
	s32 selected = 0;
	MarioWork* mario = marioGetPtr();
	PartyEntry* party = partyGetPtr(0);
	HitEntry* hit = (HitEntry*)mario->hitobjStandOn;
	s32 buttons;
	s32 i;

	if (wp->freeState & 1) {
		wp->freeState = wp->freeState | 2;
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

	if (party != NULL && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
		if (wp->selObj != -1) {
			wp->selObj = -1;
			winMgrClose(wp->winId[0]);
			winMgrClose(wp->winId[1]);
		}
		return EVT_RETURN_BLOCK;
	}

	if (winGhostDiaryChk() || N_itemPickUpFromFieldCheck()) {
		if (wp->selObj != -1) {
			wp->selObj = -1;
			winMgrClose(wp->winId[0]);
			winMgrClose(wp->winId[1]);
		}
		return EVT_RETURN_BLOCK;
	}

	if (!(wp->flags & 2)) {
		if (hit != NULL) {
			for (i = 0; i < wp->count; i++) {
				if (strcmp(wp->objList[i].hitName, hitGetName(hit)) == 0) {
					break;
				}
			}
			if (!(wp->taken[i] & 1) && i < wp->count) {
				if (wp->selObj != i) {
					if (wp->selObj == -1) {
						winMgrOpen(wp->winId[0]);
						winMgrOpen(wp->winId[1]);
					}
					wp->selObj = i;
					wp->index = i;
					winMgrHelpInit(winMgrGetPtr(wp->winId[1]));
				}
				selected = 1;
			}
		}
		if (selected == 0 && wp->selObj != -1) {
			wp->selObj = -1;
			winMgrClose(wp->winId[0]);
			winMgrClose(wp->winId[1]);
		}
		if (wp->selObj != -1) {
			wp->flags |= 1;
			if (buttons & 0x100) {
				marioKeyOff();
				wp->useEvt = evtEntry(wp->objList[wp->index].evtScript, 0, 0);
				wp->useEvt->lwData[0] = (s32)wp;
				wp->flags |= 2;
				winMgrClose(wp->winId[0]);
				winMgrClose(wp->winId[1]);
				wp->selObj = -1;
				wp->useTaken = 0;
			}
		}
	} else {
		if (evtCheckID(wp->useEvt->eventId) == 0) {
			wp->flags &= ~2;
			marioKeyOn();
			if (wp->useTaken != 0) {
				wp->taken[wp->index] |= 1;
			}
		}
	}

	for (i = 0; i < wp->count; i++) {
		MapObject* obj = mapGetMapObj(wp->objList[i].name);
		if (obj != NULL) {
			char name[32];

			sprintf(name, str_8041DAC0, i);
			if (wp->taken[i] & 1) {
				iconFlagOn(name, 2);
			} else if (*(u32*)obj & 1) {
				iconFlagOn(name, 2);
			} else {
				iconFlagOff(name, 2);
			}
		}
	}
	return EVT_RETURN_BLOCK;
}

/// @todo reconstructed: retail's evt_win .rodata has "%s_%04d" (str_802F68B0), which no retail function references,
/// between the signed and unsigned conversion doubles, at this slot. Invented: the name, the arguments and the body
/// (deletes the icon named "<name>_<no>").
#ifdef MUST_MATCH
USER_FUNC(unitwin_icon_delete) {
	s32* args = event->args;
	const char* name = (const char*)evtGetValue(event, args[0]);
	s32 no = evtGetValue(event, args[1]);
	char buf[32];

	sprintf(buf, "%s_%04d", name, no);
	iconDelete(buf);
	return EVT_RETURN_DONE;
}
#endif

USER_FUNC(evt_unitwin_main_func) {
	UnitWinWork* wp = (UnitWinWork*)event->lwData[0];

	if (wp->freeState & 1) {
		wp->freeState = wp->freeState | 2;
		return EVT_RETURN_DONE;
	}

	if (wp->hideTimer > 0) {
		wp->hideTimer--;
	} else {
		wp->flags &= ~1;
	}
	dispEntry(8, 1000.0f, 0, evt_unitwin_disp_func, wp);
	return EVT_RETURN_BLOCK;
}

// Yoshi's icon follows the colour he hatched with.
static inline s32 unitwin_party_icon(s32 iconId) {
	if (iconId == 0x158) {
		switch (pouchGetPartyColor(PARTNER_YOSHI)) {
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
	return iconId;
}

// The partner names, in party_id_table order.
static inline const char* unitwin_party_name(s32 i) {
	const char* name;

	switch (i) {
	case 0: name = msgSearch("name_party0"); break;
	case 1: name = msgSearch("name_party1"); break;
	case 2: name = msgSearch("name_party4"); break;
	case 3: name = pouchGetYoshiName(); break;
	case 4: name = msgSearch("name_party5"); break;
	case 5: name = msgSearch("name_party2"); break;
	case 6: name = msgSearch("name_party6"); break;
	}
	return name;
}

// Display callback for the shop window. The list rows sit at dispCnt * 0x18 (i * 0x18 for the
// informant list), which MWCC strength-reduces into a per-loop offset register; the loop counter `i`
// and a few flags are function-scope locals reused by every block.
void evt_unitwin_disp_func(CameraId cameraId, void* param) {
	UnitWinWork* wp = (UnitWinWork*)param;
	PouchData* pouch = pouchGetPtr();
	s32 i;
	s32 blink;
	s32 changed;
	s32 lvl;

	// --- flags & 1: selected item's name/description, plus the on-map price icons ---
	if (wp->flags & 1) {
		u32 len;
		s32 sellItem = wp->items[wp->index];
		const char* sellName = itemDataTable[sellItem].id;
		const char* sellDesc = itemDataTable[sellItem].description;

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9c0, float_neg150_8041dac8[0], float_120_8041dacc[0], float_300_8041dad0[0], float_50_8041dad4[0],
		                      float_20_8041dad8[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041d9c4);
		len = strlen(sellName);
		FontDrawString(sellName, (f32)(s32)(((len * 0x18) >> 2) * -1), float_108_8041dadc[0]);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9c8, float_neg240_8041dae0[0], float_neg130_8041dae4[0], float_480_8041dae8[0], float_70_8041daec[0],
		                      float_20_8041dad8[0]);
		FontDrawStart();
		FontDrawMessage(-220, -138, msgSearch(sellDesc));

		for (i = 0; i < wp->count; i++) {

			if (wp->items[i] != 0) {
				Mtx mtx;
				f32 projv[7];
				u8 price;
				f32 vp[6];
				f32 sx, sy, sz;
				CameraEntry* cam;
				Vec pos;

				mapObjGetPos(wp->objList[i].name, &pos);
				cam = camGetPtr(CAMERA_3D);
				GXSetProjection(cam->projection, cam->projType);
				GXGetProjectionv(projv);
				GXGetViewportv(vp);
				GXProject(pos.x, pos.y, pos.z, cam->view, projv, vp, &sx, &sy, &sz);
				cam = camGetPtr(cameraId);
				GXSetProjection(cam->projection, cam->projType);
				{
					s32 item = wp->items[i];
					if (item >= BADGE_MIN && item < BADGE_MAX) {
						price = (u16)itemDataTable[item].discount_price;
					} else {
						price = ((ItemDataView*)&itemDataTable[item])->buy_price;
					}
				}
				PSMTXTrans(mtx, float_8_8041daf0[0] + (float_neg304_8041daf4[0] + sx),
				           float_240_8041daf8[0] - (sy - float_60_8041dafc[0]), float_0_8041db00[0]);
				iconNumberDispGx(mtx, price, 1, *(GXColor*)&dat_8041d9cc);
			}
		}
	}


	// --- flags & 0xD000: coin / super coin / star piece window ---
	if (wp->flags & 0xD000) {
		Mtx mtx;
		Mtx scale;
		s32 cur;

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9d0, float_neg250_8041db04[0], float_neg75_8041db08[0], float_150_8041db0c[0], float_45_8041db10[0],
		                      float_20_8041dad8[0]);
		PSMTXTrans(mtx, float_neg220_8041db14[0], float_neg113_8041db18[0], float_0_8041db00[0]);
		PSMTXScale(scale, float_0p7_8041db1c[0], float_0p7_8041db1c[0], float_0p7_8041db1c[0]);
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
		PSMTXTrans(mtx, float_neg192_8041db20[0], float_neg113_8041db18[0], float_0_8041db00[0]);
		iconDispGx2(mtx, 0x10, 0x1D6);
		PSMTXTrans(mtx, float_neg126_8041db24[0], float_neg113_8041db18[0], float_0_8041db00[0]);
		iconNumberDispGx(mtx, wp->dispCoin, 0, *(GXColor*)&dat_8041d9d4);
		{
			s32 delta = cur - wp->dispCoin;

			if (delta != 0) {
				delta /= 10;
				if (delta == 0) {
					delta = (cur > wp->dispCoin) ? 1 : -1;
				}
				if (wp->coinTick & 1) {
					wp->dispCoin += delta;
					statusWinForceUpdateCoin();
				}
				wp->coinTick = wp->coinTick + 1;
				wp->coinTick = wp->coinTick % 1200;
				if (unk_804189fc == 0) {
					psndSFXOn("SFX_COUNT_COIN1");
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

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9d8, float_neg250_8041db04[0], float_neg130_8041dae4[0], float_150_8041db0c[0], float_45_8041db10[0],
		                      float_20_8041dad8[0]);
		PSMTXTrans(mtx, float_neg210_8041db28[0], float_neg166_8041db2c[0], float_0_8041db00[0]);
		PSMTXScale(scale, float_1p1_8041db30[0], float_1p1_8041db30[0], float_1p1_8041db30[0]);
		PSMTXConcat(mtx, scale, mtx);
		iconDispGx2(mtx, 0x10, 0x1F1);
		PSMTXTrans(mtx, float_neg126_8041db24[0], float_neg168_8041db34[0], float_0_8041db00[0]);
		iconNumberDispGx(mtx, wp->dispNum, 0, *(GXColor*)&dat_8041d9dc);
		if ((gp->retraceCount & 3) == 0) {
			s32 delta = pouch->mShineSpriteCount - wp->dispNum;

			if (delta != 0) {
				delta /= 10;
				if (delta == 0) {
					delta = (pouch->mShineSpriteCount > wp->dispNum) ? 1 : -1;
				}
				wp->dispNum += delta;
			}
		}
	}

	// --- flags & 0x100: lottery number ---
	if (wp->flags & 0x100) {
		if (lotteryGetPtr()->mFlags & 2) {
			char buf[0x20];
			Mtx mtx;

			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9e0, float_neg250_8041db04[0], float_neg130_8041dae4[0], float_150_8041db0c[0], float_45_8041db10[0],
			                      float_20_8041dad8[0]);
			PSMTXTrans(mtx, float_neg220_8041db14[0], float_neg172_8041db38[0], float_0_8041db00[0]);
			iconDispGx2(mtx, 0x10, 0x140);
			sprintf(buf, str_8041DB3C, *(s16*)&lotteryGetPtr()->field_0x2[0x1E]);
			for (i = 0; i < 4; i++) {
				PSMTXTrans(mtx, float_20_8041dad8[0] * (f32)i + float_neg126_8041db24[0] - float_60_8041dafc[0],
				           float_neg168_8041db34[0], float_0_8041db00[0]);
				iconNumberDispGx(mtx, (s8)buf[i] - '0', 0, *(GXColor*)&dat_8041d9e4);
			}
		}
	}

	switch (wp->mode) {
	case 4: {
		// Stored-item (keep) list.
		s32 dispCnt = 0;
		const char* selDesc = 0;
		s32 cnt = pouchGetKeepItemCnt();
		u32 scX, scY, scW, scH;

		cnt = (8 - cnt > 0) ? (8 - pouchGetKeepItemCnt()) : 0;
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9e8, float_neg24_8041db44[0], float_100_8041db48[0], float_300_8041dad0[0], (f32)(0xdc - cnt * 0x18),
		                      float_20_8041dad8[0]);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9ec, float_51_8041db4c[0], float_114_8041db50[0], float_150_8041db0c[0], float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041d9f0);
		FontDrawString(msgSearch("msg_window_title_1"), float_81_8041db58[0], float_111_8041db5c[0]);
		FontDrawEdgeOff();
		GXGetScissor(&scX, &scY, &scW, &scH);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - cnt * 0x18);
		for (i = 0; i < 32; i++) {
			s32 it = pouchKeepItem(i);
			ItemData* data = &itemDataTable[it];

			if (it != 0) {
				iconDispGxS(0.5f, &(Vec){ 11.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18), 0.0f }, 0x10, ((ItemDataView*)data)->icon_id);
				FontDrawStart();
				FontDrawString(data->id, 36.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
				if ((f32)dispCnt == wp->cursorRow) {
					selDesc = data->description;
				}
				dispCnt++;
			}
		}
		GXSetScissor(scX, scY, scW, scH);
		if (selDesc != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9f4, float_neg240_8041dae0[0], float_neg130_8041dae4[0], float_480_8041dae8[0], float_70_8041daec[0],
			                      float_20_8041dad8[0]);
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
		u32 scX, scY, scW, scH;
		const char* title;
		s32 xofs;
		s32 listOff;
		s32 dispCnt = 0;
		const char* selDesc = 0;

		s32 mode50 = wp->mode;
		s32 cnt;
		s32 limit;
		if (mode50 == 2 || mode50 == 3) {
			cnt = pouchGetHaveItemCnt();
			listOff = ((8 - cnt > 0) ? (8 - pouchGetHaveItemCnt()) : 0) * 0x18;
			limit = 0x14;
			title = msgSearch("msg_window_title_1");
			xofs = 0x1E;
		} else if (mode50 == 12) {
			cnt = pouchGetHaveBadgeCnt();
			listOff = ((8 - cnt > 0) ? (8 - pouchGetHaveBadgeCnt()) : 0) * 0x18;
			limit = 0xC8;
			title = msgSearch("msg_window_title_2");
			xofs = 0x28;
		} else if (mode50 == 14) {
			limit = 0x152;
			cnt = 8 - wp->rowCount;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			title = msgSearch("msg_window_title_3");
			xofs = 0x14;
		} else if (mode50 == 15) {
			limit = wp->rowCount;
			cnt = 8 - wp->rowCount;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			title = msgSearch("msg_window_title_3");
			xofs = 0x14;
		} else if (mode50 == 16) {
			limit = 0x152;
			cnt = 8 - wp->rowCount;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			title = msgSearch("msg_window_title_4");
			xofs = 0x1E;
		} else {
			cnt = 8 - wp->rowCount;
			cnt = (cnt > 0) ? cnt : 0;
			listOff = cnt * 0x18;
			limit = 0x63;
			title = msgSearch("msg_window_title_2");
			xofs = 0x28;
		}

		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9f8, float_neg24_8041db44[0], float_100_8041db48[0], float_300_8041dad0[0], (f32)(0xdc - listOff),
		                      float_20_8041dad8[0]);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041d9fc, float_51_8041db4c[0], float_114_8041db50[0], float_150_8041db0c[0], float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da00);
		FontDrawString(title, (f32)(xofs + 0x33), float_111_8041db5c[0]);
		FontDrawEdgeOff();

		if (wp->mode != 3) {
			Mtx mtx;
			Mtx scale;
			s32 iconId;

			PSMTXTrans(mtx, 244.0f, 82.0f, float_0_8041db00[0]);
			PSMTXScale(scale, 0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGxCol(mtx, 0x10, 0x211, *(GXColor*)&dat_8041da04);
			if (wp->mode == 0xD) {
				iconId = 0x18D;
			} else if (wp->mode == 0x10) {
				iconId = 0x13F;
			} else {
				iconId = 0x18B;
			}
			iconDispGxS(0.6000000238418579f, &(Vec){ 244.0f, 86.0f, 0.0f }, 0x10, iconId);
		}

		GXGetScissor(&scX, &scY, &scW, &scH);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);

		for (i = 0; i < limit; i++) {
			s32 item;
			s32 flag = 0;
			GXColor icolor = *(GXColor*)&dat_8041da08;
			ItemData* data;

			switch (wp->mode) {
			case 2:
			case 3:
				item = pouchHaveItem(i);
				break;
			case 12:
				item = pouchHaveBadge(i);
				break;
			case 11: {
				s32 got = badgeShop_get(bdsw->bargain_table, (s16)(i + BADGE_MIN));
				if (got - badgeShop_ThrowCheck(i + 0xF0) > 0) {
					item = i + 0xF0;
				} else {
					item = 0;
				}
				break;
			}
			case 13:
				if ((s16)badgeShop_get(bdsw->starmaniac_table, (s16)(i + BADGE_MIN)) > 0) {
					item = i + 0xF0;
				} else {
					item = 0;
				}
				break;
			case 14:
				if ((s16)badgeShop_get(bdsw->bottakuru_table, (s16)(i + 1)) > 0) {
					item = i + 1;
				} else {
					item = 0;
				}
				break;
			case 15:
				item = badge_bottakuru100_table[i];
				break;
			case 16:
				if ((s16)badgeShop_get(bdsw->bteresa_table, (s16)(i + 1)) > 0) {
					item = i + 1;
				} else {
					item = 0;
				}
				break;
			default:
				break;
			}

			data = &itemDataTable[item];
			if (item == 0) {
				continue;
			}
			{
				s32 price;
				char buf[0x20];

				if (wp->mode == 0xD) {
					if (data->star_piece_price > pouchGetStarPiece()) {
						flag = 0x20;
						icolor = *(GXColor*)&dat_8041da0c;
					}
				}
				iconDispGxS(0.5f, &(Vec){ 11.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18), 0.0f }, (u16)(flag | 0x10), ((ItemDataView*)data)->icon_id);
				FontDrawStart();
				FontDrawScale(0.800000011920929f);
				{
					FontDrawColorV(icolor);
				}
				FontDrawString(data->id, 36.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18) - 2.0f);
				if (wp->mode != 3) {
					f32 scale;
					s32 x;

					switch (wp->mode) {
					case 0xE:
						price = ((ItemDataView*)data)->buy_price * 12 / 10;
						sprintf(buf, "%2d", price);
						scale = 1.0f;
						x = 0;
						break;
					case 0xF:
						price = ((ItemDataView*)data)->buy_price * (evtGetValue(NULL, 0xF5DE06A9) + 11) / 10;
						sprintf(buf, "%3d", price);
						scale = 1.0f;
						x = -0xE;
						break;
					case 0x10:
						price = (((ItemDataView*)data)->buy_price * 2 + 2) / 3;
						sprintf(buf, "%4d", price);
						scale = 0.800000011920929f;
						x = -0x14;
						break;
					case 0xB:
						sprintf(buf, "%2d", ((ItemDataView*)data)->buy_price);
						scale = 1.0f;
						x = 0;
						break;
					case 0xD:
						sprintf(buf, "%2d", data->star_piece_price);
						scale = 1.0f;
						x = 0;
						break;
					default:
						sprintf(buf, "%2d", data->sell_price);
						x = 0;
						scale = 1.0f;
						break;
					}
					FontDrawStart();
					FontDrawScale(scale);
					{
						FontDrawColorV(icolor);
					}
					FontDrawString(buf, (f32)(x + 0xE2), wp->listY + (f32)(0x50 - dispCnt * 0x18));
				}
				if ((f32)dispCnt == wp->cursorRow) {
					selDesc = data->description;
				}
			}
			dispCnt++;
		}
		GXSetScissor(scX, scY, scW, scH);
		if (selDesc != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da10, float_neg240_8041dae0[0], float_neg130_8041dae4[0], float_480_8041dae8[0], float_70_8041daec[0],
			                      float_20_8041dad8[0]);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(selDesc));
		}
		break;
	}

	case 17:
	case 18: {
		const char* title;
		s32 m8 = 8 - wp->rowCount;
		s32 mode50 = wp->mode;
		u32 scX, scY, scW, scH;
		InformantInfo* info;
		const u8* prefix;
		InformantWork* jdt = &_jdt;
		s32 listOff;

		listOff = ((m8 > 0) ? m8 : 0) * 0x18;
		if (mode50 == 0x11) {
			title = msgSearch("msg_window_title_5");
			prefix = jdt->unk14.unkD;
		} else {
			title = msgSearch("msg_window_title_6");
			prefix = jdt->unk14.unk1A;
		}
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da14, float_neg24_8041db44[0], float_100_8041db48[0], float_300_8041dad0[0], (f32)(0xdc - listOff),
		                      float_20_8041dad8[0]);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da18, float_51_8041db4c[0], float_114_8041db50[0], float_150_8041db0c[0], float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da1c);
		FontDrawString(title, 71.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		if (wp->mode == 0x11) {
			Mtx mtx;
			Mtx scale;

			PSMTXTrans(mtx, 244.0f, 82.0f, float_0_8041db00[0]);
			PSMTXScale(scale, 0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f);
			PSMTXConcat(mtx, scale, mtx);
			iconDispGxCol(mtx, 0x10, 0x211, *(GXColor*)&dat_8041da20);
			iconDispGxS(0.6000000238418579f, &(Vec){ 244.0f, 86.0f, 0.0f }, 0x10, 0x18B);
		}
		GXGetScissor(&scX, &scY, &scW, &scH);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);

		info = jdt->info;
		for (i = 0; i < wp->rowCount; i++, info++) {
			char buf[0x20];
			char saved;
			GXColor tcolor;

			if (johoya_get((u8*)prefix, info->storyId) != 0) {
				sprintf(buf, "");
				tcolor = *(GXColor*)&dat_8041da24;
			} else {
				sprintf(buf, "%3d", info->unkA);
				tcolor = *(GXColor*)&dat_8041da28;
			}
			FontDrawStart();
			saved = ((char*)info->initEndPos)[info->tagEndPos];
			((char*)info->initEndPos)[info->tagEndPos] = 0;
			FontDrawColorV(tcolor);
			FontDrawScale(0.800000011920929f);
			FontDrawString((char*)info->initEndPos, 1.0f, wp->listY + (f32)(0x50 - i * 0x18));
			((char*)info->initEndPos)[info->tagEndPos] = saved;
			if (wp->mode == 0x11) {
				FontDrawStart();
				FontDrawColorV(tcolor);
				FontDrawString(buf, 221.0f, wp->listY + (f32)(0x50 - i * 0x18));
			}
		}
		GXSetScissor(scX, scY, scW, scH);
		break;
	}

	case 5:
	case 6:
	case 7: {
		// Party member stats window.
		u32 scX, scY, scW, scH;
		s32 m8;
		s32 listOff;
		char help[0x40] = "";
		s32 dispCnt = 0;

		m8 = 8 - wp->rowCount;
		listOff = ((m8 > 0) ? m8 : 0) * 0x18;
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da2c, float_neg24_8041db44[0], float_100_8041db48[0], float_300_8041dad0[0], (f32)(0xdc - listOff),
		                      float_20_8041dad8[0]);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da30, 16.0f, float_114_8041db50[0], float_120_8041dacc[0], float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da34);
		FontDrawString(msgSearch("msg_window_title_7"), 41.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da38, 167.0f, float_114_8041db50[0], 85.0f, float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da3c);
		FontDrawString(msgSearch("msg_window_title_8"), 177.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		GXGetScissor(&scX, &scY, &scW, &scH);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);

		for (i = 0; party_id_table[i] != PARTY_MEMBER_NONE; i++) {
			char buf[0x20];
			const char* name;
			s32 flag;
			GXColor nameColor;

			if (wp->mode == 5 || wp->mode == 6) {
				if (!partyChkJoin(party_id_table[i])) {
					continue;
				}
				if (evtGetValue(NULL, 0xF8406022) == 0) {
					if (pouch->partyData[party_id_table[i]].techLevel < 1) {
						nameColor = *(GXColor*)&dat_8041da40;
						lvl = 0;
						flag = 0;
					} else {
						nameColor = *(GXColor*)&dat_8041da44;
						lvl = 1;
						flag = 0x20;
					}
				} else {
					if (pouch->partyData[party_id_table[i]].techLevel < 2) {
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
				if (!partyChkJoin(party_id_table[i]) || wp->excludeMember == party_id_table[i]) {
					continue;
				}
				if (pouch->partyData[party_id_table[i]].techLevel > 0) {
					nameColor = *(GXColor*)&dat_8041da50;
					lvl = 0;
					flag = 0;
				} else {
					nameColor = *(GXColor*)&dat_8041da54;
					lvl = 1;
					flag = 0x20;
				}
			}

			iconDispGxS(0.5f, &(Vec){ 11.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18), 0.0f }, flag | 0x10, unitwin_party_icon(party_icon_table[i]));
			FontDrawStart();
			FontDrawColorV(nameColor);
			name = unitwin_party_name(i);
			FontDrawString(name, 36.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			sprintf(buf, "%d", pouch->partyData[party_id_table[i]].techLevel);
			FontDrawStart();
			FontDrawString(buf, 236.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			if ((f32)dispCnt == wp->cursorRow) {
				char help2[0x10];
				char lank[0x10];

				if (lvl == 0) {
					sprintf(help2, "help");
				} else {
					sprintf(help2, "helperr");
				}
				if (wp->mode == 5 || wp->mode == 6) {
					sprintf(lank, "lankup");
				} else {
					sprintf(lank, "lankdown");
				}
				sprintf(help, "%s_%s_lank%d%s", help2, lank, pouch->partyData[party_id_table[i]].techLevel + 1, party_labelname_table[i]);
			}
			dispCnt++;
		}
		GXSetScissor(scX, scY, scW, scH);
		if ((s8)help[0] != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da58, float_neg240_8041dae0[0], float_neg130_8041dae4[0], float_480_8041dae8[0], float_70_8041daec[0],
			                      float_20_8041dad8[0]);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(help));
		}
		break;
	}

	case 8: {
		s32 dispCnt = 0;
		char help2[0x10];
		s32 listOff;
		char help[0x40] = "";
		char num[0x20];
		u32 scX, scY, scW, scH;
		s32 m8;
		m8 = 8 - wp->rowCount;
		listOff = ((m8 > 0) ? m8 : 0) * 0x18;
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da5c, float_neg24_8041db44[0], float_100_8041db48[0], float_300_8041dad0[0], (f32)(0xdc - listOff),
		                      float_20_8041dad8[0]);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da60, 16.0f, float_114_8041db50[0], float_120_8041dacc[0], float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da64);
		FontDrawString(msgSearch("msg_window_title_9"), 21.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da68, 167.0f, float_114_8041db50[0], 85.0f, float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da6c);
		FontDrawString(msgSearch("msg_window_title_10"), 177.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		GXGetScissor(&scX, &scY, &scW, &scH);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);
		for (i = 0; i < wp->rowCount; i++) {
			s32 j;
			s32 cnt;
			s32 numval;
			s32 flag;
			s32 arrow;
			GXColor rowColor;
			cnt = 0;
			for (j = 0; j < wp->rowCount; j++) {
				if ((f32)j != wp->cursorRow && mario_status_point_table[j][0] - mario_status_henka_table[j] <= 0) {
					cnt = 1;
				}
			}
			if (cnt == 0) {
				rowColor = *(GXColor*)&dat_8041da70;
				changed = 0;
				flag = 0;
			} else {
				rowColor = *(GXColor*)&dat_8041da74;
				changed = 1;
				flag = 0x20;
			}
			iconDispGxS(0.5f, &(Vec){ 11.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18), 0.0f }, flag | 0x10, mario_status_icon_table[i]);
			FontDrawStart();
			FontDrawColorV(rowColor);
			sprintf(num, "%s%3d", mario_status_name_table[i], mario_status_point_table[i][0]);
			FontDrawString(num, 36.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			blink = ((gp->retraceCount & 0xF) > 7) ? 0 : 2;
			if ((f32)dispCnt == wp->cursorRow) {
				if (changed == 0) {
					sprintf(help2, "help");
				} else {
					sprintf(help2, "helperr");
				}
				sprintf(help, "%s_levelup%s", help2, mario_status_labelname_table[i]);
				numval = mario_status_point_table[i][0] + mario_status_henka_table[i] * 2;
				if (numval > 0) {
					arrow = 0x1B5;
				} else {
					arrow = 0x162;
					blink = 0;
				}
				iconDispGxS(0.5f, &(Vec){ 166.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18) + (f32)blink, 0.0f }, flag | 0x10, arrow);
				iconDispGxS(0.5f, &(Vec){ 186.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18) + (f32)blink, 0.0f }, flag | 0x10, arrow);
			} else {
				numval = mario_status_point_table[i][0] - mario_status_henka_table[i];
				if (numval > 0) {
					arrow = 0x1B6;
				} else {
					arrow = 0x162;
					blink = 0;
				}
				iconDispGxS(0.5f, &(Vec){ 176.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18) - (f32)blink, 0.0f }, flag | 0x10, arrow);
			}
			if (numval > 0) {
				sprintf(num, "%3d", numval);
			} else {
				sprintf(num, "  \x81H", numval);
			}
			FontDrawStart();
			FontDrawString(num, 206.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			dispCnt++;
		}
		GXSetScissor(scX, scY, scW, scH);
		if ((s8)help[0] != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da78, float_neg240_8041dae0[0], float_neg130_8041dae4[0], float_480_8041dae8[0], float_70_8041daec[0],
			                      float_20_8041dad8[0]);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(help));
		}
		break;
	}
	case 9: {
		s32 dispCnt = 0;
		char help2[0x10];
		s32 listOff;
		char help[0x40] = "";
		char num[0x20];
		u32 scX, scY, scW, scH;
		s32 m8;
		m8 = 8 - wp->rowCount;
		listOff = ((m8 > 0) ? m8 : 0) * 0x18;
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da7c, float_neg24_8041db44[0], float_100_8041db48[0], float_300_8041dad0[0], (f32)(0xdc - listOff),
		                      float_20_8041dad8[0]);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da80, 16.0f, float_114_8041db50[0], float_120_8041dacc[0], float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da84);
		FontDrawString(msgSearch("msg_window_title_9"), 21.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da88, 167.0f, float_114_8041db50[0], 85.0f, float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041da8c);
		FontDrawString(msgSearch("msg_window_title_10"), 177.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		GXGetScissor(&scX, &scY, &scW, &scH);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);
		for (i = 0; i < wp->rowCount; i++) {
			s32 j;
			s32 cnt;
			s32 numval;
			s32 flag;
			s32 arrow;
			GXColor rowColor;
			cnt = 0;
			for (j = 0; j < wp->rowCount; j++) {
				if ((f32)j != wp->cursorRow && mario_status_point_table[j][0] - mario_status_henka_table[j] <= 0) {
					cnt += 1;
				}
			}
			if (cnt < 2) {
				rowColor = *(GXColor*)&dat_8041da90;
				changed = 0;
				flag = 0;
			} else {
				rowColor = *(GXColor*)&dat_8041da94;
				changed = 1;
				flag = 0x20;
			}
			iconDispGxS(0.5f, &(Vec){ 11.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18), 0.0f }, flag | 0x10, mario_status_icon_table[i]);
			FontDrawStart();
			FontDrawColorV(rowColor);
			sprintf(num, "%s%3d", mario_status_name_table[i], mario_status_point_table[i][0]);
			FontDrawString(num, 36.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			blink = ((gp->retraceCount & 0xF) > 7) ? 0 : 2;
			if ((f32)dispCnt == wp->cursorRow) {
				if (changed == 0) {
					sprintf(help2, "help");
				} else {
					sprintf(help2, "helperr");
				}
				sprintf(help, "%s_levelup%s", help2, mario_status_labelname_table[i]);
				numval = mario_status_point_table[i][0] + mario_status_henka_table[i];
				if (numval > 0) {
					sprintf(num, "%3d", numval);
				} else {
					sprintf(num, "  \x81H", numval);
				}
				FontDrawStart();
				FontDrawString(num, 206.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			}
			dispCnt++;
		}
		GXSetScissor(scX, scY, scW, scH);
		if ((s8)help[0] != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da98, float_neg240_8041dae0[0], float_neg130_8041dae4[0], float_480_8041dae8[0], float_70_8041daec[0],
			                      float_20_8041dad8[0]);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(help));
		}
		break;
	}
	case 10: {
		s32 dispCnt = 0;
		char help2[0x10];
		s32 listOff;
		char help[0x40] = "";
		char num[0x20];
		u32 scX, scY, scW, scH;
		s32 m8;
		m8 = 8 - wp->rowCount;
		listOff = ((m8 > 0) ? m8 : 0) * 0x18;
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041da9c, float_neg24_8041db44[0], float_100_8041db48[0], float_300_8041dad0[0], (f32)(0xdc - listOff),
		                      float_20_8041dad8[0]);
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041daa0, 16.0f, float_114_8041db50[0], float_120_8041dacc[0], float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041daa4);
		FontDrawString(msgSearch("msg_window_title_9"), 21.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041daa8, 167.0f, float_114_8041db50[0], 85.0f, float_28_8041db54[0],
		                      float_8_8041daf0[0]);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColorV(*(GXColor*)&dat_8041daac);
		FontDrawString(msgSearch("msg_window_title_10"), 177.0f, float_111_8041db5c[0]);
		FontDrawEdgeOff();
		GXGetScissor(&scX, &scY, &scW, &scH);
		GXSetScissor(0x120, 0xA2, 0x11C, 0xbe - listOff);
		for (i = 0; i < 3; i++) {
			s32 blocked;
			s32 numval;
			s32 flag;
			s32 arrow;
			GXColor rowColor;
			if (i == wp->excludeMember) { continue; }
			if (mario_status_point_table[i][0] - mario_status_henka_table[i] > 0) {
				rowColor = *(GXColor*)&dat_8041dab0;
				blocked = 0;
				flag = 0;
			} else {
				rowColor = *(GXColor*)&dat_8041dab4;
				blocked = 1;
				flag = 0x20;
			}
			iconDispGxS(0.5f, &(Vec){ 11.0f, wp->listY + (f32)(0x38 - dispCnt * 0x18), 0.0f }, flag | 0x10, mario_status_icon_table[i]);
			FontDrawStart();
			FontDrawColorV(rowColor);
			sprintf(num, "%s%3d", mario_status_name_table[i], mario_status_point_table[i][0]);
			FontDrawString(num, 36.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			blink = ((gp->retraceCount & 0xF) > 7) ? 0 : 2;
			if ((f32)dispCnt == wp->cursorRow) {
				if (blocked == 0) {
					sprintf(help2, "help");
				} else {
					sprintf(help2, "helperr");
				}
				sprintf(help, "%s_leveldown%s", help2, mario_status_labelname_table[i]);
				numval = mario_status_point_table[i][0] - mario_status_henka_table[i];
				if (numval > 0) {
					sprintf(num, "%3d", numval);
				} else {
					sprintf(num, "  \x81H", numval);
				}
				FontDrawStart();
				FontDrawString(num, 206.0f, wp->listY + (f32)(0x50 - dispCnt * 0x18));
			}
			dispCnt++;
		}
		GXSetScissor(scX, scY, scW, scH);
		if ((s8)help[0] != 0) {
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041dab8, float_neg240_8041dae0[0], float_neg130_8041dae4[0], float_480_8041dae8[0], float_70_8041daec[0],
			                      float_20_8041dad8[0]);
			FontDrawStart();
			FontDrawMessage(-220, -138, msgSearch(help));
		}
		break;
	}

	default:
		break;
	}

	// --- common tail: title/help window selected by wp->mode ---
	if (wp->mode != 0 && wp->mode != 1) {
		char* p;
		char buf[0x88];
		s32 lines;
		char* second = NULL;

		switch (wp->mode) {
		case 11:
		case 14:
		case 15: strcpy(buf, msgSearch("msg_window_select_6")); break;
		case 17: strcpy(buf, msgSearch("msg_window_select_8")); break;
		case 18: strcpy(buf, msgSearch("msg_window_select_7")); break;
		case 13:
		case 16: strcpy(buf, msgSearch("msg_window_select_10")); break;
		case 2:
		case 12: strcpy(buf, msgSearch("msg_window_select_2")); break;
		case 3: strcpy(buf, msgSearch("msg_window_select_3")); break;
		case 4: strcpy(buf, msgSearch("msg_window_select_4")); break;
		case 5: strcpy(buf, msgSearch("msg_window_select_11")); break;
		case 6: strcpy(buf, msgSearch("msg_window_select_11")); break;
		case 7: strcpy(buf, msgSearch("msg_window_select_12")); break;
		case 8:
		case 9: strcpy(buf, msgSearch("msg_window_select_9")); break;
		case 10: strcpy(buf, msgSearch("msg_window_select_9_1")); break;
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
		windowDispGX_Waku_col(0, *(GXColor*)&dat_8041dabc, -280.0f, float_100_8041db48[0], float_240_8041daf8[0],
		                      (f32)((lines - 1) * 0x18 + 0x2D), float_20_8041dad8[0]);
		FontDrawStart();
		{
			s32 w2;

			if (p != NULL) {
				i = FontGetMessageWidth(p);
			} else {
				i = 0;
			}
			if (second != NULL) {
				w2 = FontGetMessageWidth(second);
			} else {
				w2 = 0;
			}
			if (w2 > i) {
				i = w2;
			}
			if (i <= 0xE4) {
				i = (0xE4 - i) / 2;
			} else {
				FontDrawScale(228.0f / (f32)i);
				i = 0;
			}
			FontDrawString(p, (f32)(i - 0x112), 90.0f);
			if (second != NULL) {
				FontDrawString(second, (f32)(i - 0x112), 66.0f);
			}
		}
		iconFlagOff("yubicursor_2", 2);
		iconSetPos("yubicursor_2", wp->cursorX, wp->cursorY, float_0_8041db00[0]);
	} else {
		iconFlagOn("yubicursor_2", 2);
	}
}

// itemDataTable[].icon_id is declared s16 in the header, but the original loads it with
// `lhz` (u16) and its address build only matches through a local u16 view of the field.
USER_FUNC(set_new_goods_list) {
	s32* args = event->args;
	UnitWinWork* wp = (UnitWinWork*)evtGetValue(event, args[0]);
	s32 item = evtGetValue(event, args[1]);
	char name[32];

	wp->items[wp->index] = item;
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

USER_FUNC(unitwin_get_work_ptr) {
	evtSetValue(event, event->args[0], (s32)_wp);
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwin_getpartylank) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	s32 id = evtGetValue(event, args[0]);

	evtSetValue(event, args[1], pouch->partyData[id].techLevel);
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

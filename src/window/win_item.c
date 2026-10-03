#include "window/win_item.h"
#include "window/win_root.h"
#include "data/item_data.h"
#include <dolphin/gx.h>
#include "window/win_main.h"
#include "win/win_main.h"
#include "manager/winmgr.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "motion/mot_slit.h"
#include "motion/mot_roll.h"
#include "driver/swdrv.h"
#include "data/mail_data.h"
#include "system.h"

// These .rodata strings are deliberately NOT defined here: defining them would push the
// compiler's anonymous int->float pool doubles behind them (0x958 instead of 0x8f0), which
// breaks the base+offset operands of itemUseDisp2/winItemDisp. They belong inline in the
// functions that use them (owned by the other agents working on this file).
// The pool doubles themselves need no declaration: the compiler emits its own @NNN@ entries.
// str_802F55F4 ("msg_menu_sort_narabikae") is used as an inline literal in item_disp instead:
// pooling it there puts it at 0x90c, between itemUseDisp's name_mario and winItemMain's three
// strings, so winItemMain's displacements come out right.
extern const char str_802F560C[];
extern const char str_802F5620[];
extern const char str_802F5638[];
#ifdef MUST_MATCH
// The original reaches these through the str_802F4CE8 base register; inline literals pool so that the base folds.
#define STR_MOCHI_ITEM "msg_menu_mochi_item"
#define STR_MOCHI_DAIJI "msg_menu_mochi_daiji"
#define STR_MAIL_TPL "%s/w/jp/mail.tpl"
#else
#define STR_MOCHI_ITEM str_802F560C
#define STR_MOCHI_DAIJI str_802F5620
#define STR_MAIL_TPL str_802F5638
#endif

// Extended fields of WindowWork used by this unit. win_main.h only declares the
// first 0x1210 bytes, so the rest are reached through raw offsets below.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_S16(w, off) (*(s16*)((u8*)(w) + (off)))
#define WW_U16(w, off) (*(u16*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

extern u16 menu_skip_list[18];
extern const char** luigi_book_list[5];
extern WindowPartyDt winPartyDt[];
extern const f32 float_neg310_8041d550;
extern const f32 float_neg170_8041d558;
extern const f32 float_160_8041d4dc;
extern const f32 float_neg144_8041d55c;
extern const f32 float_180_8041d560;
extern const f32 float_neg96_8041d568;
extern const f32 float_neg80_8041d56c;
extern const f32 float_130_8041d570;
extern const f32 float_28_8041d574;
extern const f32 float_60_8041d564;
extern const f32 float_neg16_8041d4b0;
extern const f32 float_neg28_8041d4b4;
extern const f32 float_190_8041d544;
extern const f32 float_0p25_8041d548;
extern const f32 float_6_8041d54c;
extern const f32 float_neg275_8041d554;
extern const f32 float_320_8041d528;
extern const f32 float_neg240_8041d4f4;
extern const f32 float_0_8041d4ac;
extern const f32 float_20_8041d4e8;
extern const f32 float_105_8041d4fc;
extern const f32 float_120_8041d530;
extern const f32 float_150_8041d52c;
extern const f32 float_210_8041d538;
extern const f32 float_215_8041d53c;
extern const f32 float_415_8041d534;
extern const f32 float_4_8041d540;
extern const GXColor dat_8041d468;
extern const GXColor dat_8041d46c;
extern const GXColor dat_8041d470;
extern const GXColor dat_8041d474;
extern const GXColor dat_8041d478;
extern const GXColor dat_8041d47c;
extern const f32 float_100_8041d4e4;
extern const f32 float_10_8041d4ec;
extern const f32 float_240_8041d4f0;
extern const f32 float_125_8041d4f8;
extern const f32 float_250_8041d500;
extern const f32 float_18_8041d504;
extern const f32 float_110_8041d508;
extern const f32 float_113_8041d50c;
extern const f32 float_90_8041d510;
extern const f32 float_15_8041d514;
extern const f32 float_0p7_8041d518;
extern const f32 float_220_8041d51c;
extern const f32 float_92_8041d520;
extern const f32 float_30_8041d524;

extern const f32 float_0p5_8041d4a8;
extern const GXColor dat_8041d4a4;

// .sdata2 colours, floats and format strings used by itemUseDisp (defined elsewhere).
extern const GXColor dat_8041d480;
extern const GXColor dat_8041d484;
extern const GXColor dat_8041d488;
extern const GXColor dat_8041d48c;
extern const GXColor dat_8041d490;
extern const GXColor dat_8041d494;
extern const GXColor dat_8041d498;
extern const GXColor dat_8041d49c;
extern const GXColor dat_8041d4a0;
extern const f32 float_neg16_8041d4b0;
extern const f32 float_neg28_8041d4b4;
extern const f32 float_0p9_8041d4b8;
extern __declspec(section ".sdata2") const char str_8041D4BC[];
extern __declspec(section ".sdata2") const char str_8041D4C4[];
extern __declspec(section ".sdata2") const char str_8041D4C8[];
extern __declspec(section ".sdata2") const char str_8041D4CC[];
extern const f32 float_14_8041d4d4;
extern const f32 float_1_8041d4d8;
extern const f32 float_160_8041d4dc;
extern const f32 float_2_8041d4e0;
extern WindowPartyDt winPartyDt[];

// .rodata prefix of window/win_item: the 60 msg_luigi_book strings and the vec3_802f52xx /
// vec3_802f55xx aggregate constants. Defining them here (rather than extern) is what lets MWCC
// address itemUseDisp/winItemDisp's rodata through one base register plus displacements. The tail
// (str_802F55E8.. and the int->float pool doubles) is intentionally left extern/implicit above.
// The vec3 names/sizes/order and every constant are taken byte-for-byte from the original .rodata.
const char str_802F4CE8[24] = "msg_luigi_book1_txt1\000\000\000";

const char str_802F4D00[24] = "msg_luigi_book1_txt2\000\000\000";

const char str_802F4D18[24] = "msg_luigi_book1_txt3\000\000\000";

const char str_802F4D30[24] = "msg_luigi_book1_txt4\000\000\000";

const char str_802F4D48[24] = "msg_luigi_book1_txt5\000\000\000";

const char str_802F4D60[24] = "msg_luigi_book1_txt6\000\000\000";

const char str_802F4D78[24] = "msg_luigi_book1_txt7\000\000\000";

const char str_802F4D90[24] = "msg_luigi_book1_txt8\000\000\000";

const char str_802F4DA8[24] = "msg_luigi_book1_txt9\000\000\000";

const char str_802F4DC0[24] = "msg_luigi_book1_txt10\000\000";

const char str_802F4DD8[24] = "msg_luigi_book1_txt11\000\000";

const char str_802F4DF0[24] = "msg_luigi_book1_txt12\000\000";

const char str_802F4E08[24] = "msg_luigi_book2_txt1\000\000\000";

const char str_802F4E20[24] = "msg_luigi_book2_txt2\000\000\000";

const char str_802F4E38[24] = "msg_luigi_book2_txt3\000\000\000";

const char str_802F4E50[24] = "msg_luigi_book2_txt4\000\000\000";

const char str_802F4E68[24] = "msg_luigi_book2_txt5\000\000\000";

const char str_802F4E80[24] = "msg_luigi_book2_txt6\000\000\000";

const char str_802F4E98[24] = "msg_luigi_book2_txt7\000\000\000";

const char str_802F4EB0[24] = "msg_luigi_book2_txt8\000\000\000";

const char str_802F4EC8[24] = "msg_luigi_book2_txt9\000\000\000";

const char str_802F4EE0[24] = "msg_luigi_book2_txt10\000\000";

const char str_802F4EF8[24] = "msg_luigi_book2_txt11\000\000";

const char str_802F4F10[24] = "msg_luigi_book2_txt12\000\000";

const char str_802F4F28[24] = "msg_luigi_book3_txt1\000\000\000";

const char str_802F4F40[24] = "msg_luigi_book3_txt2\000\000\000";

const char str_802F4F58[24] = "msg_luigi_book3_txt3\000\000\000";

const char str_802F4F70[24] = "msg_luigi_book3_txt4\000\000\000";

const char str_802F4F88[24] = "msg_luigi_book3_txt5\000\000\000";

const char str_802F4FA0[24] = "msg_luigi_book3_txt6\000\000\000";

const char str_802F4FB8[24] = "msg_luigi_book3_txt7\000\000\000";

const char str_802F4FD0[24] = "msg_luigi_book3_txt8\000\000\000";

const char str_802F4FE8[24] = "msg_luigi_book3_txt9\000\000\000";

const char str_802F5000[24] = "msg_luigi_book3_txt10\000\000";

const char str_802F5018[24] = "msg_luigi_book3_txt11\000\000";

const char str_802F5030[24] = "msg_luigi_book3_txt12\000\000";

const char str_802F5048[24] = "msg_luigi_book4_txt1\000\000\000";

const char str_802F5060[24] = "msg_luigi_book4_txt2\000\000\000";

const char str_802F5078[24] = "msg_luigi_book4_txt3\000\000\000";

const char str_802F5090[24] = "msg_luigi_book4_txt4\000\000\000";

const char str_802F50A8[24] = "msg_luigi_book4_txt5\000\000\000";

const char str_802F50C0[24] = "msg_luigi_book4_txt6\000\000\000";

const char str_802F50D8[24] = "msg_luigi_book4_txt7\000\000\000";

const char str_802F50F0[24] = "msg_luigi_book4_txt8\000\000\000";

const char str_802F5108[24] = "msg_luigi_book4_txt9\000\000\000";

const char str_802F5120[24] = "msg_luigi_book4_txt10\000\000";

const char str_802F5138[24] = "msg_luigi_book4_txt11\000\000";

const char str_802F5150[24] = "msg_luigi_book4_txt12\000\000";

const char str_802F5168[24] = "msg_luigi_book5_txt1\000\000\000";

const char str_802F5180[24] = "msg_luigi_book5_txt2\000\000\000";

const char str_802F5198[24] = "msg_luigi_book5_txt3\000\000\000";

const char str_802F51B0[24] = "msg_luigi_book5_txt4\000\000\000";

const char str_802F51C8[24] = "msg_luigi_book5_txt5\000\000\000";

const char str_802F51E0[24] = "msg_luigi_book5_txt6\000\000\000";

const char str_802F51F8[24] = "msg_luigi_book5_txt7\000\000\000";

const char str_802F5210[24] = "msg_luigi_book5_txt8\000\000\000";

const char str_802F5228[24] = "msg_luigi_book5_txt9\000\000\000";

const char str_802F5240[24] = "msg_luigi_book5_txt10\000\000";

const char str_802F5258[24] = "msg_luigi_book5_txt11\000\000";

const char str_802F5270[24] = "msg_luigi_book5_txt12\000\000";

const Vec vec3_802f5288 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5294 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f52a0 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f52ac = { 1.0, 1.0, 1.0 };

const Vec vec3_802f52b8 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f52c4 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f52d0 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f52dc = { 0.8, 0.8, 0.8 };

const Vec vec3_802f52e8 = { 0.6666667, 0.6666667, 0.6666667 };

const Vec vec3_802f52f4 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5300 = { 0.9, 0.9, 0.9 };

const Vec vec3_802f530c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5318 = { -1.0, -1.0, -1.0 };

const Vec vec3_802f5324 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5330 = { 0.6, 0.6, 0.6 };

const Vec vec3_802f533c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5348 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f5354 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5360 = { 0.6, 0.6, 0.6 };

const Vec vec3_802f536c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5378 = { 0.7, 0.7, 0.7 };

const Vec vec3_802f5384 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5390 = { 0.6, 0.6, 0.6 };

const Vec vec3_802f539c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f53a8 = { 0.65, 0.65, 0.65 };

const Vec vec3_802f53b4 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f53c0 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f53cc = { 0.0, 0.0, 0.0 };

const Vec vec3_802f53d8 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f53e4 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f53f0 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f53fc = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5408 = { 0.9, 0.9, 0.9 };

const Vec vec3_802f5414 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5420 = { 0.9, 0.9, 0.9 };

const Vec vec3_802f542c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5438 = { 0.8, 0.8, 0.8 };

const Vec vec3_802f5444 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5450 = { 0.8, 0.8, 0.8 };

const Vec vec3_802f545c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5468 = { 0.9, 0.9, 0.9 };

const Vec vec3_802f5474 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5480 = { 0.9, 0.9, 0.9 };

const Vec vec3_802f548c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5498 = { 0.8, 0.8, 0.8 };

const Vec vec3_802f54a4 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f54b0 = { 0.8, 0.8, 0.8 };

const Vec vec3_802f54bc = { 0.0, 0.0, 0.0 };

const Vec vec3_802f54c8 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f54d4 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f54e0 = { 0.65, 0.65, 0.65 };

const Vec vec3_802f54ec = { 0.0, 0.0, 0.0 };

const Vec vec3_802f54f8 = { 0.0, 1.0, 1.0 };

const Vec vec3_802f5504 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5510 = { 0.9, 0.9, 0.9 };

const Vec vec3_802f551c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5528 = { 0.9, 0.9, 0.9 };

const Vec vec3_802f5534 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5540 = { 0.8, 0.8, 0.8 };

const Vec vec3_802f554c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5558 = { 0.8, 0.8, 0.8 };

const Vec vec3_802f5564 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5570 = { 0.15, 0.15, 0.15 };

const Vec vec3_802f557c = { 0.0, 0.0, 0.0 };

const Vec vec3_802f5588 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f5594 = { 0.0, 0.0, 0.0 };

const Vec vec3_802f55a0 = { 1.0, 1.0, 1.0 };

const Vec vec3_802f55ac = { 0.0, 134.0, 0.0 };

const Vec vec3_802f55b8 = { 1.0, 1.0, 1.0 };

const char str_802F55C4[20] = "msg_window_select_5";




typedef struct ItemWorkTail {
    u8 pad[0x2E8];
    u16 skipList[0x79];
    s16 count;
} ItemWorkTail;

static WinMgrDesc window_desc[2];

// Item window state machine: the held-item grid, the key-item list, the Luigi book
// (case 100) and the received-mail viewer (cases 200/201/202). Transliterated from the
// original assembly; every call, branch, loop and store is present.
// The original keeps str_802F4CE8 in r30 and reaches str_802F560C/20/38 as r30+0x924/0x938/
// 0x950 (one lis/addi + displacement). Our object pools the string literals used here, in the
// same order (name_mario 0x900, msg_menu_sort_narabikae 0x90c, then the three below), so the
// base register folds correctly. The frame (0x60), the int->float scratch doubles
// (r1+0x30..0x3f), the callee-saved block (stmw r27, r1+0x4c) and the party-pointer list
// (0-based, read as `list[cursor - 1]`) all match, and the function is the same length as the
// original (1180 instructions). The `dirRep & 0xC000` arm's base/value register pair (r5/r4)
// came out swapped until the element was reached through a pointer
// (`s32* e = &list[sel]; idx = *e;`) rather than a repeated subscript, which colours it as the
// original. 100%.
//
// `winItemMailLine` only exists to defeat MWCC's CSE: written inline, the `work + 0x26C` of
// the case-201 call argument is shared with the following `+= 1` / `+= 0xA`, so MWCC
// materialises it into r31 and copies it into r4 (`addi r31, r28, 0x26c` / `mr r4, r31`),
// where the original computes it straight into r4 and uses displacements afterwards. Going
// through a (trivially inlined) function makes the address a separate value and restores the
// original sequence.
static inline u16* winItemMailLine(void* w) { return (u16*)((u8*)w + 0x26C); }

// 100%: every call, branch, constant, store and control-flow edge matches.
s32 winItemMain(WindowWork* work) {
    s32 sel;
    s32 count;
    s32 rows;
    s32 idx;
    s32 idx2;
    s32 item;
    s32 i;
    s32 n;
    s32 old;
    s32 page;
    WindowPartyDt* list[9];
    s32 leader;

    switch (WW_S32(work, 0x20C)) {
    case 0:
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            return -1;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        if (work->dirRep & 0x3000) {
            WW_S32(work, 0x210) = 1 - WW_S32(work, 0x210);
            psndSFXOn((const char*)0x20005);
        } else if (work->buttonTrg & 0x100) {
            if (WW_S32(work, 0x210) == 0) {
                if (pouchGetHaveItemCnt() == 0) {
                    break;
                }
            }
            if (WW_S32(work, 0x210) == 1) {
                if (WW_S16(work, 0x3DA) == 0) {
                    break;
                }
            }
            WW_S32(work, 0x20C) = 10;
            psndSFXOn((const char*)0x20012);
            break;
        } else if (work->buttonTrg & 0x10) {
            winSortEntry(work, WW_S32(work, 0x210) != 0, float_neg310_8041d550, float_150_8041d52c);
            WW_S32(work, 0x124) = WW_S32(work, 0x20C);
            psndSFXOn((const char*)0x20012);
            WW_S32(work, 0x20C) = 1000;
        }
        WW_F32(work, 0x158) = float_neg275_8041d554;
        WW_F32(work, 0x15C) = (f32)(0x7D - WW_S32(work, 0x210) * 0x28);
        winMsgEntry(work, 0, WW_S32(work, 0x210) == 0 ? STR_MOCHI_ITEM : STR_MOCHI_DAIJI, 0);
        break;

    case 10:
        if (WW_S32(work, 0x210) == 0) {
            count = pouchGetHaveItemCnt();
        } else {
            count = WW_S16(work, 0x3DA);
        }
        sel = WW_S32(work, 0x210);
        rows = (count + 9) / 10;

        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            WW_S32(work, 0x20C) = 0;
        }
        if (work->buttonTrg & 0x100) {
            if (WW_S32(work, 0x210) == 0) {
                item = pouchHaveItem(((s32*)((u8*)work + 0x214))[sel]);
                if (itemDataTable[item].usable_locations & 4) {
                    WinMgrEntry* entry;

                    WW_S32(work, 0x2D4) = item;
                    WW_S32(work, 0x2D8) = ((s32*)((u8*)work + 0x214))[sel];
                    WW_S32(work, 0x20C) = 300;
                    WW_S32(work, 0x2DC) = 0;
                    WW_S32(work, 0x2E0) = 0;
                    psndSFXOn((const char*)0x20012);
                    entry = winMgrGetPtr(WW_S32(work, 0x1208));
                    winMgrSetSize(WW_S32(work, 0x1208), entry->field_0x18, entry->field_0x1C, entry->field_0x20,
                                  WW_S32(work, 0x1E0) * 0x1C + 0x44);
                    winMgrOpen(WW_S32(work, 0x1208));
                    winMgrOpen(WW_S32(work, 0x120C));
                }
            } else {
                idx = ((s32*)((u8*)work + 0x214))[sel];
                idx2 = ((s16*)((u8*)work + 0x2E8))[idx];
                item = idx2;
                if ((u32)(item - 0x37) <= 3 || item == 0x3B) {
                    const char** book;

                    WW_S32(work, 0x20C) = 100;
                    WW_F32(work, 0x238) = float_neg170_8041d558;
                    WW_F32(work, 0x240) = float_160_8041d4dc;
                    work->flags |= 0x8000;
                    book = luigi_book_list[item - 0x37];
                    WW_S32(work, 0x2C8) = item - 0x37;
                    WW_S32(work, 0x2D0) = 0;
                    WW_S32(work, 0x2CC) = 0;
                    while (book[WW_S32(work, 0x2CC)] != 0) {
                        WW_S32(work, 0x2CC)++;
                    }
                    WW_S32(work, 0x2CC) = (WW_S32(work, 0x2CC) + 1) / 2;
                    psndSFXOn((const char*)0x20012);
                } else if (item == 0x36 && pouchReceiveMailCount() != 0) {
                    WW_S32(work, 0x20C) = 200;
                    WW_F32(work, 0x248) = float_neg144_8041d55c;
                    WW_F32(work, 0x250) = float_180_8041d560;
                    psndSFXOn((const char*)0x20012);
                } else if (item == 0x4E) {
                    WW_S32(work, 0x2D4) = item;
                    psndSFXOn((const char*)0x20012);
                    return -2;
                }
            }
            goto update10;
        }
        if (work->buttonRep & 0x40) {
            ((s32*)((u8*)work + 0x21C))[sel] -= 1;
            if (((s32*)((u8*)work + 0x21C))[sel] < 0) {
                ((s32*)((u8*)work + 0x21C))[sel] = 0;
            }
            ((s32*)((u8*)work + 0x214))[sel] = ((s32*)((u8*)work + 0x21C))[sel] * 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->buttonRep & 0x20) {
            ((s32*)((u8*)work + 0x21C))[sel] += 1;
            if (((s32*)((u8*)work + 0x21C))[sel] >= rows) {
                ((s32*)((u8*)work + 0x21C))[sel] = rows - 1;
            }
            ((s32*)((u8*)work + 0x214))[sel] = ((s32*)((u8*)work + 0x21C))[sel] * 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->dirRep & 0xC000) {
            // A pointer to the element (rather than the array subscript repeated) is what makes
            // MWCC colour the base r5 / value r4 as the original does.
            s32* e = &((s32*)((u8*)work + 0x214))[sel];
            idx = *e;
            if (idx % 2 != 0) {
                s32 v = idx - 1;
                if (v >= 0 && v < count) {
                    *e = idx - 1;
                }
            } else {
                s32 v = idx + 1;
                if (v >= 0 && v < count) {
                    *e = idx + 1;
                }
            }
            psndSFXOn((const char*)0x20035);
        } else if (work->dirRep & 0x1000) {
            idx = ((s32*)((u8*)work + 0x214))[sel];
            {
                s32 v = idx - 2;
                if (v >= 0 && v < count) {
                    ((s32*)((u8*)work + 0x214))[sel] = idx - 2;
                }
            }
            ((s32*)((u8*)work + 0x21C))[sel] = ((s32*)((u8*)work + 0x214))[sel] / 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->dirRep & 0x2000) {
            idx = ((s32*)((u8*)work + 0x214))[sel];
            {
                s32 v = idx + 2;
                if (v >= 0 && v < count) {
                    ((s32*)((u8*)work + 0x214))[sel] = idx + 2;
                } else if (idx + (idx & 1) < count) {
                    ((s32*)((u8*)work + 0x214))[sel] = count - 1;
                }
            }
            ((s32*)((u8*)work + 0x21C))[sel] = ((s32*)((u8*)work + 0x214))[sel] / 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->buttonTrg & 0x10) {
            winSortEntry(work, WW_S32(work, 0x210) != 0, float_neg310_8041d550, float_150_8041d52c);
            WW_S32(work, 0x124) = WW_S32(work, 0x20C);
            psndSFXOn((const char*)0x20012);
            WW_S32(work, 0x20C) = 1000;
        } else if (work->buttonTrg & 0x1000) {
            return -2;
        }
    update10: {
        WW_F32(work, 0x158) = (f32)((((s32*)((u8*)work + 0x214))[sel] % 10 % 2) * 0xCC - 0x9B);
        WW_F32(work, 0x15C) = (f32)(0x6E - (((s32*)((u8*)work + 0x214))[sel] % 10 / 2) * 0x26);
        item = (WW_S32(work, 0x210) == 0) ? pouchHaveItem(((s32*)((u8*)work + 0x214))[sel])
                                          : (s32)((s16*)((u8*)work + 0x2E8))[((s32*)((u8*)work + 0x214))[sel]];
        winMsgEntry(work, item, itemDataTable[item].description, 0);
    }
        break;

    case 100:
        if (work->buttonTrg & 0x100) {
            psndSFXOn((const char*)0x20012);
            WW_S32(work, 0x2D0) += 1;
            if (WW_S32(work, 0x2D0) >= WW_S32(work, 0x2CC)) {
                WW_S32(work, 0x2D0) = WW_S32(work, 0x2CC) - 1;
                WW_S32(work, 0x20C) = 10;
                WW_F32(work, 0x238) = float_320_8041d528;
                WW_F32(work, 0x240) = float_neg240_8041d4f4;
                work->flags &= ~0x8000;
                WW_F32(work, 0x150) = WW_F32(work, 0x158);
                WW_F32(work, 0x154) = WW_F32(work, 0x15C);
            }
        } else if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            WW_S32(work, 0x2D0) -= 1;
            if (WW_S32(work, 0x2D0) < 0) {
                WW_S32(work, 0x2D0) = 0;
                WW_S32(work, 0x20C) = 10;
                WW_F32(work, 0x238) = float_320_8041d528;
                WW_F32(work, 0x240) = float_neg240_8041d4f4;
                work->flags &= ~0x8000;
                WW_F32(work, 0x150) = WW_F32(work, 0x158);
                WW_F32(work, 0x154) = WW_F32(work, 0x15C);
            }
        } else if (work->buttonTrg & 0x1000) {
            WW_F32(work, 0x238) = float_320_8041d528;
            WW_F32(work, 0x240) = float_neg240_8041d4f4;
            work->flags &= ~0x8000;
            WW_F32(work, 0x150) = WW_F32(work, 0x158);
            WW_F32(work, 0x154) = WW_F32(work, 0x15C);
            return -2;
        }
        WW_F32(work, 0x158) = float_neg170_8041d558;
        WW_F32(work, 0x15C) = float_150_8041d52c;
        break;

    case 200:
        if (work->buttonTrg & 0x100) {
            u8* mail;
            u8* b;

            b = (u8*)work;
            b += WW_S32(work, 0x258) * 2;
            if ((s32)pouchCheckMail(b[0x26F]) == 1) {
                b = (u8*)work;
                b += WW_S32(work, 0x258) * 2;
                pouchOpenMail(b[0x26F]);
            }
            swSet(0x26);
            psndSFXOn((const char*)0x20036);
            WW_S32(work, 0x20C) = 201;
            b = (u8*)work;
            b += WW_S32(work, 0x258) * 2;
            mail = (u8*)mailGetPtr(b[0x26F]);
            FontGetMessageWidthLine(msgSearch(*(const char**)(mail + 0xC)), winItemMailLine(work));
            WW_U16(work, 0x26C) += 1;
            WW_S32(work, 0x268) = 0;
            if (*(u16*)(mail + 0) & 0x1000) {
                WW_U16(work, 0x26C) += 0xA;
            }
        } else if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            WW_S32(work, 0x20C) = 10;
            WW_F32(work, 0x248) = float_320_8041d528;
            WW_F32(work, 0x250) = float_neg240_8041d4f4;
        } else if (work->dirRep & 0x1000) {
            old = WW_S32(work, 0x258);
            WW_S32(work, 0x258) = old - 1;
            if (WW_S32(work, 0x258) < 0) {
                WW_S32(work, 0x258) = 0;
            }
            WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
            WW_S32(work, 0x268) = 0;
            if (old != WW_S32(work, 0x258)) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->dirRep & 0x2000) {
            old = WW_S32(work, 0x258);
            WW_S32(work, 0x258) = old + 1;
            if (WW_S32(work, 0x258) >= WW_S32(work, 0x254)) {
                WW_S32(work, 0x258) = WW_S32(work, 0x254) - 1;
            }
            WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
            WW_S32(work, 0x268) = 0;
            if (old != WW_S32(work, 0x258)) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->buttonRep & 0x40) {
            old = WW_S32(work, 0x25C);
            WW_S32(work, 0x25C) = old - 1;
            if (WW_S32(work, 0x25C) < 0) {
                WW_S32(work, 0x25C) = 0;
            }
            WW_S32(work, 0x258) = WW_S32(work, 0x25C) * 5;
            if (old != WW_S32(work, 0x25C)) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->buttonRep & 0x20) {
            page = WW_S32(work, 0x254);
            old = WW_S32(work, 0x25C);
            WW_S32(work, 0x25C) = old + 1;
            page = (page + 4) / 5;
            if (WW_S32(work, 0x25C) >= page) {
                WW_S32(work, 0x25C) = page - 1;
            }
            WW_S32(work, 0x258) = WW_S32(work, 0x25C) * 5;
            if (old != WW_S32(work, 0x25C)) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->buttonTrg & 0x10) {
            winSortEntry(work, 7, WW_F32(work, 0x248), WW_F32(work, 0x250) - float_60_8041d564);
            WW_S32(work, 0x124) = WW_S32(work, 0x20C);
            psndSFXOn((const char*)0x20012);
            WW_S32(work, 0x20C) = 1000;
        } else if (work->buttonTrg & 0x1000) {
            WW_F32(work, 0x248) = float_320_8041d528;
            WW_F32(work, 0x250) = float_neg240_8041d4f4;
            return -2;
        }
        WW_F32(work, 0x158) = float_neg96_8041d568;
        WW_F32(work, 0x15C) = float_120_8041d530 - (f32)((WW_S32(work, 0x258) % 5) * 0x28);
        break;

    case 201: {
        u8* b = (u8*)work;
        u8* mail;

        b += WW_S32(work, 0x258) * 2;
        mail = (u8*)mailGetPtr(b[0x26F]);

        if (*(u16*)(mail + 0) & 0x1000) {
            WW_S32(work, 0x2C) = fileAsyncf(4, NULL, STR_MAIL_TPL, getMarioStDvdRoot());
            if ((u32)WW_S32(work, 0x2C) != 0) {
                WW_S32(work, 0x2C) = (s32)fileAllocf(4, STR_MAIL_TPL, getMarioStDvdRoot());
                WW_S32(work, 0x20C) += 1;
            }
        } else {
            WW_S32(work, 0x2C) = 0;
            WW_S32(work, 0x20C) += 1;
        }
        break;
    }

    case 202:
        if ((u32)WW_U16(work, 0x26C) > 8) {
            if (keyGetDirRep(0) & 0x100) {
                if (WW_S32(work, 0x268) > 0) {
                    WW_S32(work, 0x268) -= 1;
                }
            } else if (keyGetDirRep(0) & 0x200) {
                if (WW_S32(work, 0x268) + 6 < (s32)WW_U16(work, 0x26C)) {
                    WW_S32(work, 0x268) += 1;
                }
            }
        }
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20036);
            if ((u32)WW_S32(work, 0x2C) != 0) {
                fileFree((FileEntry*)WW_S32(work, 0x2C));
            }
            WW_S32(work, 0x20C) = 200;
        } else if (work->buttonTrg & 0x1000) {
            if ((u32)WW_S32(work, 0x2C) != 0) {
                fileFree((FileEntry*)WW_S32(work, 0x2C));
            }
            WW_F32(work, 0x248) = float_320_8041d528;
            WW_F32(work, 0x250) = float_neg240_8041d4f4;
            return -2;
        }
        WW_F32(work, 0x158) = float_neg96_8041d568;
        WW_F32(work, 0x15C) = float_120_8041d530;
        break;

    case 300:
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            WW_S32(work, 0x20C) = 10;
            WW_S32(work, 0x2D4) = 0;
            winMgrClose(WW_S32(work, 0x1208));
            winMgrClose(WW_S32(work, 0x120C));
        } else if (work->buttonTrg & 0x100) {
            MarioWork* mario = marioGetPtr();
            s32 ok = 0;

            psndSFXOn((const char*)0x20012);
            if (mario->currMotionId == 0x19 || mario->currMotionId == 0x18) {
                ok = 1;
            }
            if (unk_8009fcbc() || U_marioSlitContinueChk()) {
                ok = 1;
            }
            if (WW_S32(work, 0x2DC) > 0) {
                if (marioBgmodeChk()) {
                    ok = 1;
                }
            }
            if (WW_S32(work, 0x2DC) > 1) {
                if (!marioBgmodeChk()) {
                    ok = 1;
                }
            }
            if (!ok) {
                winMgrClose(WW_S32(work, 0x1208));
                winMgrClose(WW_S32(work, 0x120C));
                return -2;
            }
            if (marioBgmodeChk() == 0) {
                leader = marioGetParty();
            } else {
                leader = marioGetPtr()->prevPartyMemberId[0];
            }
            n = 0;
            for (i = 0; i < 7; i++) {
                if (leader == winPartyDt[i].field_0x0) {
                    list[n++] = &winPartyDt[i];
                }
            }
            for (i = 0; i < 7; i++) {
                if (leader != winPartyDt[i].field_0x0) {
                    list[n++] = &winPartyDt[i];
                }
            }
            if (WW_S32(work, 0x2DC) < 1) {
                pouchSetHP(pouchGetHP() + itemDataTable[WW_S32(work, 0x2D4)].hp_restored);
                pouchSetFP((*(s32(*)(void))pouchGetFP)() + itemDataTable[WW_S32(work, 0x2D4)].fp_restored);
                pouchSetAP((*(s32(*)(void))pouchGetAP)() + itemDataTable[WW_S32(work, 0x2D4)].sp_restored);
            } else {
                PartyId pid = list[WW_S32(work, 0x2DC) - 1]->field_0x0;

                (*(void(*)(PartyId, s32))pouchSetPartyHP)(pid, (*(s32(*)(PartyId))pouchGetPartyHP)(pid) + itemDataTable[WW_S32(work, 0x2D4)].hp_restored);
                pouchSetFP((*(s32(*)(void))pouchGetFP)() + itemDataTable[WW_S32(work, 0x2D4)].fp_restored);
                pouchSetAP((*(s32(*)(void))pouchGetAP)() + itemDataTable[WW_S32(work, 0x2D4)].sp_restored);
            }
            WW_S32(work, 0x2E0) = 0;
            WW_S32(work, 0x20C) = 301;
            break;
        } else if (work->dirRep & 0x1000) {
            old = WW_S32(work, 0x2DC);
            WW_S32(work, 0x2DC) = old - 1;
            if (WW_S32(work, 0x2DC) < 0) {
                WW_S32(work, 0x2DC) = WW_S32(work, 0x1E0);
            }
            if (old != WW_S32(work, 0x2DC)) {
                psndSFXOn((const char*)0x20005);
            }
        } else if (work->dirRep & 0x2000) {
            old = WW_S32(work, 0x2DC);
            WW_S32(work, 0x2DC) = old + 1;
            if (WW_S32(work, 0x2DC) >= WW_S32(work, 0x1E0) + 1) {
                WW_S32(work, 0x2DC) = 0;
            }
            if (old != WW_S32(work, 0x2DC)) {
                psndSFXOn((const char*)0x20005);
            }
        } else if (work->buttonTrg & 0x1000) {
            WW_S32(work, 0x2D4) = 0;
            winMgrClose(WW_S32(work, 0x1208));
            winMgrClose(WW_S32(work, 0x120C));
            return -2;
        }
        WW_F32(work, 0x158) = float_neg80_8041d56c;
        {
            f32 f4;
            s32 t = WW_S32(work, 0x2DC);

            if (t == 0) {
                f4 = float_0_8041d4ac;
            } else if (t == 1) {
                f4 = float_neg16_8041d4b0;
            } else {
                f4 = float_neg28_8041d4b4;
            }
            {
                f32 v = float_130_8041d570 - float_28_8041d574 * (f32)t;

                WW_F32(work, 0x15C) = v + f4;
            }
        }
        break;

    case 301:
        WW_S32(work, 0x2E0) += 1;
        if (WW_S32(work, 0x2E0) > gp->framerate * 2) {
            WW_S32(work, 0x2E0) = 0;
            pouchRemoveItemIndex(WW_S32(work, 0x2D4), WW_S32(work, 0x2D8));
            WW_S32(work, 0x20C) = 10;
            WW_S32(work, 0x2D4) = 0;
            if (pouchGetHaveItemCnt() == 0) {
                WW_S32(work, 0x20C) = 0;
            } else if (WW_S32(work, 0x214) >= pouchGetHaveItemCnt()) {
                WW_S32(work, 0x214) = pouchGetHaveItemCnt() - 1;
            }
            winMgrClose(WW_S32(work, 0x1208));
            winMgrClose(WW_S32(work, 0x120C));
        }
        break;

    case 1000:
        if (!winSortWait(work)) {
            WW_S32(work, 0x20C) = WW_S32(work, 0x124);
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        break;

    default:
        break;
    }

    return 0;
}

// MUST_MATCH: retail keeps the blend difference in a temp so MWCC contracts it as
// `diff * 0.25f + old` rather than `0.25f * diff + old`; using the same temp for the
// 190.0f scale also makes the allocator colour that value f5, as retail does. The natural
// form is under #else.
void winItemMain2(WindowWork* work) {
#ifdef MUST_MATCH
    f32 t;

    t = float_190_8041d544;
    WW_F32(work, 0x22C) = t * (f32)WW_S32(work, 0x21C);
    t = WW_F32(work, 0x22C) - WW_F32(work, 0x224);
    WW_F32(work, 0x224) = t * float_0p25_8041d548 + WW_F32(work, 0x224);

    t = float_190_8041d544;
    WW_F32(work, 0x230) = t * (f32)WW_S32(work, 0x220);
    t = WW_F32(work, 0x230) - WW_F32(work, 0x228);
    WW_F32(work, 0x228) = t * float_0p25_8041d548 + WW_F32(work, 0x228);
#else
    WW_F32(work, 0x22C) = float_190_8041d544 * (f32)WW_S32(work, 0x21C);
    WW_F32(work, 0x224) += (WW_F32(work, 0x22C) - WW_F32(work, 0x224)) * float_0p25_8041d548;

    WW_F32(work, 0x230) = float_190_8041d544 * (f32)WW_S32(work, 0x220);
    WW_F32(work, 0x228) += (WW_F32(work, 0x230) - WW_F32(work, 0x228)) * float_0p25_8041d548;
#endif

    WW_F32(work, 0x234) += (WW_F32(work, 0x238) - WW_F32(work, 0x234)) / float_6_8041d54c;
    WW_F32(work, 0x23C) += (WW_F32(work, 0x240) - WW_F32(work, 0x23C)) / float_6_8041d54c;
    WW_F32(work, 0x244) += (WW_F32(work, 0x248) - WW_F32(work, 0x244)) / float_6_8041d54c;
    WW_F32(work, 0x24C) += (WW_F32(work, 0x250) - WW_F32(work, 0x24C)) / float_6_8041d54c;

    WW_F32(work, 0x264) = (f32)(WW_S32(work, 0x25C) * 200);
#ifdef MUST_MATCH
    t = WW_F32(work, 0x264) - WW_F32(work, 0x260);
    WW_F32(work, 0x260) += t * float_0p25_8041d548;
#else
    WW_F32(work, 0x260) += (WW_F32(work, 0x264) - WW_F32(work, 0x260)) * float_0p25_8041d548;
#endif
}

#ifdef MUST_MATCH
// Match shape: building the modified Vec in an inline that returns it (rather than a local
// `Vec translate` per branch) puts the caller's translate temp in the original's stack slot.
// The plain local form under #else is the natural code and scores 96.78%.
static inline Vec winItemMkVec(Vec base, f32 x, f32 y) {
    base.x = x;
    base.y = y;
    return base;
}
#endif

// The original reaches the six Vecs and the int->float double through one base register
// (r31 = str_802F4CE8) plus displacements; defining win_item's .rodata in this unit is what
// makes MWCC pool them that way.
// State: every instruction now matches except the callee-saved GPR colouring:
// `work` lands in r28 (target r29) and the loop counter `i` in r29 (target r28); the FP
// scratch f0/f1 in the a/b/c block is swapped with it. Returning the modified Vec from an
// inline (winItemMkVec, below) gives the three translate temporaries the original's stack
// slots and lifted 96.78% -> 97.46% (the plain local-Vec form leaves them at 0x54 etc.).
// Exhausted on top of that: declaration-order permutations, every loop shape and increment
// order, splitting work/i/ofs per block, alias pointers, register/volatile qualifiers, a/b/c
// statement reorders, local float constants, and every relevant pragma; a full compile-line
// sweep (all -O levels x inline modes) finds no better flag set. Sibling winItemInit settled
// the same kind of colouring with a pointer/loop-index increment order.
// NONMATCHING (97.46%): only register colours differ - `work` lands in r28 (target r29) and the
// loop counter `i` in r29 (target r28); every instruction, operand and stack slot otherwise matches.
void winItemDisp(CameraId camId, WindowWork* work, s32 idx) {
    GXColor color = dat_8041d468;
    f32 c;
    f32 b;
    f32 a;
    f32 x = WW_F32(work, idx * 0x14 + 0xC4);
    f32 y = WW_F32(work, idx * 0x14 + 0xC8);
    s32 ofs;
    s32 i;

    winBgGX(work, 2, x, y);
    winKirinukiGX(work, 0, x - float_150_8041d52c, float_20_8041d4e8 + (float_120_8041d530 + y), float_415_8041d534,
                  float_210_8041d538);

    a = float_105_8041d4fc + y;
    b = float_4_8041d540 + a;
    c = (x - float_215_8041d53c) - float_4_8041d540;

    for (i = 0, ofs = 0; i < 2; i++, ofs += 0x28) {
        if (i == WW_S32(work, 0x210)) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
#ifdef MUST_MATCH
            winTexSet(0xB3, winItemMkVec(vec3_802f5288, x - float_215_8041d53c, float_20_8041d4e8 + (a - (f32)ofs)), vec3_802f5294, dat_8041d46c);
#else
            {
                Vec translate = vec3_802f5288;

                translate.x = x - float_215_8041d53c;
                translate.y = float_20_8041d4e8 + (a - (f32)ofs);
                winTexSet(0xB3, translate, vec3_802f5294, dat_8041d46c);
            }
#endif
            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
#ifdef MUST_MATCH
            winTexSet_x2(i + 0x18, 0xB3, winItemMkVec(vec3_802f52a0, c, float_20_8041d4e8 + (b - (f32)ofs)), vec3_802f52ac, color);
#else
            {
                Vec translate = vec3_802f52a0;

                translate.x = c;
                translate.y = float_20_8041d4e8 + (b - (f32)ofs);
                winTexSet_x2(i + 0x18, 0xB3, translate, vec3_802f52ac, color);
            }
#endif
        } else {
            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
#ifdef MUST_MATCH
            winTexSet_x2(i + 0x18, 0xB3, winItemMkVec(vec3_802f52b8, x - float_215_8041d53c, float_20_8041d4e8 + (a - (f32)ofs)), vec3_802f52c4, dat_8041d470);
#else
            {
                Vec translate = vec3_802f52b8;

                translate.x = x - float_215_8041d53c;
                translate.y = float_20_8041d4e8 + (a - (f32)ofs);
                winTexSet_x2(i + 0x18, 0xB3, translate, vec3_802f52c4, dat_8041d470);
            }
#endif
        }
    }

    item_disp(work, x, y);
}

// Draws the item window: the held/key-item grid, the selected item's large
// icon and name, the overflow arrows and the sort label. The two int->float pool
// doubles come out as anonymous @NNN@ instead of double_to_int_802f55e0 /
// double_to_int_mask_802f55d8 (defining them makes MWCC emit fresh anonymous copies).
// The .rodata prefix is now defined in this file, so the Vec/GXColor strings below address
// through the base register; score went 68.53% -> 83.60% from that alone.
// 87.03%: the FP register colours now match (f27=base, f28=float_20, f29=double_to_int,
// f25=item y, f26=row base) via the `twenty` local and the f26/f25 declaration order, and
// each winIconSet gets a by-value GXColor copy like the original.
// 92.29%: building the by-value/pointed Vec arguments through the `winItemMkVec` inline
// (returning the modified Vec) instead of a source Vec plus assignment lands several of the
// argument copies in the original's stack slots - the loop's winIconSet translate copy, the
// loop's winFontSetPitch translate, the final winFontSet translate, and the two overflow
// winTexSet translate arguments.
// 92.29% -> 93.16%: writing item_disp's two sort-tag lookups as the inline literal
// "msg_menu_sort_narabikae" (instead of the extern str_802F55F4) pools that string at 0x90c,
// between itemUseDisp's name_mario and winItemMain's three strings, which is what makes
// winItemMain exactly 100%. That pooling rearranges item_disp's own register allocation, so
// the remaining wins came from re-shaping the loop: swapping the `parity`/`half` declaration
// order (the original computes parity first), writing the grey flag as `!colored` rather than
// `colored == 0` (MWCC emits a different cntlzw shape), and putting `i++` before `entry += 2`
// in the for-increment. The frame is now the original's 0x250. Still differs in the integer
// colours (ours keeps `work` in r23, the original r31, and the rest rotate) and the loop
// block's stack-slot order (the winIconSet/winFontSetPitch argument copies and the `scale`
// source land at different offsets. 94.75% -> 95.27%: a second `white` local
// (whiteOverflow, MUST_MATCH) reproduces the original's third spill and gets the frame to
// the original's 0x250; `white` now homes at r1+0x8 with copies at 0x2c (overflow) and
// 0x38 (loop), as the original does). 95.27% -> 95.29%: declaring `entry` before `i` and putting
// `entry += 2` before `i++` in the for-increment matches the original's two `addi` order and removes
// two operand diffs (the remaining 228 are the allocator's colours and stack offsets).
// NONMATCHING (95.29%): integer register colours and the loop's stack-slot order; the target keeps
// two per-branch `scale` temps our compiler coalesces (2 `addi` short).
void item_disp(WindowWork* work, f32 x, f32 y) {
    GXColor white = dat_8041d474;
    GXColor dark = dat_8041d478;
    GXColor gray = dat_8041d47c;
#ifdef MUST_MATCH
    // Match shape: the original spills `white` three times (home, one copy for the two
    // overflow winTexSet colour arguments and one for the loop's icon colour). A second
    // local initialised straight from the palette object reproduces that spill and the
    // original's -0x250 frame; without it the frame is -0x240 and the score is 94.75%.
    GXColor whiteOverflow = dat_8041d474;
#endif
    f32 twenty;
    f32 base;
    f32 f22;
    f32 f26;
    f32 f25;
    u8* entry;
    s32 i;
    s32 count;

    if (WW_S32(work, 0x210) == 0) {
        count = pouchGetHaveItemCnt();
    } else {
        count = WW_S16(work, 0x3DA);
    }

    GXSetScissor(0, 0x6D, 0x260, 0xC0);
    twenty = float_20_8041d4e8;
    base = float_100_8041d4e4 + y;

    for (i = 0, entry = (u8*)work; i < count; entry += 2, i++) {
        s32 item;
        s32 parity = i % 2;
        s32 half = i / 2;
        s32 colored;
        s32 grayFlag;

        if (WW_S32(work, 0x210) == 0) {
            item = pouchHaveItem(i);
        } else {
            item = *(s16*)(entry + 0x2E8);
        }
        f25 = *(f32*)((u8*)work + WW_S32(work, 0x210) * 4 + 0x224);

        colored = (WW_S32(work, 0x210) != 0) || (itemDataTable[item].usable_locations & 4);
        grayFlag = !colored;
        f26 = base - (f32)(half * 0x26);
        f22 = f25 + (twenty + (f26 - float_10_8041d4ec));
        if (f22 > float_240_8041d4f0) {
            continue;
        }
        if (f22 < float_neg240_8041d4f4) {
            break;
        }

        if (grayFlag) {
            winIconGrayInit();
        } else {
            winIconInit();
        }

        {
            GXColor iconColor = white;
            Vec scale;
#ifdef MUST_MATCH
            // The original calls winIconSet with by-value Vecs (one copy per struct);
            // the shared header declares Vec* (see report), so copy them here to match.
            Vec tcopy;
#endif
#ifndef MUST_MATCH
            Vec translate = vec3_802f52d0;
#endif

            if (WW_S32(work, 0x210) == 0) {
                scale = vec3_802f52dc;
            } else {
                scale = vec3_802f52e8;
            }

#ifdef MUST_MATCH
            tcopy = winItemMkVec(vec3_802f52d0, (x - float_125_8041d4f8) + (f32)(parity * 0xCC), f22);
            winIconSet(*(u16*)&itemDataTable[item].icon_id, &tcopy, &scale, &iconColor);
#else
            translate.x = (x - float_125_8041d4f8) + (f32)(parity * 0xCC);
            translate.y = f22;
            winIconSet(*(u16*)&itemDataTable[item].icon_id, &translate, &scale, &iconColor);
#endif
        }
        winFontInit();
        {
            GXColor color = grayFlag ? gray : dark;
            Vec scale = vec3_802f5300;
#ifdef MUST_MATCH
            Vec translate = winItemMkVec(vec3_802f52f4, (x - float_105_8041d4fc) + (f32)(parity * 0xCC), f25 + (float_20_8041d4e8 + f26));
#else
            Vec translate = vec3_802f52f4;
#endif

#ifndef MUST_MATCH
            translate.x = (x - float_105_8041d4fc) + (f32)(parity * 0xCC);
            translate.y = f25 + (float_20_8041d4e8 + f26);
#endif
            winFontSetPitch(&translate, scale, &color, float_20_8041d4e8,
                            msgSearch(itemDataTable[item].name));
        }
    }

    GXSetScissor(0, 0, 0x260, 0x1E0);

    {
        if (WW_S32(work, 0x21C + WW_S32(work, 0x210) * 4) > 0) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            f22 = float_250_8041d500 + x;
            {
#ifdef MUST_MATCH
                Vec scale = vec3_802f5318;

                winTexSet(0x17, winItemMkVec(vec3_802f530c, f22, float_20_8041d4e8 + (float_18_8041d504 + (float_110_8041d508 + y))), scale, whiteOverflow);
#else
                Vec translate = vec3_802f530c;
                Vec scale = vec3_802f5318;

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (float_18_8041d504 + (float_110_8041d508 + y));
                winTexSet(0x17, translate, scale, white);
#endif
            }
            winIconInit();
            {
                Vec translate = vec3_802f5324;
                Vec scale = vec3_802f5330;
#ifdef MUST_MATCH
                Vec tcopy;
                GXColor ccopy;
#endif

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (float_113_8041d50c + y);
#ifdef MUST_MATCH
                tcopy = translate;
                ccopy = white;
                winIconSet(0x7E, &tcopy, &scale, &ccopy);
#else
                winIconSet(0x7E, &translate, &scale, &white);
#endif
            }
        }
        if (count > WW_S32(work, 0x21C + WW_S32(work, 0x210) * 4) * 10 + 10) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            f22 = float_250_8041d500 + x;
            {
#ifdef MUST_MATCH
                Vec scale = vec3_802f5348;

                winTexSet(0x17, winItemMkVec(vec3_802f533c, f22, float_20_8041d4e8 + ((y - float_90_8041d510) - float_15_8041d514)), scale, whiteOverflow);
#else
                Vec translate = vec3_802f533c;
                Vec scale = vec3_802f5348;

                translate.x = f22;
                translate.y = float_20_8041d4e8 + ((y - float_90_8041d510) - float_15_8041d514);
                winTexSet(0x17, translate, scale, white);
#endif
            }
            winIconInit();
            {
                Vec translate = vec3_802f5354;
                Vec scale = vec3_802f5360;
#ifdef MUST_MATCH
                Vec tcopy;
                GXColor ccopy;
#endif

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (y - float_90_8041d510);
#ifdef MUST_MATCH
                tcopy = translate;
                ccopy = white;
                winIconSet(0x80, &tcopy, &scale, &ccopy);
#else
                winIconSet(0x80, &translate, &scale, &white);
#endif
            }
        }
    }

    {
        const char* msg = msgSearch("msg_menu_sort_narabikae");
        f32 w = float_0p7_8041d518 * (f32)FontGetMessageWidth(msg);

        winFontInit();
        f22 = x + (float_220_8041d51c - w);
        {
#ifdef MUST_MATCH
            Vec translate = winItemMkVec(vec3_802f536c, f22, float_20_8041d4e8 + ((y - float_92_8041d520) - float_10_8041d4ec));
#else
            Vec translate = vec3_802f536c;
#endif
            Vec scale = vec3_802f5378;
            GXColor dcopy = dark;

#ifndef MUST_MATCH
            translate.x = f22;
            translate.y = float_20_8041d4e8 + ((y - float_92_8041d520) - float_10_8041d4ec);
#endif
            winFontSet(&translate, &scale, &dcopy, msgSearch("msg_menu_sort_narabikae"));
        }
        winIconInit();
        {
            Vec translate = vec3_802f5384;
            Vec scale = vec3_802f5390;
#ifdef MUST_MATCH
            Vec tcopy;
            GXColor ccopy;
#endif

            translate.x = f22 - float_30_8041d524;
            translate.y = float_20_8041d4e8 + ((y - float_100_8041d4e4) - float_10_8041d4ec);
#ifdef MUST_MATCH
            tcopy = translate;
            ccopy = white;
            winIconSet(0x217, &tcopy, &scale, &ccopy);
#else
            winIconSet(0x217, &translate, &scale, &white);
#endif
        }
    }

    if (WW_F32(work, 0x234) < float_320_8041d528 && WW_F32(work, 0x23C) > float_neg240_8041d4f4) {
        winBookGX(work, 0);
    }
}

void winItemExit(WindowWork* work) {
    winMgrDelete(WW_S32(work, 0x1208));
    winMgrDelete(WW_S32(work, 0x120C));
}

void winItemInit2(WindowWork* work) {
    f32 t;

    WW_F32(work, 0x158) = float_neg275_8041d554;
    WW_F32(work, 0x15C) = (f32)(0x7D - WW_S32(work, 0x210) * 0x28);

    t = float_190_8041d544 * (f32)WW_S32(work, 0x21C);
    WW_F32(work, 0x22C) = t;
    WW_F32(work, 0x224) = t;

    t = float_190_8041d544 * (f32)WW_S32(work, 0x220);
    WW_F32(work, 0x230) = t;
    WW_F32(work, 0x228) = t;

    WW_F32(work, 0x238) = float_320_8041d528;
    WW_F32(work, 0x234) = float_320_8041d528;
    WW_F32(work, 0x240) = float_neg240_8041d4f4;
    WW_F32(work, 0x23C) = float_neg240_8041d4f4;
    WW_F32(work, 0x248) = float_320_8041d528;
    WW_F32(work, 0x244) = float_320_8041d528;
    WW_F32(work, 0x250) = float_neg240_8041d4f4;
    WW_F32(work, 0x24C) = float_neg240_8041d4f4;

    t = (f32)(WW_S32(work, 0x258) * 0xC8);
    WW_F32(work, 0x264) = t;
    WW_F32(work, 0x260) = t;

    winMsgEntry(work, 0, WW_S32(work, 0x210) == 0 ? str_802F560C : str_802F5620, 0);
    WW_S32(work, 0x2D4) = 0;
}

// The key-list scan walks the skip table with both an index and a pointer; the
// original increments the pointer before the index, which is what settles MWCC's
// register choice (pointer r5, index r3).
void winItemInit(WindowWork* work) {
    u8* p;
    s32 i;
    u16 item;
    s32 j;

    WW_S32(work, 0x20C) = 0;
    WW_S32(work, 0x210) = 0;
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = 0;
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = 0;
    WW_F32(work, 0x238) = float_320_8041d528;
    WW_F32(work, 0x234) = float_320_8041d528;
    WW_F32(work, 0x240) = float_neg240_8041d4f4;
    WW_F32(work, 0x23C) = float_neg240_8041d4f4;
    WW_F32(work, 0x248) = float_320_8041d528;
    WW_F32(work, 0x244) = float_320_8041d528;
    WW_F32(work, 0x250) = float_neg240_8041d4f4;
    WW_F32(work, 0x24C) = float_neg240_8041d4f4;

    if ((s32)pouchCheckItem(0x36) == 0) {
        pouchGetItem(0x36);
    }
    WW_S32(work, 0x254) = pouchReceiveMailCount();

    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = 0;
    WW_F32(work, 0x264) = float_0_8041d4ac;
    WW_F32(work, 0x260) = float_0_8041d4ac;

    {
        p = (u8*)work;
        for (i = 0; i < WW_S32(work, 0x254); i++) {
            u8* pouch;
            s32 off;
            p[0x26E] = WW_S32(work, 0x254) - i - 1;
            pouch = (u8*)pouchGetPtr();
            off = 0x519 + (WW_S32(work, 0x254) - i);
            p[0x26F] = pouch[off];
            p += 2;
        }
    }

    WW_S32(work, 0x2D4) = 0;
    WW_S16(work, 0x3DA) = 0;
    for (i = 0; i < 0x79; i++) {
        item = pouchKeyItem(i);

        if (item == 0) {
            continue;
        }
        {
            u16* list;
            j = 0;
            list = menu_skip_list;
            while (j < 0x12) {
                if (item == *list) {
                    break;
                }
                list++;
                j++;
            }
        }
        if ((u32)j >= 0x12) {
            ItemWorkTail* w = (ItemWorkTail*)work;
            w->skipList[w->count] = item;
            w->count++;
        }
    }

    WW_S32(work, 0x1208) = winMgrEntry(&window_desc[0]);
    WW_S32(work, 0x120C) = winMgrEntry(&window_desc[1]);
    winMgrSetPriority(WW_S32(work, 0x1208), -0xC8);
    winMgrSetPriority(WW_S32(work, 0x120C), -0x64);
    fileAsyncf(4, NULL, str_802F5638, getMarioStDvdRoot());
}

void winMakeSkipList(WindowWork* work) {
    s32 i;
    s32 j;

    WW_S16(work, 0x3DA) = 0;
    for (i = 0; i < 0x79; i++) {
        u16 item = pouchKeyItem(i);

        if (item == 0) {
            continue;
        }
        for (j = 0; j < 0x12; j++) {
            if (item == menu_skip_list[j]) {
                break;
            }
        }
        if ((u32)j >= 0x12) {
            ItemWorkTail* w = (ItemWorkTail*)work;
            w->skipList[w->count] = item;
            w->count++;
        }
    }
}

// Draws the item/submenu window: the selected item's icon, name and description, the
// party members' names and HP/FP, the item grid and the window's background textures.
// Complete and logically correct; MWCC saves the original's 9 callee-saved registers and, with the
// per-block `Vec tcopy`, the frame matches the original's 0x490 exactly. The original walks the party
// list with a running pointer (the target increments r27 by 4 in the loop header and reads `-4(r27)`),
// so the else branch keeps `lp[-1]` and the loop increments `lp++`; that shape matched the target's
// list increment and lifted 94.44 -> 94.50. Remaining: `i`/`ofs` land in r28/r25 where the target has
// r25/r28 (the list pointer and `ofs2` are already r27/r26), the low stack area is shifted +0xC from
// the target, and the by-value `winTexSet` argument copies sit in different slots (the target passes
// some source locals in place). Every call, constant and instruction otherwise matches.
// NONMATCHING (94.50%): stack-slot offsets and integer register colours only.
void itemUseDisp(WinMgrEntry* entry) {
    WindowWork* work = winGetPtr();
    WindowPartyDt* list[7];
    WindowPartyDt** lp;
    WindowPartyDt* party;
    GXColor color = dat_8041d480;
    GXColor iconColor = dat_8041d484;
    GXColor white = dat_8041d484;
    s32 ofs2;
    s32 ofs;
    s32 i;
    s32 sel2e0;
    f32 f31;
    f32 w;
    const char* name;
    s32 n;
    s32 gray;
    Vec tcopy;

    if (winMgrAction(WW_S32(work, 0x1208))) {
        return;
    }

    {
        s32 partyId;

        partyId = !marioBgmodeChk() ? marioGetParty() : marioGetPtr()->prevPartyMemberId[0];

        party = winPartyDt;
        n = 0;
        for (i = 0; i < 7; i++) {
            if (party[i].field_0x0 == partyId) {
                list[n++] = &party[i];
            }
        }
        for (i = 0; i < 7; i++) {
            if (party[i].field_0x0 != partyId) {
                list[n++] = &party[i];
            }
        }

    }

    lp = list;
    for (i = 0, ofs = 0, ofs2 = 0; i < WW_S32(work, 0x1E0) + 1; ofs += 0x16, lp++, ofs2 += 0x1C, i++) {
        if (i == 0) {
            f31 = float_0_8041d4ac;
        } else if (i == 1) {
            f31 = float_neg16_8041d4b0;
        } else {
            f31 = float_neg28_8041d4b4;
        }

        sel2e0 = WW_S32(work, 0x2E0);
        gray = 0;
        if (sel2e0 != 0) {
            if (i == WW_S32(work, 0x2DC)) {
                color = dat_8041d488;
            } else {
                color = dat_8041d48c;
                gray = 1;
            }
        }

        if (i == 0) {

            if (sel2e0 % 20 > 15 && i == WW_S32(work, 0x2DC)) {
                continue;
            }
            if (gray) {
                winIconGrayInit();
            } else {
                winIconInit();
            }
            {
                GXColor c = iconColor;
                Vec scale = vec3_802f53a8;
                Vec translate = vec3_802f539c;
                translate.x = (f32)(entry->field_0x18 + 0x1E);
                translate.y = (f32)(entry->field_0x1C - 0x14 - ofs);
#ifdef MUST_MATCH
                tcopy = translate;
                winIconSet(0x19E, &tcopy, &scale, &c);
#else
                winIconSet(0x19E, &translate, &scale, &c);
#endif
            }
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
            GXSetTevAlphaIn(0, 7, 1, 4, 7);
            {
                GXColor c = dat_8041d490;
                Vec scale = vec3_802f53c0;
                Vec translate = vec3_802f53b4;
                w = (f32)FontGetMessageWidth(str_8041D4BC);
                translate.x = float_0p9_8041d4b8 * w * float_0p5_8041d4a8 + (f32)(entry->field_0x18 + 0xDC);
                translate.y = (f32)(entry->field_0x1C - 0x13 - ofs);
                winTexSet(0xAF, translate, scale, c);
            }
            {
                GXColor c = dat_8041d494;
                Vec scale = vec3_802f53d8;
                Vec translate = vec3_802f53cc;
                w = (f32)FontGetMessageWidth(str_8041D4BC);
                translate.x = float_0p9_8041d4b8 * w * float_0p5_8041d4a8 + (f32)(entry->field_0x18 + 0xDC);
                translate.y = (f32)(entry->field_0x1C - 0x13 - (i + 1) * 0x16);
                winTexSet(0xAF, translate, scale, c);
            }
            winFontInit();
            {
                GXColor c = color;
                Vec scale = vec3_802f53f0;
                Vec translate = vec3_802f53e4;
                Vec tcopy;
                translate.x = (f32)(entry->field_0x18 + 0x32);
                translate.y = (f32)(entry->field_0x1C - 8 - ofs);
                tcopy = translate;
                winFontSet(&tcopy, &scale, &c, msgSearch("name_mario"));
            }
            {
                GXColor c = color;
                Vec scale = vec3_802f5408;
                Vec translate = vec3_802f53fc;
                Vec tcopy;

                translate.x = (f32)(entry->field_0x18 + 0xDC);
                translate.y = (f32)(entry->field_0x1C - 8 - ofs);
                tcopy = translate;
                winFontSet(&tcopy, &scale, &c, str_8041D4BC);
            }
            {
                Vec scale;
                Vec translate = vec3_802f5414;
                Vec tcopy;
                GXColor c = color;
                scale = vec3_802f5420;
                translate.x = (f32)(entry->field_0x18 + 0xF5);
                translate.y = (f32)(entry->field_0x1C - 8 - ofs);
                tcopy = translate;
                winFontSetR(&tcopy, &scale, &c, str_8041D4C4, pouchGetHP());
            }
            {
                GXColor c = color;
                Vec scale = vec3_802f5438;
                Vec translate = vec3_802f542c;
                Vec tcopy;

                translate.x = (f32)(entry->field_0x18 + 0x10B);
                translate.y = (f32)(entry->field_0x1C - 8 - ofs);
                tcopy = translate;
                winFontSetR(&tcopy, &scale, &c, str_8041D4C8);
            }
            {
                Vec scale;
                Vec translate = vec3_802f5444;
                Vec tcopy;
                GXColor c = color;
                scale = vec3_802f5450;
                translate.x = (f32)(entry->field_0x18 + 0x136);
                translate.y = (f32)(entry->field_0x1C - 0xA - ofs);
                tcopy = translate;
                winFontSet(&tcopy, &scale, &c, str_8041D4C4, pouchGetMaxHP());
            }
            if (itemDataTable[WW_S32(work, 0x2D4)].fp_restored != 0) {
                if (WW_S32(work, 0x2E0) % 20 > 15) {
                    continue;
                }
                color = dat_8041d498;
            }
            winFontInit();
            {
                Vec scale;
                Vec translate = vec3_802f545c;
                Vec tcopy;
                GXColor c = color;
                scale = vec3_802f5468;
                translate.x = (f32)(entry->field_0x18 + 0xDC);
                translate.y = (f32)(entry->field_0x1C - 8 - (i + 1) * 0x16);
                tcopy = translate;
                winFontSet(&tcopy, &scale, &c, str_8041D4CC);
            }
            {
                GXColor c = color;
                Vec scale = vec3_802f5480;
                Vec translate = vec3_802f5474;
                Vec tcopy;

                translate.x = (f32)(entry->field_0x18 + 0xF5);
                translate.y = (f32)(entry->field_0x1C - 8 - (i + 1) * 0x16);
                tcopy = translate;
                winFontSetR(&tcopy, &scale, &c, str_8041D4C4, pouchGetFP());
            }
            {
                Vec scale;
                Vec translate = vec3_802f548c;
                Vec tcopy;
                GXColor c = color;
                scale = vec3_802f5498;
                translate.x = (f32)(entry->field_0x18 + 0x10B);
                translate.y = (f32)(entry->field_0x1C - 8 - (i + 1) * 0x16);
                tcopy = translate;
                winFontSetR(&tcopy, &scale, &c, str_8041D4C8);
            }
            {
                GXColor c = color;
                Vec scale = vec3_802f54b0;
                Vec translate = vec3_802f54a4;
                Vec tcopy;

                translate.x = (f32)(entry->field_0x18 + 0x136);
                translate.y = (f32)(entry->field_0x1C - 0xA - (i + 1) * 0x16);
                tcopy = translate;
                winFontSet(&tcopy, &scale, &c, str_8041D4C4, pouchGetMaxFP());
            }
        } else {



            if (lp[-1]->field_0x0 != 4) {
                name = msgSearch(lp[-1]->field_0x8);
            } else {
                name = pouchGetYoshiName();
            }
            if (WW_S32(work, 0x2E0) % 20 > 15 && i == WW_S32(work, 0x2DC)) {
                continue;
            }
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
            GXSetTevAlphaIn(0, 7, 1, 4, 7);
            {
                GXColor c = dat_8041d49c;
                Vec scale = vec3_802f54c8;
                Vec translate = vec3_802f54bc;
                w = (f32)FontGetMessageWidth(str_8041D4BC);
                translate.x = float_0p9_8041d4b8 * w * float_0p5_8041d4a8 + (f32)(entry->field_0x18 + 0xDC);
                translate.y = f31 + ((f32)(entry->field_0x1C - 8 - ofs2)) - float_14_8041d4d4;
                winTexSet(0xAF, translate, scale, c);
            }
            if (gray) {
                winIconGrayInit();
            } else {
                winIconInit();
            }
            {
                GXColor c = iconColor;
                Vec scale = vec3_802f54e0;
                Vec translate = vec3_802f54d4;
                Vec tcopy;
                translate.x = (f32)(entry->field_0x18 + 0x1E);
                translate.y = f31 + ((f32)(entry->field_0x1C - 0x14 - ofs2));
#ifdef MUST_MATCH
                tcopy = translate;
                winIconSet(lp[-1]->field_0x4, &tcopy, &scale, &c);
#else
                winIconSet(lp[-1]->field_0x4, &translate, &scale, &c);
#endif
            }
            winFontInit();
            {
                GXColor c = color;
                Vec scale = vec3_802f54f8;
                GXColor* pc = &c;
                Vec cscale;
                Vec translate;
                w = (f32)FontGetMessageWidth(name);
                if (w < float_160_8041d4dc) {
                    scale.x = float_1_8041d4d8;
                } else {
                    scale.x = float_160_8041d4dc / (f32)FontGetMessageWidth(name);
                }
                cscale = scale;
                translate = vec3_802f54ec;
                translate.x = (f32)(entry->field_0x18 + 0x32);
                translate.y = f31 + ((f32)(entry->field_0x1C - 8 - ofs2));
                tcopy = translate;
                winFontSet(&tcopy, &cscale, pc, name);
            }
            {
                GXColor c = color;
                Vec scale = vec3_802f5510;
                Vec translate = vec3_802f5504;
                Vec tcopy;

                translate.x = (f32)(entry->field_0x18 + 0xDC);
                translate.y = f31 + ((f32)(entry->field_0x1C - 8 - ofs2));
                tcopy = translate;
                winFontSet(&tcopy, &scale, &c, str_8041D4BC);
            }
            {
                Vec scale;
                Vec translate = vec3_802f551c;
                GXColor c = color;
                scale = vec3_802f5528;
                translate.x = (f32)(entry->field_0x18 + 0xF5);
                translate.y = f31 + ((f32)(entry->field_0x1C - 8 - ofs2));
                tcopy = translate;
                winFontSetR(&tcopy, &scale, &c, str_8041D4C4, pouchGetPtr()->partyData[lp[-1]->field_0x0].currentHP);
            }
            {
                GXColor c = color;
                Vec scale = vec3_802f5540;
                Vec translate = vec3_802f5534;

                translate.x = (f32)(entry->field_0x18 + 0x10B);
                translate.y = f31 + ((f32)(entry->field_0x1C - 8 - ofs2));
                tcopy = translate;
                winFontSetR(&tcopy, &scale, &c, str_8041D4C8);
            }
            {
                GXColor c = color;
                Vec scale = vec3_802f5558;
                Vec translate = vec3_802f554c;

                translate.x = (f32)(entry->field_0x18 + 0x136);
                translate.y = f31 + ((f32)(entry->field_0x1C - 8 - ofs2)) - float_2_8041d4e0;
                tcopy = translate;
                winFontSet(&tcopy, &scale, &c, str_8041D4C4, pouchGetPtr()->partyData[lp[-1]->field_0x0].maximumHP);
            }
        }
    }

    if (WW_S32(work, 0x1E0) > 1) {
        winIconInit();
        for (n = 2; n < 0x1E; n++) {
            GXColor c = color;
#ifdef MUST_MATCH
            /* Retail reloads these rodata Vecs every iteration instead of hoisting them into
               callee-saved registers; the volatile view keeps the loads in the loop and the
               objects in .rodata. */
            Vec scale = *(const volatile Vec*)&vec3_802f5570;
            Vec translate = *(const volatile Vec*)&vec3_802f5564;
#else
            Vec scale = vec3_802f5570;
            Vec translate = vec3_802f5564;
#endif
            translate.x = (f32)(entry->field_0x18 + (entry->field_0x20 / 31) * n);
            translate.y = (f32)(entry->field_0x1C - 0x54);
#ifdef MUST_MATCH
            tcopy = translate;
            winIconSet(0x1B7, &tcopy, &scale, &c);
#else
            winIconSet(0x1B7, &translate, &scale, &c);
#endif
        }
    }
    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    {
        Vec scale = *(const volatile Vec*)&vec3_802f5588;
        Vec translate = *(const volatile Vec*)&vec3_802f557c;
        GXColor c = dat_8041d4a0;
        translate.y = WW_F32(work, 0x154) - float_2_8041d4e0;
        translate.x = float_2_8041d4e0 + WW_F32(work, 0x150);
        winTexSet(0, translate, scale, c);
    }
    {
        Vec scale = *(const volatile Vec*)&vec3_802f55a0;
        Vec translate = *(const volatile Vec*)&vec3_802f5594;
        GXColor c = white;
        translate.x = WW_F32(work, 0x150);
        translate.y = WW_F32(work, 0x154);
        winTexSet(0, translate, scale, c);
    }
}

// The original reaches vec3_802f55ac/b8, str_802F4CE8 and the two int->float pool doubles through
// one base register (r31 = str_802F4CE8) plus displacements (0x8c4, 0x8d0, 0x8f0, 0x8f8). Defining
// the .rodata prefix in this unit is what makes MWCC emit those offsets; the pool doubles must stay
// un-named so the compiler puts its own @NNN@ entries at 0x8f0/0x8f8 (naming them makes MWCC append
// fresh copies after the whole section, shifting them to 0x958). The strings after str_802F55C4 are
// left extern for the same reason; they belong inline in the functions that use them (other agents).
// 100%: the .rodata prefix plus the MUST_MATCH shapes below (GXColor copied into the 0x8 argument
// slot from an inner block, the pos temp/struct copy, and the dh0 temp that fixes the FP colours).
void itemUseDisp2(WinMgrEntry* entry) {
    WindowWork* work = winGetPtr();
    s32 id = WW_S32(work, 0x120C);
    const char* msg;
    f32 width;
    GXColor color;
    Vec pos;
#ifdef MUST_MATCH
    Vec tmp;
    f32 dh0;
#endif
    Vec scale;

    color = dat_8041d4a4;
    if (!winMgrAction(id)) {
        msg = msgSearch(str_802F55C4);
        width = (f32)FontGetMessageWidth(msg);
        winFontInit();
        // The original keeps the GXColor in its own 0xc home and copies it to the 0x8 argument
        // slot for the variadic winFontSet call; declaring the copy in this inner block (after
        // winFontInit) reproduces both the slot and the copy's schedule.
        {
            GXColor c = color;
            scale = vec3_802f55b8;
#ifdef MUST_MATCH
            // A separate temp plus the struct copy is what reproduces the original's 0x1c scratch
            // and the lwz/stw (not lfs/stfs) copy of pos into its final slot. The dh0 temp is what
            // colours the pool double f3 and 0.5f f1 (target), rather than f2/f3.
            tmp = vec3_802f55ac;
            dh0 = (f32)entry->field_0x20 - width;
            tmp.x = (f32)entry->field_0x18 + dh0 * float_0p5_8041d4a8;
            pos = tmp;
#else
            pos = vec3_802f55ac;
            pos.x = (f32)entry->field_0x18 + ((f32)entry->field_0x20 - width) * float_0p5_8041d4a8;
#endif
            winFontSet(&pos, &scale, &c, msg);
        }
    }
}

static WinMgrDesc window_desc[2] = {
    {
        {0},
        8, // camId
        -0x46,
        0x96,
        0x15E,
        0x108,
        {0xF0, 0xF0, 0xF0, 0xFF},
        NULL,
        (void*)itemUseDisp,
    },
    {
        {0, 0, 0, 2},
        8, // camId
        -0x118,
        0x91,
        0xC8,
        0x32,
        {0xFF, 0x99, 0x00, 0xFF},
        NULL,
        (void*)itemUseDisp2,
    },
};

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
#include "system.h"

// These .rodata strings are deliberately NOT defined here: defining them would push the
// compiler's anonymous int->float pool doubles behind them (0x958 instead of 0x8f0), which
// breaks the base+offset operands of itemUseDisp2/winItemDisp. They belong inline in the
// functions that use them (owned by the other agents working on this file).
// The pool doubles themselves need no declaration: the compiler emits its own @NNN@ entries.
extern const char str_802F55F4[];
extern const char str_802F560C[];
extern const char str_802F5620[];
extern const char str_802F5638[];

// Extended fields of WindowWork used by this unit. win_main.h only declares the
// first 0x1210 bytes, so the rest are reached through raw offsets below.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_S16(w, off) (*(s16*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

extern u16 menu_skip_list[18];
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
// each winIconSet gets a by-value GXColor copy like the original. Still differs in the
// stack-slot order (the original allocates the by-value Vec/GXColor argument copies above
// the source Vec locals; our emulated copies land below them, frame 0x230 vs 0x250) and the
// integer colours (ours saves r22-r31, the original r23-r31: grayFlag does not reuse half's
// r26, and i/parity/entry are rotated).
// NONMATCHING (87.03%): stack-slot order and integer register colours, see above.
void item_disp(WindowWork* work, f32 x, f32 y) {
    GXColor white = dat_8041d474;
    GXColor dark = dat_8041d478;
    GXColor gray = dat_8041d47c;
    f32 twenty;
    f32 base;
    f32 f22;
    f32 f26;
    f32 f25;
    s32 count;
    s32 i;
    u8* entry;

    if (WW_S32(work, 0x210) == 0) {
        count = pouchGetHaveItemCnt();
    } else {
        count = WW_S16(work, 0x3DA);
    }

    GXSetScissor(0, 0x6D, 0x260, 0xC0);
    twenty = float_20_8041d4e8;
    base = float_100_8041d4e4 + y;

    for (i = 0, entry = (u8*)work; i < count; i++, entry += 2) {
        s32 item;
        s32 half = i / 2;
        s32 parity = i % 2;
        s32 colored;
        s32 grayFlag;

        if (WW_S32(work, 0x210) == 0) {
            item = pouchHaveItem(i);
        } else {
            item = *(s16*)(entry + 0x2E8);
        }
        f25 = *(f32*)((u8*)work + WW_S32(work, 0x210) * 4 + 0x224);

        colored = (WW_S32(work, 0x210) != 0) || (itemDataTable[item].usable_locations & 4);
        f26 = base - (f32)(half * 0x26);
        grayFlag = (colored == 0);
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
            Vec translate = vec3_802f52d0;
            Vec scale;
#ifdef MUST_MATCH
            // The original calls winIconSet with by-value Vecs (one copy per struct);
            // the shared header declares Vec* (see report), so copy them here to match.
            Vec tcopy;
            Vec scopy;
#endif

            if (WW_S32(work, 0x210) == 0) {
                scale = vec3_802f52dc;
            } else {
                scale = vec3_802f52e8;
            }

            translate.x = (x - float_125_8041d4f8) + (f32)(parity * 0xCC);
            translate.y = f22;
#ifdef MUST_MATCH
            tcopy = translate;
            scopy = scale;
            winIconSet(*(u16*)&itemDataTable[item].icon_id, &tcopy, &scopy, &iconColor);
#else
            winIconSet(*(u16*)&itemDataTable[item].icon_id, &translate, &scale, &iconColor);
#endif
        }
        winFontInit();
        {
            GXColor color = grayFlag ? gray : dark;
            Vec translate = vec3_802f52f4;
            Vec scale = vec3_802f5300;

            translate.x = (x - float_105_8041d4fc) + (f32)(parity * 0xCC);
            translate.y = f25 + (float_20_8041d4e8 + f26);
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
                Vec translate = vec3_802f530c;
                Vec scale = vec3_802f5318;

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (float_18_8041d504 + (float_110_8041d508 + y));
                winTexSet(0x17, translate, scale, white);
            }
            winIconInit();
            {
                Vec translate = vec3_802f5324;
                Vec scale = vec3_802f5330;
#ifdef MUST_MATCH
                Vec tcopy;
                Vec scopy;
                GXColor ccopy;
#endif

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (float_113_8041d50c + y);
#ifdef MUST_MATCH
                tcopy = translate;
                scopy = scale;
                ccopy = white;
                winIconSet(0x7E, &tcopy, &scopy, &ccopy);
#else
                winIconSet(0x7E, &translate, &scale, &white);
#endif
            }
        }
        if (count > WW_S32(work, 0x21C + WW_S32(work, 0x210) * 4) * 10 + 10) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            f22 = float_250_8041d500 + x;
            {
                Vec translate = vec3_802f533c;
                Vec scale = vec3_802f5348;

                translate.x = f22;
                translate.y = float_20_8041d4e8 + ((y - float_90_8041d510) - float_15_8041d514);
                winTexSet(0x17, translate, scale, white);
            }
            winIconInit();
            {
                Vec translate = vec3_802f5354;
                Vec scale = vec3_802f5360;
#ifdef MUST_MATCH
                Vec tcopy;
                Vec scopy;
                GXColor ccopy;
#endif

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (y - float_90_8041d510);
#ifdef MUST_MATCH
                tcopy = translate;
                scopy = scale;
                ccopy = white;
                winIconSet(0x80, &tcopy, &scopy, &ccopy);
#else
                winIconSet(0x80, &translate, &scale, &white);
#endif
            }
        }
    }

    {
        const char* msg = msgSearch(str_802F55F4);
        f32 w = float_0p7_8041d518 * (f32)FontGetMessageWidth(msg);

        winFontInit();
        f22 = x + (float_220_8041d51c - w);
        {
            Vec translate = vec3_802f536c;
            Vec scale = vec3_802f5378;

            translate.x = f22;
            translate.y = float_20_8041d4e8 + ((y - float_92_8041d520) - float_10_8041d4ec);
            winFontSet(&translate, &scale, &dark, msgSearch(str_802F55F4));
        }
        winIconInit();
        {
            Vec translate = vec3_802f5384;
            Vec scale = vec3_802f5390;
#ifdef MUST_MATCH
            Vec tcopy;
            Vec scopy;
            GXColor ccopy;
#endif

            translate.x = f22 - float_30_8041d524;
            translate.y = float_20_8041d4e8 + ((y - float_100_8041d4e4) - float_10_8041d4ec);
#ifdef MUST_MATCH
            tcopy = translate;
            scopy = scale;
            ccopy = white;
            winIconSet(0x217, &tcopy, &scopy, &ccopy);
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

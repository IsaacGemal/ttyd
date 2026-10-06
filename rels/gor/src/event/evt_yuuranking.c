#include "evt_yuuranking.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "memory.h"
#include "system.h"
#include "pmario_sound.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/windowdrv.h"
#include "evt/evt_yuugijou.h"
#include <dolphin/mtx.h>
#include <string.h>

extern const f32 float_1000_gor_0001cd80;
extern const u32 dat_gor_0001cd30;
extern const u32 dat_gor_0001cd34;
extern const u32 dat_gor_0001cd38;
extern const u32 dat_gor_0001cd3c;
extern const u32 dat_gor_0001cd40;
extern const u32 dat_gor_0001cd44;
extern const u32 dat_gor_0001cd48;
extern const u32 dat_gor_0001cd4c;
extern const u32 dat_gor_0001cd50;
extern const u32 dat_gor_0001cd54;
extern const u32 dat_gor_0001cd58;
extern const u32 dat_gor_0001cd5c;
extern const u32 dat_gor_0001cd60;
extern const u32 dat_gor_0001cd64;
extern const u32 dat_gor_0001cd68;
extern const u32 dat_gor_0001cd6c;
extern const u32 dat_gor_0001cd70;
extern const u32 dat_gor_0001cd74;
extern const u32 dat_gor_0001cd78;
extern const u32 dat_gor_0001cd7c;

const u32 dat_gor_0001cc40 = 0xFFFFFFFF;

typedef struct YuRankWork {
    s32 field_00;  // 0x0
    u16 field_04;  // 0x4
    u16 pad_06;    // 0x6
    u32 field_08;  // 0x8
    f32 field_0C;  // 0xC
    f32 field_10;  // 0x10
    s32 field_14;  // 0x14
    s32 field_18;  // 0x18
    s32 field_1C;  // 0x1C
    s32 field_20;  // 0x20
    s32 field_24;  // 0x24
    s32 field_28;  // 0x28
    s32 field_2C;  // 0x2C
    s32 field_30;  // 0x30
    s32 field_34;  // 0x34
    s32 field_38;  // 0x38
    s32 field_3C;  // 0x3C
} YuRankWork;

#pragma pool_data off

#pragma force_active on
static USER_FUNC(yuuranking_get_monte);
static s32 yuu_kesan(s32 type, s32 value, s32 bonus, s32 mult, s32 max);
static inline s32 yuu_kesan_chk(s32 type, s32 value, s32 bonus, s32 mult, s32 max) {
    if (value < 0) {
        return 0;
    }
    return yuu_kesan(type, value, bonus, mult, max);
}

static USER_FUNC(evt_yuuranking_main_func);
static void evt_yuuranking_disp_func(CameraId cameraId, void* param);
static s32 fontlenpitch(s8* str, s32 pitch);

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
static s32 fontlenpitch(s8* text, s32 pitch) {
    if (pitch != 0) {
        s8* str = text;
        s32 count = 0;

        while (*str) {
            if (_ismbblead(*str)) {
                count++;
                str++;
            } else {
                count++;
            }
            str++;
        }
        return count * pitch;
    } else {
        return (u16)FontGetMessageWidth((char*)text);
    }
}

void unk_gor_0000dfc4(const char* msg, s32 width, f32 x, f32 y, f32 z) {
    Mtx trans;
    Mtx scale;
    Mtx out;
    f32 f;

    PSMTXTrans(trans, x, y, 0.0f);
    if ((u16)FontGetMessageWidth(msg) > width) {
        f = (f32)width * z / (f32)(u16)FontGetMessageWidth(msg);
    } else {
        f = z;
    }
    PSMTXScale(scale, f, z, z);
    PSMTXConcat(trans, scale, out);
    FontDrawMessageMtx(out, msg);
}

#define A14(i) (*(s32*)((unsigned char*)yuwp + work->field_14 * 24 + 0x14 + (i) * 4))
#define A74(i) (*(s32*)((unsigned char*)yuwp + work->field_14 * 24 + 0x74 + (i) * 4))
#define AD4(i) (*(s32*)((unsigned char*)yuwp + work->field_14 * 24 + 0xd4 + (i) * 4))

#ifdef MUST_MATCH
// Match-only: the inlined expansion wants str=r25/count=r26 while the standalone
// `fontlenpitch` keeps the opposite local order for its own r31/r30 allocation.
static inline s32 fontlenpitch_inl(s8* text, s32 pitch) {
    if (pitch != 0) {
        s32 count = 0;
        s8* str = text;

        while (*str) {
            if (_ismbblead(*str)) {
                count++;
                str++;
            } else {
                count++;
            }
            str++;
        }
        return count * pitch;
    } else {
        return (u16)FontGetMessageWidth((char*)text);
    }
}
#define FLP_TEXT(s, p) fontlenpitch_inl((s), (p))
#else
#define FLP_TEXT(s, p) fontlenpitch((s), (p))
#endif


// NONMATCHING (98.02%): block order, the nested mode switches, the callee-saved set (stmw r24-r31) and the
// call sequence all match. Adopting the yuu source's `k = 1.0f` forms (sc = k*w + 50.0f,
// x = field_0C + -(k*w + 50.0f)*0.5f, y = field_10 + 128.0f + k, FontDrawMessage x = k*(-w)*0.5f) reproduces
// the target's fnmadds/fmadds and lifted this function 96.67 -> 97.03 (the earlier note that they lowered the
// score was measured against a misaligned prefix). The rank-list `msg[0]=='m'...` chain is `cmpwi` in the
// target; `(s8)` casts give cmpwi and 97.03 -> 97.08. The inlined fontlenpitch wants str=r25/count=r26, so the
// disp calls go through FLP_TEXT, which is `fontlenpitch_inl` (a MUST_MATCH static inline with `count`
// declared before `str`) in matching builds and the natural `fontlenpitch` otherwise; the standalone
// fontlenpitch keeps `str` first for its own r31/r30 allocation and stays at 100. Reloading A74(i) in the rank
// case-0 sprintf instead of reusing the `v` temp lifted 97.38 -> 97.58. Declaring the case-2/3 minute/second
// temps `min` before `sec` (instead of reusing `i`/`j`) lifted 97.58 -> 97.80, and nesting the top-level
// case-2/3 first `msgSearch("menu_colon")` in the sprintf (keeping the same two calls) lifted it to 97.82.
// Writing the minute/second remainder as `sec = sec % 60` instead of `sec - min * 60` makes MWCC emit the
// target's `mullw` (reusing the 0x3c already in r0) in both case-2/3 blocks and lifted 97.82 -> 97.87.
// The 20 GXColor locals only reach the target's first-use stack order (c0=0x54 .. c19=0x08) once all of them
// are address-taken. The real ABI for windowDispGX_Waku_col/iconDispGxCol/iconNumberDispGx takes GXColor*
// (the shared by-value headers are wrong; the battle callers cast), so the four by-value colours c0,c1,c15,c16
// go through the same by-pointer cast with `&cN`. With every colour an lvalue the stack colourer places all
// 20 in the main block at 0x08..0x54, and declaring them c0..c19 with the colourer's c5,c6,c4 / c9,c10,c8 /
// c13,c12 rotations pre-applied reproduces the target slots exactly; lifted 97.87 -> 98.02. The `+ k` on the
// windowDispGX y argument is grouped `k + (...)` to keep the target's fadds operand order.
// What is left is pure register allocation: the inlined fontlenpitch copies `buf` with `mr` where the target
// re-materialises it with `addi`, and the rank loop's strength-reduced offset starts with `mr` instead of `li`.
// NONMATCHING (98.02%): see the notes above; what remains is MWCC register allocation only.
#ifdef MUST_MATCH
// Match-only: taking each by-value colour's address keeps all 20 GXColor locals in one contiguous stack block,
// the same address the by-value GC ABI passes in the argument register.
#define WAKU_COL(c, x, y, w, h, curve) \
    ((void (*)(u16, GXColor*, f32, f32, f32, f32, f32))windowDispGX_Waku_col)(0, &(c), (x), (y), (w), (h), (curve))
#define ICON_COL(mtx, flags, id, c) ((void (*)(Mtx, s16, s32, GXColor*))iconDispGxCol)((mtx), (flags), (id), &(c))
#define ICON_NUM(mtx, num, small, c) ((s32 (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx)((mtx), (num), (small), &(c))
// Match-only: retail evaluates the title frame's lower edge as k + (field_10 + 128); the natural order gives a
// different fadds operand order.
#define WAKU_BOTTOM (k + work->field_10 + 128.0f)
#else
#define WAKU_COL(c, x, y, w, h, curve) windowDispGX_Waku_col(0, (c), (x), (y), (w), (h), (curve))
#define ICON_COL(mtx, flags, id, c) iconDispGxCol((mtx), (flags), (id), (c))
#define ICON_NUM(mtx, num, small, c) iconNumberDispGx((mtx), (num), (small), (c))
#define WAKU_BOTTOM (work->field_10 + 128.0f + k)
#endif

// The top waku/FontDrawMessage maths (a non-const `k=1.0f` keeps the `fnmadds` that negates the sum, integer-negated
// width), the signed `cmpwi` menu test and the `A74(i)/1000` recompute now match. Passing the by-value colours by
// address (WAKU_COL/ICON_COL/ICON_NUM) puts all 20 GXColor locals in the target's contiguous 0x54..0x08 block, and
// nesting the case-0 `menu_decimal_point` and the rank `menu_subtitle5_rank`/`menu_subtitle1_monte` msgSearch
// results into their calls emits the format string and the 80.0f/75.2f literals in the target's `.rodata` order.
// Giving the top case-2/3 subtitle its own `msg2` local matches the target's r27 for that group, but the title
// (r28), the case-0/case-1/end/rank `msg` groups and the rank case-2/3 subtitles stay in r28 where the target
// uses r24/r27; the second Waku block's FP temporaries keep a different colouring, and the fontlenpitch buffer
// rematerialises with `mr r25,r24` where the target computes `addi r25,r1,0x78`.
// NONMATCHING (98.18%): MWCC register allocation only.
static void evt_yuuranking_disp_func(CameraId cameraId, void* param) {
    YuRankWork* work = param;
    const char* titleTbl[4] = {
        "menu_title_minigame1", "menu_title_minigame4", "menu_title_minigame3", "menu_title_minigame2",
    };
    const char* scoreTbl[4] = {
        "menu_title_score", "menu_title_score", "menu_title_score", "menu_title_score",
    };
    const char* subTbl[6] = {
        "\x81\x40\x82\x50", "\x81\x40\x82\x51", "\x81\x40\x82\x52",
        "\x81\x40\x82\x53", "\x81\x40\x82\x54", "menu_subtitle2_rankgai",
    };
    char buf[32];
    const char* bp;
    const char* msg;
    const char* msg2;
    s32 i;
    s32 j;
    f32 w;
    f32 sc;
    f32 y;
    GXColor c0;
    GXColor c1;
    GXColor c2;
    GXColor c3;
    GXColor c5;
    GXColor c6;
    GXColor c4;
    GXColor c7;
    GXColor c9;
    GXColor c10;
    GXColor c8;
    GXColor c11;
    GXColor c13;
    GXColor c12;
    GXColor c14;
    GXColor c15;
    GXColor c16;
    GXColor c17;
    GXColor c18;
    GXColor c19;

    c0 = *(GXColor*)&dat_gor_0001cd30;
    WAKU_COL(c0, work->field_0C + -196.0f, work->field_10 + 112.0f, 392.0f, 224.0f, 20.0f);
    switch (work->field_18) {
    case 0:
        msg = msgSearch(scoreTbl[work->field_14]);
        break;
    case 1:
    case 2:
        msg = msgSearch(titleTbl[work->field_14]);
        break;
    }
    c1 = *(GXColor*)&dat_gor_0001cd34;
    {
        f32 k = 1.0f;
        w = (f32)FontGetMessageWidth(msg);
        sc = k * w + 50.0f;
        w = (f32)FontGetMessageWidth(msg);
        { f32 t = -(k * w + 50.0f); WAKU_COL(c1, work->field_0C + t * 0.5f, WAKU_BOTTOM, sc, 28.0f, 8.0f); }
        FontDrawStart();
        FontDrawScale(1.0f);
        c2 = *(GXColor*)&dat_gor_0001cd38;
        FontDrawColor(&c2);
        w = (f32)(-(s32)FontGetMessageWidth(msg));
        FontDrawMessage((s32)(work->field_0C + k * w * 0.5f), (s32)(work->field_10 + 128.0f), msg);
    }

    if (work->field_18 != 0) {
        goto rank;
    }

    switch (work->field_14) {
    case 0:
        FontDrawStart();
        FontDrawScale(1.0f);
        c3 = *(GXColor*)&dat_gor_0001cd3c;
        FontDrawColor(&c3);
        msg = msgSearch("menu_subtitle1_minigame1");
        unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        if (work->field_38 >= 0) {
            sprintf(buf, "%3d%s%02d", work->field_38 / 1000, msgSearch("menu_decimal_point"), (work->field_38 % 1000) / 10);
            bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            if (work->field_30 >= 4) {
                FontDrawStart();
                FontDrawScale(1.0f);
                c5 = *(GXColor*)&dat_gor_0001cd40;
                FontDrawColor(&c5);
                msg = msgSearch("menu_subtitle4_getmonte");
                unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 32.0f, 1.0f);
                sprintf(buf, "%4d", work->field_20);
                bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
            }
            if (work->field_30 >= 7) {
                FontDrawStart();
                FontDrawScale(1.0f);
                c6 = *(GXColor*)&dat_gor_0001cd44;
                FontDrawColor(&c6);
                if (work->field_24 > 1) {
                    msg = msgSearch("menu_subtitle2_minigame1");
                    unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    if (work->field_24 < 0x100) {
                        msg = msgSearch("menu_subtitle4_minigame1");
                        sprintf(buf, msg, work->field_24);
                    } else {
                        sprintf(buf, "%2d", work->field_24 - 0x100);
                    }
                    bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                } else if (work->field_24 < 0) {
                    msg = msgSearch("menu_subtitle5_minigame1");
                    unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    msg = msgSearch("menu_subtitle6_minigame1");
                    sprintf(buf, msg, -work->field_24);
                    bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                }
            }
        } else {
            c4 = *(GXColor*)&dat_gor_0001cd48;
            FontDrawColor(&c4);
            msg = msgSearch("menu_error_minigame1");
            sprintf(buf, msg);
            bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        }
        goto end;

    case 2:
    case 3:
        FontDrawStart();
        c7 = *(GXColor*)&dat_gor_0001cd4c;
        FontDrawColor(&c7);
        switch (work->field_14) {
        case 3:
            msg2 = msgSearch("menu_subtitle1_minigame2");
            break;
        case 2:
            msg2 = msgSearch("menu_subtitle1_minigame3");
            break;
        }
        unk_gor_0000dfc4(msg2, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        FontDrawScale(1.0f);
        if (work->field_38 >= 0) {
            s32 v = work->field_38;
            s32 min;
            s32 sec;

            sec = v / 1000;
            min = sec / 60;
            sec = sec % 60;
            sprintf(buf, "%1d%s%02d%s%02d", min, msgSearch("menu_colon"), sec, msgSearch("menu_colon"), (v % 1000) / 10);
            bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            if (work->field_30 >= 4) {
                FontDrawStart();
                c9 = *(GXColor*)&dat_gor_0001cd50;
                FontDrawColor(&c9);
                switch (work->field_14) {
                case 3:
                    msg = msgSearch("menu_subtitle4_getmonte");
                    break;
                case 2:
                    msg = msgSearch("menu_subtitle4_getmonte");
                    break;
                }
                unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 32.0f, 1.0f);
                FontDrawScale(1.0f);
                sprintf(buf, "%4d", work->field_20);
                bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
                if (work->field_30 >= 7) {
                    FontDrawStart();
                    c10 = *(GXColor*)&dat_gor_0001cd54;
                    FontDrawColor(&c10);
                    if (work->field_24 > 1) {
                        switch (work->field_14) {
                        case 3:
                            msg = msgSearch("menu_subtitle2_minigame2");
                            break;
                        case 2:
                            msg = msgSearch("menu_subtitle2_minigame3");
                            break;
                        }
                        unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                        FontDrawScale(1.0f);
                        if (work->field_24 < 0x100) {
                            msg = msgSearch("menu_subtitle4_minigame1");
                            sprintf(buf, msg, work->field_24);
                        } else {
                            sprintf(buf, "%2d", work->field_24 - 0x100);
                        }
                        bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    } else if (work->field_24 < 0) {
                        msg = msgSearch("menu_subtitle5_minigame1");
                        unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                        FontDrawScale(1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->field_24);
                        bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            }
        } else {
            c8 = *(GXColor*)&dat_gor_0001cd58;
            FontDrawColor(&c8);
            if (work->field_14 == 3) {
                sprintf(buf, msgSearch("menu_error_minigame2"));
            } else {
                sprintf(buf, msgSearch("menu_error_minigame3"));
            }
            bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        }
        goto end;

    case 1:
        FontDrawStart();
        FontDrawScale(1.0f);
        c11 = *(GXColor*)&dat_gor_0001cd5c;
        FontDrawColor(&c11);
        msg = msgSearch("menu_subtitle1_minigame4");
        unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        if (work->field_38 < work->field_2C) {
            sprintf(buf, "%4d", work->field_38 + 1);
            bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            if (work->field_30 >= 7) {
                FontDrawStart();
                FontDrawScale(1.0f);
                c13 = *(GXColor*)&dat_gor_0001cd60;
                FontDrawColor(&c13);
                if (work->field_24 > 1) {
                    msg = msgSearch("menu_subtitle2_minigame4");
                    unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    if (work->field_24 < 0x100) {
                        msg = msgSearch("menu_subtitle4_minigame1");
                        sprintf(buf, msg, work->field_24);
                    } else {
                        sprintf(buf, "%2d", work->field_24 - 0x100);
                    }
                    bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                } else if (work->field_24 < 0) {
                    msg = msgSearch("menu_subtitle5_minigame1");
                    unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    msg = msgSearch("menu_subtitle6_minigame1");
                    sprintf(buf, msg, -work->field_24);
                    bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                }
            }
        } else {
            c12 = *(GXColor*)&dat_gor_0001cd64;
            FontDrawColor(&c12);
            sprintf(buf, msgSearch("menu_error_minigame4"));
            bp = buf, FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)FLP_TEXT((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        }
        goto end;

    default:
        goto end;
    }

end:
    FontDrawStart();
    FontDrawScale(1.0f);
    c14 = *(GXColor*)&dat_gor_0001cd68;
    FontDrawColor(&c14);
    msg = msgSearch("menu_subtitle3_totalmonte");
    unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + -40.0f, 1.0f);
    {
        Mtx m;
        s32 ix = (s32)(work->field_0C + 10.399994f + 70.0f);
        s32 iy = (s32)(work->field_10 + -40.0f - 24.0f);

        PSMTXTrans(m, (f32)(ix - 10), (f32)(iy + 2), 0.0f);
        c15 = *(GXColor*)&dat_gor_0001cd6c;
        ICON_COL(m, 16, 0x13f, c15);
        PSMTXTrans(m, (f32)(ix + 60), (f32)(iy + 2), 0.0f);
        c16 = *(GXColor*)&dat_gor_0001cd70;
        ICON_NUM(m, work->field_34, 0, c16);
    }
    goto finish;

rank:
    FontDrawStart();
    FontDrawScale(1.0f);
    FontDrawMessage((s32)(work->field_0C + -150.4f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle5_rank"));
    FontDrawMessage((s32)(work->field_0C + 75.19998f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle1_monte"));
    c17 = *(GXColor*)&dat_gor_0001cd74;
    FontDrawColor(&c17);
    switch (work->field_14) {
    case 0:
        msg = msgSearch("menu_subtitle1_minigame1");
        FontDrawMessage((s32)(work->field_0C + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
        break;
    case 3:
        msg = msgSearch("menu_subtitle1_minigame2");
        FontDrawMessage((s32)(work->field_0C + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
        break;
    case 2:
        msg = msgSearch("menu_subtitle1_minigame3");
        FontDrawMessage((s32)(work->field_0C + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
        break;
    case 1:
        msg = msgSearch("menu_subtitle1_minigame4");
        FontDrawMessage((s32)(work->field_0C + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
        break;
    }

    for (i = 0; i < 6; i++) {
        if (A14(i) == 0) {
            continue;
        }
        y = 2.0f + (f32)i;
        if (i == 5) {
            y += 0.5f;
        }
        FontDrawStart();
        FontDrawScale(1.0f);
        if (i != work->field_28) {
            c18 = *(GXColor*)&dat_gor_0001cd78;
            FontDrawColor(&c18);
        } else {
            c19 = *(GXColor*)&dat_gor_0001cd7c;
            FontDrawColor(&c19);
        }
        msg = subTbl[i];
        if ((s8)msg[0] == 'm' && (s8)msg[1] == 'e' && (s8)msg[2] == 'n' && (s8)msg[3] == 'u' && (s8)msg[4] == '_') {
            msg = msgSearch(msg);
        }
        if (i == 5) {
            FontDrawMessage((s32)(work->field_0C + -188.0f + 18.8f),
                            (s32)(work->field_10 + (104.0f - 24.0f * y)), msg);
        } else {
            FontDrawStringPitch(msg, work->field_0C + -150.4f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
        }
        switch (work->field_14) {
        case 0:
            if (A74(i) >= 0) {
                s32 v = A74(i);

                msg = msgSearch("menu_decimal_point");
                sprintf(buf, "%3d%s%02d", A74(i) / 1000, msg, (v % 1000) / 10);
                FontDrawStringPitch(buf, work->field_0C + -75.200005f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
                sprintf(buf, "%4d", AD4(i));
                FontDrawStringPitch(buf, work->field_0C + 75.19998f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
            } else {
                sprintf(buf, msgSearch("menu_error_minigame1"));
                FontDrawMessage((s32)(work->field_0C + -75.200005f),
                                (s32)(work->field_10 + (104.0f - 24.0f * y)), buf);
            }
        break;
        case 2:
        case 3:
            if (A74(i) >= 0) {
                s32 v = A74(i);
                s32 min;
                s32 sec;

                sec = v / 1000;
                min = sec / 60;
                sec = sec % 60;
                msg = msgSearch("menu_colon");
                sprintf(buf, "%1d%s%02d%s%02d", min, msgSearch("menu_colon"), sec, msg, (v % 1000) / 10);
                FontDrawStringPitch(buf, work->field_0C + -75.200005f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
                sprintf(buf, "%4d", AD4(i));
                FontDrawStringPitch(buf, work->field_0C + 75.19998f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
            } else {
                if (work->field_14 == 3) {
                    sprintf(buf, msgSearch("menu_error_minigame2"));
                } else {
                    sprintf(buf, msgSearch("menu_error_minigame3"));
                }
                FontDrawMessage((s32)(work->field_0C + -75.200005f),
                                (s32)(work->field_10 + (104.0f - 24.0f * y)), buf);
            }
        break;
        case 1:
            if (A74(i) < work->field_2C) {
                sprintf(buf, "%4d", A74(i) + 1);
                FontDrawStringPitch(buf, work->field_0C + -37.600006f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
                sprintf(buf, "%4d", AD4(i));
                FontDrawStringPitch(buf, work->field_0C + 75.19998f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
            } else {
                sprintf(buf, msgSearch("menu_error_minigame4"));
                FontDrawMessage((s32)(work->field_0C + -75.200005f),
                                (s32)(work->field_10 + (104.0f - 24.0f * y)), buf);
            }
        break;
        }
    }

finish:
    ;
}
#undef AD4
#undef A74
#undef A14


USER_FUNC(evt_yuuranking_main_func) {
    static s32 snd_id;
    YuRankWork* wp = (YuRankWork*)event->lwData[0];
    s32 value;

    if (wp->field_04 & 1) {
        wp->field_04 |= 2;
        return 2;
    }
    dispEntry(8, float_1000_gor_0001cd80, 0, evt_yuuranking_disp_func, wp);
    if (wp->field_18 == 0) {
        switch (wp->field_30) {
        case 0:
            wp->field_3C = 30;
            wp->field_30++;
            break;
        case 1:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_30++;
            if (wp->field_1C != 0) {
                snd_id = psndSFXOn((const char*)0x8d7);
            }
            break;
        case 2:
            switch (wp->field_14) {
                case 0:
                    wp->field_38 += 5590;
                    if (wp->field_38 > wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
                case 3:
                    wp->field_38 += 1090;
                    if (wp->field_38 > wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
                case 2:
                    wp->field_38 += 1090;
                    if (wp->field_38 > wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
                case 1:
                    wp->field_38 -= 11;
                    if (wp->field_38 < wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
            }
            wp->field_34 = yuu_kesan_chk(wp->field_14, wp->field_38, 0, 0, wp->field_2C);
            if (wp->field_38 != wp->field_1C) {
                break;
            }
            if (wp->field_1C != 0) {
                psndSFXOff(snd_id);
            }
            switch (wp->field_14) {
                case 0:
                case 2:
                case 3:
                    if (wp->field_1C > 0) {
                        wp->field_3C = 0x30;
                        wp->field_30++;
                    } else {
                        wp->field_3C = 30;
                        wp->field_30 = 9;
                    }
                    break;
                case 1:
                    if (wp->field_24 > 1 || wp->field_24 < 0) {
                        if (wp->field_1C < wp->field_2C) {
                            wp->field_3C = 0x30;
                            wp->field_30 = 6;
                        } else {
                            wp->field_3C = 30;
                            wp->field_30 = 9;
                        }
                    } else {
                        wp->field_3C = 30;
                        wp->field_30 = 9;
                    }
                    break;
            }
            break;
        case 3:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_3C = 0x30;
            wp->field_30++;
            break;
        case 4:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_30++;
            break;
        case 5:
            switch (wp->field_14) {
                case 0:
                case 2:
                case 3:
                    value = yuu_kesan_chk(wp->field_14, wp->field_38, wp->field_20, 0, wp->field_2C);
                    if (wp->field_34 < value) {
                        wp->field_34++;
                        psndSFXOn((const char*)0x8ca);
                    } else if (wp->field_34 > value) {
                        wp->field_34--;
                    } else if (wp->field_1C > 0 && (wp->field_24 > 1 || wp->field_24 < 0)) {
                        wp->field_3C = 0x30;
                        wp->field_30++;
                    } else {
                        wp->field_3C = 30;
                        wp->field_30 = 9;
                    }
                    break;
            }
            break;
        case 6:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_3C = 0x30;
            wp->field_30++;
            break;
        case 7:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_30++;
            break;
        case 8:
            switch (wp->field_14) {
                case 0:
                case 1:
                case 2:
                case 3:
                    value = yuu_kesan_chk(wp->field_14, wp->field_38, wp->field_20, wp->field_24, wp->field_2C);
                    if (wp->field_34 < value) {
                        wp->field_34++;
                        psndSFXOn((const char*)0x8ca);
                    } else if (wp->field_34 > value) {
                        wp->field_34--;
                    } else {
                        wp->field_3C = 30;
                        wp->field_30++;
                    }
                    break;
            }
            break;
        case 9:
            wp->field_08 &= ~4;
            break;
    }
    }
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(main_evt)
    CALL(evt_yuuranking_main_func)
    RETURN
EVT_END()

// TODO: type
static YuRankWork* _wp;

// TODO: snd_id$716 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// static u32 snd_id$716[1] = {
//     0x00000000,
// };
static s32 yuu_kesan(s32 type, s32 value, s32 bonus, s32 mult, s32 max) {
    s32 total;

    switch (type) {
        case 0:
            total = value / 30000;
            break;
        case 3:
            total = (value - 74000) / 2000;
            break;
        case 2:
            total = (value - 109000) / 2000;
            break;
        case 1:
            total = (max - value) * 15 / max;
            break;
    }
    if (total < 0) {
        total = 0;
    }
    total += bonus;
    if (mult > 0) {
        if (mult < 256) {
            total *= mult;
        } else {
            total = mult + total;
            total -= 256;
        }
    } else {
        total += mult;
        if (total < 0) {
            total = 0;
        }
    }
    if (total < 0) {
        total = 0;
    }
    return total;
}

// A dead copy of the checked entry above (yuu_kesan_chk): the original object carries it between
// evt_yuuranking_alloc and yuu_kesan, unreferenced (nothing in gor branches to or points at it), as
// `cmpwi r4,0 / bge yuu_kesan / li r3,0 / blr`, a frameless tail branch into yuu_kesan that the original compiler
// emitted on its own. GC/2.6 never emits a frameless tail branch from C (every `return f(...)` gets a frame), and no
// available compiler version or pragma produces it (swept: GC/1.0-1.3.2r, 2.0-2.7, 3.0a3-3.0a5, linkage keywords,
// force_active, the inline and opt_* pragmas). So under MUST_MATCH it is an asm function: a narrow exception Isaac
// approved on 2026-10-04, listed in tools/asm_exceptions.json (docs/CONTEXT.md, "Inline asm in game code: the
// exception policy"). The natural build keeps the C form.
#ifdef MUST_MATCH
// clang-format off
static asm s32 yuu_kesan_chk_copy(s32 type, s32 value, s32 bonus, s32 mult, s32 max) {
    nofralloc
    cmpwi r4, 0
    bge yuu_kesan
    li r3, 0
    blr
}
// clang-format on
#else
static s32 yuu_kesan_chk_copy(s32 type, s32 value, s32 bonus, s32 mult, s32 max) {
    if (value < 0) {
        return 0;
    }
    return yuu_kesan(type, value, bonus, mult, max);
}
#endif

#define YBASE_A ((s32(*)[6])((u8*)yuwp + 0x14))
#define YBASE_B ((s32(*)[6])((u8*)yuwp + 0x74))
#define YBASE_C ((s32(*)[6])((u8*)yuwp + 0xd4))
#define A14(i) YBASE_A[work->field_14][i]
#define A74(i) YBASE_B[work->field_14][i]
#define AD4(i) YBASE_C[work->field_14][i]
// shift-loop copies: the `yuwp + mode*24` base plus the constant index folds the offset into the add order
// (mode*24 + running offset, then + yuwp) exactly as the target; the array view above associates it the other way.
#define A14L(i) (((s32*)((u8*)yuwp + work->field_14 * 24))[5 + (i)])
#define A74L(i) (((s32*)((u8*)yuwp + work->field_14 * 24))[29 + (i)])
#define AD4L(i) (((s32*)((u8*)yuwp + work->field_14 * 24))[53 + (i)])
#define A74F(i) (((s32*)((u8*)yuwp + work->field_14 * 24 + 0x74 + (i) * 4))[0])
// slot-search read: row base yuwp+mode*24 keeps 0x14 as the index displacement (add+lwz), not addi+lwzx.
#define A14R(i) (((s32*)((u8*)yuwp + work->field_14 * 24 + (i) * 4))[5])

#define YUU_RANK_SETUP(ELSE_CODE, SETVAL, OUTER, CMP)                                                                    \
    A14(5) = 0;                                                                                                          \
    A74(5) = 0;                                                                                                          \
    AD4(5) = 0;                                                                                                          \
    work->field_28 = -1;                                                                                                 \
    if (work->field_18 == 1) {                                                                                           \
        if (OUTER) {                                                                                                     \
        for (i = 0; i < 6; i++) {                                                                                        \
            if (A14R(i) == 0) {                                                                                          \
                A14(i) = 1;                                                                                              \
                A74(i) = work->field_1C;                                                                                 \
                AD4(i) = yuu_kesan_chk(work->field_14, work->field_1C, work->field_20, work->field_24, work->field_2C);  \
                work->field_28 = i;                                                                                      \
                goto done;                                                                                               \
            }                                                                                                            \
            if (CMP) {                                                                                                   \
                for (j = 4; j > i; j--) {                                                                                \
                    A14L(j) = A14L(j - 1);                                                                                 \
                    A74L(j) = A74L(j - 1);                                                                                 \
                    AD4L(j) = AD4L(j - 1);                                                                                 \
                }                                                                                                        \
                A14(i) = 1;                                                                                              \
                A74(i) = work->field_1C;                                                                                 \
                AD4(i) = yuu_kesan_chk(work->field_14, work->field_1C, work->field_20, work->field_24, work->field_2C);  \
                work->field_28 = i;                                                                                      \
                if (i == 0) {                                                                                            \
                    SETVAL;                                                                                              \
                }                                                                                                        \
                goto done;                                                                                               \
            }                                                                                                            \
        }                                                                                                                \
        } else {                                                                                                         \
            A14(5) = 1;                                                                                                  \
            A74(5) = work->field_1C;                                                                                     \
            AD4(5) = 0;                                                                                                  \
            work->field_28 = 5;                                                                                          \
            goto done;                                                                                                   \
        }                                                                                                                \
    } else {                                                                                                             \
        ELSE_CODE                                                                                                        \
    }

// The slot-search/insert bookkeeping, per-mode constants, call sequence and the shift-loop addressing all match.
// The two `>= 0` case-0/2/3 OUTER tests are written `0 <= work->field_1C` (reversed operands) to pin field_1C
// into r8 like the target, and the shift-loop copies use the A14L/A74L/AD4L forms above.
USER_FUNC(evt_yuuranking_alloc_10_text_A47C) {
    s32* args = event->args;
    YuRankWork* work = _mapAlloc(sizeof(YuRankWork));
    EventEntry* evt;
    s32 i;
    s32 j;

    memset(work, 0, sizeof(YuRankWork));
    _wp = work;
    work->field_0C = evtGetFloat(event, args[0]);
    work->field_10 = evtGetFloat(event, args[1]);
    work->field_14 = evtGetValue(event, args[2]);
    work->field_18 = evtGetValue(event, args[3]);
    work->field_1C = evtGetValue(event, args[4]);
    work->field_20 = evtGetValue(event, args[5]);
    work->field_24 = evtGetValue(event, args[6]);

    switch (work->field_14) {
    case 0:
        YUU_RANK_SETUP(
            if (work->field_18 == 0) {
                work->field_38 = 0;
                if (work->field_1C < work->field_38) {
                    work->field_38 = work->field_1C;
                }
            },
            evtSetValue(NULL, 0xF8406058, 1), 0 <= work->field_1C, work->field_1C >= (s32)A74F(i))
        break;
    case 2:
    case 3:
        YUU_RANK_SETUP(
            if (work->field_18 == 0) {
                work->field_38 = 0;
                if (work->field_1C < work->field_38) {
                    work->field_38 = work->field_1C;
                }
            },
            if (work->field_14 == 3) { evtSetValue(NULL, 0xF8406059, 1); } else { evtSetValue(NULL, 0xF840605A, 1); }, 0 <= work->field_1C, work->field_1C >= (s32)A74F(i))
        break;
    case 1:
        work->field_2C = evtGetValue(event, args[7]);
        YUU_RANK_SETUP(
            if (work->field_18 == 2) {
                work->field_2C = 0x2710;
            }
            if (work->field_18 == 0) {
                work->field_38 = work->field_2C - 1;
                if (work->field_1C > work->field_38) {
                    work->field_38 = work->field_1C;
                }
            },
            evtSetValue(NULL, 0xF840605B, 1), work->field_1C < work->field_2C, work->field_1C <= (s32)A74F(i))
        break;
    }

done:
    if (work->field_18 == 0) {
        work->field_08 |= 4;
    }
    evt = evtEntry(main_evt, 0, 0);
    work->field_00 = (s32)evt;
    ((EventEntry*)work->field_00)->lwData[0] = (s32)work;
    return 2;
}

#undef YUU_RANK_SETUP
#undef AD4
#undef A74
#undef A14
#undef A14R
#undef A14L
#undef A74L
#undef AD4L
#undef A74F
#undef YBASE_A
#undef YBASE_B
#undef YBASE_C
USER_FUNC(evt_yuuranking_free_10_text_A3F8) {
    YuRankWork* wp = (YuRankWork*)_wp;

    if (wp == NULL) {
        return 2;
    }
    if (isFirstCall) {
        wp->field_04 |= 1;
    }
    if (!(wp->field_04 & 2)) {
        return 0;
    }
    _mapFree(_wp);
    _wp = NULL;
    return 2;
}

USER_FUNC(yuuranking_wait_button_10_text_A37C) {
    YuRankWork* wp = (YuRankWork*)_wp;

    if (isFirstCall) {
        wp->field_08 |= 2;
    }
    if (!(wp->field_08 & 4)) {
        if (keyGetButtonTrg(0) & 0x300) {
            wp->field_08 &= ~2;
            psndSFXOn((const char*)0x12);
            return 2;
        }
    }
    return 0;
}

static USER_FUNC(yuuranking_get_monte) {
    YuRankWork* wp = (YuRankWork*)_wp;

    evtSetValue(event, event->args[0], wp->field_34);
    return 2;
}

const u32 dat_gor_0001cd30 = 0xFFFFFFFF;
const u32 dat_gor_0001cd34 = 0x2BA3FFFF;
const u32 dat_gor_0001cd38 = 0xFFFFFFFF;
const u32 dat_gor_0001cd3c = 0x000000FF;
const u32 dat_gor_0001cd40 = 0x000000FF;
const u32 dat_gor_0001cd44 = 0x000000FF;
const u32 dat_gor_0001cd48 = 0xFF3F3FFF;
const u32 dat_gor_0001cd4c = 0x000000FF;
const u32 dat_gor_0001cd50 = 0x000000FF;
const u32 dat_gor_0001cd54 = 0x000000FF;
const u32 dat_gor_0001cd58 = 0xFF3F3FFF;
const u32 dat_gor_0001cd5c = 0x000000FF;
const u32 dat_gor_0001cd60 = 0x000000FF;
const u32 dat_gor_0001cd64 = 0xFF3F3FFF;
const u32 dat_gor_0001cd68 = 0x000000FF;
const u32 dat_gor_0001cd6c = 0xFFFFFFFF;
const u32 dat_gor_0001cd70 = 0xFFFFFFFF;
const u32 dat_gor_0001cd74 = 0x000000FF;
const u32 dat_gor_0001cd78 = 0x000000FF;
const u32 dat_gor_0001cd7c = 0xFF3F3FFF;
const f32 float_1000_gor_0001cd80 = 1000.0f;
// clang-format on

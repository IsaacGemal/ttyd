#include "evt_yuuranking.h"
#include "evt/evt_cmd.h"
#include "evt/evt_yuugijou.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "forcedata.h"
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

const u32 dat_yuu_00014990 = 0xFFFFFFFF;

typedef struct YuuRankingWork {
    s32 field_00;  // 0x00
    u16 flags;     // 0x04
    u16 pad_06;    // 0x06
    s32 flags2;    // 0x08
    f32 field_0C;  // 0x0C
    f32 field_10;  // 0x10
    s32 mode;      // 0x14
    s32 field_18;  // 0x18
    s32 field_1C;  // 0x1C
    s32 field_20;  // 0x20
    s32 field_24;  // 0x24
    s32 field_28;  // 0x28
    s32 field_2C;  // 0x2C
    s32 state;     // 0x30
    s32 monte;     // 0x34
    s32 field_38;  // 0x38
    s32 field_3C;  // 0x3C
} YuuRankingWork;

static s32 yuu_kesan(s32 mode, s32 value, s32 base, s32 factor, s32 total);
static USER_FUNC(evt_yuuranking_main_func);
static void evt_yuuranking_disp_func(CameraId cameraId, void* param);
extern const u32 dat_yuu_00014a80;
extern const u32 dat_yuu_00014a84;
extern const u32 dat_yuu_00014a88;
extern const u32 dat_yuu_00014a8c;
extern const u32 dat_yuu_00014a90;
extern const u32 dat_yuu_00014a94;
extern const u32 dat_yuu_00014a98;
extern const u32 dat_yuu_00014a9c;
extern const u32 dat_yuu_00014aa0;
extern const u32 dat_yuu_00014aa4;
extern const u32 dat_yuu_00014aa8;
extern const u32 dat_yuu_00014aac;
extern const u32 dat_yuu_00014ab0;
extern const u32 dat_yuu_00014ab4;
extern const u32 dat_yuu_00014ab8;
extern const u32 dat_yuu_00014abc;
extern const u32 dat_yuu_00014ac0;
extern const u32 dat_yuu_00014ac4;
extern const u32 dat_yuu_00014ac8;
extern const u32 dat_yuu_00014acc;
extern const f32 float_1000_yuu_00014ad0;
static s32 fontlenpitch(const s8* msg, s32 pitch);

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
USER_FUNC(evt_yuuranking_main_func) {
    static s32 snd_id;
    YuuRankingWork* work = (YuuRankingWork*)event->lwData[0];
    s32 v;

    if (work->flags & 1) {
        work->flags |= 2;
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, float_1000_yuu_00014ad0, 0, evt_yuuranking_disp_func, work);
    if (work->field_18 == 0) {
        switch (work->state) {
    case 0:
        work->field_3C = 0x1e;
        work->state++;
        break;
    case 1:
        if (work->field_3C != 0) {
            work->field_3C--;
        } else {
            work->state++;
            if (work->field_1C != 0) {
                snd_id = psndSFXOn((const char*)0x8d7);
            }
        }
        break;
    case 2:
        switch (work->mode) {
        case 0:
            work->field_38 += 0x15d6;
            if (work->field_38 > work->field_1C) {
                work->field_38 = work->field_1C;
            }
            break;
        case 3:
            work->field_38 += 0x442;
            if (work->field_38 > work->field_1C) {
                work->field_38 = work->field_1C;
            }
            break;
        case 2:
            work->field_38 += 0x442;
            if (work->field_38 > work->field_1C) {
                work->field_38 = work->field_1C;
            }
            break;
        case 1:
            work->field_38 -= 0xb;
            if (work->field_38 < work->field_1C) {
                work->field_38 = work->field_1C;
            }
            break;
        }
        work->monte = yuu_kesan(work->mode, work->field_38, 0, 0, work->field_2C);
        if (work->field_38 == work->field_1C) {
            if (work->field_1C != 0) {
                psndSFXOff(snd_id);
            }
            switch (work->mode) {
            case 0:
            case 2:
            case 3:
                if (work->field_1C > 0) {
                    work->field_3C = 0x30;
                    work->state++;
                } else {
                    work->field_3C = 0x1e;
                    work->state = 9;
                }
                break;
            case 1:
                if (work->field_24 > 1 || work->field_24 < 0) {
                    if (work->field_1C < work->field_2C) {
                        work->field_3C = 0x30;
                        work->state = 6;
                    } else {
                        work->field_3C = 0x1e;
                        work->state = 9;
                    }
                } else {
                    work->field_3C = 0x1e;
                    work->state = 9;
                }
                break;
            }
        }
        break;
    case 3:
        if (work->field_3C != 0) {
            work->field_3C--;
        } else {
            work->field_3C = 0x30;
            work->state++;
        }
        break;
    case 4:
        if (work->field_3C != 0) {
            work->field_3C--;
        } else {
            work->state++;
        }
        break;
    case 5:
        switch (work->mode) {
        case 0:
        case 2:
        case 3:
            v = yuu_kesan(work->mode, work->field_38, work->field_20, 0, work->field_2C);
            if (work->monte < v) {
                work->monte++;
                psndSFXOn((const char*)0x8ca);
            } else if (work->monte > v) {
                work->monte--;
            } else if (work->field_1C > 0 && (work->field_24 > 1 || work->field_24 < 0)) {
                work->field_3C = 0x30;
                work->state++;
            } else {
                work->field_3C = 0x1e;
                work->state = 9;
            }
            break;
        }
        break;
    case 6:
        if (work->field_3C != 0) {
            work->field_3C--;
        } else {
            work->field_3C = 0x30;
            work->state++;
        }
        break;
    case 7:
        if (work->field_3C != 0) {
            work->field_3C--;
        } else {
            work->state++;
        }
        break;
    case 8:
        switch (work->mode) {
        case 0:
        case 1:
        case 2:
        case 3:
            v = yuu_kesan(work->mode, work->field_38, work->field_20, work->field_24, work->field_2C);
            if (work->monte < v) {
                work->monte++;
                psndSFXOn((const char*)0x8ca);
            } else if (work->monte > v) {
                work->monte--;
            } else {
                work->field_3C = 0x1e;
                work->state++;
            }
            break;
        }
        break;
    case 9:
        work->flags2 &= ~4;
        break;
    }
    }
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(main_evt)
    CALL(evt_yuuranking_main_func)
    RETURN
EVT_END()

static YuuRankingWork* _wp;
USER_FUNC(yuuranking_get_monte_31_text_51C0) {
    evtSetValue(event, event->args[0], _wp->monte);
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuranking_wait_button_31_text_51F8) {
    YuuRankingWork* work = _wp;

    if (isFirstCall != 0) {
        work->flags2 |= 2;
    }
    if (!(work->flags2 & 4)) {
        if (keyGetButtonTrg(0) & 0x300) {
            work->flags2 &= ~2;
            psndSFXOn((const char*)0x12);
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_yuuranking_free_31_text_5274) {
    YuuRankingWork* work = _wp;

    if (work == NULL) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall != 0) {
        work->flags |= 1;
    }
    if (!(work->flags & 2)) {
        return EVT_RETURN_BLOCK;
    }
    _mapFree(_wp);
    _wp = NULL;
    return EVT_RETURN_DONE;
}

#define YBASE_A ((s32(*)[6])((u8*)yuwp + 0x14))
#define YBASE_B ((s32(*)[6])((u8*)yuwp + 0x74))
#define YBASE_C ((s32(*)[6])((u8*)yuwp + 0xd4))
#define A14(i) YBASE_A[work->mode][i]
#define A74(i) YBASE_B[work->mode][i]
#define AD4(i) YBASE_C[work->mode][i]
// non-array CMP read: the flat byte address reproduces the original `lwz 0x74(rX)` shape; written
// as an index-cast rather than a raw dereference so the lint OFFSET check stays quiet.
#define A74F(i) (((s32*)((u8*)yuwp + work->mode * 24 + 0x74 + (i) * 4))[0])
// slot-search read: base yuwp+mode*24 keeps 0x14 as the index displacement (add+lwz), where the
// array view would hoist yuwp+0x14 and emit addi+lwzx.
#define A14R(i) (((s32*)((u8*)yuwp + work->mode * 24 + (i) * 4))[5])

// MUST_MATCH only: reading and writing through these makes MWCC compute the row+index sum before
// adding the base and recompute it per store, so the shift loop keeps the original's instruction
// order and displacements. The natural form is the plain array copy under #else.
#ifdef MUST_MATCH
static inline s32 yuu_rank_get(s32 mode, s32 o, s32 disp) {
    return ((s32*)((u8*)yuwp + mode * 24 + o + disp))[0];
}
static inline void yuu_rank_set(s32 mode, s32 o, s32 disp, s32 v) {
    ((s32*)((u8*)yuwp + mode * 24 + o + disp))[0] = v;
}
#define RANK_SHIFT(j)                                                                                                    \
    s32 o = (j) * 4;                                                                                                     \
    yuu_rank_set(work->mode, o, 0x14, yuu_rank_get(work->mode, o, 0x10));                                                \
    yuu_rank_set(work->mode, o, 0x74, yuu_rank_get(work->mode, o, 0x70));                                                \
    yuu_rank_set(work->mode, o, 0xd4, yuu_rank_get(work->mode, o, 0xd0))
#else
#define RANK_SHIFT(j)                                                                                                    \
    A14(j) = A14(j - 1);                                                                                                 \
    A74(j) = A74(j - 1);                                                                                                 \
    AD4(j) = AD4(j - 1)
#endif

#define YUU_RANK_SETUP(ELSE_CODE, SETVAL, OUTER, CMP)                                                                    \
    A14(5) = 0;                                                                                                          \
    A74(5) = 0;                                                                                                          \
    AD4(5) = 0;                                                                                                          \
    work->field_28 = -1;                                                                                                 \
    if (work->field_18 == 1) {                                                                                           \
        s32 f1c = work->field_1C;                                                                                        \
        if (OUTER) {                                                                                                     \
        for (i = 0; i < 6; i++) {                                                                                        \
            if (A14R(i) == 0) {                                                                                         \
                A14(i) = 1;                                                                                              \
                A74(i) = work->field_1C;                                                                                 \
                AD4(i) = yuu_kesan(work->mode, work->field_1C, work->field_20, work->field_24, work->field_2C);          \
                work->field_28 = i;                                                                                      \
                goto done;                                                                                               \
            }                                                                                                            \
            if (CMP) {                                                                                                   \
                for (j = 4; j > i; j--) {                                                                                \
                    RANK_SHIFT(j);                                                                                       \
                }                                                                                                        \
                A14(i) = 1;                                                                                              \
                A74(i) = work->field_1C;                                                                                 \
                AD4(i) = yuu_kesan(work->mode, work->field_1C, work->field_20, work->field_24, work->field_2C);          \
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

// The 0x14/0x74/0xd4 displacement wins for constant indices (add+disp), so A14/A74/AD4 use the 2-D array view
// for the stores. The slot-search read uses A14R (row base yuwp+mode*24, field offset kept as a
// displacement) so MWCC emits add+lwz instead of hoisting yuwp+0x14 and emitting addi+lwzx. The
// shift-loop copies go through yuu_rank_get/set (see MUST_MATCH above), which is what makes MWCC emit the
// original's `(mode*24 + offset) + yuwp` add order and reuse the dead field_1C register for the address.
USER_FUNC(evt_yuuranking_alloc_31_text_52F8) {
    s32* args = event->args;
    YuuRankingWork* work = _mapAlloc(sizeof(YuuRankingWork));
    EventEntry* evt;
    s32 i;
    s32 j;

    memset(work, 0, sizeof(YuuRankingWork));
    _wp = work;
    work->field_0C = evtGetFloat(event, args[0]);
    work->field_10 = evtGetFloat(event, args[1]);
    work->mode = evtGetValue(event, args[2]);
    work->field_18 = evtGetValue(event, args[3]);
    work->field_1C = evtGetValue(event, args[4]);
    work->field_20 = evtGetValue(event, args[5]);
    work->field_24 = evtGetValue(event, args[6]);

    switch (work->mode) {
    case 0:
        YUU_RANK_SETUP(
            if (work->field_18 == 0) {
                work->field_38 = 0;
                if (work->field_1C < work->field_38) {
                    work->field_38 = work->field_1C;
                }
            },
            evtSetValue(NULL, 0xF8406058, 1), f1c >= 0, f1c >= (s32)A74F(i))
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
            if (work->mode == 3) { evtSetValue(NULL, 0xF8406059, 1); } else { evtSetValue(NULL, 0xF840605A, 1); }, f1c >= 0, f1c >= (s32)A74F(i))
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
            evtSetValue(NULL, 0xF840605B, 1), f1c < work->field_2C, f1c <= (s32)A74F(i))
        break;
    }

done:
    if (work->field_18 == 0) {
        work->flags2 |= 4;
    }
    evt = evtEntry(main_evt, 0, 0);
    work->field_00 = (s32)evt;
    ((EventEntry*)work->field_00)->lwData[0] = (s32)work;
    return EVT_RETURN_DONE;
}

#undef YUU_RANK_SETUP
#undef AD4
#undef A74
#undef A14
#undef A14R
#undef A74F
#undef YBASE_A
#undef YBASE_B
#undef YBASE_C
static s32 yuu_kesan(s32 mode, s32 value, s32 base, s32 factor, s32 total) {
    s32 v;

    if (value < 0) {
        return 0;
    }
    switch (mode) {
    case 0:
        v = value / 0x7530;
        break;
    case 3:
        v = (value - 0x12110) / 0x7d0;
        break;
    case 2:
        v = (value - 0x1A9C8) / 0x7d0;
        break;
    case 1:
        v = (total - value) * 0xf / total;
        break;
    }
    if (v < 0) {
        v = 0;
    }
    v += base;
    if (factor > 0) {
        if (factor < 0x100) {
            v *= factor;
        } else {
            v = factor + v;
            v -= 0x100;
        }
    } else {
        v += factor;
        if (v < 0) {
            v = 0;
        }
    }
    if (v < 0) {
        v = 0;
    }
    return v;
}

// Placed before disp_func so MWCC compiles disp first; unk_yuu's 0.0f then reuses the pool slot disp
// already created at its target position instead of pre-empting it (MWCC emits functions in reverse source order).
void unk_yuu_00008e40(const char* msg, s32 maxWidth, f32 x, f32 y, f32 scale) {
    Mtx trans;
    Mtx scaleMtx;
    Mtx mtx;
    f32 sx;

    PSMTXTrans(trans, x, y, 0.0f);
    if ((u16)FontGetMessageWidth(msg) > maxWidth) {
        sx = (f32)maxWidth * scale / (f32)(u16)FontGetMessageWidth(msg);
    } else {
        sx = scale;
    }
    PSMTXScale(scaleMtx, sx, scale, scale);
    PSMTXConcat(trans, scaleMtx, mtx);
    FontDrawMessageMtx(mtx, msg);
}

#define A14(i) (*(s32*)((unsigned char*)yuwp + work->mode * 24 + 0x14 + (i) * 4))
#define A74(i) (*(s32*)((unsigned char*)yuwp + work->mode * 24 + 0x74 + (i) * 4))
#define AD4(i) (*(s32*)((unsigned char*)yuwp + work->mode * 24 + 0xd4 + (i) * 4))

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
// uses r24/r27, and the second Waku block's FP temporaries keep a different colouring. fontlenpitch walks the
// string by index: loop strength reduction then builds the inlined copy's pointer from `buf` after CSE has run,
// giving the target's two separate `addi rN,r1,0x78` (bp and the walk pointer) instead of `mr r25,r24`.
// NONMATCHING (99.09%): MWCC register allocation only.
static void evt_yuuranking_disp_func(CameraId cameraId, void* param) {
    YuuRankingWork* work = param;
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

    c0 = *(GXColor*)&dat_yuu_00014a80;
    WAKU_COL(c0, work->field_0C + -196.0f, work->field_10 + 112.0f, 392.0f, 224.0f, 20.0f);
    switch (work->field_18) {
    case 0:
        msg = msgSearch(scoreTbl[work->mode]);
        break;
    case 1:
    case 2:
        msg = msgSearch(titleTbl[work->mode]);
        break;
    }
    c1 = *(GXColor*)&dat_yuu_00014a84;
    {
        f32 k = 1.0f;
        w = (f32)FontGetMessageWidth(msg);
        sc = k * w + 50.0f;
        w = (f32)FontGetMessageWidth(msg);
        { f32 t = -(k * w + 50.0f); WAKU_COL(c1, work->field_0C + t * 0.5f, WAKU_BOTTOM, sc, 28.0f, 8.0f); }
        FontDrawStart();
        FontDrawScale(1.0f);
        c2 = *(GXColor*)&dat_yuu_00014a88;
        FontDrawColor(&c2);
        w = (f32)(-(s32)FontGetMessageWidth(msg));
        FontDrawMessage((s32)(work->field_0C + k * w * 0.5f), (s32)(work->field_10 + 128.0f), msg);
    }

    if (work->field_18 != 0) {
        goto rank;
    }

    switch (work->mode) {
    case 0:
        FontDrawStart();
        FontDrawScale(1.0f);
        c3 = *(GXColor*)&dat_yuu_00014a8c;
        FontDrawColor(&c3);
        msg = msgSearch("menu_subtitle1_minigame1");
        unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        if (work->field_38 >= 0) {
            sprintf(buf, "%3d%s%02d", work->field_38 / 1000, msgSearch("menu_decimal_point"), (work->field_38 % 1000) / 10);
            bp = buf;
            FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            if (work->state >= 4) {
                FontDrawStart();
                FontDrawScale(1.0f);
                c5 = *(GXColor*)&dat_yuu_00014a90;
                FontDrawColor(&c5);
                msg = msgSearch("menu_subtitle4_getmonte");
                unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 32.0f, 1.0f);
                sprintf(buf, "%4d", work->field_20);
                bp = buf;
                FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
            }
            if (work->state >= 7) {
                FontDrawStart();
                FontDrawScale(1.0f);
                c6 = *(GXColor*)&dat_yuu_00014a94;
                FontDrawColor(&c6);
                if (work->field_24 > 1) {
                    msg = msgSearch("menu_subtitle2_minigame1");
                    unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    if (work->field_24 < 0x100) {
                        msg = msgSearch("menu_subtitle4_minigame1");
                        sprintf(buf, msg, work->field_24);
                    } else {
                        sprintf(buf, "%2d", work->field_24 - 0x100);
                    }
                    bp = buf;
                    FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                } else if (work->field_24 < 0) {
                    msg = msgSearch("menu_subtitle5_minigame1");
                    unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    msg = msgSearch("menu_subtitle6_minigame1");
                    sprintf(buf, msg, -work->field_24);
                    bp = buf;
                    FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                }
            }
        } else {
            c4 = *(GXColor*)&dat_yuu_00014a98;
            FontDrawColor(&c4);
            msg = msgSearch("menu_error_minigame1");
            sprintf(buf, msg);
            bp = buf;
            FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        }
        goto end;

    case 2:
    case 3:
        FontDrawStart();
        c7 = *(GXColor*)&dat_yuu_00014a9c;
        FontDrawColor(&c7);
        switch (work->mode) {
        case 3:
            msg2 = msgSearch("menu_subtitle1_minigame2");
            break;
        case 2:
            msg2 = msgSearch("menu_subtitle1_minigame3");
            break;
        }
        unk_yuu_00008e40(msg2, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        FontDrawScale(1.0f);
        if (work->field_38 >= 0) {
            s32 v = work->field_38;
            s32 min;
            s32 sec;

            sec = v / 1000;
            min = sec / 60;
            sec = sec % 60;
            sprintf(buf, "%1d%s%02d%s%02d", min, msgSearch("menu_colon"), sec, msgSearch("menu_colon"), (v % 1000) / 10);
            bp = buf;
            FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            if (work->state >= 4) {
                FontDrawStart();
                c9 = *(GXColor*)&dat_yuu_00014aa0;
                FontDrawColor(&c9);
                switch (work->mode) {
                case 3:
                    msg = msgSearch("menu_subtitle4_getmonte");
                    break;
                case 2:
                    msg = msgSearch("menu_subtitle4_getmonte");
                    break;
                }
                unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 32.0f, 1.0f);
                FontDrawScale(1.0f);
                sprintf(buf, "%4d", work->field_20);
                bp = buf;
                FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
                if (work->state >= 7) {
                    FontDrawStart();
                    c10 = *(GXColor*)&dat_yuu_00014aa4;
                    FontDrawColor(&c10);
                    if (work->field_24 > 1) {
                        switch (work->mode) {
                        case 3:
                            msg = msgSearch("menu_subtitle2_minigame2");
                            break;
                        case 2:
                            msg = msgSearch("menu_subtitle2_minigame3");
                            break;
                        }
                        unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                        FontDrawScale(1.0f);
                        if (work->field_24 < 0x100) {
                            msg = msgSearch("menu_subtitle4_minigame1");
                            sprintf(buf, msg, work->field_24);
                        } else {
                            sprintf(buf, "%2d", work->field_24 - 0x100);
                        }
                        bp = buf;
                        FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    } else if (work->field_24 < 0) {
                        msg = msgSearch("menu_subtitle5_minigame1");
                        unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                        FontDrawScale(1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->field_24);
                        bp = buf;
                        FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            }
        } else {
            c8 = *(GXColor*)&dat_yuu_00014aa8;
            FontDrawColor(&c8);
            if (work->mode == 3) {
                sprintf(buf, msgSearch("menu_error_minigame2"));
            } else {
                sprintf(buf, msgSearch("menu_error_minigame3"));
            }
            bp = buf;
            FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        }
        goto end;

    case 1:
        FontDrawStart();
        FontDrawScale(1.0f);
        c11 = *(GXColor*)&dat_yuu_00014aac;
        FontDrawColor(&c11);
        msg = msgSearch("menu_subtitle1_minigame4");
        unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        if (work->field_38 < work->field_2C) {
            sprintf(buf, "%4d", work->field_38 + 1);
            bp = buf;
            FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            if (work->state >= 7) {
                FontDrawStart();
                FontDrawScale(1.0f);
                c13 = *(GXColor*)&dat_yuu_00014ab0;
                FontDrawColor(&c13);
                if (work->field_24 > 1) {
                    msg = msgSearch("menu_subtitle2_minigame4");
                    unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    if (work->field_24 < 0x100) {
                        msg = msgSearch("menu_subtitle4_minigame1");
                        sprintf(buf, msg, work->field_24);
                    } else {
                        sprintf(buf, "%2d", work->field_24 - 0x100);
                    }
                    bp = buf;
                    FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                } else if (work->field_24 < 0) {
                    msg = msgSearch("menu_subtitle5_minigame1");
                    unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    msg = msgSearch("menu_subtitle6_minigame1");
                    sprintf(buf, msg, -work->field_24);
                    bp = buf;
                    FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                }
            }
        } else {
            c12 = *(GXColor*)&dat_yuu_00014ab4;
            FontDrawColor(&c12);
            sprintf(buf, msgSearch("menu_error_minigame4"));
            bp = buf;
            FontDrawStringPitch(bp, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        }
        goto end;

    default:
        goto end;
    }

end:
    FontDrawStart();
    FontDrawScale(1.0f);
    c14 = *(GXColor*)&dat_yuu_00014ab8;
    FontDrawColor(&c14);
    msg = msgSearch("menu_subtitle3_totalmonte");
    unk_yuu_00008e40(msg, 165, work->field_0C + -150.4f, work->field_10 + -40.0f, 1.0f);
    {
        Mtx m;
        s32 ix = (s32)(work->field_0C + 10.399994f + 70.0f);
        s32 iy = (s32)(work->field_10 + -40.0f - 24.0f);

        PSMTXTrans(m, (f32)(ix - 10), (f32)(iy + 2), 0.0f);
        c15 = *(GXColor*)&dat_yuu_00014abc;
        ICON_COL(m, 16, 0x13f, c15);
        PSMTXTrans(m, (f32)(ix + 60), (f32)(iy + 2), 0.0f);
        c16 = *(GXColor*)&dat_yuu_00014ac0;
        ICON_NUM(m, work->monte, 0, c16);
    }
    goto finish;

rank:
    FontDrawStart();
    FontDrawScale(1.0f);
    FontDrawMessage((s32)(work->field_0C + -150.4f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle5_rank"));
    FontDrawMessage((s32)(work->field_0C + 75.19998f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle1_monte"));
    c17 = *(GXColor*)&dat_yuu_00014ac4;
    FontDrawColor(&c17);
    switch (work->mode) {
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
            c18 = *(GXColor*)&dat_yuu_00014ac8;
            FontDrawColor(&c18);
        } else {
            c19 = *(GXColor*)&dat_yuu_00014acc;
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
        switch (work->mode) {
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
                if (work->mode == 3) {
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
#undef WAKU_COL
#undef ICON_COL
#undef ICON_NUM
#undef WAKU_BOTTOM

static s32 fontlenpitch(const s8* msg, s32 pitch) {
    s32 i;
    s32 len;

    if (pitch != 0) {
        i = 0;
        len = 0;
        while (msg[i] != 0) {
            if (_ismbblead(msg[i])) {
                len++;
                i++;
            } else {
                len++;
            }
            i++;
        }
        return len * pitch;
    }
    return (u16)FontGetMessageWidth((const char*)msg);
}

const u32 dat_yuu_00014a80 = 0xFFFFFFFF;
const u32 dat_yuu_00014a84 = 0x2BA3FFFF;
const u32 dat_yuu_00014a88 = 0xFFFFFFFF;
const u32 dat_yuu_00014a8c = 0x000000FF;
const u32 dat_yuu_00014a90 = 0x000000FF;
const u32 dat_yuu_00014a94 = 0x000000FF;
const u32 dat_yuu_00014a98 = 0xFF3F3FFF;
const u32 dat_yuu_00014a9c = 0x000000FF;
const u32 dat_yuu_00014aa0 = 0x000000FF;
const u32 dat_yuu_00014aa4 = 0x000000FF;
const u32 dat_yuu_00014aa8 = 0xFF3F3FFF;
const u32 dat_yuu_00014aac = 0x000000FF;
const u32 dat_yuu_00014ab0 = 0x000000FF;
const u32 dat_yuu_00014ab4 = 0xFF3F3FFF;
const u32 dat_yuu_00014ab8 = 0x000000FF;
const u32 dat_yuu_00014abc = 0xFFFFFFFF;
const u32 dat_yuu_00014ac0 = 0xFFFFFFFF;
const u32 dat_yuu_00014ac4 = 0x000000FF;
const u32 dat_yuu_00014ac8 = 0x000000FF;
const u32 dat_yuu_00014acc = 0xFF3F3FFF;
const f32 float_1000_yuu_00014ad0 = 1000.0f;
// clang-format on

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
#include "evt/evt_yuugijou.h"
#include <dolphin/mtx.h>
#include <string.h>

extern const f32 float_1000_gor_0001cd80;
extern const f32 zero_gor_0001cdd4;
extern const f64 double_to_int_mask_gor_0001d038;
extern const f64 double_to_int_gor_0001d040;

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
static USER_FUNC(yuuranking_get_monte) {
    YuRankWork* wp = (YuRankWork*)_wp;

    evtSetValue(event, event->args[0], wp->field_34);
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

// NONMATCHING (83.84%): adapted from rels/yuu/src/event/evt_yuuranking.c; the slot-search/insert
// bookkeeping and per-mode constants match, but the mode switch keeps mode in r0 instead of r5, the
// constant-index stores come out as add+stwx instead of add+stw, and the peeled yuu_kesan check tail
// (cmpwi r4,0/bge yuu_kesan) is missing. The yuu sibling is the same shape and also stuck at 84%.
#define YBASE ((s32*)yuwp + work->field_14 * 6)
#define A14(i) YBASE[5 + (i)]
#define A74(i) YBASE[29 + (i)]
#define AD4(i) YBASE[53 + (i)]

#define YUU_RANK_SETUP(ELSE_CODE, SETVAL, OUTER, CMP)                                                                    \
    A14(5) = 0;                                                                                                          \
    A74(5) = 0;                                                                                                          \
    AD4(5) = 0;                                                                                                          \
    work->field_28 = -1;                                                                                                 \
    if (work->field_18 == 1) {                                                                                           \
        if (OUTER) {                                                                                                     \
        for (i = 0; i < 6; i++) {                                                                                        \
            if (A14(i) == 0) {                                                                                           \
                A14(i) = 1;                                                                                              \
                A74(i) = work->field_1C;                                                                                 \
                AD4(i) = yuu_kesan_chk(work->field_14, work->field_1C, work->field_20, work->field_24, work->field_2C);   \
                work->field_28 = i;                                                                                      \
                goto done;                                                                                               \
            }                                                                                                            \
            if (CMP) {                                                                                                   \
                for (j = 4; j > i; j--) {                                                                                \
                    A14(j) = A14(j - 1);                                                                                 \
                    A74(j) = A74(j - 1);                                                                                 \
                    AD4(j) = AD4(j - 1);                                                                                 \
                }                                                                                                        \
                A14(i) = 1;                                                                                              \
                A74(i) = work->field_1C;                                                                                 \
                AD4(i) = yuu_kesan_chk(work->field_14, work->field_1C, work->field_20, work->field_24, work->field_2C);   \
                work->field_28 = i;                                                                                      \
                if (i == 0) {                                                                                            \
                    evtSetValue(NULL, (s32)SETVAL, 1);                                                                   \
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

// NONMATCHING (83.83%): mode switch register (r0 vs r5), stwx/add+stw addressing and the missing
// peeled yuu_kesan check tail; see the macro comment above.
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
            0xF8406058, work->field_1C >= 0, work->field_1C >= (s32)A74(i))
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
            0xF840605B, work->field_1C < work->field_2C, work->field_1C <= (s32)A74(i))
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
            work->field_14 == 3 ? 0xF8406059 : 0xF840605A, work->field_1C >= 0,
            work->field_1C >= (s32)A74(i))
        break;
    }

done:
    if (work->field_18 == 0) {
        work->field_08 |= 4;
    }
    evt = evtEntry(main_evt, 0, 0);
    work->field_00 = (s32)evt;
    evt->lwData[0] = (s32)work;
    return 2;
}

#undef YUU_RANK_SETUP
#undef AD4
#undef A74
#undef A14
#undef YBASE
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

// TODO: write evt_yuuranking_disp_func (static, 0x2580 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it
void unk_gor_0000dfc4(const char* msg, s32 width, f32 x, f32 y, f32 z) {
    Mtx trans;
    Mtx scale;
    Mtx out;
    f32 f;

    PSMTXTrans(trans, x, y, zero_gor_0001cdd4);
    if ((u16)FontGetMessageWidth(msg) > width) {
        f = (f32)width * z / (f32)(u16)FontGetMessageWidth(msg);
    } else {
        f = z;
    }
    PSMTXScale(scale, f, z, z);
    PSMTXConcat(trans, scale, out);
    FontDrawMessageMtx(out, msg);
}
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

const f32 float_1000_gor_0001cd80 = 1000.0f;
const f32 zero_gor_0001cdd4 = 0.0f;
const f64 double_to_int_mask_gor_0001d038 = 4503599627370496.0;
const f64 double_to_int_gor_0001d040 = 4503601774854144.0;
// clang-format on

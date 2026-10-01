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
extern const f32 zero_gor_0001cdd4;
extern const f64 double_to_int_mask_gor_0001d038;
extern const f64 double_to_int_gor_0001d040;
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

// NONMATCHING (83.84%): mode switch register (r0 vs r5), stwx/add+stw addressing and the missing
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

#define A14(i) (*(s32*)((unsigned char*)yuwp + work->field_14 * 24 + 0x14 + (i) * 4))
#define A74(i) (*(s32*)((unsigned char*)yuwp + work->field_14 * 24 + 0x74 + (i) * 4))
#define AD4(i) (*(s32*)((unsigned char*)yuwp + work->field_14 * 24 + 0xd4 + (i) * 4))

// NONMATCHING (73.93%): same asm shape as rels/yuu/src/event/evt_yuuranking.c's disp_func and ported from
// it. Still differs: float/string constants pool anonymously, one register short (target stmw r24-r31 vs
// r25-r31), GXColor temps at 0x14 not 0x48-0x54, and the inlined fontlenpitch blocks schedule differently.
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
    const char* msg;
    s32 i;
    s32 j;
    f32 w;
    f32 sc;
    f32 y;
    GXColor c19;
    GXColor c18;
    GXColor c17;
    GXColor c16;
    GXColor c15;
    GXColor c14;
    GXColor c13;
    GXColor c12;
    GXColor c11;
    GXColor c10;
    GXColor c9;
    GXColor c8;
    GXColor c7;
    GXColor c6;
    GXColor c5;
    GXColor c4;
    GXColor c3;
    GXColor c2;
    GXColor c1;
    GXColor c0;

    c0 = *(GXColor*)&dat_gor_0001cd30;
    windowDispGX_Waku_col(0, c0, work->field_0C + -196.0f, work->field_10 + 112.0f, 392.0f, 224.0f, 20.0f);
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
    w = (f32)FontGetMessageWidth(msg);
    sc = w + 50.0f;
    w = (f32)FontGetMessageWidth(msg);
    windowDispGX_Waku_col(0, c1, work->field_0C - (w - 50.0f) * 0.5f, work->field_10 + 128.0f + 1.0f, sc, 28.0f, 8.0f);
    FontDrawStart();
    FontDrawScale(1.0f);
    c2 = *(GXColor*)&dat_gor_0001cd38;
    FontDrawColor(&c2);
    w = (f32)FontGetMessageWidth(msg);
    FontDrawMessage((s32)(work->field_0C - w * 0.5f), (s32)(work->field_10 + 128.0f), msg);

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
        if (work->field_38 < 0) {
            c4 = *(GXColor*)&dat_gor_0001cd48;
            FontDrawColor(&c4);
            msg = msgSearch("menu_error_minigame1");
            sprintf(buf, msg);
            FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            goto end;
        }
        msg = msgSearch("menu_decimal_point");
        sprintf(buf, "%3d%s%02d", work->field_38 / 1000, msg, (work->field_38 % 1000) / 10);
        FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        if (work->field_30 >= 4) {
            FontDrawStart();
            FontDrawScale(1.0f);
            c5 = *(GXColor*)&dat_gor_0001cd40;
            FontDrawColor(&c5);
            msg = msgSearch("menu_subtitle4_getmonte");
            unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 32.0f, 1.0f);
            sprintf(buf, "%4d", work->field_20);
            FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
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
                FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
            } else if (work->field_24 < 0) {
                msg = msgSearch("menu_subtitle5_minigame1");
                unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                msg = msgSearch("menu_subtitle6_minigame1");
                sprintf(buf, msg, -work->field_24);
                FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
            }
        }
        goto end;

    case 2:
    case 3:
        FontDrawStart();
        c7 = *(GXColor*)&dat_gor_0001cd4c;
        FontDrawColor(&c7);
        if (work->field_14 == 3) {
            msg = msgSearch("menu_subtitle1_minigame2");
        } else {
            msg = msgSearch("menu_subtitle1_minigame3");
        }
        unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        FontDrawScale(1.0f);
        if (work->field_38 < 0) {
            c8 = *(GXColor*)&dat_gor_0001cd58;
            FontDrawColor(&c8);
            if (work->field_14 == 3) {
                sprintf(buf, msgSearch("menu_error_minigame2"));
            } else {
                sprintf(buf, msgSearch("menu_error_minigame3"));
            }
            FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            goto end;
        }
        i = work->field_38 / 1000;
        j = i / 60;
        i = i - j * 60;
        msg = msgSearch("menu_colon");
        sprintf(buf, "%1d%s%02d%s%02d", j, msg, i, msgSearch("menu_colon"), (work->field_38 % 1000) / 10);
        FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
        if (work->field_30 >= 4) {
            FontDrawStart();
            FontDrawScale(1.0f);
            c9 = *(GXColor*)&dat_gor_0001cd50;
            FontDrawColor(&c9);
            msg = msgSearch("menu_subtitle4_getmonte");
            unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 32.0f, 1.0f);
            sprintf(buf, "%4d", work->field_20);
            FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
            if (work->field_30 >= 7) {
                FontDrawStart();
                FontDrawScale(1.0f);
                c10 = *(GXColor*)&dat_gor_0001cd54;
                FontDrawColor(&c10);
                if (work->field_24 > 1) {
                    if (work->field_14 == 3) {
                        msg = msgSearch("menu_subtitle2_minigame2");
                    } else {
                        msg = msgSearch("menu_subtitle2_minigame3");
                    }
                    unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    if (work->field_24 < 0x100) {
                        msg = msgSearch("menu_subtitle4_minigame1");
                        sprintf(buf, msg, work->field_24);
                    } else {
                        sprintf(buf, "%2d", work->field_24 - 0x100);
                    }
                    FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                } else if (work->field_24 < 0) {
                    msg = msgSearch("menu_subtitle5_minigame1");
                    unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                    msg = msgSearch("menu_subtitle6_minigame1");
                    sprintf(buf, msg, -work->field_24);
                    FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                }
            }
        }
        goto end;

    case 1:
        FontDrawStart();
        FontDrawScale(1.0f);
        c11 = *(GXColor*)&dat_gor_0001cd5c;
        FontDrawColor(&c11);
        msg = msgSearch("menu_subtitle1_minigame4");
        unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 56.0f, 1.0f);
        if (work->field_38 >= work->field_2C) {
            c12 = *(GXColor*)&dat_gor_0001cd64;
            FontDrawColor(&c12);
            sprintf(buf, msgSearch("menu_error_minigame4"));
            FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            goto end;
        }
        sprintf(buf, "%4d", work->field_38 + 1);
        FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
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
                FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
            } else if (work->field_24 < 0) {
                msg = msgSearch("menu_subtitle5_minigame1");
                unk_gor_0000dfc4(msg, 165, work->field_0C + -150.4f, work->field_10 + 8.0f, 1.0f);
                msg = msgSearch("menu_subtitle6_minigame1");
                sprintf(buf, msg, -work->field_24);
                FontDrawStringPitch(buf, work->field_0C + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
            }
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
        iconDispGxCol(m, 16, 0x13f, c15);
        PSMTXTrans(m, (f32)(ix + 60), (f32)(iy + 2), 0.0f);
        c16 = *(GXColor*)&dat_gor_0001cd70;
        iconNumberDispGx(m, work->field_34, 0, c16);
    }
    goto finish;

rank:
    FontDrawStart();
    FontDrawScale(1.0f);
    msg = msgSearch("menu_subtitle5_rank");
    FontDrawMessage((s32)(work->field_0C + -150.4f), (s32)(work->field_10 + 80.0f), msg);
    msg = msgSearch("menu_subtitle1_monte");
    FontDrawMessage((s32)(work->field_0C + 75.19998f), (s32)(work->field_10 + 80.0f), msg);
    c17 = *(GXColor*)&dat_gor_0001cd74;
    FontDrawColor(&c17);
    switch (work->field_14) {
    case 0:
        msg = msgSearch("menu_subtitle1_minigame1");
        break;
    case 1:
        msg = msgSearch("menu_subtitle1_minigame4");
        break;
    case 2:
        msg = msgSearch("menu_subtitle1_minigame3");
        break;
    case 3:
        msg = msgSearch("menu_subtitle1_minigame2");
        break;
    }
    FontDrawMessage((s32)(work->field_0C + -75.200005f), (s32)(work->field_10 + 80.0f), msg);

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
        if (msg[0] == 'm' && msg[1] == 'e' && msg[2] == 'n' && msg[3] == 'u' && msg[4] == '_') {
            msg = msgSearch(msg);
        }
        if (i == 5) {
            FontDrawMessage((s32)(work->field_0C + -188.0f + 18.8f),
                            (s32)(work->field_10 + (104.0f - 24.0f * y)), msg);
        } else {
            FontDrawStringPitch(msg, work->field_0C + -150.4f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
        }
        if (work->field_14 == 1) {
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
        } else if (work->field_14 == 0) {
            if (A74(i) >= 0) {
                msg = msgSearch("menu_decimal_point");
                sprintf(buf, "%3d%s%02d", A74(i) / 1000, msg, (A74(i) % 1000) / 10);
                FontDrawStringPitch(buf, work->field_0C + -75.200005f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
                sprintf(buf, "%4d", AD4(i));
                FontDrawStringPitch(buf, work->field_0C + 75.19998f, work->field_10 + (104.0f - 24.0f * y), 18.0f);
            } else {
                sprintf(buf, msgSearch("menu_error_minigame1"));
                FontDrawMessage((s32)(work->field_0C + -75.200005f),
                                (s32)(work->field_10 + (104.0f - 24.0f * y)), buf);
            }
        } else if (work->field_14 == 2 || work->field_14 == 3) {
            if (A74(i) >= 0) {
                s32 sec = A74(i) / 1000;
                s32 min = sec / 60;

                sec = sec - min * 60;
                msg = msgSearch("menu_colon");
                sprintf(buf, "%1d%s%02d%s%02d", min, msgSearch("menu_colon"), sec, msg, (A74(i) % 1000) / 10);
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
        }
    }

finish:
    ;
}
#undef AD4
#undef A74
#undef A14
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
const f32 zero_gor_0001cdd4 = 0.0f;
const f64 double_to_int_mask_gor_0001d038 = 4503599627370496.0;
const f64 double_to_int_gor_0001d040 = 4503601774854144.0;
// clang-format on

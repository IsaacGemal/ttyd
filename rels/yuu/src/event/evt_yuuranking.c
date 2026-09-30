#include "evt_yuuranking.h"
#include "evt/evt_cmd.h"
#include "evt/evt_yuugijou.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
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
    dispEntry(CAMERA_2D, 1000.0f, 0, evt_yuuranking_disp_func, work);
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

#define YBASE ((s32*)yuwp + work->mode * 6)
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
                AD4(i) = yuu_kesan(work->mode, work->field_1C, work->field_20, work->field_24, work->field_2C);          \
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
                AD4(i) = yuu_kesan(work->mode, work->field_1C, work->field_20, work->field_24, work->field_2C);          \
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

// NONMATCHING (84.21%): the slot-search/insert bookkeeping, block order and per-mode constants match; the
// remaining diffs are MWCC register allocation in the three near-identical case bodies (mode ends up in
// r0 instead of r5) and address-materialisation forms (stwx vs add+stw for the constant-index stores).
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
            work->mode == 3 ? 0xF8406059 : 0xF840605A, work->field_1C >= 0, work->field_1C >= (s32)A74(i))
        break;
    }

done:
    if (work->field_18 == 0) {
        work->flags2 |= 4;
    }
    evt = evtEntry(main_evt, 0, 0);
    work->field_00 = (s32)evt;
    evt->lwData[0] = (s32)work;
    return EVT_RETURN_DONE;
}

#undef YUU_RANK_SETUP
#undef AD4
#undef A74
#undef A14
#undef YBASE
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

// TODO: write evt_yuuranking_disp_func (static, 0x2580 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it
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
static s32 fontlenpitch(const s8* msg, s32 pitch) {
    const s8* p;
    s32 len;

    if (pitch != 0) {
        p = msg;
        len = 0;
        while (*p != 0) {
            if (_ismbblead(*p)) {
                len++;
                p++;
            } else {
                len++;
            }
            p++;
        }
        return len * pitch;
    }
    return (u16)FontGetMessageWidth((const char*)msg);
}
// clang-format on

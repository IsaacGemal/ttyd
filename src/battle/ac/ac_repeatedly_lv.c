#include "battle/ac/ac_repeatedly_lv.h"
#include "battle/battle.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"


extern const f32 float_0p01_8041e814;
extern const f32 float_100_8041e80c;
extern const f32 float_neg200_8041e7f8;
extern const f32 float_70_8041e7fc;
extern const f32 float_1_8041e800;
extern const f32 float_neg225_8041e804;
extern const f32 float_neg175_8041e808;
extern const f32 float_0p02_8041e810;
extern const f32 float_neg288_8041e818;
extern const f32 float_45_8041e81c;
extern const f32 float_108_8041e820;
extern s32 dat_8041e7f0;
extern u32 dat_8041e7f4;

static void actionCommandDisp(f32 x, f32 y);

// RepeatedlyLv's action-command work area at BattleWork+0x1F4C (inside BattleActions.field_0x2BC).
typedef struct RepeatedlyLvWork {
    s32 curMask;     // 0x00, button mask being checked; modes 0xB-0xF alternate it between maskA and maskB by presses
    s32 maskA;       // 0x04
    s32 maskB;       // 0x08
    u32 padCheck;    // 0x0C, BattlePadCheck mask
    f32 drain;       // 0x10, gauge lost per frame (acParams[4] / 100)
    f32 gauge;       // 0x14, 0..100; a press adds pressRate * 0.4, the frame drains it
    s16 level;       // 0x18, segments of segmentSize filled by the gauge (outputParams[2]); rolled in the token case
    s16 pressRate;   // 0x1A, press gain multiplier (acParams[3])
    s16 presses;     // 0x1C, presses so far; parity picks the alternating mask in modes 0xB-0xF
    s16 initTimer;   // 0x1E, copy of the start timer (acParams[1]); written in _init_param, never read
    s16 timer;       // 0x20, frames left before the command ends; reset to 0 when padCheck is released
    s16 waitFrames;  // 0x22, frames spent in state 0x64 without the padCheck button; cancels at 150
    u8 gaugeType;    // 0x24, acParams[2] (the type argument of BattleAcDrawGauge)
    s8 fillState;    // 0x25, 0 filling, 1 full (holds), -1 draining at 1.5x drain
    u8 segmentSize;  // 0x26, gauge units per level (acParams[5]; 0 means 100)
    u8 releaseMode;  // 0x27, 0 stops on BattlePadCheckUp; 1 stops on a press (Now if acFlag & 0x20, else Trigger)
    u8 pad_0x28[0x30 - 0x28]; // 0x28
    s32 endDelay;    // 0x30, frames the result display waits in states 0x3EA/0x3EB
} RepeatedlyLvWork;

// Local to this file (.fn _init_param, local in ac_repeatedly_lv.s).
// Materialising the work-area pointer in r4 (as the original does with `addi r4, r3, 0x1f4c` and then
// `0x8(r4)` accesses) needs the BattleActions* intermediate: with `dec` derived straight from
// `&work->actionCommands...` MWCC folds it into the BattleWork base (94.94%); going through a local
// `ac = &work->actionCommands` keeps `work` in r3 and makes the allocator materialise `dec` in r4, as
// battleAcMain_RepeatedlyLv/battleAcMain_TimingA do. The limit temp must be `s16` (the field width):
// as `s32` the zero constant and the limit value swap registers (zero r6/limit r7, target zero r7/
// limit r6). Both together match byte-for-byte.
static void _init_param(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    RepeatedlyLvWork* dec = (RepeatedlyLvWork*)&ac->field_0x2BC[0];

    dec->padCheck = 0;
    dec->releaseMode = 1;

    switch (ac->acParams[0]) {
    case 2:
        dec->maskB = 0x200;
        dec->maskA = 0x200;
        dec->curMask = 0x200;
        break;
    case 3:
        dec->maskB = 0x400;
        dec->maskA = 0x400;
        dec->curMask = 0x400;
        break;
    case 4:
        dec->maskB = 0x800;
        dec->maskA = 0x800;
        dec->curMask = 0x800;
        break;
    case 5:
        dec->maskB = 0x20;
        dec->maskA = 0x20;
        dec->curMask = 0x20;
        break;
    case 8:
        dec->padCheck = 0x100;
        // fall through
    case 7:
        dec->releaseMode = 0;
        // fall through
    case 6:
        dec->maskB = 0x40000;
        dec->maskA = 0x40000;
        dec->curMask = 0x40000;
        break;
    case 11:
        dec->maskA = 0x100;
        dec->curMask = 0x100;
        dec->maskB = 0x200;
        break;
    case 12:
        dec->maskA = 0x400;
        dec->curMask = 0x400;
        dec->maskB = 0x800;
        break;
    case 13:
        dec->maskA = 0x10000;
        dec->curMask = 0x10000;
        dec->maskB = 0x20000;
        break;
    case 14:
        dec->maskA = 0x40000;
        dec->curMask = 0x40000;
        dec->maskB = 0x80000;
        break;
    case 15:
        dec->maskA = 0x40;
        dec->curMask = 0x40;
        dec->maskB = 0x20;
        break;
    default:
        dec->maskB = 0x100;
        dec->maskA = 0x100;
        dec->curMask = 0x100;
        break;
    }

    dec->waitFrames = 0;
    {
        s16 limit = ac->acParams[1];
        dec->timer = limit;
        dec->initTimer = limit;
    }
    dec->level = 0;
    dec->gauge = 0.0f;
    dec->presses = 0;
    dec->gaugeType = ac->acParams[2];
    dec->pressRate = ac->acParams[3];
    dec->drain = float_0p01_8041e814 * (f32)ac->acParams[4];
    dec->segmentSize = ac->acParams[5];
    if (ac->guardFrames > 0) {
        ac->guardFrames -= 1;
        dec->fillState = 1;
    } else {
        dec->fillState = 0;
    }
    ac->outputParams[2] = 0;
    ac->outputParams[0] = 0;
    ac->outputParams[1] = 0;
}

// 0.0f and the four constants after it (float_neg300, float_30, float_0p4, float_1p5) are written
// as literals throughout the file, so MWCC pools them into .sdata2 in codegen order and lands them
// at 0x38..0x4B, exactly where the original puts them. The first twelve floats stay named objects
// so their 0x00..0x37 block is emitted first (in source order). The literal 0.0f is what the target's
// five lfs sites name, and folding it (rather than an extern) is also what makes battleAcMain's
// clamp register allocation match: the constant lands in f0 and the temp in f1. The missing trailing
// four zero bytes are produced by the linker padding this 0x4C-byte .sdata2 up to its 8-byte
// alignment (the target's 0x4C gap).

s32 battleAcMain_RepeatedlyLv(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    RepeatedlyLvWork* dec = (RepeatedlyLvWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 stateChanged = 0;
    s32 autoCommand = 0;

    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    switch (ac->mAcState) {
    case 0:
        memset(disp, 0, 0x2C);
        disp->iconX = -300.0f;
        disp->iconY = 0.0f;
        disp->counter = 0x14;
        disp->field_0xC = dat_8041e7f0;
        disp->field_0x10[0] = 0;
        *(s8*)&disp->field_0x10[1] = -0x10;
        *(s8*)&disp->field_0x10[2] = -0x10;
        disp->field_0x10[3] = 0;
        disp->value = 0.0f;
        ac->result = 1;
        _init_param(work);
        ac->mAcState = 0x63;
        break;
    case 0x63:
        break;
    case 0x64:
        if (!((ac->field_0x4 & 1) && (ac->mAcUnit->mTokenFlags & 0x10)) &&
            dec->padCheck != 0 &&
            (s32)BattlePadCheckNow(dec->padCheck) == 0 &&
            autoCommand == 0) {
            dec->waitFrames++;
            if (dec->waitFrames >= 0x96) {
                ac->outputParams[2] = 0;
                ac->result &= ~1;
                ac->result |= 2;
                ac->mAcState = 0x3EA;
            }
            break;
        }
        BattleAcGaugeSeInit();
        ac->outputParams[0] = 1;
        ac->mAcState = 0x3E8;
        // fall through
    case 0x3E8:
        if ((ac->field_0x4 & 1) && (ac->mAcUnit->mTokenFlags & 0x10)) {
            if (ac->field_0x4 & 2) {
                ac->field_0x8--;
                if (ac->field_0x8 >= 0) {
                    break;
                }
            }
            ac->result &= ~1;
            ac->result |= 2;
            {
                s32 r = irand(100);

                dec->level = 0;
                while (r < 0) {
                    r -= dec->segmentSize;
                    if (r < 0) {
                        break;
                    }
                    dec->level++;
                }
            }
            ac->outputParams[2] = dec->level;
            ac->mResultCount++;
            battleAcDelete_RepeatedlyLv(work);
            return 0;
        }
        switch (ac->acParams[0]) {
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            if (dec->presses & 1) {
                dec->curMask = dec->maskB;
            } else {
                dec->curMask = dec->maskA;
            }
            break;
        }
        if (dec->releaseMode != 0) {
            if (ac->acFlag & 0x20) {
                if ((s32)BattlePadCheckNow(dec->curMask) != 0) {
                    stateChanged = 1;
                    dec->presses++;
                }
            } else if ((s32)BattlePadCheckTrigger(dec->curMask) != 0) {
                stateChanged = 1;
                dec->presses++;
            }
        } else if ((s32)BattlePadCheckUp(dec->curMask) != 0) {
            stateChanged = 1;
            dec->presses++;
        }
        if (autoCommand && !stateChanged) {
            stateChanged = 1;
            dec->presses++;
        }
        if (dec->fillState == 0) {
            if (stateChanged) {
                dec->gauge += 0.4f * (f32)dec->pressRate;
            }
            {
                f32 v = dec->gauge;
                if (v >= float_100_8041e80c) {
                    dec->gauge = float_100_8041e80c;
                    dec->fillState = 1;
                    if (ac->acFlag & 0x20) {
                        dec->fillState = -1;
                    }
                } else {
                    dec->gauge = v - dec->drain;
                    if (dec->gauge <= 0.0f) {
                        dec->gauge = 0.0f;
                    }
                }
            }
        } else if (dec->fillState == -1) {
            dec->gauge = dec->gauge - 1.5f * dec->drain;
            if (dec->gauge <= 0.0f) {
                dec->gauge = 0.0f;
                dec->fillState = 0;
            }
        }
        ac->outputParams[1] = (s32)dec->gauge;
        {
            s32 r = (s32)dec->gauge;
            if (dec->segmentSize == 0) {
                dec->segmentSize = 100;
            }
            dec->level = 0;
            while (r > 0) {
                r -= dec->segmentSize;
                dec->level++;
                if (r <= 0) {
                    break;
                }
            }
        }
        if (dec->level == 0 && (ac->acFlag & 0x40)) {
            dec->level = 1;
        }
        if (dec->fillState != 0 && !(ac->acFlag & 1)) {
            dec->level++;
        }
        ac->outputParams[2] = dec->level;
        if (dec->padCheck != 0 && (s32)BattlePadCheckNow(dec->padCheck) == 0) {
            dec->timer = 0;
        }
        BattleAcGaugeSeUpdate(dec->gauge);
        if (dec->timer > 0) {
            dec->timer--;
            break;
        }
        ac->result &= ~1;
        if (dec->gauge >= (f32)ac->acParams[6]) {
            ac->result |= 2;
        } else {
            ac->result &= ~2;
        }
        battleAcDelete_RepeatedlyLv(work);
        ac->mAcState = 0x3EA;
        break;
    case 0x3EA:
        dec->endDelay = 0x3C;
        ac->mAcState = 0x3EB;
        break;
    case 0x3EB:
        dec->endDelay--;
        if (dec->endDelay <= 0) {
            ac->mAcState = 0x3EC;
        }
        break;
    case 0x3EC:
        ac->maincb = NULL;
        ac->dispcb = NULL;
        ac->resultcb = NULL;
        ac->deletecb = NULL;
        break;
    }
    return 1;
}

s32 battleAcResult_RepeatedlyLv(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_RepeatedlyLv(CameraId camId, void* param) {
    BattleWork* work = (BattleWork*)param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* ag = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
    case 0x3E8:
    case 0x3E9:
        ag->iconX = intplGetValue(4, 0x14 - ag->counter, 0x14, -300.0f, 30.0f);
        actionCommandDisp(ag->iconX, ag->iconY);
        if (ag->counter > 0) {
            ag->counter--;
        }
        break;
    case 0x3EA:
    case 0x3EB:
        if (ag->counter >= 0x28) {
            ag->iconX = intplGetValue(4, ag->counter - 0x28, 0x14, 30.0f, -300.0f);
        } else {
            ag->iconX = 30.0f;
        }
        actionCommandDisp(ag->iconX, ag->iconY);
        if (ag->counter < 0x3C) {
            ag->counter++;
        }
        break;
    }
}

// dont_inline: the original keeps the battleAcDelete_RepeatedlyLv call in battleAcMain instead of
// inlining its two stores.
#pragma dont_inline on
void battleAcDelete_RepeatedlyLv(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EA;
    BattleAcGaugeSeDelete();
}
#pragma dont_inline reset

// Match-only: the header declares BattleAcDrawGauge's float ratio last, but the original's prototype
// lists it between `count` and the three thresholds: the ratio `lfs f1` is scheduled between the two
// `fctiwz` conversions only when the float parameter is evaluated before the threshold arguments. The
// ABI is identical (ints -> r3-r10, float -> f1). Same workaround as ac_power_gauge_lv2.c.
#ifdef MUST_MATCH
#define BattleAcDrawGaugeFM(x, y, type, count, ratio, t0, t1, t2, flags)                                  \
    ((void (*)(s32, f32, s32, s32, s32, s32, s32, s32, s32))BattleAcDrawGauge)((x), (ratio), (y), (type), \
                                                                               (count), (t0), (t1), (t2), (flags))
#else
#define BattleAcDrawGaugeFM(x, y, type, count, ratio, t0, t1, t2, flags) \
    BattleAcDrawGauge((x), (y), (type), (count), (t0), (t1), (t2), (flags), (ratio))
#endif

// optimize_for_size: the original actionCommandDisp is size-optimized (the /7 is a real `divw`, not the
// magic-multiply -O4,p emits, and the frame/allocator match only at this level). Other functions in the
// file match at the default -O4,p, so the pragma is scoped to just this function.
#pragma optimize_for_size on
// The two-icon branches name the first `-225 + x` sum and compute the paired `70 + y` only after
// `vec.x` is set. That order keeps `-225` in f0 across the sum (so the later `70.0f` load reloads f0)
// and colours the sum f3; folding the sum into `vec.x` colours it f0 and `70` f3 instead.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    RepeatedlyLvWork* dec = (RepeatedlyLvWork*)&ac->field_0x2BC[0];
    u8* p = (u8*)disp;
    s32 iconA;
    s32 iconB;
    s32 iconC;
    s32 iconD;
    s32 offA;
    s32 offB;
    s32 numIcons;
    s32 flags;
    s32 count;
    s32 arr[4];
    s32 i;
    f32 target;
    s32 div7;

    camGetPtr(CAMERA_2D);

    iconD = 0;
    iconC = 0;
    numIcons = 1;
    offB = 0;
    offA = 0;
    (*(s32*)&disp->field_0x0[0])++;

    iconA = BattleACGetButtonIcon(dec->curMask, 1);
    {
        s32 t = BattleACGetButtonIcon(dec->curMask, 0);
        iconB = t;
    }

    switch (ac->acParams[0]) {
    case 0xF:
        offA = -10;
        offB = 10;
        // fall through
    case 0xB:
    case 0xC:
        iconA = BattleACGetButtonIcon(dec->maskA, 1);
        iconB = BattleACGetButtonIcon(dec->maskA, 0);
        iconC = BattleACGetButtonIcon(dec->maskB, 1);
        iconD = BattleACGetButtonIcon(dec->maskB, 0);
        numIcons = 2;
        break;
    case 0xD:
        iconA = 0x7C;
        iconB = 0x76;
        break;
    case 0xE:
        iconA = 0x78;
        iconB = 0x7A;
        break;
    }

    div7 = *(s32*)&disp->field_0x0[0] / 7;
    if (numIcons == 1) {
        if ((div7 & 1) == 1) {
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = float_neg200_8041e7f8 + x;
            vec.y = float_70_8041e7fc + y;
            iconDispGx(float_1_8041e800, &vec, 0x10, iconB);
        } else {
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = float_neg200_8041e7f8 + x;
            vec.y = float_70_8041e7fc + y;
            iconDispGx(float_1_8041e800, &vec, 0x10, iconA);
        }
    } else {
        if ((div7 & 1) == 1) {
            f32 x0 = float_neg225_8041e804 + x;
            f32 y0;
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = x0 + (f32)offA;
            y0 = float_70_8041e7fc + y;
            vec.y = y0;
            iconDispGx(float_1_8041e800, &vec, 0x10, iconB);
            {
                f32 x1 = float_neg175_8041e808 + x;
                Vec vec2 = (Vec){0.0f, 0.0f, 0.0f};
                vec2.x = x1 + (f32)offB;
                vec2.y = y0;
                iconDispGx(float_1_8041e800, &vec2, 0x10, iconC);
            }
        } else {
            f32 x0 = float_neg225_8041e804 + x;
            f32 y0;
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = x0 + (f32)offA;
            y0 = float_70_8041e7fc + y;
            vec.y = y0;
            iconDispGx(float_1_8041e800, &vec, 0x10, iconA);
            {
                f32 x1 = float_neg175_8041e808 + x;
                Vec vec2 = (Vec){0.0f, 0.0f, 0.0f};
                vec2.x = x1 + (f32)offB;
                vec2.y = y0;
                iconDispGx(float_1_8041e800, &vec2, 0x10, iconD);
            }
        }
    }

    target = dec->gauge / float_100_8041e80c;
    if (disp->value <= target) {
        if (ac->acFlag & 8) {
            disp->value += float_0p02_8041e810;
        } else {
            disp->value += float_0p01_8041e814;
        }
        if (ac->acFlag & 0x10) {
            disp->value = target;
        }
        if (disp->value > target) {
            disp->value = target;
        }
    } else {
        if (ac->acFlag & 8) {
            disp->value -= float_0p02_8041e810;
        } else {
            disp->value -= float_0p01_8041e814;
        }
        if (ac->acFlag & 0x10) {
            disp->value = target;
        }
        if (disp->value < target) {
            disp->value = target;
        }
    }

    count = 0;
    flags = 0;
    if ((s8)dec->fillState == 1 && disp->value >= float_1_8041e800) {
        flags |= 1;
        p[0xD] += p[0x11];
        p[0xE] += p[0x12];
        if (p[0xD] <= 0x80 && (s8)p[0x11] < 0) {
            p[0x12] = 0x10;
            p[0x11] = 0x10;
        }
        if (p[0xD] >= 0xF5 && (s8)p[0x11] > 0) {
            *(s8*)&p[0x12] = -0x10;
            *(s8*)&p[0x11] = -0x10;
        }
    }
    if ((s8)dec->fillState == -1) {
        flags |= 2;
    }

    for (i = 0; i < 4; i++) {
        arr[i] = 100;
    }
    for (i = 0; i < 4; i++, count++) {
        arr[i] = ac->acGaugeParams[i];
        if (i != 0 && arr[i] - arr[i - 1] <= 0) {
            break;
        }
    }

    BattleAcDrawGaugeFM((s32)x, (s32)y, dec->gaugeType, count, disp->value, arr[0], arr[1],
                        arr[2], flags);

    if (ac->acFlag & 2) {
        Vec vec = (Vec){0.0f, 0.0f, 0.0f};
        vec.x = float_neg288_8041e818 + x +
                (f32)(ac->acParams[6] * 0xB0) / float_100_8041e80c;
        vec.y = float_45_8041e81c + y;
        iconDispGx(float_1_8041e800, &vec, 0x10, 0x96);
    }

    if (ac->acFlag & 4) {
        Mtx mtx;
        Mtx trans;
        Mtx scale;
        s32 num = ac->outputParams[2] + ac->acParams[7];

        f32 tx = float_neg200_8041e7f8 + x;

        PSMTXTrans(trans, tx + float_108_8041e820, 25.0f + y,
                   0.0f);
        PSMTXScale(scale, float_1_8041e800, float_1_8041e800, float_1_8041e800);
        PSMTXConcat(trans, scale, mtx);
        iconNumberDispGx(mtx, num, 0, *(GXColor*)&dat_8041e7f4);
    }
}
#pragma optimize_for_size reset

// Defined after the code that uses them, so the references stay bound to these names.
__declspec(section ".sdata2") s32 dat_8041e7f0 = -1;
__declspec(section ".sdata2") u32 dat_8041e7f4 = 0xFFFFFFFF;
const f32 float_neg200_8041e7f8 = -200.0f;
const f32 float_70_8041e7fc = 70.0f;
const f32 float_1_8041e800 = 1.0f;
const f32 float_neg225_8041e804 = -225.0f;
const f32 float_neg175_8041e808 = -175.0f;
const f32 float_100_8041e80c = 100.0f;
const f32 float_0p02_8041e810 = 0.02f;
const f32 float_0p01_8041e814 = 0.01f;
const f32 float_neg288_8041e818 = -288.0f;
const f32 float_45_8041e81c = 45.0f;
const f32 float_108_8041e820 = 108.0f;


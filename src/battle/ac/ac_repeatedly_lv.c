#include "battle/ac/ac_repeatedly_lv.h"
#include "battle/battle.h"
#include "driver/icondrv.h"
#include "system.h"

extern const f32 float_neg300_8041e82c;
extern const f32 float_30_8041e830;

extern const f32 float_0p01_8041e814;
extern const f32 float_0_8041e828;
extern const f32 float_100_8041e80c;
extern const f32 float_0p4_8041e834;
extern const f32 float_1p5_8041e838;
extern const f32 float_neg200_8041e7f8;
extern const f32 float_70_8041e7fc;
extern const f32 float_1_8041e800;
extern const f32 float_neg225_8041e804;
extern const f32 float_neg175_8041e808;
extern const f32 float_0p02_8041e810;
extern const f32 float_neg288_8041e818;
extern const f32 float_45_8041e81c;
extern const f32 float_108_8041e820;
extern const f32 float_25_8041e824;
extern s32 dat_8041e7f0;
extern u32 dat_8041e7f4;

// .rodata zero vectors actionCommandDisp copies onto the stack per icon; the original pools every
// access through the vec3_802fa0e8 base and indexes the rest by offset.
const Vec vec3_802fa0e8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fa0f4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fa100 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fa10c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fa118 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fa124 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fa130 = {0.0f, 0.0f, 0.0f};

static void actionCommandDisp(f32 x, f32 y);

// RepeatedlyLv's action-command work area at BattleWork+0x1F4C (inside BattleActions.field_0x2BC).
typedef struct RepeatedlyLvWork {
    s32 field_0x00; // 0x00
    s32 field_0x04; // 0x04
    s32 field_0x08; // 0x08
    u32 padCheck;   // 0x0C, BattlePadCheck mask
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    s16 field_0x18; // 0x18
    s16 field_0x1A; // 0x1A
    s16 field_0x1C; // 0x1C
    s16 field_0x1E; // 0x1E
    s16 field_0x20; // 0x20
    s16 field_0x22; // 0x22
    u8 field_0x24;  // 0x24
    s8 field_0x25;  // 0x25
    u8 field_0x26;  // 0x26
    u8 field_0x27;  // 0x27
    u8 pad_0x28[0x30 - 0x28]; // 0x28
    s32 field_0x30; // 0x30, delay timer
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
    dec->field_0x27 = 1;

    switch (*(s32*)&ac->field_34[4]) {
    case 2:
        dec->field_0x08 = 0x200;
        dec->field_0x04 = 0x200;
        dec->field_0x00 = 0x200;
        break;
    case 3:
        dec->field_0x08 = 0x400;
        dec->field_0x04 = 0x400;
        dec->field_0x00 = 0x400;
        break;
    case 4:
        dec->field_0x08 = 0x800;
        dec->field_0x04 = 0x800;
        dec->field_0x00 = 0x800;
        break;
    case 5:
        dec->field_0x08 = 0x20;
        dec->field_0x04 = 0x20;
        dec->field_0x00 = 0x20;
        break;
    case 8:
        dec->padCheck = 0x100;
        // fall through
    case 7:
        dec->field_0x27 = 0;
        // fall through
    case 6:
        dec->field_0x08 = 0x40000;
        dec->field_0x04 = 0x40000;
        dec->field_0x00 = 0x40000;
        break;
    case 11:
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        dec->field_0x08 = 0x200;
        break;
    case 12:
        dec->field_0x04 = 0x400;
        dec->field_0x00 = 0x400;
        dec->field_0x08 = 0x800;
        break;
    case 13:
        dec->field_0x04 = 0x10000;
        dec->field_0x00 = 0x10000;
        dec->field_0x08 = 0x20000;
        break;
    case 14:
        dec->field_0x04 = 0x40000;
        dec->field_0x00 = 0x40000;
        dec->field_0x08 = 0x80000;
        break;
    case 15:
        dec->field_0x04 = 0x40;
        dec->field_0x00 = 0x40;
        dec->field_0x08 = 0x20;
        break;
    default:
        dec->field_0x08 = 0x100;
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        break;
    }

    dec->field_0x22 = 0;
    {
        s16 limit = *(s32*)&ac->field_34[0x08];
        dec->field_0x20 = limit;
        dec->field_0x1E = limit;
    }
    dec->field_0x18 = 0;
    dec->field_0x14 = float_0_8041e828;
    dec->field_0x1C = 0;
    dec->field_0x24 = *(s32*)&ac->field_34[0x0C];
    dec->field_0x1A = *(s32*)&ac->field_34[0x10];
    dec->field_0x10 = float_0p01_8041e814 * (f32)*(s32*)&ac->field_34[0x14];
    dec->field_0x26 = *(s32*)&ac->field_34[0x18];
    if (*(s16*)&ac->field_78[0x10] > 0) {
        *(s16*)&ac->field_78[0x10] -= 1;
        dec->field_0x25 = 1;
    } else {
        dec->field_0x25 = 0;
    }
    ac->outputParams[2] = 0;
    ac->outputParams[0] = 0;
    ac->outputParams[1] = 0;
}

// float_0_8041e828 must be defined (value visible to the compiler) immediately before battleAcMain:
// the gauge-clamp branches then put the zero constant in f0 and the running value in f1, matching the
// original. As an extern (or defined earlier/later in the file) the allocator rotates f0/f1.
const f32 float_0_8041e828 = 0.0f;

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
        disp->iconX = float_neg300_8041e82c;
        disp->iconY = float_0_8041e828;
        disp->counter = 0x14;
        disp->field_0xC = dat_8041e7f0;
        disp->field_0x10[0] = 0;
        *(s8*)&disp->field_0x10[1] = -0x10;
        *(s8*)&disp->field_0x10[2] = -0x10;
        disp->field_0x10[3] = 0;
        disp->value = float_0_8041e828;
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
            dec->field_0x22++;
            if (dec->field_0x22 >= 0x96) {
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

                dec->field_0x18 = 0;
                while (r < 0) {
                    r -= dec->field_0x26;
                    if (r < 0) {
                        break;
                    }
                    dec->field_0x18++;
                }
            }
            ac->outputParams[2] = dec->field_0x18;
            ac->mResultCount++;
            battleAcDelete_RepeatedlyLv(work);
            return 0;
        }
        switch (*(s32*)&ac->field_34[4]) {
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            if (dec->field_0x1C & 1) {
                dec->field_0x00 = dec->field_0x08;
            } else {
                dec->field_0x00 = dec->field_0x04;
            }
            break;
        }
        if (dec->field_0x27 != 0) {
            if (*(s32*)&ac->field_34[0] & 0x20) {
                if ((s32)BattlePadCheckNow(dec->field_0x00) != 0) {
                    stateChanged = 1;
                    dec->field_0x1C++;
                }
            } else if ((s32)BattlePadCheckTrigger(dec->field_0x00) != 0) {
                stateChanged = 1;
                dec->field_0x1C++;
            }
        } else if ((s32)BattlePadCheckUp(dec->field_0x00) != 0) {
            stateChanged = 1;
            dec->field_0x1C++;
        }
        if (autoCommand && !stateChanged) {
            stateChanged = 1;
            dec->field_0x1C++;
        }
        if (dec->field_0x25 == 0) {
            if (stateChanged) {
                dec->field_0x14 += float_0p4_8041e834 * (f32)dec->field_0x1A;
            }
            {
                f32 v = dec->field_0x14;
                if (v >= float_100_8041e80c) {
                    dec->field_0x14 = float_100_8041e80c;
                    dec->field_0x25 = 1;
                    if (*(s32*)&ac->field_34[0] & 0x20) {
                        dec->field_0x25 = -1;
                    }
                } else {
                    dec->field_0x14 = v - dec->field_0x10;
                    if (dec->field_0x14 <= float_0_8041e828) {
                        dec->field_0x14 = float_0_8041e828;
                    }
                }
            }
        } else if (dec->field_0x25 == -1) {
            dec->field_0x14 = dec->field_0x14 - float_1p5_8041e838 * dec->field_0x10;
            if (dec->field_0x14 <= float_0_8041e828) {
                dec->field_0x14 = float_0_8041e828;
                dec->field_0x25 = 0;
            }
        }
        ac->outputParams[1] = (s32)dec->field_0x14;
        {
            s32 r = (s32)dec->field_0x14;
            if (dec->field_0x26 == 0) {
                dec->field_0x26 = 100;
            }
            dec->field_0x18 = 0;
            while (r > 0) {
                r -= dec->field_0x26;
                dec->field_0x18++;
                if (r <= 0) {
                    break;
                }
            }
        }
        if (dec->field_0x18 == 0 && (*(s32*)&ac->field_34[0] & 0x40)) {
            dec->field_0x18 = 1;
        }
        if (dec->field_0x25 != 0 && !(*(s32*)&ac->field_34[0] & 1)) {
            dec->field_0x18++;
        }
        ac->outputParams[2] = dec->field_0x18;
        if (dec->padCheck != 0 && (s32)BattlePadCheckNow(dec->padCheck) == 0) {
            dec->field_0x20 = 0;
        }
        BattleAcGaugeSeUpdate(dec->field_0x14);
        if (dec->field_0x20 > 0) {
            dec->field_0x20--;
            break;
        }
        ac->result &= ~1;
        if (dec->field_0x14 >= (f32)*(s32*)&ac->field_34[0x1C]) {
            ac->result |= 2;
        } else {
            ac->result &= ~2;
        }
        battleAcDelete_RepeatedlyLv(work);
        ac->mAcState = 0x3EA;
        break;
    case 0x3EA:
        dec->field_0x30 = 0x3C;
        ac->mAcState = 0x3EB;
        break;
    case 0x3EB:
        dec->field_0x30--;
        if (dec->field_0x30 <= 0) {
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
        ag->iconX = intplGetValue(4, 0x14 - ag->counter, 0x14, float_neg300_8041e82c, float_30_8041e830);
        actionCommandDisp(ag->iconX, ag->iconY);
        if (ag->counter > 0) {
            ag->counter--;
        }
        break;
    case 0x3EA:
    case 0x3EB:
        if (ag->counter >= 0x28) {
            ag->iconX = intplGetValue(4, ag->counter - 0x28, 0x14, float_30_8041e830, float_neg300_8041e82c);
        } else {
            ag->iconX = float_30_8041e830;
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

// Match-only: iconDispGx declares the icon id `s16`, but the original passes the value returned by
// BattleACGetButtonIcon straight through (no `extsh`); a differently-typed function pointer keeps
// the call a direct `bl iconDispGx`. Same workaround as ac_repeatedly.c / ac_power_gauge.c.
#ifdef MUST_MATCH
#define ICON_DISP(v, f, id, s) (((void (*)(Vec*, s16, s32, f32))iconDispGx)((v), (f), (id), (s)))
#else
#define ICON_DISP(v, f, id, s) iconDispGx((v), (f), (id), (s))
#endif

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
//
// The remaining difference (8 instructions) is a register-colouring tie-break in the two-icon branches:
// for the FIRST icon of each pair the original computes `neg225 + x` into f3 and loads the paired
// `float_70` into f0; ours swaps them (neg225+x -> f0, float_70 -> f3), and the final `fadds` operand
// order follows. Every other instruction, including the second icon of each pair and PSMTXTrans, is
// byte-identical. MWCC 2.6 picks f0 for the first temp regardless of declaration/statement order, of
// routing the sum through a local / a constant local / an `offAf` temp, a nested block, or a MUST_MATCH
// inline. `#pragma opt_propagation off` does move the sum to f3 but also perturbs the prologue's r3/r4
// assignment and the iconNumberDispGx argument order (and scores lower), so it is not used.
// NONMATCHING (99.88%): in the two-icon branches the first `neg225 + x` sum is coloured f0 (orig f3) and the paired float_70 f3 (orig f0); 8 instructions, all register-only.
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

    iconA = BattleACGetButtonIcon(dec->field_0x00, 1);
    {
        s32 t = BattleACGetButtonIcon(dec->field_0x00, 0);
        iconB = t;
    }

    switch (*(s32*)&ac->field_34[4]) {
    case 0xF:
        offA = -10;
        offB = 10;
        // fall through
    case 0xB:
    case 0xC:
        iconA = BattleACGetButtonIcon(dec->field_0x04, 1);
        iconB = BattleACGetButtonIcon(dec->field_0x04, 0);
        iconC = BattleACGetButtonIcon(dec->field_0x08, 1);
        iconD = BattleACGetButtonIcon(dec->field_0x08, 0);
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
            Vec vec = vec3_802fa0e8;
            vec.x = float_neg200_8041e7f8 + x;
            vec.y = float_70_8041e7fc + y;
            ICON_DISP(&vec, 0x10, iconB, float_1_8041e800);
        } else {
            Vec vec = vec3_802fa0f4;
            vec.x = float_neg200_8041e7f8 + x;
            vec.y = float_70_8041e7fc + y;
            ICON_DISP(&vec, 0x10, iconA, float_1_8041e800);
        }
    } else {
        if ((div7 & 1) == 1) {
            f32 y0 = float_70_8041e7fc + y;
            Vec vec = vec3_802fa100;
            vec.x = float_neg225_8041e804 + x + (f32)offA;
            vec.y = y0;
            ICON_DISP(&vec, 0x10, iconB, float_1_8041e800);
            {
                f32 x1 = float_neg175_8041e808 + x;
                Vec vec2 = vec3_802fa10c;
                vec2.x = x1 + (f32)offB;
                vec2.y = y0;
                ICON_DISP(&vec2, 0x10, iconC, float_1_8041e800);
            }
        } else {
            f32 y0 = float_70_8041e7fc + y;
            Vec vec = vec3_802fa118;
            vec.x = float_neg225_8041e804 + x + (f32)offA;
            vec.y = y0;
            ICON_DISP(&vec, 0x10, iconA, float_1_8041e800);
            {
                f32 x1 = float_neg175_8041e808 + x;
                Vec vec2 = vec3_802fa124;
                vec2.x = x1 + (f32)offB;
                vec2.y = y0;
                ICON_DISP(&vec2, 0x10, iconD, float_1_8041e800);
            }
        }
    }

    target = dec->field_0x14 / float_100_8041e80c;
    if (disp->value <= target) {
        if (*(s32*)&ac->field_34[0] & 8) {
            disp->value += float_0p02_8041e810;
        } else {
            disp->value += float_0p01_8041e814;
        }
        if (*(s32*)&ac->field_34[0] & 0x10) {
            disp->value = target;
        }
        if (disp->value > target) {
            disp->value = target;
        }
    } else {
        if (*(s32*)&ac->field_34[0] & 8) {
            disp->value -= float_0p02_8041e810;
        } else {
            disp->value -= float_0p01_8041e814;
        }
        if (*(s32*)&ac->field_34[0] & 0x10) {
            disp->value = target;
        }
        if (disp->value < target) {
            disp->value = target;
        }
    }

    count = 0;
    flags = 0;
    if ((s8)dec->field_0x25 == 1 && disp->value >= float_1_8041e800) {
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
    if ((s8)dec->field_0x25 == -1) {
        flags |= 2;
    }

    for (i = 0; i < 4; i++) {
        arr[i] = 100;
    }
    for (i = 0; i < 4; i++, count++) {
        arr[i] = *(s32*)&ac->field_78[i * 4];
        if (i != 0 && arr[i] - arr[i - 1] <= 0) {
            break;
        }
    }

    BattleAcDrawGaugeFM((s32)x, (s32)y, dec->field_0x24, count, disp->value, arr[0], arr[1],
                        arr[2], flags);

    if (*(s32*)&ac->field_34[0] & 2) {
        Vec vec = vec3_802fa130;
        vec.x = float_neg288_8041e818 + x +
                (f32)(*(s32*)&ac->field_34[0x1C] * 0xB0) / float_100_8041e80c;
        vec.y = float_45_8041e81c + y;
        ICON_DISP(&vec, 0x10, 0x96, float_1_8041e800);
    }

    if (*(s32*)&ac->field_34[0] & 4) {
        Mtx mtx;
        Mtx trans;
        Mtx scale;
        s32 num = ac->outputParams[2] + *(s32*)&ac->field_34[0x20];

        f32 tx = float_neg200_8041e7f8 + x;

        PSMTXTrans(trans, tx + float_108_8041e820, float_25_8041e824 + y,
                   float_0_8041e828);
        PSMTXScale(scale, float_1_8041e800, float_1_8041e800, float_1_8041e800);
        PSMTXConcat(trans, scale, mtx);
        iconNumberDispGx(mtx, num, 0, *(GXColor*)&dat_8041e7f4);
    }
}
#pragma optimize_for_size reset

// Defined after the code that uses them, so the references stay bound to these names.
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
const f32 float_25_8041e824 = 25.0f;
const f32 float_neg300_8041e82c = -300.0f;
const f32 float_30_8041e830 = 30.0f;
const f32 float_0p4_8041e834 = 0.4f;
const f32 float_1p5_8041e838 = 1.5f;
s32 dat_8041e7f0 = -1;
u32 dat_8041e7f4 = 0xFFFFFFFF;

// 4 zero bytes that end the unit's .sdata2 (see the original .sdata2:0x4C gap).
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") u32 gap_09_8041E83C_sdata2[1] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

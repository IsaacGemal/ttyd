#include "battle/ac/ac_repeatedly.h"
#include "battle/battle.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include <string.h>
#include <dolphin/os.h>
#include <dolphin/si.h>

static void actionCommandDisp(f32 x, f32 y);

// Float literals this unit emitted into .sdata2.
extern const f32 float_0_8041c7d8;
extern const f32 float_0p01_8041c7c0;
extern const f32 float_neg300_8041c7d0;
extern const f32 float_30_8041c7d4;
extern const f32 float_neg1_8041c7dc;
extern const f32 float_100_8041c7bc;
extern const f32 float_neg200_8041c7ac;
extern const f32 float_70_8041c7b0;
extern const f32 float_1_8041c7b4;
extern const f32 float_0p5_8041c7b8;
extern const f32 float_neg288_8041c7c4;
extern const f32 float_176_8041c7c8;
extern const f32 float_45_8041c7cc;

// sdata2 word this unit emitted; the original symbol is undefined elsewhere.
__declspec(section ".sdata2") s32 dat_8041c7a8 = -1;

// int->float conversion magic the original unit emitted into .rodata. It must NOT be defined here:
// defining it forces MWCC to emit a second anonymous copy, which shifts actionCommandDisp's pooled
// `lfd 0x40(r30)` to 0x48. Left undefined, MWCC emits its own constant at the original offset and the
// symbol name difference is ignored by the scorer.
//const f64 double_to_int_802f31a0 = 4503601774854144.0;

// .sdata table of per-difficulty power values (local to this unit in the original asm).
static s32 _power_table[7] = {0x37, 0x32, 0x2D, 0x28, 0x23, 0x1E, 0x19};

// Repeatedly's action-command work area at BattleWork+0x1F4C. No header declares it.
typedef struct AcRepeatedlyWork {
    u32 curMask;     // 0x0, button mask being checked; swapped between maskA and maskB (mode 0xB/0xC, 0x10)
    u32 maskA;       // 0x4
    u32 maskB;       // 0x8
    u32 holdMask;    // 0xC, nonzero: the button that must stay held (BattlePadCheckNow) for the command to run
    f32 goal;        // 0x10, mark 0..100 the power must reach when the timer runs out (bounces by goalStep)
    f32 goalStep;    // 0x14, goal speed per frame, sign flips at 0 and 100; 0 when the goal is fixed
    f32 power;       // 0x18, accumulated power 0..100; each press adds powerStep * table value
    f32 decay;       // 0x1C, subtracted from power every frame (acParams[6] / 100)
    u16 presses;     // 0x20, inputs counted (button, probe edge, reset edge, auto); bit 0 picks the mask in modes 0xB/0xC
    u16 swapTimer;   // 0x22, frames until curMask switches to the other mask (mode 0x10)
    s16 powerStep;   // 0x24, multiplier applied to the press power (acParams[5])
    s16 initTimer;   // 0x26, copy of the start timer (acParams[1]); written in _init_param, never read
    s16 timer;       // 0x28, frames left before the command ends; reset to 0 when holdMask is released
    u8 gaugeWidth;   // 0x2A, 0xB2 at init; the width argument of BattleAcDrawGauge
    s8 full;         // 0x2B, 1 once power reaches 100 (or when guardFrames was still running at init); draws test 1 and -1
    u8 probeHeld;    // 0x2C, toggled by the reset button (mode 0xA) or the SI probe (mode 9)
    u8 pad_0x2D[3];  // 0x2D
    s32 endDelay;    // 0x30, frames the result display waits in states 0x3EA/0x3EB
} AcRepeatedlyWork;

static void _init_param(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 param;
    s32 timer;

    dec->probeHeld = 0;
    dec->holdMask = 0;

    switch (ac->acParams[0]) {
    case 10:
        work->flags |= 0x20000000;
        break;
    case 8:
        dec->holdMask = 0x100;
        // fallthrough
    case 6:
        dec->maskB = 0x40000;
        dec->maskA = 0x40000;
        dec->curMask = 0x40000;
        break;
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
    case 11:
        dec->maskA = 0x100;
        dec->curMask = 0x100;
        dec->maskB = 0x200;
        break;
    case 16:
        dec->maskA = 0x100;
        dec->curMask = 0x100;
        dec->maskB = 0x200;
        dec->swapTimer = irand(0x1E) + 0x3C;
        break;
    case 12:
        dec->maskA = 0x400;
        dec->curMask = 0x400;
        dec->maskB = 0x800;
        break;
    default:
        dec->maskB = 0x100;
        dec->maskA = 0x100;
        dec->curMask = 0x100;
        break;
    }

    param = ac->acParams[1];
    dec->timer = param;
    dec->initTimer = param;
    dec->goal = (f32)ac->acParams[2];
    dec->goalStep = (f32)ac->acParams[3] / 10.0f;
    if (dec->goalStep != float_0_8041c7d8) {
        dec->goal = (f32)irand(100);
    }
    dec->power = (f32)ac->acParams[7];
    disp->value = float_0p01_8041c7c0 * dec->power;
    dec->presses = 0;
    dec->powerStep = ac->acParams[5];
    dec->decay = float_0p01_8041c7c0 * (f32)ac->acParams[6];
    dec->gaugeWidth = 0xB2;

    timer = ac->guardFrames;
    if (timer > 0) {
        ac->guardFrames = timer - 1;
        dec->full = 1;
    } else {
        dec->full = 0;
    }
    ac->outputParams[0] = 0;
}
s32 battleAcMain_Repeatedly(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 difficulty = BattleActionCommandGetDifficulty(work);
    s32 stateChanged;
    s32 autoCommand = 0;
    f32 v;
    f32 target;

    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    stateChanged = 0;
    switch (ac->mAcState) {
    case 0:
        memset(disp, 0, 0x2C);
        disp->iconX = float_neg300_8041c7d0;
        disp->iconY = float_0_8041c7d8;
        disp->counter = 0x14;
        disp->field_0xC = dat_8041c7a8;
        disp->field_0x10 = 0;
        disp->dg = -0x10;
        disp->db = -0x10;
        disp->field_0x13 = 0;
        disp->value = float_0_8041c7d8;
        ac->result = 1;
        dec->timer = 0;
        _init_param(work);
        ac->mAcState = 0x63;
        break;
    case 0x63:
        break;
    case 0x64:
        if (!((ac->field_0x4 & 1) && (ac->mAcUnit->mTokenFlags & 0x10)) &&
            dec->holdMask != 0 &&
            BattlePadCheckNow(dec->holdMask) == 0) {
            break;
        }
        BattleAcGaugeSeInit();
        ac->mAcState = 0x3E8;
        ac->outputParams[0] = 1;
        // fallthrough
    case 0x3E8:
        switch (ac->acParams[0]) {
        case 0xA:
            if (dec->probeHeld != 0) {
                if (OSGetResetButtonState() == 0) {
                    dec->probeHeld = 0;
                    stateChanged = 1;
                    dec->presses++;
                }
            } else if (OSGetResetButtonState() != 0) {
                dec->probeHeld = 1;
                stateChanged = 1;
                dec->presses++;
            }
            break;
        case 9:
            if (dec->probeHeld != 0) {
                if (SIProbe(0) == 8) {
                    dec->probeHeld = 0;
                    stateChanged = 1;
                    dec->presses++;
                }
            } else if (SIProbe(0) != 8 && SIProbe(0) != 0x80) {
                dec->probeHeld = 1;
                stateChanged = 1;
                dec->presses++;
            }
            break;
        case 0x10:
            dec->swapTimer--;
            if (dec->swapTimer == 0) {
                dec->swapTimer = irand(0x1E) + 0x3C;
                if (dec->curMask == dec->maskA) {
                    dec->curMask = dec->maskB;
                } else {
                    dec->curMask = dec->maskA;
                }
            }
            if (BattlePadCheckTrigger(dec->curMask) != 0) {
                stateChanged = 1;
                dec->presses++;
            }
            break;
        case 0xB:
        case 0xC:
            if (dec->presses & 1) {
                dec->curMask = dec->maskB;
            } else {
                dec->curMask = dec->maskA;
            }
            // fallthrough
        default:
            if (BattlePadCheckTrigger(dec->curMask) != 0) {
                stateChanged = 1;
                dec->presses++;
            }
            break;
        }

        if (autoCommand && !stateChanged) {
            stateChanged = 1;
            dec->presses++;
        }
        if (dec->full == 0) {
            if (stateChanged) {
                dec->power += float_0p01_8041c7c0 * (_power_table[difficulty] * (f32)dec->powerStep);
            }
#ifdef MUST_MATCH
            // Volatile reads: retail reloads both fields here after the store above.
            target = *(volatile f32*)&dec->power;
            v = *(volatile f32*)&dec->decay;
#else
            target = dec->power;
            v = dec->decay;
#endif
            v = target - v;
            dec->power = v;
#ifdef MUST_MATCH
            {
                // Match-only: the volatile read forces the reload into f1 the target has;
                // the natural form reuses f0 and swaps the compare operands.
                f32 g = *(volatile f32*)&dec->power;
                if (g <= float_0_8041c7d8) {
                    dec->power = float_0_8041c7d8;
                }
            }
#else
            if (dec->power <= float_0_8041c7d8) {
                dec->power = float_0_8041c7d8;
            }
#endif
#ifdef MUST_MATCH
            {
                // Match-only: the volatile read forces the reload into f1 the target has;
                // the natural form reuses f0 and swaps the compare operands.
                f32 g = *(volatile f32*)&dec->power;
                if (g >= float_100_8041c7bc) {
                    dec->power = float_100_8041c7bc;
                    dec->full = 1;
                }
            }
#else
            if (dec->power >= float_100_8041c7bc) {
                dec->power = float_100_8041c7bc;
                dec->full = 1;
            }
#endif
        }
        if (float_0_8041c7d8 != dec->goalStep) {
            dec->goal += dec->goalStep;
            if (dec->goal <= float_0_8041c7d8) {
                dec->goal = float_0_8041c7d8;
                dec->goalStep = dec->goalStep * float_neg1_8041c7dc;
            }
#ifdef MUST_MATCH
            {
                // Match-only: the volatile read forces the reload into f1 the target has;
                // the natural form reuses f0 and swaps the compare operands.
                f32 g = *(volatile f32*)&dec->goal;
                if (g >= float_100_8041c7bc) {
                    dec->goal = float_100_8041c7bc;
                    dec->goalStep = dec->goalStep * float_neg1_8041c7dc;
                }
            }
#else
            if (dec->goal >= float_100_8041c7bc) {
                dec->goal = float_100_8041c7bc;
                dec->goalStep = dec->goalStep * float_neg1_8041c7dc;
            }
#endif
        }
        if (dec->holdMask != 0 && BattlePadCheckNow(dec->holdMask) == 0) {
            dec->timer = 0;
        }
        BattleAcGaugeSeUpdate(dec->power);
        if (dec->timer > 0) {
            dec->timer--;
        } else {
            ac->result &= ~1;
            if (dec->power >= dec->goal || dec->full != 0) {
                ac->result |= 2;
            }
            battleAcDelete_Repeatedly(work);
            ac->mAcState = 0x3EA;
        }
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
s32 battleAcResult_Repeatedly(BattleWork* work) {
    return work->actionCommands.result;
}
void battleAcDisp_Repeatedly(CameraId camId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 99:
    case 100:
    case 1000:
    case 1001:
        disp->iconX = intplGetValue(4, 20 - disp->counter, 20, float_neg300_8041c7d0, float_30_8041c7d4);
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 1002:
    case 1003:
        if (disp->counter >= 40) {
            disp->iconX = intplGetValue(4, disp->counter - 40, 20, float_30_8041c7d4, float_neg300_8041c7d0);
        } else {
            disp->iconX = float_30_8041c7d4;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 60) {
            disp->counter++;
        }
        break;
    }
}

void battleAcDelete_Repeatedly(BattleWork* work) {
    switch (work->actionCommands.acParams[0]) {
    case 0xA:
        work->flags &= ~0x20000000;
        break;
    }
    BattleAcGaugeSeDelete();
    work->actionCommands.mAcState = 0x3EA;
}

// 100% matched.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->field_0x2BC[0];
    u8* p = (u8*)disp;
    f32 v;
    f32 target;
    s32 flags;

    camGetPtr(CAMERA_2D);

    if (ac->acParams[0] != 0x10) {
        s32 iconA;
        s32 iconB;

        switch (dec->curMask) {
        case 0x100:
            iconA = 0x65;
            iconB = 0x64;
            break;
        case 0x200:
            iconA = 0x67;
            iconB = 0x66;
            break;
        case 0x400:
            iconA = 0x69;
            iconB = 0x68;
            break;
        case 0x800:
            iconA = 0x6B;
            iconB = 0x6A;
            break;
        case 0x40000:
        default:
            iconA = 0x78;
            iconB = 0x79;
            break;
        }
        if (((disp->field_0x0 / 7) & 1) == 1) {
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = float_neg200_8041c7ac + x;
            vec.y = float_70_8041c7b0 + y;
            iconDispGx(float_1_8041c7b4, &vec, 0x10, iconB);
        } else {
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = float_neg200_8041c7ac + x;
            vec.y = float_70_8041c7b0 + y;
            iconDispGx(float_1_8041c7b4, &vec, 0x10, iconA);
        }
    } else {
        s32 alt = (disp->field_0x0 / 7) & 1;
        f32 baseX = float_neg200_8041c7ac + x;
        f32 baseY = float_70_8041c7b0 + y;
        s32 i;
        s32 off;

        for (i = 0, off = 0; i < 2; i++, off += 0x28) {
            s32 iconA;
            s32 iconB;
            s32 match;

            if (i == 0) {
                iconA = 0x65;
                iconB = 0x64;
                if (dec->curMask == dec->maskA) {
                    match = 1;
                } else {
                    match = 0;
                }
            } else {
                iconA = 0x67;
                iconB = 0x66;
                if (dec->curMask == dec->maskB) {
                    match = 1;
                } else {
                    match = 0;
                }
            }
            if (match) {
                s32 icon = (alt == 1) ? iconB : iconA;
                Vec vec = (Vec){0.0f, 0.0f, 0.0f};
                vec.x = baseX + (f32)off;
                vec.y = baseY;
                iconDispGx(float_1_8041c7b4, &vec, 0x10, icon);
            } else {
                Vec vec = (Vec){0.0f, 0.0f, 0.0f};
                vec.x = baseX + (f32)off;
                vec.y = baseY;
                iconDispGx(float_0p5_8041c7b8, &vec, 0x10, iconB);
            }
        }
    }

    target = dec->power;
    target = target / float_100_8041c7bc;
    v = disp->value;
    if (v <= target) {
        disp->value = v + float_0p01_8041c7c0;
        if (disp->value > target) {
            disp->value = target;
        }
    } else {
        disp->value = v - float_0p01_8041c7c0;
        if (disp->value < target) {
            disp->value = target;
        }
    }

    flags = 0;
    if (dec->full == 1 && disp->value >= float_1_8041c7b4) {
        flags |= 1;
        p[0xD] += disp->dg;
        p[0xE] += disp->db;
        if (p[0xD] <= 0x80 && disp->dg < 0) {
            disp->db = 0x10;
            disp->dg = 0x10;
        }
        if (p[0xD] >= 0xF5 && disp->dg > 0) {
            disp->db = -0x10;
            disp->dg = -0x10;
        }
    }
    if (dec->full == -1) {
        flags |= 2;
    }

    v = disp->value;
    BattleAcDrawGauge((s32)x, (s32)y, v, dec->gaugeWidth, 2, (ac->acFlag & 2) ? 100 : (s32)dec->goal, 100, 100, flags);
    {
        Vec vec = (Vec){0.0f, 0.0f, 0.0f};
        vec.x = float_neg288_8041c7c4 + x + (float_176_8041c7c8 * dec->goal) / float_100_8041c7bc;
        vec.y = float_45_8041c7cc + y;
        iconDispGx(float_1_8041c7b4, &vec, 0x10, 0x96);
    }
    disp->field_0x0++;
    disp->field_0x4++;
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_neg200_8041c7ac = -200.0f;
const f32 float_70_8041c7b0 = 70.0f;
const f32 float_1_8041c7b4 = 1.0f;
const f32 float_0p5_8041c7b8 = 0.5f;
const f32 float_100_8041c7bc = 100.0f;
const f32 float_0p01_8041c7c0 = 0.01f;
const f32 float_neg288_8041c7c4 = -288.0f;
const f32 float_176_8041c7c8 = 176.0f;
const f32 float_45_8041c7cc = 45.0f;
const f32 float_neg300_8041c7d0 = -300.0f;
const f32 float_30_8041c7d4 = 30.0f;
const f32 float_0_8041c7d8 = 0.0f;
const f32 float_neg1_8041c7dc = -1.0f;

// 20 zero bytes that end the unit's .sdata: an all-zero object needs explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804117CC_sdata[5] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

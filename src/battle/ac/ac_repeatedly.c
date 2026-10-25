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
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->acWork[0];
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
    if (dec->goalStep) {
        dec->goal = (f32)irand(100);
    }
    dec->power = (f32)ac->acParams[7];
    disp->value = 0.01f * dec->power;
    dec->presses = 0;
    dec->powerStep = ac->acParams[5];
    dec->decay = 0.01f * (f32)ac->acParams[6];
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
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->acWork[0];
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
        disp->iconX = -300.0f;
        disp->iconY = 0.0f;
        disp->counter = 0x14;
        disp->gaugeColor.rgba = dat_8041c7a8;
        disp->field_0x10 = 0;
        disp->dg = -0x10;
        disp->db = -0x10;
        disp->field_0x13 = 0;
        disp->value = 0.0f;
        ac->result = 1;
        dec->timer = 0;
        _init_param(work);
        ac->mAcState = 0x63;
        break;
    case 0x63:
        break;
    case 0x64:
        if (!((ac->autoFlags & 1) && (ac->mAcUnit->mTokenFlags & 0x10)) &&
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
                dec->power += 0.01f * (_power_table[difficulty] * (f32)dec->powerStep);
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
            if (dec->power <= 0.0f) {
                dec->power = 0.0f;
            }
            if (dec->power >= 100.0f) {
                dec->power = 100.0f;
                dec->full = 1;
            }
        }
        if (0.0f != dec->goalStep) {
            dec->goal += dec->goalStep;
            if (dec->goal <= 0.0f) {
                dec->goal = 0.0f;
                dec->goalStep *= -1.0f;
            }
            if (dec->goal >= 100.0f) {
                dec->goal = 100.0f;
                dec->goalStep *= -1.0f;
            }
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
        disp->iconX = intplGetValue(4, 20 - disp->counter, 20, -300.0f, 30.0f);
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 1002:
    case 1003:
        if (disp->counter >= 40) {
            disp->iconX = intplGetValue(4, disp->counter - 40, 20, 30.0f, -300.0f);
        } else {
            disp->iconX = 30.0f;
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
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->acWork[0];
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
        if (((disp->dispFrame / 7) & 1) == 1) {
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = -200.0f + x;
            vec.y = 70.0f + y;
            iconDispGx(1.0f, &vec, 0x10, iconB);
        } else {
            Vec vec = (Vec){0.0f, 0.0f, 0.0f};
            vec.x = -200.0f + x;
            vec.y = 70.0f + y;
            iconDispGx(1.0f, &vec, 0x10, iconA);
        }
    } else {
        s32 alt = (disp->dispFrame / 7) & 1;
        f32 baseX = -200.0f + x;
        f32 baseY = 70.0f + y;
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
                iconDispGx(1.0f, &vec, 0x10, icon);
            } else {
                Vec vec = (Vec){0.0f, 0.0f, 0.0f};
                vec.x = baseX + (f32)off;
                vec.y = baseY;
                iconDispGx(0.5f, &vec, 0x10, iconB);
            }
        }
    }

    target = dec->power;
    target = target / 100.0f;
    v = disp->value;
    if (v <= target) {
        disp->value += 0.01f;
        if (disp->value > target) {
            disp->value = target;
        }
    } else {
        disp->value -= 0.01f;
        if (disp->value < target) {
            disp->value = target;
        }
    }

    flags = 0;
    if (dec->full == 1 && disp->value >= 1.0f) {
        flags |= 1;
        disp->gaugeColor.color.g += disp->dg;
        disp->gaugeColor.color.b += disp->db;
        if (disp->gaugeColor.color.g <= 0x80 && disp->dg < 0) {
            disp->db = 0x10;
            disp->dg = 0x10;
        }
        if (disp->gaugeColor.color.g >= 0xF5 && disp->dg > 0) {
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
        vec.x = -288.0f + x + (176.0f * dec->goal) / 100.0f;
        vec.y = 45.0f + y;
        iconDispGx(1.0f, &vec, 0x10, 0x96);
    }
    disp->dispFrame++;
    disp->field_0x4++;
}

// 20 zero bytes that end the unit's .sdata: an all-zero object needs explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804117CC_sdata[5] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

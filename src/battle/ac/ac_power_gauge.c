#include "battle/ac/ac_power_gauge.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "system.h"
#include <string.h>

// actionCommandDisp is this unit's local helper (.fn actionCommandDisp, local).
static void actionCommandDisp(f32 x, f32 y);

// .sdata2 literals referenced by battleAcMain_PowerGauge and battleAcDisp_PowerGauge
// (offsets in REPORT.md).
extern const s32 dat_80421588;
extern const f32 float_100_8042158c;
extern const f32 float_neg300_804215bc;
extern const f32 float_30_804215c0;
extern const f32 float_0_804215c4;
extern const f32 float_101_804215c8;
extern const f64 double_to_int_802fea30;

// Runtime work area of the power gauge action command, at BattleWork+0x1F4C (inside
// BattleActions.field_0x2BC). No header declares it yet.
typedef struct PowerGaugeDecWork {
    s32 frame;        // 0x0
    f32 gaugePercent; // 0x4
    s32 counter;      // 0x8, input-frame counter
    s32 value;        // 0xC
    s8 flag;          // 0x10
    s8 state;         // 0x11
    s8 flag2;         // 0x12
    u8 pad_0x13;      // 0x13
    u32 button;       // 0x14, BattlePad button mask
    s32 countdown;    // 0x18
} PowerGaugeDecWork;

// NONMATCHING (98.26%): register allocation of the two base pointers only; the original materialises
// dec (work+0x1F4C) in r31 and disp (work+0x1F20) in r28, while MWCC folds dec to 0x2C(r31) and
// materialises disp in r31. Deriving dec from `work` folds it into the work base entirely (93.83%).
s32 battleAcMain_PowerGauge(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    PowerGaugeDecWork* dec = (PowerGaugeDecWork*)((u8*)disp + 0x2C);
    s32 autoCommand = 0;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    switch (work->actionCommands.mAcState) {
    case 0x63:
        break;
    case 0x0:
        memset(disp, 0, 0x2C);
        disp->iconX = float_neg300_804215bc;
        disp->iconY = float_0_804215c4;
        disp->counter = 0x14;
        disp->value = float_0_804215c4;
        disp->field_0xC = dat_80421588;
        dec->frame = 0;
        dec->counter = 0;
        work->actionCommands.outputParams[0] = 0;
        work->actionCommands.mAcState = 0x63;
        work->actionCommands.result = 1;
        dec->value = *(s32*)((u8*)work + 0x1CCC) + *(s32*)((u8*)work + 0x1CD0);
        dec->gaugePercent = float_0_804215c4;
        switch (*(s32*)((u8*)work + 0x1CC8)) {
        case 0:
            dec->flag2 = 1;
            dec->button = 0x60;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 1:
            dec->flag2 = 1;
            dec->button = 0x100;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 2:
            dec->flag2 = 0;
            dec->button = 0x100;
            dec->state = 0;
            break;
        case 3:
            dec->flag2 = 0;
            dec->button = 0x20;
            dec->state = 0;
            break;
        default:
            dec->flag2 = 1;
            dec->button = 0x100;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        }
        break;
    case 0x64:
        if ((work->actionCommands.field_0x4 & 1) &&
            (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
            if ((s32)irand(100) >= 0) {
                work->actionCommands.field_0x8 = 0x1E;
                dec->flag = 0;
            }
            else {
                dec->flag = 1;
                work->actionCommands.field_0x8 = 0x1E;
            }
            work->actionCommands.outputParams[0] = 1;
            work->actionCommands.mAcState = 0x3EA;
            break;
        }
        if (dec->flag2 != 0) {
            work->actionCommands.mAcState = 0x3E9;
        }
        else {
            work->actionCommands.mAcState = 0x3E8;
        }
        if (dec->flag2 != 0) {
            BattleAcGaugeSeInit();
        }
        // fallthrough
    case 0x3E8:
    case 0x3E9:
        dec->frame++;
        switch (dec->state) {
        case 0:
            if (dec->flag2 == 0 && (BattlePadGetNow() == dec->button || autoCommand != 0)) {
                dec->state = 1;
                work->actionCommands.mAcState = 0x3E9;
                dec->frame = 0;
                work->actionCommands.outputParams[0] = 1;
                BattleAcGaugeSeInit();
            }
            break;
        case 1:
            if (dec->flag2 != 0) {
                if (BattlePadGetNow() == dec->button) {
                    if (BattlePadCheckTrigger(dec->button) != 0) {
                        dec->state = 2;
                    }
                }
            }
            else {
                if ((s32)BattlePadCheckNow(dec->button) == 0) {
                    if ((s32)BattlePadCheckUp(dec->button) != 0) {
                        dec->state = 2;
                    }
                }
            }
            break;
        }
        if ((*(u32*)((u8*)work + 0x1CC4) & 1) == 0) {
            dec->counter++;
        }
        else if (dec->state == 1) {
            dec->counter++;
        }
        if (work->actionCommands.mAcState == 0x3E8) {
            if (dec->frame > 0x78) {
                dec->frame = 0;
                work->actionCommands.result = 0;
                dec->gaugePercent = float_0_804215c4;
                work->actionCommands.mAcState = 0x3EB;
                work->actionCommands.outputParams[0] = 2;
                break;
            }
        }
        else {
            if (dec->frame > dec->value) {
                dec->frame = 0;
                work->actionCommands.result = 0;
                dec->gaugePercent = float_101_804215c8;
                work->actionCommands.mAcState = 0x3EB;
                break;
            }
        }
        if (dec->frame <= *(s32*)((u8*)work + 0x1CCC)) {
            dec->gaugePercent = intplGetValue(*(s32*)((u8*)work + 0x1CD4), dec->counter,
                                              *(s32*)((u8*)work + 0x1CCC), float_0_804215c4,
                                              float_100_8042158c);
        }
        else {
            dec->gaugePercent = float_100_8042158c;
        }
        BattleAcGaugeSeUpdate(dec->gaugePercent);
        if (autoCommand != 0) {
            if (dec->gaugePercent >= (f32)*(s32*)((u8*)work + 0x1CD8)) {
                dec->state = 2;
            }
            else {
                dec->state = 1;
            }
        }
        if (dec->state == 2) {
            if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1CD8)) {
                work->actionCommands.result = 0;
                work->actionCommands.result &= ~2;
            }
            else {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            }
            work->actionCommands.mAcState = 0x3EB;
        }
        break;
    case 0x3EA:
        work->actionCommands.field_0x8--;
        if (work->actionCommands.field_0x8 < 0) {
            if (dec->flag != 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            }
            else {
                work->actionCommands.result = 0;
            }
            work->actionCommands.field_0x30 |= 1;
            dec->countdown = 0x3C;
            work->actionCommands.mAcState = 0x3ED;
            return 0;
        }
        break;
    case 0x3EB:
        work->actionCommands.mAcState = 0x3EC;
        // fallthrough
    case 0x3EC:
        work->actionCommands.field_0x30 |= 1;
        BattleAcGaugeSeDelete();
        dec->countdown = 0x3C;
        work->actionCommands.mAcState = 0x3ED;
        // fallthrough
    case 0x3ED:
        dec->countdown--;
        if (dec->countdown <= 0) {
            work->actionCommands.mAcState = 0x3EE;
        }
        break;
    case 0x3EE:
        work->actionCommands.maincb = NULL;
        work->actionCommands.dispcb = NULL;
        work->actionCommands.resultcb = NULL;
        work->actionCommands.deletecb = NULL;
        return 0;
    }
    return 1;
}

s32 battleAcResult_PowerGauge(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_PowerGauge(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    s32 state = ac->mAcState;

    switch (state) {
    case 0x63:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_804215bc, float_30_804215c0);
        // fallthrough
    case 0x64:
    case 0x3e8:
    case 0x3e9:
    case 0x3eb:
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3ec:
    case 0x3ed:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_804215c0, float_neg300_804215bc);
        } else {
            disp->iconX = float_30_804215c0;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 0x3c) {
            disp->counter++;
        }
        break;
    }
}

void battleAcDelete_PowerGauge(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

// .sdata2 definitions of this unit, in the original's order.
const s32 dat_80421588 = -1;
const f32 float_100_8042158c = 100.0f;
const f32 float_neg300_804215bc = -300.0f;
const f32 float_30_804215c0 = 30.0f;
const f32 float_0_804215c4 = 0.0f;
const f32 float_101_804215c8 = 101.0f;
const f64 double_to_int_802fea30 = 4503601774854144.0;

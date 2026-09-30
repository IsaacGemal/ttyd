#include "battle/ac/ac_repeatedly_lv.h"
#include "battle/battle.h"
#include "system.h"

const f32 float_neg300_8041e82c = -300.0f;
const f32 float_30_8041e830 = 30.0f;

extern const f32 float_0p01_8041e814;
extern const f32 float_0_8041e828;

static void actionCommandDisp(f32 x, f32 y);

// RepeatedlyLv's action-command work area at BattleWork+0x1F4C (inside BattleActions.field_0x2BC).
typedef struct RepeatedlyLvWork {
    s32 field_0x00; // 0x00
    s32 field_0x04; // 0x04
    s32 field_0x08; // 0x08
    s32 padCheck;   // 0x0C, BattlePadCheck mask
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
} RepeatedlyLvWork;

// Local to this file (.fn _init_param, local in ac_repeatedly_lv.s).
// The instruction sequence is identical (same switch table, same case bodies, same tail) but MWCC folds
// the work-area pointer into the BattleWork base (`stw r0, 0x1f54(r3)`) where the original materialises
// `dec = work + 0x1F4C` in r4 (`addi r4, r3, 0x1f4c` then `0x8(r4)`). Same wall as battleAcDisp_TimingA/
// ac_timing_wide; every pragma (opt_propagation/opt_common_subs/opt_lifetimes/global_optimizer/
// optimize_for_size) and source form (u8*/member pointer, volatile, register, inline helper, u32 cast)
// folds the same way. No compiler version (GC/1.3.2..GC/2.5) changes it.
// NONMATCHING (94.94%): work-area pointer folded into the BattleWork base instead of materialised in r4.
static void _init_param(BattleWork* work) {
    RepeatedlyLvWork* dec = (RepeatedlyLvWork*)&work->actionCommands.field_0x2BC[0];

    dec->padCheck = 0;
    dec->field_0x27 = 1;

    switch (*(s32*)&work->actionCommands.field_34[4]) {
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
        s32 limit = *(s32*)&work->actionCommands.field_34[0x08];
        dec->field_0x20 = limit;
        dec->field_0x1E = limit;
    }
    dec->field_0x18 = 0;
    dec->field_0x14 = float_0_8041e828;
    dec->field_0x1C = 0;
    dec->field_0x24 = *(s32*)&work->actionCommands.field_34[0x0C];
    dec->field_0x1A = *(s32*)&work->actionCommands.field_34[0x10];
    dec->field_0x10 = float_0p01_8041e814 * (f32)*(s32*)&work->actionCommands.field_34[0x14];
    dec->field_0x26 = *(s32*)&work->actionCommands.field_34[0x18];
    if (*(s16*)&work->actionCommands.field_78[0x10] > 0) {
        *(s16*)&work->actionCommands.field_78[0x10] -= 1;
        dec->field_0x25 = 1;
    } else {
        dec->field_0x25 = 0;
    }
    work->actionCommands.outputParams[2] = 0;
    work->actionCommands.outputParams[0] = 0;
    work->actionCommands.outputParams[1] = 0;
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

void battleAcDelete_RepeatedlyLv(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EA;
    BattleAcGaugeSeDelete();
}

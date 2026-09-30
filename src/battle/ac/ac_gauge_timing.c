#include "battle/ac/ac_gauge_timing.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "system.h"

// sdata2 constants used by battleAcDisp_GaugeTiming.
const f32 float_neg300_804218c0 = -300.0f;
const f32 float_30_804218c4 = 30.0f;

// Local to this file in the original (`.fn actionCommandDisp, local`).
static void actionCommandDisp(f32 x, f32 y);

s32 battleAcResult_GaugeTiming(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_GaugeTiming(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_804218c0, float_30_804218c4);
        // falls through
    case 0x64:
    case 0x3E8:
    case 0x3E9:
    case 0x3EB:
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3EC:
    case 0x3ED:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_804218c4, float_neg300_804218c0);
        } else {
            disp->iconX = float_30_804218c4;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 0x3C) {
            disp->counter++;
        }
        break;
    default:
        break;
    }
}

void battleAcDelete_GaugeTiming(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

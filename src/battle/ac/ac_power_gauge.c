#include "battle/ac/ac_power_gauge.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "system.h"

// actionCommandDisp is this unit's local helper (.fn actionCommandDisp, local).
static void actionCommandDisp(f32 x, f32 y);

// .sdata2 literals referenced by battleAcDisp_PowerGauge (offsets in REPORT.md).
extern const f32 float_neg300_804215bc;
extern const f32 float_30_804215c0;

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

// .sdata2 definitions of this unit.
const f32 float_neg300_804215bc = -300.0f;
const f32 float_30_804215c0 = 30.0f;

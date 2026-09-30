#include "battle/ac/ac_repeatedly_lv.h"
#include "battle/battle.h"
#include "system.h"

const f32 float_neg300_8041e82c = -300.0f;
const f32 float_30_8041e830 = 30.0f;

static void actionCommandDisp(f32 x, f32 y);

s32 battleAcResult_RepeatedlyLv(BattleWork* work) {
    return work->actionCommands.result;
}

// NONMATCHING (98.78%): MWCC folds the disp pointer into the BattleWork base (`mr r31, r4`, offsets
// 0x1f34..0x1f40) instead of `disp = param + 0x1F20` in r31 (`addi r31, r4, 0x1f20`, offsets 0x14..0x20).
void battleAcDisp_RepeatedlyLv(CameraId camId, void* param) {
    BattleWork* work = (BattleWork*)param;
    BattleACDispParams* ag = &work->actionCommands.dispParams;

    switch (work->actionCommands.mAcState) {
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

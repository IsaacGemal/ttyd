#include "battle/ac/ac_gauge_timing.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"

s32 battleAcResult_GaugeTiming(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_GaugeTiming(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

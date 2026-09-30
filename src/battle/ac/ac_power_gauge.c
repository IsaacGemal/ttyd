#include "battle/ac/ac_power_gauge.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"

s32 battleAcResult_PowerGauge(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_PowerGauge(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

#include "battle/ac/ac_power_gauge.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"

s32 battleAcResult_PowerGauge(BattleWork* work) {
    return work->actionCommands.result;
}

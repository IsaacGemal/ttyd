#include "battle/ac/ac_gauge_timing.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"

s32 battleAcResult_GaugeTiming(BattleWork* work) {
    return work->actionCommands.result;
}

#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_signal_timing.h"

s32 battleAcResult_SignalTiming(BattleWork* work) {
    return work->actionCommands.result;
}

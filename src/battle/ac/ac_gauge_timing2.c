#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_gauge_timing2.h"

s32 battleAcResult_GaugeTiming2(BattleWork* work) {
	return work->actionCommands.result;
}

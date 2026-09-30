#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_gauge_timing2.h"

s32 battleAcResult_GaugeTiming2(BattleWork* work) {
	return work->actionCommands.result;
}

void battleAcDelete_GaugeTiming2(BattleWork* work) {
	if ((work->actionCommands.field_0x30 & 1) == 0) {
		work->actionCommands.mAcState = 0x3EB;
	}
}

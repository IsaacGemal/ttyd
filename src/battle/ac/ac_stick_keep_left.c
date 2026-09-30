#include "battle/ac/ac_stick_keep_left.h"

#include "battle/battle.h"
#include "battle/battle_ac.h"

s32 battleAcResult_StickKeepLeft(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_StickKeepLeft(BattleWork* work) {
    work->actionCommands.mAcState = 0x3ED;
}

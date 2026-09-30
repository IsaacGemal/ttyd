#include "battle/ac/ac_shot.h"
#include "battle/battle.h"

s32 battleAcResult_Shot(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_Shot(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}

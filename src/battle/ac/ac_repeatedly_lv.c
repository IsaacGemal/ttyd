#include "battle/ac/ac_repeatedly_lv.h"
#include "battle/battle.h"


s32 battleAcResult_RepeatedlyLv(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_RepeatedlyLv(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EA;
    BattleAcGaugeSeDelete();
}

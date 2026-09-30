#include "battle/ac/ac_repeatedly.h"
#include "battle/battle.h"

s32 battleAcResult_Repeatedly(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_Repeatedly(BattleWork* work) {
    switch (*(s32*)&work->actionCommands.field_34[0x04]) {
    case 0xA:
        work->flags &= ~0x20000000;
        break;
    }
    BattleAcGaugeSeDelete();
    work->actionCommands.mAcState = 0x3EA;
}

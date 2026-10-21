// togezo_copy_status, shared by unit_jyugem and unit_hyper_jyugem. Included where it sits in the unit's function order
// (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(togezo_copy_status) {
    s32* args;
    s32 srcId;
    s32 dstId;
    BattleWork* wp;
    BattleUnit* src;
    BattleUnit* dst;

    args = event->args;
    srcId = BattleTransID(event, evtGetValue(event, args[0]));
    dstId = BattleTransID(event, evtGetValue(event, args[1]));
    wp = _battleWorkPointer;
    src = BattleGetUnitPtr(wp, srcId);
    dst = BattleGetUnitPtr(wp, dstId);
    dst->mSizeChangeTurns = src->mSizeChangeTurns;
    dst->mSizeChangeStrength = src->mSizeChangeStrength;
    return EVT_RETURN_DONE;
}

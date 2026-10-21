// killer_copy_status, shared by unit_killer_cannon (dou) and unit_super_killer_cannon (las). Included where it sits in
// the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(killer_copy_status) {
    s32* args = event->args;
    s32 id1 = BattleTransID(event, evtGetValue(event, args[0]));
    s32 id2 = BattleTransID(event, evtGetValue(event, args[1]));
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit1 = BattleGetUnitPtr(work, id1);
    BattleUnit* unit2 = BattleGetUnitPtr(work, id2);
    unit2->mSizeChangeTurns = unit1->mSizeChangeTurns;
    unit2->mSizeChangeStrength = unit1->mSizeChangeStrength;
    return EVT_RETURN_DONE;
}

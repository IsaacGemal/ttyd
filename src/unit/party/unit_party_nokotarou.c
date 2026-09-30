#include "unit/party/unit_party_nokotarou.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"

USER_FUNC(_check_mario_move_count) {
    s32 index = event->args[0];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);

    evtSetValue(event, index, mario->movesRemaining);
    return EVT_RETURN_DONE;
}

USER_FUNC(_color_lv_set) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 mode = evtGetValue(event, args[1]);
    s32 target = args[2];
    s32 colorLv = evtGetValue(event, target);
    BattleUnit* unit = BattleGetUnitPtr(work, BattleTransID(event, id));

    if (mode != 0) {
        unit->mMoveColorLv = colorLv;
    } else {
        evtSetValue(event, target, unit->mMoveColorLv);
    }
    return EVT_RETURN_DONE;
}

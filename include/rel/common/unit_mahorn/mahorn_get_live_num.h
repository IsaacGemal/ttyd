// mahorn_get_live_num, shared by the Wizzerd units (unit_mahorn, unit_mahorn_custom, unit_super_mahorn). Included where
// it sits in the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "system.h"

static USER_FUNC(mahorn_get_live_num) {
    s32 i;
    s32* args = event->args;
    s32 count = 0;
    BattleWork* work = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = work->units[i];
        if (unit != NULL && unit->mAlliance == 1 && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}

// kamec_get_live_num, shared by the Magikoopa units (tik's unit_kamec and tou2's unit_kamec_green, _red and _white).
// Included where it sits in the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(kamec_get_live_num) {
    s32 i;
    s32* args = event->args;
    s32 count = 0;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = wp->units[i];

        if (unit != NULL && unit->mAlliance == 1 && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}

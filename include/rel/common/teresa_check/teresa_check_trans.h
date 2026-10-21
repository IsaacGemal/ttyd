// teresa_check_trans, shared by unit_teresa and unit_purple_teresa. Included where it sits in the unit's function order (it
// sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(teresa_check_trans) {
    s32* args;
    s32 id;
    s32 count;
    s32 i;
    BattleUnit* unit;

    count = 0;
    args = event->args;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(_battleWorkPointer, i);
        if (unit) {
            if (unit->currentKind == UNIT_BOO && !BtlUnit_CheckStatus(unit, STATUS_INVISIBLE) && i != id) {
                count++;
            }
            if (unit->currentKind == UNIT_DARK_BOO && !BtlUnit_CheckStatus(unit, STATUS_INVISIBLE) && i != id) {
                count++;
            }
        }
    }
    if (count >= 1) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

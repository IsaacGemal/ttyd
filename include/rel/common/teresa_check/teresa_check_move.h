// teresa_check_move, shared by unit_teresa and unit_purple_teresa. Included where it sits in the unit's function order (it
// sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(teresa_check_move) {
    s32* args;
    BattleUnit* unit;
    s32 count;
    s32 i;

    args = event->args;
    count = 0;
    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(_battleWorkPointer, i);
        if (unit) {
            if (unit->currentKind == UNIT_BOO && unit->work[0] == 0) {
                count++;
            }
            if (unit->currentKind == UNIT_DARK_BOO && unit->work[0] == 0) {
                count++;
            }
        }
    }
    if (count >= 1) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

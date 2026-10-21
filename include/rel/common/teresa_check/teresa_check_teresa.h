// teresa_check_teresa, shared by unit_teresa and unit_purple_teresa. Included where it sits in the unit's function order (it
// sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(teresa_check_teresa) {
    s32* args;
    s32 id;
    BattleUnit* unit;
    s32 found;
    s32 kind;

    args = event->args;
    id = evtGetValue(event, args[0]);
    found = 0;
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (unit == NULL) {
        evtSetValue(event, args[1], 0);
        return EVT_RETURN_DONE;
    }
    kind = unit->currentKind;
    if (kind == UNIT_BOO) {
        found = 1;
    }
    if (kind == UNIT_DARK_BOO) {
        found = 1;
    }
    evtSetValue(event, args[1], found);
    return EVT_RETURN_DONE;
}

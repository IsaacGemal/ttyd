// get_id, shared by the two Iron Cleft units of tou2 (unit_iron_sinemon and unit_iron_sinemon2). Included where it sits
// in the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

// redId/greenId are deliberately left uninitialized: the original does not initialize them
// (it never stores before the loop and no `li` precedes it in the asm).
static USER_FUNC(get_id) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 i;
    s32 redId;
    s32 greenId;

    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(_battleWorkPointer, i);
        if (unit != NULL) {
            if (unit->currentKind == UNIT_IRON_CLEFT_RED) {
                redId = i;
            }
            if (unit->currentKind == UNIT_IRON_CLEFT_GREEN) {
                greenId = i;
            }
        }
    }
    evtSetValue(event, args[0], redId);
    evtSetValue(event, args[1], greenId);
    return EVT_RETURN_DONE;
}

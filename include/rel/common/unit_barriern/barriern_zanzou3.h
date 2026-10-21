// barriern_zanzou3. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

static USER_FUNC(barriern_zanzou3) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    return barriern_zanzou_sub(isFirstCall, unit, zanzou3_scale, 2);
}

// outSplineTableSet_Sub. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

static void outSplineTableSet_Sub(BattleUnit* unit, s32 index, void* spline) {
    BattleUnit* satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]);

    BtlUnit_GetPartsPtr(satellite, 1)->work[2] = (s32)spline;
}

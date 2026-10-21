// barriernSatelliteTo_Init. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"
#include "memory.h"

static u8 barriernSatelliteTo_Init(BattleUnit* unit, s32 index, s32 id) {
    BarriernSatelliteTo* work;

    if (unit->work[4 + index] == -1) {
        return 0;
    }
    work = __memAlloc(4, 0x5c);
    unit->work[10 + index] = (s32)work;
    work->type = 5;
    work->unitId = id;
    work->field_58 = 0;
    spline_maketable(5, work->unitId, work->points, work->field_1C);
    BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]), 1)->work[2] = id;
    return 1;
}

// outSplineTableSet. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

static USER_FUNC(outSplineTableSet) {
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    s32 type;
    s32* args;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    type = evtGetValue(event, args[1]);
    switch (type) {
    case 1:
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4]), 1);
        part->work[2] = (s32)splineTableOut1_1;
        break;
    case 2:
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4]), 1);
        part->work[2] = (s32)splineTableOut2_1;
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[5]), 1);
        part->work[2] = (s32)splineTableOut2_2;
        break;
    case 3:
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4]), 1);
        part->work[2] = (s32)splineTableOut3_1;
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[5]), 1);
        part->work[2] = (s32)splineTableOut3_2;
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[6]), 1);
        part->work[2] = (s32)splineTableOut3_3;
        break;
    case 4:
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4]), 1);
        part->work[2] = (s32)splineTableOut4_1;
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[5]), 1);
        part->work[2] = (s32)splineTableOut4_2;
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[6]), 1);
        part->work[2] = (s32)splineTableOut4_3;
        part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[7]), 1);
        part->work[2] = (s32)splineTableOut4_4;
        break;
    }
    return EVT_RETURN_DONE;
}

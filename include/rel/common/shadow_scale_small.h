// shadow_scale_small and shadow_scale_return, shared by the Piranha Plant units (unit_pakkun_flower,
// unit_monochrome_pakkun, unit_ice_pakkun, unit_poison_pakkun). Included where they sit in the unit's function order
// (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "system.h"

static USER_FUNC(shadow_scale_small) {
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    f32 start;
    f32 end;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, 1);
    if (isFirstCall) {
        unit->work[1] = 0;
    }
    start = (f32)unit->work[0];
    end = 0.0f;
    part->field_0x78 = intplGetValue(0, unit->work[1], 15, start, end);
    if (unit->work[1] >= 15) {
        return EVT_RETURN_DONE;
    }
    unit->work[1]++;
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(shadow_scale_return) {
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    f32 start;
    f32 end;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, 1);
    if (isFirstCall) {
        unit->work[1] = 0;
    }
    start = 0.0f;
    end = (f32)unit->work[0];
    part->field_0x78 = intplGetValue(0, unit->work[1], 15, start, end);
    if (unit->work[1] >= 15) {
        return EVT_RETURN_DONE;
    }
    unit->work[1]++;
    return EVT_RETURN_BLOCK;
}

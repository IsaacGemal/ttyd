// pakkun_ceil_exist and shadow_scale_memory, shared by the Piranha Plant units (unit_pakkun_flower,
// unit_monochrome_pakkun, unit_ice_pakkun, unit_poison_pakkun). Included where they sit in the unit's function order
// (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "system.h"

static USER_FUNC(pakkun_ceil_exist) {
    s32* args;

    args = event->args;
    if (_battleWorkPointer->field_180C0.stageFlags & 0x20) {
        evtSetValue(event, args[0], 0);
    } else {
        evtSetValue(event, args[0], 1);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(shadow_scale_memory) {
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, 1);
    unit->work[0] = (s32)part->field_0x78;
    return EVT_RETURN_DONE;
}

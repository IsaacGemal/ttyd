// eff_scanning, shared by the two Beldam units (unit_boss_rampell, unit_boss_rampell_las). Included where it sits in
// the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "eff/eff_scanning.h"

static USER_FUNC(eff_scanning) {
    s32* args;
    s32 id;
    f32 x;
    f32 y;
    f32 z;
    BattleUnit* unit;
    EffectEntry* entry;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    entry = effScanningEntry(0, x, y, z);
    ((effScanningData*)entry->userdata)->scale = unit->mSizeMultiplier;
    return EVT_RETURN_DONE;
}

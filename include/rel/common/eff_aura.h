// eff_aura, shared by unit_ura_noko and unit_ura_pata (tou2). Included where it sits in the unit's function order (it
// sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "eff/eff_uranoko.h"

static USER_FUNC(eff_aura) {
    BattleUnit* unit;
    EffectEntry* effect;
    effUranokoData* data;
    f32 x;
    f32 y;
    f32 z;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        unit->work[2] = (s32)effUranokoEntry(0, 120, 0.0f, -1000.0f, 0.0f);
    }
    BtlUnit_GetPos(unit, &x, &y, &z);
    effect = (EffectEntry*)unit->work[2];
    if (!(effect->flags & 1)) {
        return EVT_RETURN_DONE;
    }
    data = effect->userdata;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->baseScale = unit->mSizeMultiplier;
    return EVT_RETURN_BLOCK;
}

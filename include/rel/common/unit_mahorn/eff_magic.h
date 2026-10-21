// eff_magic and eff_magic_get_posX, shared by the Wizzerd units (unit_mahorn, unit_mahorn_custom, unit_super_mahorn).
// Included where they sit in the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "system.h"

static USER_FUNC(eff_magic) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 posX, posY, posZ;
    f32 dstX;
    f32 radius;
    s32 count;
    s32 id;
    double tmp;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    dstX = evtGetFloat(event, args[1]);
    evtGetFloat(event, args[2]);
    evtGetFloat(event, args[3]);
    count = evtGetValue(event, args[4]);
    BtlUnit_GetPos(unit, &posX, &posY, &posZ);
    posX += unit->mSizeMultiplier * unit->faceDirection;
    posY += 18.0f * unit->mSizeMultiplier;
    posZ += 5.0f;
    tmp = __fabs((dstX - posX) / (f32)count);
    radius = (f32)tmp;
    unit->work[7] = (s32)effMahorn2Entry(FALSE, posX, posY, posZ, dstX, posY, posZ, unit->mSizeMultiplier, radius);
    return EVT_RETURN_DONE;
}

static USER_FUNC(eff_magic_get_posX) {
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    effMahorn2Data* data;
    s32 i;

    s32 id = BattleTransID(event, evtGetValue(event, *args++));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    effect = (EffectEntry*)unit->work[7];
    data = (effMahorn2Data*)effect->userdata;
    for (i = 0; i < effect->count; i++, data++) {
        if (data->type == 3) {
            evtSetFloat(event, *args++, data->position.x);
            break;
        }
    }
    if (i == effect->count)
        evtSetFloat(event, *args++, 0.0f);
    return EVT_RETURN_DONE;
}

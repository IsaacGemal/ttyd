// eff_poison_barrier, shared by the Poison Puff units (unit_dokugassun, both versions) and unit_poison_pakkun. Included
// where it sits in the unit's function order (it sets the .text layout).
#include "eff/n64/eff_vapor_n64.h"
#include "evt/evt_cmd.h"
#include "system.h"
#include <stdlib.h>

static USER_FUNC(eff_poison_barrier) {
    BattleUnit* unit;
    effVaporN64Data* data;
    s32 id;
    s32 i;
    f32 x;
    f32 y;
    f32 z;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    for (i = 0; i < 3; i++) {
        BtlUnit_GetPos(unit, &x, &y, &z);
        x += (f32)(irand(20) - 10);
        y += (f32)irand(30);
        data = effVaporN64Entry(2, (i + 1) * 10, x, y, z, 0.5f + 0.5f * (f32)rand() / 32767.0f)->userdata;
        data->color1R = 32;
        data->color1G = 115;
        data->color1B = 100;
        data->color1A = 255;
        data->color2R = 32;
        data->color2G = 115;
        data->color2B = 100;
        data->color2A = 255;
        data->scale = unit->mSizeMultiplier;
    }
    return EVT_RETURN_DONE;
}

// battleGetBarriernSatelliteInitPos. Included where it sits in the unit's function order (it sets the .text layout);
// see rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

static USER_FUNC(battleGetBarriernSatelliteInitPos) {
    s32* args;
    BattleUnit* unit;
    s32 id;
    s32 n;
    s32 m;
    s32 i;
    s32 j;
    s32 indexX;
    s32 indexY;
    s32 indexZ;
    const Vec* p;
    BattleWork* wp;
    static const Vec VecData[] = {
        { 25.f, 0.f, 0.f },   { 25.f, 0.f, 0.f },   { -25.f, 0.f, 0.f },  { 0.f, 25.f, 0.f },
        { 25.f, -25.f, 0.f }, { -25.f, -25.f, 0.f }, { 25.f, 25.f, 0.f }, { -25.f, 25.f, 0.f },
        { 25.f, -25.f, 0.f }, { -25.f, -25.f, 0.f },
    };

    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    n = evtGetValue(event, args[1]);
    m = evtGetValue(event, args[2]);
    indexX = args[3];
    indexY = args[4];
    indexZ = args[5];
    p = VecData;
    for (i = 0; i < n; i++) {
        for (j = 0; j < i; j++) {
            p++;
        }
    }
    for (j = 0; j < m; j++) {
        p++;
    }
    evtSetValue(event, indexX, (s32)(p->x * unit->faceDirection));
    evtSetValue(event, indexY, (s32)p->y);
    evtSetValue(event, indexZ, (s32)p->z);
    return EVT_RETURN_DONE;
}

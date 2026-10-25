// barriernSatelliteTo_Main. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

static u8 barriernSatelliteTo_Main(BattleUnit* unit, s32 index) {
    BarriernSatelliteTo* work;
    BattleUnit* satellite;
    f32 pos[3];
    f32 out[3];
    f32 t;

    if (unit->work[4 + index] == -1) {
        return 0;
    }
    work = (BarriernSatelliteTo*)unit->work[10 + index];
    if (work == NULL) {
        return 0;
    }
    if (work->field_58 >= 0x3c) {
        if (work != NULL) {
            __memFree(4, work);
            unit->work[10 + index] = 0;
        }
        return 0;
    }
    t = intplGetValue(4, work->field_58, 0x3c, 0.0f, 1.0f);
    spline_getvalue(out, work->type, work->points, work->knots, work->tangents, t);
    satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]);
    if (satellite == NULL) {
        return 0;
    }
    BtlUnit_GetPos(unit, &pos[0], &pos[1], &pos[2]);
    BtlUnit_SetPos(satellite, pos[0] + out[0] * unit->faceDirection, pos[1] + out[1], pos[2] + out[2]);
    BtlUnit_SetHomePos(satellite, pos[0] + out[0] * unit->faceDirection, pos[1] + out[1], pos[2] + out[2]);
    work->field_58++;
    return 1;
}

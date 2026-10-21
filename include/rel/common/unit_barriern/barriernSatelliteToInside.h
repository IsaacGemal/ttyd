// barriernSatelliteToInside. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

static USER_FUNC(barriernSatelliteToInside) {
    s32* args;
    s32 id;
    s32 type;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 i;
    s32 spline;
    u8 hasSat;

    args = event->args;
    id = evtGetValue(event, args[0]);
    type = evtGetValue(event, args[1]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        for (i = 0; i < 4; i++) {
            if (unit->work[4 + i] == -1) {
                continue;
            }
            part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + i]), 1);
            spline = part->work[2];
            if (spline == (s32)splineTableOut1_1) {
                barriernSatelliteTo_Init(unit, i, splineTableIn1_1);
            }
            if (spline == (s32)splineTableOut2_1) {
                barriernSatelliteTo_Init(unit, i, splineTableIn2_1);
            }
            if (spline == (s32)splineTableOut2_2) {
                barriernSatelliteTo_Init(unit, i, splineTableIn2_2);
            }
            if (spline == (s32)splineTableOut3_1) {
                barriernSatelliteTo_Init(unit, i, splineTableIn3_1);
            }
            if (spline == (s32)splineTableOut3_2) {
                barriernSatelliteTo_Init(unit, i, splineTableIn3_2);
            }
            if (spline == (s32)splineTableOut3_3) {
                barriernSatelliteTo_Init(unit, i, splineTableIn3_3);
            }
            if (spline == (s32)splineTableOut4_1) {
                barriernSatelliteTo_Init(unit, i, splineTableIn4_1);
            }
            if (spline == (s32)splineTableOut4_2) {
                barriernSatelliteTo_Init(unit, i, splineTableIn4_2);
            }
            if (spline == (s32)splineTableOut4_3) {
                barriernSatelliteTo_Init(unit, i, splineTableIn4_3);
            }
            if (spline == (s32)splineTableOut4_4) {
                barriernSatelliteTo_Init(unit, i, splineTableIn4_4);
            }
        }
    }
    hasSat = 0;
    for (i = 0; i < 4; i++) {
        if (barriernSatelliteTo_Main(unit, i) == 1) {
            hasSat = 1;
        }
    }
    return !hasSat ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

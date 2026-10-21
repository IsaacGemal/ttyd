// barriernSatelliteToOutside. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

// Matches via barriernSatelliteTo_InitInline (rel/common/unit_barriern/barriernSatelliteToInside.h): the volatile table parameter makes MWCC rematerialise
// the spline-table address for the inlined `work->unitId` store while `inv` carries the loop-cached copy to
// the parts->work[2] store, exactly as the target does.
static USER_FUNC(barriernSatelliteToOutside) {
    BattleUnit* unit;
    s32* args;
    s32 id;
    s32 type;
    s32 i;
    u8 ret;

    args = event->args;
    id = evtGetValue(event, args[0]);
    type = evtGetValue(event, args[1]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        switch (type) {
        case 1:
            i = 0;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut1_1, (s32*)splineTableOut1_1) == 1)
                    break;
                i++;
            }
            break;
        case 2:
            i = 0;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut2_1, (s32*)splineTableOut2_1) == 1)
                    break;
                i++;
            }
            i++;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut2_2, (s32*)splineTableOut2_2) == 1)
                    break;
                i++;
            }
            break;
        case 3:
            i = 0;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut3_1, (s32*)splineTableOut3_1) == 1)
                    break;
                i++;
            }
            i++;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut3_2, (s32*)splineTableOut3_2) == 1)
                    break;
                i++;
            }
            i++;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut3_3, (s32*)splineTableOut3_3) == 1)
                    break;
                i++;
            }
            break;
        case 4:
            i = 0;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut4_1, (s32*)splineTableOut4_1) == 1)
                    break;
                i++;
            }
            i++;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut4_2, (s32*)splineTableOut4_2) == 1)
                    break;
                i++;
            }
            i++;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut4_3, (s32*)splineTableOut4_3) == 1)
                    break;
                i++;
            }
            i++;
            for (;;) {
                if (barriernSatelliteTo_Init(unit, i, (s32*)splineTableOut4_4, (s32*)splineTableOut4_4) == 1)
                    break;
                i++;
            }
            break;
        default:
            return EVT_RETURN_DONE;
        }
    }
    ret = 0;
    for (i = 0; i < 4; i++) {
        if (barriernSatelliteTo_Main(unit, i) == 1) {
            ret = 1;
        }
    }
    return ret == 0 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
#undef barriernSatelliteTo_Init

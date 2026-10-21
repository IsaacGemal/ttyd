// barriernSatelliteToInside. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"
#include "memory.h"

// barriernSatelliteToInside and barriernSatelliteToOutside (rel/common/unit_barriern/barriernSatelliteToOutside.h) call
// barriernSatelliteTo_Init through this macro; barriernSatelliteToOutside.h undefines it.
#ifdef MUST_MATCH
// MUST_MATCH: same body as barriernSatelliteTo_Init, but the spline-table pointer arrives through a
// `volatile` parameter so MWCC rematerialises the table address for the inlined `work->unitId` store
// (the target does), while `inv` keeps a normal copy alive across the calls for the later
// parts->work[2] store. Passing the same symbol twice is behaviourally identical.
static inline u8 barriernSatelliteTo_InitInline(BattleUnit* unit, s32 index, s32* volatile table, s32* inv) {
    BarriernSatelliteTo* work;

    if (unit->work[4 + index] == -1) {
        return 0;
    }
    work = __memAlloc(4, 0x5c);
    unit->work[10 + index] = (s32)work;
    work->type = 5;
    work->unitId = (s32)table;
    work->field_58 = 0;
    spline_maketable(5, work->unitId, work->points, work->field_1C);
    BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]), 1)->work[2] = (s32)inv;
    return 1;
}
#define barriernSatelliteTo_Init(unit, index, table, inv) barriernSatelliteTo_InitInline(unit, index, table, inv)
#else
#define barriernSatelliteTo_Init(unit, index, table, inv) barriernSatelliteTo_Init(unit, index, (s32)inv)
#endif

// Matches via barriernSatelliteTo_InitInline: the `volatile` table argument makes MWCC rematerialise
// the In-table address for the inlined `work->unitId` store, while the plain `inv` argument reuses the
// loop-cached copy for the later parts->work[2] store, exactly as the target's first loop does.
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
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn1_1, (s32*)splineTableIn1_1);
            }
            if (spline == (s32)splineTableOut2_1) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn2_1, (s32*)splineTableIn2_1);
            }
            if (spline == (s32)splineTableOut2_2) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn2_2, (s32*)splineTableIn2_2);
            }
            if (spline == (s32)splineTableOut3_1) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn3_1, (s32*)splineTableIn3_1);
            }
            if (spline == (s32)splineTableOut3_2) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn3_2, (s32*)splineTableIn3_2);
            }
            if (spline == (s32)splineTableOut3_3) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn3_3, (s32*)splineTableIn3_3);
            }
            if (spline == (s32)splineTableOut4_1) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn4_1, (s32*)splineTableIn4_1);
            }
            if (spline == (s32)splineTableOut4_2) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn4_2, (s32*)splineTableIn4_2);
            }
            if (spline == (s32)splineTableOut4_3) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn4_3, (s32*)splineTableIn4_3);
            }
            if (spline == (s32)splineTableOut4_4) {
                barriernSatelliteTo_Init(unit, i, (s32*)splineTableIn4_4, (s32*)splineTableIn4_4);
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

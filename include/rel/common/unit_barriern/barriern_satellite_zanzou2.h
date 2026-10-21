// barriern_satellite_zanzou2. Included where it sits in the unit's function order (it sets the .text layout); see
// rel/common/unit_barriern.h.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_barriern.h"

static USER_FUNC(barriern_satellite_zanzou2) {
    s32 index;
    BattleUnit* unit;
#ifdef MUST_MATCH
    BattleUnit* p;
#endif
    s32* args;
    s32 id;
    BattleWork* wp;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    index = evtGetValue(event, args[1]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, id);
#ifdef MUST_MATCH
    return barriern_satellite_zanzou_sub(isFirstCall, (p = BattleGetUnitPtr(wp, unit->work[4 + index]), p),
                                  satellite_zanzou2_scale, index, 1);
#else
    return barriern_satellite_zanzou_sub(isFirstCall, BattleGetUnitPtr(wp, unit->work[4 + index]),
                                  satellite_zanzou2_scale, index, 1);
#endif
}

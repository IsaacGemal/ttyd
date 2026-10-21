// _set_draw_callback, shared by unit_heavy_bom and unit_giant_bomb. Included where it sits in the unit's function order
// (it sets the .text layout); the unit declares _fire_point_callback before it.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/animdrv.h"
#include "evt/evt_cmd.h"

static USER_FUNC(_set_draw_callback) {
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;

    wp = _battleWorkPointer;
    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(wp, id);
    part = BtlUnit_GetPartsPtr(unit, 1);
    animPoseSetDispCallBack(part->poseId, _fire_point_callback, part);
    part = BtlUnit_GetPartsPtr(unit, 2);
    part->attributes |= 0x1000000;
    return EVT_RETURN_DONE;
}

// chain_init, shared by unit_wanwan and unit_burst_wanwan. Included where it sits in the unit's function order (it sets
// the .text layout).
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(chain_init) {
    s32* args;
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    s32 i;
    f32 x;
    f32 y;
    f32 z;

    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, *args++);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    for (i = 2; i <= 7; i++) {
        part = BtlUnit_GetPartsPtr(unit, i);
        BtlUnit_SetPartsPos(part, x, y, z);
        BtlUnit_SetAnim(part, "WAN_S_1B");
    }
    part = BtlUnit_GetPartsPtr(unit, 8);
    BtlUnit_SetPartsPos(part, x - 20.0f * unit->faceDirection, y, z);
    BtlUnit_SetAnim(part, "WAN_S_1B");
    part = BtlUnit_GetPartsPtr(unit, 9);
    BtlUnit_SetPartsPos(part, x - 20.0f * unit->faceDirection, y, z);
    BtlUnit_SetAnim(part, "WAN_Z_2");
    return EVT_RETURN_DONE;
}

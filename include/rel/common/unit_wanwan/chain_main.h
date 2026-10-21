// chain_main, shared by unit_wanwan and unit_burst_wanwan. Included where it sits in the unit's function order (it sets
// the .text layout).
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

static USER_FUNC(chain_main) {
    s32* args;
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    s32 i;
    f32 x;
    f32 y;
    f32 z;
    f32 ax;
    f32 ay;
    f32 az;
    f32 bx;
    f32 by;
    f32 bz;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 len;
    f32 scale;
    f32 pull;

    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, *args++);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    if (unit == NULL) {
        return EVT_RETURN_BLOCK;
    }
    BtlUnit_GetPartsPtr(unit, 1);
    unit = BattleGetUnitPtr(wp, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    part = BtlUnit_GetPartsPtr(unit, 9);
    BtlUnit_SetPartsPos(part, x - 20.0f * unit->faceDirection, y, z);
    if (unit->work[2] == 2) {
        part = BtlUnit_GetPartsPtr(unit, 9);
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        for (i = 8; i >= 1; i--) {
            part = BtlUnit_GetPartsPtr(unit, i);
            BtlUnit_GetPartsPos(part, &bx, &by, &bz);
            part->work[0] = (s32)(bx - x);
            part->work[1] = (s32)(by - y);
        }
        unit->work[2] = 1;
    }
    if (unit->work[2] == 1) {
        part = BtlUnit_GetPartsPtr(unit, 9);
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        for (i = 8; i >= 1; i--) {
            part = BtlUnit_GetPartsPtr(unit, i);
            BtlUnit_SetPartsPos(part, x + part->work[0], y + part->work[1], z);
        }
        return EVT_RETURN_DONE;
    }
    part = BtlUnit_GetPartsPtr(unit, 1);
    BtlUnit_GetPartsPos(part, &ax, &ay, &az);
    part = BtlUnit_GetPartsPtr(unit, 9);
    BtlUnit_GetPartsPos(part, &bx, &by, &bz);
    dx = ax - bx;
    dy = ay - by;
    dz = az - bz;
    scale = (f32)sqrt(dx * dx + dy * dy + dz * dz) / 8.0f;
    if (scale < 10.0f) {
        scale = 10.0f;
    }
    pull = 300.0f / (scale * scale);
    part = BtlUnit_GetPartsPtr(unit, 1);
    BtlUnit_GetPartsPos(part, &x, &y, &z);
    if (y > 110.0f) {
        y = 110.0f;
    }
    BtlUnit_SetPartsPos(part, x, y, z);
    part = BtlUnit_GetPartsPtr(unit, 1);
    BtlUnit_GetPartsPos(part, &x, &y, &z);
    x += (f32)(-(unit->faceDirection << 4));
    y += 16.0f;
    part = BtlUnit_GetPartsPtr(unit, 2);
    BtlUnit_SetPartsPos(part, x, y, z);
    i = 2;
    do {
        part = BtlUnit_GetPartsPtr(unit, i);
        BtlUnit_GetPartsPos(part, &ax, &ay, &az);
        part = BtlUnit_GetPartsPtr(unit, i + 1);
        BtlUnit_GetPartsPos(part, &bx, &by, &bz);
        by -= pull;
        dx = ax - bx;
        dy = ay - by;
        dz = az - bz;
        len = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        len = 1.0f - scale / len;
        bx += dx * len;
        by += dy * len;
        bz += dz * len;
        if (by < 0.0f) {
            by = 0.0f;
        }
        BtlUnit_SetPartsPos(part, bx, by, bz);
        i++;
    } while (i <= 7);
    i = 9;
    do {
        part = BtlUnit_GetPartsPtr(unit, i);
        BtlUnit_GetPartsPos(part, &ax, &ay, &az);
        part = BtlUnit_GetPartsPtr(unit, i - 1);
        BtlUnit_GetPartsPos(part, &bx, &by, &bz);
        by -= pull;
        dx = ax - bx;
        dy = ay - by;
        dz = az - bz;
        len = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        len = 1.0f - scale / len;
        bx += dx * len;
        by += dy * len;
        bz += dz * len;
        if (by < 0.0f) {
            by = 0.0f;
        }
        BtlUnit_SetPartsPos(part, bx, by, bz);
        i--;
    } while (i >= 3);
    return EVT_RETURN_DONE;
}

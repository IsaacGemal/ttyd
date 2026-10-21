// hand_init, hand_end, hand_ctrl, set_hand_dest and mahorn_get_bunsin, shared by the Wizzerd units (unit_mahorn,
// unit_mahorn_custom, unit_super_mahorn). Included where they sit in the unit's function order (it sets the .text
// layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "system.h"

static USER_FUNC(hand_init) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    f32 x, y, z;
    s32 partNum;
    s32 index;
    s32 id;

    id = evtGetValue(event, *args++);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    BtlUnit_GetPos(unit, &x, &y, &z);
    index = 0;
    for (partNum = 2; partNum <= 5; partNum++, index++) {
        Vec* dest;
        BtlUnit_SetAnim(BtlUnit_GetPartsPtr(unit, partNum), "MAH_Z_2");
        dest = (Vec*)__memAlloc(HEAP_BATTLE, sizeof(Vec));
        unit->work[index] = (s32)dest;
        dest->x = 0.0f;
        dest->y = 0.0f;
        dest->z = 0.0f;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(hand_end) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 i;
    s32 index;
    s32 id;

    id = evtGetValue(event, *args++);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    index = 0;
    for (i = 2; i <= 5; i++, index++) {
        void* ptr = (void*)unit->work[index];
        if (ptr != NULL) {
            BattleFree(ptr);
            unit->work[index] = 0;
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(hand_ctrl) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s8 turns;
    s8 strength;
    s32 partNum;
    s32 index;
    s32 id;

    id = evtGetValue(event, *args++);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    if (unit == NULL)
        return EVT_RETURN_BLOCK;
    BtlUnit_GetStatus(unit, STATUS_STOP, &turns, &strength);
    if (turns > 0)
        return EVT_RETURN_DONE;
    BtlUnit_GetStatus(unit, STATUS_FREEZE, &turns, &strength);
    if (turns > 0)
        return EVT_RETURN_DONE;
    BtlUnit_GetStatus(unit, STATUS_SLEEP, &turns, &strength);
    if (turns > 0)
        return EVT_RETURN_DONE;
    index = 0;
    for (partNum = 2; partNum <= 5; partNum++, index++) {
        BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, partNum);
        Vec* dest = (Vec*)unit->work[index];
        f32 px = part->mPositionOffset.x * part->mBaseScale.x;
        f32 py = part->mPositionOffset.y * part->mBaseScale.y;
        f32 pz = part->mPositionOffset.z * part->mBaseScale.z;
        f32 dx = dest->x - px;
        f32 dy = dest->y - py;
        f32 dz = dest->z - pz;
        px = px + dx / 10.0f;
        py = py + dy / 10.0f;
        pz = pz + dz / 10.0f;
        part->mPositionOffset.x = px;
        part->mPositionOffset.y = py;
        part->mPositionOffset.z = pz;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(set_hand_dest) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 unitId;
    s32 partNum;
    s32 index;
    f32 x, y, z;
    Vec* dest;

    unitId = evtGetValue(event, args[0]);
    partNum = evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, unitId));
    if (unit == NULL)
        return EVT_RETURN_BLOCK;
    index = partNum - 2;
    dest = (Vec*)unit->work[index];
    if (BtlUnit_CheckStatus(unit, STATUS_HUGE)) {
        dest->x = 1.25f * -x;
        dest->y = 1.25f * y;
        dest->z = 1.25f * z;
    } else if (BtlUnit_CheckStatus(unit, STATUS_TINY)) {
        dest->x = 0.75f * -x;
        dest->y = 0.75f * y;
        dest->z = 0.75f * z;
    } else {
        dest->x = -x;
        dest->y = y;
        dest->z = z;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(mahorn_get_bunsin) {
    s32 found[4];
    s32 i;
    s32 j;
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 count = 0;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = wp->units[i];
        if (unit != NULL
            && (unit->currentKind == UNIT_DARK_WIZZERD_CLONE || unit->currentKind == UNIT_ELITE_WIZZERD_CLONE)) {
            found[count++] = i;
        }
    }
    for (j = 0; j < 4; j++) {
        if (j < count)
            evtSetValue(event, *args++, found[j]);
        else
            evtSetValue(event, *args++, -1);
    }
    evtSetValue(event, *args++, count);
    return EVT_RETURN_DONE;
}

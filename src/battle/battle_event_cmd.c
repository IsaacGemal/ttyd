/* TODO: whoever's unfucking this file, every function before btlevtcmd_GetHp
 * needs to be refactored to have BattleWork* wp = _battleWorkPointer; right
 * below args (line 2 in each function), along with having to explicitly store
 * btlId (can't condense to one line), should fix most problems
*/
#include "battle/battle_event_cmd.h"
#include "battle/battle_acrobat.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_unit_event.h"
#include "eff/eff_nice.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include <stdlib.h>
#include "pmario_sound.h"
#include "battle/battle_enemy_item.h"
#include "battle/battle_ac_help.h"
#include "battle/battle_stage_object.h"
#include "battle/battle.h"
#include "battle/battle_seq.h"
#include "battle/ac/ac_timing_a.h"
#include "battle/battle_seq_command.h"
#include "battle/battle_damage.h"
#include "battle/battle_audience.h"
#include "battle/battle_ac.h"

// LOCAL_PROTO(BattleCheckCounter): battle.c uses this function symbol as a byte array for split-owned jump-table relocations; a shared function declaration conflicts.
s32 BattleCheckCounter(void* work, BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part, s32 value, s32* out);

// The original unit was built for size; without this the neg/or/srawi mask and
// the /60 reciprocal-multiply codegen differ from the target (verified with the
// GC/2.6 compiler).

extern BattleWork* _battleWorkPointer;

//s32 frameIndex, s32 retIndex
USER_FUNC(btlevtcmd_ftof) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    s32 index = args[1];
    s32 frame = sysMsec2Frame(value * 1000 / 60);

    evtSetValue(event, index, frame);
    return EVT_RETURN_DONE;
}

//s32 frameIndex, s32 retIndex
USER_FUNC(btlevtcmd_ftomsec) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], value * 1000 / 60);
    return EVT_RETURN_DONE;
}

//s32* eventId
USER_FUNC(btlevtcmd_WaitEventEnd) {
    s32 id;

    id = evtGetValue(event, *event->args);
    return evtCheckID(id) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

//s32 retIndex, s32 battleFlags
// NONMATCHING (98.25%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_check_battleflag) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 index = args[0];

    if (wp->flags & args[1]) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}

//BOOL* onoff, s32 flags
// NONMATCHING (72.67%): 2 instructions differ, 2 missing, 3 extra, plus register differences.
USER_FUNC(btlevtcmd_onoff_battleflag) {
    s32* args = event->args;
    s32 flags = args[1];

    if (evtGetValue(event, args[0]) == TRUE) {
        _battleWorkPointer->flags |= flags;
    }
    else {
        _battleWorkPointer->flags &= ~flags;
    }
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_get_turn) {
    evtSetValue(event, *event->args, _battleWorkPointer->turnCount);
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_reset_turn) {
    _battleWorkPointer->flags |= 0x2000;
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_reset_move_color_lv_all) {
    btlsubResetMoveColorLvAll(_battleWorkPointer);
    return EVT_RETURN_DONE;
}

//s32 retIndex, s32 phaseSeqId
USER_FUNC(btlevtcmd_CheckPhase) {
    s32* args = event->args;
    s32 index = args[0];
    u32 seq = args[1];
    if (seq != BattleGetSeq(_battleWorkPointer, SEQ_PHASE)) {
        evtSetValue(event, index, FALSE);
    }
    else {
        evtSetValue(event, index, TRUE);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 indexX, s32 indexY, s32 indexZ
USER_FUNC(btlevtcmd_GetPos) {
    s32* args = event->args;
    s32 indexX, indexY, indexZ;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    indexX = args[1];
    indexY = args[2];
    indexZ = args[3];
    BtlUnit_GetPos(unit, &x, &y, &z);
    if (indexX != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexX, (s32)x);
    }
    if (indexY != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexY, (s32)y);
    }
    if (indexZ != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexZ, (s32)z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 indexX, s32 indexY, s32 indexZ
USER_FUNC(btlevtcmd_GetPosFloat) {
    s32* args = event->args;
    s32 indexX, indexY, indexZ;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;


    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    indexX = args[1];
    indexY = args[2];
    indexZ = args[3];
    BtlUnit_GetPos(unit, &x, &y, &z);
    if (indexX != EVTDAT_POINTER_MAX) {
        evtSetFloat(event, indexX, x);
    }
    if (indexY != EVTDAT_POINTER_MAX) {
        evtSetFloat(event, indexY, y);
    }
    if (indexZ != EVTDAT_POINTER_MAX) {
        evtSetFloat(event, indexZ, z);
    }
    return EVT_RETURN_DONE;
}

//s32* maxValue, s32 retIndex
USER_FUNC(btlevtcmd_GetRandomValue) {
    s32* args = event->args;
    s32 max, index;

    max = evtGetValue(event, args[0]);
    index = args[1];
    evtSetValue(event, index, irand(max));
    return EVT_RETURN_DONE;
}

//s32 retIndex, s32 countIndex, s32 sizeIndex[]
// NONMATCHING (90.24%): 1 instruction differs, 2 missing, 2 extra, plus register differences.
USER_FUNC(btlevtcmd_DrawLots) {
    s32* args = event->args;
    s32 *alloc, *args2;
    s32 i, index, countId, count, max;
    s32 ceiling, ceilcount, retcount;

    index = args[0];
    countId = args[1];
    args2 = &args[2];
    count = evtGetValue(event, countId);
    max = 0;
    alloc = BattleAlloc(sizeof(s32) * count);
    for (i = 0; i < count; i++) {
        alloc[i] = evtGetValue(event, (*args2)++);
        max += alloc[i];
    }
    ceiling = irand(max);
    ceilcount = 0;
    retcount = 0;
    for (i = 0; i < count; i++) {
        ceilcount += alloc[i];
        if (ceiling < ceilcount) {
            break;
        }
        retcount++;
    }
    evtSetValue(event, index, retcount);
    BattleFree(alloc);
    return EVT_RETURN_DONE;
}

//s32 retIndex for unitId, BattleUnitSetup** setup, s32 spawnFlags
// NONMATCHING (75.31%): 1 instruction differs, 4 missing, 3 extra, plus register differences.
USER_FUNC(btlevtcmd_SpawnUnit) {
    s32* args = event->args;
    BattleUnitSetup* setup;
    BattleUnit* unit;
    s32 index;
    
    index = args[0];
    setup = (BattleUnitSetup*)evtGetValue(event, args[1]);
    unit = BtlUnit_Spawn(setup, args[2]);
    if (unit) {
        evtSetValue(event, index, unit->unitId);
    }
    else {
        evtSetValue(event, index, -1);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 flags
USER_FUNC(btlevtcmd_KillUnit) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, flags;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    flags = args[1];
    if (!unit) {
        return EVT_RETURN_DONE;
    }
    unit->currentHp = 0;
    BtlUnit_ClearStatus(unit);
    if (flags & 2) {
        unit->mAttributeFlags |= 0x20000;
        return EVT_RETURN_DONE;
    }
    else {
        BtlUnit_SetStatus(unit, STATUS_INSTANT_KILL, 1, 1);
        if (!(flags & 1)) {
            unit->flags |= 0x80000000;
        }
        return EVT_RETURN_DONE;
    }
}

//s32* battleId, s32* copyToPartId, BattleUnitKindPart* kind, s32* copyFromPartId
// NONMATCHING (89.87%): 2 instructions differ, 4 missing, 2 extra, plus register differences.
USER_FUNC(btlevtcmd_ReplaceParts) {
    s32* args = event->args;
    s32 id, transId, lookupPart, partNum;
    BattleUnitKindPart* kind;
    BattleUnit* unit;
    BattleUnitPart* part;

    id = evtGetValue(event, args[0]);
    transId = BattleTransID(event, id);
    lookupPart = evtGetValue(event, args[1]);
    kind = (BattleUnitKindPart*)args[2];
    partNum = evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, transId);
    part = BattleGetUnitPartsPtr(transId, lookupPart);
    if (kind) {
        while (kind->partNum != partNum) {
            ;
        }
        part->mKindPartParams = kind;
        part->partName = part->mKindPartParams->partName;
        part->attributes = part->mKindPartParams->attributes;
        part->counterAttributes = part->mKindPartParams->counterAttributes;
        part->defense = part->mKindPartParams->defense;
        part->defenseAttr = part->mKindPartParams->defenseAttr;
        part->mPositionOffset = part->mKindPartParams->positionOffset;
        BtlUnit_SetPartsRotate(part, 0.0f, 0.0f, 0.0f);
        BtlUnit_SetPartsRotateOffset(part, 0.0f, 0.0f, 0.0f);
        part->mHitBaseOffset.x = part->mKindPartParams->hitBaseOffset.x;
        part->mHitBaseOffset.y = part->mKindPartParams->hitBaseOffset.y;
        part->mHitOffset.x = 0.0f;
        part->mHitOffset.y = 0.0f;
        part->field_0x1A0 = 0;
    }
    btlDispEntAnime(unit);
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_ChangeParty) {
    BattleUnit* unit;
    s32 index, value;

    value = -1;
    index = *event->args;
    unit = BattleChangeParty(_battleWorkPointer);
    if (unit) {
        value = unit->unitId;
    }
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to set current)
// NONMATCHING (98.63%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_SetPos) {
    f32 x, y, z, currX, currY, currZ;
    s32 indexX, indexY, indexZ, id;
    s32* args = event->args;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    indexX = args[1];
    x = (f32)evtGetValue(event, indexX);
    indexY = args[2];
    y = (f32)evtGetValue(event, indexY);
    indexZ = args[3];
    z = (f32)evtGetValue(event, indexZ);

    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetPos(unit, &currX, &currY, &currZ);
    if (indexX == EVTDAT_POINTER_MAX) {
        x = currX;
    }
    if (indexY == EVTDAT_POINTER_MAX) {
        y = currY;
    }
    if (indexZ == EVTDAT_POINTER_MAX) {
        z = currZ;
    }
    BtlUnit_SetPos(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddPos) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_AddPos(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32 indexX, s32 indexY, s32 indexZ (can be EVTDAT_POINTER_MAX for noret)
USER_FUNC(btlevtcmd_GetPartsPos) {
    s32* args = event->args;
    s32 id, partId;
    s32 indexX, indexY, indexZ;
    f32 currX, currY, currZ;
    BattleUnitPart* part;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id);
    part = BattleGetUnitPartsPtr(id, partId);
    indexX = args[2];
    indexY = args[3];
    indexZ = args[4];
    BtlUnit_GetPartsWorldPos(part, &currX, &currY, &currZ);
    if (indexX != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexX, (s32)currX);
    }
    if (indexY != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexY, (s32)currY);
    }
    if (indexZ != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexZ, (s32)currZ);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
// NONMATCHING (96.59%): 3 missing, plus register differences.
USER_FUNC(btlevtcmd_SetPartsPos) {
    s32* args = event->args;
    f32 x, y, z, currX, currY, currZ;
    BattleUnitPart* part;
    BattleUnit* unit;
    s32 id, partId;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BattleGetUnitPartsPtr(id, partId);
    if (part->attributes & 0x10000000) {
        BtlUnit_SetPartsPos(part, x, y, z);
    }
    else {
        BtlUnit_GetPos(unit, &currX, &currY, &currZ);
        BtlUnit_SetPartsOffsetPos(part, x - currX, y - currY, z - currZ);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddPartsPos) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    if (part->attributes & 0x10000000) {
        BtlUnit_AddPartsPos(part, x, y, z);
    }
    else {
        BtlUnit_AddPartsOffsetPos(part, x, y, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetDispOffset) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_SetDispOffset(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetPartsDispOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    BattleUnit* unit;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id); //unused variable
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetPartsDispOffset(part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddPartsDispOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    BattleUnit* unit;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id); //unused variable
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_AddPartsDispOffset(part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetTogeOffset) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    if (x != (f32)EVTDAT_POINTER_MAX) {
        unit->togeOffset.x = x;
    }
    if (y != (f32)EVTDAT_POINTER_MAX) {
        unit->togeOffset.y = y;
    }
    if (z != (f32)EVTDAT_POINTER_MAX) {
        unit->togeOffset.z = z;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 indexX, s32 indexY, s32 indexZ
USER_FUNC(btlevtcmd_GetHomePos) {
    s32* args = event->args;
    s32 indexX, indexY, indexZ;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    indexX = args[1];
    indexY = args[2];
    indexZ = args[3];
    BtlUnit_GetHomePos(unit, &x, &y, &z);
    evtSetValue(event, indexX, (s32)x);
    evtSetValue(event, indexY, (s32)y);
    evtSetValue(event, indexZ, (s32)z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetHomePos) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_SetHomePos(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddHomePos) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_AddHomePos(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
// NONMATCHING (17.61%): 9 instructions differ, 10 missing, 31 extra, plus register differences.
USER_FUNC(btlevtcmd_SetPartsHomePos) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetPartsHomePos(part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32 indexX, s32 indexY, s32 indexZ
// NONMATCHING (99.44%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_GetHitPos) {
    s32* args = event->args;
    s32 indexX, indexY, indexZ;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BattleGetUnitPartsPtr(id, partId);
    indexX = args[2];
    indexY = args[3];
    indexZ = args[4];
    BtlUnit_GetHitPos(unit, part, &x, &y, &z);
    if (indexX != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexX, (s32)x);
    }
    if (indexY != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexY, (s32)y);
    }
    if (indexZ != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexZ, (s32)z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
// NONMATCHING (17.13%): 7 instructions differ, 13 missing, 34 extra, plus register differences.
USER_FUNC(btlevtcmd_SetHitOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    BattleUnit* unit;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetHitOffset(unit, part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
// NONMATCHING (17.13%): 7 instructions differ, 13 missing, 34 extra, plus register differences.
USER_FUNC(btlevtcmd_SetHitCursorOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    BattleUnit* unit;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetHitCursorOffset(unit, part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
// NONMATCHING (83.18%): 2 instructions differ, 2 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_GetWidth) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, index, width;

    btlId = evtGetValue(event, args[0]);
    index = args[1];
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    width = BtlUnit_GetWidth(unit);
    evtSetValue(event, index, width);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
// NONMATCHING (83.18%): 2 instructions differ, 2 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_GetHeight) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, index, height;

    btlId = evtGetValue(event, args[0]);
    index = args[1];
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    height = BtlUnit_GetHeight(unit);
    evtSetValue(event, index, height);
    return EVT_RETURN_DONE;
}

//s32* battleId, s16* height
// NONMATCHING (39.39%): 1 instruction differs, 8 missing, 8 extra, plus register differences.
USER_FUNC(btlevtcmd_SetHeight) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id;
    s16 height;

    btlId = evtGetValue(event, args[0]);
    height = (s16)evtGetValue(event, args[1]);
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_SetHeight(unit, height); //TODO: make sure extsh
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
// NONMATCHING (88.67%): 1 instruction differs, 1 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_GetStatusMg) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, index;

    btlId = evtGetValue(event, args[0]);
    index = args[1];
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetFloat(event, index, unit->mSizeMultiplier);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* xpos, s32* ypos (can be EVTDAT_POINTER_MAX to not store)
// NONMATCHING (91.66%): 1 instruction differs, 1 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_SetStatusIconOffset) {
    s32* args = event->args;
    s32 id, btlId, xpos, ypos;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    xpos = evtGetValue(event, args[1]);
    ypos = evtGetValue(event, args[2]);
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (xpos != EVTDAT_POINTER_MAX) {
        unit->mStatusIconOffset[0] = (s16)xpos;
    }
    if (ypos != EVTDAT_POINTER_MAX) {
        unit->mStatusIconOffset[1] = (s16)ypos;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* xpos, s32* ypos (can be EVTDAT_POINTER_MAX to not store)
// NONMATCHING (91.66%): 1 instruction differs, 1 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_SetHpGaugeOffset) {
    s32* args = event->args;
    s32 id, btlId, xpos, ypos;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    xpos = evtGetValue(event, args[1]);
    ypos = evtGetValue(event, args[2]);
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (xpos != EVTDAT_POINTER_MAX) {
        unit->mHpGaugeOffset[0] = (s16)xpos;
    }
    if (ypos != EVTDAT_POINTER_MAX) {
        unit->mHpGaugeOffset[1] = (s16)ypos;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retX, s32 retY
// NONMATCHING (67.34%): 3 instructions differ, 8 missing, 2 extra, plus register differences.
USER_FUNC(btlevtcmd_GetHpGaugeOffset) {
    s32* args = event->args;
    s32 id, btlId, indexX, indexY;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    indexX = args[1];
    indexY = args[2];
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, indexX, unit->mHpGaugeOffset[0]);
    evtSetValue(event, indexY, unit->mHpGaugeOffset[1]);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, u8* alpha
// NONMATCHING (97.41%): 1 instruction differs, plus register differences.
USER_FUNC(btlevtcmd_SetAlpha) {
    s32* args = event->args;
    s32 id, btlId, partId;
    u8 alpha;

    btlId = evtGetValue(event, args[0]);
    partId = evtGetValue(event, args[1]);
    alpha = (u8)evtGetValue(event, args[2]);
    id = BattleTransID(event, btlId);
    BattleGetUnitPartsPtr(id, partId)->color.a = alpha;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, u8* redIndex, u8* greenIndex, u8* blueIndex
// NONMATCHING (98.12%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_GetRGB) {
    s32* args = event->args;
    s32 indexR, indexG, indexB;
    s32 btlId, partId;
    BattleUnitPart* part;

    btlId = evtGetValue(event, args[0]);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(BattleTransID(event, btlId), partId);
    indexR = args[2];
    indexG = args[3];
    indexB = args[4];
    evtSetValue(event, indexR, part->color.r);
    evtSetValue(event, indexG, part->color.g);
    evtSetValue(event, indexB, part->color.b);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, u8* red, u8* green, u8* blue
// NONMATCHING (89.87%): 2 instructions differ, 1 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_SetRGB) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, btlId, partId;
    u8 r, g, b;

    btlId = evtGetValue(event, args[0]);
    partId = evtGetValue(event, args[1]);
    r = (u8)evtGetValue(event, args[2]);
    g = (u8)evtGetValue(event, args[3]);
    b = (u8)evtGetValue(event, args[4]);
    id = BattleTransID(event, btlId);
    part = BattleGetUnitPartsPtr(id, partId);
    part->color.r = r;
    part->color.g = g;
    part->color.b = b;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to skip)
USER_FUNC(btlevtcmd_GetRotate) {
    s32* args = event->args;
    f32 rotateX, rotateY, rotateZ;
    s32 indexX, indexY, indexZ;
    BattleUnit* unit;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    indexX = args[1];
    indexY = args[2];
    indexZ = args[3];
    BtlUnit_GetRotate(unit, &rotateX, &rotateY, &rotateZ);
    if (indexX != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexX, (s32)rotateX);
    }
    if (indexY != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexY, (s32)rotateY);
    }
    if (indexZ != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexZ, (s32)rotateZ);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to skip)
// NONMATCHING (98.80%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_SetRotate) {
    s32* args = event->args;
    s32 id, indexX, indexY, indexZ;
    f32 x, y, z, currX, currY, currZ;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    indexX = args[1];
    x = (f32)evtGetValue(event, indexX);
    indexY = args[2];
    y = (f32)evtGetValue(event, indexY);
    indexZ = args[3];
    z = (f32)evtGetValue(event, indexZ);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetRotate(unit, &currX, &currY, &currZ);
    if (indexX == EVTDAT_POINTER_MAX) {
        x = currX;
    }
    if (indexY == EVTDAT_POINTER_MAX) {
        y = currY;
    }
    if (indexZ == EVTDAT_POINTER_MAX) {
        z = currZ;
    }
    BtlUnit_SetRotate(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddRotate) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_AddRotate(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_GetPartsRotate) {
    s32* args = event->args;
    s32 id, partId, indexX, indexY, indexZ;
    BattleUnitPart* part;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    indexX = args[2];
    indexY = args[3];
    indexZ = args[4];
    BtlUnit_GetPartsRotate(part, &x, &y, &z);
    evtSetValue(event, indexX, (s32)x);
    evtSetValue(event, indexY, (s32)y);
    evtSetValue(event, indexZ, (s32)z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetPartsRotate) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId, x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetValue(event, args[2]);
    y = evtGetValue(event, args[3]);
    z = evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetPartsRotate(part, (f32)x, (f32)y, (f32)z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddPartsRotate) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId, x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetValue(event, args[2]);
    y = evtGetValue(event, args[3]);
    z = evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_AddPartsRotate(part, (f32)x, (f32)y, (f32)z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to skip)
// NONMATCHING (98.80%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_SetBaseRotate) {
    s32* args = event->args;
    s32 id, indexX, indexY, indexZ;
    f32 x, y, z, currX, currY, currZ;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    indexX = args[1];
    x = (f32)evtGetValue(event, indexX);
    indexY = args[2];
    y = (f32)evtGetValue(event, indexY);
    indexZ = args[3];
    z = (f32)evtGetValue(event, indexZ);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetBaseRotate(unit, &currX, &currY, &currZ);
    if (indexX == EVTDAT_POINTER_MAX) {
        x = currX;
    }
    if (indexY == EVTDAT_POINTER_MAX) {
        y = currY;
    }
    if (indexZ == EVTDAT_POINTER_MAX) {
        z = currZ;
    }
    BtlUnit_SetBaseRotate(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to skip)
USER_FUNC(btlevtcmd_SetPartsBaseRotate) {
    s32* args = event->args;
    BattleUnitPart* part;
    f32 currX, currY, currZ;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_GetPartsBaseRotate(part, &currX, &currY, &currZ);
    if (x != (f32)EVTDAT_POINTER_MAX) {
        currX = x;
    }
    if (y != (f32)EVTDAT_POINTER_MAX) {
        currY = y;
    }
    if (z != (f32)EVTDAT_POINTER_MAX) {
        currZ = z;
    }
    BtlUnit_SetPartsBaseRotate(part, currX, currY, currZ);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetRotateOffset) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_SetRotateOffset(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetPartsRotateOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId, x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetValue(event, args[2]);
    y = evtGetValue(event, args[3]);
    z = evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetPartsRotateOffset(part, (f32)x, (f32)y, (f32)z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddPartsRotateOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId, x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetValue(event, args[2]);
    y = evtGetValue(event, args[3]);
    z = evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_AddPartsRotateOffset(part, (f32)x, (f32)y, (f32)z);
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_SetRotateOffsetFromCenterOffset) {
    BattleUnit* unit;
    s32 btlId;

    btlId = evtGetValue(event, *event->args);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    BtlUnit_SetRotateOffset(unit, unit->offsetCenter.x, unit->offsetCenter.y, unit->offsetCenter.z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetCutBaseOffset) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->cutBaseOffset.x = x;
    unit->cutBaseOffset.y = y;
    unit->cutBaseOffset.z = z;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetBintaHitOffset) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->bintaHitOffset.x = x;
    unit->bintaHitOffset.y = y;
    unit->bintaHitOffset.z = z;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetKissHitOffset) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->kissHitOffset.x = x;
    unit->kissHitOffset.y = y;
    unit->kissHitOffset.z = z;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* value
// NONMATCHING (52.29%): 6 missing, 10 extra, plus register differences.
USER_FUNC(btlevtcmd_SetCutWidth) {
    s32* args = event->args;
    f32 value;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    value = (f32)evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id)->cutWidth = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* value
// NONMATCHING (52.29%): 6 missing, 10 extra, plus register differences.
USER_FUNC(btlevtcmd_SetCutHeight) {
    s32* args = event->args;
    f32 value;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    value = (f32)evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id)->cutHeight = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to skip)
// NONMATCHING (94.93%): 1 instruction differs, 2 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_SetPossessionItemOffset) {
    s32* args = event->args;
    s32 id, x, y, z, indexX, indexY, indexZ;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    indexX = args[1];
    x = evtGetValue(event, indexX);
    indexY = args[2];
    y = evtGetValue(event, indexY);
    indexZ = args[3];
    z = evtGetValue(event, indexZ);
    if (indexX != EVTDAT_POINTER_MAX) {
        unit->heldItemOffset.x = (f32)x;
    }
    if (indexY != EVTDAT_POINTER_MAX) {
        unit->heldItemOffset.y = (f32)y;
    }
    if (indexZ != EVTDAT_POINTER_MAX) {
        unit->heldItemOffset.z = (f32)z;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetBaseScale) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    if (unit) {
        BtlUnit_SetBaseScale(unit, x, y, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
// NONMATCHING (89.81%): 1 instruction differs, 4 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_SetPartsBaseScale) {
    s32* args = event->args;
    BattleUnitPart* part;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id != -1) { //TODO: un-hardcode?
        part = BattleGetUnitPartsPtr(id, evtGetValue(event, args[1]));
        x = evtGetFloat(event, args[2]);
        y = evtGetFloat(event, args[3]);
        z = evtGetFloat(event, args[4]);
        BtlUnit_SetPartsBaseScale(part, x, y, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to skip)
USER_FUNC(btlevtcmd_GetScale) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 id, indexX, indexY, indexZ;
    BattleUnit* unit;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id == -1) {
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    indexX = args[1];
    indexY = args[2];
    indexZ = args[3];
    BtlUnit_GetScale(unit, &x, &y, &z);
    if (indexX != EVTDAT_POINTER_MAX) {
        evtSetFloat(event, indexX, x);
    }
    if (indexY != EVTDAT_POINTER_MAX) {
        evtSetFloat(event, indexY, y);
    }
    if (indexZ != EVTDAT_POINTER_MAX) {
        evtSetFloat(event, indexZ, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
// NONMATCHING (86.88%): 3 instructions differ, 4 missing, 1 extra.
USER_FUNC(btlevtcmd_SetScale) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id != -1) { //TODO: un-hardcode?
        unit = BattleGetUnitPtr(_battleWorkPointer, id);
        x = evtGetFloat(event, args[1]);
        y = evtGetFloat(event, args[2]);
        z = evtGetFloat(event, args[3]);
        BtlUnit_SetScale(unit, x, y, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
// NONMATCHING (86.88%): 3 instructions differ, 4 missing, 1 extra.
USER_FUNC(btlevtcmd_AddScale) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id != -1) { //TODO: un-hardcode?
        unit = BattleGetUnitPtr(_battleWorkPointer, id);
        x = evtGetFloat(event, args[1]);
        y = evtGetFloat(event, args[2]);
        z = evtGetFloat(event, args[3]);
        BtlUnit_AddScale(unit, x, y, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
// NONMATCHING (92.12%): 1 instruction differs, 3 missing, 1 extra.
USER_FUNC(btlevtcmd_SetPartsScale) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id != -1) { //TODO: un-hardcode?
        partId = evtGetValue(event, args[1]);
        part = BattleGetUnitPartsPtr(id, partId);
        x = evtGetFloat(event, args[2]);
        y = evtGetFloat(event, args[3]);
        z = evtGetFloat(event, args[4]);
        BtlUnit_SetPartsScale(part, x, y, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
// NONMATCHING (92.12%): 1 instruction differs, 3 missing, 1 extra.
USER_FUNC(btlevtcmd_AddPartsScale) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id != -1) { //TODO: un-hardcode?
        partId = evtGetValue(event, args[1]);
        part = BattleGetUnitPartsPtr(id, partId);
        x = evtGetFloat(event, args[2]);
        y = evtGetFloat(event, args[3]);
        z = evtGetFloat(event, args[4]);
        BtlUnit_AddPartsScale(part, x, y, z);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex
USER_FUNC(btlevtcmd_GetHp) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtSetValue(event, args[1], unit->currentHp);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex
USER_FUNC(btlevtcmd_GetFp) { //1:1
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id, index;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    index = args[1];
    evtSetValue(event, index, BtlUnit_GetFp(unit));
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex
USER_FUNC(btlevtcmd_GetMaxHp) { //1:1
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtSetValue(event, args[1], unit->maxHp);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetMaxFp) { //1:1
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id, index;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    index = args[1];
    evtSetValue(event, index, BtlUnit_GetMaxFp(unit));
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetHp) { //1:1
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    f32 value; //TODO: rename
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    unit->currentHp = (s16)evtGetValue(event, args[1]);
    unit->hpGauge.field_0xC = unit->currentHp;
    unit->hpGauge.field_0x10 = unit->currentHp;
    value = (f32)unit->currentHp / (f32)unit->maxHp;
    unit->hpGauge.field_0x18 = value;
    unit->hpGauge.field_0x14 = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetFp) { //1:1
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id, value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    value = evtGetValue(event, args[1]);
    BtlUnit_SetFp(unit, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetMaxFp) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id, value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    value = evtGetValue(event, args[1]);
    BtlUnit_SetMaxFp(unit, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetSwallowParam) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, value;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    value = evtGetValue(event, args[1]);
    if (unit) {
        unit->swallowRate = value;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetSwallowAttribute) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId;
    u8 value;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    value = (u8)evtGetValue(event, args[1]);
    if (unit) {
        unit->swallowAttribute = value;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetMaxMoveCount) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    unit->maxMoveCount = (s8)evtGetValue(event, args[1]);
    if (unit->movesRemaining > unit->maxMoveCount) {
        unit->movesRemaining = unit->maxMoveCount;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetDamagePartsId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtSetValue(event, args[1], unit->currentTarget->mKindPartParams->partNum);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetHpDamage) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtSetValue(event, args[1], unit->hpDamageTaken);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetFpDamage) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtSetValue(event, args[1], unit->fpDamageTaken);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetHpDamageCount) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) { //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtSetValue(event, args[1], unit->hpDamageCount);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_RecoverHp) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id, hp;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) {  //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtGetValue(event, args[1]); //unused, debug?
    hp = evtGetValue(event, args[2]);
    if (!(unit->mAttributeFlags & kUnk_ImmageToOHKO)) {
        unit->hpGauge.field_0xC = unit->currentHp;
        BtlUnit_RecoverHp(unit, hp);
        unit->hpGauge.field_0x10 = unit->currentHp;
        unit->hpGauge.field_0x4 = unit->hpGauge.field_0x6;
        unit->hpGauge.field_0x8 = unit->hpGauge.field_0xA;
        unit->hpGauge.field_0x2 |= 1;
    }
    else {
        BtlUnit_RecoverHp(unit, hp);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_RecoverFp) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, id, value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    if (id == -1) {  //TODO: un-hardcode?
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    evtGetValue(event, args[1]); //unused, debug?
    value = evtGetValue(event, args[2]);
    BtlUnit_RecoverFp(unit, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetOverTurnCount) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, index;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, index, unit->flippedTurns);
    return EVT_RETURN_DONE;
}

// NONMATCHING (96.11%): 1 instruction differs, plus register differences.
USER_FUNC(btlevtcmd_SetOverTurnCount) {
    s32* args = event->args;
    s32 btlId, id;
    s8 value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    value = (s8)evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id)->flippedTurns = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetUnitWork) {
    s32* args = event->args;
    s32 id, i, index;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    i = evtGetValue(event, args[1]);
    index = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, index, unit->work[i]);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetUnitWork) {
    s32* args = event->args;
    s32 btlId, id, value, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = evtGetValue(event, args[1]);
    value = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->work[index] = value;
    return EVT_RETURN_DONE;
}

// NONMATCHING (82.29%): 3 missing, 3 extra, plus register differences.
USER_FUNC(btlevtcmd_AddUnitWork) {
    s32* args = event->args;
    s32 btlId, id, value, index;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = evtGetValue(event, args[1]);
    value = evtGetValue(event, args[2]);
    BattleGetUnitPtr(_battleWorkPointer, id)->work[index] += value;
    return EVT_RETURN_DONE;
}

// NONMATCHING (98.82%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_GetUnitWorkFloat) {
    s32* args = event->args;
    s32 btlId, id, i, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    i = evtGetValue(event, args[1]);
    index = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetFloat(event, index, *(f32*)&unit->work[i]);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetUnitWorkFloat) {
    s32* args = event->args;
    s32 btlId, id, index;
    f32 value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = evtGetValue(event, args[1]);
    value = evtGetFloat(event, args[2]);
    *(f32*)&BattleGetUnitPtr(_battleWorkPointer, id)->work[index] = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetPartsWork) {
    s32* args = event->args;
    s32 id, partId, i, index;
    BattleUnitPart* part;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    i = evtGetValue(event, args[2]);
    index = args[3];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    evtSetValue(event, index, part->work[i]);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetPartsWork) {
    s32* args = event->args;
    s32 btlId, id, partId, index, value;
    BattleUnit* unit;
    BattleUnitPart* part;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    index = evtGetValue(event, args[2]);
    value = evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->work[index] = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetDamage) {
    s32* args = event->args;
    s32 btlId, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    index = args[1];
    if (unit) {
        evtSetValue(event, index, BtlUnit_GetHitDamage(unit));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetTotalDamage) {
    s32* args = event->args;
    s32 btlId, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    index = args[1];
    if (unit) {
        evtSetValue(event, index, BtlUnit_GetTotalHitDamage(unit));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetDamageCode) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, index;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, index, (s32)(u8)unit->damageCode); //TODO: double check this cast
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CheckDamageCode) {
    s32* args = event->args;
    s32 btlId, id, mask, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    mask = args[1];
    index = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if ((unit->damageCode & mask)) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (82.76%): 2 instructions differ, 4 missing, 5 extra, plus register differences.
USER_FUNC(btlevtcmd_StartAvoid) {
    s32* args = event->args;
    s32 btlId, id, damageCode;
    NiceEffectWork* effect;
    BattleUnit* unit;
    f32 x, y, z;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    damageCode = args[1] | 0x100;
    if ((u8)damageCode == 0x26) {
        BtlUnit_GetPos(unit, &x, &y, &z);
        if (y >= 85.0f) {
            y += -50.0f;
        }
        else {
            y += 50.0f;
        }
        x -= 45.0f;
        effect = effNiceEntry(6, x, y, z)->userdata;
        effect->field_0x1C = 0.75f;
    }
    unit->damagePattern = 0;
    BattleRunHitEvent(unit, damageCode);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_RunHitEventDirect) {
    s32* args = event->args;
    s32 btlId;
    BattleUnit* unit;
    s32 damageCode;
    void* code;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    damageCode = args[1];
    code = (void*)evtGetValue(event, args[2]);
    unit->damagePattern = evtGetValue(event, args[3]);
    BattleRunHitEventDirect(unit, damageCode | 0x100, code);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CheckDamagePattern) {
    s32* args = event->args;
    s32 btlId, id, index;
    u32 pattern;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    pattern = args[1];
    index = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (unit->damagePattern == pattern) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (83.92%): 1 instruction differs, 8 missing, 8 extra, plus register differences.
USER_FUNC(btlevtcmd_CheckSpace) { //TODO: probably wrong
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 boundX, boundY, boundZ;
    s32 baseX, baseY, baseZ;
    BattleUnit* unit;
    s32 index, i;
    f32 x, y, z;
    BOOL found;

    index = args[0];
    baseX = evtGetValue(event, args[1]);
    baseY = evtGetValue(event, args[2]);
    baseZ = evtGetValue(event, args[3]);
    boundX = evtGetValue(event, args[4]);
    boundY = evtGetValue(event, args[5]);
    boundZ = evtGetValue(event, args[6]);
    for (i = 0; i < 64; ++i) {
        found = FALSE;
        unit = BattleGetUnitPtr(wp, i);
        if (unit && UNIT_ALLIANCE_PARTY) {
            BtlUnit_GetHomePos(unit, &x, &y, &z);
            if (boundX) {
                if (abs((s32)x - baseX) < boundX) {
                    found = TRUE;
                }
            }
            if (boundY) {
                if (abs((s32)y - baseY) < boundY) {
                    found = TRUE;
                }
            }
            if (boundZ) {
                if (abs((s32)z - baseZ) < boundZ) {
                    found = TRUE;
                }
            }
            if (found) {
                evtSetValue(event, index, i);
                return EVT_RETURN_DONE;
            }
        }
    }
    evtSetValue(event, index, -1);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetEnemyBelong) {
    s32* args = event->args;
    s32 btlId, id, index, value;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    value = BtlUnit_GetEnemyBelong(unit);
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetFriendBelong) {
    s32* args = event->args;
    s32 btlId, id, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, index, unit->mAlliance);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetProtectId) {
    s32* args = event->args;
    s32 btlId, id, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, index, unit->protectId);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetUnitKind) {
    s32* args = event->args;
    s32 btlId, id, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, index, unit->currentKind);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetPartnerId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit, * partner;
    s32 btlId, id, index;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(wp, id);
    partner = BattleGetPartnerPtr(wp, unit);
    if (partner) {
        evtSetValue(event, index, partner->unitId);
    }
    else {
        evtSetValue(event, index, -1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetMarioId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, id, index, alliance;
    BattleUnit* unit;
    int i;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    alliance = BattleGetUnitPtr(wp, id)->mAlliance;
    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(wp, i);
        if (unit && unit->mAlliance == alliance && unit->currentKind == UNIT_MARIO) {
            break;
        }
    }
    if (i >= 64) {
        evtSetValue(event, index, -1);
    }
    else {
        evtSetValue(event, index, unit->unitId);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetPartyId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, id, index, alliance, kind;
    BattleUnit* unit;
    int i;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    alliance = BattleGetUnitPtr(wp, id)->mAlliance;
    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(wp, i);
        if (unit && unit->mAlliance == alliance) {
            kind = unit->currentKind;
            if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
                break;
            }
        }
    }
    if (i >= 64) {
        evtSetValue(event, index, -1);
    }
    else {
        evtSetValue(event, index, unit->unitId);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetBodyId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, id, index, value;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(wp, id);
    value = BtlUnit_GetBodyPartsId(unit);
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetUnitId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, id, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(wp, id);
    if (unit && (unit->flags & 0x80000000)) {
        unit = NULL;
    }
    if (unit) {
        evtSetValue(event, index, id);
    }
    else {
        evtSetValue(event, index, -1);
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (96.71%): 2 instructions differ, plus register differences.
USER_FUNC(btlevtcmd_GetPartyTechLv) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, id, index, value, i;
    BattleUnitType kind;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    kind = BattleGetUnitPtr(wp, id)->currentKind;
    value = 0;
    if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        i = BattleTransPartyId(kind);
        value = pouchGetPtr()->partyData[i].techLevel;
    }
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

// NONMATCHING (91.89%): 4 instructions differ, 4 missing, 7 extra, plus register differences.
USER_FUNC(btlevtcmd_CalculateFaceDirection) {
    BattleUnitPart* thisPart = NULL;
    BattleUnitPart* targetPart = NULL;
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, flags, index, value;
    s32 thisId, thisPartId;
    s32 targetId, targetPartId;
    BattleUnit *targetUnit, *thisUnit;
    f32 calculate;
    f32 thisX, thisY, thisZ;
    f32 targetX, targetY, targetZ;
    f32 offsetX, offsetY, offsetZ;
    f32 localX, localY, localZ;
    f32 thisOffsetX, thisOffsetY, thisOffsetZ;

    btlId = evtGetValue(event, args[0]);
    thisId = BattleTransID(event, btlId);
    thisPartId = evtGetValue(event, args[1]);
    targetId = evtGetValue(event, args[2]);
    targetPartId = evtGetValue(event, args[3]);
    flags = args[4];
    index = args[5];

    if (!(flags & 9)) {
        targetId = BattleTransID(event, targetId);
    }

    thisUnit = BattleGetUnitPtr(_battleWorkPointer, thisId);
    if (thisPartId != -1) {
        thisPart = BtlUnit_GetPartsPtr(thisUnit, thisPartId);
    }

    if (flags & 0x10) {
        targetUnit = BattleGetUnitPtr(_battleWorkPointer, targetId);
        if (targetPartId != -1) {
            targetPart = BtlUnit_GetPartsPtr(targetUnit, targetPartId);
        }
    }

    value = thisUnit->faceDirection;
    if (thisPart) {
        value = thisPart->faceDirection;
    }

    if (flags & 4) {
        value = wp->alliances[thisUnit->mAlliance].attackDirection;
    }

    if (flags & 8) {
        value = thisUnit->moveDirection;
        if (thisPart) {
            value = thisPart->moveDirection;
        }
    }

    if (flags & 0x10) {
        BtlUnit_GetPos(thisUnit, &thisX, &thisY, &thisZ);
        if (thisPart) {
            if (thisPart->attributes & 0x10000000) {
                BtlUnit_GetPartsPos(thisPart, &thisX, &thisY, &thisZ);
            }
            else {
                BtlUnit_GetPartsOffsetPos(thisPart, &offsetX, &offsetY, &offsetZ);
                thisX += offsetX;
            }
        }

        BtlUnit_GetPos(targetUnit, &targetX, &targetY, &targetZ);
        if (targetPart) {
            if (targetPart->attributes & 0x10000000) {
                BtlUnit_GetPartsPos(targetPart, &targetX, &targetY, &targetZ);
            }
            else {
                BtlUnit_GetPartsOffsetPos(targetPart, &offsetX, &offsetY, &offsetZ);
                targetX += offsetX;
            }
        }

        calculate = targetX - thisX;
        if (calculate <= 0.0f) {
            if (calculate < 0.0f) {
                value = -1;
            }
        }
        else {
            value = 1;
        }
    }

    if (flags & 1) {
        BtlUnit_GetPos(thisUnit, &localX, &localY, &localZ);
        if (thisPart) {
            if (thisPart->attributes & 0x10000000) {
                BtlUnit_GetPartsPos(thisPart, &localX, &localY, &localZ);
            }
            else {
                BtlUnit_GetPartsOffsetPos(thisPart, &thisOffsetX, &thisOffsetY, &thisOffsetZ);
                localX += thisOffsetX;
            }
        }
        calculate = (f32)targetId - localX;
        if (calculate <= 0.0f) {
            if (calculate < 0.0f) {
                value = -1;
            }
        }
        else {
            value = 1;
        }
    }
    if (flags & 0x100) {
        value = -value;
    }
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetFaceDirection) {
    s32* args = event->args;
    s32 btlId, id, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, index, unit->faceDirection);
    return EVT_RETURN_DONE;
}

// NONMATCHING (96.11%): 1 instruction differs, plus register differences.
USER_FUNC(btlevtcmd_ChangeFaceDirection) {
    s32* args = event->args;
    s32 btlId, id;
    s8 value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    value = (s8)evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id)->faceDirection = value;
    return EVT_RETURN_DONE;
}

// NONMATCHING (96.55%): 1 instruction differs, plus register differences.
USER_FUNC(btlevtcmd_ChangePartsFaceDirection) {
    s32* args = event->args;
    s32 btlId, id, partId;
    s8 value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    value = (s8)evtGetValue(event, args[2]);
    BattleGetUnitPartsPtr(id, partId)->faceDirection = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_ResetFaceDirection) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId;

    btlId = evtGetValue(event, *event->args);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    unit->faceDirection = wp->alliances[unit->mAlliance].attackDirection;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CheckAttribute) {
    s32* args = event->args;
    s32 btlId, id, mask, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    mask = args[1];
    index = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (unit->mAttributeFlags & mask) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_OnAttribute) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, mask;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    mask = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->mAttributeFlags |= mask;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_OffAttribute) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, mask;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    mask = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->mAttributeFlags = unit->mAttributeFlags & ~mask;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CheckPartsAttribute) {
    s32* args = event->args;
    s32 id, partId, index, mask;
    BattleUnit* unit;
    BattleUnitPart* part;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    mask = args[2];
    index = args[3];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    if (part->attributes & mask) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_OnPartsAttribute) {
    s32* args = event->args;
    s32 id, partId, mask;
    BattleUnitPart* part;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    mask = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->attributes |= mask;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_OffPartsAttribute) {
    s32* args = event->args;
    s32 id, partId, mask;
    BattleUnitPart* part;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    mask = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->attributes = part->attributes & ~mask;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CheckPartsCounterAttribute) {
    s32* args = event->args;
    s32 id, partId, index, mask;
    BattleUnit* unit;
    BattleUnitPart* part;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    mask = args[2];
    index = args[3];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    if (part->counterAttributes & mask) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_OnPartsCounterAttribute) {
    s32* args = event->args;
    s32 id, partId, mask;
    BattleUnitPart* part;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    mask = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->counterAttributes |= mask;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_OffPartsCounterAttribute) {
    s32* args = event->args;
    s32 id, partId, mask;
    BattleUnitPart* part;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    mask = args[2];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->counterAttributes = part->counterAttributes & ~mask;
    return EVT_RETURN_DONE;
}

// NONMATCHING (90.44%): 8 instructions differ, plus register differences.
USER_FUNC(btlevtcmd_SetPartsBlur) {
    s32* args = event->args;
    BattleUnitPart* part;
    BattleUnit* unit;
    s8 color0, color1, color2, color3;
    s8 color4, color5, color6, color7;
    s32 btlId, id, partId;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    color0 = (s8)evtGetValue(event, args[2]);
    color1 = (s8)evtGetValue(event, args[3]);
    color2 = (s8)evtGetValue(event, args[4]);
    color3 = (s8)evtGetValue(event, args[5]);
    color4 = (s8)evtGetValue(event, args[6]);
    color5 = (s8)evtGetValue(event, args[7]);
    color6 = (s8)evtGetValue(event, args[8]);
    color7 = (s8)evtGetValue(event, args[9]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->blurWork.color[0] = color0;
    part->blurWork.color[1] = color1;
    part->blurWork.color[2] = color2;
    part->blurWork.color[3] = color3;
    part->blurWork.color[4] = color4;
    part->blurWork.color[5] = color5;
    part->blurWork.color[6] = color6;
    part->blurWork.color[7] = color7;
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.60%): only the OR operand order differs (original `or r0, r0, r4`, ours `or r0, r4, r0`);
// field and args[1] land in the right registers via `args[1] | unit->mTokenFlags` but MWCC emits the
// expression's left operand first.
USER_FUNC(btlevtcmd_OnToken) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    unit->mTokenFlags = args[1] | unit->mTokenFlags;
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.12%): the ANDC operands are swapped; original `andc r0, r0, r4`, ours `andc r0, r4, r0`
// (semantically identical, only the evaluation/register order differs).
USER_FUNC(btlevtcmd_OffToken) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    unit->mTokenFlags = unit->mTokenFlags & ~args[1];
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.70%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_CheckToken) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, index;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    index = args[2];
    if (args[1] & unit->mTokenFlags) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_SetRegistStatus) {
    s32* args = event->args;
    BattleVulnerableStatus* status;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    status = (BattleVulnerableStatus*)evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id)->vulnStatus = status;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CheckCommandUnit) {
    s32* args = event->args;
    s32 btlId, index;
    BattleUnitType kind;

    btlId = evtGetValue(event, args[0]);
    kind = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId))->currentKind;
    index = args[1];
    if (kind == UNIT_MARIO || kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}




















































//s32* battleId, s32* windowStartFrame, s32* windowEndFrame, s32* endFrame, s32* earlyFrames
USER_FUNC(btlevtcmd_ACRStart) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 id, windowStartFrame, windowEndFrame, endFrame, earlyFrames;

    if (isFirstCall) {
        id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
        windowStartFrame = evtGetValue(event, args[1]);
        windowEndFrame = evtGetValue(event, args[2]);
        endFrame = evtGetValue(event, args[3]);
        earlyFrames = evtGetValue(event, args[4]);
        BattleAcrobatStart(wp, id, windowStartFrame, windowEndFrame, endFrame, earlyFrames);
    }
    return BattleAcrobatMain(wp);
}

//s32 resultRetIndex, s32 frameRetIndex (can be EVTDAT_POINTER_MAX to skip)
USER_FUNC(btlevtcmd_ACRGetResult) {
    s32* args = event->args;
    s32 index1, index2, result, frame;

    index1 = args[0];
    index2 = args[1];
    BattleAcrobatGetResult(_battleWorkPointer, &result, &frame);
    if (index1 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index1, result);
    }
    if (index2 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index2, frame);
    }
    return EVT_RETURN_DONE;
}

//=== added by agent: local prototypes for undeclared externals ===

#define BTLW_OFF(wp, off) ((u8*)(wp) + (off))

//no params
USER_FUNC(btlevtcmd_GuardMoveEnd) {
    _battleWorkPointer->flags &= ~1;
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_WaitGuardMove) {
    return (_battleWorkPointer->flags & 1) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

//s32 flag
USER_FUNC(btlevtcmd_AcSetFlag) {
    *(s32*)BTLW_OFF(_battleWorkPointer, 0x1CC4) = *event->args;
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_StageDispellFog) {
    BattleFogForceStop();
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_SortPhaseMoveTable) {
    battleSortPhaseMoveTable(_battleWorkPointer, -1);
    return EVT_RETURN_DONE;
}

//s32 exp
USER_FUNC(btlevtcmd_StoreExp) {
    s32 exp = evtGetValue(event, *event->args);

    BattleStoreExp(_battleWorkPointer, exp);
    return EVT_RETURN_DONE;
}

//s32 coin
USER_FUNC(btlevtcmd_StoreCoin) {
    s32 coin = evtGetValue(event, *event->args);

    BattleStoreCoin(_battleWorkPointer, coin);
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_StopAC) {
    BattleActionCommandStop(_battleWorkPointer);
    BattleAcHelpSetDispType(0);
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_GetResultAC) {
    evtSetValue(event, *event->args, _battleWorkPointer->actionCommands.result);
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_GetResultCountAC) {
    evtSetValue(event, *event->args, _battleWorkPointer->actionCommands.mResultCount);
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_GetResultACDefence) {
    s32 index = *event->args;

    evtSetValue(event, index, BattleActionCommandGetDefenceResult());
    return EVT_RETURN_DONE;
}

//s32 acHelp
USER_FUNC(btlevtcmd_ACHelpSet) {
    s32 value = evtGetValue(event, *event->args);

    BattleAcHelpSetHelp(*(const char**)((u8*)value + 0x70));
    BattleAcHelpSetDispType(EVT_RETURN_DONE);
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_ResultAC) {
    BattleWork* wp = _battleWorkPointer;

    BattleActionCommandResult(wp);
    if (wp->actionCommands.result & 1) {
        return EVT_RETURN_BLOCK;
    }
    else {
        BattleAcHelpSetDispType(0);
        return EVT_RETURN_DONE;
    }
}

//s32 retIndex
USER_FUNC(btlevtcmd_ac_timing_get_success_frame) {
    s32* args = event->args;
    s32 index = args[0];
    s32 frame = battleAcTimingA_GetSuccessFrame(_battleWorkPointer);

    evtSetValue(event, index, frame);
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_CommandGetWeaponActionLv) {
    BattleWeapon* weapon;
    s32 index = *event->args;

    weapon = BattleGetSelectWeapon(_battleWorkPointer);
    evtSetValue(event, index, *(u8*)BTLW_OFF(weapon, 0x6E));
    return EVT_RETURN_DONE;
}

//s32 retIndex, s32* weaponIndex
USER_FUNC(btlevtcmd_CommandGetWeaponItemId) {
    BattleWeapon* weapon;
    s32 index = *event->args;

    weapon = (BattleWeapon*)evtGetValue(event, event->args[1]);
    evtSetValue(event, index, *(s32*)BTLW_OFF(weapon, 0x8));
    return EVT_RETURN_DONE;
}

//s32 retIndex, s32* weaponIndex
// NONMATCHING (86.06%): 2 instructions differ, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_GetWeaponActionLv) {
    BattleWeapon* weapon = (BattleWeapon*)evtGetValue(event, *event->args);

    evtSetValue(event, event->args[1], *(u8*)BTLW_OFF(weapon, 0x6E));
    return EVT_RETURN_DONE;
}

//s32 retIndex
// NONMATCHING (96.84%): 1 instruction differs.
USER_FUNC(btlevtcmd_AcGetDifficulty) {
    s32 index = *event->args;
    s32 diff = BattleActionCommandGetDifficulty(_battleWorkPointer);

    evtSetValue(event, index, diff);
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_AttackDeclareAll) {
    BattleWork* wp = _battleWorkPointer;
    s32 id;

    id = evtGetValue(event, *event->args);
    BattleTransID(event, id);
    BattleAttackDeclareAll(wp);
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_PhaseEventStartDeclare) {
    s32 btlId = evtGetValue(event, *event->args);

    BattlePhaseEventStartDeclare(BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId)));
    return EVT_RETURN_DONE;
}

//no params
// NONMATCHING (81.53%): 2 instructions differ, 1 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_InviteApInfoReport) {
    BattleWork* wp = _battleWorkPointer;

    BattleAudience_ApRecoveryBuild(&wp->impendingBonuses);
    memset(&wp->impendingBonuses, 0, 0xC);
    return EVT_RETURN_DONE;
}


//s32* battleId, s32 talkBodyId
USER_FUNC(btlevtcmd_SetTalkBodyId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 id, value;

    id = evtGetValue(event, args[0]);
    value = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    *(s32*)BTLW_OFF(unit, 0x2D0) = value;
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_StopWaitEvent) {
    s32 btlId = evtGetValue(event, *event->args);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    if (unit->waitEventId) {
        evtDeleteID(unit->waitEventId);
        unit->waitEventId = 0;
    }
    return EVT_RETURN_DONE;
}

//s32* p0 ... s32* p7
USER_FUNC(btlevtcmd_AcSetParamAll) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 i;

    for (i = 0; i < 8; i++) {
        s32 v = evtGetValue(event, *args++);
        if (v != EVTDAT_POINTER_MAX) {
            *(s32*)((u8*)wp + 0x1CC8 + i * 4) = v;
        }
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_EnemyItemUseCheck) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 id, index;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    index = args[1];
    evtSetValue(event, index, (s32)BattleEnemyUseItemCheck(unit));
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* kind
// NONMATCHING (98.30%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_ChangeKind) {
    s32* args = event->args;
    s32 btlId, id, kind;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    kind = evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id)->currentKind = kind;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
// NONMATCHING (85.93%): 1 instruction differs, 2 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_GetExp) {
    s32* args = event->args;
    s32 id, index;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    index = args[1];
    evtSetValue(event, index, BtlUnit_GetExp(BattleGetUnitPtr(_battleWorkPointer, id)));
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
// NONMATCHING (85.93%): 1 instruction differs, 2 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_GetCoin) {
    s32* args = event->args;
    s32 id, index;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    index = args[1];
    evtSetValue(event, index, BtlUnit_GetCoin(BattleGetUnitPtr(_battleWorkPointer, id)));
    return EVT_RETURN_DONE;
}


// Keep _MoveSoundControlEndCore out of line: the original calls it rather than inlining.
#pragma dont_inline on
void _MoveSoundControlEndCore(void* a, u32* work) {
    if (work[6] != 0xFFFFFFFF) {
        psndSFXOff(work[6]);
        work[6] = 0xFFFFFFFF;
    }
}
#pragma dont_inline reset

void _UnitMoveSoundControlEnd(BattleUnit* unit) {
    _MoveSoundControlEndCore(unit, (u32*)((u8*)unit + 0x188));
}

void _PartsMoveSoundControlEnd(BattleUnitPart* part) {
    u32* work = (u32*)((u8*)part + 0xC0);
    _MoveSoundControlEndCore(*(void**)((u8*)part + 0x4EC), work);
}

void _UnitMoveSoundControl(BattleUnit* unit) {
    f32 x, y, z;

    BtlUnit_GetPos(unit, &x, &y, &z);
    _MoveSoundControlCore(unit, (u8*)unit + 0x188, 0, x, y, z);
}

void _PartsMoveSoundControl(BattleUnitPart* part) {
    f32 x, y, z;

    BtlUnit_GetPartsPos(part, &x, &y, &z);
    _MoveSoundControlCore(*(BattleUnit**)((u8*)part + 0x4EC), (u8*)part + 0xC0, part, x, y, z);
}

//s32 retIndex
// NONMATCHING (85.59%): 2 instructions differ, 1 missing, plus register differences.
USER_FUNC(btlevtcmd_GetConsumeItem) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 index = args[0];
    s32 idx = *(s32*)((u8*)wp + 0x1C38);
    u8* base = (u8*)wp + 0x171C + idx * 0x1C;
    s32 value = *(s32*)(base + 0x94);

    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_ClearAllStatus) {
    s32 btlId = evtGetValue(event, *event->args);

    BtlUnit_ClearStatus(BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId)));
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_StartWaitEvent) {
    s32 btlId = evtGetValue(event, *event->args);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    if (unit->waitEventCode) {
        BattleRunWaitEvent(unit);
    }
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_WaitAttackEnd) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, _battleWorkPointer->activeUnitId);

    if (!unit) {
        return EVT_RETURN_DONE;
    }
    if (!unit->attackEventId) {
        return EVT_RETURN_DONE;
    }
    if (evtCheckID(unit->attackEventId)) {
        return EVT_RETURN_BLOCK;
    }
    unit->attackEventId = 0;
    return EVT_RETURN_DONE;
}

//s32 type
USER_FUNC(btlevtcmd_InterruptStop) {
    s32 type = evtGetValue(event, *event->args);
    BattleWork* wp = _battleWorkPointer;

    if (isFirstCall) {
        switch (type) {
        case 0:
            wp->flags |= 0x10;
            break;
        case 1:
            wp->flags |= 0x20;
            break;
        }
    }
    return EVT_RETURN_DONE;
}

extern s32 subsetevt_confuse_flustered[];

//s32* battleId, s32* retIndex, s32* dir
// NONMATCHING (72.18%): 1 instruction differs, 4 missing, 5 extra, plus register differences.
USER_FUNC(btlevtcmd_FaceDirectionAdd) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, retIndex, dir;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    retIndex = args[1];
    dir = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, retIndex, evtGetValue(event, retIndex) + dir * unit->faceDirection);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex, s32* dir
// NONMATCHING (74.95%): 1 instruction differs, 4 missing, 4 extra, plus register differences.
USER_FUNC(btlevtcmd_FaceDirectionSub) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, retIndex, dir;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    retIndex = args[1];
    dir = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    evtSetValue(event, retIndex, evtGetValue(event, retIndex) - dir * unit->faceDirection);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex, s32* dir
// NONMATCHING (77.43%): 2 instructions differ, 4 missing, 4 extra, plus register differences.
USER_FUNC(btlevtcmd_MoveDirectionAdd) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, retIndex, dir, value;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    retIndex = args[1];
    dir = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (unit->moveDirection) {
        value = dir * unit->moveDirection;
    }
    else {
        value = dir * unit->faceDirection;
    }
    evtSetValue(event, retIndex, evtGetValue(event, retIndex) + value);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex
// NONMATCHING (86.46%): 2 instructions differ, 3 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_GetConfuseActEvent) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, retIndex, i;
    s32 evt;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    retIndex = args[1];
    i = 0;
    evt = 0;
    do {
        evt = (s32)BattleSetConfuseAct(_battleWorkPointer, unit);
        i++;
        if (i > 20) {
            evt = (s32)subsetevt_confuse_flustered;
        }
    } while (evt == 0);
    evtSetValue(event, retIndex, evt);
    return EVT_RETURN_DONE;
}


//s32* onoff, s32 flags
USER_FUNC(btlevtcmd_ac_timing_flag_onoff) {
    s32* args = event->args;
    s32 onoff;
    u32 flags;
    u32* p;

    onoff = evtGetValue(event, args[0]);
    flags = args[1];
    p = (u32*)((u8*)_battleWorkPointer + 0x1F50);
    if (onoff) {
        *p |= flags;
    }
    else {
        *p &= ~flags;
    }
    return EVT_RETURN_DONE;
}

//s32 mode
USER_FUNC(btlevtcmd_StatusWindowOnOff) {
    BattleWork* wp = _battleWorkPointer;

    switch (evtGetValue(event, *event->args)) {
    case 2:
        *(s32*)((u8*)wp + 0x2744) = 1;
        break;
    case 1:
        *(s32*)((u8*)wp + 0x2748) = 1;
        break;
    case 0:
        *(s32*)((u8*)wp + 0x2748) = 0;
        break;
    }
    BattleStatusWindowCheck();
    return EVT_RETURN_DONE;
}


//s32* battleId, s32 status, s32* turns, s32* strength, s32* onoff
USER_FUNC(btlevtcmd_OnOffStatus) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, turns, strength, onoff;
    s8 status;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    status = args[1];
    turns = evtGetValue(event, args[2]);
    strength = evtGetValue(event, args[3]);
    onoff = evtGetValue(event, args[4]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (onoff) {
        BtlUnit_SetStatus(unit, status, (s8)turns, (s8)strength);
    }
    else {
        BtlUnit_SetStatus(unit, status, 0, 0);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* acId, s32* gauge, s32* diff
USER_FUNC(btlevtcmd_SetupAC) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, acId, gauge, diff;

    id = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    acId = evtGetValue(event, args[1]);
    gauge = args[2];
    diff = evtGetValue(event, args[3]);
    BattleActionCommandSetup(wp, acId, unit, gauge, diff);
    return EVT_RETURN_DONE;
}


//s32* battleId, s32* a, s32* b, s32* c, s32* d, s32* e
USER_FUNC(btlevtcmd_SetRunSound) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, v1, v2, v3, v4, v5;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = evtGetValue(event, args[3]);
    v4 = evtGetValue(event, args[4]);
    v5 = evtGetValue(event, args[5]);
    *(s32*)((u8*)unit + 0x1C8) = v1;
    *(s32*)((u8*)unit + 0x1CC) = v2;
    *(s16*)((u8*)unit + 0x1D0) = v3;
    *(s16*)((u8*)unit + 0x1D2) = v4;
    *(s16*)((u8*)unit + 0x1D4) = v5;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* a, s32* b, s32* c, s32* d, s32* e
USER_FUNC(btlevtcmd_SetWalkSound) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, v1, v2, v3, v4, v5;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = evtGetValue(event, args[3]);
    v4 = evtGetValue(event, args[4]);
    v5 = evtGetValue(event, args[5]);
    *(s32*)((u8*)unit + 0x1D8) = v1;
    *(s32*)((u8*)unit + 0x1DC) = v2;
    *(s16*)((u8*)unit + 0x1E0) = v3;
    *(s16*)((u8*)unit + 0x1E2) = v4;
    *(s16*)((u8*)unit + 0x1E4) = v5;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* a, s32* b, s32* c, s32* d, s32* e
USER_FUNC(btlevtcmd_SetDiveSound) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, v1, v2, v3, v4, v5;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = evtGetValue(event, args[3]);
    v4 = evtGetValue(event, args[4]);
    v5 = evtGetValue(event, args[5]);
    *(s32*)((u8*)unit + 0x1E8) = v1;
    *(s32*)((u8*)unit + 0x1EC) = v2;
    *(s16*)((u8*)unit + 0x1F0) = v3;
    *(s16*)((u8*)unit + 0x1F2) = v4;
    *(s16*)((u8*)unit + 0x1F4) = v5;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* a, s32* b, s32* c, s32* retIndex
// NONMATCHING (84.68%): 3 missing, 4 extra, plus register differences.
USER_FUNC(btlevtcmd_snd_se) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, v1, v2, v3, v4, ret;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = evtGetValue(event, args[3]);
    v4 = args[4];
    ret = BtlUnit_snd_se(unit, (const char*)v1, v2, (s16)v3);
    if (v4 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, v4, ret);
    }
    return EVT_RETURN_DONE;
}


//s32* battleId, s32* partId, s32* a, s32* b, s32* c, s32* d, s32* e
USER_FUNC(btlevtcmd_SetPartsRunSound) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 btlId, id, partId, v1, v2, v3, v4, v5;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    v1 = evtGetValue(event, args[2]);
    v2 = evtGetValue(event, args[3]);
    v3 = evtGetValue(event, args[4]);
    v4 = evtGetValue(event, args[5]);
    v5 = evtGetValue(event, args[6]);
    *(s32*)((u8*)part + 0x100) = v1;
    *(s32*)((u8*)part + 0x104) = v2;
    *(s16*)((u8*)part + 0x108) = v3;
    *(s16*)((u8*)part + 0x10A) = v4;
    *(s16*)((u8*)part + 0x10C) = v5;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* a, s32* b, s32* c, s32* d, s32* e
USER_FUNC(btlevtcmd_SetPartsWalkSound) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 btlId, id, partId, v1, v2, v3, v4, v5;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    v1 = evtGetValue(event, args[2]);
    v2 = evtGetValue(event, args[3]);
    v3 = evtGetValue(event, args[4]);
    v4 = evtGetValue(event, args[5]);
    v5 = evtGetValue(event, args[6]);
    *(s32*)((u8*)part + 0x110) = v1;
    *(s32*)((u8*)part + 0x114) = v2;
    *(s16*)((u8*)part + 0x118) = v3;
    *(s16*)((u8*)part + 0x11A) = v4;
    *(s16*)((u8*)part + 0x11C) = v5;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* a, s32* b, s32* c, s32* d, s32* e
USER_FUNC(btlevtcmd_SetPartsDiveSound) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 btlId, id, partId, v1, v2, v3, v4, v5;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    v1 = evtGetValue(event, args[2]);
    v2 = evtGetValue(event, args[3]);
    v3 = evtGetValue(event, args[4]);
    v4 = evtGetValue(event, args[5]);
    v5 = evtGetValue(event, args[6]);
    *(s32*)((u8*)part + 0x120) = v1;
    *(s32*)((u8*)part + 0x124) = v2;
    *(s16*)((u8*)part + 0x128) = v3;
    *(s16*)((u8*)part + 0x12A) = v4;
    *(s16*)((u8*)part + 0x12C) = v5;
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.53%): `part` lands in r30 instead of r28 and `type1` in r28 instead of r30; MWCC gives the
// long-lived `part` the higher callee-saved register where the original gives it the lower one. Semantics match.
//s32* battleId, s32* partId, s32* type1, s32* type2, s32* retIndex
USER_FUNC(btlevtcmd_AnimeCmpPoseType) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId, type1, type2, retIndex;
    char* name1;
    char* name2;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    type1 = evtGetValue(event, args[2]);
    type2 = evtGetValue(event, args[3]);
    retIndex = args[4];
    name1 = BtlUnit_GetPoseNameFromType(part, type1);
    name2 = BtlUnit_GetPoseNameFromType(part, type2);
    evtSetValue(event, retIndex, strcmp(name1, name2) != 0);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* speed
USER_FUNC(btlevtcmd_SetPartsMoveSpeed) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, partId;
    f32 speed;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    speed = evtGetFloat(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_SetPartsMoveSpeed(BtlUnit_GetPartsPtr(unit, partId), speed);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* accel
USER_FUNC(btlevtcmd_SetPartsFallAccel) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, partId;
    f32 accel;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    accel = evtGetFloat(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_SetPartsFallAccel(BtlUnit_GetPartsPtr(unit, partId), accel);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32 a, f32 b, f32 c
USER_FUNC(btlevtcmd_JumpPartsSetting) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id, partId, a;
    f32 b, c;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    a = evtGetValue(event, args[2]);
    b = evtGetFloat(event, args[3]);
    c = evtGetFloat(event, args[4]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    *(s32*)((u8*)part + 0xA0) = a;
    part->mMoveSpeedXZ = b;
    part->mFallAccel = c;
    return EVT_RETURN_DONE;
}


//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* b, s32* retIndex
// NONMATCHING (99.55%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_PreCheckDamage) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* attacker;
    BattleUnit* target;
    BattleUnitPart* part;
    s32 btlId, targetId, partId, value, retIndex, a, result;

    btlId = evtGetValue(event, args[0]);
    attacker = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    targetId = evtGetValue(event, args[1]);
    target = BattleGetUnitPtr(wp, BattleTransID(event, targetId));
    partId = evtGetValue(event, args[2]);
    part = BtlUnit_GetPartsPtr(target, partId);
    value = evtGetValue(event, args[3]);
    a = args[4];
    retIndex = args[5];
    result = BattlePreCheckDamage(attacker, target, part, value, a);
    if (result < 0) {
        return 0xFF;
    }
    evtSetValue(event, retIndex, result);
    if (evtCheckID(event->eventId)) {
        return EVT_RETURN_DONE;
    }
    return 0xFF;
}

//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* retIndex
USER_FUNC(btlevtcmd_PreCheckCounter) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* attacker;
    BattleUnit* target;
    BattleUnitPart* part;
    u8 work[0x30];
    s32 btlId, targetId, partId, value, retIndex, out, result;

    out = 0;
    BattleInitCounterPreCheckWork(&work);
    btlId = evtGetValue(event, args[0]);
    attacker = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    targetId = evtGetValue(event, args[1]);
    target = BattleGetUnitPtr(wp, BattleTransID(event, targetId));
    partId = evtGetValue(event, args[2]);
    part = BtlUnit_GetPartsPtr(target, partId);
    value = evtGetValue(event, args[3]);
    retIndex = args[5];
    result = BattleCheckCounter(&work, attacker, target, part, value, &out);
    evtSetValue(event, retIndex, result);
    if (evtCheckID(event->eventId)) {
        return EVT_RETURN_DONE;
    }
    return 0xFF;
}

//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* b, s32* retIndex
// NONMATCHING (99.55%): same instructions; only register numbers or operands differ.
USER_FUNC(btlevtcmd_CheckDamage) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* attacker;
    BattleUnit* target;
    BattleUnitPart* part;
    BattleWeapon* weapon;
    s32 btlId, targetId, partId, a, retIndex, result;

    btlId = evtGetValue(event, args[0]);
    attacker = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    targetId = evtGetValue(event, args[1]);
    target = BattleGetUnitPtr(wp, BattleTransID(event, targetId));
    partId = evtGetValue(event, args[2]);
    part = BtlUnit_GetPartsPtr(target, partId);
    weapon = (BattleWeapon*)evtGetValue(event, args[3]);
    a = args[4];
    retIndex = args[5];
    result = BattleCheckDamage(attacker, target, part, weapon, a);
    if (result < 0) {
        return 0xFF;
    }
    evtSetValue(event, retIndex, result);
    if (evtCheckID(event->eventId)) {
        return EVT_RETURN_DONE;
    }
    return 0xFF;
}

//s32 index, s32 value
USER_FUNC(btlevtcmd_AcSetParam) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    ((s32*)((u8*)wp + 0x1CC8))[index] = value;
    return EVT_RETURN_DONE;
}

//s32 index, s32 retIndex
// NONMATCHING (93.96%): 2 instructions differ, plus register differences.
USER_FUNC(btlevtcmd_AcGetParam) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 index = evtGetValue(event, args[0]);
    s32 value = *(s32*)((u8*)wp + 0x1CC8 + index * 4);

    if (args[1] != EVTDAT_POINTER_MAX) {
        evtSetValue(event, args[1], value);
    }
    return EVT_RETURN_DONE;
}

//s32 index, s32 value
USER_FUNC(btlevtcmd_AcSetOutputParam) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    ((s32*)((u8*)wp + 0x1CE8))[index] = value;
    return EVT_RETURN_DONE;
}

//s32 index, s32 retIndex
// NONMATCHING (93.96%): 2 instructions differ, plus register differences.
USER_FUNC(btlevtcmd_AcGetOutputParam) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 index = evtGetValue(event, args[0]);
    s32 value = *(s32*)((u8*)wp + 0x1CE8 + index * 4);

    if (args[1] != EVTDAT_POINTER_MAX) {
        evtSetValue(event, args[1], value);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 pose
USER_FUNC(btlevtcmd_SetTalkPose) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 value = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    *(s32*)((u8*)unit + 0x2D4) = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 pose
USER_FUNC(btlevtcmd_SetStayPose) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 value = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    *(s32*)((u8*)unit + 0x2D8) = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 dataTable
// NONMATCHING (92.17%): 1 instruction differs, 1 missing, plus register differences.
USER_FUNC(btlevtcmd_ChangeDataTable) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleDataEntry* dataTable = (BattleDataEntry*)args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->dataTable = dataTable;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 flags
USER_FUNC(btlevtcmd_OnStatusFlag) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 flags = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    BtlUnit_OnStatusFlag(unit, flags);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 flags
USER_FUNC(btlevtcmd_OffStatusFlag) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 flags = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    BtlUnit_OffStatusFlag(unit, flags);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 flags
USER_FUNC(btlevtcmd_OnUnitFlag) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 flags = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    BtlUnit_OnUnitFlag(unit, flags);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 flags
USER_FUNC(btlevtcmd_OffUnitFlag) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 flags = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    BtlUnit_OffUnitFlag(unit, flags);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* eventCode
USER_FUNC(btlevtcmd_SetEventWait) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->waitEventCode = (void*)args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* eventCode
USER_FUNC(btlevtcmd_SetEventUnisonPhase) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->unisonPhaseEventCode = (void*)args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* eventCode
USER_FUNC(btlevtcmd_SetEventPhase) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->phaseEventCode = (void*)args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* eventCode
USER_FUNC(btlevtcmd_SetEventAttack) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    *(s32*)((u8*)unit + 0x29C) = args[1];
    *(s32*)((u8*)unit + 0x2A8) = 0;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 value
USER_FUNC(btlevtcmd_SetEventConfusion) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    *(s32*)((u8*)unit + 0x2A0) = args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* eventCode
USER_FUNC(btlevtcmd_SetEventDamage) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->damageEventCode = (void*)args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 value
USER_FUNC(btlevtcmd_SetEventEntry) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    *(s32*)((u8*)unit + 0x2B4) = args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 value
USER_FUNC(btlevtcmd_SetEventCeilFall) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    *(s32*)((u8*)unit + 0x2BC) = args[1];
    return EVT_RETURN_DONE;
}


//s32* battleId
USER_FUNC(btlevtcmd_AudienceAppeal) {
    BattleWork* wp = _battleWorkPointer;
    s32 btlId = evtGetValue(event, *event->args);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));

    if (unit != NULL) {
        BattleAudience_Case_Appeal(unit);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* data
USER_FUNC(btlevtcmd_AfterReactionEntry) {
    s32* args = event->args;
    s32 id;
    s32 data;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    data = evtGetValue(event, args[1]);
    BattleAfterReactionEntry(id, data);
    return EVT_RETURN_DONE;
}

//s32 g0, s32 g1, s32 g2, s32 g3
USER_FUNC(btlevtcmd_AcSetGaugeParam) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;

    *(s32*)((u8*)wp + 0x1D08) = evtGetValue(event, args[0]);
    *(s32*)((u8*)wp + 0x1D0C) = evtGetValue(event, args[1]);
    *(s32*)((u8*)wp + 0x1D10) = evtGetValue(event, args[2]);
    *(s32*)((u8*)wp + 0x1D14) = evtGetValue(event, args[3]);
    return EVT_RETURN_DONE;
}

//s32 chan, s32 mask, s32 retIndex
// NONMATCHING (88.82%): 1 instruction differs, 1 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_PadCheckTrigger) {
    s32* args = event->args;
    s32 index = args[2];

    if (BattlePadMultiCheckTrigger(evtGetValue(event, args[0]), args[1])) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}

//s32 chan, s32 mask, s32 retIndex
// NONMATCHING (88.82%): 1 instruction differs, 1 missing, 1 extra, plus register differences.
USER_FUNC(btlevtcmd_PadCheckNow) {
    s32* args = event->args;
    s32 index = args[2];

    if (BattlePadMultiCheckNow(evtGetValue(event, args[0]), args[1])) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}

//s32 value
USER_FUNC(btlevtcmd_StartAC) {
    BattleWork* wp = _battleWorkPointer;
    s32 value = evtGetValue(event, *event->args);

    BattleActionCommandStart(wp);
    if (value != 0 && !(wp->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)) {
        BattleAcHelpSetDispType(EVT_RETURN_DONE);
    }
    return EVT_RETURN_DONE;
}


//s32* battleId, f32* speed
USER_FUNC(btlevtcmd_SetMoveSpeed) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    f32 value = evtGetFloat(event, args[1]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    BtlUnit_SetMoveSpeed(unit, value);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* speed
USER_FUNC(btlevtcmd_SetJumpSpeed) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    f32 value = evtGetFloat(event, args[1]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    BtlUnit_SetJumpSpeed(unit, value);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* accel
USER_FUNC(btlevtcmd_SetFallAccel) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    f32 value = evtGetFloat(event, args[1]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    BtlUnit_SetFallAccel(unit, value);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* guardId
USER_FUNC(btlevtcmd_SetGuard) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    s32 value;

    value = evtGetValue(event, args[1]);
    value = BattleTransID(event, value);
    unit->protectId = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 difficulty
USER_FUNC(btlevtcmd_AcSetDifficulty) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 difficulty = evtGetValue(event, args[1]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));

    BattleActionCommandSetDifficulty(wp, unit, difficulty);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* soundId, s32* soundId2
USER_FUNC(btlevtcmd_SetJumpSound) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    s32 a = evtGetValue(event, args[1]);
    s32 b = evtGetValue(event, args[2]);

    *(s32*)((u8*)unit + 0x1C0) = a;
    *(s32*)((u8*)unit + 0x1C4) = b;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 flags, s32 retIndex
USER_FUNC(btlevtcmd_CheckUnitFlag) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 flags = args[1];
    s32 index = args[2];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    if (BtlUnit_CheckUnitFlag(unit, flags)) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 status, s32 retIndex
USER_FUNC(btlevtcmd_CheckStatus) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 index = args[2];
    s8 status = (s8)args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    if (BtlUnit_CheckStatus(unit, status)) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}


//s32* battleId, s32* partId, s32 anime
USER_FUNC(btlevtcmd_AnimeChangePaperAnime) {
    s32 btlId, id, partId;
    s32* args = event->args;
    BattleUnitPart* part;
    s32 anime;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    anime = evtGetValue(event, args[2]);
    btlDispChangePeraAnime(part, (const char*)anime);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32 type
USER_FUNC(btlevtcmd_AnimeChangePoseType) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;
    s32 type;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    type = evtGetValue(event, args[2]);
    BtlUnit_GetPoseNameFromType(part, type);
    BtlUnit_SetAnimType(part, type);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* anime
USER_FUNC(btlevtcmd_AnimeChangePose) {
    s32 btlId, id, partId;
    s32* args = event->args;
    BattleUnitPart* part;
    s32 anime;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    anime = evtGetValue(event, args[2]);
    BtlUnit_SetAnim(part, (char*)anime);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId
USER_FUNC(btlevtcmd_AnimeChangePoseFromTable) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    btlDispPoseAnime(part);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* table
USER_FUNC(btlevtcmd_AnimeSetPoseTable) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    part->poseTable = (void*)args[2];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* speed
USER_FUNC(btlevtcmd_AnimeSetMotionSpeed) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    *(f32*)((u8*)part + 0x20C) = evtGetFloat(event, args[2]);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* table
USER_FUNC(btlevtcmd_AnimeSetPoseSoundTable) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    *(s32*)((u8*)part + 0x4D0) = args[2];
    BtlUnit_PoseSoundInit(part);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32 flags, BOOL onoff
USER_FUNC(btlevtcmd_AnimeFlagOnOff) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;
    s32 flags;
    u32 onoff;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    flags = args[2];
    onoff = evtGetValue(event, args[3]);
    if (onoff) {
        *(u32*)((u8*)part + 0x204) = *(u32*)((u8*)part + 0x204) | (u32)flags;
    }
    else {
        *(u32*)((u8*)part + 0x204) = *(u32*)((u8*)part + 0x204) & ~(u32)flags;
    }
    return EVT_RETURN_DONE;
}

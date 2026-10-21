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
#include "eff/eff_itemget.h"
#include "eff/n64/eff_coin_n64.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include <stdlib.h>
#include "math_ext.h"
#include "pmario_sound.h"
#include "battle/battle_enemy_item.h"
#include "battle/battle_ac_help.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"
#include "battle/battle.h"
#include "battle/battle_seq.h"
#include "battle/ac/ac_timing_a.h"
#include "battle/battle_seq_command.h"
#include "battle/battle_damage.h"
#include "battle/battle_detect.h"
#include "battle/battle_audience.h"
#include "battle/battle_ac.h"
#include "data/item_data.h"
#include "driver/animdrv.h"
#include "driver/itemdrv.h"

#pragma pool_data on

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

// LOCAL_PROTO(BattleCheckCounter): battle.c uses this function symbol as a byte array for split-owned jump-table relocations; a shared function declaration conflicts.
s32 BattleCheckCounter(void* work, BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part, BattleWeapon* weapon, s32* out);

// The original unit was built for size; without this the neg/or/srawi mask and
// the /60 reciprocal-multiply codegen differ from the target (verified with the
// GC/2.6 compiler).

extern BattleWork* _battleWorkPointer;
extern ItemData itemDataTable[];
extern const char str_SFX_BTL_ACROBAT_MISS1[];
extern const double double_0p01_802ee040;
extern const double double_to_int_802ee038;
extern const f64 double_to_int_mask_802ee170;
// The original .rodata starts with six zero Vecs: the templates of the `Vec` copies in the functions
// below, pooled while the source is read. The first five are named objects here (their users match
// with the named reads); _MoveSoundControlCore's is its own compound literal's template.
extern s32 psndSFX_pit(s32 id, s32 pit);

extern s32 subsetevt_confuse_flustered[];
#define NAN (*(f32*)__float_nan)
#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5
#ifdef MUST_MATCH
#define ACSE_STR(off, sym) (base + (off))
#define ACSE_SYSSFX(sym, lit) (lit)
#else
extern const char str_SFX_SYSTEM_NICE_25PERCENT1[], str_SFX_SYSTEM_NICE_50PERCENT1[], str_SFX_SYSTEM_NICE1[], str_SFX_SYSTEM_GOOD1[], str_SFX_SYSTEM_GREAT1[], str_SFX_SYSTEM_WONDERFUL1[],
    str_SFX_SYSTEM_EXCELLENT1[], str_SFX_ITEM_MARIO_USE1[], str_SFX_ITEM_PARTY_USE1[], str_SFX_ENM_ITEM_USE1[];
#define ACSE_STR(off, sym) (sym)
#define ACSE_SYSSFX(sym, lit) (sym)
#endif

//s32* eventId
// Prototypes for the forward references the original function order needs.
static inline s32 __fpclassifyf(f32 x);
static inline f32 sqrtf(f32 x);

USER_FUNC(btlevtcmd_WaitEventEnd) {
    s32 id;

    id = evtGetValue(event, *event->args);
    return evtCheckID(id) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

//s32 retIndex, s32 battleFlags
// opt_propagation off: keep wp in r4 and args in r5 as the target does (the natural propagation
// order gives args r6/args[1] r5).
#pragma opt_propagation off
USER_FUNC(btlevtcmd_check_battleflag) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 index = args[0];
    u32 flags = args[1];

    if (wp->flags & flags) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}
#pragma opt_propagation reset

//BOOL* onoff, s32 flags
USER_FUNC(btlevtcmd_onoff_battleflag) {
    s32* args = event->args;
    BOOL onoff = evtGetValue(event, args[0]);
    u32 flags = args[1];
    BattleWork* wp = _battleWorkPointer;

    if (onoff == TRUE) {
        wp->flags |= flags;
    }
    else {
        wp->flags &= ~flags;
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
// The pick loop's `ceilcount`/`retcount` are an inline helper's locals, i.e. MWCC compiler
// variables; its backend CSE then turns the second `li 0` into `mr r5,r6` for the
// `retcount = ceilcount` copy, where named locals keep `li r5,0`. Two separate `= 0`
// statements (not `retcount = ceilcount = 0`) are needed so the front end emits two `li`s
// for the CSE to merge. The loads below put `args[0]` before
// `args[1]` (reading `args[-1]` after `args += 2`).
static inline s32 btlevtcmd_DrawLotsPick(s32* alloc, s32 count, s32 ceiling) {
    s32 ceilcount, retcount, i;
    ceilcount = 0;
    retcount = 0;
    for (i = 0; i < count; i++) {
        ceilcount += alloc[i];
        if (ceiling < ceilcount) {
            break;
        }
        retcount++;
    }
    return retcount;
}
USER_FUNC(btlevtcmd_DrawLots) {
    s32* args = event->args;
    s32 index;
    s32 *alloc;
    s32 i, countId, count, max;
    s32 ceiling, retcount;

    index = args[0];
    args += 2;
    countId = args[-1];
    count = evtGetValue(event, countId);
    alloc = BattleAlloc(sizeof(s32) * count);
    max = 0;
    for (i = 0; i < count; i++) {
        alloc[i] = evtGetValue(event, *args++);
        max += alloc[i];
    }
    ceiling = irand(max);
    retcount = btlevtcmd_DrawLotsPick(alloc, count, ceiling);
    evtSetValue(event, index, retcount);
    BattleFree(alloc);
    return EVT_RETURN_DONE;
}

//s32 retIndex for unitId, BattleUnitSetup** setup, s32 spawnFlags
USER_FUNC(btlevtcmd_SpawnUnit) {
    s32* args = event->args;
    BattleUnitSetup* setup;
    BattleUnit* unit;
    s32 index;
    
    index = args[0];
    setup = (BattleUnitSetup*)evtGetValue(event, args[1]);
    unit = BtlUnit_Spawn(setup, args[2]);
    if (!unit) {
        evtSetValue(event, index, -1);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, index, unit->unitId);
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
        unit->mAttributeFlags |= kInactive;
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
USER_FUNC(btlevtcmd_ReplaceParts) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, transId, lookupPart, partNum;
    BattleUnitKindPart* kind;
    BattleUnit* unit;

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
        part->mPositionOffset.x = part->mKindPartParams->positionOffset.x;
        part->mPositionOffset.y = part->mKindPartParams->positionOffset.y;
        part->mPositionOffset.z = part->mKindPartParams->positionOffset.z;
        BtlUnit_SetPartsRotate(part, 0.0f, 0.0f, 0.0f);
        BtlUnit_SetPartsRotateOffset(part, 0.0f, 0.0f, 0.0f);
        part->mHitBaseOffset.x = part->mKindPartParams->hitBaseOffset.x;
        part->mHitBaseOffset.y = part->mKindPartParams->hitBaseOffset.y;
        part->mHitOffset.x = 0.0f;
        part->mHitOffset.y = 0.0f;
        part->detectOffsetX = 0;
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
USER_FUNC(btlevtcmd_SetPos) {
    f32 x, y, z, currX, currY, currZ;
    s32* args = event->args;
    BattleUnit* unit;
    s32 indexX, indexY, indexZ, id;

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
USER_FUNC(btlevtcmd_SetPartsPos) {
    s32* args = event->args;
    f32 x, y, z, currX, currY, currZ;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id, partId;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = (f32)evtGetValue(event, args[2]);
    y = (f32)evtGetValue(event, args[3]);
    z = (f32)evtGetValue(event, args[4]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BattleGetUnitPartsPtr(id, partId);
    if (part->attributes & kAbsolutePosition) {
        BtlUnit_SetPartsPos(part, x, y, z);
    }
    else {
        BtlUnit_GetPos(unit, &currX, &currY, &currZ);
        x -= currX;
        y -= currY;
        z -= currZ;
        BtlUnit_SetPartsOffsetPos(part, x, y, z);
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
    if (part->attributes & kAbsolutePosition) {
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
    if (x != -250000000.0f) {
        unit->togeOffset.x = x;
    }
    if (y != -250000000.0f) {
        unit->togeOffset.y = y;
    }
    if (z != -250000000.0f) {
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
USER_FUNC(btlevtcmd_SetPartsHomePos) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId;
    s32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetValue(event, args[2]);
    y = evtGetValue(event, args[3]);
    z = evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetPartsHomePos(part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32 indexX, s32 indexY, s32 indexZ
USER_FUNC(btlevtcmd_GetHitPos) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;
    s32 indexX, indexY, indexZ;

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
USER_FUNC(btlevtcmd_SetHitOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id;
    s32 partId;
    BattleUnit* unit;
    s32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetValue(event, args[2]);
    y = evtGetValue(event, args[3]);
    z = evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetHitOffset(unit, part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetHitCursorOffset) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id;
    s32 partId;
    BattleUnit* unit;
    s32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    partId = evtGetValue(event, args[1]);
    x = evtGetValue(event, args[2]);
    y = evtGetValue(event, args[3]);
    z = evtGetValue(event, args[4]);
    part = BattleGetUnitPartsPtr(id, partId);
    BtlUnit_SetHitCursorOffset(unit, part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_GetWidth) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, index, width;

    btlId = evtGetValue(event, args[0]);
    index = args[1];
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    width = BtlUnit_GetWidth(unit);
    evtSetValue(event, index, width);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_GetHeight) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, index, height;

    btlId = evtGetValue(event, args[0]);
    index = args[1];
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    height = BtlUnit_GetHeight(unit);
    evtSetValue(event, index, height);
    return EVT_RETURN_DONE;
}

//s32* battleId, s16* height
USER_FUNC(btlevtcmd_SetHeight) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;
    s32 height;

    btlId = evtGetValue(event, args[0]);
    height = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    BtlUnit_SetHeight(unit, height);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_GetStatusMg) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId, index;

    btlId = evtGetValue(event, args[0]);
    index = args[1];
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    evtSetFloat(event, index, unit->mSizeMultiplier);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* xpos, s32* ypos (can be EVTDAT_POINTER_MAX to not store)
USER_FUNC(btlevtcmd_SetStatusIconOffset) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 btlId, xpos, ypos;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    xpos = evtGetValue(event, args[1]);
    ypos = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    if (xpos != EVTDAT_POINTER_MAX) {
        unit->mStatusIconOffset[0] = (s16)xpos;
    }
    if (ypos != EVTDAT_POINTER_MAX) {
        unit->mStatusIconOffset[1] = (s16)ypos;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* xpos, s32* ypos (can be EVTDAT_POINTER_MAX to not store)
USER_FUNC(btlevtcmd_SetHpGaugeOffset) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 btlId, xpos, ypos;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    xpos = evtGetValue(event, args[1]);
    ypos = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    if (xpos != EVTDAT_POINTER_MAX) {
        unit->mHpGaugeOffset[0] = (s16)xpos;
    }
    if (ypos != EVTDAT_POINTER_MAX) {
        unit->mHpGaugeOffset[1] = (s16)ypos;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retX, s32 retY
USER_FUNC(btlevtcmd_GetHpGaugeOffset) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, indexX, indexY;
    s32 valueX, valueY;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    indexX = args[1];
    indexY = args[2];
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    valueX = unit->mHpGaugeOffset[0];
    valueY = unit->mHpGaugeOffset[1];
    if (valueX != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexX, valueX);
    }
    if (valueY != EVTDAT_POINTER_MAX) {
        evtSetValue(event, indexY, valueY);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, u8* alpha
USER_FUNC(btlevtcmd_SetAlpha) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, btlId, partId;
    s32 alpha;

    btlId = evtGetValue(event, args[0]);
    partId = evtGetValue(event, args[1]);
    alpha = evtGetValue(event, args[2]);
    id = BattleTransID(event, btlId);
    part = BattleGetUnitPartsPtr(id, partId);
    part->color.a = alpha;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, u8* redIndex, u8* greenIndex, u8* blueIndex
USER_FUNC(btlevtcmd_GetRGB) {
#ifdef MUST_MATCH
    // Reading args through a volatile pointer stops the scheduler from hoisting args[3]/args[4]
    // above args[2] (it otherwise puts the caller-saved args[2] load last); the original loads
    // them in order, so this reproduces the target's register allocation.
    volatile s32* args = event->args;
#else
    s32* args = event->args;
#endif
    s32 btlId, partId, indexR, indexG, indexB;
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
USER_FUNC(btlevtcmd_SetRGB) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, btlId, partId;
    s32 r, g, b;

    btlId = evtGetValue(event, args[0]);
    partId = evtGetValue(event, args[1]);
    r = evtGetValue(event, args[2]);
    g = evtGetValue(event, args[3]);
    b = evtGetValue(event, args[4]);
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
USER_FUNC(btlevtcmd_SetRotate) {
    s32* args = event->args;
    f32 x, y, z, currX, currY, currZ;
    BattleUnit* unit;
    s32 id, indexX, indexY, indexZ;

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
USER_FUNC(btlevtcmd_SetBaseRotate) {
    s32* args = event->args;
    f32 x, y, z, currX, currY, currZ;
    BattleUnit* unit;
    s32 id, indexX, indexY, indexZ;

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
    if (x != -250000000.0f) {
        currX = x;
    }
    if (y != -250000000.0f) {
        currY = y;
    }
    if (z != -250000000.0f) {
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
USER_FUNC(btlevtcmd_SetCutWidth) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 value;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    value = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->cutWidth = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* value
USER_FUNC(btlevtcmd_SetCutHeight) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 value;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    value = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->cutHeight = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z (can be EVTDAT_POINTER_MAX to skip)
USER_FUNC(btlevtcmd_SetPossessionItemOffset) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 id, x, y, z, indexX, indexY, indexZ;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
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

USER_FUNC(btlevtcmd_SetPartsBaseScale) {
    s32* args = event->args;
    BattleUnitPart* part;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id == -1) {
        return EVT_RETURN_DONE;
    }
    {
        s32 partId = evtGetValue(event, args[1]);
        part = BattleGetUnitPartsPtr(id, partId);
    }
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    BtlUnit_SetPartsBaseScale(part, x, y, z);
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
USER_FUNC(btlevtcmd_SetScale) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id == -1) {
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    BtlUnit_SetScale(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddScale) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    f32 x, y, z;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id == -1) {
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    BtlUnit_AddScale(unit, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_SetPartsScale) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id == -1) {
        return EVT_RETURN_DONE;
    }
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    BtlUnit_SetPartsScale(part, x, y, z);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, f32* x, f32* y, f32* z
USER_FUNC(btlevtcmd_AddPartsScale) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id, partId;
    f32 x, y, z;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    if (id == -1) {
        return EVT_RETURN_DONE;
    }
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    BtlUnit_AddPartsScale(part, x, y, z);
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
    BattleUnit* unit;
    BattleWork* wp = _battleWorkPointer;
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

USER_FUNC(btlevtcmd_SetOverTurnCount) {
    s32 btlId;
    s32 id;
    s32* args = event->args;
    s32 value;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    value = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->flippedTurns = value;
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

USER_FUNC(btlevtcmd_AddUnitWork) {
    s32* args = event->args;
    s32 btlId, id, value, index;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    index = evtGetValue(event, args[1]);
    value = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->work[index] += value;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_GetUnitWorkFloat) {
    s32 btlId;
    s32 id;
    s32* args = event->args;
    s32 i;
    s32 index;
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

USER_FUNC(btlevtcmd_StartAvoid) {
    s32* args = event->args;
    s32 btlId, damageCode;
    EffectEntry* entry;
    BattleUnit* unit;
    Vec pos;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    damageCode = args[1] | 0x100;
    if ((u8)damageCode == 0x26) {
        BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
        if (pos.y < 85.0f) {
            pos.y += 50.f;
        }
        else {
            pos.y += -50.f;
        }
        pos.x -= 45.0f;
        entry = effNiceEntry(6, pos.x, pos.y, pos.z);
        ((NiceEffectWork*)entry->userdata)->scale = 0.75f;
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

USER_FUNC(btlevtcmd_CheckSpace) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 boundX, boundY, boundZ;
    s32 baseX, baseY, baseZ;
    BattleUnit* unit;
    s32 index, i;
    f32 x, y, z;
    s32 ix, iy, iz;
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
            ix = (s32)x;
            iy = (s32)y;
            iz = (s32)z;
            if (boundX) {
                if (abs(ix - baseX) < boundX) {
                    found = TRUE;
                }
            }
            if (boundY) {
                if (abs(iy - baseY) < boundY) {
                    found = TRUE;
                }
            }
            if (boundZ) {
                if (abs(iz - baseZ) < boundZ) {
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
        PouchPartyData* d;
        i = BattleTransPartyId(kind);
        d = &pouchGetPtr()->partyData[i];
        value = d->techLevel;
    }
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CalculateFaceDirection) {
    BattleWork* wp;
    s32 thisId;
    u32 targetUnitId;
    s32 thisPartId;
    s32 targetPartId;
    s32* args;
    s32 index;
    s32 targetId;
    s32 btlId;
    u32 flags;
    BattleUnit* targetUnit;
    s32 value;
    BattleUnit* thisUnit;
    BattleUnitPart* thisPart;
    BattleUnitPart* targetPart;
    f32 calculate;
    f32 thisX, thisY, thisZ;
    f32 targetX, targetY, targetZ;
    f32 offsetX, offsetY, offsetZ;
    f32 localX, localY, localZ;
    f32 thisOffsetX, thisOffsetY, thisOffsetZ;

    thisPart = NULL;
    targetPart = NULL;
    args = event->args;
    wp = _battleWorkPointer;
    btlId = evtGetValue(event, args[0]);
    thisId = BattleTransID(event, btlId);
    thisPartId = evtGetValue(event, args[1]);
    targetId = evtGetValue(event, args[2]);
    // the translated id goes to a second variable: the original keeps the raw event value for the
    // `(f32)targetId - localX` cast below and translates only the copy used by BattleGetUnitPtr
    targetUnitId = targetId;
    targetPartId = evtGetValue(event, args[3]);
    flags = args[4];
    index = args[5];

    if (!(flags & 9)) {
        targetUnitId = BattleTransID(event, targetUnitId);
    }

    thisUnit = BattleGetUnitPtr(_battleWorkPointer, thisId);
    if (thisPartId != -1) {
        thisPart = BtlUnit_GetPartsPtr(thisUnit, thisPartId);
    }

    if (flags & 0x10) {
        targetUnit = BattleGetUnitPtr(_battleWorkPointer, targetUnitId);
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
            if (thisPart->attributes & kAbsolutePosition) {
                BtlUnit_GetPartsPos(thisPart, &thisX, &thisY, &thisZ);
            }
            else {
                BtlUnit_GetPartsOffsetPos(thisPart, &offsetX, &offsetY, &offsetZ);
                thisX += offsetX;
            }
        }

        BtlUnit_GetPos(targetUnit, &targetX, &targetY, &targetZ);
        if (targetPart) {
            if (targetPart->attributes & kAbsolutePosition) {
                BtlUnit_GetPartsPos(targetPart, &targetX, &targetY, &targetZ);
            }
            else {
                BtlUnit_GetPartsOffsetPos(targetPart, &offsetX, &offsetY, &offsetZ);
                targetX += offsetX;
            }
        }

        calculate = targetX - thisX;
        if (calculate > 0.0f) {
            value = 1;
        }
        else if (calculate < 0.0f) {
            value = -1;
        }
    }

    if (flags & 1) {
        BtlUnit_GetPos(thisUnit, &localX, &localY, &localZ);
        if (thisPart) {
            if (thisPart->attributes & kAbsolutePosition) {
                BtlUnit_GetPartsPos(thisPart, &localX, &localY, &localZ);
            }
            else {
                BtlUnit_GetPartsOffsetPos(thisPart, &thisOffsetX, &thisOffsetY, &thisOffsetZ);
                localX += thisOffsetX;
            }
        }
        calculate = (f32)targetId - localX;
        if (calculate > 0.0f) {
            value = 1;
        }
        else if (calculate < 0.0f) {
            value = -1;
        }
    }
    if (flags & 0x100) {
        value *= -1;
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

USER_FUNC(btlevtcmd_ChangeFaceDirection) {
    s32 btlId;
    s32 id;
    s32* args = event->args;
    s32 value;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    value = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->faceDirection = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_ChangePartsFaceDirection) {
    s32* args = event->args;
    s32 btlId, id, partId;
    s32 value;
    BattleUnitPart* part;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    value = evtGetValue(event, args[2]);
    part = BattleGetUnitPartsPtr(id, partId);
    part->faceDirection = value;
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

USER_FUNC(btlevtcmd_SetPartsBlur) {
    s32 id;
    s32* args = event->args;
    s32 btlId, partId;
    BattleUnitPart* part;
    BattleUnit* unit;
    u8 color0, color1, color2, color3;
    u8 color4, color5, color6, color7;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    color0 = (u8)evtGetValue(event, args[2]);
    color1 = (u8)evtGetValue(event, args[3]);
    color2 = (u8)evtGetValue(event, args[4]);
    color3 = (u8)evtGetValue(event, args[5]);
    color4 = (u8)evtGetValue(event, args[6]);
    color5 = (u8)evtGetValue(event, args[7]);
    color6 = (u8)evtGetValue(event, args[8]);
    color7 = (u8)evtGetValue(event, args[9]);
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

USER_FUNC(btlevtcmd_OnToken) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;
    s32 t;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    // routing args[1] through a temp makes MWCC keep the field as the OR destination (`or r0, r0, r4`)
    t = args[1];
    t = unit->mTokenFlags | t;
    unit->mTokenFlags = t;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_OffToken) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;
    s32 t;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    // routing args[1] through a temp makes MWCC keep the field as the ANDC destination (`andc r0, r0, r4`)
    t = args[1];
    t = unit->mTokenFlags & ~t;
    unit->mTokenFlags = t;
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_CheckToken) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, index, mask;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    mask = args[1];
    index = args[2];
    mask = unit->mTokenFlags & mask;
    if (mask) {
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

//s32* battleId, s32* kind
USER_FUNC(btlevtcmd_ChangeKind) {
    s32 btlId;
    s32 id;
    s32* args = event->args;
    s32 kind;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    kind = evtGetValue(event, args[1]);
    BattleGetUnitPtr(_battleWorkPointer, id)->currentKind = kind;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* defenceTable
USER_FUNC(btlevtcmd_SetPartsDefenceTable) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    s32 partId;
    BattleUnitDefense* value;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    value = (BattleUnitDefense*)evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->defense = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* defenceAttrTable
USER_FUNC(btlevtcmd_SetPartsDefenceAttrTable) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    s32 partId;
    BattleUnitDefenseAttr* value;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    partId = evtGetValue(event, args[1]);
    value = (BattleUnitDefenseAttr*)evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    part->defenseAttr = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 dataTable
USER_FUNC(btlevtcmd_ChangeDataTable) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleDataEntry* dataTable;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, btlId);
    dataTable = (BattleDataEntry*)args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->dataTable = dataTable;
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_GetBackItem) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    NpcBattleInfo* setup;
    ItemType item;
    s32 id;
    s32 i;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    if (unit->flags & 0x40000000) {
        item = unit->heldItem;
        if (item != 0) {
            setup = wp->fbatInfo->setup;
            for (i = 0; i < 8; i++) {
                if (setup->recoveredItems[i] == 0) {
                    break;
                }
            }
            setup->recoveredItems[i] = item;
            unit->heldItem = 0;
        }
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_GetExp) {
    s32* args = event->args;
    s32 id, index, value;

    id = evtGetValue(event, args[0]);
    index = args[1];
    id = BattleTransID(event, id);
    value = BtlUnit_GetExp(BattleGetUnitPtr(_battleWorkPointer, id));
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

//s32 exp
USER_FUNC(btlevtcmd_StoreExp) {
    s32 exp = evtGetValue(event, *event->args);

    BattleStoreExp(_battleWorkPointer, exp);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_GetCoin) {
    s32* args = event->args;
    s32 id, index, value;

    id = evtGetValue(event, args[0]);
    index = args[1];
    id = BattleTransID(event, id);
    value = BtlUnit_GetCoin(BattleGetUnitPtr(_battleWorkPointer, id));
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}

//s32 coin
USER_FUNC(btlevtcmd_StoreCoin) {
    s32 coin = evtGetValue(event, *event->args);

    BattleStoreCoin(_battleWorkPointer, coin);
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

//s32* battleId, s32* a, f32* xz, f32* fall
USER_FUNC(btlevtcmd_JumpSetting) {
    BattleUnit* unit;
    s32* args = event->args;
    s32 id;
    s32 value;
    f32 xz;
    f32 fall;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    value = evtGetValue(event, args[1]);
    xz = evtGetFloat(event, args[2]);
    fall = evtGetFloat(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    unit->moveFrames = value;
    unit->mMoveSpeedXZ = xz;
    unit->mFallAccel = fall;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* tx, s32* ty, s32* tz, s32* frame, s32* a
// The two psndSFXOn_3D calls each need two Vecs (source + argument copy) and distinct zero Vec
// literals (retail vec3_802ede88 / vec3_802ede94), which is what fixes the frame; the declaration order
// `s32 btlId, id; s32* args; BattleUnit* unit; s32 frame, a;` fixes the args/id allocation. The tail is
// written `if (frame <= 0) { SetPos(target); ...; return 1; } return 0;`.
USER_FUNC(btlevtcmd_JumpPosition) {
    s32 btlId, id;
    s32* args = event->args;
    BattleUnit* unit;
    s32 frame, a;
    f32 tx, ty, tz;
    f32 x, y, z;
    Vec posb;
    Vec posb2;
    Vec pos;
    Vec pos2;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    frame = evtGetValue(event, args[4]);
    a = evtGetValue(event, args[5]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        event->blocked = 1;
        if (frame != 0) {
            unit->moveFrames = frame;
        }
        BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetMoveCurrentPos(unit, x, y, z);
        unit->moveAngle = angleABf(x, z, tx, tz);
        unit->moveDist = distABf(x, z, tx, tz);
        if (tx - x > 0.0f) {
            unit->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            unit->moveDirection = -1;
        }
        y = ty - y;
        if (unit->moveFrames == 0) {
            unit->moveFrames =
                (s32)(unit->moveDist / unit->mMoveSpeedXZ);
            unit->moveDist -=
                (f32)unit->moveFrames * unit->mMoveSpeedXZ;
        } else {
            unit->mMoveSpeedXZ =
                unit->moveDist / (f32)unit->moveFrames;
            unit->moveDist -=
                (f32)unit->moveFrames * unit->mMoveSpeedXZ;
        }
        if (unit->moveFrames == 0) {
            return 2;
        }
        {
            f32 t = unit->mFallAccel * (f32)unit->moveFrames;
            unit->mMoveSpeedY = y / (f32)unit->moveFrames + t / 2.0f;
        }
        unit->mMoveSpeedXZ = unit->mMoveSpeedXZ +
                             unit->moveDist / (f32)unit->moveFrames;
        BtlUnit_LoadSeMode(2, a, &unit->seModeSrc, &unit->seMode);
        if (unit->seMode.sfxA != 0) {
            pos = (Vec){ 0.0f, 0.0f, 0.0f };
            pos.x = unit->mMoveCurrentPos.x;
            pos.y = unit->mMoveCurrentPos.y;
            pos.z = unit->mMoveCurrentPos.z;
            posb = pos;
            psndSFXOn_3D((const char*)unit->seMode.sfxA, &posb);
        }
    }
    unit->mMoveCurrentPos.y += unit->mMoveSpeedY;
    unit->mMoveSpeedY -= unit->mFallAccel;
    if (unit->mMoveSpeedY < 0.0f) {
#ifdef MUST_MATCH
        // The volatile pins the current.y load before the target.y load; a plain read lets the
        // scheduler emit target.y first, which is the only remaining difference.
        f32 cur = *(volatile f32*)&unit->mMoveCurrentPos.y;
#else
        f32 cur = unit->mMoveCurrentPos.y;
#endif
        f32 tgt = unit->mMoveTargetPos.y;
        if (cur < tgt) {
            unit->mMoveCurrentPos.y = tgt;
        }
    }
    btlMovePos(&unit->mMoveCurrentPos.x, &unit->mMoveCurrentPos.z, unit->mMoveSpeedXZ,
               unit->moveAngle);
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    unit->moveFrames--;
    if (unit->moveFrames <= 0) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        if (unit->seMode.sfxB != 0) {
            pos2 = (Vec){ 0.0f, 0.0f, 0.0f };
            pos2.x = unit->mMoveTargetPos.x;
            pos2.y = unit->mMoveTargetPos.y;
            pos2.z = unit->mMoveTargetPos.z;
            posb2 = pos2;
            psndSFXOn_3D((const char*)unit->seMode.sfxB, &posb2);
        }
        return 1;
    }
    return 0;
}

//s32* battleId
USER_FUNC(btlevtcmd_JumpContinue) {
    BattleUnit* unit;
    f32 x, y, z, floor;
    s32 btlId;

    btlId = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    if (isFirstCall) {
        event->blocked = 1;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetMoveCurrentPos(unit, x, y, z);
    }
    unit->mMoveCurrentPos.y += unit->mMoveSpeedY;
    unit->mMoveSpeedY -= unit->mFallAccel;
    btlMovePos(&unit->mMoveCurrentPos.x, &unit->mMoveCurrentPos.z, unit->mMoveSpeedXZ, unit->moveAngle);
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    floor = BattleGetFloorHeight(_battleWorkPointer, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    if (unit->mMoveCurrentPos.y < floor) {
        unit->mMoveCurrentPos.y = floor;
        BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
        return 1;
    }
    return 0;
}

//s32* battleId, s32* tx, s32* ty, s32* tz, s32* frame
USER_FUNC(btlevtcmd_FallPosition) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, frame;
    f32 tx, ty, tz;
    f32 x, y, z;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    frame = evtGetValue(event, args[4]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        event->blocked = 1;
        unit->moveFrames = frame;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetMoveCurrentPos(unit, x, y, z);
        BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
        unit->moveAngle = angleABf(x, z, tx, tz);
        unit->moveDist = distABf(x, z, tx, tz);
        if (tx - x > 0.0f) {
            unit->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            unit->moveDirection = -1;
        }
        /* fallthrough */
        y = ty - y;
        if (unit->moveFrames == 0) {
            if (0.0f == unit->mMoveSpeedXZ) {
                unit->mMoveSpeedXZ = 1.0f;
            }
            unit->moveFrames = (s32)(unit->moveDist / unit->mMoveSpeedXZ);
            if (unit->moveFrames == 0) {
                unit->moveFrames = 1;
            }
        } else {
            unit->mMoveSpeedXZ = unit->moveDist / (f32)unit->moveFrames;
        }
        unit->mMoveSpeedY = 0.0f;
        unit->mFallAccel = (y / (f32)unit->moveFrames - unit->mMoveSpeedY) /
                           ((f32)(-unit->moveFrames) / 2.0f);
    }
    unit->mMoveCurrentPos.y += unit->mMoveSpeedY;
    unit->mMoveSpeedY -= unit->mFallAccel;
    btlMovePos(&unit->mMoveCurrentPos.x, &unit->mMoveCurrentPos.z, unit->mMoveSpeedXZ,
               unit->moveAngle);
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    unit->moveFrames--;
    if (unit->moveFrames <= 0) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        return 1;
    }
    return 0;
}

//s32* battleId, f32* x, f32* y, f32* z, s32* moveFrame, s32* a, s32* b
USER_FUNC(btlevtcmd_MovePosition) {
    // declaring `id` first is what puts args/unit in r30 and id in r31 as the target does
    s32 id;
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, frame, a, b;
    f32 tx, ty, tz;
    f32 x, y, z;
    f32 speed;
    f32 dist;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    frame = evtGetValue(event, args[4]);
    a = evtGetValue(event, args[5]);
    b = evtGetValue(event, args[6]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        event->blocked = 1;
        unit->moveFrames = frame;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetMoveCurrentPos(unit, x, y, z);
        BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
        unit->moveAngle = angleABf(x, z, tx, tz);
        unit->moveDist = distABf(x, z, tx, tz);
        if (tx - x > 0.0f) {
            unit->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            unit->moveDirection = -1;
        }
        /* fallthrough */
        if (unit->moveFrames == 0) {
            unit->moveFrames = (s32)(unit->moveDist / unit->mMoveSpeedXZ);
            if (unit->moveFrames == 0) {
                unit->moveFrames = 1;
            }
            speed = unit->mMoveSpeedXZ;
            dist = unit->moveDist;
            unit->mMoveSpeedXZ =
                speed +
                (dist - (f32)unit->moveFrames * speed) / (f32)unit->moveFrames;
        } else {
            unit->mMoveSpeedXZ = unit->moveDist / (f32)unit->moveFrames;
        }
        BtlUnit_LoadSeMode(b, a, &unit->seModeSrc, &unit->seMode);
    }
    btlMovePos(&unit->mMoveCurrentPos.x, &unit->mMoveCurrentPos.z, unit->mMoveSpeedXZ, unit->moveAngle);
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    unit->moveFrames--;
    if (unit->moveFrames <= 0) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        _UnitMoveSoundControlEnd(unit);
        return 1;
    }
    _UnitMoveSoundControl(unit);
    return 0;
}

//s32* battleId, s32* x, s32* y, s32* z, s32* speed, s32* retIndex
USER_FUNC(btlevtcmd_GetMoveFrame) {
    BattleUnit* unit;
    s32 btlId, id;
    s32* args = event->args;
    f32 x2, z2;
    f32 speed;
    s32 retIndex;
    f32 x, y, z;
    f32 dist;
    s32 frames;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    x2 = (f32)evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    z2 = (f32)evtGetValue(event, args[3]);
    speed = evtGetFloat(event, args[4]);
    retIndex = args[5];
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    dist = distABf(x, z, x2, z2);
    if (!speed) {
        speed = unit->mMoveSpeedXZ;
        if (!speed) {
            speed = 1.0f;
        }
    }
    frames = (s32)(dist / speed);
    if (dist - (f32)frames * speed > 0.0f) {
        frames++;
    }
    if (frames == 0) {
        frames = 1;
    }
    if (retIndex != EVTDAT_POINTER_MAX) {
        evtSetValue(event, retIndex, frames);
    }
    return 2;
}

//s32* battleId, s32* tx, s32* ty, s32* tz, s32* frame, s32* height, s32* interp, s32* curve, s32* a
// MATCHING. A dive from the start position to the target through intpl_sub (interp >= 100 keeps the unit
// above the floor); curve 0 offsets the height by a sine, otherwise the offset follows the direction of
// travel. The non-obvious bits: the sqrt inputs are assigned dx-first (MWCC's deferred inliner emits the
// second variable's subtraction/square first).
USER_FUNC(btlevtcmd_DivePosition) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId, id, frame, height, interp, curve;
    s32 aboveFloor, a;
    s32 left;
    f32 tx, ty, tz;
    f32 x, y, z;
    f32 dx, dy, dz;
    f32 dist, dist2;
    f32 h, ang, offX, offZ, floor;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    frame = evtGetValue(event, args[4]);
    height = evtGetValue(event, args[5]);
    interp = evtGetValue(event, args[6]);
    curve = evtGetValue(event, args[7]);
    a = evtGetValue(event, args[8]);
    aboveFloor = 0;
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (interp >= 100) {
        aboveFloor = 1;
        interp -= 100;
    }
    if (isFirstCall) {
        event->blocked = 1;
        unit->moveFrames = frame;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetMoveStartPos(unit, x, y, z);
        BtlUnit_SetMoveCurrentPos(unit, x, y, z);
        BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
        dx = x - tx;
        dy = y - ty;
        dz = z - tz;
        dist = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        unit->moveDist = dist;
        if (unit->moveFrames == 0) {
            unit->moveFrames = (s32)(unit->moveDist / unit->mMoveSpeedXZ);
        } else {
            unit->mMoveSpeedXZ = unit->moveDist / (f32)unit->moveFrames;
        }
        if (unit->moveFrames == 0) {
            return 2;
        }
        unit->moveProgress = 0.0f;
        unit->moveAngle = 0.0f;
        if (tx - x > 0.0f) {
            unit->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            unit->moveDirection = -1;
        }
        BtlUnit_LoadSeMode(3, a, &unit->seModeSrc, &unit->seMode);
    }
    unit->mMoveCurrentPos.x = intpl_sub(interp, (s32)unit->moveProgress, unit->moveFrames,
                                        unit->mMoveStartPos.x, unit->mMoveTargetPos.x);
    unit->mMoveCurrentPos.y = intpl_sub(interp, (s32)unit->moveProgress, unit->moveFrames,
                                        unit->mMoveStartPos.y, unit->mMoveTargetPos.y);
    unit->mMoveCurrentPos.z = intpl_sub(interp, (s32)unit->moveProgress, unit->moveFrames,
                                        unit->mMoveStartPos.z, unit->mMoveTargetPos.z);
    if (aboveFloor) {
        floor = BattleGetFloorHeight(_battleWorkPointer, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y,
                                     unit->mMoveCurrentPos.z);
        if (unit->mMoveCurrentPos.y < floor) {
            unit->moveProgress = (f32)unit->moveFrames;
            BtlUnit_SetMoveTargetPos(unit, unit->mMoveCurrentPos.x, floor, unit->mMoveCurrentPos.z);
        }
    }
    unit->moveProgress += 1.0f;
    if (unit->moveProgress > (f32)unit->moveFrames) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        _UnitMoveSoundControlEnd(unit);
        return 2;
    }
    if (curve == 0) {
        dx = unit->mMoveTargetPos.x - unit->mMoveCurrentPos.x;
        dy = unit->mMoveTargetPos.y - unit->mMoveCurrentPos.y;
        dz = unit->mMoveTargetPos.z - unit->mMoveCurrentPos.z;
        dist2 = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        if (0.0f == dist2) {
            dist2 = 1.0f;
        }
        if (0.0f == unit->moveDist) {
            unit->moveDist = 1.0f;
        }
        h = sinfd(180.f * (1.0f - dist2 / unit->moveDist));
        if (height == 0) {
            h = 0.0f;
        }
        if (height < 0) {
            h = -h * (f32)(-height);
        }
        if (height > 0) {
            h = h * (f32)height;
        }
        BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y + h, unit->mMoveCurrentPos.z);
    } else {
        f32 distc;
        dx = unit->mMoveTargetPos.x - unit->mMoveCurrentPos.x;
        dy = unit->mMoveTargetPos.y - unit->mMoveCurrentPos.y;
        dz = unit->mMoveTargetPos.z - unit->mMoveCurrentPos.z;
        dist2 = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        distc = dist2;
        if (unit->mMoveTargetPos.x - unit->mMoveStartPos.x > 0.0f) {
            left = 1;
        } else if (unit->mMoveTargetPos.x - unit->mMoveStartPos.x < 0.0f) {
            left = 0;
        } else if (unit->mMoveTargetPos.y - unit->mMoveStartPos.y < 0.0f) {
            left = 0;
        } else {
            left = 1;
        }
        if (0.0f == dist2) {
            distc = 1.0f;
        }
        if (0.0f == unit->moveDist) {
            unit->moveDist = 1.0f;
        }
        h = sinfd(180.f * (1.0f - distc / unit->moveDist));
        if (left) {
            ang = 360.f * atan2f_safety(unit->mMoveTargetPos.x - unit->mMoveStartPos.x,
                                                     unit->mMoveTargetPos.y - unit->mMoveStartPos.y) /
                  6.2831855f;
        } else {
            ang = 180.f + 360.f * atan2f_safety(unit->mMoveTargetPos.x - unit->mMoveStartPos.x,
                                                                          unit->mMoveTargetPos.y - unit->mMoveStartPos.y) /
                                           6.2831855f;
        }
        offX = h * sinfd(ang);
        offZ = h * cosfd(180.f + ang);
        if (height == 0) {
            offX = 0.0f;
            offZ = offX;
        } else {
            offX = offX * (f32)height;
            offZ = offZ * (f32)height;
        }
        BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x + offZ, unit->mMoveCurrentPos.y + offX, unit->mMoveCurrentPos.z);
    }
    _UnitMoveSoundControl(unit);
    return 0;
}

// Inline sqrt (with the NaN classification) the original expands at both
// distance sites; same helper as system.c/evt_npc.c.
int __float_nan[];


static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

//s32* battleId, s32* tx, s32* ty, s32* tz, s32* frame, s32* height, s32* mode
// The pool offsets, the arg/e schedule and the dist/ellipse temps match: `frame, mode` before `args`, `e`
// last with its i2f deferred to `eRaw`, both mMoveSpeedXZ divisions reread moveDist, and the dx/dz/cx/cz/radius
// source order with the start-first sums and the radius constant first colours the f3/f4/f5 x temps like the
// original.
USER_FUNC(btlevtcmd_EllipseMovePosition) {
    s32 btlId, id;
    s32 frame, mode;
    s32* args = event->args;
    BattleUnit* unit;
    f32 tx;
    f32 ty;
    f32 tz;
    f32 x;
    f32 y;
    f32 z;
    f32 dist;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 cx;
    f32 cz;
    f32 radius;
    f32 angle;
    f32 t;
    f32 e;
    s32 eRaw;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    frame = evtGetValue(event, args[4]);
    eRaw = evtGetValue(event, args[5]);
    mode = evtGetValue(event, args[6]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        event->blocked = 1;
        unit->moveFrames = frame;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetMoveStartPos(unit, x, y, z);
        BtlUnit_SetMoveCurrentPos(unit, x, y, z);
        BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
        dx = x - tx;
        dy = y - ty;
        dz = z - tz;
        dist = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        unit->moveDist = dist;
        if (unit->moveFrames == 0) {
            unit->moveFrames = (s32)(unit->moveDist / unit->mMoveSpeedXZ);
        } else {
            unit->mMoveSpeedXZ = unit->moveDist / (f32)unit->moveFrames;
        }
        if (unit->moveFrames == 0) {
            return 2;
        }
        unit->moveProgress = 0.0f;
        unit->moveAngle = 0.0f;
        if (tx - x > 0.0f) {
            unit->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            unit->moveDirection = -1;
        }
    }
    dx = unit->mMoveTargetPos.x - unit->mMoveStartPos.x;
    dz = unit->mMoveTargetPos.z - unit->mMoveStartPos.z;
    cx = (unit->mMoveStartPos.x + unit->mMoveTargetPos.x) / 2.0f;
    cz = (unit->mMoveStartPos.z + unit->mMoveTargetPos.z) / 2.0f;
    radius = 0.5f * sqrtf(dx * dx + dz * dz);
    e = (f32)eRaw;
    angle = angleABf(cx, cz, unit->mMoveStartPos.x, unit->mMoveStartPos.z);
    t = intpl_sub(mode, (s32)unit->moveProgress, unit->moveFrames,
                  180.f, 0.0f);
    unit->mMoveCurrentPos.x =
        cx - (radius * sinfd(angle) * cosfd(t) + e * cosfd(angle) * sinfd(t));
    unit->mMoveCurrentPos.z =
        cz + radius * cosfd(angle) * cosfd(t) - e * sinfd(angle) * sinfd(t);
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    unit->moveProgress += 1.0f;
    if (unit->moveProgress > (f32)unit->moveFrames) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        return 2;
    }
    return 0;
}

//s32* battleId, s32* retIndex, s32* dir
USER_FUNC(btlevtcmd_MoveDirectionAdd) {
    s32 id;
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;
    s32 retIndex;
    s32 dir;
    s32 value;
    s32 sum;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    retIndex = args[1];
    dir = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(wp, id);
    if (unit->moveDirection) {
        value = dir * unit->moveDirection;
    }
    else {
        value = dir * unit->faceDirection;
    }
    sum = evtGetValue(event, retIndex);
    sum = sum + value;
    evtSetValue(event, retIndex, sum);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex, s32* dir
USER_FUNC(btlevtcmd_FaceDirectionAdd) {
    s32 id;
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;
    s32 retIndex;
    s32 dir;
    s32 delta;
    s32 sum;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    retIndex = args[1];
    dir = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(wp, id);
    delta = dir * unit->faceDirection;
    sum = evtGetValue(event, retIndex);
    sum = sum + delta;
    evtSetValue(event, retIndex, sum);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex, s32* dir
USER_FUNC(btlevtcmd_FaceDirectionSub) {
    s32 id;
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 btlId;
    s32 retIndex;
    s32 dir;
    s32 delta;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    retIndex = args[1];
    dir = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(wp, id);
    delta = dir * unit->faceDirection;
    evtSetValue(event, retIndex, evtGetValue(event, retIndex) - delta);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* retIndex, s32* dir
USER_FUNC(btlevtcmd_PartsFaceDirectionSub) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 btlId;
    s32 id;
    BattleWork* wp = _battleWorkPointer;
    s32 partId;
    s32 retIndex;
    s32 dir;
    s32 delta;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    retIndex = args[2];
    dir = evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(wp, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    delta = dir * part->faceDirection;
    evtSetValue(event, retIndex, evtGetValue(event, retIndex) - delta);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 belong, s32* weaponIndex
USER_FUNC(btlevtcmd_SamplingEnemy) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    u8* sampling = (u8*)&wp->sampling;
    BattleWeapon* weapon;
    s32 id;
    s8 belong;
    s8 direction;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    belong = (s8)evtGetValue(event, args[1]);
    weapon = (BattleWeapon*)evtGetValue(event, args[2]);
    direction = -wp->alliances[belong].attackDirection;
    if (weapon->targetPropertyFlags & 0x10000000) {
        direction *= -1;
    }
    BattleSamplingEnemy(sampling, weapon, id, belong, weapon->targetClassFlags,
                        weapon->targetPropertyFlags, direction);
    return EVT_RETURN_DONE;
}

//s32* weaponIndex, s32 retIndex1, s32 retIndex2
USER_FUNC(btlevtcmd_ChoiceSamplingEnemy) {
    s32* args = event->args;
    u8* sampling = (u8*)&_battleWorkPointer->sampling;
    BattleWeapon* weapon;
    s32 index1;
    s32 index2;
    s32 outId;
    s32 outPartId;

    weapon = (BattleWeapon*)evtGetValue(event, args[0]);
    index1 = args[1];
    index2 = args[2];
    BattleChoiceSamplingEnemy(sampling, weapon->targetWeightingFlags, &outId, &outPartId);
    evtSetValue(event, index1, outId);
    evtSetValue(event, index2, outPartId);
    return EVT_RETURN_DONE;
}

//s32 retIndex1, s32 retIndex2
USER_FUNC(btlevtcmd_GetSelectEnemy) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    u8* sampling = (u8*)&wp->sampling;
    s32 index1;
    s32 index2;
    s32 selectIdx;
    u8* entry;

    index1 = args[0];
    index2 = args[1];

    if ((s8)sampling[0xA6C] <= 0) {
        evtSetValue(event, index1, -1);
        evtSetValue(event, index2, -1);
        return EVT_RETURN_DONE;
    }
    selectIdx = (s8)sampling[0xAB7];
    entry = sampling + (s8)sampling[selectIdx + 0xA6D] * 0x24 + 4;
    evtSetValue(event, index1, *(s16*)(entry + 0));
    evtSetValue(event, index2, *(s16*)(entry + 2));
    return EVT_RETURN_DONE;
}

//s32 retIndex1, s32 retIndex2
USER_FUNC(btlevtcmd_GetSelectNextEnemy) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    u8* sampling = (u8*)&wp->sampling;
    s32 index1 = args[0];
    s32 index2 = args[1];
    s32 selectIdx;
    u8* entry;

    selectIdx = (u8)sampling[0xAB7] + 1;
    sampling[0xAB7] = selectIdx;
    selectIdx = (s8)sampling[0xAB7];
    if (selectIdx < (s8)sampling[0xA6C]) {
        entry = sampling + (s8)sampling[selectIdx + 0xA6D] * 0x24 + 4;
        evtSetValue(event, index1, *(s16*)(entry + 0));
        evtSetValue(event, index2, *(s16*)(entry + 2));
    }
    else {
        evtSetValue(event, index1, -1);
        evtSetValue(event, index2, 0);
    }
    return EVT_RETURN_DONE;
}

//s32 retIndex1, s32 retIndex2
USER_FUNC(btlevtcmd_GetFirstAttackTarget) {
    s32* args = event->args;
    s32 index1 = args[0];
    s32 index2 = args[1];
    BattleUnit* attackUnit;
    BattleUnit* selectUnit;
    s32 partsId;

    BattleGetFirstAttackUnit(_battleWorkPointer, &attackUnit, &selectUnit);
    partsId = BtlUnit_GetBodyPartsId(selectUnit);
    evtSetValue(event, index1, selectUnit->unitId);
    evtSetValue(event, index2, partsId);
    return EVT_RETURN_DONE;
}

#pragma opt_dead_assignments off // needed: without it btlevtcmd_GetConsumeItem 97.94%
USER_FUNC(btlevtcmd_GetConsumeItem) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 idx = wp->commandMenu.cursor[11].field_0;
    u8* tbl = (u8*)&wp->commandMenu;
    s32 index;
    u8* base;
    s32 value;

    index = args[0];
    base = tbl;
    base += idx * 0x1C;
    value = *(s32*)(base + 0x94);
    evtSetValue(event, index, value);
    return EVT_RETURN_DONE;
}
#pragma opt_dead_assignments reset

//s32* a, s32* b, s32* c, s32* d, s32* e
USER_FUNC(btlevtcmd_DispItemIcon) {
    s32* args = event->args;
    s32 a, b, c, d, e;
    ItemEntry* entry;

    a = evtGetValue(event, args[0]);
    b = evtGetValue(event, args[1]);
    c = evtGetValue(event, args[2]);
    d = evtGetValue(event, args[3]);
    e = evtGetValue(event, args[4]);
    if (isFirstCall) {
        event->userdata[0] = e;
        entry = itemEntry((const char*)0, (ItemType)d, 0x11, (f32)a, (f32)b, (f32)c, -1, 0);
        event->userdata[1] = (s32)entry;
        return 0;
    }
    if (--event->userdata[0] > 0) {
        return 0;
    }
    itemDelete(((ItemEntry*)event->userdata[1])->name);
    return 2;
}

//s32* battleId, s32* count
USER_FUNC(btlevtcmd_CommandFlyItem) {
    s32* args = event->args;
    u8* tbl;
    s32 btlId;
    s32 rawId;
    BattleWork* wp;
    BattleUnit* unit;
    s32 count;
    s32 itemId;
    s32 kind;
    Vec pos;

    rawId = evtGetValue(event, args[0]);
    count = evtGetValue(event, args[1]);
    btlId = BattleTransID(event, rawId);
    if (isFirstCall) {
        wp = _battleWorkPointer;
        tbl = (u8*)&wp->commandMenu;
        unit = BattleGetUnitPtr(wp, btlId);
        kind = unit->currentKind;
        if (kind == UNIT_MARIO || (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX)) {
            u8* base2 = tbl;
            s32 idx = *(s32*)(tbl + 0x51C);
            base2 += idx * 0x1C;
            itemId = *(s32*)(base2 + 0x94);
        } else {
            itemId = unit->heldItem;
        }
        event->userdata[0] = count;
        BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
        pos.y += 10.f;
        if (!(unit->mAttributeFlags & kVeiled)) {
            if (unit->mAttributeFlags & kCeiling) {
                pos.y -= 5.0f;
            } else {
                pos.y += unit->mSizeMultiplier * (f32)BtlUnit_GetHeight(unit);
            }
        }
        event->userdata[1] = (s32)itemEntry(NULL, (ItemType)itemId, 0x11, pos.x, pos.y, pos.z, -1, 0);
        effItemGetEntry(0xA, pos.x, 12.0f + pos.y, pos.z - 5.0f);
        if (kind == UNIT_MARIO) {
            psndSFXOn_3D("SFX_ITEM_MARIO_USE1", &pos);
        } else if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
            psndSFXOn_3D("SFX_ITEM_PARTY_USE1", &pos);
        } else {
            psndSFXOn_3D("SFX_ENM_ITEM_USE1", &pos);
        }
        return EVT_RETURN_BLOCK;
    }
    if (--event->userdata[0] > 0) {
        return EVT_RETURN_BLOCK;
    }
    itemDelete(((ItemEntry*)event->userdata[1])->name);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* itemId
USER_FUNC(btlevtcmd_ConsumeItem) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 btlId;
    s32 itemId, slot, coins;
    Vec pos;

    btlId = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    if (unit->currentKind == UNIT_MARIO || (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX)) {
        BattleCommand* cm = &wp->commandMenu;
        s32* cmd = (s32*)((u8*)cm + cm->cursor[11].field_0 * 0x1C);
        itemId = cmd[0x94 / 4];
        slot = cmd[0x90 / 4];
        if (itemDataTable[itemId].usable_locations & 1) {
            pouchRemoveItemIndex(itemId, slot);
        }
        if (unit->badgesEquipped.Refund != 0) {
            coins = (s32)((f32)(unit->badgesEquipped.Refund * 5 + 0x46) * (f32)itemDataTable[itemId].sell_price / 100.f);
            if (coins > 0) {
                pouchAddCoin(coins);
                BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
                effCoinN64Entry(0, 0, pos.x, pos.y, pos.z, 1.0f);
            }
        }
    } else {
        unit->heldItem = 0;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* itemId
USER_FUNC(btlevtcmd_ConsumeItemReserve) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId;
    s32 itemId;
    s32 i;
    s32 coins;
    Vec pos;

    btlId = evtGetValue(event, args[0]);
    itemId = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    if (unit->currentKind == UNIT_MARIO || (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX)) {
        if (itemDataTable[itemId].usable_locations & 1) {
            for (i = 0; i < 4; i++) {
                if ((s32)wp->reserveItems[i] == itemId) {
                    break;
                }
                if ((s32)wp->reserveItems[i] == 0) {
                    wp->reserveItems[i] = itemId;
                    break;
                }
            }
        }
        if (unit->badgesEquipped.Refund != 0) {
            coins = (s32)((f32)(unit->badgesEquipped.Refund * 5 + 0x46) *
                          (f32)itemDataTable[itemId].sell_price / 100.f);
            if (coins > 0) {
                pouchAddCoin(coins);
                BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
                effCoinN64Entry(0, 0, pos.x, pos.y, pos.z, 1.0f);
            }
        }
    } else if (unit->heldItem == 0x97) {
        unit->heldItem = 0;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* index1, s32* index2
USER_FUNC(btlevtcmd_AttackDeclare) {
    s32* args = event->args;
    s32 btlId;
    s32 index1;
    s32 index2;
    s32 id1;
    s32 id2;

    btlId = evtGetValue(event, args[0]);
    BattleTransID(event, btlId);
    index1 = args[1];
    index2 = args[2];
    id1 = evtGetValue(event, index1);
    id1 = BattleTransID(event, id1);
    id2 = evtGetValue(event, index2);
    BattleAttackDeclare(&id1, &id2);
    evtSetValue(event, index1, id1);
    evtSetValue(event, index2, id2);
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

//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* b, s32* retIndex
#pragma opt_propagation off // force the a=args[4] load before retIndex=args[5] so retIndex reuses the args register
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
    result = BattlePreCheckDamage(attacker, target, part, (BattleWeapon*)value, a);
    if (result < 0) {
        return 0xFF;
    }
    evtSetValue(event, retIndex, result);
    if (evtCheckID(event->eventId)) {
        return EVT_RETURN_DONE;
    }
    return 0xFF;
}
#pragma opt_propagation reset

//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* retIndex
USER_FUNC(btlevtcmd_CommandPreCheckDamage) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* attacker;
    BattleUnit* target;
    BattleUnitPart* part;
    BattleWeapon* weapon;
    s32 btlId, targetId, partId, retIndex, a, result;

    btlId = evtGetValue(event, args[0]);
    attacker = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    targetId = evtGetValue(event, args[1]);
    target = BattleGetUnitPtr(wp, BattleTransID(event, targetId));
    partId = evtGetValue(event, args[2]);
    part = BtlUnit_GetPartsPtr(target, partId);
    a = args[3];
    retIndex = args[4];
    weapon = BattleGetSelectWeapon(wp);
    result = BattlePreCheckDamage(attacker, target, part, weapon, a);
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
    result = BattleCheckCounter(&work, attacker, target, part, (BattleWeapon*)value, &out);
    evtSetValue(event, retIndex, result);
    if (evtCheckID(event->eventId)) {
        return EVT_RETURN_DONE;
    }
    return 0xFF;
}

//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* retIndex
USER_FUNC(btlevtcmd_CommandPreCheckCounter) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* attacker;
    BattleUnit* target;
    BattleUnitPart* part;
    u8 work[0x30];
    s32 btlId, targetId, partId, retIndex, out, result;

    out = 0;
    BattleInitCounterPreCheckWork(&work);
    btlId = evtGetValue(event, args[0]);
    attacker = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    targetId = evtGetValue(event, args[1]);
    target = BattleGetUnitPtr(wp, BattleTransID(event, targetId));
    partId = evtGetValue(event, args[2]);
    part = BtlUnit_GetPartsPtr(target, partId);
    retIndex = args[4];
    result = BattleCheckCounter(&work, attacker, target, part, BattleGetSelectWeapon(wp), &out);
    evtSetValue(event, retIndex, result);
    if (evtCheckID(event->eventId)) {
        return EVT_RETURN_DONE;
    }
    return 0xFF;
}

//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* b, s32* retIndex
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
#ifdef MUST_MATCH
    // The original materializes args[4] before args[5], so MWCC lets retIndex reuse the dying
    // `args` register (r30); as a plain load the value is rematerialized at the call and retIndex
    // is pushed to r26. The volatile read pins the load here without changing the instruction.
    a = ((volatile s32*)args)[4];
#else
    a = args[4];
#endif
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

//s32* attackerId, s32* targetId, s32* partId, s32* a, s32* retIndex
USER_FUNC(btlevtcmd_CommandCheckDamage) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* attacker;
    BattleUnit* target;
    BattleUnitPart* part;
    BattleWeapon* weapon;
    s32 btlId, targetId, partId, retIndex, a, result;

    btlId = evtGetValue(event, args[0]);
    attacker = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    targetId = evtGetValue(event, args[1]);
    target = BattleGetUnitPtr(wp, BattleTransID(event, targetId));
    partId = evtGetValue(event, args[2]);
    part = BtlUnit_GetPartsPtr(target, partId);
    a = args[3];
    retIndex = args[4];
    weapon = BattleGetSelectWeapon(wp);
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

//s32* battleId, s32* partId, s32* damage, s32 flags, s32* a, s32* b
USER_FUNC(btlevtcmd_DamageDirect) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    s32 partId = evtGetValue(event, args[1]);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, partId);
    s32 damage = evtGetValue(event, args[2]);
    s32 flags = args[3];
    s32 a = evtGetValue(event, args[4]);
    s32 b = evtGetValue(event, args[5]);

    BattleDamageDirect(-5, unit, (s32)part, damage, 0, flags, a, b);
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

//s32* battleId, s32* eventCode
USER_FUNC(btlevtcmd_SetEventWait) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->waitEventCode = (void*)args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 value
USER_FUNC(btlevtcmd_SetEventEntry) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->entryEventCode = (void*)args[1];
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
USER_FUNC(btlevtcmd_SetEventUnisonPhase) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->unisonPhaseEventCode = (void*)args[1];
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* eventCode
USER_FUNC(btlevtcmd_SetEventAttack) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->attackEventCode = (void*)args[1];
    unit->field_0x2A8 = 0;
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

//s32* battleId, s32 value
USER_FUNC(btlevtcmd_SetEventCeilFall) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->ceilFallEventCode = (void*)args[1];
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
USER_FUNC(btlevtcmd_StopWaitEvent) {
    s32 btlId = evtGetValue(event, *event->args);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    if (unit->waitEventId) {
        evtDeleteID(unit->waitEventId);
        unit->waitEventId = 0;
    }
    return EVT_RETURN_DONE;
}

//=== added by agent: local prototypes for undeclared externals ===


//no params
USER_FUNC(btlevtcmd_GuardMoveEnd) {
    _battleWorkPointer->flags &= ~1;
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_WaitGuardMove) {
    return (_battleWorkPointer->flags & 1) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
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

//s32 retIndex
USER_FUNC(btlevtcmd_WaitAttackEndCheck) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 index;

    unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    index = args[0];
    evtSetValue(event, index, 1);
    if (!unit) {
        return EVT_RETURN_DONE;
    }
    if (!unit->attackEventId) {
        return EVT_RETURN_DONE;
    }
    if (evtCheckID(unit->attackEventId)) {
        evtSetValue(event, index, 0);
    }
    else {
        unit->attackEventId = 0;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 dataTag
USER_FUNC(btlevtcmd_RunDataEventChild) {
    s32* args = event->args;
    BattleUnit* unit;
    EventEntry* child;
    void* data;
    s32 id;
    s32 dataTag;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    dataTag = evtGetValue(event, args[1]);
    data = BtlUnit_GetData(unit, dataTag);
    if (data == NULL) {
        return EVT_RETURN_DONE;
    }
    child = evtChildEntry(event, data, 0);
    event->opcode = 0;
    child->unitId = id;
    return EVT_RETURN_FINISH;
}

//s32* battleId, s32 retIndex, s32 dataTag
USER_FUNC(btlevtcmd_CheckDataOfDataTable) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    s32 index;
    s32 data;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    index = args[1];
    data = evtGetValue(event, args[2]);
    evtSetValue(event, index, BtlUnit_CheckData(unit, data));
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex, s32 dataTag
USER_FUNC(btlevtcmd_GetDataFromDataTable) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    s32 index;
    s32 data;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    index = args[1];
    data = evtGetValue(event, args[2]);
    evtSetValue(event, index, (s32)BtlUnit_GetData(unit, data));
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_PhaseEventStartDeclare) {
    s32 btlId = evtGetValue(event, *event->args);

    BattlePhaseEventStartDeclare(BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId)));
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex
USER_FUNC(btlevtcmd_GetConfuseActEvent) {
    s32* args = event->args;
    s32 retIndex;
    BattleUnit* unit;
    s32 id, i;
    void* evt;

    i = 0;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    retIndex = args[1];
    evt = 0;
    while (evt == 0) {
        evt = BattleSetConfuseAct(_battleWorkPointer, unit);
        i++;
        if (i > 20) {
            evt = (void*)subsetevt_confuse_flustered;
        }
    }
    evtSetValue(event, retIndex, (s32)evt);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* x, s32* y, s32* z, s32* retIndex
USER_FUNC(btlevtcmd_MarioJumpParam) {
    BattleUnit* unit;
    s32 btlId, id;
    s32* args = event->args;
    f32 x2, z2;
    s32 retIndex, retIndexArg;
    f32 x, y, z;
    f32 dist;
    s32 frames;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    x2 = (f32)evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    z2 = (f32)evtGetValue(event, args[3]);
    retIndexArg = args[4];
    retIndex = evtGetValue(event, retIndexArg);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    dist = distABf(x, z, x2, z2);
    frames = (retIndex != 0) ? retIndex : (s32)(dist / unit->mMoveSpeedXZ);
    if (dist > 60.f) {
        frames += (s32)(0.15f * (dist - 60.f));
    }
    if (retIndex == 0 || frames > retIndex) {
        retIndex = frames;
    }
    if (retIndex != EVTDAT_POINTER_MAX) {
        evtSetValue(event, retIndexArg, retIndex);
    }
    return 2;
}

//s32* battleId, s32* tx, s32* ty, s32* tz, s32* frame, s32* type, s32* a
// Type 1 skips the setup; 2 and 4 stretch the arc. The `h` temp holding each sin result and the two
// `*=` forms pin the target's FPRs and load order; `pos` is the zero Vec (retail vec3_802edea0) the target reads
// at vec3_802ede88+0x18 (0x18 is declared only as an unnamed gap in this unit's symbols).
USER_FUNC(btlevtcmd_MarioJumpPosition) {
    s32* args = event->args;
    BattleWork* wp;
    BattleUnit* unit;
    s32 btlId, id, frame, type, a;
    f32 tx, ty, tz;
    f32 x, y, z;
    f32 remain;
    f32 h;
    Vec pos2, pos;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    frame = evtGetValue(event, args[4]);
    type = evtGetValue(event, args[5]);
    a = evtGetValue(event, args[6]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, id);
    if (isFirstCall) {
        event->blocked = 1;
        if (type == 1) {
            event->blocked = 2;
        } else {
            BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
            BtlUnit_GetPos(unit, &x, &y, &z);
            BtlUnit_SetMoveCurrentPos(unit, x, y, z);
            unit->moveAngle = angleABf(x, z, tx, tz);
            unit->moveDist = distABf(x, z, tx, tz);
            if (tx - x > 0.0f) {
                unit->moveDirection = 1;
            } else if (tx - x < 0.0f) {
                unit->moveDirection = -1;
            }
            unit->moveFrames = frame;
            if (unit->moveFrames == 0) {
                unit->moveFrames = (s32)(unit->moveDist / unit->mMoveSpeedXZ);
                remain = unit->moveDist -
                         (f32)unit->moveFrames * unit->mMoveSpeedXZ;
            } else {
                unit->mMoveSpeedXZ = unit->moveDist / (f32)unit->moveFrames;
                remain = unit->moveDist -
                         (f32)unit->moveFrames * unit->mMoveSpeedXZ;
            }
            if (unit->moveFrames == 0) {
                return 2;
            }
            unit->moveStep.x =
                (unit->mMoveTargetPos.x - unit->mMoveCurrentPos.x) / (f32)unit->moveFrames;
            unit->moveStep.y =
                (unit->mMoveTargetPos.y - unit->mMoveCurrentPos.y) / (f32)unit->moveFrames;
            unit->moveStep.z =
                (unit->mMoveTargetPos.z - unit->mMoveCurrentPos.z) / (f32)unit->moveFrames;
            unit->mMoveSpeedY = 0.0f;
            unit->mFallAccel = 3.1415927f / (f32)unit->moveFrames;
            unit->mMoveSpeedXZ = unit->mMoveSpeedXZ + remain / (f32)unit->moveFrames;
            if (type < 3) {
                unit->hopAngle = 90.f;
                unit->hopAngleStep = 360.f / (f32)unit->moveFrames;
                h = unit->moveDist;
                h -= 20.0;
                h /= 6.0;
                h += 47.0;
                unit->moveProgress = h;
                if (type == 2) {
                    unit->moveProgress *= 1.12;
                }
                unit->hopY = 0.0f;
                unit->hopY = 0.0f;
                h = (f32)sin(6.2831855f * unit->hopAngle / 360.f);
                unit->mMoveSpeedY = unit->mFallAccel * (0.53 * h + 1.0) + unit->mMoveSpeedY;
            } else {
                unit->hopAngle = 90.f;
                unit->hopAngleStep = (f32)(360 / unit->moveFrames);
                h = unit->moveDist;
                h -= 20.0;
                h /= 6.0;
                h += 47.0;
                unit->moveProgress = h;
                if (type == 4) {
                    unit->moveProgress *= 1.25;
                }
                unit->hopY = 0.0f;
                unit->hopY = 0.0f;
                h = (f32)sin(6.2831855f * unit->hopAngle / 360.f);
                unit->mMoveSpeedY = unit->mFallAccel * (0.8 * h + 1.0) + unit->mMoveSpeedY;
            }
            BtlUnit_LoadSeMode(2, a, &unit->seModeSrc, &unit->seMode);
            if (unit->seMode.sfxA != 0) {
                pos = (Vec){ 0.0f, 0.0f, 0.0f };
                pos.x = unit->mMoveCurrentPos.x;
                pos.y = unit->mMoveCurrentPos.y;
                pos.z = unit->mMoveCurrentPos.z;
                pos2 = pos;
                psndSFXOn_3D((const char*)unit->seMode.sfxA, &pos2);
            }
        }
    }
    switch ((s8)event->blocked) {
    case 1:
        unit->mMoveCurrentPos.x += unit->moveStep.x;
        unit->mMoveCurrentPos.y += unit->moveStep.y;
        unit->mMoveCurrentPos.z += unit->moveStep.z;
        BtlUnit_GetPos(unit, &x, &y, &z);
        unit->hopY = y;
        x = unit->mMoveCurrentPos.x;
        y = unit->moveProgress * (f32)sin(unit->mMoveSpeedY) + unit->mMoveCurrentPos.y;
        z = unit->mMoveCurrentPos.z;
        BtlUnit_SetPos(unit, x, y, z);
        if (unit->mMoveTargetPos.y > y && unit->moveFrames < 3) {
            y = unit->mMoveTargetPos.y;
            BtlUnit_SetPos(unit, x, y, z);
        }
        unit->hopY = y;
        if (type < 3) {
            h = (f32)sin(6.2831855f * unit->hopAngle / 360.f);
            unit->mMoveSpeedY = unit->mFallAccel * (0.53 * h + 1.0) + unit->mMoveSpeedY;
        } else {
            h = (f32)sin(6.2831855f * unit->hopAngle / 360.f);
            unit->mMoveSpeedY = unit->mFallAccel * (0.8 * h + 1.0) + unit->mMoveSpeedY;
        }
        unit->hopAngle += unit->hopAngleStep;
        unit->hopAngle = reviseAngle(unit->hopAngle);
        (unit->moveFrames)--;
        if (unit->moveFrames == 0) {
            y = unit->mMoveTargetPos.y;
            BtlUnit_SetPos(unit, x, y, z);
            unit->mMoveSpeedY = -(unit->hopY - unit->hopY);
            unit->mFallAccel = 1.8f;
            return 1;
        }
        break;
    case 2:
        if (!BtlUnit_GetACPossibility(unit)) {
            return 2;
        }
        unit->mMoveSpeedY = -(unit->hopY - unit->hopY);
        unit->mFallAccel = 1.8f;
        unit->moveFrames = 1;
        unit->hopAngle = 90.f;
        unit->hopAngleStep = (f32)(360 / unit->moveFrames);
        unit->moveProgress = (f32)(__fabs(unit->hopY - unit->hopY) / 16.5);
        BtlUnit_GetPos(unit, &unit->mMoveCurrentPos.x, &unit->mMoveCurrentPos.y, &unit->mMoveCurrentPos.z);
        event->blocked = 3;
        break;
    case 3:
        h = (f32)sin(6.2831855f * unit->hopAngle / 360.f);
        unit->mMoveCurrentPos.x = (f32)(unit->mMoveCurrentPos.x +
                                        (unit->moveProgress * h) / 33.0);
        unit->mMoveCurrentPos.y -= unit->moveProgress *
                                   (f32)sin(6.2831855f * unit->hopAngle / 360.f);
        unit->hopAngle += unit->hopAngleStep;
        unit->hopAngle = reviseAngle(unit->hopAngle);
        BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
        (unit->moveFrames)--;
        if (unit->moveFrames == 0) {
            return 1;
        }
        break;
    }
    return 0;
}

//s32* battleId, s32* targetBtlId, s32* partId, s32* x, s32* y, s32* z
// The translated id must reuse the `btlId` variable (not a separate `id`): with a separate variable
// MWCC colours `unit` into the dead `args` register r24 and stages the BattleTransID result through
// r0; reusing btlId puts the translated id in r24 and unit in r25 exactly as the target does.
// `double_to_int_802ee038` prints as an anonymous `@NNNN@` constant but objdiff ignores that
// (FallPartsPosition is 100% likewise).
USER_FUNC(btlevtcmd_GetTakeoffPosition) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnit* targetUnit;
    s32 btlId;
    BattleUnitPart* targetPart;
    s32 result;
    s32 indexX;
    s32 indexY;
    s32 indexZ;
    BattleUnitPart* part;
    s32 targetId;
    s32 partId;
    BattleUnit* targetUnit2;
    s8 belong;
    u8 sampling[0xAD0];
    f32 x, y, z;
    f32 tmp;
    f32 unitX;
    f32 hitX;
    s32 outId;
    s32 outPartId;

    btlId = evtGetValue(event, args[0]);
    targetId = evtGetValue(event, args[1]);
    partId = evtGetValue(event, args[2]);
    indexX = args[3];
    indexY = args[4];
    indexZ = args[5];
    btlId = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(wp, btlId);
    BtlUnit_GetPos(unit, &unitX, &tmp, &tmp);
    targetId = BattleTransID(event, targetId);
    targetUnit = BattleGetUnitPtr(wp, targetId);
    part = BtlUnit_GetPartsPtr(targetUnit, partId);
    BtlUnit_GetHomePos(unit, &x, &y, &z);
    BtlUnit_GetHitPos(targetUnit, part, &x, &tmp, &z);

    belong = (s8)BtlUnit_GetEnemyBelong(unit);
    result = BattleSamplingEnemy(sampling, NULL, btlId, belong, 0x1001240, 0x1000000,
                                 -wp->alliances[belong].attackDirection);
    BattleChoiceSamplingEnemy(sampling, 4, &outId, &outPartId);
    if (*(s8*)(sampling + 0xA6C) > 0) {
        outId = BattleTransID(event, outId);
        targetUnit2 = BattleGetUnitPtr(wp, outId);
        targetPart = BtlUnit_GetPartsPtr(targetUnit2, outPartId);
        BtlUnit_GetHitPos(targetUnit2, targetPart, &hitX, &tmp, &tmp);
        if (__fabs(unitX - x) > __fabs(unitX - hitX)) {
            x = hitX;
        }
        if (part == targetPart) {
            if (result != 0) {
                x += (f32)(wp->alliances[belong].attackDirection * -60);
            } else {
                x += (f32)(wp->alliances[belong].attackDirection * 60);
            }
        } else {
            if (result != 0) {
                x += (f32)(wp->alliances[belong].attackDirection * -30);
            } else {
                x += (f32)(wp->alliances[belong].attackDirection * 30);
            }
        }
    } else {
        x += (f32)(wp->alliances[belong].attackDirection * 60);
    }
    evtSetValue(event, indexX, (s32)x);
    evtSetValue(event, indexY, (s32)y);
    evtSetValue(event, indexZ, (s32)z);
    return 2;
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

//s32* battleId, s32* partId, s32* tx, s32* ty, s32* tz, s32* frame, s32* a
USER_FUNC(btlevtcmd_JumpPartsPosition) {
    s32 btlId, id;
    s32* args = event->args;
    s32 partId, frame, a;
    BattleUnitPart* part;
    f32 tx, ty, tz;
    f32 x, y, z;
    Vec posb;
    Vec posb2;
    Vec pos;
    Vec pos2;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    tx = (f32)evtGetValue(event, args[2]);
    ty = (f32)evtGetValue(event, args[3]);
    tz = (f32)evtGetValue(event, args[4]);
    frame = evtGetValue(event, args[5]);
    a = evtGetValue(event, args[6]);
    part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, id), partId);
    if (isFirstCall) {
        event->blocked = 1;
        if (frame != 0) {
            part->moveFrames = frame;
        }
        BtlUnit_SetPartsMoveTargetPos(part, tx, ty, tz);
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        BtlUnit_SetPartsMoveCurrentPos(part, x, y, z);
        part->moveAngle = angleABf(x, z, tx, tz);
        part->moveDist = distABf(x, z, tx, tz);
        if (tx - x > 0.0f) {
            part->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            part->moveDirection = -1;
        }
        y = ty - y;
        if (part->moveFrames == 0) {
            part->moveFrames =
                (s32)(part->moveDist / part->mMoveSpeedXZ);
            part->moveDist -=
                (f32)part->moveFrames * part->mMoveSpeedXZ;
        } else {
            part->mMoveSpeedXZ =
                part->moveDist / (f32)part->moveFrames;
            part->moveDist -=
                (f32)part->moveFrames * part->mMoveSpeedXZ;
        }
        if (part->moveFrames == 0) {
            return 2;
        }
        {
            f32 t = part->mFallAccel * (f32)part->moveFrames;
            part->mMoveSpeedY = y / (f32)part->moveFrames +
                                          t / 2.0f;
        }
        part->mMoveSpeedXZ =
            part->mMoveSpeedXZ +
            part->moveDist / (f32)part->moveFrames;
        BtlUnit_LoadSeMode(2, a, &part->seModeSrc, &part->seMode);
        if (part->seMode.sfxA != 0) {
            pos = (Vec){ 0.0f, 0.0f, 0.0f };
            pos.x = part->mMoveCurrentPos.x;
            pos.y = part->mMoveCurrentPos.y;
            pos.z = part->mMoveCurrentPos.z;
            posb = pos;
            psndSFXOn_3D((const char*)part->seMode.sfxA, &posb);
        }
    }
    part->mMoveCurrentPos.y += part->mMoveSpeedY;
    part->mMoveSpeedY -= part->mFallAccel;
    if (part->mMoveSpeedY < 0.0f) {
#ifdef MUST_MATCH
        // The volatile pins the current.y load before the target.y load; a plain read lets the
        // scheduler emit target.y first, which is the only remaining difference.
        f32 cur = *(volatile f32*)&part->mMoveCurrentPos.y;
#else
        f32 cur = part->mMoveCurrentPos.y;
#endif
        f32 tgt = part->mMoveTargetPos.y;
        if (cur < tgt) {
            part->mMoveCurrentPos.y = tgt;
        }
    }
    btlMovePos(&part->mMoveCurrentPos.x, &part->mMoveCurrentPos.z, part->mMoveSpeedXZ,
               part->moveAngle);
    BtlUnit_SetPartsPos(part, part->mMoveCurrentPos.x, part->mMoveCurrentPos.y,
                        part->mMoveCurrentPos.z);
    part->moveFrames--;
    if (part->moveFrames <= 0) {
        BtlUnit_SetPartsPos(part, part->mMoveTargetPos.x, part->mMoveTargetPos.y,
                            part->mMoveTargetPos.z);
        if (part->seMode.sfxB != 0) {
            pos2 = (Vec){ 0.0f, 0.0f, 0.0f };
            pos2.x = part->mMoveTargetPos.x;
            pos2.y = part->mMoveTargetPos.y;
            pos2.z = part->mMoveTargetPos.z;
            posb2 = pos2;
            psndSFXOn_3D((const char*)part->seMode.sfxB, &posb2);
        }
        return 1;
    }
    return 0;
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
    part->moveFrames = a;
    part->mMoveSpeedXZ = b;
    part->mFallAccel = c;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId
USER_FUNC(btlevtcmd_JumpPartsContinue) {
    s32* args = event->args;
    s32 btlId, partId;
    BattleUnitPart* part;
    f32 x, y, z, floor;

    btlId = evtGetValue(event, args[0]);
    btlId = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, btlId), partId);
    if (isFirstCall) {
        event->blocked = 1;
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        BtlUnit_SetPartsMoveCurrentPos(part, x, y, z);
    }
    part->mMoveCurrentPos.y += part->mMoveSpeedY;
    part->mMoveSpeedY -= part->mFallAccel;
    btlMovePos(&part->mMoveCurrentPos.x, &part->mMoveCurrentPos.z, part->mMoveSpeedXZ, part->moveAngle);
    BtlUnit_SetPartsPos(part, part->mMoveCurrentPos.x, part->mMoveCurrentPos.y, part->mMoveCurrentPos.z);
    floor = BattleGetFloorHeight(_battleWorkPointer, part->mMoveCurrentPos.x, part->mMoveCurrentPos.y, part->mMoveCurrentPos.z);
    if (part->mMoveCurrentPos.y < floor) {
        part->mMoveCurrentPos.y = floor;
        BtlUnit_SetPartsPos(part, part->mMoveCurrentPos.x, part->mMoveCurrentPos.y, part->mMoveCurrentPos.z);
        return 1;
    }
    return 0;
}

//s32* battleId, s32* partId, s32* tx, s32* ty, s32* tz, s32* frame
// Declaration order `unit; s32 btlId, id; s32* args; s32 partId, frame; BattleUnitPart* part;` is
// needed for the callee-saved allocation; the speed guard must be written with the constant on the
// left (`0.0f == part->mMoveSpeedXZ`).
USER_FUNC(btlevtcmd_FallPartsPosition) {
    BattleUnit* unit;
    s32 btlId, id;
    s32* args = event->args;
    s32 partId, frame;
    BattleUnitPart* part;
    f32 tx, ty, tz;
    f32 x, y, z;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    tx = (f32)evtGetValue(event, args[2]);
    ty = (f32)evtGetValue(event, args[3]);
    tz = (f32)evtGetValue(event, args[4]);
    frame = evtGetValue(event, args[5]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partId);
    if (isFirstCall) {
        event->blocked = 1;
        part->moveFrames = frame;
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        BtlUnit_SetPartsMoveCurrentPos(part, x, y, z);
        BtlUnit_SetPartsMoveTargetPos(part, tx, ty, tz);
        part->moveAngle = angleABf(x, z, tx, tz);
        part->moveDist = distABf(x, z, tx, tz);
        if (tx - x > 0.0f) {
            part->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            part->moveDirection = -1;
        }
        y = ty - y;
        if (part->moveFrames == 0) {
            if (0.0f == part->mMoveSpeedXZ) {
                part->mMoveSpeedXZ = 1.0f;
            }
            part->moveFrames =
                (s32)(part->moveDist / part->mMoveSpeedXZ);
            if (part->moveFrames == 0) {
                part->moveFrames = 1;
            }
        } else {
            part->mMoveSpeedXZ =
                part->moveDist / (f32)part->moveFrames;
        }
        part->mMoveSpeedY = 0.0f;
        part->mFallAccel = (y / (f32)part->moveFrames -
                            part->mMoveSpeedY) /
                           ((f32)(-part->moveFrames) / 2.0f);
    }
    part->mMoveCurrentPos.y += part->mMoveSpeedY;
    part->mMoveSpeedY -= part->mFallAccel;
    btlMovePos(&part->mMoveCurrentPos.x, &part->mMoveCurrentPos.z, part->mMoveSpeedXZ,
               part->moveAngle);
    BtlUnit_SetPartsPos(part, part->mMoveCurrentPos.x, part->mMoveCurrentPos.y,
                        part->mMoveCurrentPos.z);
    part->moveFrames--;
    if (part->moveFrames <= 0) {
        BtlUnit_SetPartsPos(part, part->mMoveTargetPos.x, part->mMoveTargetPos.y,
                            part->mMoveTargetPos.z);
        return 1;
    }
    return 0;
}

//s32* battleId, s32* partNo, s32* tx, s32* ty, s32* tz, s32* frame, s32* height, s32* interp, s32* curve, s32* a
// MATCHING. All callee-saved GPRs land where the target has them (id r28, partNo r27, part r27, aboveFloor
// r28) because of the within-statement declarator order below. The 0.0f literals give the compares the
// constant FPRs the target uses, and the block-scoped `dist2` in the curve != 0 branch keeps the sqrt result in
// f4 for the zero test with a copy in f3 for the division (`fmr f3, f4`).
USER_FUNC(btlevtcmd_DivePartsPosition) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 aboveFloor, curve, id, partNo, btlId, height, interp, a, frame;
    BattleUnitPart* part;
    s32 left;
    f32 tx, ty, tz;
    f32 x, y, z;
    f32 dx, dy, dz;
    f32 dist, dist2;
    f32 h, ang, offX, offZ, floor;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partNo = evtGetValue(event, args[1]);
    tx = (f32)evtGetValue(event, args[2]);
    ty = (f32)evtGetValue(event, args[3]);
    tz = (f32)evtGetValue(event, args[4]);
    frame = evtGetValue(event, args[5]);
    height = evtGetValue(event, args[6]);
    interp = evtGetValue(event, args[7]);
    curve = evtGetValue(event, args[8]);
    a = evtGetValue(event, args[9]);
    aboveFloor = 0;
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, partNo);
    if (interp >= 100) {
        aboveFloor = 1;
        interp -= 100;
    }
    if (isFirstCall) {
        event->blocked = 1;
        part->moveFrames = frame;
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        BtlUnit_SetPartsMoveStartPos(part, x, y, z);
        BtlUnit_SetPartsMoveCurrentPos(part, x, y, z);
        BtlUnit_SetPartsMoveTargetPos(part, tx, ty, tz);
        dx = x - tx;
        dy = y - ty;
        dz = z - tz;
        dist = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        part->moveDist = dist;
        if (part->moveFrames == 0) {
            part->moveFrames = (s32)(part->moveDist / part->mMoveSpeedXZ);
        } else {
            part->mMoveSpeedXZ = part->moveDist / (f32)part->moveFrames;
        }
        if (part->moveFrames == 0) {
            return 2;
        }
        part->moveProgress = 0.0f;
        part->moveAngle = 0.0f;
        if (tx - x > 0.0f) {
            part->moveDirection = 1;
        } else if (tx - x < 0.0f) {
            part->moveDirection = -1;
        }
        BtlUnit_LoadSeMode(3, a, &part->seModeSrc, &part->seMode);
    }
    part->mMoveCurrentPos.x = intpl_sub(interp, (s32)part->moveProgress, part->moveFrames,
                                        part->mMoveStartPos.x, part->mMoveTargetPos.x);
    part->mMoveCurrentPos.y = intpl_sub(interp, (s32)part->moveProgress, part->moveFrames,
                                        part->mMoveStartPos.y, part->mMoveTargetPos.y);
    part->mMoveCurrentPos.z = intpl_sub(interp, (s32)part->moveProgress, part->moveFrames,
                                        part->mMoveStartPos.z, part->mMoveTargetPos.z);
    if (aboveFloor) {
        floor = BattleGetFloorHeight(_battleWorkPointer, part->mMoveCurrentPos.x, part->mMoveCurrentPos.y,
                                     part->mMoveCurrentPos.z);
        if (part->mMoveCurrentPos.y < floor) {
            part->moveProgress = (f32)part->moveFrames;
            BtlUnit_SetPartsMoveTargetPos(part, part->mMoveCurrentPos.x, floor, part->mMoveCurrentPos.z);
        }
    }
    part->moveProgress += 1.0f;
    if (part->moveProgress > (f32)part->moveFrames) {
        BtlUnit_SetPartsPos(part, part->mMoveTargetPos.x, part->mMoveTargetPos.y, part->mMoveTargetPos.z);
        _PartsMoveSoundControlEnd(part);
        return 2;
    }
    if (curve == 0) {
        dx = part->mMoveTargetPos.x - part->mMoveCurrentPos.x;
        dy = part->mMoveTargetPos.y - part->mMoveCurrentPos.y;
        dz = part->mMoveTargetPos.z - part->mMoveCurrentPos.z;
        dist2 = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        if (0.0f == dist2) {
            dist2 = 1.0f;
        }
        if (0.0f == part->moveDist) {
            part->moveDist = 1.0f;
        }
        h = sinfd(180.f * (1.0f - dist2 / part->moveDist));
        if (height == 0) {
            h = 0.0f;
        }
        if (height < 0) {
            h = -h * (f32)(-height);
        }
        if (height > 0) {
            h = h * (f32)height;
        }
        BtlUnit_SetPartsPos(part, part->mMoveCurrentPos.x, part->mMoveCurrentPos.y + h, part->mMoveCurrentPos.z);
    } else {
        // Block-scoped dist2 (shadows the function-scope one) is what makes mwcc keep the sqrt result in f4 for
        // the zero test and a copy in f3 for the division, as the target does.
        f32 dist2;
        dx = part->mMoveTargetPos.x - part->mMoveCurrentPos.x;
        dy = part->mMoveTargetPos.y - part->mMoveCurrentPos.y;
        dz = part->mMoveTargetPos.z - part->mMoveCurrentPos.z;
        dist2 = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        if (part->mMoveTargetPos.x - part->mMoveStartPos.x > 0.0f) {
            left = 1;
        } else if (part->mMoveTargetPos.x - part->mMoveStartPos.x < 0.0f) {
            left = 0;
        } else if (part->mMoveTargetPos.y - part->mMoveStartPos.y < 0.0f) {
            left = 0;
        } else {
            left = 1;
        }
        if (0.0f == dist2) {
            dist2 = 1.0f;
        }
        if (0.0f == part->moveDist) {
            part->moveDist = 1.0f;
        }
        h = sinfd(180.f * (1.0f - dist2 / part->moveDist));
        if (left) {
            ang = 360.f * atan2f_safety(part->mMoveTargetPos.x - part->mMoveStartPos.x,
                                                     part->mMoveTargetPos.y - part->mMoveStartPos.y) /
                  6.2831855f;
        } else {
            ang = 180.f + 360.f * atan2f_safety(part->mMoveTargetPos.x - part->mMoveStartPos.x,
                                                                          part->mMoveTargetPos.y - part->mMoveStartPos.y) /
                                           6.2831855f;
        }
        offX = h * sinfd(ang);
        offZ = h * cosfd(180.f + ang);
        if (height == 0) {
            offX = 0.0f;
            offZ = offX;
        } else {
            offX = offX * (f32)height;
            offZ = offZ * (f32)height;
        }
        BtlUnit_SetPartsPos(part, part->mMoveCurrentPos.x + offZ, part->mMoveCurrentPos.y + offX, part->mMoveCurrentPos.z);
    }
    _PartsMoveSoundControl(part);
    return 0;
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

//s32 retIndex
USER_FUNC(btlevtcmd_AcGetDifficulty) {
    s32 index = *event->args;

    evtSetValue(event, index, BattleActionCommandGetDifficulty(_battleWorkPointer));
    return EVT_RETURN_DONE;
}

//s32 flag
USER_FUNC(btlevtcmd_AcSetFlag) {
    _battleWorkPointer->actionCommands.acFlag = *event->args;
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
            wp->actionCommands.acParams[i] = v;
        }
    }
    return EVT_RETURN_DONE;
}

//s32 index, s32 value
USER_FUNC(btlevtcmd_AcSetParam) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    wp->actionCommands.acParams[index] = value;
    return EVT_RETURN_DONE;
}

//s32 index, s32 retIndex
USER_FUNC(btlevtcmd_AcGetParam) {
    BattleWork* wp;
    s32* args;
    s32 index;
    s32 value;

    args = event->args;
    wp = _battleWorkPointer;
    index = evtGetValue(event, args[0]);
    value = wp->actionCommands.acParams[index];

    if (args[1] != EVTDAT_POINTER_MAX) {
        evtSetValue(event, args[1], value);
    }
    return EVT_RETURN_DONE;
}

//s32 g0, s32 g1, s32 g2, s32 g3
USER_FUNC(btlevtcmd_AcSetGaugeParam) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;

    wp->actionCommands.acGaugeParams[0] = evtGetValue(event, args[0]);
    wp->actionCommands.acGaugeParams[1] = evtGetValue(event, args[1]);
    wp->actionCommands.acGaugeParams[2] = evtGetValue(event, args[2]);
    wp->actionCommands.acGaugeParams[3] = evtGetValue(event, args[3]);
    return EVT_RETURN_DONE;
}

//s32 index, s32 retIndex
USER_FUNC(btlevtcmd_AcGetOutputParam) {
    BattleWork* wp;
    s32* args;
    s32 index;
    s32 value;

    args = event->args;
    wp = _battleWorkPointer;
    index = evtGetValue(event, args[0]);
    value = wp->actionCommands.outputParams[index];

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

    wp->actionCommands.outputParams[index] = value;
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

//no params
USER_FUNC(btlevtcmd_StopAC) {
    BattleActionCommandStop(_battleWorkPointer);
    BattleAcHelpSetDispType(0);
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

//s32* battleId, BattleWeapon* weapon
USER_FUNC(btlevtcmd_ResultACDefence) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    BattleWeapon* weapon;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    weapon = (BattleWeapon*)evtGetValue(event, args[1]);
    BattleActionCommandCheckDefence(unit, weapon);
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_GetResultAC) {
    evtSetValue(event, *event->args, _battleWorkPointer->actionCommands.result);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 exp, s32 retIndex
USER_FUNC(btlevtcmd_GetResultPrizeLv) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit = NULL;
    s32 btlId;
    s32 exp;
    s32 index;

    btlId = evtGetValue(event, args[0]);
    if (btlId != -1 && btlId != -5) {
        unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    }
    exp = evtGetValue(event, args[1]);
    index = args[2];
    evtSetValue(event, index, BattleActionCommandGetPrizeLv(wp, unit, exp));
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_GetResultACDefence) {
    s32 index = *event->args;

    evtSetValue(event, index, BattleActionCommandGetDefenceResult());
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_GetResultCountAC) {
    evtSetValue(event, *event->args, _battleWorkPointer->actionCommands.mResultCount);
    return EVT_RETURN_DONE;
}

//s32* weaponIndex, s32 acResult
USER_FUNC(btlevtcmd_AudienceDeclareACResult) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleWeapon* weapon;
    u8* counter;
    s32 result;

    weapon = (BattleWeapon*)evtGetValue(event, args[0]);
    result = evtGetValue(event, args[1]);
    counter = &wp->field_16F1C[0x3C - 0x1C];
    if (result != -1) {
        if (counter[0] < 0xC8) {
            counter[0]++;
        }
        wp->field_19064[0] = 1;
    }
    else {
        if (counter[1] < 0xC8) {
            counter[1]++;
        }
        wp->field_19064[0] = 0;
    }
    BattleActionCommandDeclareACResult(wp, weapon, result);
    return EVT_RETURN_DONE;
}

//s32* result, s32* flag, f32* dx, f32* dy, f32* dz
USER_FUNC(btlevtcmd_AudienceDeclareAcrobatResult) {
    s32* args = event->args;
    BattleWork* wp;
    s32 a, b;
    BattleUnit* unit;
    f32 dx, dy, dz;
    Vec pos;
    EffectEntry* entry;
    StarPowerInfo* bonus;

    a = evtGetValue(event, args[0]);
    b = evtGetValue(event, args[1]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, event->unitId);
    bonus = &wp->impendingBonuses;
    dx = evtGetFloat(event, args[2]);
    dy = evtGetFloat(event, args[3]);
    dz = evtGetFloat(event, args[4]);
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    if (b != 0) {
        bonus->mStylishCommandMultiplier = *(s8*)(a + 0x18);
        bonus->field_0xB = *(s8*)(a + 0x1B);
        BattleAudience_Case_AcrobatGood();
        pos.y += (f32)BtlUnit_GetHeight(unit);
        pos.x += (f32)(BtlUnit_GetWidth(unit) / 2);
        if (pos.x > 150.0f) {
            pos.x = 150.0f;
        }
        entry = effNiceEntry(7, pos.x + dx, pos.y + dy, pos.z + dz);
        ((NiceEffectWork*)entry->userdata)->scale = 0.75f;
    } else {
        BattleAudience_Case_AcrobatBad();
#ifdef MUST_MATCH
        psndSFXOn_3D("SFX_BTL_ACROBAT_MISS1", &pos);
#else
        psndSFXOn_3D(str_SFX_BTL_ACROBAT_MISS1, &pos);
#endif
    }
    return 2;
}

// The original addresses its sound names from the retail vec3_802ede88 base.
// The SFX_SYSTEM names are pooled string literals in the original (ACSuccessEffect reads them at
// vec3+0x1c0..0x24c), so MUST_MATCH spells them as literals and the natural build uses the symbols.

//s32* type, s32* x, s32* y, s32* z
// The angle halving is written as `angle / 2.0f` (not `angle * 0.5f`): it keeps 160 in f0, pos.x in f1 and
// the 0.5 in f2 when the clamp is evaluated, as the original does.
USER_FUNC(btlevtcmd_ACSuccessEffect) {
    s32* args = event->args;
    s32 type;
    Vec pos;
    f32 scale;
    f32 angle;
    const char* sfx;
    s32 id;
    EffectEntry* entry;

    type = evtGetValue(event, args[0]);
    scale = 0.75f;
    pos.x = (f32)evtGetValue(event, args[1]);
    pos.y = (f32)evtGetValue(event, args[2]);
    pos.z = (f32)evtGetValue(event, args[3]);
    switch (type) {
    case 0:
        id = 0;
        sfx = ACSE_SYSSFX(str_SFX_SYSTEM_NICE_25PERCENT1, "SFX_SYSTEM_NICE_25PERCENT1");
        scale *= 0.3f;
        angle = 90.f;
        break;
    case 1:
        id = 0;
        sfx = ACSE_SYSSFX(str_SFX_SYSTEM_NICE_50PERCENT1, "SFX_SYSTEM_NICE_50PERCENT1");
        scale *= 0.6f;
        angle = 90.f;
        break;
    case 2:
        id = 0;
        sfx = ACSE_SYSSFX(str_SFX_SYSTEM_NICE1, "SFX_SYSTEM_NICE1");
        scale *= 1.2f;
        angle = 90.f;
        break;
    case 3:
        id = 1;
        sfx = ACSE_SYSSFX(str_SFX_SYSTEM_GOOD1, "SFX_SYSTEM_GOOD1");
        scale *= 1.2f;
        angle = 90.f;
        break;
    case 4:
        id = 2;
        sfx = ACSE_SYSSFX(str_SFX_SYSTEM_GREAT1, "SFX_SYSTEM_GREAT1");
        scale *= 1.2f;
        angle = 120.f;
        break;
    case 5:
        id = 3;
        sfx = ACSE_SYSSFX(str_SFX_SYSTEM_WONDERFUL1, "SFX_SYSTEM_WONDERFUL1");
        angle = 150.0f;
        break;
    case 6:
        id = 4;
        sfx = ACSE_SYSSFX(str_SFX_SYSTEM_EXCELLENT1, "SFX_SYSTEM_EXCELLENT1");
        angle = 180.f;
        break;
    default:
        return 2;
    }
    angle = angle * scale;
    angle = angle / 2.0f;
    if (pos.x + angle > 160.0f) {
        pos.x = 160.0f - angle;
    }
    if (pos.y < 80.f) {
        entry = effNiceEntry(id, pos.x - angle, 50.f + pos.y, pos.z);
    } else {
        entry = effNiceEntry(id, pos.x - angle, pos.y - 50.f, pos.z);
    }
    ((NiceEffectWork*)entry->userdata)->scale = scale;
    psndSFX_pit(psndSFXOn_3D(sfx, &pos), 0);
    return 2;
}

//s32* onoff, s32 flags
USER_FUNC(btlevtcmd_ac_timing_flag_onoff) {
    s32* args = event->args;
    s32 onoff;
    u32 flags;
    u32* p;

    onoff = evtGetValue(event, args[0]);
    flags = args[1];
    p = &((AcTimingADecWork*)_battleWorkPointer->actionCommands.field_0x2BC)->flags;
    if (onoff) {
        *p |= flags;
    }
    else {
        *p &= ~flags;
    }
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_ac_timing_get_success_frame) {
    s32* args = event->args;
    s32 index = args[0];
    s32 frame = battleAcTimingA_GetSuccessFrame(_battleWorkPointer);

    evtSetValue(event, index, frame);
    return EVT_RETURN_DONE;
}

//s32 acHelp
USER_FUNC(btlevtcmd_ACHelpSet) {
    s32 value = evtGetValue(event, *event->args);

    BattleAcHelpSetHelp(*(const char**)((u8*)value + 0x70));
    BattleAcHelpSetDispType(EVT_RETURN_DONE);
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

//s32* battleId, s32* partId, s32* group
USER_FUNC(btlevtcmd_AnimeChangeGroup) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;
    s32 group;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    group = evtGetValue(event, args[2]);
    if (isFirstCall) {
        if (part->poseId != -1) {
            animPoseRelease(part->poseId);
            part->poseId = -1;
        }
        animEffectAsync((const char*)group, 1);
    }
    if (animEffectAsync((const char*)group, 1) == 0) {
        return EVT_RETURN_BLOCK;
    }
    part->poseId = animPoseEntry((const char*)group, 1);
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

//s32* battleId, s32* partId, s32* poseName
USER_FUNC(btlevtcmd_AnimeChangePoseDirect) {
    s32 btlId, partId;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 name;

    btlId = evtGetValue(event, args[0]);
    btlId = BattleTransID(event, btlId);
    unit = BattleGetUnitPtr(wp, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(btlId, partId);
    name = evtGetValue(event, args[2]);

    strcpy(part->poseName, (const char*)name);
    part->partFlags &= ~0x80;
    part->partFlags &= ~0x10;
    animPoseSetAnim(part->poseId, (const char*)name, 1);
    part->statusPoseKind = 1;
    if (BtlUnit_CheckStatus(unit, 0x12)) {
        part->statusPoseKind = 7;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId
USER_FUNC(btlevtcmd_AnimeWaitPlayComplete) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 btlId;
    s32 id;
    s32 partId;
    f32 loopTimes;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    if (isFirstCall) {
        event->userdata[0] = 1;
    }
    if (event->userdata[0] > 0) {
        event->userdata[0]--;
        return EVT_RETURN_BLOCK;
    }
    loopTimes = animPoseGetLoopTimes(part->poseId);
    if (loopTimes < 1.0f) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId
USER_FUNC(btlevtcmd_AnimeWaitPlayLoopTiming) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;
    f32 loopTimes;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    if (isFirstCall) {
        event->userdata[0] = 1;
        event->userdata[1] = 0;
        event->userdata[2] = 0;
    }
    // The original threshold double is (double)0.01f (3f847ae140000000), not the exact 0.01 double.
    if (__fabs(animPoseGetAnimPosePtr(part->poseId)->localTimeRate) < (f64)0.01f) {
        return 2;
    }
    loopTimes = animPoseGetLoopTimes(part->poseId);
    if (event->userdata[0] > 0) {
        event->userdata[0]--;
        return 0;
    }
    switch (event->userdata[1]) {
    case 0:
        event->userdata[2] = (s32)loopTimes;
        event->userdata[1]++;
        return 0;
    default:
        return ((s32)loopTimes != event->userdata[2]) ? 2 : 0;
    }
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
    unit->mBodyPartsId = value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 pose
USER_FUNC(btlevtcmd_SetTalkPose) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 value = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->mTalkPoseName = (char*)value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 pose
USER_FUNC(btlevtcmd_SetStayPose) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 value = args[1];
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));

    unit->mStayPoseName = (char*)value;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* poseType
USER_FUNC(btlevtcmd_SetTalkPoseType) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id, poseType, bodyPartsId;

    id = evtGetValue(event, args[0]);
    poseType = evtGetValue(event, args[1]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    bodyPartsId = BtlUnit_GetBodyPartsId(unit);
    part = BattleGetUnitPartsPtr(id, bodyPartsId);
    unit->mTalkPoseName = BtlUnit_GetPoseNameFromType(part, poseType);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* poseType
USER_FUNC(btlevtcmd_SetStayPoseType) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id, poseType, bodyPartsId;

    id = evtGetValue(event, args[0]);
    poseType = evtGetValue(event, args[1]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    bodyPartsId = BtlUnit_GetBodyPartsId(unit);
    part = BattleGetUnitPartsPtr(id, bodyPartsId);
    unit->mStayPoseName = BtlUnit_GetPoseNameFromType(part, poseType);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_AnimeCmpPoseType) {
    s32* args = event->args;
    s32 id;
    s32 partId;
    s32 type1;
    BattleUnitPart* part;
    s32 type2;
    s32 retIndex;
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

//s32* battleId, s32* partId, s32* table
USER_FUNC(btlevtcmd_AnimeSetPoseSoundTable) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    part->poseSound.table = (void*)args[2];
    BtlUnit_PoseSoundInit(part);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32 poseType, s32* retIndex
USER_FUNC(btlevtcmd_CheckDefinePosetype) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 partId = evtGetValue(event, args[1]);
    BattleUnitPart* part = BattleGetUnitPartsPtr(id, partId);
    s32 type = evtGetValue(event, args[2]);
    s32 index = args[3];
    BattleDispPose* pose = (BattleDispPose*)part->poseTable;
    s32 result = 0;
    s32 group;

    if (type <= 0x1C) {
        group = 0x1C;
    }
    else if (type < 0x28) {
        group = 0x27;
    }
    else if (type < 0x46) {
        group = 0x45;
    }
    else {
        evtSetValue(event, index, 0);
        return EVT_RETURN_DONE;
    }
    while (pose != NULL) {
        if ((s32)pose->poseId >= 0x46) {
            result = 0;
            break;
        }
        if ((s32)pose->poseId == type) {
            result = 1;
            break;
        }
        if ((s32)pose->poseId == group) {
            result = 0;
            break;
        }
        pose++;
    }
    evtSetValue(event, index, result);
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
    part->motionSpeed = evtGetFloat(event, args[2]);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* group, s32* flag
USER_FUNC(btlevtcmd_AnimeChangePaperGroup) {
    s32 btlId, id, partId;
    BattleUnitPart* part;
    s32* args = event->args;
    s32 group;
    s32 flag;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    group = evtGetValue(event, args[2]);
    flag = evtGetValue(event, args[3]);
    animPoseSetEffect(part->poseId, (const char*)group, flag);
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
        part->partFlags = part->partFlags | (u32)flags;
    }
    else {
        part->partFlags = part->partFlags & ~(u32)flags;
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* retIndex
USER_FUNC(btlevtcmd_CheckGuardMotionEnable) {
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 id = BattleTransID(event, btlId);
    s32 index = args[1];
    BOOL result;
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    result = TRUE;

    if (BtlUnit_CheckStatus(unit, 1) || BtlUnit_CheckStatus(unit, 2) || BtlUnit_CheckStatus(unit, 9) ||
        BtlUnit_CheckStatus(unit, 0xB) || BtlUnit_CheckStatus(unit, 0x1B) || BtlUnit_CheckStatus(unit, 0x12)) {
        result = FALSE;
    }
    if (BtlUnit_CheckStatusFlag(unit, 1)) {
        result = FALSE;
    }
    evtSetValue(event, index, result);
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

//s32* battleId
USER_FUNC(btlevtcmd_ClearAllStatus) {
    s32 btlId = evtGetValue(event, *event->args);

    BtlUnit_ClearStatus(BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId)));
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

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_CheckActStatus) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    s32 index;

    id = evtGetValue(event, args[0]);
    index = args[1];
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (BtlUnit_CanActStatus(unit)) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_GetACPossibility) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    s32 index;

    id = evtGetValue(event, args[0]);
    index = args[1];
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (BtlUnit_GetACPossibility(unit)) {
        evtSetValue(event, index, 1);
    }
    else {
        evtSetValue(event, index, 0);
    }
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

//s32* weaponId, s32 retIndex
USER_FUNC(btlevtcmd_CheckWeaponPluralTarget) {
    s32* args = event->args;
    BattleWeapon* weapon = (BattleWeapon*)evtGetValue(event, args[0]);
    s32 index = args[1];

    if (weapon->targetClassFlags & 0x01000000) {
        evtSetValue(event, index, 0);
    }
    else {
        evtSetValue(event, index, 1);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* weaponId
USER_FUNC(btlevtcmd_PayWeaponCost) {
    s32* args = event->args;
    BattleWeapon* weapon;
    s32 id;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    weapon = (BattleWeapon*)evtGetValue(event, args[1]);
    BtlUnit_PayWeaponCost(BattleGetUnitPtr(_battleWorkPointer, id), weapon);
    return EVT_RETURN_DONE;
}

//s32* battleId
USER_FUNC(btlevtcmd_CommandPayWeaponCost) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleWeapon* weapon;
    s32 id;

    id = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    weapon = BattleGetSelectWeapon(wp);
    BtlUnit_PayWeaponCost(unit, weapon);
    return EVT_RETURN_DONE;
}

//s32 retIndex, s32* weaponIndex
USER_FUNC(btlevtcmd_GetWeaponActionLv) {
    s32* args = event->args;
    BattleWeapon* weapon = (BattleWeapon*)evtGetValue(event, args[0]);

    evtSetValue(event, args[1], weapon->actionLevel);
    return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(btlevtcmd_CommandGetWeaponActionLv) {
    BattleWeapon* weapon;
    s32 index = *event->args;

    weapon = BattleGetSelectWeapon(_battleWorkPointer);
    evtSetValue(event, index, weapon->actionLevel);
    return EVT_RETURN_DONE;
}

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_CommandGetWeaponAddress) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    BattleWeapon* weapon;
    s32 btlId;
    s32 index;

    btlId = evtGetValue(event, args[0]);
    index = args[1];
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, btlId));
    if (unit->currentKind == UNIT_MARIO ||
        (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX)) {
        weapon = BattleGetSelectWeapon(wp);
    }
    else {
        weapon = itemDataTable[unit->heldItem].battle_params;
    }
    evtSetValue(event, index, (s32)weapon);
    return EVT_RETURN_DONE;
}

//s32 retIndex, s32* weaponIndex
USER_FUNC(btlevtcmd_CommandGetWeaponItemId) {
    BattleWeapon* weapon;
    s32 index = *event->args;

    weapon = (BattleWeapon*)evtGetValue(event, event->args[1]);
    evtSetValue(event, index, weapon->item);
    return EVT_RETURN_DONE;
}

//s32* itemId, s32 retIndex1, s32 retIndex2, s32 retIndex3
USER_FUNC(btlevtcmd_GetItemRecoverParam) {
    s32* args = event->args;
    s32 index1;
    s32 index2;
    s32 index3;
    ItemData* item;

    item = &itemDataTable[evtGetValue(event, args[0])];
    index1 = args[1];
    index2 = args[2];
    index3 = args[3];
    if (index1 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index1, item->hp_restored);
    }
    if (index2 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index2, item->fp_restored);
    }
    if (index3 != EVTDAT_POINTER_MAX) {
        // header declares sp_restored as s8, but the original loads it with lbz (no extsb): it is u8.
        evtSetValue(event, index3, (u8)item->sp_restored);
    }
    return EVT_RETURN_DONE;
}

//s32* weaponIndex
USER_FUNC(btlevtcmd_WeaponAftereffect) {
    BattleWork* wp = _battleWorkPointer;
    BattleWeapon* weapon = (BattleWeapon*)evtGetValue(event, event->args[0]);
    s32 a1;
    s32 a2;
    s32 b;
    s32 value;

    b = 0;
    a2 = 0;
    a1 = 0;

    value = irand(weapon->field_B4[0] + weapon->field_B4[1] + weapon->field_B4[2] + weapon->field_B4[3]);
    if (wp->forceFlags & 0x10) {
        a1 = 1;
    }
    if (wp->forceFlags & 0x20) {
        a2 = 1;
    }
    value -= weapon->field_B4[0];
    if (value < 0) {
        a2 = 1;
        a1 = 1;
    } else {
        value -= weapon->field_B4[1];
        if (value < 0) {
            a1 = 1;
        } else {
            value -= weapon->field_B4[2];
            if (value < 0) {
                a2 = 1;
            }
        }
    }
    if (wp->field_180C0[0x1C] & 0x4) {
        a2 = 0;
    }
    if (wp->field_180C0[0x1C] & 0x2) {
        a1 = 0;
    }
    if ((wp->field_180C0[0x1C] & 0x2) && !(wp->field_180C0[0x1C] & 0x8)) {
        if (irand(100) < weapon->field_B4[4] ||
            (wp->forceFlags & 0x40)) {
            b = 1;
            if (!(wp->field_180C0[0x1C] & 0x4)) {
                a2 = 1;
            }
        }
    }
    if (a1) {
        BattleStage_DestroyA1();
    }
    if (a2) {
        BattleStage_DestroyA2();
    }
    if (b) {
        BattleStage_DestroyB();
    }
    if (irand(100) < weapon->field_B4[5] || (wp->forceFlags & 0x200)) {
        BattleStage_NozzleDirChangeCheck();
    }
    if (irand(100) < weapon->field_B4[6] || (wp->forceFlags & 0x100)) {
        BattleStage_NozzleWorkCheck(1);
    }
    if (irand(100) < weapon->field_B4[7] || (wp->forceFlags & 0x80)) {
        BattleStage_IronFrameFallCheck();
    }
    if (wp->field_180C0[0x1C] & 0x20) {
        if (wp->forceFlags & 0x1) {
            BattleStage_ObjectFallCheck();
        } else if (irand(100) < weapon->field_B4[8]) {
            BattleStage_ObjectFallCheck();
        }
    }
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

//s32* battleId, s32 retIndex
USER_FUNC(btlevtcmd_GetGuardKouraId) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnit* koura;
    s32 id;
    s32 index;

    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    index = args[1];
    unit = BattleGetUnitPtr(wp, id);
    koura = BtlUnit_GetGuardKouraPtr(unit);
    if (koura != NULL) {
        evtSetValue(event, index, koura->unitId);
    }
    else {
        evtSetValue(event, index, -1);
    }
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

//no params
USER_FUNC(btlevtcmd_SortPhaseMoveTable) {
    battleSortPhaseMoveTable(_battleWorkPointer, -1);
    return EVT_RETURN_DONE;
}

//s32* retIndex1, s32* retIndex2, s32* retIndex3
USER_FUNC(btlevtcmd_GetStageSize) {
    s32* args = event->args;
    BattleStage* stage;
    s32 index1;
    s32 index2;
    s32 index3;

    stage = BattleStageGetPtr();
    index1 = args[0];
    index2 = args[1];
    index3 = args[2];
    if (index1 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index1, (s32)stage->stageSize.x);
    }
    if (index2 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index2, (s32)stage->stageSize.y);
    }
    if (index3 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index3, (s32)stage->stageSize.z);
    }
    return EVT_RETURN_DONE;
}

//s32* retIndex1, s32* retIndex2, s32* retIndex3
USER_FUNC(btlevtcmd_GetStageAudienceAreaSize) {
    s32* args = event->args;
    BattleStage* stage;
    s32 index1;
    s32 index2;
    s32 index3;

    stage = BattleStageGetPtr();
    index1 = args[0];
    index2 = args[1];
    index3 = args[2];
    if (index1 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index1, (s32)stage->audienceAreaSize.x);
    }
    if (index2 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index2, (s32)stage->audienceAreaSize.y);
    }
    if (index3 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index3, (s32)stage->audienceAreaSize.z);
    }
    return EVT_RETURN_DONE;
}

//s32* x, s32* retIndex, s32* z
USER_FUNC(btlevtcmd_TransStageFloorPosition) {
    s32* args = event->args;
    f32 x;
    f32 z;
    s32 index;

    x = evtGetValue(event, args[0]);
    index = args[1];
    z = evtGetValue(event, args[2]);
    evtSetValue(event, index, (s32)BattleGetFloorHeight(_battleWorkPointer, x, 0.0f, z));
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* v1, s32* v2, s32* v3, s32* v4, s32* v5, s32* v6, s32* v7
USER_FUNC(btlevtcmd_SetMoveSoundDataWork) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    s32 v1 = evtGetValue(event, args[1]);
    s32 v2 = evtGetValue(event, args[2]);
    s32 v3 = evtGetValue(event, args[3]);
    s32 v4 = evtGetValue(event, args[4]);
    s32 v5 = evtGetValue(event, args[5]);
    s16 v6 = (s16)evtGetValue(event, args[6]);
    s16 v7 = (s16)evtGetValue(event, args[7]);

    BtlUnit_SetSeMode(&unit->seMode, v1, v2, (s16)v3, (s16)v4, (s16)v5, v6, v7);
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
    unit->seModeSrc.mode0SfxA = v1;
    unit->seModeSrc.mode0SfxB = v2;
    unit->seModeSrc.mode0Delay = v3;
    unit->seModeSrc.mode0IntervalA = v4;
    unit->seModeSrc.mode0IntervalB = v5;
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
    part->seModeSrc.mode0SfxA = v1;
    part->seModeSrc.mode0SfxB = v2;
    part->seModeSrc.mode0Delay = v3;
    part->seModeSrc.mode0IntervalA = v4;
    part->seModeSrc.mode0IntervalB = v5;
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
    unit->seModeSrc.mode1SfxA = v1;
    unit->seModeSrc.mode1SfxB = v2;
    unit->seModeSrc.mode1Delay = v3;
    unit->seModeSrc.mode1IntervalA = v4;
    unit->seModeSrc.mode1IntervalB = v5;
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
    part->seModeSrc.mode1SfxA = v1;
    part->seModeSrc.mode1SfxB = v2;
    part->seModeSrc.mode1Delay = v3;
    part->seModeSrc.mode1IntervalA = v4;
    part->seModeSrc.mode1IntervalB = v5;
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
    unit->seModeSrc.mode3SfxA = v1;
    unit->seModeSrc.mode3SfxB = v2;
    unit->seModeSrc.mode3Delay = v3;
    unit->seModeSrc.mode3IntervalA = v4;
    unit->seModeSrc.mode3IntervalB = v5;
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
    part->seModeSrc.mode3SfxA = v1;
    part->seModeSrc.mode3SfxB = v2;
    part->seModeSrc.mode3Delay = v3;
    part->seModeSrc.mode3IntervalA = v4;
    part->seModeSrc.mode3IntervalB = v5;
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

    unit->seModeSrc.mode2SfxA = a;
    unit->seModeSrc.mode2SfxB = b;
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* partId, s32* a, s32* b
USER_FUNC(btlevtcmd_SetPartsJumpSound) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 btlId, id, partId, v1, v2;

    btlId = evtGetValue(event, args[0]);
    id = BattleTransID(event, btlId);
    partId = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, partId);
    v1 = evtGetValue(event, args[2]);
    v2 = evtGetValue(event, args[3]);
    part->seModeSrc.mode2SfxA = v1;
    part->seModeSrc.mode2SfxB = v2;
    return EVT_RETURN_DONE;
}

// the original calls _UnitMoveSoundControl with `bl`, not inlined
#pragma dont_inline on
void _UnitMoveSoundControl(BattleUnit* unit) {
    f32 x, y, z;

    BtlUnit_GetPos(unit, &x, &y, &z);
    _MoveSoundControlCore(unit, &unit->seMode, NULL, x, y, z);
}

// the original calls _PartsMoveSoundControl with `bl`, not inlined
void _PartsMoveSoundControl(BattleUnitPart* part) {
    f32 x, y, z;

    BtlUnit_GetPartsPos(part, &x, &y, &z);
    _MoveSoundControlCore(part->mOwner, &part->seMode, part, x, y, z);
}
#pragma dont_inline reset

// work is the unit's seMode, or the part's when owner is that part (NULL for the unit itself); x/y/z is where the
// beat sounds play.
void _MoveSoundControlCore(BattleUnit* unit, BtlUnitSeMode* work, BattleUnitPart* owner, f32 x, f32 y, f32 z) {
    Vec pos;
    Vec unitPos;
    s32 a;
    s32 b;
    pos = (Vec){ x, y, z };
    a = work->sfxA;

    if (a == 0 && work->sfxB == 0) {
        return;
    }
    if (a != 0 && work->sfxB == 0) {
        if ((u32)work->sfxHandle == 0xFFFFFFFF) {
            work->sfxHandle = BtlUnit_snd_se_pos(unit, (const char*)a, 0xF1194D80, 0, &pos);
        }
        if (owner != NULL) {
            BtlUnit_GetPartsWorldPos(owner, &unitPos.x, &unitPos.y, &unitPos.z);
        }
        else {
            BtlUnit_GetPos(unit, &unitPos.x, &unitPos.y, &unitPos.z);
        }
        psndSFX_pos(work->sfxHandle, &unitPos);
    }
    else {
        s32 t = work->countdown;

        if (t < 0) {
            return;
        }
        work->countdown = t - 1;
        if (work->countdown >= 0) {
            return;
        }
        work->beat = work->beat + 1;
        if (work->beat & 1) {
            b = work->sfxA;
            work->countdown = work->intervalA;
        }
        else {
            b = work->sfxB;
            work->countdown = work->intervalB;
        }
        if (b != 0) {
            BtlUnit_snd_se_pos(unit, (const char*)b, 0xF1194D80, 0, &pos);
        }
    }
}

// the original calls _UnitMoveSoundControlEnd with `bl`, not inlined
#pragma dont_inline on
void _UnitMoveSoundControlEnd(BattleUnit* unit) {
    _MoveSoundControlEndCore(unit, &unit->seMode);
}

// the original calls _PartsMoveSoundControlEnd with `bl`, not inlined
void _PartsMoveSoundControlEnd(BattleUnitPart* part) {
    BtlUnitSeMode* work = &part->seMode;
    _MoveSoundControlEndCore(part->mOwner, work);
}

// Keep _MoveSoundControlEndCore out of line: the original calls it rather than inlining.
void _MoveSoundControlEndCore(BattleUnit* unit, BtlUnitSeMode* work) {
    // unsigned compare, as in retail (a signed one against -1 is an instruction shorter)
    if ((u32)work->sfxHandle != 0xFFFFFFFF) {
        psndSFXOff(work->sfxHandle);
        work->sfxHandle = -1;
    }
}
#pragma dont_inline reset

//s32* battleId, s32* a, s32* b, s32* c, s32* retIndex
USER_FUNC(btlevtcmd_snd_se) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 btlId, v1, v2, v3, v4, ret;
    BattleUnit* unit;

    btlId = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = (s16)evtGetValue(event, args[3]);
    v4 = args[4];
    ret = BtlUnit_snd_se(unit, (const char*)v1, v2, v3);
    if (v4 != EVTDAT_POINTER_MAX) {
        evtSetValue(event, v4, ret);
    }
    return EVT_RETURN_DONE;
}

//s32* battleId, s32* a, s32* b, s32* c, s32* dx, s32* dy, s32* dz, s32 retIndex
USER_FUNC(btlevtcmd_snd_se_offset) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    Vec pos;
    s32 index;
    s32 a, b;
    BattleUnit* unit;
    s16 c;
    s32 btlId = evtGetValue(event, args[0]);
    s32 ret;

    unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    a = evtGetValue(event, args[1]);
    b = evtGetValue(event, args[2]);
    c = (s16)evtGetValue(event, args[3]);
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.x += evtGetFloat(event, args[4]);
    pos.y += evtGetFloat(event, args[5]);
    pos.z += evtGetFloat(event, args[6]);
    index = args[7];
    ret = BtlUnit_snd_se_pos(unit, (const char*)a, b, c, &pos);
    if (index != EVTDAT_POINTER_MAX) {
        evtSetValue(event, index, ret);
    }
    return EVT_RETURN_DONE;
}

const char* _mario_acrobat_voice_table[5] = {
    "SFX_VOICE_MARIO_AC3_1", "SFX_VOICE_MARIO_AC3_2", "SFX_VOICE_MARIO_AC3_3", "SFX_VOICE_MARIO_AC3_4",
    "SFX_VOICE_MARIO_AC3_5",
};

const char* _mario_attack_voice_table[10] = {
    "SFX_VOICE_MARIO_AC1_1", "SFX_VOICE_MARIO_AC1_2", "SFX_VOICE_MARIO_AC1_3", "SFX_VOICE_MARIO_AC1_4",
    "SFX_VOICE_MARIO_AC1_5", "SFX_VOICE_MARIO_AC2_1", "SFX_VOICE_MARIO_AC2_2", "SFX_VOICE_MARIO_AC2_3",
    "SFX_VOICE_MARIO_AC2_4", "SFX_VOICE_MARIO_AC2_5",
};

#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80410D5C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

//s32* battleId, s32* voiceType
USER_FUNC(btlevtcmd_snd_voice) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 btlId = evtGetValue(event, args[0]);
    s32 voiceType = evtGetValue(event, args[1]);
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, btlId));
    switch (voiceType) {
    case 1:
        BtlUnit_snd_se(unit, _mario_acrobat_voice_table[irand(5)], 0xF1194D80, 0);
        break;
    case 2:
        BtlUnit_snd_se(unit, _mario_attack_voice_table[irand(10)], 0xF1194D80, 0);
        break;
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

//s32 chan, s32 mask, s32 retIndex
// opt_propagation off: keep the `mask = args[1]` load before `index = args[2]` so index reuses
// the args register (two callee-saved registers, as in the target).
#pragma opt_propagation off
USER_FUNC(btlevtcmd_PadCheckTrigger) {
    s32* args = event->args;
    s32 chan, mask, index;

    chan = evtGetValue(event, args[0]);
    mask = args[1];
    index = args[2];
    if (BattlePadMultiCheckTrigger(chan, mask)) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}

//s32 chan, s32 mask, s32 retIndex
// opt_propagation off: keep the `mask = args[1]` load before `index = args[2]` so index reuses
// the args register (two callee-saved registers, as in the target).
USER_FUNC(btlevtcmd_PadCheckNow) {
    s32* args = event->args;
    s32 chan, mask, index;

    chan = evtGetValue(event, args[0]);
    mask = args[1];
    index = args[2];
    if (BattlePadMultiCheckNow(chan, mask)) {
        evtSetValue(event, index, TRUE);
    }
    else {
        evtSetValue(event, index, FALSE);
    }
    return EVT_RETURN_DONE;
}
#pragma opt_propagation reset

//no params
USER_FUNC(btlevtcmd_InviteApInfoReport) {
    BattleWork* wp = _battleWorkPointer;
    StarPowerInfo* info = &wp->impendingBonuses;

    BattleAudience_ApRecoveryBuild(info);
    memset(info, 0, 0xC);
    return EVT_RETURN_DONE;
}

//s32* valueIndex, BOOL isFirstCall
USER_FUNC(btlevtcmd_SacBgsetMove) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 value;
    s32 i;
    BattleStageObject* obj;
    s32 mode;
    f32 move;

    value = evtGetValue(event, args[0]);
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    event->userdata[0]++;
    mode = (value != 0) ? 5 : 2;
    for (i = 0; i < 32; i++) {
        obj = &wp->stageObjects[i];
        if (obj->id > 0) {
            switch (obj->data->mLayer) {
            case 0:
            case 1:
                if (value != 0) {
                    if (obj->mPosition.x >= 0.0f) {
                        move = intplGetValue(mode, event->userdata[0], 45, 450.f, 0.0f);
                    } else {
                        move = -intplGetValue(mode, event->userdata[0], 45, 450.f, 0.0f);
                    }
                } else {
                    if (obj->mPosition.x >= 0.0f) {
                        move = intplGetValue(mode, event->userdata[0], 45, 0.0f, 450.f);
                    } else {
                        move = -intplGetValue(mode, event->userdata[0], 45, 0.0f, 450.f);
                    }
                }
                obj->mPositionOffset2.x = move;
                break;
            case 2:
                if (value != 0) {
                    move = intplGetValue(mode, event->userdata[0], 45, 260.f, 0.0f);
                } else {
                    move = intplGetValue(mode, event->userdata[0], 45, 0.0f, 260.f);
                }
                obj->mPositionOffset2.y = move;
                break;
            }
        }
    }
    if (event->userdata[0] >= 45) {
        return TRUE;
    }
    return FALSE;
}

//s32* name, u32 flag, s32 onOff
USER_FUNC(btlevtcmd_BtlStageObjFlagOnOff) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 i;
    BattleStageObject* obj;
    const char* name = (const char*)evtGetValue(event, args[0]);
    u32 flag = args[1];
    s32 onOff = evtGetValue(event, args[2]);

    for (i = 0; i < 32; i++) {
        obj = &wp->stageObjects[i];
        if (obj->id > 0 && strcmp(obj->data->name, name) == 0) {
            if (onOff) {
                obj->mFlags |= flag;
            } else {
                obj->mFlags &= ~flag;
            }
        }
    }
    return EVT_RETURN_DONE;
}

//s32* name, s32* x, s32* y, s32* z
USER_FUNC(btlevtcmd_BtlStageObjSetRotate) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 i;
    BattleStageObject* obj;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 indexX = args[1];
    s32 valueX = evtGetValue(event, indexX);
    s32 indexY = args[2];
    s32 valueY = evtGetValue(event, indexY);
    s32 indexZ = args[3];
    s32 valueZ = evtGetValue(event, indexZ);

    for (i = 0; i < 32; i++) {
        obj = &wp->stageObjects[i];
        if (obj->id > 0 && strcmp(obj->data->name, name) == 0) {
            if (indexX != EVTDAT_POINTER_MAX) {
                obj->mRotation.x = (f32)valueX;
            }
            if (indexY != EVTDAT_POINTER_MAX) {
                obj->mRotation.y = (f32)valueY;
            }
            if (indexZ != EVTDAT_POINTER_MAX) {
                obj->mRotation.z = (f32)valueZ;
            }
            break;
        }
    }
    return EVT_RETURN_DONE;
}

//s32* name, s32* retX, s32* retY, s32* retZ
USER_FUNC(btlevtcmd_BtlStageObjGetRotate) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 i;
    BattleStageObject* obj;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 indexX = args[1];
    s32 indexY = args[2];
    s32 indexZ = args[3];

    for (i = 0; i < 32; i++) {
        obj = &wp->stageObjects[i];
        if (obj->id > 0 && strcmp(obj->data->name, name) == 0) {
            if (indexX != EVTDAT_POINTER_MAX) {
                evtSetFloat(event, indexX, obj->mRotation.x);
            }
            if (indexY != EVTDAT_POINTER_MAX) {
                evtSetFloat(event, indexY, obj->mRotation.y);
            }
            if (indexZ != EVTDAT_POINTER_MAX) {
                evtSetFloat(event, indexZ, obj->mRotation.z);
            }
            break;
        }
    }
    return EVT_RETURN_DONE;
}

//s32 mode
USER_FUNC(btlevtcmd_StatusWindowOnOff) {
    BattleWork* wp = _battleWorkPointer;

    switch (evtGetValue(event, *event->args)) {
    case 2:
        wp->statusWinSystem = 1;
        break;
    case 1:
        wp->statusWinEvent = 1;
        break;
    case 0:
        wp->statusWinEvent = 0;
        break;
    }
    BattleStatusWindowCheck();
    return EVT_RETURN_DONE;
}

//no params
USER_FUNC(btlevtcmd_StageDispellFog) {
    BattleFogForceStop();
    return EVT_RETURN_DONE;
}

//s32 frameIndex, s32 retIndex
USER_FUNC(btlevtcmd_ftomsec) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], value * 1000 / 60);
    return EVT_RETURN_DONE;
}

//s32 frameIndex, s32 retIndex
USER_FUNC(btlevtcmd_ftof) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    s32 index = args[1];
    s32 frame = sysMsec2Frame(value * 1000 / 60);

    evtSetValue(event, index, frame);
    return EVT_RETURN_DONE;
}

// The unit's .sdata2 pool constants, defined after the code that loads them (defined first, the
// compiler folds their values and the code changes). Address order matches the original.

#include "battle/battle_unit.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "system.h"
#include <string.h>
#include "pmario_sound.h"
#include "battle/battle_audience.h"
#include "battle/battle_actrecord.h"
#include "math_ext.h"
#include "driver/animdrv.h"
#include "memory.h"
#include "manager/evtmgr.h"

// LOCAL_PROTO(psndSFX_get_vol): retail battle callers consume a full-width result; sharing the u8 implementation signature adds a mask and regresses BtlUnit_snd_se_pos.
extern s32 psndSFX_get_vol(s32 id);

// LOCAL_PROTO(psndSFX_pit): retail callers disagree on pitch width and signedness; the shared u16 signature regresses matched sound commands.
extern void psndSFX_pit(s32 id, s32 pitch);

// LOCAL_PROTO(psndSFX_vol): retail units disagree on volume width; a shared u8 prototype adds narrowing instructions in battle callers.
extern void psndSFX_vol(s32 id, s32 vol);


extern BattleWork* _battleWorkPointer;
u32 _enemy_can_use_item_table[] = {
    0x00000080, 0x00000084, 0x0000008B, 0x00000081, 0x00000082, 0x00000083, 0x00000085, 0x00000090,
    0x0000008A, 0x0000008C, 0x0000008D, 0x00000092, 0x00000093, 0x00000089, 0x0000008E, 0x0000008F,
    0x00000086, 0x00000087, 0x00000088, 0x00000094, 0x00000095, 0x00000096, 0x00000097, 0x00000098,
    0x00000099, 0x0000009A, 0x0000009B, 0x0000009C, 0x0000009D, 0x0000009E, 0x0000009F, 0x000000A0,
    0x00000000,
};

// Part colour initialiser (opaque white).
#ifdef MUST_MATCH
// Retail places this colour template in the small read-only data section.
__declspec(section ".sdata2") GXColor dat_8041c3b8 = { 0xFF, 0xFF, 0xFF, 0xFF };
#else
GXColor dat_8041c3b8 = { 0xFF, 0xFF, 0xFF, 0xFF };
#endif
// 30-byte status block copied between BattleWorkPartyInfo::field_4 and BattleUnit::mSleepTurns.
#ifdef MUST_MATCH
// For the struct-copy below: MWCC emits the original's lwzu/stwu copy loop for a struct
// assignment, but a plain memcpy of the 30-byte blob compiles to a call.
typedef struct UnitStatusBlock {
    s16 v[15]; // 0x0
} UnitStatusBlock;
#endif


//local prototypes


typedef struct HpGaugeLocal {
    u16 field_0;  // 0x0
    u16 field_2;  // 0x2
    s16 field_4;  // 0x4
    s16 field_6;  // 0x6
    s16 field_8;  // 0x8
    s16 field_A;  // 0xA
    s32 field_C;  // 0xC
    s32 field_10; // 0x10
    f32 field_14; // 0x14
    f32 field_18; // 0x18
} HpGaugeLocal;

BOOL BtlUnit_Init(void) {
    int i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleSetUnitPtr(wp, i, NULL);
    }
    return TRUE;
}

// Keep BtlUnit_Entry out of line: retail calls it from BtlUnit_Spawn.
#pragma dont_inline on
BattleUnit* BtlUnit_Entry(BattleUnitSetup* setup) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnitPart* part;
    BattleUnitKindPart* kindPart;
    s32 i;
    s32 n;

    for (i = 0; i < 64; i++) {
        if (!BattleGetUnitPtr(wp, i)) break;
    }
    if (i >= 64) return NULL;

    unit = (BattleUnit*)BattleAlloc(sizeof(BattleUnit));
    if (!unit) return NULL;

    memset(unit, 0, sizeof(BattleUnit));
    BattleSetUnitPtr(wp, i, unit);
    unit->unitId = i;

    unit->mKindParams = setup->mUnitKindParams;
    unit->dataTable = unit->mKindParams->dataTable;
    unit->initialKind = setup->mUnitKindParams->mUnitType;
    unit->currentKind = setup->mUnitKindParams->mUnitType;

    BattleStatusEffectInit(unit);
    BattleStatusIconInit(unit);
    BattleStatusChangeInfoWorkInit(unit);

    unit->groupSetupId = -1;
    unit->mAlliance = setup->alliance;
    unit->mLevel = unit->mKindParams->mLevel;
    if (unit->currentKind == UNIT_MARIO || (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX)) {
        unit->mLevel = pouchGetPtr()->level;
    }

    unit->seqPhase = setup->field_0x8;
    unit->unisonPhaseEventCode = NULL;
    unit->phaseEventCode = NULL;
    unit->phaseEventId = 0;
    unit->waitEventCode = NULL;
    unit->waitEventId = 0;
    unit->attackEventCode = NULL;
    unit->attackEventId = 0;

    BtlUnit_SetPos(unit, setup->position.x, setup->position.y, setup->position.z);
    BtlUnit_SetOffsetPos(unit, setup->position.x, setup->position.y, setup->position.z);
    BtlUnit_SetHomePos(unit, setup->position.x, setup->position.y, setup->position.z);
    BtlUnit_SetBaseRotate(unit, 0.0f, 0.0f, 0.0f);
    BtlUnit_SetRotate(unit, 0.0f, 0.0f, 0.0f);
    BtlUnit_SetRotateOffset(unit, 0.0f, 0.0f, 0.0f);
    BtlUnit_SetScale(unit, 1.0f, 1.0f, 1.0f);
    BtlUnit_SetBaseScale(unit, 1.0f, 1.0f, 1.0f);

    unit->offsetCenter = unit->mKindParams->centerOffset;
    unit->field_0xB4 = unit->mKindParams->field_0x3C;
    unit->bintaHitOffset = unit->mKindParams->bintaHitOffset;
    unit->kissHitOffset = unit->mKindParams->kissHitOffset;

    unit->baseMaxHp = setup->mUnitKindParams->maxHp;
    unit->maxHp = unit->baseMaxHp;
    unit->currentHp = setup->mUnitKindParams->maxHp;
    unit->baseMaxFp = setup->mUnitKindParams->maxFp;
    unit->maxFp = unit->baseMaxFp;
    unit->currentFp = setup->mUnitKindParams->maxFp;

    unit->mSizeMultiplier = 1.0f;
    unit->mTokenFlags = 0;
    unit->mStatusFlags = 0;
    unit->protectId = -1;
    BtlUnit_HpGaugeInit(unit);

    unit->mWidth = unit->mKindParams->width;
    unit->mHeight = unit->mKindParams->height;
    unit->mStatusIconOffset[0] = unit->mKindParams->statusIconOffset[0];
    unit->mStatusIconOffset[1] = unit->mKindParams->statusIconOffset[1];
    unit->mHpGaugeOffset[0] = unit->mKindParams->hpGaugeOffset[0];
    unit->mHpGaugeOffset[1] = unit->mKindParams->hpGaugeOffset[1];
    unit->cutBaseOffset = unit->mKindParams->cutBaseOffset;
    unit->cutWidth = unit->mKindParams->cutWidth;
    unit->cutHeight = unit->mKindParams->cutHeight;
    unit->swallowRate = unit->mKindParams->swallowRate;
    unit->swallowAttribute = unit->mKindParams->swallowAttribute;

    BtlUnit_ClearStatus(unit);

    unit->mAttributeFlags = unit->mKindParams->attributeFlags;
    unit->vulnStatus = unit->mKindParams->vulnStatus;
    unit->maxMoveCount = 1;
    unit->movesRemaining = 1;
    unit->moveDirection = 0;
    unit->faceDirection = wp->alliances[unit->mAlliance].attackDirection;

    unit->work[0] = setup->unitWork[0];
    unit->work[1] = setup->unitWork[1];
    unit->work[2] = setup->unitWork[2];
    unit->work[3] = setup->unitWork[3];

    n = unit->mKindParams->numParts;
    part = (BattleUnitPart*)BattleAlloc(n * sizeof(BattleUnitPart));
    memset(part, 0, n * sizeof(BattleUnitPart));
    unit->mParts = part;

    kindPart = unit->mKindParams->mParts;
    for (;;) {
        part->mKindPartParams = kindPart;
        part->mOwner = unit;
        part->attributes = part->mKindPartParams->attributes;
        part->counterAttributes = part->mKindPartParams->counterAttributes;
        if (part->attributes & kMainPart) {
            unit->mBodyPartsId = part->mKindPartParams->partNum;
        }
        part->defense = part->mKindPartParams->defense;
        part->defenseAttr = part->mKindPartParams->defenseAttr;
        part->field_0x1A2 = part->mKindPartParams->field_0x30;
        part->field_0x1A4 = part->mKindPartParams->field_0x32;
        part->field_0x78 = 1.0f;
        part->moveDirection = 0;
        part->faceDirection = wp->alliances[unit->mAlliance].attackDirection;
        part->mPositionOffset.x = part->mKindPartParams->positionOffset.x;
        part->mPositionOffset.y = part->mKindPartParams->positionOffset.y;
        part->mPositionOffset.z = part->mKindPartParams->positionOffset.z;

        BtlUnit_SetPartsBaseScale(part, 1.0f, 1.0f, 1.0f);
        BtlUnit_SetPartsScale(part, 1.0f, 1.0f, 1.0f);
        BtlUnit_SetPartsBaseRotate(part, 0.0f, 0.0f, 0.0f);
        BtlUnit_SetPartsRotate(part, 0.0f, 0.0f, 0.0f);
        BtlUnit_SetPartsRotateOffset(part, 0.0f, 0.0f, 0.0f);

        *(u8*)((u8*)part + 0x4FC) = 0;
        part->mHitBaseOffset.x = part->mKindPartParams->hitBaseOffset.x;
        part->mHitBaseOffset.y = part->mKindPartParams->hitBaseOffset.y;
        part->mHitBaseOffset.z = part->mKindPartParams->hitBaseOffset.z;
        part->mHitOffset.x = 0.0f;
        part->mHitOffset.y = 0.0f;
        part->mHitOffset.z = 0.0f;
        part->mHitCursorBaseOffset.x = part->mKindPartParams->hitCursorBaseOffset.x;
        part->mHitCursorBaseOffset.y = part->mKindPartParams->hitCursorBaseOffset.y;
        part->mHitCursorBaseOffset.z = part->mKindPartParams->hitCursorBaseOffset.z;
        part->mHitCursorOffset.x = 0.0f;
        part->mHitCursorOffset.y = 0.0f;
        part->mHitCursorOffset.z = 0.0f;
        part->color = dat_8041c3b8;
        part->detectOffsetX = setup->field_0x18;
        part->poseTable = kindPart->poseTable;
        part->poseId = -1;
        part->poseName[0] = 0;
        part->field_0x208 = 1.0f;
        part->motionSpeed = 1.0f;
        part->statusPoseKind = 0;
        part->statusPoseStep = 0;
        part->poseSoundEventId = -1;
        part->partName = part->mKindPartParams->partName;

        if (--n > 0) {
            BattleUnitPart* oldPart = part;
            part++;
            oldPart->mNextPart = part;
            kindPart++;
        } else {
            part->mNextPart = NULL;
            break;
        }
    }

    part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
    unit->mTalkPoseName = BtlUnit_GetPoseNameFromType(part, 0x41);
    unit->mStayPoseName = BtlUnit_GetPoseNameFromType(part, 0x2B);

    unit->mMoveColorLv = 0xFF;
    unit->mMoveColorCur = 0xFF;
    btlDispEntAnime(unit);
    unit->mExtraWork = NULL;
    unit->mExtraWorkFree = NULL;

    switch (unit->currentKind) {
    case UNIT_MARIO:
        break;
    case UNIT_UNUSED_UFO:
        unit->mExtraWork = BattleAlloc(0x44);
        break;
    }

    BtlUnit_SetParamFromPouch(unit);
    unit->initEventId = 0;
    if (unit->mKindParams->initEvtScript) {
        EventEntry* entry = evtEntry(unit->mKindParams->initEvtScript, 0xA, 0);

        entry->unitId = unit->unitId;
        unit->initEventId = entry->eventId;
    }

    return unit;
}
#pragma dont_inline reset


BOOL BtlUnit_Delete(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;
    s32 j;
    BattleUnitPart* part;
    s32 id;

    if (!unit) {
        return FALSE;
    }
    for (i = 0; i < 64; i++) {
        if (BattleGetUnitPtr(wp, i) == unit) {
            break;
        }
    }
    if (i >= 64) {
        return FALSE;
    }
    if (unit->waitEventId) {
        evtDeleteID(unit->waitEventId);
        unit->waitEventId = 0;
    }
    if (unit->phaseEventId) {
        evtDeleteID(unit->phaseEventId);
        unit->phaseEventId = 0;
    }
    if (unit->damageEventId) {
        evtDeleteID(unit->damageEventId);
        unit->damageEventId = 0;
    }
    id = unit->unitId;
    for (j = 0; j < 64; j++) {
        if (wp->unitIdTable[j] == id) {
            wp->unitIdTable[j] = -1;
        }
    }
    if (unit->flags & 0x40000000) {
        NpcBattleInfo* setup = wp->fbatInfo->setup;

        if (unit->groupSetupId >= 0) {
            setup->stolenItems[unit->groupSetupId] = unit->heldItem;
        }
    }
    BattleStatusEffectDelete(unit);
    BattleStatusIconDelete(unit);
    if (unit->mExtraWork) {
        if (unit->mExtraWorkFree) {
            ((void (*)(BattleUnit*)) unit->mExtraWorkFree)(unit);
        } else {
            BattleFree(unit->mExtraWork);
        }
    }
    if (unit->mSmartEntry) {
        smartFree(unit->mSmartEntry);
        unit->mSmartEntry = NULL;
    }
    part = unit->mParts;
    for (; part; part = part->mNextPart) {
        if (part->poseSoundEventId != -1) {
            part->poseSoundEventId = -1;
        }
        if (part->poseId != -1) {
            animPoseSetEffect(part->poseId, NULL, FALSE);
            animPoseRelease(part->poseId);
        }
    }
    BattleFree(unit->mParts);
    BattleFree(unit);
    BattleSetUnitPtr(wp, i, NULL);
    return TRUE;
}

// Retail resolves the free-list slot through a base+displacement store (`add r3,r31,r3;
// stw r5,0x120(r3)`) while the load uses the indexed form; writing the store as a
// dereferenced element address reproduces it (the natural form reuses the load's
// `stwx`).
BattleUnit* BtlUnit_Spawn(BattleUnitSetup* setup, s32 flags) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;
    s32 id;
    BattleUnit* unit;

    if (!(unit = BtlUnit_Entry(setup))) {
        return unit;
    }
    if (flags & 2) {
        unit->moveState = 0;
        id = unit->unitId;
        for (i = 0; i < 64; i++) {
            if (id == -1) {
                break;
            }
            if (wp->unitIdTable[i] == -1) {
                wp->unitIdTable[i] = id;
                break;
            }
            {
                s32 prev = wp->unitIdTable[i];
                wp->unitIdTable[i] = id;
                id = prev;
            }
        }
    }
    else if (flags & 1) {
        unit->moveState = 0;
        id = unit->unitId;
        for (i = 63; i >= 0; i--) {
            if (id == -1) {
                break;
            }
            if (wp->unitIdTable[i] == -1) {
                wp->unitIdTable[i] = id;
                break;
            }
            {
                s32 prev = wp->unitIdTable[i];
                wp->unitIdTable[i] = id;
                id = prev;
            }
        }
    }
    return unit;
}

s32 BtlUnit_GetUnitId(BattleUnit* unit) {
    return unit->unitId;
}

// Keep BtlUnit_GetPartsPtr out of line: the original calls it rather than inlining.
#pragma dont_inline on
BattleUnitPart* BtlUnit_GetPartsPtr(BattleUnit* unit, s32 partNum) {
    BattleUnitPart* part = unit->mParts;

    if (partNum < 0) {
        return part;
    }
    if (part->mNextPart == NULL) {
        return part;
    }
    for (; part; part = part->mNextPart) {
        if (part->mKindPartParams->partNum == partNum) {
            return part;
        }
    }
    return part;
}
#pragma dont_inline reset


// Keep BtlUnit_GetBodyPartsId out of line, as in retail.
#pragma dont_inline on
s32 BtlUnit_GetBodyPartsId(BattleUnit* unit) {
    BattleUnitPart* part;
    s32 ret = -1;

    if (unit) {
        for (part = unit->mParts; part; part = part->mNextPart) {
            if (part->attributes & kMainPart) {
                ret = part->mKindPartParams->partNum;
                break;
            }
        }
    }
    return ret;
}
#pragma dont_inline reset

// Keep BtlUnit_GetPos out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_GetPos(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mPosition.x;
    *y = unit->mPosition.y;
    *z = unit->mPosition.z;
}
#pragma dont_inline reset

// Keep BtlUnit_SetPos out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mPosition.x = x;
    unit->mPosition.y = y;
    unit->mPosition.z = z;
}
#pragma dont_inline reset

void BtlUnit_AddPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mPosition.x += x;
    unit->mPosition.y += y;
    unit->mPosition.z += z;
}

void BtlUnit_GetPartsPos(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    *x = part->mPosition.x;
    *y = part->mPosition.y;
    *z = part->mPosition.z;
}

void BtlUnit_SetPartsPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mPosition.x = x;
    part->mPosition.y = y;
    part->mPosition.z = z;
}
void BtlUnit_AddPartsPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mPosition.x += x;
    part->mPosition.y += y;
    part->mPosition.z += z;
}

// Keep BtlUnit_GetPartsWorldPos out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_GetPartsWorldPos(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    BattleUnit* owner = part->mOwner;
    f32 ownerX, offsetX, ownerY, offsetY, ownerZ, offsetZ;

    if (part->attributes & kAbsolutePosition) {
        *x = part->mPosition.x;
        *y = part->mPosition.y;
        *z = part->mPosition.z;
    } else {
        BtlUnit_GetPos(owner, &ownerX, &ownerY, &ownerZ);
        BtlUnit_GetPartsOffsetPos(part, &offsetX, &offsetY, &offsetZ);
        *x = ownerX + offsetX * (f32)owner->faceDirection;
        *y = ownerY + offsetY;
        *z = ownerZ + offsetZ;
    }
}
#pragma dont_inline reset


void BtlUnit_GetHitPos(BattleUnit* unit, BattleUnitPart* part, f32* x, f32* y, f32* z) {
    f32 y_multiply = 1.0f;
    s32 direction = _battleWorkPointer->alliances[unit->mAlliance].attackDirection;
    BtlUnit_GetPartsWorldPos(part, x, y, z);
    //add hit base offset
    *x = ((f32)direction * (part->mHitBaseOffset.x * unit->mSizeMultiplier)) + *x;
    *y = (y_multiply * (part->mHitBaseOffset.y * unit->mSizeMultiplier)) + *y;
    *z = (part->mHitBaseOffset.z * unit->mSizeMultiplier) + *z;
    //add hit offset
    *x = ((f32)direction * (part->mHitOffset.x * unit->mSizeMultiplier)) + *x;
    *y = (y_multiply * (part->mHitOffset.y * unit->mSizeMultiplier)) + *y;
    *z = (part->mHitOffset.z * unit->mSizeMultiplier) + *z;
}

void BtlUnit_SetHitOffset(BattleUnit* unit, BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mHitOffset.x = x;
    part->mHitOffset.y = y;
    part->mHitOffset.z = z;
}
void BtlUnit_SetHitCursorOffset(BattleUnit* unit, BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mHitCursorOffset.x = x;
    part->mHitCursorOffset.y = y;
    part->mHitCursorOffset.z = z;
}

void BtlUnit_GetHomePos(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mHomePosition.x;
    *y = unit->mHomePosition.y;
    *z = unit->mHomePosition.z;
}

// Keep BtlUnit_SetHomePos out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetHomePos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mHomePosition.x = x;
    unit->mHomePosition.y = y;
    unit->mHomePosition.z = z;
}
#pragma dont_inline reset


void BtlUnit_AddHomePos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mHomePosition.x += x;
    unit->mHomePosition.y += y;
    unit->mHomePosition.z += z;
}

void BtlUnit_SetPartsHomePos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mHomePosition.x = x;
    part->mHomePosition.y = y;
    part->mHomePosition.z = z;
}

// Keep BtlUnit_SetRotate out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetRotate(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 new_x, new_y, new_z;

    new_x = reviseAngle(x);
    if (new_x >= 360.0f) {
        new_x = 0.0f;
    }
    unit->mRotation.x = new_x;

    new_y = reviseAngle(y);
    if (new_y >= 360.0f) {
        new_y = 0.0f;
    }
    unit->mRotation.y = new_y;

    new_z = reviseAngle(z);
    if (new_z >= 360.0f) {
        new_z = 0.0f;
    }
    unit->mRotation.z = new_z;
}
#pragma dont_inline reset


void BtlUnit_GetRotate(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mRotation.x;
    *y = unit->mRotation.y;
    *z = unit->mRotation.z;
}
void BtlUnit_AddRotate(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mRotation.x = reviseAngle(unit->mRotation.x + x);
    if (unit->mRotation.x >= 360.0f) {
        unit->mRotation.x = 0.0f;
    }
    unit->mRotation.y = reviseAngle(unit->mRotation.y + y);
    if (unit->mRotation.y >= 360.0f) {
        unit->mRotation.y = 0.0f;
    }
    unit->mRotation.z = reviseAngle(unit->mRotation.z + z);
    if (unit->mRotation.z >= 360.0f) {
        unit->mRotation.z = 0.0f;
    }
}

// Keep BtlUnit_SetPartsRotate out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetPartsRotate(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mRotation.x = x;
    part->mRotation.y = y;
    part->mRotation.z = z;
}
#pragma dont_inline reset


void BtlUnit_GetPartsRotate(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    *x = part->mRotation.x;
    *y = part->mRotation.y;
    *z = part->mRotation.z;
}

void BtlUnit_AddPartsRotate(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mRotation.x += x;
    if (part->mRotation.x > 360.0f) {
        part->mRotation.x -= 360.0f;
    } else if (part->mRotation.x < 0.0f) {
        part->mRotation.x += 360.0f;
    }

    part->mRotation.y += y;
    if (part->mRotation.y > 360.0f) {
        part->mRotation.y -= 360.0f;
    } else if (part->mRotation.y < 0.0f) {
        part->mRotation.y += 360.0f;
    }

    part->mRotation.z += z;
    if (part->mRotation.z > 360.0f) {
        part->mRotation.z -= 360.0f;
    } else if (part->mRotation.z < 0.0f) {
        part->mRotation.z += 360.0f;
    }
}
// Keep BtlUnit_SetBaseRotate out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetBaseRotate(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mBaseRotation.x = reviseAngle(x);
    unit->mBaseRotation.y = reviseAngle(y);
    unit->mBaseRotation.z = reviseAngle(z);
}
#pragma dont_inline reset

void BtlUnit_GetBaseRotate(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mBaseRotation.x;
    *y = unit->mBaseRotation.y;
    *z = unit->mBaseRotation.z;
}
// Keep BtlUnit_SetPartsBaseRotate out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetPartsBaseRotate(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mBaseRotation.x = x;
    part->mBaseRotation.y = y;
    part->mBaseRotation.z = z;
}
#pragma dont_inline reset

void BtlUnit_GetPartsBaseRotate(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    *x = part->mBaseRotation.x;
    *y = part->mBaseRotation.y;
    *z = part->mBaseRotation.z;
}

// Keep BtlUnit_SetRotateOffset out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetRotateOffset(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mRotationOffset.x = x;
    }
    if (unchanged != y) {
        unit->mRotationOffset.y = y;
    }
    if (unchanged != z) {
        unit->mRotationOffset.z = z;
    }
}
#pragma dont_inline reset


// Keep BtlUnit_SetPartsRotateOffset out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetPartsRotateOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mRotationOffset.x = x;
    }
    if (unchanged != y) {
        part->mRotationOffset.y = y;
    }
    if (unchanged != z) {
        part->mRotationOffset.z = z;
    }
}
#pragma dont_inline reset


void BtlUnit_AddPartsRotateOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mRotationOffset.x += x;
    part->mRotationOffset.y += y;
    part->mRotationOffset.z += z;
}

// Keep BtlUnit_SetBaseScale out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetBaseScale(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mBaseScale.x = x;
    }
    if (unchanged != y) {
        unit->mBaseScale.y = y;
    }
    if (unchanged != z) {
        unit->mBaseScale.z = z;
    }
}
#pragma dont_inline reset

void BtlUnit_GetScale(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mScale.x;
    *y = unit->mScale.y;
    *z = unit->mScale.z;
}

// Keep BtlUnit_SetScale out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetScale(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mScale.x = x;
    }
    if (unchanged != y) {
        unit->mScale.y = y;
    }
    if (unchanged != z) {
        unit->mScale.z = z;
    }
}
#pragma dont_inline reset


void BtlUnit_AddScale(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mScale.x += x;
    }
    if (unchanged != y) {
        unit->mScale.y += y;
    }
    if (unchanged != z) {
        unit->mScale.z += z;
    }
}

// Keep BtlUnit_SetPartsBaseScale out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetPartsBaseScale(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mBaseScale.x = x;
    }
    if (unchanged != y) {
        part->mBaseScale.y = y;
    }
    if (unchanged != z) {
        part->mBaseScale.z = z;
    }
}
#pragma dont_inline reset


// Keep BtlUnit_SetPartsScale out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetPartsScale(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mScale.x = x;
    }
    if (unchanged != y) {
        part->mScale.y = y;
    }
    if (unchanged != z) {
        part->mScale.z = z;
    }
}
#pragma dont_inline reset

void BtlUnit_AddPartsScale(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mScale.x += x;
    }
    if (unchanged != y) {
        part->mScale.y += y;
    }
    if (unchanged != z) {
        part->mScale.z += z;
    }
}

s32 BtlUnit_GetWidth(BattleUnit* unit) {
    return unit->mWidth;
}

s32 BtlUnit_GetHeight(BattleUnit* unit) {
    return unit->mHeight;
}
void BtlUnit_SetHeight(BattleUnit* unit, s16 height) {
    unit->mHeight = height;
}

// Keep BtlUnit_SetOffsetPos out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetOffsetPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mPositionOffset.x = x;
    }
    if (unchanged != y) {
        unit->mPositionOffset.y = y;
    }
    if (unchanged != z) {
        unit->mPositionOffset.z = z;
    }
}
#pragma dont_inline reset


// Keep BtlUnit_GetPartsOffsetPos out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_GetPartsOffsetPos(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    *x = part->mPositionOffset.x;
    *y = part->mPositionOffset.y;
    *z = part->mPositionOffset.z;
}
#pragma dont_inline reset

void BtlUnit_SetPartsOffsetPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mPositionOffset.x = x;
    }
    if (unchanged != y) {
        part->mPositionOffset.y = y;
    }
    if (unchanged != z) {
        part->mPositionOffset.z = z;
    }
}

void BtlUnit_AddPartsOffsetPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mPositionOffset.x += x;
    part->mPositionOffset.y += y;
    part->mPositionOffset.z += z;
}

void BtlUnit_SetDispOffset(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mDisplayOffset.x = x;
    }
    if (unchanged != y) {
        unit->mDisplayOffset.y = y;
    }
    if (unchanged != z) {
        unit->mDisplayOffset.z = z;
    }
}
void BtlUnit_SetPartsDispOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mDisplayOffset.x = x;
    }
    if (unchanged != y) {
        part->mDisplayOffset.y = y;
    }
    if (unchanged != z) {
        part->mDisplayOffset.z = z;
    }
}

void BtlUnit_AddPartsDispOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mDisplayOffset.x += x;
    }
    if (unchanged != y) {
        part->mDisplayOffset.y += y;
    }
    if (unchanged != z) {
        part->mDisplayOffset.z += z;
    }
}
void BtlUnit_SetMoveStartPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mMoveStartPos.x = x;
    }
    if (unchanged != y) {
        unit->mMoveStartPos.y = y;
    }
    if (unchanged != z) {
        unit->mMoveStartPos.z = z;
    }
}

void BtlUnit_SetPartsMoveStartPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mMoveStartPos.x = x;
    }
    if (unchanged != y) {
        part->mMoveStartPos.y = y;
    }
    if (unchanged != z) {
        part->mMoveStartPos.z = z;
    }
}
void BtlUnit_SetMoveCurrentPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mMoveCurrentPos.x = x;
    }
    if (unchanged != y) {
        unit->mMoveCurrentPos.y = y;
    }
    if (unchanged != z) {
        unit->mMoveCurrentPos.z = z;
    }
}
void BtlUnit_SetPartsMoveCurrentPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mMoveCurrentPos.x = x;
    }
    if (unchanged != y) {
        part->mMoveCurrentPos.y = y;
    }
    if (unchanged != z) {
        part->mMoveCurrentPos.z = z;
    }
}

void BtlUnit_SetMoveTargetPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        unit->mMoveTargetPos.x = x;
    }
    if (unchanged != y) {
        unit->mMoveTargetPos.y = y;
    }
    if (unchanged != z) {
        unit->mMoveTargetPos.z = z;
    }
}
void BtlUnit_SetPartsMoveTargetPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 unchanged = -250000000.0f;
    if (unchanged != x) {
        part->mMoveTargetPos.x = x;
    }
    if (unchanged != y) {
        part->mMoveTargetPos.y = y;
    }
    if (unchanged != z) {
        part->mMoveTargetPos.z = z;
    }
}

void BtlUnit_SetFallAccel(BattleUnit* unit, f32 fallAccel) {
    unit->mFallAccel = fallAccel;
}
void BtlUnit_SetPartsFallAccel(BattleUnitPart* part, f32 fallAccel) {
    part->mFallAccel = fallAccel;
}

void BtlUnit_SetMoveSpeed(BattleUnit* unit, f32 moveSpeedXZ) {
    unit->mMoveSpeedXZ = moveSpeedXZ;
}
void BtlUnit_SetPartsMoveSpeed(BattleUnitPart* part, f32 moveSpeedXZ) {
    part->mMoveSpeedXZ = moveSpeedXZ;
}
void BtlUnit_SetJumpSpeed(BattleUnit* unit, f32 moveSpeedY) {
    unit->mMoveSpeedY = moveSpeedY;
}

s8 BtlUnit_GetBelong(BattleUnit* unit) {
    return unit->mAlliance;
}

// Keep _CheckMoveCount out of line: the original calls it rather than inlining.
#pragma dont_inline on
void _CheckMoveCount(BattleUnit* unit) {
    s8 maxmoves;

    if (!(unit->mTokenFlags & kCurrentlyActing)) {
        maxmoves = unit->maxMovesThisTurn;
        if (maxmoves > 0 && maxmoves == unit->movesRemaining) {
            BtlUnit_ResetMoveStatus(unit);
        }
    }
}
#pragma dont_inline reset


void BtlUnit_GetStatus(BattleUnit* unit, StatusEffectType type, s8* turns, s8* strength) {
    s8 status_turns = 0, status_strength = 0;

    switch ((s8)type) {
        case STATUS_SLEEP:
            status_strength = unit->mSleepTurns;
            status_turns = status_strength;
            break;
        case STATUS_STOP:
            status_strength = unit->mStopTurns;
            status_turns = status_strength;
            break;
        case STATUS_DIZZY:
            status_strength = unit->mDizzyTurns;
            status_turns = status_strength;
            break;
        case STATUS_POISON:
            status_turns = unit->mPoisonTurns;
            status_strength = unit->mPoisonStrength;
            break;
        case STATUS_CONFUSE:
            status_strength = unit->mConfusionTurns;
            status_turns = status_strength;
            break;
        case STATUS_ELECTRIC:
            status_strength = unit->mElectricTurns;
            status_turns = status_strength;
            break;
        case STATUS_DODGY:
            status_strength = unit->mDodgyTurns;
            status_turns = status_strength;
            break;
        case STATUS_BURN:
            status_strength = unit->mBurnTurns;
            status_turns = status_strength;
            break;
        case STATUS_FREEZE:
            status_strength = unit->mFreezeTurns;
            status_turns = status_strength;
            break;
        case STATUS_HUGE:
            if (unit->mSizeChangeStrength > 0) {
                status_turns = unit->mSizeChangeTurns;
                status_strength = unit->mSizeChangeStrength;
            }
            break;
        case STATUS_TINY:
            if (unit->mSizeChangeStrength < 0) {
                status_turns = unit->mSizeChangeTurns;
                status_strength = unit->mSizeChangeStrength;
            }
            break;
        case STATUS_ATTACK_UP:
            if (unit->mAtkChangeStrength > 0) {
                status_turns = unit->mAtkChangeTurns;
                status_strength = unit->mAtkChangeStrength;
            }
            break;
        case STATUS_ATTACK_DOWN:
            if (unit->mAtkChangeStrength < 0) {
                status_turns = unit->mAtkChangeTurns;
                status_strength = unit->mAtkChangeStrength;
            }
            break;
        case STATUS_DEFENSE_UP:
            if (unit->mDefChangeStrength > 0) {
                status_turns = unit->mDefChangeTurns;
                status_strength = unit->mDefChangeStrength;
            }
            break;
        case STATUS_DEFENSE_DOWN:
            if (unit->mDefChangeStrength < 0) {
                status_turns = unit->mDefChangeTurns;
                status_strength = unit->mDefChangeStrength;
            }
            break;
        case STATUS_FAST:
            status_strength = unit->mFastTurns;
            status_turns = status_strength;
            break;
        case STATUS_SLOW:
            status_strength = unit->mSlowTurns;
            status_turns = status_strength;
            break;
        case STATUS_CHARGE:
            status_strength = unit->mChargeStrength;
            status_turns = status_strength;
            break;
        case STATUS_ALLERGIC:
            status_strength = unit->mAllergicTurns;
            status_turns = status_strength;
            break;
        case STATUS_FLIPPED:
            status_strength = unit->flippedTurns;
            status_turns = status_strength;
            break;
        case STATUS_INVISIBLE:
            status_strength = unit->mInvisibleTurns;
            status_turns = status_strength;
            break;
        case STATUS_PAYBACK:
            status_strength = unit->mPaybackTurns;
            status_turns = status_strength;
            break;
        case STATUS_HOLD_FAST:
            status_strength = unit->mHoldFastTurns;
            status_turns = status_strength;
            break;
        case STATUS_HP_REGEN:
            status_turns = unit->mHpRegenTurns;
            status_strength = unit->mHpRegenStrength;
            break;
        case STATUS_FP_REGEN:
            status_turns = unit->mFpRegenTurns;
            status_strength = unit->mFpRegenStrength;
            break;
        case STATUS_INSTANT_KILL:
            status_strength = unit->mInstantKillStrength;
            status_turns = status_strength;
            break;
        default:
            break;
    }
    if (turns) {
        *turns = status_turns;
    }
    if (strength) {
        *strength = status_strength;
    }
}

BOOL BtlUnit_SetStatus(BattleUnit* unit, StatusEffectType type, s8 turns, s8 strength) {
    switch ((s8)type) {
        case STATUS_SLEEP:
            unit->mSleepTurns = turns;
            unit->mSleepStrength = 0;
            break;
        case STATUS_STOP:
            unit->mStopTurns = turns;
            break;
        case STATUS_DIZZY:
            unit->mDizzyTurns = turns;
            break;
        case STATUS_POISON:
            unit->mPoisonTurns = turns;
            unit->mPoisonStrength = strength;
            break;
        case STATUS_CONFUSE:
            unit->mConfusionTurns = turns;
            break;
        case STATUS_ELECTRIC:
            unit->mElectricTurns = turns;
            break;
        case STATUS_DODGY:
            unit->mDodgyTurns = turns;
            break;
        case STATUS_BURN:
            if (unit->mFreezeTurns) {
                unit->mFreezeTurns = 0;
                return FALSE;
            }
            unit->mBurnTurns = turns;
            break;
        case STATUS_FREEZE:
            if (unit->mBurnTurns) {
                unit->mBurnTurns = 0;
                return FALSE;
            }
            unit->mFreezeTurns = turns;
            break;
        case STATUS_HUGE:
        case STATUS_TINY:
            if (!strength) {
                if ((s8)type == STATUS_HUGE) {
                    if (unit->mSizeChangeStrength >= 0) {
                        unit->mSizeChangeTurns = 0;
                        unit->mSizeChangeStrength = 0;
                        return FALSE;
                    }
                }
                else if (unit->mSizeChangeStrength <= 0) {
                    unit->mSizeChangeTurns = 0;
                    unit->mSizeChangeStrength = 0;
                    return FALSE;
                }
                return FALSE;
            }
            unit->mSizeChangeTurns = turns;
            unit->mSizeChangeStrength = strength;
            break;
        case STATUS_ATTACK_UP:
            if (!strength) {
                if (unit->mAtkChangeStrength >= 0) {
                    unit->mAtkChangeTurns = 0;
                    unit->mAtkChangeStrength = 0;
                }
                return FALSE;
            }
            unit->mAtkChangeTurns = turns;
            unit->mAtkChangeStrength = strength;
            break;
        case STATUS_ATTACK_DOWN:
            if (!strength) {
                if (unit->mAtkChangeStrength <= 0) {
                    unit->mAtkChangeTurns = 0;
                    unit->mAtkChangeStrength = 0;
                }
                return FALSE;
            }
            unit->mAtkChangeTurns = turns;
            unit->mAtkChangeStrength = strength;
            break;
        case STATUS_DEFENSE_UP:
        case STATUS_DEFENSE_DOWN:
            if (strength > 0) {
                if (unit->mDefChangeStrength < 0) {
                    unit->mDefChangeTurns = 0;
                    unit->mDefChangeStrength = 0;
                    return FALSE;
                }
                unit->mDefChangeTurns = turns;
                unit->mDefChangeStrength = strength;
            }
            else {
                if (strength == 0) {
                    if ((s8)type == STATUS_DEFENSE_DOWN) {
                        if (unit->mDefChangeStrength <= 0) {
                            unit->mDefChangeTurns = 0;
                            unit->mDefChangeStrength = 0;
                        }
                    }
                    else if (unit->mDefChangeStrength >= 0) {
                        unit->mDefChangeTurns = 0;
                        unit->mDefChangeStrength = 0;
                    }
                    return FALSE;
                }
                if (unit->mDefChangeStrength > 0) {
                    unit->mDefChangeTurns = 0;
                    unit->mDefChangeStrength = 0;
                    return FALSE;
                }
                unit->mDefChangeTurns = turns;
                unit->mDefChangeStrength = strength;
            }
            break;
        case STATUS_CHARGE:
            unit->mChargeStrength += strength;
            if (unit->mChargeStrength < 0) {
                unit->mChargeStrength = 0;
                return FALSE;
            }
            if (unit->mChargeStrength > 99) {
                unit->mChargeStrength = 99;
            }
            break;
        case STATUS_ALLERGIC:
            if (turns > 0) {
                unit->mAllergicTurns = turns;
            }
            else {
                unit->mAllergicTurns = 0;
                return FALSE;
            }
            break;
        case STATUS_FLIPPED:
            unit->flippedTurns = turns;
            break;
        case STATUS_INVISIBLE:
            unit->mInvisibleTurns = turns;
            break;
        case STATUS_PAYBACK:
            if (turns > 0) {
                unit->mPaybackTurns = turns;
            }
            else {
                unit->mPaybackTurns = 0;
                return FALSE;
            }
            break;
        case STATUS_HOLD_FAST:
            unit->mHoldFastTurns = turns;
            break;
        case STATUS_FAST:
            if (unit->mSlowTurns > 0) {
                if (turns > 0) {
                    unit->mSlowTurns = 0;
                    _CheckMoveCount(unit);
                    return FALSE;
                }
                unit->mFastTurns = turns;
            }
            else {
                unit->mFastTurns = turns;
            }
            _CheckMoveCount(unit);
            break;
        case STATUS_SLOW:
            if (unit->mFastTurns > 0) {
                if (turns > 0) {
                    unit->mFastTurns = 0;
                    _CheckMoveCount(unit);
                    return FALSE;
                }
                unit->mSlowTurns = turns;
            }
            else {
                unit->mSlowTurns = turns;
            }
            _CheckMoveCount(unit);
            break;
        case STATUS_HP_REGEN:
            unit->mHpRegenTurns = turns;
            unit->mHpRegenStrength = strength;
            break;
        case STATUS_FP_REGEN:
            unit->mFpRegenTurns = turns;
            unit->mFpRegenStrength = strength;
            break;
        case STATUS_INSTANT_KILL:
            unit->mInstantKillStrength = turns;
            break;
        default:
            break;
    }
    return TRUE;
}
//TODO: define that chunk of statuses to do sizeof?
// Keep BtlUnit_ClearStatus out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_ClearStatus(BattleUnit* unit) {
    memset(&unit->mSleepTurns, 0, 0x1Eu);
    BtlUnit_SetStatus(unit, STATUS_CHARGE, 1, -99);
    BattleStatusChangeInfoWorkInit(unit);
}
#pragma dont_inline reset

BOOL BtlUnit_CheckRecoveryStatus(BattleUnit* unit, StatusEffectType type) {
    s8 turns, strength;

    if (!unit) {
        return FALSE;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
        return FALSE;
    }
    if (unit->currentHp <= 0) {
        return FALSE;
    }
    BtlUnit_GetStatus(unit, type, &turns, &strength);
    if (!turns) {
        return FALSE;
    }
    switch ((s8)type) {
        case STATUS_SLEEP:
            if (turns > 1) {
                turns--;
            }
            else {
                turns = 0;
            }
            unit->mSleepTurns = turns;
            if (turns == 0) {
                return TRUE;
            }
            return FALSE;
        case STATUS_ALLERGIC:
        case STATUS_STOP:
        case STATUS_DIZZY:
        case STATUS_CONFUSE:
        case STATUS_ELECTRIC:
        case STATUS_DODGY:
        case STATUS_BURN:
        case STATUS_FREEZE:
        case STATUS_INVISIBLE:
        case STATUS_FAST:
        case STATUS_SLOW:
        case STATUS_PAYBACK:
        case STATUS_HOLD_FAST:
            if (turns >= 0) {
                turns--;
            }
            else {
                turns++;
            }
            if (strength >= 0) {
                strength--;
            }
            else {
                strength++;
            }
            if (!turns) {
                strength = 0;
            }
            BtlUnit_SetStatus(unit, type, turns, strength);
            if (turns == 0) {
                return TRUE;
            }
            return FALSE;
        case STATUS_HUGE:
        case STATUS_TINY:
            if (turns > 0) {
                turns--;
            }
            else {
                if (turns < 0) {
                    turns++;
                }
                else {
                    return FALSE;
                }
            }
            if (!turns) {
                strength = 0;
            }
            unit->mSizeChangeTurns = turns;
            unit->mSizeChangeStrength = strength;
            if (turns == 0) {
                return TRUE;
            }
            return FALSE;
        case STATUS_ATTACK_UP:
        case STATUS_ATTACK_DOWN:
            if (turns > 0) {
                turns--;
            }
            else {
                if (turns < 0) {
                    turns++;
                }
                else {
                    return FALSE;
                }
            }
            if (!turns) {
                strength = 0;
            }
            unit->mAtkChangeTurns = turns;
            unit->mAtkChangeStrength = strength;
            if (turns == 0) {
                return TRUE;
            }
            return FALSE;
        case STATUS_POISON:
        case STATUS_DEFENSE_UP:
        case STATUS_DEFENSE_DOWN:
        case STATUS_HP_REGEN:
        case STATUS_FP_REGEN:
            if (turns >= 0) {
                turns--;
            }
            else {
                turns++;
            }
            if (!turns) {
                strength = 0;
            }
            BtlUnit_SetStatus(unit, type, turns, strength);
            if (turns == 0) {
                return TRUE;
            }
            return FALSE;
        default:
            return FALSE;
    }
}

// Keep BtlUnit_CheckStatus out of line: the original calls it rather than inlining.
#pragma dont_inline on
BOOL BtlUnit_CheckStatus(BattleUnit* unit, StatusEffectType type) {
    s8 turns, strength;

    if (!unit) {
        return FALSE;
    }
    BtlUnit_GetStatus(unit, type, &turns, &strength);
    if (turns) {
        return TRUE;
    }
    if ((s8)type == STATUS_ELECTRIC && unit->badgesEquipped.ZapTap) {
        return TRUE;
    }
    return FALSE;
}
#pragma dont_inline reset


// Keep BtlUnit_CheckStatusFlag out of line: the original calls it rather than inlining.
#pragma dont_inline on
s32 BtlUnit_CheckStatusFlag(BattleUnit* unit, s32 flags) {
    if (unit) {
        return unit->mStatusFlags & flags;
    }
    return 0;
}
#pragma dont_inline reset


void BtlUnit_OnStatusFlag(BattleUnit* unit, s32 flags) {
    unit->mStatusFlags |= flags;
}

void BtlUnit_OffStatusFlag(BattleUnit* unit, s32 flags) {
    unit->mStatusFlags &= ~flags;
}

s32 BtlUnit_CheckUnitFlag(BattleUnit* unit, s32 flags) {
    return unit->flags & flags;
}

void BtlUnit_OnUnitFlag(BattleUnit* unit, s32 flags) {
    unit->flags |= flags;
}

void BtlUnit_OffUnitFlag(BattleUnit* unit, s32 flags) {
    unit->flags &= ~flags;
}

// Keep BtlUnit_GetPoseNameFromType out of line: the original calls it rather than inlining.
#pragma dont_inline on
char* BtlUnit_GetPoseNameFromType(BattleUnitPart* part, s32 type) {
    BattleDispPose* pose = part->poseTable;
    s32 bound;

    if (type <= 0x1C) {
        bound = 0x1C;
    } else if (type < 0x28) {
        bound = 0x27;
    } else if (type < 0x46) {
        bound = 0x45;
    } else {
        return NULL;
    }
    while (pose) {
        if ((s32)pose->poseId >= 0x46) {
            return NULL;
        }
        if ((s32)pose->poseId != type && (s32)pose->poseId != bound) {
            pose++;
            continue;
        }
        return (char*)pose->poseName;
    }
    return NULL;
}
#pragma dont_inline reset


// Keep BtlUnit_SetAnimType out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetAnimType(BattleUnitPart* part, s32 type) {
    const char* animName = BtlUnit_GetPoseNameFromType(part, type);

    if (type >= 0x1D && type < 0x28) {
        btlDispChangeAnime(part, animName, TRUE);
    } else {
        btlDispChangeAnime(part, animName, FALSE);
    }
    // The original emits the two adjacent cases as `cmp 0x1f; cmp 0x1d` with a
    // redundant branch, which a range test does not; a 2-case switch reproduces it.
    switch (type) {
    case 0x1D:
    case 0x1E:
        part->partFlags |= 0x10;
        break;
    }
}
#pragma dont_inline reset


void BtlUnit_SetAnim(BattleUnitPart* part, const char* animName) {
    btlDispChangeAnime(part, animName, FALSE);
}

void BtlUnit_SetBodyAnimType(BattleUnit* unit, s32 type) {
    s32 bodyId = BtlUnit_GetBodyPartsId(unit);

    BtlUnit_SetAnimType(BtlUnit_GetPartsPtr(unit, bodyId), type);
}

void BtlUnit_SetBodyAnim(BattleUnit* unit, const char* animName) {
    s32 bodyId = BtlUnit_GetBodyPartsId(unit);

    btlDispChangeAnime(BtlUnit_GetPartsPtr(unit, bodyId), animName, FALSE);
}

void BtlUnit_ChangeTalkAnim(s32 unitId) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, unitId);

    btlDispChangeAnime(BtlUnit_GetPartsPtr(unit, unit->mBodyPartsId),
                       unit->mTalkPoseName, FALSE);
}

void BtlUnit_ChangeStayAnim(s32 unitId) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, unitId);

    btlDispChangeAnime(BtlUnit_GetPartsPtr(unit, unit->mBodyPartsId),
                       unit->mStayPoseName, FALSE);
}

void BtlUnit_GetTalkTogePos(s32 unitId, f32* x, f32* y, f32* z) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, unitId);
    f32 x2, y2, z2;

    BtlUnit_GetPos(unit, &x2, &y2, &z2);
    x2 += (unit->mKindParams->talkTogeBaseOffset.x + unit->togeOffset.x) * (f32)unit->faceDirection;
    y2 += unit->mKindParams->talkTogeBaseOffset.y + unit->togeOffset.y;
    z2 += unit->mKindParams->talkTogeBaseOffset.z + unit->togeOffset.z;
    *x = x2;
    *y = y2;
    *z = z2;
}

BOOL BtlUnit_GetEnemyBelong(BattleUnit* unit) {
    switch (unit->mAlliance) {
        case 0:
            return TRUE;
        case 1:
            return FALSE;
        default:
            return unit->mAlliance;
    }
}

s32 BtlUnit_GetHitDamage(BattleUnit* unit) {
    return unit->hpDamageTaken;
}
s32 BtlUnit_GetTotalHitDamage(BattleUnit* unit) {
    return unit->totalHpDamageTaken;
}

void BtlUnit_SetTotalHitDamage(BattleUnit* unit, s16 damage) {
    unit->totalHpDamageTaken = damage;
}

s32 BtlUnit_GetACPossibility(BattleUnit* unit) {
    if (unit->mAlliance != 0) {
        return 0;
    }
    if (unit->currentKind == UNIT_SHELL_SHIELD) {
        return 0;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_SLEEP) || BtlUnit_CheckStatus(unit, STATUS_STOP) ||
        BtlUnit_CheckStatus(unit, STATUS_FREEZE) || BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
        return 0;
    }
    return !BtlUnit_CheckStatusFlag(unit, 1);
}
void* BtlUnit_GetData(BattleUnit* unit, s32 tag) {
    BattleDataEntry* entry = unit->dataTable;

    if (!entry) {
        return NULL;
    }
    while (entry->tag) {
        if (entry->tag == tag) {
            return entry->data;
        }
        entry++;
    }
    return NULL;
}

BOOL BtlUnit_CheckData(BattleUnit* unit, s32 tag) {
    BattleDataEntry* entry = unit->dataTable;

    if (!entry) {
        return FALSE;
    }
    while (entry->tag) {
        if (entry->tag == tag) {
            return TRUE;
        }
        entry++;
    }
    return FALSE;
}
BOOL BtlUnit_CanGuardStatus(BattleUnit* unit) {
    return BtlUnit_CanActStatus(unit);
}
// Keep BtlUnit_CanActStatus out of line: the original calls it rather than inlining.
#pragma dont_inline on
BOOL BtlUnit_CanActStatus(BattleUnit* unit) {
    if (BtlUnit_CheckStatus(unit, STATUS_SLEEP) || BtlUnit_CheckStatus(unit, STATUS_STOP) ||
        BtlUnit_CheckStatus(unit, STATUS_FREEZE) || BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
        return FALSE;
    }
    return !BtlUnit_CheckStatusFlag(unit, 1);
}
#pragma dont_inline reset


// Keep BtlUnit_SetParamFromPouch out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_SetParamFromPouch(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    PouchData* pouch = pouchGetPtr();

    if (unit->currentKind == UNIT_MARIO) {
        unit->currentHp = pouch->currentHP;
        unit->currentFp = pouch->currentFP;
        BtlUnit_ReviseHpFp(unit);
    } else if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
        BattleWorkPartyInfo* info;

#ifdef MUST_MATCH
        // The single PouchPartyData subscript folds into one indexed `lhax` load here; the
        // pointer form below makes MWCC materialize the party pointer and use `lha` displacement.
        unit->currentHp = ((s16*)pouch->partyData)[BattleTransPartyId(unit->currentKind) * 7 + 3];
#else
        unit->currentHp = pouch->partyData[BattleTransPartyId(unit->currentKind)].currentHP;
#endif
        BtlUnit_ReviseHpFp(unit);
        info = &wp->partyInfo[unit->currentKind - TYPE_PARTNER_MIN];
        unit->mSizeMultiplier = info->field_0;
        unit->mStatusFlags = info->field_24;
#ifdef MUST_MATCH
        *(UnitStatusBlock*)&unit->mSleepTurns = *(UnitStatusBlock*)info->field_4;
#else
        memcpy(&unit->mSleepTurns, info->field_4, sizeof(info->field_4));
#endif
        if (unit->currentHp <= 0) {
            BtlUnit_SetStatus(unit, STATUS_INSTANT_KILL, 1, 1);
        }
    }
}
#pragma dont_inline reset


// Keep BtlUnit_ReviseHpFp out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_ReviseHpFp(BattleUnit* unit) {
    PouchData* pouch = pouchGetPtr();

    if (unit->currentKind == UNIT_MARIO) {
        unit->baseMaxHp = pouch->baseMaxHP;
        unit->maxHp = pouch->baseMaxHP + unit->badgesEquipped.HpPlus * 5;
        unit->baseMaxFp = pouch->baseMaxFP;
        unit->maxFp = pouch->baseMaxFP + unit->badgesEquipped.FpPlus * 5;
        if (unit->currentHp > unit->maxHp) {
            unit->currentHp = unit->maxHp;
        }
        if (unit->currentFp > unit->maxFp) {
            unit->currentFp = unit->maxFp;
        }
    } else if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
        s32 party = BattleTransPartyId(unit->currentKind);

        unit->baseMaxHp = pouch->partyData[party].baseMaxHP;
        unit->maxHp = pouch->partyData[party].baseMaxHP + unit->badgesEquipped.HpPlus * 5;
        unit->currentHp = pouch->partyData[party].currentHP;
        if (unit->currentHp > unit->maxHp) {
            unit->currentHp = unit->maxHp;
        }
    } else {
        unit->maxHp = unit->baseMaxHp + unit->badgesEquipped.HpPlus * 5;
        if (unit->currentHp > unit->maxHp) {
            unit->currentHp = unit->maxHp;
        }
        unit->maxFp = unit->baseMaxFp + unit->badgesEquipped.FpPlus * 5;
        if (unit->currentFp > unit->maxFp) {
            unit->currentFp = unit->maxFp;
        }
    }
}
#pragma dont_inline reset


void BtlUnit_SetParamToPouch(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    PouchData* pouch = pouchGetPtr();

    if (unit->currentKind == UNIT_MARIO) {
        pouch->currentHP = unit->currentHp;
        pouch->currentFP = unit->currentFp;
    } else if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
        BattleWorkPartyInfo* info;
        s32 party;

        party = BattleTransPartyId(unit->currentKind);
        *(s16*)((u8*)pouch + party * 0xE + 6) = unit->currentHp;
        info = &wp->partyInfo[unit->currentKind - TYPE_PARTNER_MIN];
        info->field_0 = unit->mSizeMultiplier;
        info->field_24 = unit->mStatusFlags;
#ifdef MUST_MATCH
        *(UnitStatusBlock*)info->field_4 = *(UnitStatusBlock*)&unit->mSleepTurns;
#else
        memcpy(info->field_4, &unit->mSleepTurns, sizeof(info->field_4));
#endif
    }
}

void BtlUnit_CheckPinchStatus(BattleUnit* unit, BOOL a2) {
    if (unit->currentHp <= unit->mKindParams->mPerilHP && unit->currentHp < unit->maxHp) {
        if (!(unit->mStatusFlags & 0x20000000)) {
            unit->mStatusFlags |= 0x20000000;
            if (!a2 && unit->currentKind == UNIT_MARIO) {
                BattleAudience_Case_MarioDanger();
            }
        }
    } else if (unit->mStatusFlags & 0x20000000) {
        unit->mStatusFlags &= ~0x20000000;
    }

    if (unit->currentHp <= unit->mKindParams->mDangerHP && unit->currentHp < unit->maxHp) {
        if (!(unit->mStatusFlags & 0x10000000)) {
            unit->mStatusFlags |= 0x10000000;
            if (!(unit->mStatusFlags & 0x20000000) && !a2 && unit->currentKind == UNIT_MARIO) {
                BattleAudience_Case_MarioPinch();
            }
        }
    } else if (unit->mStatusFlags & 0x10000000) {
        unit->mStatusFlags &= ~0x10000000;
    }
}

// `case 5:` is grouped with `default:` on purpose: it seeds MWCC's switch decision tree at 4 like retail
// (without it the tree roots at 3 and needs an extra `cmpwi 5`). The `prod` temp pins the multiply order.
s32 BtlUnit_GetExp(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    PouchData* pouch = pouchGetPtr();
    FieldBattleInfo* info = wp->fbatInfo;
    f32 mult;
    f32 prod;
    // A local preserves the multiply operands while letting MWCC pool the constant.
    f32 half = 0.5f;
    s32 exp;

    if (pouch->level > unit->mLevel) {
        return 0;
    }
    switch (info->setup->groupSetup->numEnemies) {
        case 0:
        case 1:
        case 2:
            mult = 1.0f;
            break;
        case 3:
            mult = 1.1f;
            break;
        case 4:
            mult = 1.3f;
            break;
        case 5:
        default:
            mult = 1.5f;
            break;
    }
    prod = mult * (f32)(unit->mLevel - pouch->level);
    exp = (s32)(prod * half);
    return exp + unit->mKindParams->mBonusExp;
}


s32 BtlUnit_GetCoin(BattleUnit* unit) {
    s32 coins;
    s32 i;

    if (!unit) {
        return 0;
    }
    coins = unit->mKindParams->baseCoin;
    for (i = 0; i < unit->mKindParams->bonusCoinRolls; i++) {
        if (irand(100) < unit->mKindParams->bonusCoinRate) {
            coins++;
        }
    }
    return coins;
}


// Keep BtlUnit_GetFp out of line: the original calls it rather than inlining.
#pragma dont_inline on
s32 BtlUnit_GetFp(BattleUnit* unit) {
    BattleUnitType kind = unit->currentKind;
    BattleWork* wp = _battleWorkPointer;
    s32 result = unit->currentFp;

    if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        result = BattleGetMarioPtr(wp)->currentFp;
    }
    return result;
}
#pragma dont_inline reset


// Keep BtlUnit_SetFp out of line, as in retail.
#pragma dont_inline on
void BtlUnit_SetFp(BattleUnit* unit, s32 value) {
    BattleUnitType kind = unit->currentKind;
    BattleWork* wp = _battleWorkPointer;

    if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        BattleGetMarioPtr(wp)->currentFp = value;
    } else {
        unit->currentFp = value;
    }
}
#pragma dont_inline reset

void BtlUnit_RecoverHp(BattleUnit* unit, s32 value) {
    unit->currentHp += value;
    if (unit->currentHp > unit->maxHp) {
        unit->currentHp = unit->maxHp;
    }
}
void BtlUnit_RecoverFp(BattleUnit* unit, s32 value) {
    BattleUnitType kind = unit->currentKind;
    BattleWork* wp = _battleWorkPointer;
    s16 max;
    s16* fp; //but why

    if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        max = BtlUnit_GetMaxFp(BattleGetMarioPtr(wp));
        fp = &BattleGetMarioPtr(wp)->currentFp;
    }
    else {
        max = BtlUnit_GetMaxFp(unit);
        fp = &unit->currentFp;
    }
    *fp += value;
    if (*fp > max) {
        *fp = max;
    }
}
// Keep BtlUnit_GetMaxFp out of line: the original calls it rather than inlining.
#pragma dont_inline on
s32 BtlUnit_GetMaxFp(BattleUnit* unit) {
    BattleUnitType kind = unit->currentKind;
    BattleWork* wp = _battleWorkPointer;
    s32 result = unit->maxFp;

    if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        result = BattleGetMarioPtr(wp)->maxFp;
    }
    return result;
}
#pragma dont_inline reset


void BtlUnit_SetMaxFp(BattleUnit* unit, s32 value) {
    BattleUnitType kind = unit->currentKind;
    BattleWork* wp = _battleWorkPointer;

    if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        BattleGetMarioPtr(wp)->maxFp = value;
    } else {
        unit->maxFp = value;
    }
}

// Keep BtlUnit_GetWeaponCost out of line: the original calls it rather than inlining its switch.
#pragma dont_inline on
s32 BtlUnit_GetWeaponCost(BattleUnit* unit, BattleWeapon* weapon) {
    s32 cost = weapon->baseFpCost;
    s32 badgeCount;

    if (weapon->item != 0) {
        badgeCount = pouchEquipCheckBadge(weapon->item);
        switch (weapon->item) {
            case 0xF0:
            case 0xF1:
            case 0xF2:
            case 0xF3:
            case 0xF7:
            case 0xF8:
            case 0xF9:
            case 0xFC:
            case 0x100:
            case 0x101:
            case 0x14C:
            case 0x14D:
            case 0x14E:
            case 0x151:
            case 0x152:
                cost = (s32)(cost * pow(2.0, badgeCount - 1));
                break;
            case 0xF4:
            case 0xF5:
            case 0xF6:
            case 0xFB:
            case 0xFD:
                cost = cost * badgeCount;
                break;
            default:
                break;
        }
    }
    if (cost != 0) {
        cost -= unit->badgesEquipped.FlowerSaver;
        if (cost <= 0) {
            cost = 1;
        }
    }
    if (cost >= 1000) {
        cost = 999;
    }
    return cost;
}
#pragma dont_inline reset


BOOL BtlUnit_CheckWeaponCost(BattleUnit* unit, BattleWeapon* weapon) {
    s32 fp = BtlUnit_GetFp(unit);
    s32 cost = BtlUnit_GetWeaponCost(unit, weapon);

    if (fp < cost) {
        return FALSE;
    }
#ifdef MUST_MATCH
    /* Match-only: the header declares pouchGetAP as `s16`, but the target consumed the raw 32-bit
       return (no extsh). Call it through an s32-returning prototype, as evt_kinopio.c does. */
    return (*(s32(*)(void))pouchGetAP)() >= (s32)weapon->baseSpCost * 100;
#else
    return pouchGetAP() >= (s32)weapon->baseSpCost * 100;
#endif
}

void BtlUnit_PayWeaponCost(BattleUnit* unit, BattleWeapon* weapon) {
    BattleWork* wp = _battleWorkPointer;
    s32 fp;
    s32 cost;

    fp = BtlUnit_GetFp(unit);
    cost = BtlUnit_GetWeaponCost(unit, weapon);
    BtlUnit_SetFp(unit, fp - cost);
    if (unit->currentKind == UNIT_MARIO) {
        BtlActRec_AddPoint((u8*)wp + 0x16F2F, cost);
        BtlActRec_AddCount((u8*)wp + 0x16F30);
    } else if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
        BtlActRec_AddPoint((u8*)wp + 0x16F31, cost);
        BtlActRec_AddCount((u8*)wp + 0x16F32);
    }
#ifdef MUST_MATCH
    /* Match-only: the header declares pouchAddAP as taking `s16`, but the target passed the full
       32-bit delta (no extsh). Call it through an s32-taking prototype. */
    (*(void(*)(s32))pouchAddAP)(-weapon->baseSpCost * 100);
#else
    pouchAddAP(-weapon->baseSpCost * 100);
#endif
}

BattleUnit* BtlUnit_GetGuardKouraPtr(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;
    s32 id = unit->unitId;
    BattleUnit* other = NULL;

    for (i = 0; i < 64; i++) {
        other = BattleGetUnitPtr(wp, i);
        if (other && other->currentKind == UNIT_SHELL_SHIELD && other->work[0] == id) {
            break;
        }
    }
    if (i >= 64) {
        return NULL;
    }
    return other;
}

// Keep BtlUnit_ResetMoveStatus out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_ResetMoveStatus(BattleUnit* unit) {
    s32 count;

    unit->moveState = 0;
    unit->movesRemaining = unit->maxMoveCount;
    if (BtlUnit_CheckStatus(unit, STATUS_FAST)) {
        unit->movesRemaining *= 2;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_SLOW)) {
        if (unit->movesRemaining >= 2) {
            unit->movesRemaining /= 2;
        } else {
            count = 2;
            if (unit->maxMoveCount < 1) {
                count *= 2;
            }
            if (unit->activeTurns >= count) {
                unit->movesRemaining = 1;
                unit->moveState = 0;
            } else {
                unit->movesRemaining = 0;
                unit->moveState = 10;
            }
        }
    }
    unit->maxMovesThisTurn = unit->movesRemaining;
    if (unit->maxMovesThisTurn == 0) {
        unit->movesRemaining = 0;
        unit->moveState = 10;
    }
}
#pragma dont_inline reset


// `case 0` shares the default body: the original routes b == 0 through the same
// zero-fill as any other unlisted value, and MWCC emits its fall-through branch.
void BtlUnit_LoadSeMode(s32 mode, s32 action, void* src, void* dst) {
    BtlUnitSeModeSrc* s = src;
    BtlUnitSeMode* d = dst;

    d->beat = 0;
    d->sfxHandle = -1;
    switch (action) {
        case -1:
            switch (mode) {
                case 0:
                    d->sfxA = s->mode0SfxA;
                    d->sfxB = s->mode0SfxB;
                    d->delay = s->mode0Delay;
                    d->intervalA = s->mode0IntervalA;
                    d->intervalB = s->mode0IntervalB;
                    break;
                case 1:
                    d->sfxA = s->mode1SfxA;
                    d->sfxB = s->mode1SfxB;
                    d->delay = s->mode1Delay;
                    d->intervalA = s->mode1IntervalA;
                    d->intervalB = s->mode1IntervalB;
                    break;
                case 2:
                    d->sfxA = s->mode2SfxA;
                    d->sfxB = s->mode2SfxB;
                    d->delay = 0;
                    d->intervalA = 0;
                    d->intervalB = 0;
                    break;
                case 3:
                    d->sfxA = s->mode3SfxA;
                    d->sfxB = s->mode3SfxB;
                    d->delay = s->mode3Delay;
                    d->intervalA = s->mode3IntervalA;
                    d->intervalB = s->mode3IntervalB;
                    break;
                default:
                    d->sfxA = 0;
                    d->sfxB = 0;
                    d->delay = 0;
                    d->intervalA = 0;
                    d->intervalB = 0;
                    break;
            }
            d->countdown = d->delay;
            break;
        case -2:
            d->countdown = d->delay;
            break;
        case 0:
        default:
            d->sfxA = 0;
            d->sfxB = 0;
            d->delay = 0;
            d->intervalA = 0;
            d->intervalB = 0;
            d->countdown = d->delay;
            break;
    }
}


void BtlUnit_SetSeMode(BtlUnitSeMode* se, s32 sfxA, s32 sfxB, s16 delay, s16 interval, s16 unused, s16 param8, s16 paramA) {
    se->sfxA = sfxA;
    se->sfxB = sfxB;
    se->delay = delay;
    se->intervalA = interval;
    se->intervalB = interval;
    se->beat = 0;
    se->param8 = param8;
    se->paramA = paramA;
    se->sfxHandle = -1;
}

void BtlUnit_SetCommandAnimPose(BattleUnit* unit) {
    BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
    BOOL flag = FALSE;

    if ((unit->mStatusFlags & 0x30000000) == 0) {
        if (_GetStatusPoseType(unit) == 0x1C) {
            flag = TRUE;
        }
    }
    if (flag) {
        BtlUnit_SetAnimType(part, 0x43);
        part->partFlags |= 0x80;
    } else {
        btlDispPoseAnime(part);
    }
}

void BtlUnit_PoseSoundInit(BattleUnitPart* part) {
    part->poseSound.frame = 0;
    part->poseSound.prev = 0.0f;
    part->poseSound.cur = 0.0f;
}


// `opt_propagation off` keeps the 0x7F -> double conversion at runtime (`lis 0x4330` +
// an int-to-float conversion double, `fsub`, `fmul`) instead of folding 127 * 0.7 at compile time. The
// `vol` temp pins that conversion before the second psndSFX_vol call.
#pragma opt_propagation off
void BtlUnit_ControlPoseSoundMain(BattleUnitPart* part) {
#ifdef MUST_MATCH
    // A local static pools in this function's place in the original .rodata order.
    static const f64 double_0p7_802efac0 = 0.7;
#endif
    BattleWork* wp = _battleWorkPointer;
    BtlUnitPartPoseSound* ps;
    const char* anim;
    BtlUnitPoseSound* entry;
    AnimationPose* pose;
    s32 soundId;
    f32 total;
    u32 frame;

    if (!part) {
        return;
    }
    ps = &part->poseSound;
    entry = ps->table;
    if (!entry) {
        return;
    }
    if (part->poseId == -1) {
        return;
    }
    pose = animPoseGetAnimPosePtr(part->poseId);
    if (!pose) {
        return;
    }
    total = pose->clipTime / 60.0f;
    frame = __cvt_fp2unsigned(pose->loopTime);
    if ((u32)ps->frame > frame) {
        ps->prev = total;
        ps->cur = -1.0f;
        ps->frame = (s32)pose->loopTime;
        ps->count = 0;
        if (BattleGetSeq(wp, SEQ_PHASE) == 0x04000002 ||
            BattleGetSeq(wp, SEQ_PHASE) == 0x04000004) {
            ps->flag = 1;
        }
    } else if ((u32)ps->frame == frame && ps->prev > total) {
        ps->prev = total;
        ps->cur = -1.0f;
    } else if ((u32)ps->frame < frame) {
        ps->prev = total;
        ps->cur = -1.0f;
        ps->frame = (s32)pose->loopTime;
    } else {
        ps->cur = ps->prev;
        ps->prev = total;
    }
    if (ps->prev == ps->cur) {
        return;
    }
    anim = animPoseGetCurrentAnim(part->poseId)->name;
    for (; entry->poseName; entry++) {
        if (strcmp(entry->poseName, anim) != 0) {
            continue;
        }
        if (entry->volume <= ps->prev && entry->volume > ps->cur) {
            if (entry->flags & 1) {
                if (entry->callback) {
                    soundId = entry->callback(part->mOwner, entry, ps);
                } else {
                    soundId = BtlUnit_snd_se(part->mOwner, entry->soundName, 0xF1194D80, entry->pitch);
                }
                psndSFX_vol(soundId, 0x7F);
                ps->count++;
                if (part->mOwner->seqPhase != BattleGetSeq(wp, SEQ_PHASE)) {
                    u32 vol = 0x7F;
#ifdef MUST_MATCH
                    // The address read preserves retail's load and multiply order.
                    s32 v2 = (s32)((f64)vol * *(const f64*)&double_0p7_802efac0);
#else
                    s32 v2 = (s32)((f64)vol * 0.7);
#endif

                    psndSFX_vol(soundId, v2);
                }
            } else {
                if (entry->callback) {
                    entry->callback(part->mOwner, entry, ps);
                } else {
                    BtlUnit_snd_se(part->mOwner, entry->soundName, 0xF1194D80, entry->pitch);
                }
                ps->count++;
            }
        }
    }
}
#pragma opt_propagation reset


// Keep BtlUnit_snd_se out of line: retail calls it rather than inlining its Vec/GetPos body
// into BtlUnit_ControlPoseSoundMain.
#pragma dont_inline on
s32 BtlUnit_snd_se(BattleUnit* unit, const char* name, s32 b, s16 c) {
    Vec pos;

    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    return BtlUnit_snd_se_pos(unit, name, b, c, &pos);
}
#pragma dont_inline reset

// Keep the sound wrapper out of line, as in retail.
#pragma dont_inline on
s32 BtlUnit_snd_se_pos(BattleUnit* unit, const char* name, s32 b, s16 c, Vec* pos) {
    s32 handle;
    s16 val = 0;
    s8 turns, strength;

    if (BtlUnit_CheckStatus(unit, STATUS_TINY)) {
        BtlUnit_GetStatus(unit, STATUS_TINY, &turns, &strength);
        val = (s16)(((s8)strength * 2 - 1) * -341);
    }
    if (BtlUnit_CheckStatus(unit, STATUS_HUGE)) {
        BtlUnit_GetStatus(unit, STATUS_HUGE, &turns, &strength);
        val = (s16)(((s8)strength * 2 + 1) * -341);
    }
    // The s16 temporary preserves retail's add operand order and delayed narrowing.
    {
        s16 sum = c;
        sum += val;
        val = sum;
    }
    val += (irand(0x29) - 0x14);
    handle = psndSFXOn_3D(name, pos);
    psndSFX_pit(handle, val);
    psndSFX_vol(handle, (u32)(b - 0xF1190000) == 0x4D80 ? psndSFX_get_vol(handle) : (u8)b);
    return handle;
}
#pragma dont_inline reset

// The gauge type is compared with `lhz`/`cmpwi`; a `switch` (not an if/else) reproduces the
// original's `beq then; b else` dispatch.
// Keep BtlUnit_HpGaugeInit out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_HpGaugeInit(BattleUnit* unit) {
    HpGaugeLocal* gauge = (HpGaugeLocal*)&unit->hpGauge;
    f32 ratio;

    gauge->field_4 = 0;
    gauge->field_8 = 0;
    gauge->field_0 = 0;
    gauge->field_C = unit->currentHp;
    gauge->field_10 = unit->currentHp;
    ratio = (f32)unit->currentHp / (f32)unit->maxHp;
    gauge->field_14 = ratio;
    gauge->field_18 = ratio;
    switch ((s32)gauge->field_0) {
    case 1:
        gauge->field_6 = 0x28;
        gauge->field_A = gauge->field_6 + 0x1E;
        break;
    default:
        gauge->field_6 = 0x14;
        gauge->field_A = gauge->field_6 + 0x1E;
        break;
    }
}
#pragma dont_inline reset


void BtlUnit_HpGaugeMain(BattleUnit* unit) {
    HpGaugeLocal* gauge = (HpGaugeLocal*)&unit->hpGauge;
    f32 ratio;
    s32 maxHp;

    if (gauge->field_4 > 0) {
        gauge->field_4--;
        if (gauge->field_4 <= 0) {
            gauge->field_C = gauge->field_10;
            ratio = (f32)unit->currentHp / (f32)unit->maxHp;
            gauge->field_18 = ratio;
            gauge->field_14 = ratio;
        } else {
            switch ((s32)gauge->field_0) {
            case 1:
                gauge->field_14 = (f32)unit->currentHp / (f32)unit->maxHp;
                gauge->field_18 = (f32)(gauge->field_C - gauge->field_10) *
                    intplGetValue(2, gauge->field_4, gauge->field_6, 0.0f, 1.0f) /
                    (f32)unit->maxHp + gauge->field_14;
                break;
            default:
                gauge->field_18 = 0.0f;
                maxHp = unit->maxHp;
                gauge->field_14 = (f32)unit->currentHp / (f32)maxHp +
                    (f32)(gauge->field_C - gauge->field_10) *
                    intplGetValue(0, gauge->field_4, gauge->field_6, 0.0f, 1.0f) /
                    (f32)maxHp;
                break;
            }
        }
    }
    if (gauge->field_8 > 0) {
        gauge->field_8--;
    }
    if ((unit->flags & 1) && gauge->field_8 <= 0 && (gauge->field_2 & 1)) {
        unit->flags &= ~1;
        gauge->field_2 &= ~1;
    }
}

s32 BtlUnit_EnemyItemCanUseCheck(s32 itemId) {
    s32* table = (s32*)_enemy_can_use_item_table;

    if (itemId >= 0xF0 && itemId < 0x153) {
        return itemId;
    }
    while (*table) {
        if (*table == itemId) {
            return itemId;
        }
        table++;
    }
    return 0;
}

BOOL BtlUnit_CheckShadowGuard(BattleUnit* unit) {
    if (unit->mAttributeFlags & kVeiled) {
        return TRUE;
    }
    if (unit->currentKind == UNIT_VIVIAN && unit->work[4] > 0) {
        return TRUE;
    }
    return FALSE;
}

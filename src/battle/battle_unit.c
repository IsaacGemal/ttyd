#include "battle/battle_unit.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "system.h"
#include <string.h>
#include "pmario_sound.h"
#include "battle/battle_audience.h"
#include "battle/battle_actrecord.h"
#include "math_ext.h"

// LOCAL_PROTO(psndSFX_get_vol): retail battle callers consume a full-width result; sharing the u8 implementation signature adds a mask and regresses BtlUnit_snd_se_pos.
extern s32 psndSFX_get_vol(s32 id);

// LOCAL_PROTO(psndSFX_pit): retail callers disagree on pitch width and signedness; the shared u16 signature regresses matched sound commands.
extern void psndSFX_pit(s32 id, s32 pitch);

// LOCAL_PROTO(psndSFX_vol): retail units disagree on volume width; a shared u8 prototype adds narrowing instructions in battle callers.
extern void psndSFX_vol(s32 id, s32 vol);


extern BattleWork* _battleWorkPointer;
extern u32 _enemy_can_use_item_table[];

const f32 float_1_8041c3c0 = 1.0f;
f32 float_1p1_8041c3cc = 1.1f;
f32 float_1p3_8041c3d0 = 1.3f;
f32 float_1p5_8041c3d4 = 1.5f;
f32 float_0p5_8041c3d8 = 0.5f;

// .rodata integer-conversion / pow constants named by the original pool.
const f64 double_to_int_802efab8 = 4503601774854144.0;
const f64 double_2_802efad0 = 2.0;


//local prototypes

// Local field overlays for regions the shared header leaves unnamed.
typedef struct BattleUnitAnimFields {
    u8 pad_0[0x2D0];
    s32 mBodyPartsId;    // 0x2D0
    char* mTalkPoseName; // 0x2D4
    char* mStayPoseName; // 0x2D8
} BattleUnitAnimFields;

typedef struct BattleUnitPartFields {
    u8 pad_0[0x204];
    u32 mFlags; // 0x204
    u8 pad_1[0x4D0 - 0x208];
    void* mPoseSoundTable; // 0x4D0
    s32 mPoseSoundFrame;   // 0x4D4
    f32 mPoseSoundPrev;    // 0x4D8
    f32 mPoseSoundCur;     // 0x4DC
    s32 mPoseSoundCount;   // 0x4E0
    u8 mPoseSoundFlag;     // 0x4E4
    u8 pad_2[3];
    s32 mPoseSoundEventId; // 0x4E8
    BattleUnit* mOwner;    // 0x4EC
    GXColor color;         // 0x4F0
    u8 pad_3[0x500 - 0x4F4];
} BattleUnitPartFields;

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

void BtlUnit_SetTotalHitDamage(BattleUnit* unit, s16 damage) {
    unit->totalHpDamageTaken = damage;
}

void BtlUnit_PoseSoundInit(BattleUnitPart* part) {
    BattleUnitPartFields* fields = (BattleUnitPartFields*)part;
    fields->mPoseSoundFrame = 0;
    fields->mPoseSoundPrev = 0.0f;
    fields->mPoseSoundCur = 0.0f;
}

BOOL BtlUnit_CheckShadowGuard(BattleUnit* unit) {
    if (unit->mAttributeFlags & kVeiled) {
        return TRUE;
    }
    if (unit->currentKind == 0xE4 && unit->work[4] > 0) {
        return TRUE;
    }
    return FALSE;
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

BattleUnit* BtlUnit_GetGuardKouraPtr(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;
    s32 id = unit->unitId;
    BattleUnit* other = NULL;

    for (i = 0; i < 64; i++) {
        other = BattleGetUnitPtr(wp, i);
        if (other && other->currentKind == 0xDF && other->work[0] == id) {
            break;
        }
    }
    if (i >= 64) {
        return NULL;
    }
    return other;
}

void BtlUnit_CheckPinchStatus(BattleUnit* unit, BOOL a2) {
    if (unit->currentHp <= unit->mKindParams->mPerilHP && unit->currentHp < unit->maxHp) {
        if (!(unit->mStatusFlags & 0x20000000)) {
            unit->mStatusFlags |= 0x20000000;
            if (!a2 && unit->currentKind == 0xDE) {
                BattleAudience_Case_MarioDanger();
            }
        }
    } else if (unit->mStatusFlags & 0x20000000) {
        unit->mStatusFlags &= ~0x20000000;
    }

    if (unit->currentHp <= unit->mKindParams->mDangerHP && unit->currentHp < unit->maxHp) {
        if (!(unit->mStatusFlags & 0x10000000)) {
            unit->mStatusFlags |= 0x10000000;
            if (!(unit->mStatusFlags & 0x20000000) && !a2 && unit->currentKind == 0xDE) {
                BattleAudience_Case_MarioPinch();
            }
        }
    } else if (unit->mStatusFlags & 0x10000000) {
        unit->mStatusFlags &= ~0x10000000;
    }
}

void BtlUnit_ReviseHpFp(BattleUnit* unit) {
    PouchData* pouch = pouchGetPtr();

    if (unit->currentKind == 0xDE) {
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

// NONMATCHING (97.50%): gauge type now matches (`lhz` + `cmpwi`, field_0 is u16); the then/else block
// layout still differs (target `beq then; b else`, ours `bne else`).
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
    if ((s32)gauge->field_0 == 1) {
        gauge->field_6 = 0x28;
        gauge->field_A = gauge->field_6 + 0x1E;
    } else {
        gauge->field_6 = 0x14;
        gauge->field_A = gauge->field_6 + 0x1E;
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

BOOL BtlUnit_CanGuardStatus(BattleUnit* unit) {
    return BtlUnit_CanActStatus(unit);
}

void BtlUnit_SetAnim(BattleUnitPart* part, const char* animName) {
    btlDispChangeAnime(part, animName, FALSE);
}

s32 BtlUnit_GetACPossibility(BattleUnit* unit) {
    if (unit->mAlliance != 0) {
        return 0;
    }
    if (unit->currentKind == 0xDF) {
        return 0;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_SLEEP) || BtlUnit_CheckStatus(unit, STATUS_STOP) ||
        BtlUnit_CheckStatus(unit, STATUS_FREEZE) || BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
        return 0;
    }
    return !BtlUnit_CheckStatusFlag(unit, 1);
}

// Keep BtlUnit_SetAnimType out of line: the original calls it rather than inlining.
#pragma dont_inline on
// NONMATCHING (96.56%): tail `type < 0x1F && type >= 0x1D` compiles to `bge body; b skip`, mine `blt skip`.
void BtlUnit_SetAnimType(BattleUnitPart* part, s32 type) {
    const char* animName = BtlUnit_GetPoseNameFromType(part, type);

    if (type >= 0x1D && type < 0x28) {
        btlDispChangeAnime(part, animName, TRUE);
    } else {
        btlDispChangeAnime(part, animName, FALSE);
    }
    if (type < 0x1F) {
        if (type >= 0x1D) {
            ((BattleUnitPartFields*)part)->mFlags |= 0x10;
        }
    }
}
#pragma dont_inline reset

void BtlUnit_SetBodyAnim(BattleUnit* unit, const char* animName) {
    s32 bodyId = BtlUnit_GetBodyPartsId(unit);

    btlDispChangeAnime(BtlUnit_GetPartsPtr(unit, bodyId), animName, FALSE);
}

void BtlUnit_SetBodyAnimType(BattleUnit* unit, s32 type) {
    s32 bodyId = BtlUnit_GetBodyPartsId(unit);

    BtlUnit_SetAnimType(BtlUnit_GetPartsPtr(unit, bodyId), type);
}

void BtlUnit_ChangeStayAnim(s32 unitId) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, unitId);

    btlDispChangeAnime(BtlUnit_GetPartsPtr(unit, ((BattleUnitAnimFields*)unit)->mBodyPartsId),
                       ((BattleUnitAnimFields*)unit)->mStayPoseName, FALSE);
}

void BtlUnit_ChangeTalkAnim(s32 unitId) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, unitId);

    btlDispChangeAnime(BtlUnit_GetPartsPtr(unit, ((BattleUnitAnimFields*)unit)->mBodyPartsId),
                       ((BattleUnitAnimFields*)unit)->mTalkPoseName, FALSE);
}

BOOL BtlUnit_Init(void) {
    int i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleSetUnitPtr(wp, i, NULL);
    }
    return TRUE;
}

// NONMATCHING (11.91%): 376 missing, 1 extra, plus register differences.
BattleUnit* BtlUnit_Entry(BattleUnitSetup* setup) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    int i;

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





    //TODO: finish
    return NULL;
}

// NONMATCHING (1.19%): 1 instruction differs, 116 missing.
BOOL BtlUnit_Delete(BattleUnit* unit) {
    return FALSE;
}

// NONMATCHING (2.41%): 1 instruction differs, 56 missing.
BattleUnit* BtlUnit_Spawn(BattleUnitSetup* setup, s32 flags) {
    return NULL;
}

s32 BtlUnit_GetUnitId(BattleUnit* unit) {
    return unit->unitId;
}

// Keep BtlUnit_GetPartsPtr, BtlUnit_GetBodyPartsId out of line: the original calls them rather than inlining.
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

void BtlUnit_SetPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mPosition.x = x;
    unit->mPosition.y = y;
    unit->mPosition.z = z;
}

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

    if (part->attributes & 0x10000000) {
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

void BtlUnit_SetHomePos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mHomePosition.x = x;
    unit->mHomePosition.y = y;
    unit->mHomePosition.z = z;
}

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

void BtlUnit_SetPartsRotate(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mRotation.x = x;
    part->mRotation.y = y;
    part->mRotation.z = z;
}

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

void BtlUnit_SetBaseRotate(BattleUnit* unit, f32 x, f32 y, f32 z) {
    unit->mBaseRotation.x = reviseAngle(x);
    unit->mBaseRotation.y = reviseAngle(y);
    unit->mBaseRotation.z = reviseAngle(z);
}

void BtlUnit_GetBaseRotate(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mBaseRotation.x;
    *y = unit->mBaseRotation.y;
    *z = unit->mBaseRotation.z;
}

void BtlUnit_SetPartsBaseRotate(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mBaseRotation.x = x;
    part->mBaseRotation.y = y;
    part->mBaseRotation.z = z;
}

void BtlUnit_GetPartsBaseRotate(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    *x = part->mBaseRotation.x;
    *y = part->mBaseRotation.y;
    *z = part->mBaseRotation.z;
}

void BtlUnit_SetRotateOffset(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mRotationOffset.x = x;
    }
    if (thefuckyone != y) {
        unit->mRotationOffset.y = y;
    }
    if (thefuckyone != z) {
        unit->mRotationOffset.z = z;
    }
}

void BtlUnit_SetPartsRotateOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mRotationOffset.x = x;
    }
    if (thefuckyone != y) {
        part->mRotationOffset.y = y;
    }
    if (thefuckyone != z) {
        part->mRotationOffset.z = z;
    }
}

void BtlUnit_AddPartsRotateOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mRotationOffset.x += x;
    part->mRotationOffset.y += y;
    part->mRotationOffset.z += z;
}

void BtlUnit_SetBaseScale(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mBaseScale.x = x;
    }
    if (thefuckyone != y) {
        unit->mBaseScale.y = y;
    }
    if (thefuckyone != z) {
        unit->mBaseScale.z = z;
    }
}

void BtlUnit_GetScale(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mScale.x;
    *y = unit->mScale.y;
    *z = unit->mScale.z;
}

void BtlUnit_SetScale(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mScale.x = x;
    }
    if (thefuckyone != y) {
        unit->mScale.y = y;
    }
    if (thefuckyone != z) {
        unit->mScale.z = z;
    }
}

void BtlUnit_AddScale(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mScale.x += x;
    }
    if (thefuckyone != y) {
        unit->mScale.y += y;
    }
    if (thefuckyone != z) {
        unit->mScale.z += z;
    }
}

void BtlUnit_SetPartsBaseScale(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mBaseScale.x = x;
    }
    if (thefuckyone != y) {
        part->mBaseScale.y = y;
    }
    if (thefuckyone != z) {
        part->mBaseScale.z = z;
    }
}

void BtlUnit_SetPartsScale(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mScale.x = x;
    }
    if (thefuckyone != y) {
        part->mScale.y = y;
    }
    if (thefuckyone != z) {
        part->mScale.z = z;
    }
}

void BtlUnit_AddPartsScale(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mScale.x += x;
    }
    if (thefuckyone != y) {
        part->mScale.y += y;
    }
    if (thefuckyone != z) {
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

void BtlUnit_SetOffsetPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mPositionOffset.x = x;
    }
    if (thefuckyone != y) {
        unit->mPositionOffset.y = y;
    }
    if (thefuckyone != z) {
        unit->mPositionOffset.z = z;
    }
}

// Keep BtlUnit_GetPartsOffsetPos out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BtlUnit_GetPartsOffsetPos(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    *x = part->mPositionOffset.x;
    *y = part->mPositionOffset.y;
    *z = part->mPositionOffset.z;
}
#pragma dont_inline reset

void BtlUnit_SetPartsOffsetPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mPositionOffset.x = x;
    }
    if (thefuckyone != y) {
        part->mPositionOffset.y = y;
    }
    if (thefuckyone != z) {
        part->mPositionOffset.z = z;
    }
}

void BtlUnit_AddPartsOffsetPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    part->mPositionOffset.x += x;
    part->mPositionOffset.y += y;
    part->mPositionOffset.z += z;
}

void BtlUnit_SetDispOffset(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mDisplayOffset.x = x;
    }
    if (thefuckyone != y) {
        unit->mDisplayOffset.y = y;
    }
    if (thefuckyone != z) {
        unit->mDisplayOffset.z = z;
    }
}

void BtlUnit_SetPartsDispOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mDisplayOffset.x = x;
    }
    if (thefuckyone != y) {
        part->mDisplayOffset.y = y;
    }
    if (thefuckyone != z) {
        part->mDisplayOffset.z = z;
    }
}

void BtlUnit_AddPartsDispOffset(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mDisplayOffset.x += x;
    }
    if (thefuckyone != y) {
        part->mDisplayOffset.y += y;
    }
    if (thefuckyone != z) {
        part->mDisplayOffset.z += z;
    }
}

void BtlUnit_SetMoveStartPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mMoveStartPos.x = x;
    }
    if (thefuckyone != y) {
        unit->mMoveStartPos.y = y;
    }
    if (thefuckyone != z) {
        unit->mMoveStartPos.z = z;
    }
}

void BtlUnit_SetPartsMoveStartPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mMoveStartPos.x = x;
    }
    if (thefuckyone != y) {
        part->mMoveStartPos.y = y;
    }
    if (thefuckyone != z) {
        part->mMoveStartPos.z = z;
    }
}

void BtlUnit_SetMoveCurrentPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mMoveCurrentPos.x = x;
    }
    if (thefuckyone != y) {
        unit->mMoveCurrentPos.y = y;
    }
    if (thefuckyone != z) {
        unit->mMoveCurrentPos.z = z;
    }
}

void BtlUnit_SetPartsMoveCurrentPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mMoveCurrentPos.x = x;
    }
    if (thefuckyone != y) {
        part->mMoveCurrentPos.y = y;
    }
    if (thefuckyone != z) {
        part->mMoveCurrentPos.z = z;
    }
}

void BtlUnit_SetMoveTargetPos(BattleUnit* unit, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        unit->mMoveTargetPos.x = x;
    }
    if (thefuckyone != y) {
        unit->mMoveTargetPos.y = y;
    }
    if (thefuckyone != z) {
        unit->mMoveTargetPos.z = z;
    }
}

void BtlUnit_SetPartsMoveTargetPos(BattleUnitPart* part, f32 x, f32 y, f32 z) {
    f32 thefuckyone = -2.0468902587890625f;
    if (thefuckyone != x) {
        part->mMoveTargetPos.x = x;
    }
    if (thefuckyone != y) {
        part->mMoveTargetPos.y = y;
    }
    if (thefuckyone != z) {
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

// NONMATCHING (82.06%): HUGE/TINY and DEFENSE_UP/DOWN branch trees differ; target also `extsb`s type before the switch.
BOOL BtlUnit_SetStatus(BattleUnit* unit, StatusEffectType type, s8 turns, s8 strength) {
    switch (type) {
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
                if (type == STATUS_HUGE) {
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
            if (strength <= 0) {
                if (!strength) {
                    if (type == STATUS_DEFENSE_DOWN) {
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
            else {
                if (unit->mDefChangeStrength < 0) {
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
            if (turns <= 0) {
                unit->mAllergicTurns = 0;
                return FALSE;
            }
            unit->mAllergicTurns = turns;
            break;
        case STATUS_FLIPPED:
            unit->flippedTurns = turns;
            break;
        case STATUS_INVISIBLE:
            unit->mInvisibleTurns = turns;
            break;
        case STATUS_PAYBACK:
            if (turns <= 0) {
                unit->mPaybackTurns = 0;
                return FALSE;
            }
            unit->mPaybackTurns = turns;
            break;
        case STATUS_HOLD_FAST:
            unit->mHoldFastTurns = turns;
            break;
        case STATUS_FAST:
            if (unit->mSlowTurns <= 0) {
                unit->mFastTurns = turns;
            }
            else {
                if (turns > 0) {
                    unit->mSlowTurns = 0;
                    _CheckMoveCount(unit);
                    return FALSE;
                }
                unit->mFastTurns = turns;
            }
            _CheckMoveCount(unit);
            break;
        case STATUS_SLOW:
            if (unit->mFastTurns <= 0) {
                unit->mSlowTurns = turns;
            }
            else {
                if (turns > 0) {
                    unit->mFastTurns = 0;
                    _CheckMoveCount(unit);
                    return FALSE;
                }
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
void BtlUnit_ClearStatus(BattleUnit* unit) {
    memset(&unit->mSleepTurns, 0, 0x1Eu);
    BtlUnit_SetStatus(unit, STATUS_CHARGE, 1, -99);
    BattleStatusChangeInfoWorkInit(unit);
}

// NONMATCHING (75.80%): case bodies/branches in the recovery switch still differ (HP/FP regen, defense).
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
    switch (type) {
        case STATUS_SLEEP:
            if (turns <= 1) {
                turns = 0;
            }
            else {
                turns--;
            }
            unit->mSleepTurns = turns;
            return turns == 0;
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
            if (turns < 0) {
                turns++;
            }
            else {
                turns--;
            }
            if (strength < 0) {
                strength++;
            }
            else {
                strength--;
            }
            if (!turns) {
                strength = 0;
            }
            BtlUnit_SetStatus(unit, type, turns, strength);
            return turns == 0;
        case STATUS_HUGE:
        case STATUS_TINY:
            if (turns <= 0) {
                if (turns >= 0) {
                    return FALSE;
                }
                turns++;
            }
            else {
                turns--;
            }
            if (!turns) {
                strength = 0;
            }
            unit->mSizeChangeTurns = turns;
            unit->mSizeChangeStrength = strength;
            return turns == 0;
        case STATUS_ATTACK_UP:
        case STATUS_ATTACK_DOWN:
            if (turns <= 0) {
                if (turns >= 0) {
                    return FALSE;
                }
                turns++;
            }
            else {
                turns--;
            }
            if (!turns) {
                strength = 0;
            }
            unit->mAtkChangeTurns = turns;
            unit->mAtkChangeStrength = strength;
            return turns == 0;
        case STATUS_POISON:
        case STATUS_DEFENSE_UP:
        case STATUS_DEFENSE_DOWN:
        case STATUS_HP_REGEN:
        case STATUS_FP_REGEN:
            if (turns < 0) {
                turns++;
            }
            else {
                turns--;
            }
            if (!turns) {
                strength = 0;
            }
            BtlUnit_SetStatus(unit, type, turns, strength);
            return turns == 0;
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
// NONMATCHING (87.19%): target uses `beq found` for the second poseId compare, mine `bne skip`, plus one dead `b`.
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
        if ((s32)pose->poseId == type || (s32)pose->poseId == bound) {
            return (char*)pose->poseName;
        }
        pose++;
    }
    return NULL;
}
#pragma dont_inline reset












// NONMATCHING (1.89%): 52 missing.
void BtlUnit_SetParamToPouch(BattleUnit* unit) {

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




// Keep BtlUnit_GetFp, BtlUnit_SetFp out of line: the original calls them rather than inlining.
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

s32 BtlUnit_GetHitDamage(BattleUnit* unit) {
    return unit->hpDamageTaken;
}

s32 BtlUnit_GetTotalHitDamage(BattleUnit* unit) {
    return unit->totalHpDamageTaken;
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

typedef struct BtlUnitSeMode {
    s32 field_0x0;  // 0x0
    s32 field_0x4;  // 0x4
    s16 field_0x8;  // 0x8
    s16 field_0xA;  // 0xA
    s16 field_0xC;  // 0xC
    s16 field_0xE;  // 0xE
    s16 field_0x10; // 0x10
    s16 field_0x12; // 0x12
    s16 field_0x14; // 0x14
    s32 field_0x18; // 0x18
} BtlUnitSeMode;

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
        ((BattleUnitPartFields*)part)->mFlags |= 0x80;
    } else {
        btlDispPoseAnime(part);
    }
}

void BtlUnit_SetSeMode(BtlUnitSeMode* se, s32 a, s32 b, s16 c, s16 d, s16 e, s16 f, s16 g) {
    se->field_0x0 = a;
    se->field_0x4 = b;
    se->field_0xC = c;
    se->field_0xE = d;
    se->field_0x10 = d;
    se->field_0x14 = 0;
    se->field_0x8 = f;
    se->field_0xA = g;
    se->field_0x18 = -1;
}

s32 BtlUnit_snd_se(BattleUnit* unit, const char* name, s32 b, s16 c) {
    Vec pos;

    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    return BtlUnit_snd_se_pos(unit, name, b, c, &pos);
}


// NONMATCHING (87.75%): register allocation is rotated (target keeps name in r27/val in r30, ours name in r30/val in r29).
#pragma dont_inline on
s32 BtlUnit_snd_se_pos(BattleUnit* unit, const char* name, s32 b, s16 c, Vec* pos) {
    s32 handle;
    s32 val = 0;
    s8 turns, strength;

    if (BtlUnit_CheckStatus(unit, STATUS_TINY)) {
        BtlUnit_GetStatus(unit, STATUS_TINY, &turns, &strength);
        val = (s16)(((s8)strength * 2 - 1) * -341);
    }
    if (BtlUnit_CheckStatus(unit, STATUS_HUGE)) {
        BtlUnit_GetStatus(unit, STATUS_HUGE, &turns, &strength);
        val = (s16)(((s8)strength * 2 + 1) * -341);
    }
    val = (s16)((c + val) + (irand(0x29) - 0x14));
    handle = psndSFXOn_3D(name, pos);
    psndSFX_pit(handle, val);
    if ((u32)(b - 0xF1190000) < 0x4D80) {
        psndSFX_vol(handle, psndSFX_get_vol(handle));
    } else {
        psndSFX_vol(handle, (u8)b);
    }
    return handle;
}
#pragma dont_inline reset

// NONMATCHING (84.68%): switch compare tree roots at 3 instead of 4; info pointer kept in a callee-save
// register instead of a temp; double_to_int constant uses a local symbol. Semantics are identical.
s32 BtlUnit_GetExp(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;
    PouchData* pouch = pouchGetPtr();
    f32 mult;

    if (pouch->level > unit->mLevel) {
        return 0;
    }
    switch (info->setup->groupSetup->numEnemies) {
        case 0:
        case 1:
        case 2:
            mult = float_1_8041c3c0;
            break;
        case 3:
            mult = float_1p1_8041c3cc;
            break;
        case 4:
            mult = float_1p3_8041c3d0;
            break;
        default:
            mult = float_1p5_8041c3d4;
            break;
    }
    return (s32)(mult * (f32)(unit->mLevel - pouch->level) * float_0p5_8041c3d8) + unit->mKindParams->mBonusExp;
}

typedef struct BattleWeaponFields {
    u8 pad_0x0[0x8];
    s32 itemId;    // 0x8
    u8 pad_0xC[0x11 - 0xC];
    u8 field_0x11; // 0x11
    u8 field_0x12; // 0x12
} BattleWeaponFields;


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
    (*(void(*)(s32))pouchAddAP)(-((BattleWeaponFields*)weapon)->field_0x12 * 100);
#else
    pouchAddAP(-((BattleWeaponFields*)weapon)->field_0x12 * 100);
#endif
}

BOOL BtlUnit_CheckWeaponCost(BattleUnit* unit, BattleWeapon* weapon) {
    s32 fp = BtlUnit_GetFp(unit);
    s32 cost = BtlUnit_GetWeaponCost(unit, weapon);

    if (fp < cost) {
        return FALSE;
    }
#ifdef MUST_MATCH
    /* Match-only: the header declares pouchGetAP as `s16`, but the target consumed the raw 32-bit
       return (no extsh). Call it through an s32-returning prototype, as evt_kinopio.c does. */
    return (*(s32(*)(void))pouchGetAP)() >= (s32)((BattleWeaponFields*)weapon)->field_0x12 * 100;
#else
    return pouchGetAP() >= (s32)((BattleWeaponFields*)weapon)->field_0x12 * 100;
#endif
}

// Keep BtlUnit_GetWeaponCost out of line: the original calls it rather than inlining its switch.
#pragma dont_inline on
s32 BtlUnit_GetWeaponCost(BattleUnit* unit, BattleWeapon* weapon) {
    s32 cost = ((BattleWeaponFields*)weapon)->field_0x11;
    s32 badgeCount;

    if (((BattleWeaponFields*)weapon)->itemId != 0) {
        badgeCount = pouchEquipCheckBadge(((BattleWeaponFields*)weapon)->itemId);
        switch (((BattleWeaponFields*)weapon)->itemId) {
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
        cost -= ((u8*)unit)[0x2F1];
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

#include "battle/battle_unit.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "system.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;
extern u32 _enemy_can_use_item_table[];

//local prototypes
void _CheckMoveCount(BattleUnit* unit);
BOOL BtlUnit_CanActStatus(BattleUnit* unit);
void BtlUnit_SetAnim(BattleUnitPart* part, const char* animName);
void BtlUnit_SetAnimType(BattleUnitPart* part, s32 type);
void BtlUnit_ChangeStayAnim(s32 unitId);
void BtlUnit_ChangeTalkAnim(s32 unitId);
void BtlUnit_SetBodyAnim(BattleUnit* unit, const char* animName);
void BtlUnit_SetBodyAnimType(BattleUnit* unit, s32 type);
void BtlUnit_PoseSoundInit(BattleUnitPart* part);
s32 BtlUnit_EnemyItemCanUseCheck(s32 itemId);
BOOL BtlUnit_CheckShadowGuard(BattleUnit* unit);
void btlDispChangeAnime(BattleUnitPart* part, const char* animName, BOOL reset);
void BattleAudience_Case_MarioDanger(void);
void BattleAudience_Case_MarioPinch(void);
BattleUnit* BtlUnit_GetGuardKouraPtr(BattleUnit* unit);
void BtlUnit_CheckPinchStatus(BattleUnit* unit, BOOL a2);
void BtlUnit_HpGaugeInit(BattleUnit* unit);

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

typedef struct BattleUnitKindCoinFields {
    u8 pad_0[0x10];
    u8 field_0x10; // 0x10
    u8 field_0x11; // 0x11
    u8 field_0x12; // 0x12
} BattleUnitKindCoinFields;

typedef struct HpGaugeLocal {
    s16 field_0;  // 0x0
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
    coins = ((BattleUnitKindCoinFields*)unit->mKindParams)->field_0x12;
    for (i = 0; i < ((BattleUnitKindCoinFields*)unit->mKindParams)->field_0x10; i++) {
        if (irand(100) < ((BattleUnitKindCoinFields*)unit->mKindParams)->field_0x11) {
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
    if (gauge->field_0 == 1) {
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

BOOL BtlUnit_CanActStatus(BattleUnit* unit) {
    if (BtlUnit_CheckStatus(unit, STATUS_SLEEP)) {
        return FALSE;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_STOP)) {
        return FALSE;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_DIZZY)) {
        return FALSE;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
        return FALSE;
    }
    return !BtlUnit_CheckStatusFlag(unit, 1);
}

BOOL BtlUnit_CanGuardStatus(BattleUnit* unit) {
    return BtlUnit_CanActStatus(unit);
}

void BtlUnit_SetAnim(BattleUnitPart* part, const char* animName) {
    btlDispChangeAnime(part, animName, FALSE);
}

void BtlUnit_SetAnimType(BattleUnitPart* part, s32 type) {
    const char* animName = BtlUnit_GetPoseNameFromType(part, type);

    if (type >= 0x1D && type < 0x28) {
        btlDispChangeAnime(part, animName, TRUE);
    } else {
        btlDispChangeAnime(part, animName, FALSE);
    }
    if (type >= 0x1D && type < 0x1F) {
        ((BattleUnitPartFields*)part)->mFlags |= 0x10;
    }
}

void BtlUnit_SetBodyAnim(BattleUnit* unit, const char* animName) {
    btlDispChangeAnime(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), animName, FALSE);
}

void BtlUnit_SetBodyAnimType(BattleUnit* unit, s32 type) {
    BtlUnit_SetAnimType(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), type);
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

BOOL BtlUnit_Init(void) { //TODO: force stmw/lmw, otherwise 1:1
    int i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleSetUnitPtr(wp, i, NULL);
    }
    return TRUE;
}

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

BOOL BtlUnit_Delete(BattleUnit* unit) {
    return FALSE;
}

BattleUnit* BtlUnit_Spawn(BattleUnitSetup* setup, s32 flags) {
    return NULL;
}

s32 BtlUnit_GetUnitId(BattleUnit* unit) {
    return unit->unitId;
}

BattleUnitPart* BtlUnit_GetPartsPtr(BattleUnit* unit, s32 partNum) {
    BattleUnitPart* part;

    part = unit->mParts;
    if (partNum >= 0) {
        if (part->mNextPart) {
            while (part && part->mKindPartParams->partNum != partNum) {
                part = part->mNextPart;
            }
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

void BtlUnit_GetPos(BattleUnit* unit, f32* x, f32* y, f32* z) {
    *x = unit->mPosition.x;
    *y = unit->mPosition.y;
    *z = unit->mPosition.z;
}

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

void BtlUnit_GetPartsWorldPos(BattleUnitPart* part, f32* x, f32* y, f32* z) {

}

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

s16 BtlUnit_GetWidth(BattleUnit* unit) {
    return unit->mWidth;
}

s16 BtlUnit_GetHeight(BattleUnit* unit) {
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

void BtlUnit_GetPartsOffsetPos(BattleUnitPart* part, f32* x, f32* y, f32* z) {
    *x = part->mPositionOffset.x;
    *y = part->mPositionOffset.y;
    *z = part->mPositionOffset.z;
}

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

void _CheckMoveCount(BattleUnit* unit) {
    s8 maxmoves;

    if (!(unit->mTokenFlags & kCurrentlyActing)) {
        maxmoves = unit->maxMovesThisTurn;
        if (maxmoves > 0 && maxmoves == unit->movesRemaining) {
            BtlUnit_ResetMoveStatus(unit);
        }
    }
}

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

s32 BtlUnit_CheckStatusFlag(BattleUnit* unit, s32 flags) {
    if (unit) {
        return unit->mStatusFlags & flags;
    }
    return 0;
}

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












void BtlUnit_SetParamToPouch(BattleUnit* unit) {

}









void BtlUnit_ResetMoveStatus(BattleUnit* unit) {

}




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

s32 BtlUnit_GetMaxFp(BattleUnit* unit) {
    BattleUnitType kind = unit->currentKind;
    BattleWork* wp = _battleWorkPointer;
    s32 result = unit->maxFp;

    if (kind >= TYPE_PARTNER_MIN && kind < TYPE_PARTNER_MAX) {
        result = BattleGetMarioPtr(wp)->maxFp;
    }
    return result;
}

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

    if (kind < TYPE_PARTNER_MIN || kind >= TYPE_PARTNER_MAX) {
        max = BtlUnit_GetMaxFp(unit);
        fp = &unit->currentFp;
    }
    else {
        max = BtlUnit_GetMaxFp(BattleGetMarioPtr(_battleWorkPointer));
        fp = &BattleGetMarioPtr(wp)->currentFp;
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

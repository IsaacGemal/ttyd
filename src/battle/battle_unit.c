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
extern u32 _enemy_can_use_item_table[];

const f32 float_0_8041c3bc = 0.0f;
const f32 float_1_8041c3c0 = 1.0f;
const f32 float_60_8041c3c4 = 60.0f;
const f32 float_neg1_8041c3c8 = -1.0f;
f32 float_1p1_8041c3cc = 1.1f;
f32 float_1p3_8041c3d0 = 1.3f;
f32 float_1p5_8041c3d4 = 1.5f;
f32 float_0p5_8041c3d8 = 0.5f;

// .rodata integer-conversion / pow constants named by the original pool.
const f64 double_to_int_802efab8 = 4503601774854144.0;
// Defined where the original .rodata places it; declared extern so MWCC does not fold 0.7.
extern const f64 double_0p7_802efac0;
const f64 double_to_int_mask_802efac8 = 4503599627370496.0;
const f64 double_2_802efad0 = 2.0;

// 30-byte status block copied between BattleWorkPartyInfo::field_4 and BattleUnit::mSleepTurns.
#ifdef MUST_MATCH
// For the struct-copy below: MWCC emits the original's lwzu/stwu copy loop for a struct
// assignment, but a plain memcpy of the 30-byte blob compiles to a call.
typedef struct UnitStatusBlock {
    s16 v[15]; // 0x0
} UnitStatusBlock;
#endif


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

// `opt_propagation off` is required for the volume reduction `(s32)(0x7F * 0.7)`: with propagation on
// GC/2.6 folds the u32 0x7F into a pooled 127.0 double instead of the retail runtime conversion
// (`lis 0x4330` + `double_to_int_mask_802efac8`, `fsub`, `fmul`). The `s32 v2` temp pins the
// conversion before the psndSFX_vol call. `-1.0f` is a literal so the load schedules as retail does.


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

// Keep BtlUnit_ReviseHpFp out of line: the original calls it rather than inlining.
#pragma dont_inline on
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
#pragma dont_inline reset

// The gauge type is compared with `lhz`/`cmpwi`; a `switch` (not an if/else) reproduces the
// original's `beq then; b else` dispatch.
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
                    intplGetValue(2, gauge->field_4, gauge->field_6, float_0_8041c3bc, float_1_8041c3c0) /
                    (f32)unit->maxHp + gauge->field_14;
                break;
            default:
                gauge->field_18 = float_0_8041c3bc;
                maxHp = unit->maxHp;
                gauge->field_14 = (f32)unit->currentHp / (f32)maxHp +
                    (f32)(gauge->field_C - gauge->field_10) *
                    intplGetValue(0, gauge->field_4, gauge->field_6, float_0_8041c3bc, float_1_8041c3c0) /
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
        ((BattleUnitPartFields*)part)->mFlags |= 0x10;
        break;
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

// BtlUnit_Entry is emitted out of line in retail; the small placeholder body otherwise
// inlines into BtlUnit_Spawn (the real function does not), so pin it.
#pragma dont_inline on
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
        if (*(s32*)((u8*)wp + 0x120 + j * 4) == id) {
            *(s32*)((u8*)wp + 0x120 + j * 4) = -1;
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
    // Cleanup hooks the header does not name: +0x310 buffer, +0x314 its destructor, +0x318 smart pointer.
    if (*(void**)((u8*)unit + 0x310)) {
        if (*(void**)((u8*)unit + 0x314)) {
            ((void (*)(BattleUnit*)) * (void**)((u8*)unit + 0x314))(unit);
        } else {
            BattleFree(*(void**)((u8*)unit + 0x310));
        }
    }
    if (*(void**)((u8*)unit + 0x318)) {
        smartFree(*(smartEntry**)((u8*)unit + 0x318));
        *(void**)((u8*)unit + 0x318) = NULL;
    }
    part = unit->mParts;
    for (; part; part = part->mNextPart) {
        if (((BattleUnitPartFields*)part)->mPoseSoundEventId != -1) {
            ((BattleUnitPartFields*)part)->mPoseSoundEventId = -1;
        }
        if (*(s32*)((u8*)part + 0x1C0) != -1) {
            animPoseSetEffect(*(s32*)((u8*)part + 0x1C0), NULL, FALSE);
            animPoseRelease(*(s32*)((u8*)part + 0x1C0));
        }
    }
    BattleFree(unit->mParts);
    BattleFree(unit);
    BattleSetUnitPtr(wp, i, NULL);
    return TRUE;
}

// Logic is byte-identical apart from register allocation. Retail colours unit=r6, id=r5, offset=r3
// (sharing `li r3,0` for offset and moveState and using `mr. r6,r3` after BtlUnit_Entry); GC/2.6
// keeps unit=r6, id=r5, offset=r4 and both `add r3,r31,r3; stw ...,0x120(r3)` break-case stores come
// out as `stwx`. Rearranging declarations/loops does not move the colours.
// NONMATCHING (93.19%): offset register rotated and break-case stores are `stwx` instead of base+0x120.
BattleUnit* BtlUnit_Spawn(BattleUnitSetup* setup, s32 flags) {
    BattleWork* wp = _battleWorkPointer;
    s32 offset;
    s32 id;
    BattleUnit* unit;

    if (!(unit = BtlUnit_Entry(setup))) {
        return unit;
    }
    if (flags & 2) {
        unit->moveState = offset = 0;
        id = unit->unitId;
        for (; offset < 0x100; offset += 4) {
            if (id == -1) {
                break;
            }
            if (*(s32*)((u8*)wp + 0x120 + offset) == -1) {
                *(s32*)((u8*)wp + 0x120 + offset) = id;
                break;
            }
            {
                s32 prev = *(s32*)((u8*)wp + 0x120 + offset);
                *(s32*)((u8*)wp + 0x120 + offset) = id;
                id = prev;
            }
        }
    }
    else if (flags & 1) {
        offset = 0xFC;
        unit->moveState = 0;
        id = unit->unitId;
        for (; offset >= 0; offset -= 4) {
            if (id == -1) {
                break;
            }
            if (*(s32*)((u8*)wp + 0x120 + offset) == -1) {
                *(s32*)((u8*)wp + 0x120 + offset) = id;
                break;
            }
            {
                s32 prev = *(s32*)((u8*)wp + 0x120 + offset);
                *(s32*)((u8*)wp + 0x120 + offset) = id;
                id = prev;
            }
        }
    }
    return unit;
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

// Source layout BtlUnit_LoadSeMode copies the four se-mode variants from. The
// variants live at 0x0, 0x8, 0x18 and 0x28; the header leaves the owning regions
// unnamed, so the fields are listed at their offsets.
typedef struct BtlUnitSeModeSrc {
    s32 field_0x0;  // 0x0
    s32 field_0x4;  // 0x4
    s32 field_0x8;  // 0x8
    s32 field_0xC;  // 0xC
    s16 field_0x10; // 0x10
    s16 field_0x12; // 0x12
    s16 field_0x14; // 0x14
    s16 field_0x16; // 0x16
    s32 field_0x18; // 0x18
    s32 field_0x1C; // 0x1C
    s16 field_0x20; // 0x20
    s16 field_0x22; // 0x22
    s16 field_0x24; // 0x24
    s16 field_0x26; // 0x26
    s32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s16 field_0x30; // 0x30
    s16 field_0x32; // 0x32
    s16 field_0x34; // 0x34
} BtlUnitSeModeSrc;

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

// `case 0` shares the default body: the original routes b == 0 through the same
// zero-fill as any other unlisted value, and MWCC emits its fall-through branch.
void BtlUnit_LoadSeMode(s32 a, s32 b, void* c, void* d) {
    BtlUnitSeModeSrc* src = c;
    BtlUnitSeMode* dst = d;

    dst->field_0x14 = 0;
    dst->field_0x18 = -1;
    switch (b) {
        case -1:
            switch (a) {
                case 0:
                    dst->field_0x0 = src->field_0x8;
                    dst->field_0x4 = src->field_0xC;
                    dst->field_0xC = src->field_0x10;
                    dst->field_0xE = src->field_0x12;
                    dst->field_0x10 = src->field_0x14;
                    break;
                case 1:
                    dst->field_0x0 = src->field_0x18;
                    dst->field_0x4 = src->field_0x1C;
                    dst->field_0xC = src->field_0x20;
                    dst->field_0xE = src->field_0x22;
                    dst->field_0x10 = src->field_0x24;
                    break;
                case 2:
                    dst->field_0x0 = src->field_0x0;
                    dst->field_0x4 = src->field_0x4;
                    dst->field_0xC = 0;
                    dst->field_0xE = 0;
                    dst->field_0x10 = 0;
                    break;
                case 3:
                    dst->field_0x0 = src->field_0x28;
                    dst->field_0x4 = src->field_0x2C;
                    dst->field_0xC = src->field_0x30;
                    dst->field_0xE = src->field_0x32;
                    dst->field_0x10 = src->field_0x34;
                    break;
                default:
                    dst->field_0x0 = 0;
                    dst->field_0x4 = 0;
                    dst->field_0xC = 0;
                    dst->field_0xE = 0;
                    dst->field_0x10 = 0;
                    break;
            }
            dst->field_0x12 = dst->field_0xC;
            break;
        case -2:
            dst->field_0x12 = dst->field_0xC;
            break;
        case 0:
        default:
            dst->field_0x0 = 0;
            dst->field_0x4 = 0;
            dst->field_0xC = 0;
            dst->field_0xE = 0;
            dst->field_0x10 = 0;
            dst->field_0x12 = dst->field_0xC;
            break;
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

// A pose-sound table entry. No header declares this type; the layout matches unit_party_yoshi.c's
// PoseSoundEntry and the 0x14-byte entries BtlUnit_ControlPoseSoundMain reads.
typedef struct PoseSoundEntry {
    const char* poseName;                                         // 0x0
    f32 volume;                                                   // 0x4
    s32 (*callback)(BattleUnit*, struct PoseSoundEntry*, void*);  // 0x8
    const char* soundName;                                        // 0xC
    s16 pitch;                                                    // 0x10
    u16 flags;                                                    // 0x12
} PoseSoundEntry;

// The pose-sound block of a BattleUnitPart, viewed with the block's own base (part + 0x4D0).
typedef struct BtlPoseSoundWork {
    void* mPoseSoundTable; // 0x0 (0x4D0)
    s32 mPoseSoundFrame;   // 0x4 (0x4D4)
    f32 mPoseSoundPrev;    // 0x8 (0x4D8)
    f32 mPoseSoundCur;     // 0xC (0x4DC)
    s32 mPoseSoundCount;   // 0x10 (0x4E0)
    u8 mPoseSoundFlag;     // 0x14 (0x4E4)
    u8 pad_0[3];
    s32 mPoseSoundEventId; // 0x18 (0x4E8)
} BtlPoseSoundWork;

// The body is byte-identical except the third `frame < animFrame` re-check: retail emits a fresh
// `cmplw r0,r3; bge`, GC/2.6 reuses the previous compare's CR0. Reversing the operands (below) forces
// a fresh compare but swaps it to `cmplw r3,r0; ble`, which is the closest GC/2.6 gets.
// `opt_propagation off` keeps the u32 0x7F -> double volume conversion that retail emits.
// NONMATCHING (99.89%): the third `<` compare is `cmplw r3,r0; ble` instead of `cmplw r0,r3; bge`.


// Keep BtlUnit_snd_se out of line: BtlUnit_ControlPoseSoundMain's original calls it rather than inlining it.
#pragma dont_inline on
s32 BtlUnit_snd_se(BattleUnit* unit, const char* name, s32 b, s16 c) {
    Vec pos;

    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    return BtlUnit_snd_se_pos(unit, name, b, c, &pos);
}
#pragma dont_inline reset


// NONMATCHING (97.03%): `val = (s16)(c + val); val = (s16)(val + (irand(0x29)-0x14));` matches the target's
// `(c+val)` association; the remaining diff is a one-register rotation (retail name=r27/val=r30, ours name=r29/val=r28).
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
    val = (s16)(c + val);
    val = (s16)(val + (irand(0x29) - 0x14));
    handle = psndSFXOn_3D(name, pos);
    psndSFX_pit(handle, val);
    psndSFX_vol(handle, (u32)(b - 0xF1190000) == 0x4D80 ? psndSFX_get_vol(handle) : (u8)b);
    return handle;
}
#pragma dont_inline reset

// `case 5:` is grouped with `default:` on purpose: it seeds MWCC's switch decision tree at 4 like retail
// (without it the tree roots at 3 and needs an extra `cmpwi 5`). The `prod` temp pins the multiply order.
s32 BtlUnit_GetExp(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    PouchData* pouch = pouchGetPtr();
    FieldBattleInfo* info = wp->fbatInfo;
    f32 mult;
    f32 prod;
    s32 exp;

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
        case 5:
        default:
            mult = float_1p5_8041c3d4;
            break;
    }
    prod = mult * (f32)(unit->mLevel - pouch->level);
    exp = (s32)(prod * float_0p5_8041c3d8);
    return exp + unit->mKindParams->mBonusExp;
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

const f64 double_0p7_802efac0 = 0.7;

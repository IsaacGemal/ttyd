#include "battle/battle_detect.h"

#include "data/item_data.h"

#include "battle/battle.h"
#include "battle/battle_information.h"
#include "battle/battle_monosiri.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "system.h"

extern ItemData itemDataTable[];

// .rodata
const f64 double_to_int_802eddc8 = 4503601774854144.0;

// .sdata2
const f32 float_1p5_8041bf08 = 1.5f;
const f32 float_0p1_8041bf0c = 0.1f;
const f32 float_0p4_8041bf10 = 0.4f;
const f32 float_0p5_8041bf14 = 0.5f;
const f32 float_2_8041bf18 = 2.0f;
const f32 float_1_8041bf1c = 1.0f;
const f32 float_0p05_8041bf20 = 0.05f;
const f32 float_5_8041bf24 = 5.0f;

// Sampling work block lives inside BattleWork at offset 0x428 (wp->field_428).
typedef struct BattleSampling {
    BattleWeapon* weapon;          // 0x0
    u8 entries[0xA6C - 0x4];       // 0x4, 74 entries of 0x24 bytes
    s8 count;                      // 0xA6C
    u8 order[0xAB7 - 0xA6D];       // 0xA6D
    s8 selectIdx;                  // 0xAB7
    s32 unitId;                    // 0xAB8
    s8 belong;                     // 0xABC
    u8 pad_0xABD[0xAC0 - 0xABD];  // 0xABD
    u32 field_0xAC0;               // 0xAC0
    u32 field_0xAC4;               // 0xAC4
    s8 attackDirection;            // 0xAC8
} BattleSampling;

// NONMATCHING (71.30%): logic follows the asm (checked in review); register allocation is globally shifted
// (sampling r31 vs r30, wp r26 vs r31) and the int->double pool constant is an anonymous @N@ symbol.
void _btlSamplingEnemy(u8* sampling) {
    BattleWork* wp = _battleWorkPointer;
    BattleSampling* s = (BattleSampling*)sampling;
    BattleSamplingEntry* entries = (BattleSamplingEntry*)(sampling + 4);
    s32 count = 0;
    s32 i;
    s32 j;
    s32 numParts;

    if (s->field_0xAC0 & 0x80000000) {
        s->count = -1;
        return;
    }

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        BattleUnitPart* part;

        if (unit == NULL) {
            continue;
        }
        if (BtlUnit_CheckStatus(unit, 0x1B)) {
            continue;
        }
        if (unit->mAttributeFlags & 0x40) {
            continue;
        }

        part = unit->mParts;
        numParts = unit->mKindParams->numParts;
        for (j = 0; j < numParts; j++) {
            BattleSamplingEntry* entry;
            BattleUnitKindPart* kindPart;
            s32 faceDirection;
            f32 x;
            f32 y;
            f32 z;

            if (part->attributes & kNotTargetable) {
                part = part->mNextPart;
                continue;
            }

            faceDirection = part->faceDirection;
            kindPart = part->mKindPartParams;
            BtlUnit_GetPartsWorldPos(part, &x, &y, &z);

            if (!(unit->mAttributeFlags & kCeiling)) {
                y = part->mPositionOffset.y * unit->mSizeMultiplier + y;
            }

            {
                f32 hitX = x + (f32)faceDirection *
                                     ((part->mHitCursorBaseOffset.x + part->mHitCursorOffset.x) *
                                      unit->mSizeMultiplier);
                f32 hitY = y + (part->mHitCursorBaseOffset.y + part->mHitCursorOffset.y) *
                                     unit->mSizeMultiplier;
                f32 hitZ = z + (part->mHitCursorBaseOffset.z + part->mHitCursorOffset.z) *
                                     unit->mSizeMultiplier;

                entries->unitId = i;
                entries->partId = kindPart->partNum;
                entries->x = (s16)hitX;
                entries->y = (s16)hitY;
                entries->z = (s16)hitZ;
                entries->field_0x18 = 0;
                entries->field_0x20 = -1;
            }

            if ((unit->mAttributeFlags & kUnknown_0x1) &&
                !(part->attributes & 0x20000)) {
                entries->field_0x18 += s->attackDirection * 300;
            }
            entries->field_0x18 += part->field_0x1A0 * faceDirection;

            entries->field_0xC = (s16)(x + entries->field_0x18);
            entries->field_0x10 = (s16)y;
            entries->field_0x14 = (s16)(float_5_8041bf24 + z);
            entries->field_0x1C = BtlCompForwardLv(s->attackDirection, (f32)entries->field_0xC);
            entries->field_0x1D = entries->field_0x14 < -30 ? 0 : 1;

            count++;
            entries++;
            part = part->mNextPart;
        }
    }

    s->selectIdx = 0;
    s->count = count;
    entries = (BattleSamplingEntry*)(sampling + 4);

    for (i = 0; i < count; i++) {
        BattleSamplingEntry* entry = &entries[i];
        BattleUnit* unit = BattleGetUnitPtr(wp, entry->unitId);
        BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, entry->partId);
        s32 ac0 = s->field_0xAC0;

        if ((ac0 & 0x80000000) ||
            ((ac0 & 0x10000) && unit->currentKind != UNIT_MARIO) ||
            ((ac0 & 0x20000) && (unit->currentKind < 0xD8 || unit->currentKind >= 0xDA)) ||
            ((ac0 & 0x1) && (unit->currentKind == UNIT_MARIO || unit->currentKind == UNIT_SHELL_SHIELD)) ||
            ((ac0 & 0x2) && (unit->currentKind >= UNIT_GOOMBELLA && unit->currentKind < UNIT_MAX)) ||
            ((ac0 & 0x10) && (unit->currentKind >= UNIT_GOOMBA && unit->currentKind < 0xD8)) ||
            ((ac0 & 0x40) && (unit->currentKind >= 0xDA && unit->currentKind < 0xDE)) ||
            ((ac0 & 0x20) && (unit->currentKind >= 0xD8 && unit->currentKind < 0xDA)) ||
            ((ac0 & 0x1000) && s->unitId == unit->unitId) ||
            ((ac0 & 0x100) && s->belong == unit->mAlliance) ||
            ((ac0 & 0x200) && s->belong != unit->mAlliance && unit->mAlliance != 2) ||
            ((ac0 & 0x2000) && s->unitId != unit->unitId &&
             unit->currentKind == BattleGetUnitPtr(wp, s->unitId)->currentKind) ||
            ((ac0 & 0x4000) && s->unitId != unit->unitId) ||
            ((ac0 & 0x100000) &&
             (!((part->attributes & kMainPart) || (part->attributes & kImportantPart)) ||
              (part->attributes & 0x40000))) ||
            ((ac0 & 0x200000) &&
             (!((part->attributes & kMainPart) || (part->attributes & kImportantPart) ||
                (part->attributes & kSemiImportantPart)) ||
              (part->attributes & 0x40000)))) {
            for (j = i; j < count - 1; j++) {
                entries[j] = entries[j + 1];
            }
            count--;
            i--;
        }
    }

    for (i = 0; i < count; i++) {
        BattleSamplingEntry* entry = &entries[i];
        BattleUnit* unit = BattleGetUnitPtr(wp, entry->unitId);
        BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, entry->partId);
        s32 ac4 = s->field_0xAC4;

        if (((ac4 & 0x1) &&
             ((part->attributes & 0x80000) || battleGetUnitMonosiriPtr(unit->currentKind) == NULL ||
              battleGetUnitMonosiriPtr(unit->currentKind)->tattleBtlTag == NULL)) ||
            ((ac4 & 0x2) && (unit->mAttributeFlags & 0x4)) ||
            ((ac4 & 0x4) && (unit->mAttributeFlags & 0x2)) ||
            ((ac4 & 0x8) && (unit->mAttributeFlags & 0x4)) ||
            ((ac4 & 0x10) && !(unit->mAttributeFlags & 0x4) && !(unit->mAttributeFlags & 0x2)) ||
            ((ac4 & 0x1000) && (part->attributes & 0x100000)) ||
            ((ac4 & 0x2000) && (part->attributes & 0x200000) &&
             ((ac4 & 0x100000) == 0 || s->unitId != unit->unitId)) ||
            ((ac4 & 0x4000) && (part->attributes & 0x400000)) ||
            ((ac4 & 0x8000) && !(part->attributes & 0x400000)) ||
            ((ac4 & 0x2000000) && (unit->mAttributeFlags & 0x20)) ||
            ((ac4 & 0x4000000) && (part->attributes & 0x2000000))) {
            for (j = i; j < count - 1; j++) {
                entries[j] = entries[j + 1];
            }
            count--;
            i--;
        }
    }

    // A unit's important part is dropped when the same unit also has its main part in the list.
    if (s->field_0xAC0 & 0x100000) {
        for (i = 0; i < count; i++) {
            BattleSamplingEntry* entry = &entries[i];
            BattleUnitPart* part = BtlUnit_GetPartsPtr(BattleGetUnitPtr(wp, entry->unitId), entry->partId);
            s32 keep = 1;

            if (part->attributes & kImportantPart) {
                for (j = 0; j < count; j++) {
                    BattleSamplingEntry* other = &entries[j];

                    if (other != entry && entry->unitId == other->unitId &&
                        (BtlUnit_GetPartsPtr(BattleGetUnitPtr(wp, other->unitId), other->partId)->attributes &
                         kMainPart)) {
                        keep = 0;
                        break;
                    }
                }
            }
            if (!keep) {
                for (j = i; j < count - 1; j++) {
                    entries[j] = entries[j + 1];
                }
                count--;
                i--;
            }
        }
    }

    for (i = 0; i < count; i++) {
        BattleSamplingEntry* entry = &entries[i];
        BattleUnit* unit = BattleGetUnitPtr(wp, entry->unitId);
        BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, entry->partId);
        s32 remove = 0;

        if (s->field_0xAC4 & 0x1000000) {
            if (part->attributes & 0x10) {
                remove = 0;
            } else {
                s32 noDup = 1;

                for (j = 0; j < count; j++) {
                    BattleSamplingEntry* other = &entries[j];
                    s32 a;
                    s32 b;

                    if (other == entry) {
                        continue;
                    }
                    if (entry->field_0x1D != other->field_0x1D) {
                        continue;
                    }
                    a = entry->field_0xC;
                    b = other->field_0xC;
                    if (s->attackDirection < 0) {
                        a = -a;
                        b = -b;
                    }
                    if (a <= b) {
                        continue;
                    }
                    {
                        BattleUnitPart* p2 =
                            BtlUnit_GetPartsPtr(BattleGetUnitPtr(wp, other->unitId), other->partId);
                        BattleUnit* u2;
                        BattleUnit* u3;

                        if (p2->attributes & 0x10) {
                            continue;
                        }
                        if (p2->attributes & 0x400000) {
                            continue;
                        }
                        u2 = BattleGetUnitPtr(wp, s->unitId);
                        u3 = BattleGetUnitPtr(wp, other->unitId);
                        if ((u2->mTokenFlags & kConfusedAction) && u2 == u3) {
                            continue;
                        }
                        noDup = 0;
                        break;
                    }
                }
                if (noDup == 0) {
                    remove = 1;
                }
            }
        }

        if (remove) {
            for (j = i; j < count - 1; j++) {
                entries[j] = entries[j + 1];
            }
            count--;
            i--;
        }
    }

    s->count = count;

    for (i = 0; i < s->count; i++) {
        s->order[i] = i;
    }

    if (s->attackDirection > 0) {
        for (i = 0; i < s->count - 1; i++) {
            for (j = i + 1; j < s->count; j++) {
                if (entries[s->order[i]].x + entries[s->order[i]].field_0x18 * 10 >
                    entries[s->order[j]].x + entries[s->order[j]].field_0x18 * 10) {
                    s8 tmp = s->order[i];
                    s->order[i] = s->order[j];
                    s->order[j] = tmp;
                }
            }
        }
    } else {
        for (i = 0; i < s->count - 1; i++) {
            for (j = i + 1; j < s->count; j++) {
                if (entries[s->order[i]].x + entries[s->order[i]].field_0x18 * 10 <=
                    entries[s->order[j]].x + entries[s->order[j]].field_0x18 * 10) {
                    s8 tmp = s->order[i];
                    s->order[i] = s->order[j];
                    s->order[j] = tmp;
                }
            }
        }
    }
}

s32 BattleSamplingEnemy(u8* sampling, BattleWeapon* weapon, s32 unitId, s8 belong, s32 arg4,
                        s32 arg5, s8 attackDirection) {
    BattleSampling* s = (BattleSampling*)sampling;
    BattleUnit* unit;
    s32 ret = 0;

    s->weapon = weapon;
    s->unitId = unitId;
    s->belong = belong;
    s->field_0xAC0 = arg4;
    s->field_0xAC4 = arg5;
    s->attackDirection = attackDirection;

    unit = BattleGetUnitPtr(_battleWorkPointer, unitId);
    if (unit->mTokenFlags & kConfusedAction) {
        if ((s->field_0xAC0 & 0x100) != 0 && (s->field_0xAC0 & 0x200) == 0) {
            s->field_0xAC0 &= ~0x100;
            s->field_0xAC0 |= 0x200;
            ret = 1;
        } else if ((s->field_0xAC0 & 0x100) == 0 && (s->field_0xAC0 & 0x200) != 0) {
            s->field_0xAC0 |= 0x100;
            s->field_0xAC0 &= ~0x200;
            ret = 1;
        }
    }
    _btlSamplingEnemy(sampling);
    return ret;
}

void BattleSamplingEnemyUpdate(u8* sampling) {
    _btlSamplingEnemy(sampling);
}

// Remaining diffs: (1) the `e = order[n]` load uses r0 instead of r15 (lbzx r0; extsb r15,r0 vs
// lbzx r15; extsb r15,r15); (2) f-register/stack-slot choices in the default itemDataTable FP block;
// (3) sp_restored is s8 in ItemData.h so it emits `lbz; extsb.` where retail's `lbz; cmplwi` shows u8.
// NONMATCHING (99.44%): logic, integer register allocation and switch order match; diffs listed above.
void BattleChoiceSamplingEnemy(u8* sampling, u32 flags, s32* outId, s32* outPartId) {
    s32 n;
    s32 e;
    BattleUnit* unit;
    f32 rate;
    BattleWork* wp = _battleWorkPointer;
    s32 weights[74];
    BattleUnit* baseUnit;
    BattleSampling* s = (BattleSampling*)sampling;
    s32 i;

    *outId = -1;
    *outPartId = 0;
    if (s->count <= 0) {
        return;
    }

    baseUnit = BattleGetUnitPtr(wp, s->unitId);
    n = (s8)s->order[0];
    *outId = ((BattleSamplingEntryBase*)(sampling + n * 0x24))->e.unitId;
    *outPartId = ((BattleSamplingEntryBase*)(sampling + n * 0x24))->e.partId;

    for (i = 0; i < 74; i++) {
        weights[i] = 0;
    }
    weights[(s8)s->order[0]] += 10;

    for (n = 0; n < s->count; n++) {
        e = (s8)s->order[n];

        unit = BattleGetUnitPtr(wp, ((BattleSamplingEntryBase*)(sampling + e * 0x24))->e.unitId);
        weights[e] += 100;
        if ((flags & 0x1) && unit->currentKind == 0xDE) {
            weights[e] = (s32)((f32)weights[e] * float_1p5_8041bf08);
        }
        if ((flags & 0x2) && unit->currentKind >= 0xE0 && unit->currentKind < 0xE7) {
            weights[e] = (s32)((f32)weights[e] * float_1p5_8041bf08);
        }
        if ((flags & 0x4)) {
            rate = (f32)(s->count - n) * float_0p4_8041bf10;
            weights[e] = (s32)((f32)weights[e] * (float_0p1_8041bf0c + rate));
        }
        if ((flags & 0x8)) {
            rate = float_0p4_8041bf10 * (f32)n;
            weights[e] = (s32)((float_0p5_8041bf14 + rate) * (f32)weights[e]);
        }
        if ((flags & 0x10) && baseUnit->mAlliance == unit->mAlliance) {
            weights[e] = (s32)((f32)weights[e] * float_1p5_8041bf08);
        }
        if ((flags & 0x20) && baseUnit->mAlliance != unit->mAlliance && unit->mAlliance != 2) {
            weights[e] = (s32)((f32)weights[e] * float_1p5_8041bf08);
        }
        if ((flags & 0x100)) {
            weights[e] = (s32)((float_2_8041bf18 - (f32)unit->currentHp / (f32)unit->maxHp) *
                               (f32)weights[e]);
        }
        if ((flags & 0x200)) {
            weights[e] = (s32)((float_1_8041bf1c + (f32)unit->currentHp / (f32)unit->maxHp) *
                               (f32)weights[e]);
        }
        if ((flags & 0x400)) {
            weights[e] = (s32)((float_1p5_8041bf08 - (f32)unit->currentHp * float_0p05_8041bf20) *
                               (f32)weights[e]);
        }
        if ((flags & 0x800)) {
            weights[e] = (s32)((float_1_8041bf1c + (f32)unit->currentHp * float_0p05_8041bf20) *
                               (f32)weights[e]);
        }
        if ((flags & 0x1000) && unit->currentHp <= unit->mKindParams->mPerilHP) {
            weights[e] = (s32)((f32)weights[e] * float_1p5_8041bf08);
        }

        if (s->weapon != NULL) {
            if (unit->mAttributeFlags & 0x20000) {
                weights[e] = 1;
            }
            switch (s->weapon->item) {
                case 0x8E:
                    if (BtlUnit_CanActStatus(unit) == 0 ||
                        BtlUnit_CheckStatus(unit, 0x0A)) {
                        weights[e] = 1;
                    }
                    break;
                case 0x92:
                    if (BtlUnit_CheckStatus(unit, 0x0B)) weights[e] = 1;
                    break;
                case 0x93:
                    if (BtlUnit_CheckStatus(unit, 0x0F)) weights[e] = 1;
                    break;
                case 0x8F:
                    if (BtlUnit_CheckStatus(unit, 0x0E)) weights[e] = 1;
                    break;
                case 0x86:
                    if (BtlUnit_CheckStatus(unit, 0x12)) weights[e] = 1;
                    break;
                case 0x87:
                    if (BtlUnit_CheckStatus(unit, 0x06)) weights[e] = 1;
                    break;
                case 0x88:
                    if (BtlUnit_CheckStatus(unit, 0x07)) weights[e] = 1;
                    break;
                case 0x89:
                    if (BtlUnit_CheckStatus(unit, 0x05)) weights[e] = 1;
                    break;
                case 0x99:
                    if (BtlUnit_CheckStatus(unit, 0x01) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x02) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x03) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x04) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x05) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x08) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x09) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x0B) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x0F) == 0 &&
                        BtlUnit_CheckStatus(unit, 0x14) == 0) {
                        weights[e] = 1;
                    }
                    break;
                case 0x0:
                    break;                default: {
                    s32 maxFp;

                    if (itemDataTable[s->weapon->item].fp_restored == 0) {
                        break;
                    }
                    maxFp = BtlUnit_GetMaxFp(unit);
                    if (BtlUnit_GetFp(unit) < maxFp) {
                        s32 curFp;
                        maxFp = BtlUnit_GetMaxFp(unit);
                        curFp = BtlUnit_GetFp(unit);
                        weights[e] = (s32)((f32)weights[e] *
                                           (float_1_8041bf1c +
                                            float_0p1_8041bf0c * (f32)(maxFp - curFp)));
                    } else {
                        if (itemDataTable[s->weapon->item].hp_restored == 0 &&
                            itemDataTable[s->weapon->item].sp_restored == 0) {
                            weights[e] = 1;
                        }
                    }
                    break;
                }
            }
        }
    }

    if ((flags & 0x80000000) == 0) {
        s32 best = 0;
        s32 bestWeight = 0;

        for (i = 0; i < 74; i++) {
            if (weights[i] > bestWeight) {
                best = i;
                bestWeight = weights[i];
            }
        }
        *outId = ((BattleSamplingEntryBase*)(sampling + best * 0x24))->e.unitId;
        *outPartId = ((BattleSamplingEntryBase*)(sampling + best * 0x24))->e.partId;
    } else {
        s32 sum = 0;
        s32 pick;

        for (i = 0; i < 74; i++) {
            sum += weights[i];
        }
        pick = irand(sum);
        for (i = 0; i < 74; i++) {
            pick -= weights[i];
            if (pick <= 0) {
                break;
            }
        }
        *outId = ((BattleSamplingEntryBase*)(sampling + i * 0x24))->e.unitId;
        *outPartId = ((BattleSamplingEntryBase*)(sampling + i * 0x24))->e.partId;
    }
}

void BattleGetFirstAttackUnit(BattleWork* wp, BattleUnit** outUnit1, BattleUnit** outUnit2) {
    BattleUnit* found;
    BattleUnit* out1;
    BattleUnit* out2;
    s32 i;

    for (i = 0; i < 64; i++) {
        found = BattleGetUnitPtr(wp, i);
        if (found != NULL && found->mAlliance == 1) {
            break;
        }
    }

    switch ((s32)wp->fbatInfo->partyId) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            out1 = BattleGetMarioPtr(wp);
            out2 = found;
            break;
        case 7:
        case 8:
            out1 = BattleGetPartnerPtr(wp, BattleGetMarioPtr(wp));
            out2 = found;
            break;
        case 9:
        default:
            out1 = found;
            out2 = BattleGetMarioPtr(wp);
            break;
    }
    *outUnit1 = out1;
    *outUnit2 = out2;
}

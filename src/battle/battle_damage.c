#include "battle/battle_damage.h"
#include "battle/battle.h"
#include "battle/battle_mario.h"
#include "battle/battle_unit.h"
#include "battle/battle_unit_event.h"
#include "battle/battle_disp.h"
#include "system.h"

#include <string.h>

// These five symbols are declared as byte arrays in battle.c (see the switch-table comment there), so a
// prototype in battle_damage.h would clash when battle.c is compiled. Keep the prototypes local to this
// translation unit until the tables are split out.
u8 _getRegistStatus(BattleUnit* unit, s32 type);
void _getSickStatusParam(BattleUnit* unit, BattleWeapon* weapon, s32 type, s8* out1, s8* out2);
s32 _getSickStatusRate(BattleWeapon* weapon, s32 type);
s32 BattleSetStatusDamage(s32* work, BattleUnit* unit, BattleUnitPart* part, s32 weaponSpecial, s32 type,
                          s32 rate, s32 levelDiff, u8 arg8, u8 arg9);
s32 BattleSetStatusDamageFromWeapon(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part,
                                    BattleWeapon* weapon, s32 flags);

void BattleInitCounterPreCheckWork(void* work) {
    memset(work, 0, 0x30);
    *(s32*)work = 1;
}

s32 _checkDamageCode_EmergencyRevival(BattleUnit* unit, s32* flags) {
    if (unit->currentKind == UNIT_MARIO) {
        if ((s32)pouchCheckItem(ITEM_LIFE_SHROOM) > 0) {
            *flags |= 0x800;
            return 1;
        }
        return 0;
    }
    if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
        if ((s32)pouchCheckItem(ITEM_LIFE_SHROOM) > 0) {
            *flags |= 0x800;
            return 1;
        }
        return 0;
    }
    if (unit->heldItem == ITEM_LIFE_SHROOM) {
        *flags |= 0x800;
        return 1;
    }
    return 0;
}

// NONMATCHING (98.87%): MWCC puts flags2 in r27 / defenseAttr in r31, the original the reverse (plus one `mr`
// and `neg` vs `mulli ... -1`); 120 declaration orders and several splits/pragmas do not move the pair.
s32 BattleCalculateDamage(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon,
                          s32* damageCode, s32 flags) {
    BattleWork* wp = _battleWorkPointer;
    s32 damage;
    s32 defense;
    s32 flags2;
    s32 defenseAttr;
    u8 element;
    s32 result;
    s8 turns;
    s8 strength;

    if (unit2->mTokenFlags & 2) {
        flags |= 0x1000;
    }
    element = weapon->element;
    if (unit1 != 0 && (weapon->special & 0x400) && BtlUnit_CheckStatus(unit1, STATUS_BURN)) {
        element = 1;
    }
    flags2 = flags | 0x1e;
    *damageCode = flags2;
    if (weapon->hpDamageFn == 0) {
        return 0;
    }
    if (part->attributes & kUnk_ImmuneToDamageOrStatus) {
        return 0;
    }
    if (unit1 != 0 && ((u32)weapon->special & 1) && unit1->badgesEquipped.AllOrNothing != 0) {
        if (!(wp->actionCommands.result & 2) && !(flags & 0x20000)) {
            return 0;
        }
    }
    damage = weapon->hpDamageWork[0];
    if (weapon->hpDamageFn != 0) {
        damage = ((s32(*)(BattleUnit*, BattleWeapon*, BattleUnit*, BattleUnitPart*))weapon->hpDamageFn)(
            unit1, weapon, unit2, part);
    }
    if (wp->field_18FE1[0] == 1 && (weapon->special & 2)) {
        damage += 3;
    }
    if ((u32)weapon->special & 1) {
        u8 allOrNothing = unit1->badgesEquipped.AllOrNothing;

        damage += unit1->badgesEquipped.PowerPlus;
        damage += unit1->badgesEquipped.PUpDDown;
        if (allOrNothing != 0) {
            if ((wp->actionCommands.result & 2) || (flags & 0x20000)) {
                damage += allOrNothing;
            } else {
                damage = 0;
                goto calculate_defense;
            }
        }
        if (unit1->currentHp <= unit1->mKindParams->mPerilHP) {
            if (unit1->badgesEquipped.MegaRush != 0) {
                damage += unit1->badgesEquipped.MegaRush * 5;
            }
        }
        if (unit1->currentHp <= unit1->mKindParams->mDangerHP) {
            if (unit1->badgesEquipped.PowerRush != 0) {
                damage += unit1->badgesEquipped.PowerRush * 2;
            }
        }
        if (unit1->badgesEquipped.IcePower != 0 && (part->attributes & kWeakToIcePower)) {
            damage += unit1->badgesEquipped.IcePower;
        }
    }
    if (weapon->special & 4) {
        if (BtlUnit_CheckStatus(unit1, STATUS_CHARGE)) {
            BtlUnit_GetStatus(unit1, STATUS_CHARGE, &turns, &strength);
            damage += strength;
            if (damage < 0) {
                damage = 0;
            }
        }
        unit1->mTokenFlags |= kChargeExpended;
    }
    if ((weapon->special & 2) && BtlUnit_CheckStatus(unit1, STATUS_HUGE)) {
        BtlUnit_GetStatus(unit1, STATUS_HUGE, &turns, &strength);
        damage += strength;
        if (damage < 0) {
            damage = 0;
        }
    }
    if ((weapon->special & 2) && BtlUnit_CheckStatus(unit1, STATUS_ATTACK_UP)) {
        BtlUnit_GetStatus(unit1, STATUS_ATTACK_UP, &turns, &strength);
        damage += strength;
        if (damage < 0) {
            damage = 0;
        }
    }
    if ((u32)weapon->special & 1) {
        damage -= unit1->badgesEquipped.PDownDUp;
    }
    if (((u32)weapon->special & 1) && unit1->badgesEquipped.HpDrain != 0) {
        damage -= unit1->badgesEquipped.HpDrain;
    }
    if (((u32)weapon->special & 1) && unit1->badgesEquipped.FpDrain != 0) {
        damage -= unit1->badgesEquipped.FpDrain;
    }
    if ((weapon->special & 2) && BtlUnit_CheckStatus(unit1, STATUS_TINY)) {
        BtlUnit_GetStatus(unit1, STATUS_TINY, &turns, &strength);
        damage += strength;
        if (damage < 0) {
            damage = 0;
        }
    }
    if ((weapon->special & 2) && BtlUnit_CheckStatus(unit1, STATUS_ATTACK_DOWN)) {
        BtlUnit_GetStatus(unit1, STATUS_ATTACK_DOWN, &turns, &strength);
        damage += strength;
        if (damage < 0) {
            damage = 0;
        }
    }
    if (damage < 0) {
        damage = 0;
    }
calculate_defense:
    defense = (s8)part->defense->defenses[element];
    defenseAttr = (s8)part->defenseAttr->defenseAttrs[element];
    switch (element) {
    case 1:
        *damageCode = flags | 0x18;
        break;
    case 2:
        *damageCode = flags | 0x1a;
        break;
    case 4:
        *damageCode = flags | 0x1b;
        break;
    case 3:
        *damageCode = flags | 0x19;
        break;
    case 0:
        *damageCode = flags | 0x17;
        break;
    }
    if (defenseAttr != 3) {
        if (unit2->badgesEquipped.DefendPlus != 0) {
            defense += unit2->badgesEquipped.DefendPlus;
        }
        if (unit2->badgesEquipped.DamageDodge != 0 && (flags & 0x40000)) {
            defense += unit2->badgesEquipped.DamageDodge;
        }
        if (BtlUnit_CheckStatusFlag(unit2, 0x01000000)) {
            defense += 1;
        }
        if (BtlUnit_CheckStatus(unit2, STATUS_DEFENSE_UP)) {
            BtlUnit_GetStatus(unit2, STATUS_DEFENSE_UP, &turns, &strength);
            defense += strength;
        }
        if (BtlUnit_CheckStatus(unit2, STATUS_DEFENSE_DOWN)) {
            BtlUnit_GetStatus(unit2, STATUS_DEFENSE_DOWN, &turns, &strength);
            defense += strength;
            if (defense < 0) {
                defense = 0;
            }
        }
        if (wp->field_18FE1[0] == 2 && unit2->currentKind == 0xde) {
            defense += 3;
        }
        if (part->attributes & 0x20) {
            defense += unit2->work[1];
            if (defense < 0) {
                defense = 0;
            }
        }
        if ((weapon->special & 0x40) && defense > 0) {
            defense = 0;
        }
    }
    result = damage - defense;
    if (defenseAttr == 3) {
        result = damage;
    }
    result += unit2->badgesEquipped.PUpDDown;
    if (result > 0) {
        if (weapon->special & 0x10) {
            result -= unit2->hpDamageCount;
            if (result <= 0) {
                result = 1;
            }
        }
        if (weapon->special & 0x20) {
            result -= *(s32*)unit1->field_0x27C;
            if (result <= 0) {
                result = 1;
            }
        }
    }
    if (flags & 0x40000) {
        result -= 1;
    }
    if (element == 1 && unit2->badgesEquipped.IcePower != 0) {
        result -= unit2->badgesEquipped.IcePower;
    }
    result -= unit2->badgesEquipped.PDownDUp;
    if (unit2->badgesEquipped.DoublePain != 0) {
        result *= unit2->badgesEquipped.DoublePain + 1;
    }
    if (unit2->badgesEquipped.LastStand != 0 && unit2->currentHp <= unit2->mKindParams->mDangerHP) {
        s32 divisor = unit2->badgesEquipped.LastStand + 1;

        result = (result + divisor - 1) / divisor;
    }
    switch (defenseAttr) {
    case 1:
        result += 1;
        break;
    case 2:
        result = 0;
        break;
    case 4:
        if (!(flags & 0x80000000)) {
            result = 0;
        }
        break;
    }
    if (result < 0) {
        result = 0;
    }
    if (defenseAttr == 3) {
        *damageCode |= 0x2000;
        return result * -1;
    }
    if (result <= 0) {
        *damageCode = flags2;
        return 0;
    }
    if ((flags & 0x100) && ((weapon->special & 0x8000) || !(unit2->mAttributeFlags & 0x100))) {
        *damageCode |= 0x10000;
    }
    return result;
}

s32 BattleCalculateFpDamage(BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part,
                            BattleWeapon* weapon, s32* damageCode, s32 flags) {
    s32 damage = 0;
    s32 guardBonus = 0;
    BattleWork* wp = _battleWorkPointer;

    if (*damageCode & 0x2000) {
        return 0;
    }
    if (weapon->fpDamageFn == 0) {
        return 0;
    }
    if (part->attributes & 0x20000000) {
        return 0;
    }
    if (attacker != 0 && (weapon->special & 1) && attacker->badgesEquipped.AllOrNothing) {
        if (!(wp->actionCommands.result & 0x2)) {
            return 0;
        }
    }
    if (weapon->fpDamageFn != 0) {
        damage = ((s32(*)(BattleUnit*, BattleWeapon*, BattleUnit*, BattleUnitPart*))weapon->fpDamageFn)(
            attacker, weapon, target, part);
    }
    if (flags & 0x40000) {
        guardBonus = 1;
    }
    damage = damage - guardBonus;
    if (damage < 0) {
        damage = 0;
    }
    if (damage > 0 && (u8)*damageCode == 0x1e) {
        *damageCode = (*damageCode & ~0xff) | 0x17;
    }
    return damage;
}

void BattleAttackDeclareAll(BattleWork* wp) {
    s32 i;
    u8* sampling = wp->field_428;

    for (i = 0; i < (s8)sampling[0xa6c]; i++) {
        u8* entry = sampling + (s8)sampling[0xa6d + i] * 0x24 + 4;
        s32 out1;
        s32 out2;

        __declare(*(s16*)(entry + 0), *(s16*)(entry + 2), &out1, &out2);
        if (out1 != -1) {
            s32 j;

            for (j = 0; j < (s8)sampling[0xa6c]; j++) {
                s16 matchId = *(s16*)(sampling + (s8)sampling[0xa6d + j] * 0x24 + 4);
                if (matchId == out1) {
                    s32 k;

                    for (k = j; k < (s8)sampling[0xa6c] - 1; k++) {
                        sampling[0xa6d + k] = sampling[0xa6d + k + 1];
                    }
                    if (j <= i) {
                        i--;
                    }
                    sampling[0xa6c]--;
                    break;
                }
            }
            *(s16*)(entry + 0) = out1;
            *(s16*)(entry + 2) = out2;
        }
    }
    sampling[0xab7] = 0;
}

void __declare(s32 unitId, s32 partId, s32* out1, s32* out2) {
    s32 i;
    BattleUnit* unit;
    BattleUnitPart* part;
    BattleWork* wp = _battleWorkPointer;

    (void)partId;
    *out1 = -1;
    *out2 = 0;
    for (i = 0; i < 0x40; i++) {
        s32 numParts;
        s32 j;

        if (i == unitId) {
            continue;
        }
        unit = BattleGetUnitPtr(wp, i);
        if (unit == 0) {
            continue;
        }
        if (unit->protectId != unitId) {
            continue;
        }
        if (unit->mTokenFlags & 1) {
            continue;
        }
        if (unit->mTokenFlags & 4) {
            continue;
        }
        if (!BtlUnit_CanGuardStatus(unit)) {
            continue;
        }
        numParts = unit->mKindParams->numParts;
        part = unit->mParts;
        for (j = 0; j < numParts; j++) {
            if (part->attributes & 0x8) {
                break;
            }
            part = part->mNextPart;
        }
        if (j < numParts) {
            break;
        }
    }
    if (i < 0x40) {
        BattleUnit* u;

        BattleGetUnitPtr(wp, unitId)->damagePattern = 0;
        BattleRunHitEvent(BattleGetUnitPtr(wp, unitId), 0x1029);
        u = BattleGetUnitPtr(wp, unitId);
        u->mTokenFlags |= 4;
        wp->flags |= 1;
        unit->damagePattern = 0;
        BattleRunHitEvent(unit, 0x1038);
        unit->mTokenFlags |= 2;
        *out1 = i;
        *out2 = part->mKindPartParams->partNum;
    }
}

void BattleAttackDeclare(s32* out1, s32* out2) {
    s32 i;
    BattleUnit* unit;
    BattleUnitPart* part;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 0x40; i++) {
        s32 numParts;
        s32 j;

        if (i == *out1) {
            continue;
        }
        unit = BattleGetUnitPtr(wp, i);
        if (unit == 0) {
            continue;
        }
        if (unit->protectId != *out1) {
            continue;
        }
        if (unit->mTokenFlags & 1) {
            continue;
        }
        if (unit->mTokenFlags & 4) {
            continue;
        }
        if (!BtlUnit_CanGuardStatus(unit)) {
            continue;
        }
        numParts = unit->mKindParams->numParts;
        part = unit->mParts;
        for (j = 0; j < numParts; j++) {
            if (part->attributes & 0x8) {
                break;
            }
            part = part->mNextPart;
        }
        if (j < numParts) {
            break;
        }
    }
    if (i < 0x40) {
        BattleUnit* u;

        BattleGetUnitPtr(wp, *out1)->damagePattern = 0;
        BattleRunHitEvent(BattleGetUnitPtr(wp, *out1), 0x1029);
        u = BattleGetUnitPtr(wp, *out1);
        u->mTokenFlags |= 4;
        wp->flags |= 1;
        unit->damagePattern = 0;
        BattleRunHitEvent(unit, 0x1038);
        unit->mTokenFlags |= 2;
        *out1 = i;
        *out2 = part->mKindPartParams->partNum;
    }
}

u8 _getRegistStatus(BattleUnit* unit, s32 type) {
    BattleVulnerableStatus* vuln = unit->vulnStatus;

    switch (type) {
    case STATUS_SLEEP:
        return vuln->sleepRate;
    case STATUS_STOP:
        return vuln->stopRate;
    case STATUS_DIZZY:
        return vuln->dizzyRate;
    case STATUS_POISON:
        return vuln->poisonRate;
    case STATUS_CONFUSE:
        return vuln->confuseRate;
    case STATUS_ELECTRIC:
        return vuln->electricRate;
    case STATUS_BURN:
        return vuln->burnRate;
    case STATUS_FREEZE:
        return vuln->freezeRate;
    case STATUS_HUGE:
        return vuln->hugeRate;
    case STATUS_TINY:
        return vuln->tinyRate;
    case STATUS_ATTACK_UP:
        return vuln->attackUpRate;
    case STATUS_ATTACK_DOWN:
        return vuln->attackDownRate;
    case STATUS_DEFENSE_UP:
        return vuln->defenseUpRate;
    case STATUS_DEFENSE_DOWN:
        return vuln->defenseDownRate;
    case STATUS_ALLERGIC:
        return vuln->allergicRate;
    case STATUS_INSTANT_KILL:
        return vuln->instaKillRate;
    case STATUS_FAST:
        return vuln->fastRate;
    case STATUS_SLOW:
        return vuln->slowRate;
    case STATUS_FRIGHT:
        return vuln->frightRate;
    case STATUS_GALE_FORCE:
        return vuln->galeForceRate;
    case STATUS_DODGY:
        return vuln->dodgyRate;
    case STATUS_INVISIBLE:
        return vuln->invisibleRate;
    default:
        return 100;
    }
}

s32 BattleCheckPikkyoro(BattleWeapon* weapon, s32* out) {
    BattleWork* wp = _battleWorkPointer;
    s32 weights[5] = { 0 };
    s32 count = 0;
    s32 selected = 0;
    s32 result = 0;

    if (out) {
        result = *out;
    }
    if (weapon->special & 0x800) {
        u32 badges = wp->badgeEquippedFlags;

        if (badges & 0x100) {
            weights[0] = 1;
            selected = 0;
            count = 1;
        }
        if (badges & 0x200) {
            weights[1] = 1;
            selected = 1;
            count++;
        }
        if (badges & 0x400) {
            weights[2] = 1;
            selected = 2;
            count++;
        }
        if (badges & 0x800) {
            weights[3] = 1;
            selected = 3;
            count++;
        }
        if (badges & 0x1000) {
            weights[4] = 1;
            selected = 4;
            count++;
        }
    }
    if (count > 0) {
        if (count >= 2) {
            s32 r = irand(count);
            s32 i;

            selected = 4;
            for (i = 4; i > 0; i--) {
                r -= weights[i];
                if (r < 0) {
                    break;
                }
                selected--;
            }
        }
        switch (selected) {
        case 0:
            result |= 0x01000000;
            break;
        case 1:
            result |= 0x02000000;
            break;
        case 2:
            result |= 0x04000000;
            break;
        case 3:
            result |= 0x08000000;
            break;
        case 4:
            result |= 0x10000000;
            break;
        }
    }
    if (out) {
        *out = result;
    }
    return result;
}

// NONMATCHING (96.87%): the GALE_FORCE compare is `cmpwi r8,0` vs `extsb. r0,r8`; the rest is register/relocation naming.
s32 BattleSetStatusDamageFromWeapon(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part,
                                    BattleWeapon* weapon, s32 flags) {
    s32 work = 0;
    u32 i;
    s8 rate;
    s8 p1;
    s8 p2;
    s32 levelDiff;

    if (!(flags & 0x20000000)) {
        for (i = 0; i < 0x1c; i++) {
            _getSickStatusParam(unit1, weapon, i, &p1, &p2);
            rate = _getSickStatusRate(weapon, i);
            levelDiff = 0;
            switch (i) {
            case STATUS_HUGE:
                if (p1 == 0 && p2 == 0) {
                    continue;
                }
                break;
            case STATUS_DEFENSE_UP:
                if (p1 == 0 && p2 == 0) {
                    continue;
                }
                break;
            case STATUS_ATTACK_UP:
                if (p1 == 0 && p2 == 0) {
                    continue;
                }
                break;
            case STATUS_GALE_FORCE:
                if (rate > 0) {
                    levelDiff = unit1->mLevel - unit2->mLevel;
                }
                break;
            }
            if (BattleSetStatusDamage(&work, unit2, part, weapon->special, i, rate, levelDiff, p1, p2)) {
                if (i == STATUS_INVISIBLE) {
                    btlDispPoseAnime(part);
                }
            }
        }
    }
    return work;
}

// NONMATCHING (1.01%): 3 instructions differ, 632 missing, plus register differences.
s32 BattleCheckDamage(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon, s32 flags) {

	if (!unit2) {
		return 18;
	}
	if (part) {
		unit2->currentTarget = part;
	}


}

void _getSickStatusParam(BattleUnit* unit, BattleWeapon* weapon, s32 type, s8* out1, s8* out2) {
    s32 param1;
    s32 param2;
    s32 item;
    s32 count;
    u32 utype = (u32)type;

    switch (type) {
    case STATUS_SLEEP:
        param1 = param2 = weapon->sleepTime;
        break;
    case STATUS_STOP:
        param1 = param2 = weapon->stopTime;
        break;
    case STATUS_DIZZY:
        param1 = param2 = weapon->dizzyTime;
        break;
    case STATUS_POISON:
        param1 = weapon->poisonTime;
        param2 = weapon->poisonStrength;
        break;
    case STATUS_CONFUSE:
        param1 = param2 = weapon->confuseTime;
        break;
    case STATUS_ELECTRIC:
        param1 = param2 = weapon->electricTime;
        break;
    case STATUS_DODGY:
        param1 = param2 = weapon->dodgyTime;
        break;
    case STATUS_BURN:
        param1 = param2 = weapon->burnTime;
        break;
    case STATUS_FREEZE:
        param1 = param2 = weapon->freezeTime;
        break;
    case STATUS_HUGE:
        if (weapon->sizeChangeStrength > 0) {
            param1 = weapon->sizeChangeTime;
            param2 = weapon->sizeChangeStrength;
        } else {
            param1 = param2 = 0;
        }
        break;
    case STATUS_TINY:
        if (weapon->sizeChangeStrength < 0) {
            param1 = weapon->sizeChangeTime;
            param2 = weapon->sizeChangeStrength;
        } else {
            param1 = param2 = 0;
        }
        break;
    case STATUS_ATTACK_UP:
        if (weapon->atkChangeStrength > 0) {
            param1 = weapon->atkChangeTime;
            param2 = weapon->atkChangeStrength;
        } else {
            param1 = param2 = 0;
        }
        break;
    case STATUS_ATTACK_DOWN:
        if (weapon->atkChangeStrength < 0) {
            param1 = weapon->atkChangeTime;
            param2 = weapon->atkChangeStrength;
        } else {
            param1 = param2 = 0;
        }
        break;
    case STATUS_DEFENSE_UP:
        if (weapon->defChangeStrength > 0) {
            param1 = weapon->defChangeTime;
            param2 = weapon->defChangeStrength;
        } else {
            param1 = param2 = 0;
        }
        break;
    case STATUS_DEFENSE_DOWN:
        if (weapon->defChangeStrength < 0) {
            param1 = weapon->defChangeTime;
            param2 = weapon->defChangeStrength;
        } else {
            param1 = param2 = 0;
        }
        break;
    case STATUS_ALLERGIC:
        param1 = param2 = weapon->allergicTime;
        break;
    case STATUS_FAST:
        param1 = param2 = weapon->fastTime;
        break;
    case STATUS_SLOW:
        param1 = param2 = weapon->slowTime;
        break;
    case STATUS_INSTANT_KILL:
#ifdef MUST_MATCH
        // The original sign-extends the byte at 0x9E, but battle_database_common.h declares ohkoChance u8.
        param1 = param2 = ((s8*)weapon)[0x9E];
#else
        param1 = param2 = weapon->ohkoChance;
#endif
        break;
    case STATUS_INVISIBLE:
        param1 = param2 = weapon->invisibleTime;
        break;
    case STATUS_HP_REGEN:
        param1 = weapon->hpRegenTime;
        param2 = weapon->hpRegenStrength;
        break;
    case STATUS_FP_REGEN:
        param1 = weapon->fpRegenTime;
        param2 = weapon->fpRegenStrength;
        break;
    case STATUS_CHARGE:
        param1 = weapon->chargeStrength;
        param2 = weapon->chargeStrength;
        break;
    case STATUS_PAYBACK:
        param1 = param2 = weapon->paybackTime;
        break;
    case STATUS_HOLD_FAST:
        param1 = param2 = weapon->holdFastTime;
        break;
    default:
        param1 = param2 = 0;
        break;
    }

    item = weapon->item;
    count = 0;
    if (item >= BADGE_MIN && item < BADGE_MAX) {
        count = pouchEquipCheckBadge(item);
    }
    if (count > 0) {
        switch (item) {
        case ITEM_SHRINK_STOMP:
            if (utype == STATUS_TINY) {
                param1 += (count - 1) * 2;
            }
            break;
        case ITEM_SLEEPY_STOMP:
            if (utype == STATUS_SLEEP) {
                param1 += (count - 1) * 2;
            }
            break;
        case ITEM_SOFT_STOMP:
            if (utype == STATUS_DEFENSE_DOWN) {
                param1 += (count - 1) * 2;
            }
            break;
        case ITEM_HEAD_RATTLE:
            if (utype == STATUS_CONFUSE) {
                param1 += (count - 1) * 2;
            }
            break;
        case ITEM_ICE_SMASH:
            if (utype == STATUS_FREEZE) {
                param1 += (count - 1) * 2;
            }
            break;
        case ITEM_UNUSED_DEFEND_BADGE:
        case ITEM_UNUSED_DEFEND_BADGE_P:
            if (utype == STATUS_DEFENSE_UP && unit->badgesEquipped.DefendUnused >= 2) {
                param2 += unit->badgesEquipped.DefendUnused - 1;
            }
            break;
        }
    }

    if (utype == STATUS_CHARGE) {
        s32 n;

        if (weapon == &badgeWeapon_Charge || weapon == &badgeWeapon_ChargeP) {
            if (unit->currentKind == UNIT_MARIO) {
                n = pouchEquipCheckBadge(ITEM_CHARGE);
            } else {
                n = pouchEquipCheckBadge(ITEM_CHARGE_P);
            }
            if (n >= 2) {
                param2 += n - 1;
            }
        } else if (weapon == &badgeWeapon_SuperCharge || weapon == &badgeWeapon_SuperChargeP) {
            if (unit->currentKind == UNIT_MARIO) {
                n = pouchEquipCheckBadge(ITEM_SUPER_CHARGE);
            } else {
                n = pouchEquipCheckBadge(ITEM_SUPER_CHARGE_P);
            }
            if (n >= 2) {
                param2 += (n - 1) * 2;
            }
        }
        if (param2 >= 10) {
            param2 = 9;
        }
    }

    *out1 = param1;
    *out2 = param2;
}

s32 _getSickStatusRate(BattleWeapon* weapon, s32 type) {
    u8* w = (u8*)weapon;

    switch (type) {
    case STATUS_SLEEP: return w[0x80];
    case STATUS_STOP: return w[0x82];
    case STATUS_DIZZY: return w[0x84];
    case STATUS_POISON: return w[0x86];
    case STATUS_CONFUSE: return w[0x89];
    case STATUS_ELECTRIC: return w[0x8B];
    case STATUS_DODGY: return w[0x8D];
    case STATUS_BURN: return w[0x8F];
    case STATUS_FREEZE: return w[0x91];
    case STATUS_HUGE:
        if ((s8)w[0x95] > 0) {
            return w[0x93];
        }
        return 0;
    case STATUS_TINY:
        if ((s8)w[0x95] <= 0) {
            return w[0x93];
        }
        return 0;
    case STATUS_ATTACK_UP:
        if ((s8)w[0x98] > 0) {
            return w[0x96];
        }
        return 0;
    case STATUS_ATTACK_DOWN:
        if ((s8)w[0x98] <= 0) {
            return w[0x96];
        }
        return 0;
    case STATUS_DEFENSE_UP:
        if ((s8)w[0x9B] > 0) {
            return w[0x99];
        }
        return 0;
    case STATUS_DEFENSE_DOWN:
        if ((s8)w[0x9B] <= 0) {
            return w[0x99];
        }
        return 0;
    case STATUS_ALLERGIC: return w[0x9C];
    case STATUS_FAST: return w[0xA0];
    case STATUS_SLOW: return w[0xA2];
    case STATUS_FRIGHT: return w[0xA4];
    case STATUS_GALE_FORCE: return w[0xA5];
    case STATUS_INSTANT_KILL: return w[0x9E];
    case STATUS_INVISIBLE: return w[0xA8];
    case STATUS_HP_REGEN: return (s8)w[0xAA] > 0 ? 100 : 0;
    case STATUS_FP_REGEN: return (s8)w[0xAC] > 0 ? 100 : 0;
    case STATUS_CHARGE: return (s8)w[0x9F] != 0 ? 100 : 0;
    case STATUS_PAYBACK: return (s8)w[0xA6] != 0 ? 100 : 0;
    case STATUS_HOLD_FAST: return (s8)w[0xA7] > 0 ? 100 : 0;
    case STATUS_FLIPPED: return 0;
    }
    return 0;
}

// NONMATCHING (99.80%): the (s32)((f32)rate * 0.5f) conversion allocates f1/f2 for the constants,
// whereas the original uses f2/f0; every other byte matches.
s32 BattlePreCheckDamage(BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part, BattleWeapon* weapon, s32 a) {
    BattleWork* wp = _battleWorkPointer;
    s32 count;
    s32 rate;

    if (target == NULL || part == NULL) {
        return 1;
    }
    if (weapon->special & 0x4) {
        attacker->mTokenFlags |= 0x8;
    }
    if ((part->attributes & 0x4000) && !(weapon->special & 0x40000)) {
        return 2;
    }
    if (weapon->special & 0x8) {
        return 1;
    }
    if (BtlUnit_CheckStatus(target, 0x12) || (target->mAttributeFlags & 0x8)) {
        return 4;
    }
    if (target->mAttributeFlags & 0x10) {
        return 4;
    }
    if ((part->counterAttributes & 0x2) && !(weapon->counterResistanceFlags & 0x4) &&
        attacker->badgesEquipped.SpikeShield == 0) {
        return 5;
    }
    if (part->attributes & 0x10) {
        return 4;
    }
    if (a & 0x100000) {
        return 1;
    }
    if (target->badgesEquipped.PrettyLucky != 0) {
        count = target->badgesEquipped.PrettyLucky;
        while (count > 0) {
            if (irand(100) >= 0x5A) {
                return 3;
            }
            count--;
        }
    }
    if (target->badgesEquipped.LuckyDay != 0) {
        count = target->badgesEquipped.LuckyDay;
        while (count > 0) {
            if (irand(100) >= 0x4B) {
                return 3;
            }
            count--;
        }
    }
    if (target->currentHp <= target->mKindParams->mDangerHP) {
        if (target->badgesEquipped.CloseCall != 0) {
            count = target->badgesEquipped.CloseCall;
            while (count > 0) {
                if (irand(100) >= 0x43) {
                    return 3;
                }
                count--;
            }
        }
    }
    if (BtlUnit_CheckStatus(target, 7)) {
        if (!BtlUnit_CheckStatus(target, 1)) {
            if (!BtlUnit_CheckStatus(target, 2)) {
                if (irand(100) >= 0x32) {
                    return 6;
                }
            }
        }
    }
    rate = ((u8*)weapon)[0x10];
    if (BtlUnit_CheckStatus(attacker, 3)) {
        rate = rate * 0x32 / 100;
    }
    if (*(u32*)(wp->field_180C0 + 0x1E0)) {
        rate = (s32)((f32)rate * 0.5f);
    }
    if (irand(100) >= rate) {
        return 2;
    }
    return 1;
}

s32 BattleCheckCounter(void* work, BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part,
                       BattleWeapon* weapon, s32* out) {
    CounterPreCheckWork* w = (CounterPreCheckWork*)work;

    if (attacker == NULL) {
        return 0;
    }
    w->field_0x28 = 0;

    if ((part->counterAttributes & kCounterTopSpiky) && attacker->badgesEquipped.SpikeShield == 0 &&
        !(weapon->counterResistanceFlags & 0x2)) {
        w->field_0x20 = 0x2B;
        w->field_0x24 = 0x12B;
        w->field_0x28++;
        w->field_0x2C = 0x1F;
        *out |= 0x100;
        w->field_0x0 = 0;
    } else if ((part->counterAttributes & kCounterPreemptiveFrontSpiky) &&
               attacker->badgesEquipped.SpikeShield == 0 && !(weapon->counterResistanceFlags & 0x4)) {
        w->field_0x20 = 0x2C;
        w->field_0x24 = 0x12C;
        w->field_0x28++;
        w->field_0x2C = 0x20;
        *out |= 0x100;
        w->field_0x0 = 0;
    } else if ((part->counterAttributes & kCounterFrontSpiky) && attacker->badgesEquipped.SpikeShield == 0 &&
               !(weapon->counterResistanceFlags & 0x8)) {
        w->field_0x20 = 0x2D;
        w->field_0x24 = 0x12D;
        w->field_0x28++;
        w->field_0x2C = 0x21;
        *out |= 0x100;
        w->field_0x0 = 0;
    } else if (((part->counterAttributes & kCounterFiery) || (part->counterAttributes & kCounterFieryStatus)) &&
               attacker->badgesEquipped.IcePower == 0 && !(weapon->counterResistanceFlags & 0x10)) {
        w->field_0x20 = 0x2E;
        w->field_0x24 = 0x12E;
        w->field_0x28++;
        w->field_0x2C = 0x22;
        *out |= 0x100;
        w->field_0x0 = 0;
        if (part->counterAttributes & kCounterFieryStatus) {
            w->field_0x8 = 1;
        }
    } else if (((part->counterAttributes & kCounterIcy) || (part->counterAttributes & kCounterIcyStatus)) &&
               !(weapon->counterResistanceFlags & 0x20)) {
        w->field_0x20 = 0x2F;
        w->field_0x24 = 0x12F;
        w->field_0x28++;
        w->field_0x2C = 0x23;
        *out |= 0x100;
        w->field_0x0 = 0;
        if (part->counterAttributes & kCounterIcyStatus) {
            w->field_0x14 = 1;
        }
    } else if (((part->counterAttributes & kCounterElectric) ||
                (part->counterAttributes & kCounterElectricStatus)) &&
               !(weapon->counterResistanceFlags & 0x1) &&
               BtlUnit_CheckStatus(attacker, STATUS_ELECTRIC) == 0) {
        w->field_0x20 = 0x30;
        w->field_0x24 = 0x130;
        w->field_0x28++;
        w->field_0x2C = 0x24;
        *out |= 0x100;
        w->field_0x0 = 0;
        if (part->counterAttributes & kCounterElectricStatus) {
            w->field_0x10 = 1;
        }
    } else {
        if ((part->counterAttributes & kCounterExplosive) && !(weapon->counterResistanceFlags & 0x80) &&
            (weapon->element == 3 || weapon->element == 4 || weapon->element == 1 ||
             ((weapon->special & 0x400) && BtlUnit_CheckStatus(attacker, STATUS_BURN) != 0))) {
            w->field_0x20 = 0x31;
            w->field_0x24 = 0x131;
            w->field_0x28++;
            *out |= 0x100;
        }
        if ((part->counterAttributes & kCounterVolatileExplosive) && !(weapon->counterResistanceFlags & 0x100)) {
            w->field_0x20 = 0x32;
            w->field_0x24 = 0x132;
            w->field_0x28++;
            *out |= 0x100;
        }
        if ((BtlUnit_CheckStatus(target, STATUS_ELECTRIC) != 0 ||
             (part->counterAttributes & kCounterElectric) ||
             (part->counterAttributes & kCounterElectricStatus)) &&
            !(weapon->counterResistanceFlags & 0x1) &&
            BtlUnit_CheckStatus(attacker, STATUS_ELECTRIC) == 0) {
            w->field_0x20 = 0x34;
            w->field_0x24 = 0x134;
            w->field_0x28++;
            *out |= 0x100;
            if (part->counterAttributes & kCounterElectricStatus) {
                w->field_0x10 = 1;
            }
        }
        if (BtlUnit_CheckStatus(target, STATUS_PAYBACK) != 0 && !(weapon->counterResistanceFlags & 0x200)) {
            w->field_0x20 = 0x35;
            w->field_0x24 = 0x135;
            *out |= 0x100;
            w->field_0x4 = 1;
        }
        if (((part->counterAttributes & kCounterPoison) || (part->counterAttributes & kCounterPoisonStatus)) &&
            !(weapon->counterResistanceFlags & 0x40)) {
            w->field_0x20 = 0x33;
            w->field_0x24 = 0x133;
            *out |= 0x100;
            w->field_0x4 = 1;
            if (part->counterAttributes & kCounterPoisonStatus) {
                w->field_0xC = 1;
            }
        }
        if (BtlUnit_CheckStatus(target, STATUS_HOLD_FAST) != 0 && !(weapon->counterResistanceFlags & 0x400)) {
            w->field_0x20 = 0x36;
            w->field_0x24 = 0x136;
            *out |= 0x100;
            w->field_0x18 = 1;
        }
        if (target->badgesEquipped.ReturnPostage != 0 && !(weapon->counterResistanceFlags & 0x200)) {
            w->field_0x20 = 0x35;
            w->field_0x24 = 0x135;
            *out |= 0x100;
            w->field_0x1C = 1;
        }
    }

    switch (w->field_0x20) {
    case 0x2B:
        return 8;
    case 0x2C:
        return 7;
    case 0x2D:
        return 9;
    case 0x2E:
        return 10;
    case 0x2F:
        return 11;
    case 0x31:
        return 12;
    case 0x32:
        return 13;
    case 0x34:
        return 14;
    case 0x33:
        return 15;
    case 0x35:
        return 16;
    case 0x36:
        return 17;
    default:
        return 0;
    }
}

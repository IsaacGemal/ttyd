#include "battle/battle_damage.h"
#include "battle/battle.h"
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

// The original uses size-oriented prologues (stmw/lmw) and -O4,s scheduling; the file wants -O4,s.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// The original uses an stmw/lmw size prologue; the file wants -O4,s.
#pragma optimize_for_size on
// NONMATCHING (99.86%): one `cmpw` has its operands swapped (r4,r0 vs r0,r4); everything else matches.
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
                if (*(s16*)(sampling + (s8)sampling[0xa6d + j] * 0x24 + 4) == out1) {
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
#pragma optimize_for_size off

// The original uses an stmw/lmw size prologue; the file wants -O4,s.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// The original uses an stmw/lmw size prologue; the file wants -O4,s.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

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

// The original uses a size-oriented prologue and keeps `weights` on the stack; the file wants -O4,s.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// Size-op: the original's instruction selection here matches -O4,s (see the header note for this file).
#pragma optimize_for_size on
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

// Size-op: the original's instruction selection here matches -O4,s (see the header note for this file).
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// Size-op: the original's instruction selection here matches -O4,s (see the header note for this file).
#pragma optimize_for_size on
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
#pragma optimize_for_size off

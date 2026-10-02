#include "battle/battle_damage.h"
#include "battle/battle.h"
#include "battle/battle_mario.h"
#include "battle/battle_unit.h"
#include "battle/battle_unit_event.h"
#include "battle/battle_disp.h"
#include "battle/battle_status_effect.h"
#include "driver/effdrv.h"
#include "eff/eff_updown.h"
#include "eff/eff_nice.h"
#include "eff/eff_bomb.h"
#include "battle/battle_ac.h"
#include "battle/battle_ac_help.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_audience.h"
#include "battle/battle_event_cmd.h"
#include "eff/n64/eff_stamp_n64.h"
#include "eff/n64/eff_vapor_n64.h"
#include "pmario_sound.h"
#include "system.h"

#include <string.h>
#include "matching.h"

// These five symbols are declared as byte arrays in battle.c (see the switch-table comment there), so a
// prototype in battle_damage.h would clash when battle.c is compiled. Keep the prototypes local to this
// translation unit until the tables are split out.
u8 _getRegistStatus(BattleUnit* unit, s32 type);
void _getSickStatusParam(BattleUnit* unit, BattleWeapon* weapon, s32 type, s8* out1, s8* out2);
s32 _getSickStatusRate(BattleWeapon* weapon, s32 type);
s32 BattleSetStatusDamage(s32* work, BattleUnit* unit, BattleUnitPart* part, s32 weaponSpecial, u32 type,
                          s32 rate, s32 levelDiff, s8 turns, s8 strength);
s32 BattleSetStatusDamageFromWeapon(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part,
                                    BattleWeapon* weapon, s32 flags);

#pragma dont_inline on
void BattleInitCounterPreCheckWork(void* work) {
    memset(work, 0, 0x30);
    *(s32*)work = 1;
}
#pragma dont_inline reset

#pragma dont_inline on
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
#pragma dont_inline reset

static inline u8 getAllOrNothing(BattleUnit* unit) {
    return unit->badgesEquipped.AllOrNothing;
}

// `int flags2` (not `s32`, which is `signed long` here) and reading the defense tables through `s8*` are what
// reproduce the original's register allocation: flags2 in r31 and defenseAttr in r27. The tables are declared
// `u8` in battle_unit.h but the original sign-extends them (`extsb`), so they are really signed bytes; the
// `s8*` view is the only way to say that without editing the header. The getAllOrNothing inline above likewise
// keeps the damage-block register pressure right, and `result * -1u` gives the original's `mulli`.
s32 BattleCalculateDamage(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon,
                          s32* damageCode, s32 flags) {
    BattleWork* wp = _battleWorkPointer;
    s32 damage;
    s32 defense;
    int flags2;
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
    if (unit1 != 0 && ((u32)weapon->special & 1) && getAllOrNothing(unit1) != 0) {
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
        u8 allOrNothing = getAllOrNothing(unit1);

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
    defense = ((s8*)part->defense->defenses)[element];
    defenseAttr = ((s8*)part->defenseAttr->defenseAttrs)[element];
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
    default:
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
        return result * -1u;
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

#pragma dont_inline on
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
#pragma dont_inline reset

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

// dont_inline: BattleSetStatusDamage calls this out of line
#pragma dont_inline on
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
#pragma dont_inline reset

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

// The six colour bytes the stamp effect reads (the tint and the fade-out tint).
#define STAMP_COLOR(e, r0, g0, b0, r1, g1, b1)                           \
    {                                                                    \
        ((effStampData*)(e)->userdata)->field_0x38 = r0;                 \
        ((effStampData*)(e)->userdata)->field_0x39 = g0;                 \
        ((effStampData*)(e)->userdata)->field_0x3A = b0;                 \
        ((effStampData*)(e)->userdata)->field_0x3B = r1;                 \
        ((effStampData*)(e)->userdata)->field_0x3C = g1;                 \
        ((effStampData*)(e)->userdata)->field_0x3D = b1;                 \
    }

// Applies one status (sleep, stop, poison, ...) to `unit` from an attack of `part`'s side: the immunity and
// resist checks, the roll against `rate`, the status itself, the hit effect and the announcement. `*work` collects
// what happened (1/2 got/lost a status, 4/8 fright/gale force, 0x10 instant kill, 0x20 sleep).
// NONMATCHING (98.63%): register allocation differences only.
s32 BattleSetStatusDamage(s32* work, BattleUnit* unit, BattleUnitPart* part, s32 weaponSpecial, u32 type,
                          s32 rate, s32 levelDiff, s8 turns, s8 strength) {
    u8 chargeStrength;
    u8 sp9;
#ifdef MUST_MATCH
    int new_var2;
#endif
    u8 sp8;
    f32 x, y, z;
    Vec hit;
    s32 isRemoving;
    s32 applied;
    s32 announced;
    s32 resist;
    u32 resistFlag;
#ifdef MUST_MATCH
    f32 new_var;
#endif
    EffectEntry* eff;

    isRemoving = 0;
    BtlUnit_GetStatus(unit, STATUS_CHARGE, NULL, (s8*)&chargeStrength);
    switch (type) {
    case STATUS_FAST:
    case STATUS_SLOW:
        if (turns <= 0) {
            isRemoving = 1;
        }
        break;
    case STATUS_ALLERGIC:
    case STATUS_PAYBACK:
        if (turns <= 0) {
            isRemoving = 1;
        }
        break;
    case STATUS_HUGE:
    case STATUS_TINY:
    case STATUS_ATTACK_UP:
    case STATUS_ATTACK_DOWN:
    case STATUS_DEFENSE_UP:
    case STATUS_DEFENSE_DOWN:
        if (strength == 0) {
            isRemoving = 1;
        }
        break;
    case STATUS_SLEEP:
    case STATUS_STOP:
    case STATUS_DIZZY:
    case STATUS_POISON:
    case STATUS_CONFUSE:
    case STATUS_ELECTRIC:
    case STATUS_DODGY:
    case STATUS_BURN:
    case STATUS_FREEZE:
        if (turns == 0) {
            isRemoving = 1;
        }
        break;
    case STATUS_CHARGE:
    case STATUS_FLIPPED:
    case STATUS_INVISIBLE:
    case STATUS_HOLD_FAST:
    case STATUS_HP_REGEN:
    case STATUS_FP_REGEN:
        break;
    }
    if (part->attributes & kUnk_ImmuneToDamageOrStatus) {
        return 0;
    }
    if (part->attributes & kUnk_ImmuneToStatus) {
        return 0;
    }
    if (BtlUnit_CheckStatus(unit, STATUS_ALLERGIC) && isRemoving == 0) {
        return 0;
    }
    if (part->attributes & kUnk_ImmageToOHKO) {
        switch (type) {
        case STATUS_ALLERGIC:
        case STATUS_SLEEP:
        case STATUS_STOP:
        case STATUS_DIZZY:
        case STATUS_POISON:
        case STATUS_CONFUSE:
        case STATUS_BURN:
        case STATUS_FREEZE:
        case STATUS_TINY:
        case STATUS_ATTACK_DOWN:
        case STATUS_DEFENSE_DOWN:
        case STATUS_SLOW:
        case STATUS_FRIGHT:
        case STATUS_GALE_FORCE:
        case STATUS_INSTANT_KILL:
            if (isRemoving == 0) {
                return 0;
            }
            break;
        }
    }
    if (unit->badgesEquipped.FeelingFine != 0) {
        switch (type) {
        case STATUS_SLEEP:
        case STATUS_STOP:
        case STATUS_DIZZY:
        case STATUS_POISON:
        case STATUS_CONFUSE:
        case STATUS_ELECTRIC:
        case STATUS_TINY:
        case STATUS_ATTACK_DOWN:
        case STATUS_DEFENSE_DOWN:
        case STATUS_INSTANT_KILL:
            return 0;
        }
    }
    if (type == STATUS_BURN && unit->badgesEquipped.IcePower != 0) {
        return 0;
    }
    if (rate <= 0) {
        return 0;
    }
    resist = _getRegistStatus(unit, type);
    resistFlag = weaponSpecial & 0x100;
    if (resistFlag == 0 || resist == 0) {
        rate = rate * (u8)resist / 100;
        if (resistFlag == 0 && resist > 0) {
            rate += levelDiff;
        }
    }
    if (irand(100) >= rate) {
        return 0;
    }
    applied = 0;
    if (type == STATUS_FRIGHT) {
        *work |= 4;
    } else if (type == STATUS_GALE_FORCE) {
        *work |= 8;
    } else {
        if (type == STATUS_SLEEP) {
            *work |= 0x20;
        }
        if (type == STATUS_INSTANT_KILL) {
            *work |= 0x10;
        }
        applied = BtlUnit_SetStatus(unit, (s8)type, turns, strength);
    }
    switch (type) {
    case STATUS_ELECTRIC:
    case STATUS_DODGY:
    case STATUS_HUGE:
    case STATUS_ATTACK_UP:
    case STATUS_DEFENSE_UP:
    case STATUS_CHARGE:
    case STATUS_INVISIBLE:
    case STATUS_FAST:
    case STATUS_PAYBACK:
    case STATUS_HOLD_FAST:
    case STATUS_HP_REGEN:
    case STATUS_FP_REGEN:
        *work |= 2;
        break;
    default:
        if (turns > 0) {
            *work |= 1;
        } else {
            *work |= 2;
        }
        break;
    }
    if (applied != 0) {
        BtlUnit_GetPos(unit, &x, &y, &z);
        if (unit->mAttributeFlags & 2) {
            y = y - (f32)(BtlUnit_GetHeight(unit) / 2);
        } else {
            y = y + (f32)(BtlUnit_GetHeight(unit) / 2);
        }
        switch (unit->currentKind) {
        case 0x17:
        case 0x84:
        case 0xAB:
            x -= 300.0f;
            z += 30.0f;
            break;
        }
        switch (type) {
        case STATUS_SLEEP:
            effStampN64Entry(0, x, y, 10.0f + z);
            break;
        case STATUS_CONFUSE:
            effStampN64Entry(1, x, y, 10.0f + z);
            break;
        case STATUS_STOP:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0x6E, 0x6E, 0xA, 0xA, 0xA);
            break;
        case STATUS_DIZZY:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0, 0x6E, 0xDC, 0xA, 0xA, 0xA);
            break;
        case STATUS_POISON:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0, 0xDC, 0, 0xA, 0xA, 0xA);
            break;
        case STATUS_ELECTRIC:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0, 0xA, 0xA, 0xA);
            break;
        case STATUS_CHARGE:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0xDC, 0xDC, 0);
            break;
        case STATUS_PAYBACK:
        case STATUS_HOLD_FAST:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0, 0, 0, 0xDC);
            break;
        case STATUS_HUGE:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0xDC, 0, 0);
            break;
        case STATUS_TINY:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0, 0, 0xA, 0xA, 0xA);
            break;
        case STATUS_ATTACK_UP:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0xDC, 0, 0);
            break;
        case STATUS_ATTACK_DOWN:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0, 0, 0xA, 0xA, 0xA);
            break;
        case STATUS_DEFENSE_UP:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0, 0, 0xDC);
            break;
        case STATUS_DEFENSE_DOWN:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0, 0, 0xDC, 0xA, 0xA, 0xA);
            break;
        case STATUS_FAST:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0xDC, 0x6E, 0);
            break;
        case STATUS_SLOW:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0x6E, 0, 0xA, 0xA, 0xA);
            break;
        case STATUS_BURN:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0, 0xDC, 0, 0);
            break;
        case STATUS_FREEZE:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0, 0x6E, 0xDC);
            break;
        case STATUS_ALLERGIC:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xA0, 0xDC, 0, 0xA, 0xA, 0xA);
            break;
        case STATUS_INVISIBLE:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0x6E, 0x6E, 0x6E);
            break;
        case STATUS_HP_REGEN:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0xDC, 0x6E, 0x6E);
            break;
        case STATUS_FP_REGEN:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0xA0, 0xDC, 0);
            break;
        case STATUS_DODGY:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0, 0xDC, 0);
            break;
        case STATUS_INSTANT_KILL:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xDC, 0xDC, 0xDC, 0xA, 0xA, 0xA);
            break;
        default:
            eff = effStampN64Entry(2, x, y, 10.0f + z);
            STAMP_COLOR(eff, 0xC8, 0xC8, 0xC8, 0xA, 0xA, 0xA);
            break;
        }
        switch ((s32)type) {
        case STATUS_HUGE:
        case STATUS_TINY:
        case STATUS_ATTACK_UP:
        case STATUS_ATTACK_DOWN:
        case STATUS_CHARGE:
#ifdef MUST_MATCH
            new_var = y;
            effUpdownEntry(0, x, new_var, 15.0f + z, strength, 60);
#else
            effUpdownEntry(0, x, y, 15.0f + z, strength, 60);
#endif
            break;
        case STATUS_DEFENSE_UP:
        case STATUS_DEFENSE_DOWN:
            effUpdownEntry(1, x, y, 20.0f + z, strength, 60);
            break;
        }
    } else {
        BtlUnit_GetPos(unit, &x, &y, &z);
        y += (f32)(BtlUnit_GetHeight(unit) / 2);
        switch (type) {
        case STATUS_BURN:
            if (unit->mFreezeEff != NULL) {
                effSoftDelete(unit->mFreezeEff);
            }
            break;
        case STATUS_FREEZE:
            eff = effVaporN64Entry(0, 60, x, y, z, 1.0f);
            ((effVaporN64Data*)eff->userdata)->color1R = 0xFF;
            ((effVaporN64Data*)eff->userdata)->color1G = 0xFF;
            ((effVaporN64Data*)eff->userdata)->color1B = 0xFF;
            ((effVaporN64Data*)eff->userdata)->color2R = 0xFF;
            ((effVaporN64Data*)eff->userdata)->color2G = 0xFF;
            ((effVaporN64Data*)eff->userdata)->color2B = 0xFF;
            break;
        }
    }
    announced = 0;
    if (applied != 0) {
        BtlUnit_GetStatus(unit, (s8)type, (s8*)&sp9, (s8*)&sp8);
        switch ((s32)type) {
        case STATUS_CHARGE:
            if ((s8)sp8 > 0) {
                if ((s8)chargeStrength != (s8)sp8) {
                    BattleStatusChangeInfoSetAnnouce(unit, type, (s8)sp8, 1);
                    BattleStatusChangeMsgSetAnnouce(unit, type, 1);
                    announced = 1;
                } else {
                    BattleStatusChangeInfoSetAnnouce(unit, type, (s8)sp8, 0);
                    BattleStatusChangeMsgSetAnnouce(unit, type, 0);
                    announced = 1;
                }
            }
            break;
        case STATUS_POISON:
            if ((s8)sp8 > 0) {
                BattleStatusChangeInfoSetAnnouce(unit, type, (s8)sp8, 1);
                BattleStatusChangeMsgSetAnnouce(unit, type, 1);
                announced = 1;
            }
            break;
        case STATUS_ALLERGIC:
        case STATUS_SLEEP:
        case STATUS_STOP:
        case STATUS_DIZZY:
        case STATUS_CONFUSE:
        case STATUS_ELECTRIC:
        case STATUS_DODGY:
        case STATUS_BURN:
        case STATUS_FREEZE:
        case STATUS_HUGE:
        case STATUS_TINY:
        case STATUS_ATTACK_UP:
        case STATUS_ATTACK_DOWN:
        case STATUS_DEFENSE_UP:
        case STATUS_DEFENSE_DOWN:
        case STATUS_INVISIBLE:
        case STATUS_FAST:
        case STATUS_SLOW:
        case STATUS_PAYBACK:
        case STATUS_HOLD_FAST:
        case STATUS_HP_REGEN:
        case STATUS_FP_REGEN:
            if ((s8)sp9 > 0) {
#ifdef MUST_MATCH
                BattleStatusChangeInfoSetAnnouce(unit, type, (s8)(new_var2 = sp9), 1);
#else
                BattleStatusChangeInfoSetAnnouce(unit, type, (s8)sp9, 1);
#endif
                BattleStatusChangeMsgSetAnnouce(unit, type, 1);
                announced = 1;
            }
            break;
        }
    }
    BtlUnit_GetHitPos(unit, part, &hit.x, &hit.y, &hit.z);
    if (announced != 0) {
        switch (type) {
        case STATUS_SLEEP:
            psndSFXOn_3D("SFX_CONDITION_SLEEP1", &hit);
            break;
        case STATUS_STOP:
            psndSFXOn_3D("SFX_CONDITION_STOP1", &hit);
            break;
        case STATUS_DIZZY:
            psndSFXOn_3D("SFX_CONDITION_GURUGURU1", &hit);
            break;
        case STATUS_CONFUSE:
            psndSFXOn_3D("SFX_CONDITION_CONFUSE1", &hit);
            break;
        case STATUS_POISON:
            psndSFXOn_3D("SFX_CONDITION_POISON1", &hit);
            break;
        case STATUS_DODGY:
            psndSFXOn_3D("SFX_CONDITION_YOKEYOKE1", &hit);
            break;
        case STATUS_ELECTRIC:
            psndSFXOn_3D("SFX_CONDITION_BIRIBIRI1", &hit);
            break;
        case STATUS_HUGE:
            psndSFXOn_3D("SFX_CONDITION_DEKADEKA1", &hit);
            break;
        case STATUS_TINY:
            psndSFXOn_3D("SFX_CONDITION_MINIMINI1", &hit);
            break;
        case STATUS_DEFENSE_UP:
            psndSFXOn_3D("SFX_CONDITION_KATIKATI1", &hit);
            break;
        case STATUS_DEFENSE_DOWN:
            psndSFXOn_3D("SFX_BTL_FUNYA1", &hit);
            break;
        case STATUS_SLOW:
            psndSFXOn_3D("SFX_CONDITION_SLOW1", &hit);
            break;
        case STATUS_BURN:
            psndSFXOn_3D("SFX_CONDITION_FIRE1", &hit);
            break;
        case STATUS_FREEZE:
            psndSFXOn_3D("SFX_CONDITION_ICE1", &hit);
            break;
        case STATUS_FAST:
            psndSFXOn_3D("SFX_CONDITION_FAST1", &hit);
            break;
        case STATUS_HP_REGEN:
            psndSFXOn_3D("SFX_CONDITION_REST_HP_SLOW1", &hit);
            break;
        case STATUS_FP_REGEN:
            psndSFXOn_3D("SFX_CONDITION_REST_FP_SLOW1", &hit);
            break;
        case STATUS_ALLERGIC:
            psndSFXOn_3D("SFX_CONDITION_ALLERGY1", &hit);
            break;
        case STATUS_ATTACK_UP:
            psndSFXOn_3D("SFX_CONDITION_TUYOTUYO1", &hit);
            break;
        case STATUS_ATTACK_DOWN:
            psndSFXOn_3D("SFX_CONDITION_YOWAYOWA1", &hit);
            break;
        case STATUS_CHARGE:
            psndSFXOn_3D("SFX_CONDITION_CHARGE1", &hit);
            break;
        case STATUS_INVISIBLE:
            psndSFXOn_3D("SFX_CONDITION_TOUMEI1", &hit);
            break;
        case STATUS_PAYBACK:
            psndSFXOn_3D("SFX_CONDITION_COUNTER1", &hit);
            break;
        case STATUS_HOLD_FAST:
            psndSFXOn_3D("SFX_BTL_THUNDERS_COUNTER1", &hit);
            break;
        }
    }
    return 1;
}

// The `s32 rate` plus `(s8)` casts reproduce the original `extsb r8,r3` / `cmpwi r8,0` (a plain s8
// rate makes the GALE_FORCE test `extsb. r0,r8`). The call casts BattleSetStatusDamage to a prototype
// whose arg8/arg9 are `s8` (the callee sign-extends both, `extsb. r0,r28`/`extsb. r0,r29`); with the
// file-scope `u8` prototype the scheduler loads p1 (arg8) before `weapon->special` (arg4), where the
// original loads `lwz r6,0x74(r30)` first and then `lbz r10,0x11(r1)`. The jump-table reloc name
// differs; objdiff ignores it.
#pragma dont_inline on
s32 BattleSetStatusDamageFromWeapon(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part,
                                    BattleWeapon* weapon, s32 flags) {
    s32 work = 0;
    u32 i;
#ifdef MUST_MATCH
    s32 rate;
#else
    s8 rate;
#endif
    s8 p1;
    s8 p2;
    s32 levelDiff;

    if (!(flags & 0x20000000)) {
        for (i = 0; i < 0x1c; i++) {
            _getSickStatusParam(unit1, weapon, i, &p1, &p2);
#ifdef MUST_MATCH
            rate = (s8)_getSickStatusRate(weapon, i);
#else
            rate = _getSickStatusRate(weapon, i);
#endif
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
#ifdef MUST_MATCH
            if (((s32 (*)(s32*, BattleUnit*, BattleUnitPart*, s32, s32, s32, s32, s8, s8))BattleSetStatusDamage)(&work, unit2, part, weapon->special, i, (s8)rate, levelDiff, p1, p2)) {
#else
            if (BattleSetStatusDamage(&work, unit2, part, weapon->special, i, rate, levelDiff, p1, p2)) {
#endif
                if (i == STATUS_INVISIBLE) {
                    btlDispPoseAnime(part);
                }
            }
        }
    }
    return work;
}
#pragma dont_inline reset

#define BU_S8(u, off) (*(s8*)((u8*)(u) + (off)))
#define BU_S32(u, off) (*(s32*)((u8*)(u) + (off)))
#define BU_U32(u, off) (*(u32*)((u8*)(u) + (off)))

// NONMATCHING: first draft.
s32 BattleCheckDamage(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon, s32 flags) {
    BattleWork* wp;
    CounterPreCheckWork counter;
    s32 result = -1;
    s32 statusResult = 0;
    s32 guarded = 0;
    s32 damage;
    s32 fpDamage;
    s32 damageCode;
    s32 dc1;
    s32 dc2;
    Vec posA;
    Vec posB;
    Vec posC;

    wp = _battleWorkPointer;
    BattleInitCounterPreCheckWork(&counter);
    if (unit2 == NULL) {
        return 18;
    }
    if (part != NULL) {
        unit2->currentTarget = part;
    }
    damage = 0;
    if (!(weapon->special & 0x20000) && BtlUnit_GetACPossibility(unit2) != 0 && BattleActionCommandGetDefenceResult() == 5) {
        EffectEntry* eff;

        BtlUnit_GetHitPos(unit2, part, &posC.x, &posC.y, &posC.z);
        eff = effNiceEntry(2, posC.x - 60.0f, 45.0f + posC.y, posC.z - 10.0f);
        ((f32*)eff->userdata)[7] = 0.75f;
        flags |= 0x80000;
        psndSFXOn("SFX_SYSTEM_GREAT1");
        BattleAudience_Case_GuardGood();
        BU_S32(unit1, 0x27C)++;
        BU_S8(unit2, 0x26F) = BU_S32(unit1, 0x27C);
        switch (weapon->superguardType) {
        case 2:
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, (u8)weapon->field_0x6D, 0);
            return 21;
        case 3:
            flags |= 0x100;
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, (u8)weapon->field_0x6D, 0);
            return 21;
        case 4:
            flags |= 0x100;
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, (u8)weapon->field_0x6D, 0);
            if (BU_U32(unit1, 0x2CC) != 0) {
                BattleRunHitEventDirect(unit1, 0x137, (void*)BU_U32(unit1, 0x2CC));
            } else {
                BattleDamageDirect(-5, unit1, 0, 1, 0, 0x137, 0, 1);
            }
            return -1;
        default:
        case 1:
            flags |= 0x100;
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, (u8)weapon->field_0x6D, 0);
            BattleDamageDirect(-5, unit1, 0, 1, 0, 0x137, 0, 1);
            return -1;
        }
    } else {
        BattleCheckCounter(&counter, unit1, unit2, part, weapon, &flags);
        while (counter.field_0x0 != 0) {
            if (flags & 0x100) {
                if (part->counterAttributes & 0x2000) {
                    BattleRunHitEvent(unit2, 0x719);
                    result = 18;
                    break;
                }
                if ((part->counterAttributes & 0x1000) &&
                    (weapon->element == 3 || weapon->element == 4 || weapon->element == 1 ||
                     ((weapon->special & 0x400) && BtlUnit_CheckStatus(unit1, 8) != 0))) {
                    BattleRunHitEvent(unit2, 0x719);
                    result = 18;
                    break;
                }
            }
            if (!(weapon->special & 0x20000) && BtlUnit_GetACPossibility(unit2) != 0) {
                if (BattleActionCommandGetDefenceResult() > 3) {
                    EffectEntry* eff;
                    Vec pos;

                    BtlUnit_GetHitPos(unit2, part, &pos.x, &pos.y, &pos.z);
                    eff = effNiceEntry(0, pos.x - 60.0f, 45.0f + pos.y, pos.z - 10.0f);
                    ((f32*)eff->userdata)[7] = 0.75f;
                    flags |= 0x40000;
                    psndSFXOn("SFX_SYSTEM_NICE1");
                    BattleAudience_Case_GuardGood();
                } else {
                    BattleAudience_Case_GuardBad();
                }
            }
            if ((weapon->special & 0x1000) && (part->attributes & 0x1000)) {
                guarded = 1;
                flags |= 0x200000;
            }
            if ((weapon->special & 0x2000) && (part->attributes & 0x2000)) {
                guarded = 1;
                flags |= 0x200000;
            }
            if ((weapon->special & 0x4000) && (part->attributes & 0x800)) {
                guarded = 1;
                if (flags & 0x100) {
                    flags |= 0x400000;
                }
            }
            BattleCheckPikkyoro(weapon, &flags);
            damage = BattleCalculateDamage(unit1, unit2, part, weapon, &damageCode, flags);
            fpDamage = BattleCalculateFpDamage(unit1, unit2, part, weapon, &damageCode, flags);
            unit2->hpDamageCount++;
            if (!(flags & 0x40000)) {
                if (flags & 0x40000000) {
                    statusResult = BattleSetStatusDamageFromWeapon(unit1, unit2, part, weapon, flags);
                } else if (flags & 0x100) {
                    statusResult = BattleSetStatusDamageFromWeapon(unit1, unit2, part, weapon, flags);
                }
            }
            if ((u8)damageCode == 0x1E && guarded != 0) {
                damageCode = flags | 0x17;
            }
            if ((u8)damageCode == 0x1E && ((u8)weapon->field_0x6D == 0x13 || (u8)weapon->field_0x6D == 0x1B)) {
                damageCode = flags | 0x17;
            }
            if ((u8)damageCode == 0x1E && statusResult != 0) {
                if (statusResult & 1) {
                    damageCode = flags | 0x17;
                } else {
                    damageCode = flags | 0x16;
                }
            }
            if (statusResult & 4) {
                damageCode = flags | 0x1C;
            }
            if (statusResult & 8) {
                damageCode = flags | 0x1D;
            }
            if ((flags & 0x100) != 0 && (statusResult & 0x10)) {
                dc1 = 0x717;
                unit2->damagePattern = 0;
                _checkDamageCode_EmergencyRevival(unit2, &dc1);
                BattleRunHitEvent(unit2, dc1);
                result = 18;
            } else if ((flags & 0x100) != 0 && (weapon->special & 0x80) && BtlUnit_CheckStatus(unit2, 9) != 0
                       && irand(100) < 0) {
                dc2 = 0x8717;
                _checkDamageCode_EmergencyRevival(unit2, &dc2);
                BattleRunHitEvent(unit2, dc2);
                result = 18;
            } else {
                s32 var;

                if (statusResult == 0 && damage == 0 && fpDamage == 0 && guarded == 0) {
                    result = 19;
                    if (unit2->currentKind >= 1 && unit2->currentKind < 0xD8 && (u8)weapon->field_0x6D != 0x1B) {
                        BattleAudience_Case_EnemyNoDamage();
                    }
                } else {
                    result = 18;
                    if (unit2->currentKind >= 1 && unit2->currentKind < 0xD8 && (damage > 0 || statusResult != 0)) {
                        ((void (*)(s32))BattleAudience_Case_EnemyDamage)(damage);
                    }
                }
                var = weapon->field_0x6D;
                if (var == 0x14 && counter.field_0x24 != 0) {
                    var = 0;
                }
                if (var == 0xE || var == 0x10) {
                    if (unit2->mAttributeFlags & 0x4000) {
                        var = 0;
                    }
                    if (irand(100) >= unit2->mKindParams->field_0x8C) {
                        var = 0;
                    }
                    if (!(flags & 0x20000)) {
                        var = 0;
                    }
                }
                if (statusResult & 0x20) {
                    BattleDamageDirect(unit1->unitId, unit2, (s32)part, damage, fpDamage, damageCode, var, 0);
                } else {
                    BattleDamageDirect(unit1->unitId, unit2, (s32)part, damage, fpDamage, damageCode, var, 1);
                }
            }
            break;
        }
        BU_S32(unit1, 0x27C)++;
        BU_S8(unit2, 0x26F) = BU_S32(unit1, 0x27C);
        if (counter.field_0x24 != 0) {
            s32 w1;
            s32 w2;
            s32 w3;
            s32 w4;

            if (counter.field_0x4 != 0) {
                counter.field_0x28 += (damage * 50) / 100;
            }
            if (counter.field_0x18 != 0) {
                counter.field_0x28 += (damage * 50) / 100;
            }
            if (counter.field_0x1C != 0) {
                counter.field_0x28 += (damage * 50) / 100;
            }
            if (counter.field_0xC != 0) {
                w1 = 0;
                BattleSetStatusDamage(&w1, unit1, part, 0, 4, 100, 0, 3, 1);
            }
            if (counter.field_0x14 != 0) {
                w2 = 0;
                BattleSetStatusDamage(&w2, unit1, part, 0, 9, 100, 0, 3, 1);
            }
            if (counter.field_0x8 != 0) {
                w3 = 0;
                BattleSetStatusDamage(&w3, unit1, part, 0, 8, 100, 0, 3, 1);
            }
            if (counter.field_0x10 != 0) {
                w4 = 0;
                BattleSetStatusDamage(&w4, unit1, part, 0, 6, 100, 0, 3, 1);
            }
            if (counter.field_0x28 == 0) {
                counter.field_0x28 = 1;
            }
            if (unit1->mAlliance == 0) {
                BattleAudience_Case_Countered();
            }
            if (counter.field_0x18 != 0) {
                BtlUnit_GetHitPos(unit2, part, &posA.x, &posA.y, &posA.z);
                effBombEntry(0, posA.x, posA.y, posA.z, 1.0f);
                psndSFXOn_3D("SFX_BTL_THUNDERS_ATTACK4", &posA);
                BattleFogForceStop();
            }
            BattleActionCommandStop(wp);
            BattleAcHelpSetDispType(0);
            BattleDamageDirect(-5, unit1, 0, counter.field_0x28, 0, counter.field_0x24, 0, 1);
            if (counter.field_0x2C != 0) {
                unit2->damagePattern = 0;
                BattleRunHitEvent(unit2, counter.field_0x2C);
            }
            return -1;
        }
        return result;
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

// The original loads the 0.5f multiplier from its own .sdata2 symbol; MATCH_F32 keeps the compiler
// from folding it into an anonymous pooled literal (see matching.h).
static const f32 float_0p5_8041bedc = 0.5f;

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
        rate = (s32)((f32)rate * MATCH_F32(float_0p5_8041bedc));
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

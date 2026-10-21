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
#include "battle/battle_actrecord.h"
#include "eff/eff_damage_star.h"
#include "eff/eff_fukidashi.h"
#include "eff/eff_hit.h"
#include "eff/eff_miss_star.h"
#include "eff/eff_recovery.h"
#include "eff/n64/eff_ice_n64.h"
#include "eff/n64/eff_kemuri12_n64.h"

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
static inline u8 getAllOrNothing(BattleUnit* unit);

#define PART ((BattleUnitPart*)partArg)
// The .rodata displacements (the i2f double at 0x58 and the SFX_MARIO_HAMMER_PIKKYO strings at 0x290) only
// match when BattlePreCheckDamage is emitted after BattleCheckDamage, so its i2f double pools after the strings.
void BattleDamageDirect(s32 damageCode, BattleUnit* unit, s32 partArg, s32 damage, s32 arg4, s32 flags,
                        s32 arg6, s32 arg7) {
    BattleUnit* attacker;
    BattleWork* wp = _battleWorkPointer;
    Vec sfxPos;
    f32 effectX, effectY, effectZ;
    f32 fukidashiHitX, fukidashiHitY, fukidashiHitZ;
    f32 missHitX, missHitY, missHitZ;
    f32 recoverHitX, recoverHitY, recoverHitZ;
    f32 recoverX, recoverY, recoverZ;
    f32 fukidashiX, fukidashiY, fukidashiZ;
    s32 recovered;
    s32 count;
    s32 dir;
    u32 t;

    if (damageCode != -5) {
        attacker = BattleGetUnitPtr(wp, damageCode);
    } else {
        attacker = NULL;
    }
    if (damage > 99) {
        damage = 99;
    } else if (damage < -99) {
        damage = -99;
    }
    if (attacker != NULL) {
        attacker->field_0x280 += damage;
    }
    if (PART != NULL) {
        unit->currentTarget = PART;
    } else {
        BattleUnitPart* tmp = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        unit->currentTarget = tmp;
        partArg = (s32)tmp;
    }
    unit->damagePattern = arg6;
    if (unit->mAttributeFlags & 0x200000) {
        t = unit->damagePattern;
        if (t == 0x15 || (t - 0x16) <= 1 || t == 0x18) {
            unit->damagePattern = 0;
        }
    }
    BtlUnit_GetHitPos(unit, PART, &sfxPos.x, &sfxPos.y, &sfxPos.z);

    if (!(flags & 0x2000)) {
        if (damage != 0 || arg4 != 0) {
            BtlUnit_GetHitPos(unit, PART, &effectX, &effectY, &effectZ);
            switch ((u8)flags) {
            case 0x18:
            case 0x19:
                effKemuri12N64Entry(0, 30, effectX, effectY, effectZ, 1.0f);
                effHitEntry(1, 3, effectX, effectY, effectZ, 1.0f);
                break;
            case 0x1b:
                effHitEntry(2, 3, effectX, effectY, effectZ, 1.0f);
                break;
            case 0x1a:
                effIceN64Entry(0, effectX, effectY, effectZ);
                effHitEntry(3, 3, effectX, effectY, effectZ, 1.0f);
                break;
            default:
                effHitEntry(0, 3, effectX, effectY, effectZ, 1.0f);
                break;
            }
            if (BtlUnit_GetBelong(unit) == 0) {
                if (damage != 0 && !(unit->mAttributeFlags & 0x400000)) {
                    effDamageStarEntry(0, damage, effectX - 8.0f, 5.0f + effectY, effectZ, 10.0f, 30.0f);
                }
                if (arg4 != 0) {
                    if (damage != 0) {
                        effDamageStarEntry(1, arg4, effectX - 8.0f, 5.0f + effectY, effectZ, 10.0f, 60.0f);
                    } else {
                        effDamageStarEntry(1, arg4, effectX - 8.0f, 5.0f + effectY, effectZ, 10.0f, 30.0f);
                    }
                }
            } else {
                if (damage != 0 && !(unit->mAttributeFlags & 0x400000)) {
                    effDamageStarEntry(0, damage, effectX, 10.0f + effectY, effectZ, 10.0f, -30.0f);
                }
                if (arg4 != 0) {
                    if (damage != 0) {
                        effDamageStarEntry(1, arg4, effectX, 10.0f + effectY, effectZ, 10.0f, -60.0f);
                    } else {
                        effDamageStarEntry(1, arg4, effectX, 10.0f + effectY, effectZ, 10.0f, -30.0f);
                    }
                }
            }
            if (damageCode != -5 && attacker != NULL && attacker->badgesEquipped.MegaRush != 0 &&
                attacker->currentHp <= attacker->mKindParams->mPerilHP) {
                BtlUnit_GetHitPos(unit, PART, &fukidashiHitX, &fukidashiHitY, &fukidashiHitZ);
                effFukidashiEntry(0, 0, 0, 60, fukidashiHitX, fukidashiHitY, fukidashiHitZ, 10.0f, 0.0f);
            }
        } else {
            BtlUnit_GetHitPos(unit, PART, &missHitX, &missHitY, &missHitZ);
            if ((u32)arg6 != 2 && (u32)arg6 != 3 && (u32)arg6 != 0x15 && (u32)arg6 != 0x16 &&
                (u32)arg6 != 0x14 && (u32)arg6 != 0x13 && (u32)arg6 != 0x1b && (u32)arg6 != 7 &&
                (u8)flags != 0x16) {
                dir = BtlUnit_GetBelong(unit) == 0 ? -1 : 1;
                if (flags & 0x20000) {
                    effMissStarEntry(1, missHitX, missHitY, missHitZ, 1, dir);
                } else {
                    effMissStarEntry(0, missHitX, missHitY, missHitZ, 1, dir);
                }
            }
        }
    } else {
        BtlUnit_GetHitPos(unit, PART, &recoverHitX, &recoverHitY, &recoverHitZ);
        effRecoveryEntry(0, (s32)__fabs(damage), recoverHitX, recoverHitY, recoverHitZ);
    }

    if (!(flags & 0x2000) && damage > 0) {
        count = unit->badgesEquipped.PityFlower;
        recovered = 0;
        while (count > 0) {
            if (irand(100) < 30) {
                recovered++;
            }
            count--;
        }
        if (recovered > 0) {
            BtlUnit_RecoverFp(unit, recovered);
            BtlUnit_GetPos(unit, &recoverX, &recoverY, &recoverZ);
            recoverY = recoverY + (unit->mSizeMultiplier * BtlUnit_GetHeight(unit) + 5.0f);
            effRecoveryEntry(1, recovered, recoverX, recoverY, recoverZ);
        }
    }

    if ((u32)arg6 != 0x13 && (u32)arg6 != 0x1b) {
        if (flags & 0x1000000) {
            psndSFXOn_3D("SFX_MARIO_HAMMER_PIKKYO_R1", &sfxPos);
        } else if (flags & 0x2000000) {
            psndSFXOn_3D("SFX_MARIO_HAMMER_PIKKYO_Y1", &sfxPos);
        } else if (flags & 0x4000000) {
            psndSFXOn_3D("SFX_MARIO_HAMMER_PIKKYO_B1", &sfxPos);
        } else if (flags & 0x8000000) {
            psndSFXOn_3D("SFX_MARIO_HAMMER_PIKKYO_G1", &sfxPos);
        } else if (flags & 0x10000000) {
            psndSFXOn_3D("SFX_MARIO_HAMMER_PIKKYO_P1", &sfxPos);
        } else if (BtlUnit_GetBelong(unit) == 0 && (flags & 0x40000)) {
            psndSFXOn_3D("SFX_MARIO_GUARD1", &sfxPos);
        } else {
            if (damage > 0) {
                switch ((u8)flags) {
                case 0x18:
                    psndSFXOn_3D(unit->mKindParams->fireDamageSfxName, &sfxPos);
                    psndSFXOn_3D(unit->mKindParams->damageSfxName, &sfxPos);
                    break;
                case 0x1a:
                    psndSFXOn_3D(unit->mKindParams->iceDamageSfxName, &sfxPos);
                    psndSFXOn_3D(unit->mKindParams->damageSfxName, &sfxPos);
                    break;
                case 0x1b:
                    psndSFXOn_3D(unit->mKindParams->electricDamageSfxName, &sfxPos);
                    psndSFXOn_3D(unit->mKindParams->damageSfxName, &sfxPos);
                    break;
                default:
                    psndSFXOn_3D(unit->mKindParams->damageSfxName, &sfxPos);
                    break;
                }
            }
            if (arg4 > 0 && BtlUnit_GetBelong(unit) == 0) {
                psndSFXOn_3D("SFX_MARIO_DAMAGE_FP1", &sfxPos);
            }
        }
    }

    unit->hpDamageTaken = damage;
    unit->totalHpDamageTaken += unit->hpDamageTaken;
    unit->fpDamageTaken = arg4;
    unit->field_0x26E = arg4;
    unit->field_0x264 += unit->fpDamageTaken;

    if (!(unit->mAttributeFlags & 0x40000000) && !(PART->attributes & 0x800000)) {
        unit->hpGauge.field_0xC = unit->currentHp;
        unit->currentHp = unit->currentHp - unit->hpDamageTaken;
        if (unit->currentHp > unit->maxHp) {
            unit->currentHp = unit->maxHp;
        }
        if (unit->currentHp < 0) {
            unit->currentHp = 0;
        }
        unit->hpGauge.field_0x10 = unit->currentHp;
        if (unit->hpDamageTaken != 0) {
            unit->hpGauge.field_0x4 = unit->hpGauge.field_0x6;
            unit->hpGauge.field_0x8 = unit->hpGauge.field_0xA;
        }
        if (flags & 0x100) {
            unit->hpGauge.field_0x2 |= 1;
        } else {
            unit->hpGauge.field_0x2 &= ~1;
        }
    }
    if (unit->fpDamageTaken != 0) {
        s32 fp = BtlUnit_GetFp(unit);

        if (fp < arg4) {
            unit->field_0x26E = fp;
        } else {
            unit->field_0x26E = arg4;
        }
        fp = fp - arg4;
        if (fp < 0) {
            fp = 0;
        }
        BtlUnit_SetFp(unit, fp);
        unit->field_0x268 += unit->field_0x26E;
    }
    if (damage > 0) {
        if (unit->currentKind == 0xde) {
            BtlActRec_AddCount(wp->field_16F1C + 6);
            BtlActRec_AddPoint(wp->field_16F1C + 4, (u8)damage);
        } else if (unit->currentKind >= 0xe0 && unit->currentKind < 0xe7) {
            BtlActRec_AddCount(wp->field_16F1C + 7);
            BtlActRec_AddPoint(wp->field_16F1C + 5, (u8)damage);
        }
    }
    if (arg7 != 0 && (damage > 0 || arg4 > 0)) {
        if (BtlUnit_CheckStatus(unit, STATUS_SLEEP) != 0 && irand(100) < 50) {
            unit->mSleepTurns = 1;
            unit->mSleepStrength = 100;
            BtlUnit_CheckRecoveryStatus(unit, STATUS_SLEEP);
            BtlUnit_GetPos(unit, &fukidashiX, &fukidashiY, &fukidashiZ);
            fukidashiY = fukidashiY + BtlUnit_GetHeight(unit) / 2;
            effFukidashiEntry(0, 0, 0, 60, fukidashiX, fukidashiY, fukidashiZ, 10.0f, 0.0f);
        }
    }
    if (unit->currentKind == 0xde) {
        BattleAudience_Case_MarioBigDamage(unit->hpDamageTaken);
    }
    if (unit->currentHp <= 0) {
        unit->currentHp = 0;
        if (flags & 0x100) {
            flags |= 0x200;
            if (unit->mAttributeFlags & 0x10000) {
                switch ((u8)flags) {
                case 0x18:
                case 0x19:
                case 0x2e:
                case 0x31:
                case 0x32:
                case 0x36:
                    flags |= 0x400;
                    break;
                }
            } else {
                flags |= 0x400;
                _checkDamageCode_EmergencyRevival(unit, &flags);
            }
            if ((u32)unit->damagePattern == 0x19) {
                s32 clear = 1;

                if ((flags & 0x400) && !(flags & 0x800) && (flags & 0x100)) {
                    if (unit->vulnStatus->galeForceRate != 0) {
                        clear = 0;
                    }
                }
                if (clear) {
                    unit->damagePattern = 0;
                }
            }
            if (flags & 0x400) {
                if (unit->currentKind >= 0xe0 && unit->currentKind < 0xe7) {
                    BattleAudience_Case_PartyDown();
                }
                if (unit->currentKind >= 1 && unit->currentKind < 0xd8) {
                    BattleAudience_Case_KillEnemy(unit->mLevel);
                    BattleAudience_Case_FinalAttack();
                }
            }
        }
    } else {
        if ((u32)unit->damagePattern == 0x19) {
            unit->damagePattern = 0;
        }
    }
    unit->mTokenFlags |= 1;
    BattleRunHitEvent(unit, flags);
}
#undef PART

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
    if (unit1 != 0 && (weapon->special & 1) && getAllOrNothing(unit1) != 0) {
        if (!(wp->actionCommands.result & 2) && !(flags & 0x20000)) {
            return 0;
        }
    }
    damage = weapon->hpDamageWork[0];
    if (weapon->hpDamageFn != 0) {
        damage = weapon->hpDamageFn(unit1, weapon, unit2, part);
    }
    if (wp->field_18FE1 == 1 && (weapon->special & 2)) {
        damage += 3;
    }
    if (weapon->special & 1) {
        u8 allOrNothing = getAllOrNothing(unit1);

        damage += unit1->badgesEquipped.PowerPlus;
        damage += unit1->badgesEquipped.PUpDDown;
        if (allOrNothing != 0) {
            if ((wp->actionCommands.result & 2) || (flags & 0x20000)) {
                damage += allOrNothing;
            } else {
                damage = 0;
                goto calculate_defense; // skips every later attack modifier: no structured form without a flag
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
        if (wp->field_18FE1 == 2 && unit2->currentKind == 0xde) {
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
            result -= unit1->field_0x27C;
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
static inline u8 getAllOrNothing(BattleUnit* unit) {
    return unit->badgesEquipped.AllOrNothing;
}

// dont_inline: BattleCheckDamage calls this out of line in the original.
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
        damage = weapon->fpDamageFn(attacker, weapon, target, part);
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


s32 _getSickStatusRate(BattleWeapon* weapon, s32 type) {
    switch (type) {
    case STATUS_SLEEP: return weapon->sleepChance;
    case STATUS_STOP: return weapon->stopChance;
    case STATUS_DIZZY: return weapon->dizzyChance;
    case STATUS_POISON: return weapon->poisonChance;
    case STATUS_CONFUSE: return weapon->confuseChance;
    case STATUS_ELECTRIC: return weapon->electricChance;
    case STATUS_DODGY: return weapon->dodgyChance;
    case STATUS_BURN: return weapon->burnChance;
    case STATUS_FREEZE: return weapon->freezeChance;
    case STATUS_HUGE:
        if (weapon->sizeChangeStrength > 0) {
            return weapon->sizeChangeChance;
        }
        return 0;
    case STATUS_TINY:
        if (weapon->sizeChangeStrength <= 0) {
            return weapon->sizeChangeChance;
        }
        return 0;
    case STATUS_ATTACK_UP:
        if (weapon->atkChangeStrength > 0) {
            return weapon->atkChangeChance;
        }
        return 0;
    case STATUS_ATTACK_DOWN:
        if (weapon->atkChangeStrength <= 0) {
            return weapon->atkChangeChance;
        }
        return 0;
    case STATUS_DEFENSE_UP:
        if (weapon->defChangeStrength > 0) {
            return weapon->defChangeChance;
        }
        return 0;
    case STATUS_DEFENSE_DOWN:
        if (weapon->defChangeStrength <= 0) {
            return weapon->defChangeChance;
        }
        return 0;
    case STATUS_ALLERGIC: return weapon->allergicChance;
    case STATUS_FAST: return weapon->fastChance;
    case STATUS_SLOW: return weapon->slowChance;
    case STATUS_FRIGHT: return weapon->frightChance;
    case STATUS_GALE_FORCE: return weapon->galeForceChance;
    case STATUS_INSTANT_KILL: return weapon->ohkoChance;
    case STATUS_INVISIBLE: return weapon->invisibleChance;
    case STATUS_HP_REGEN: return weapon->hpRegenTime > 0 ? 100 : 0;
    case STATUS_FP_REGEN: return weapon->fpRegenTime > 0 ? 100 : 0;
    case STATUS_CHARGE: return weapon->chargeStrength != 0 ? 100 : 0;
    case STATUS_PAYBACK: return weapon->paybackTime != 0 ? 100 : 0;
    case STATUS_HOLD_FAST: return weapon->holdFastTime > 0 ? 100 : 0;
    case STATUS_FLIPPED: return 0;
    }
    return 0;
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
        // The original sign-extends ohkoChance here (a param, like the s8 times), while _getSickStatusRate reads
        // it unsigned.
        param1 = param2 = *(s8*)&weapon->ohkoChance;
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


// The stamp effect's two TEV colours: GX_TEVREG0 (r0, g0, b0) and GX_TEVREG1 (r1, g1, b1).
#define STAMP_COLOR(e, r0, g0, b0, r1, g1, b1)                           \
    {                                                                    \
        ((effStampData*)(e)->userdata)->tevReg0R = r0;                 \
        ((effStampData*)(e)->userdata)->tevReg0G = g0;                 \
        ((effStampData*)(e)->userdata)->tevReg0B = b0;                 \
        ((effStampData*)(e)->userdata)->tevReg1R = r1;                 \
        ((effStampData*)(e)->userdata)->tevReg1G = g1;                 \
        ((effStampData*)(e)->userdata)->tevReg1B = b1;                 \
    }

// Applies one status (sleep, stop, poison, ...) to `unit` from an attack of `part`'s side: the immunity and
// resist checks, the roll against `rate`, the status itself, the hit effect and the announcement. `*work` collects
// what happened (1/2 got/lost a status, 4/8 fright/gale force, 0x10 instant kill, 0x20 sleep).
#pragma opt_common_subs off // without it the callee-saved registers come out in the wrong order (unit/part in r30/r31)
// The i2f double (double_to_int_802edad8) and the SFX_CONDITION strings this function loads through r31 must
// sit at 0x58/0x60: the double is pooled by BattlePreCheckDamage, so that function has to come after
// BattleCheckDamage (which pools the SFX_SYSTEM strings) in source order, i.e. before it in .text. With the
// functions in the original .text order this function is byte-identical
s32 BattleSetStatusDamage(s32* work, BattleUnit* unit, BattleUnitPart* part, s32 weaponSpecial, u32 type,
                          s32 rate, s32 levelDiff, s8 turns, s8 strength) {
    u8 chargeStrength;
    u8 sp9;
    u8 sp8;
    f32 x, y, z;
    Vec hit;
    s32 isRemoving;
    s32 applied;
    s32 announced;
    u32 resistFlag;
    s32 resist;
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
        applied = BtlUnit_SetStatus(unit, type, turns, strength);
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
        case STATUS_DEFENSE_UP:
        case STATUS_DEFENSE_DOWN:
            effUpdownEntry(1, x, y, 20.0f + z, strength, 60);
            break;
        case STATUS_HUGE:
        case STATUS_TINY:
        case STATUS_ATTACK_UP:
        case STATUS_ATTACK_DOWN:
        case STATUS_CHARGE:
            effUpdownEntry(0, x, y, 15.0f + z, strength, 60);
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
        BtlUnit_GetStatus(unit, type, (s8*)&sp9, (s8*)&sp8);
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
                BattleStatusChangeInfoSetAnnouce(unit, type, (s8)sp9, 1);
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
#pragma opt_common_subs reset


// The `s32 rate` plus `(s8)` casts reproduce the original `extsb r8,r3` / `cmpwi r8,0` (a plain s8
// rate makes the GALE_FORCE test `extsb. r0,r8`). The jump-table reloc name differs; objdiff ignores it.
// dont_inline: BattleCheckDamage calls this out of line in the original.
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
            if (BattleSetStatusDamage(&work, unit2, part, weapon->special, i, (s8)rate, levelDiff, p1, p2)) {
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
    rate = weapon->baseAccuracy;
    if (BtlUnit_CheckStatus(attacker, 3)) {
        rate = rate * 0x32 / 100;
    }
    if (wp->field_180C0.field_0x1E0 != NULL) {
        rate = (s32)((f32)rate * MATCH_F32(float_0p5_8041bedc));
    }
    if (irand(100) >= rate) {
        return 2;
    }
    return 1;
}

// dont_inline: BattleCheckDamage calls this out of line in the original.
#pragma dont_inline on
s32 _checkDamageCode_EmergencyRevival(BattleUnit* unit, s32* flags) {
    if (unit->currentKind == UNIT_MARIO) {
        if (pouchCheckItem(ITEM_LIFE_SHROOM) > 0) {
            *flags |= 0x800;
            return 1;
        }
        return 0;
    }
    if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
        if (pouchCheckItem(ITEM_LIFE_SHROOM) > 0) {
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

// The shapes here that reproduce the original's scheduling and block layout:
//  - `damage = 0;` is a plain assignment placed after the `part` block (not an initializer); the original
//    hoists it to between the `weapon->special` load and its test, which is also where `damage` (r22) first
//    goes live.
//  - the counter body is a `while (work.hitLands) { ... break; }` so the compiler emits the original's
//    bottom-tested block (test after the body, `bne` back into it).
//  - the tail is `if (work.damageCode != 0) { ... return -1; } return ret;` so the shared `return ret`
//    lands out of line at the end like the original.
//  - the three weaponField6D==0xe/0x10 tests are independent `if`s: `irand(100)` runs unconditionally.
//  - the second emergency-revival block does NOT clear `unit2->damagePattern`; only the first one does.
//  - `((u32)flags & 0x100)` and the `(u32)work.damageCode/hitEventCode` casts give the original's `cmplwi`s.
//  - the status-damage flag test is `flags & 0x40000000` (IBM bit 1), not `flags & 2`.
//
// The four helpers it calls (`BattleInitCounterPreCheckWork`, `_checkDamageCode_EmergencyRevival`,
// `BattleCalculateFpDamage`, `BattleSetStatusDamageFromWeapon`) are defined earlier in this file and each
// needs its `dont_inline` pragma so the original's real `bl`s survive.
#ifdef MUST_MATCH
// BattleCheckDamage's three literals. Declaring them here pools them at parse time, ahead of the i2f double
// BattlePreCheckDamage pools, which is the original's .rodata order (zero_802eda80, then these, then the
// double). Under #else the calls pass the plain literals.
static const char _sfxGreat1[20] = "SFX_SYSTEM_GREAT1";
static const char _sfxNice1[20] = "SFX_SYSTEM_NICE1";
static const char _sfxThunders[28] = "SFX_BTL_THUNDERS_ATTACK4";
#define SFX_STR_GREAT1 _sfxGreat1
#define SFX_STR_NICE1 _sfxNice1
#define SFX_STR_THUNDERS _sfxThunders
#else
#define SFX_STR_GREAT1 "SFX_SYSTEM_GREAT1"
#define SFX_STR_NICE1 "SFX_SYSTEM_NICE1"
#define SFX_STR_THUNDERS "SFX_BTL_THUNDERS_ATTACK4"
#endif

s32 BattleCheckDamage(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon, s32 flags) {
    BattleWork* wp = _battleWorkPointer;
    CounterPreCheckWork work;
    s32 damageCode;
    s32 ret = -1;
    u32 statusDamage = 0;
    s32 damage;
    s32 fpDamage;
    s32 isCounter = 0;

    CounterPreCheckWork* workp = &work;
    BattleInitCounterPreCheckWork(workp);
    if (unit2 == NULL) {
        return 0x12;
    }
    if (part) {
        unit2->currentTarget = part;
    }
    damage = 0;

    if (!(weapon->special & 0x20000) && BtlUnit_GetACPossibility(unit2) != 0 &&
        BattleActionCommandGetDefenceResult() == 5) {
        Vec pos;
        EffectEntry* eff;
        BtlUnit_GetHitPos(unit2, part, &pos.x, &pos.y, &pos.z);
        eff = effNiceEntry(2, pos.x - 60.0f, 45.0f + pos.y, pos.z - 10.0f);
        ((NiceEffectWork*)eff->userdata)->scale = 0.75f;
        flags |= 0x80000;
        psndSFXOn(SFX_STR_GREAT1);
        BattleAudience_Case_GuardGood();
        unit1->field_0x27C += 1;
        unit2->field_0x26F = unit1->field_0x27C;
        switch (weapon->superguardType) {
        case 2:
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, weapon->field_0x6D, 0);
            return 0x15;
        case 3:
            flags |= 0x100;
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, weapon->field_0x6D, 0);
            return 0x15;
        case 4:
            flags |= 0x100;
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, weapon->field_0x6D, 0);
            if (unit1->field_0x2CC != NULL) {
                BattleRunHitEventDirect(unit1, 0x137, unit1->field_0x2CC);
            } else {
                BattleDamageDirect(-5, unit1, 0, 1, 0, 0x137, 0, 1);
            }
            return -1;
        case 1:
        default:
            flags |= 0x100;
            damageCode = flags | 0x2A;
            BattleDamageDirect(unit1->unitId, unit2, (s32)part, 0, 0, damageCode, weapon->field_0x6D, 0);
            BattleDamageDirect(-5, unit1, 0, 1, 0, 0x137, 0, 1);
            return -1;
        }
    }

    BattleCheckCounter(workp, unit1, unit2, part, weapon, &flags);
    while (work.hitLands) {
        if (flags & 0x100) {
            if (part->counterAttributes & kCounterVolatileExplosive) {
                BattleRunHitEvent(unit2, 0x719);
                ret = 0x12;
                break;
            }
            if (part->counterAttributes & kCounterExplosive) {
                if (weapon->element == 3 || weapon->element == 4 || weapon->element == 1 ||
                    ((weapon->special & 0x400) && BtlUnit_CheckStatus(unit1, STATUS_BURN))) {
                    BattleRunHitEvent(unit2, 0x719);
                    ret = 0x12;
                    break;
                }
            }
        }
        if (!(weapon->special & 0x20000) && BtlUnit_GetACPossibility(unit2) != 0) {
            if (BattleActionCommandGetDefenceResult() > 3) {
                Vec pos;
                EffectEntry* eff;
                BtlUnit_GetHitPos(unit2, part, &pos.x, &pos.y, &pos.z);
                eff = effNiceEntry(0, pos.x - 60.0f, 45.0f + pos.y, pos.z - 10.0f);
                ((NiceEffectWork*)eff->userdata)->scale = 0.75f;
                flags |= 0x40000;
                psndSFXOn(SFX_STR_NICE1);
                BattleAudience_Case_GuardGood();
            } else {
                BattleAudience_Case_GuardBad();
            }
        }
        if ((weapon->special & 0x1000) && (part->attributes & 0x1000)) {
            isCounter = 1;
            flags |= 0x200000;
        }
        if ((weapon->special & 0x2000) && (part->attributes & 0x2000)) {
            isCounter = 1;
            flags |= 0x200000;
        }
        if ((weapon->special & 0x4000) && (part->attributes & 0x800)) {
            isCounter = 1;
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
                statusDamage = BattleSetStatusDamageFromWeapon(unit1, unit2, part, weapon, flags);
            } else if (flags & 0x100) {
                statusDamage = BattleSetStatusDamageFromWeapon(unit1, unit2, part, weapon, flags);
            }
        }
        if ((u8)damageCode == 0x1e && isCounter) {
            damageCode = flags | 0x17;
        }
        if ((u8)damageCode == 0x1e && (weapon->field_0x6D == 0x13 || weapon->field_0x6D == 0x1b)) {
            damageCode = flags | 0x17;
        }
        if ((u8)damageCode == 0x1e && statusDamage != 0) {
            if (statusDamage & 1) {
                damageCode = flags | 0x17;
            } else {
                damageCode = flags | 0x16;
            }
        }
        if (statusDamage & 4) {
            damageCode = flags | 0x1c;
        }
        if (statusDamage & 8) {
            damageCode = flags | 0x1d;
        }
        if (((u32)flags & 0x100) && (statusDamage & 0x10)) {
            s32 revCode = 0x717;
            unit2->damagePattern = 0;
            _checkDamageCode_EmergencyRevival(unit2, &revCode);
            BattleRunHitEvent(unit2, revCode);
            ret = 0x12;
            break;
        }
        if (((u32)flags & 0x100) && (weapon->special & 0x80) && BtlUnit_CheckStatus(unit2, STATUS_FREEZE)) {
            if (irand(100) < 0) {
                s32 revCode = 0x8717;
                _checkDamageCode_EmergencyRevival(unit2, &revCode);
                BattleRunHitEvent(unit2, revCode);
                ret = 0x12;
                break;
            }
        }
        if (statusDamage == 0 && damage == 0 && fpDamage == 0 && !isCounter) {
            ret = 0x13;
            if (unit2->currentKind >= 1 && unit2->currentKind < 0xd8 && weapon->field_0x6D != 0x1b) {
                BattleAudience_Case_EnemyNoDamage();
            }
        } else {
            ret = 0x12;
            if (unit2->currentKind >= 1 && unit2->currentKind < 0xd8) {
                if (damage > 0 || statusDamage != 0) {
                    BattleAudience_Case_EnemyDamage(damage);
                }
            }
        }
        {
            u32 weaponField6D = weapon->field_0x6D;
            if (weaponField6D == 0x14 && (u32)work.damageCode != 0) {
                weaponField6D = 0;
            }
            if (weaponField6D == 0xe || weaponField6D == 0x10) {
                if (unit2->mAttributeFlags & 0x4000) {
                    weaponField6D = 0;
                }
                if (irand(100) >= unit2->mKindParams->field_0x8C) {
                    weaponField6D = 0;
                }
                if (!(flags & 0x20000)) {
                    weaponField6D = 0;
                }
            }
            if (statusDamage & 0x20) {
                BattleDamageDirect(unit1->unitId, unit2, (s32)part, damage, fpDamage, damageCode, weaponField6D, 0);
            } else {
                BattleDamageDirect(unit1->unitId, unit2, (s32)part, damage, fpDamage, damageCode, weaponField6D, 1);
            }
        }
        break;
    }
    unit1->field_0x27C += 1;
    unit2->field_0x26F = unit1->field_0x27C;
    if ((u32)work.damageCode != 0) {
    if (work.halfDamage) {
        work.counterDamage += damage * 50 / 100;
    }
    if (work.holdFast) {
        work.counterDamage += damage * 50 / 100;
    }
    if (work.returnPostage) {
        work.counterDamage += damage * 50 / 100;
    }
    if (work.inflictPoison) {
        s32 statusWork = 0;
        BattleSetStatusDamage(&statusWork, unit1, part, 0, STATUS_POISON, 100, 0, 3, 1);
    }
    if (work.inflictFreeze) {
        s32 statusWork = 0;
        BattleSetStatusDamage(&statusWork, unit1, part, 0, STATUS_FREEZE, 100, 0, 3, 1);
    }
    if (work.inflictBurn) {
        s32 statusWork = 0;
        BattleSetStatusDamage(&statusWork, unit1, part, 0, STATUS_BURN, 100, 0, 3, 1);
    }
    if (work.inflictElectric) {
        s32 statusWork = 0;
        BattleSetStatusDamage(&statusWork, unit1, part, 0, STATUS_ELECTRIC, 100, 0, 3, 1);
    }
    if (work.counterDamage == 0) {
        work.counterDamage = 1;
    }
    if (unit1->mAlliance == 0) {
        BattleAudience_Case_Countered();
    }
    if (work.holdFast) {
        Vec pos;
        BtlUnit_GetHitPos(unit2, part, &pos.x, &pos.y, &pos.z);
        effBombEntry(0, pos.x, pos.y, pos.z, 1.0f);
        psndSFXOn_3D(SFX_STR_THUNDERS, &pos);
        BattleFogForceStop();
    }
    BattleActionCommandStop(wp);
    BattleAcHelpSetDispType(0);
    BattleDamageDirect(-5, unit1, 0, work.counterDamage, 0, work.damageCode, 0, 1);
    if ((u32)work.hitEventCode != 0) {
        unit2->damagePattern = 0;
        BattleRunHitEvent(unit2, work.hitEventCode);
    }
    return -1;
    }
    return ret;
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

void BattleAttackDeclareAll(BattleWork* wp) {
    s32 i;
    BattleSampling* sampling = &wp->sampling;

    for (i = 0; i < sampling->count; i++) {
        BattleSamplingEntry* entry = &sampling->entries[sampling->order[i]];
        s32 out1;
        s32 out2;

        __declare(entry->unitId, entry->partId, &out1, &out2);
        if (out1 != -1) {
            s32 j;

            for (j = 0; j < sampling->count; j++) {
                s16 matchId = sampling->entries[sampling->order[j]].unitId;
                if (matchId == out1) {
                    s32 k;

                    for (k = j; k < sampling->count - 1; k++) {
                        sampling->order[k] = sampling->order[k + 1];
                    }
                    if (j <= i) {
                        i--;
                    }
                    sampling->count--;
                    break;
                }
            }
            entry->unitId = out1;
            entry->partId = out2;
        }
    }
    sampling->selectIdx = 0;
}

// dont_inline: the original calls this out of line from BattleCheckDamage.
#pragma dont_inline on
void BattleInitCounterPreCheckWork(void* work) {
    memset(work, 0, 0x30);
    *(s32*)work = 1;
}
#pragma dont_inline reset



s32 BattleCheckCounter(void* work, BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part,
                       BattleWeapon* weapon, s32* out) {
    CounterPreCheckWork* w = (CounterPreCheckWork*)work;

    if (attacker == NULL) {
        return 0;
    }
    w->counterDamage = 0;

    if ((part->counterAttributes & kCounterTopSpiky) && attacker->badgesEquipped.SpikeShield == 0 &&
        !(weapon->counterResistanceFlags & 0x2)) {
        w->counterType = 0x2B;
        w->damageCode = 0x12B;
        w->counterDamage++;
        w->hitEventCode = 0x1F;
        *out |= 0x100;
        w->hitLands = 0;
    } else if ((part->counterAttributes & kCounterPreemptiveFrontSpiky) &&
               attacker->badgesEquipped.SpikeShield == 0 && !(weapon->counterResistanceFlags & 0x4)) {
        w->counterType = 0x2C;
        w->damageCode = 0x12C;
        w->counterDamage++;
        w->hitEventCode = 0x20;
        *out |= 0x100;
        w->hitLands = 0;
    } else if ((part->counterAttributes & kCounterFrontSpiky) && attacker->badgesEquipped.SpikeShield == 0 &&
               !(weapon->counterResistanceFlags & 0x8)) {
        w->counterType = 0x2D;
        w->damageCode = 0x12D;
        w->counterDamage++;
        w->hitEventCode = 0x21;
        *out |= 0x100;
        w->hitLands = 0;
    } else if (((part->counterAttributes & kCounterFiery) || (part->counterAttributes & kCounterFieryStatus)) &&
               attacker->badgesEquipped.IcePower == 0 && !(weapon->counterResistanceFlags & 0x10)) {
        w->counterType = 0x2E;
        w->damageCode = 0x12E;
        w->counterDamage++;
        w->hitEventCode = 0x22;
        *out |= 0x100;
        w->hitLands = 0;
        if (part->counterAttributes & kCounterFieryStatus) {
            w->inflictBurn = 1;
        }
    } else if (((part->counterAttributes & kCounterIcy) || (part->counterAttributes & kCounterIcyStatus)) &&
               !(weapon->counterResistanceFlags & 0x20)) {
        w->counterType = 0x2F;
        w->damageCode = 0x12F;
        w->counterDamage++;
        w->hitEventCode = 0x23;
        *out |= 0x100;
        w->hitLands = 0;
        if (part->counterAttributes & kCounterIcyStatus) {
            w->inflictFreeze = 1;
        }
    } else if (((part->counterAttributes & kCounterElectric) ||
                (part->counterAttributes & kCounterElectricStatus)) &&
               !(weapon->counterResistanceFlags & 0x1) &&
               BtlUnit_CheckStatus(attacker, STATUS_ELECTRIC) == 0) {
        w->counterType = 0x30;
        w->damageCode = 0x130;
        w->counterDamage++;
        w->hitEventCode = 0x24;
        *out |= 0x100;
        w->hitLands = 0;
        if (part->counterAttributes & kCounterElectricStatus) {
            w->inflictElectric = 1;
        }
    } else {
        if ((part->counterAttributes & kCounterExplosive) && !(weapon->counterResistanceFlags & 0x80) &&
            (weapon->element == 3 || weapon->element == 4 || weapon->element == 1 ||
             ((weapon->special & 0x400) && BtlUnit_CheckStatus(attacker, STATUS_BURN) != 0))) {
            w->counterType = 0x31;
            w->damageCode = 0x131;
            w->counterDamage++;
            *out |= 0x100;
        }
        if ((part->counterAttributes & kCounterVolatileExplosive) && !(weapon->counterResistanceFlags & 0x100)) {
            w->counterType = 0x32;
            w->damageCode = 0x132;
            w->counterDamage++;
            *out |= 0x100;
        }
        if ((BtlUnit_CheckStatus(target, STATUS_ELECTRIC) != 0 ||
             (part->counterAttributes & kCounterElectric) ||
             (part->counterAttributes & kCounterElectricStatus)) &&
            !(weapon->counterResistanceFlags & 0x1) &&
            BtlUnit_CheckStatus(attacker, STATUS_ELECTRIC) == 0) {
            w->counterType = 0x34;
            w->damageCode = 0x134;
            w->counterDamage++;
            *out |= 0x100;
            if (part->counterAttributes & kCounterElectricStatus) {
                w->inflictElectric = 1;
            }
        }
        if (BtlUnit_CheckStatus(target, STATUS_PAYBACK) != 0 && !(weapon->counterResistanceFlags & 0x200)) {
            w->counterType = 0x35;
            w->damageCode = 0x135;
            *out |= 0x100;
            w->halfDamage = 1;
        }
        if (((part->counterAttributes & kCounterPoison) || (part->counterAttributes & kCounterPoisonStatus)) &&
            !(weapon->counterResistanceFlags & 0x40)) {
            w->counterType = 0x33;
            w->damageCode = 0x133;
            *out |= 0x100;
            w->halfDamage = 1;
            if (part->counterAttributes & kCounterPoisonStatus) {
                w->inflictPoison = 1;
            }
        }
        if (BtlUnit_CheckStatus(target, STATUS_HOLD_FAST) != 0 && !(weapon->counterResistanceFlags & 0x400)) {
            w->counterType = 0x36;
            w->damageCode = 0x136;
            *out |= 0x100;
            w->holdFast = 1;
        }
        if (target->badgesEquipped.ReturnPostage != 0 && !(weapon->counterResistanceFlags & 0x200)) {
            w->counterType = 0x35;
            w->damageCode = 0x135;
            *out |= 0x100;
            w->returnPostage = 1;
        }
    }

    switch (w->counterType) {
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

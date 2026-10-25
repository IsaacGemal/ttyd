#include "unit_boss_batten_satellite.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[15];
s32 dmg_counter_event[149];
static BattleUnitKindPart parts[1];
static s32 init_event[30];
static s32 damage_event[10];
static s32 dead_event[2];
static s32 attack_event[10];
s32 batten_satellite_attack_attack_event[304];
static s32 wait_event[6];

// clang-format off
extend_data(160,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(161,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleWeapon weapon_batten_satellite_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x00000002,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleUnitKind unit_boss_batten_satellite = {
    UNIT_GRODUS_X,                 // mUnitType
    "btl_un_batten_satellite",     // name
    3,                             // maxHp
    0,                             // maxFp
    1,                             // mDangerHP
    1,                             // mPerilHP
    0,                             // mLevel
    0,                             // mBonusExp
    1,                             // bonusCoinRolls
    100,                           // bonusCoinRate
    0,                             // baseCoin
    50,                            // escapeRate
    9,                             // renzokuSoftCap
    16,                            // width
    16,                            // height
    { 0, 8 },                      // statusIconOffset
    { 0.0f, 0.0f, 0.0f },          // centerOffset
    { -3, -6 },                    // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },          // talkTogeBaseOffset
    { 8.0f, 0.0f, -10.0f },        // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // kagenukeOffsetX
    0.0f,                          // kagenukeOffsetZ
    { 8.0f, 0.0f, 0.0f },          // bintaHitOffset
    { 6.0f, 7.0f, 0.0f },          // kissHitOffset
    { -1.0f, 0.0f, 0.0f },         // cutBaseOffset
    16.0f,                         // cutWidth
    16.0f,                         // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    0,                             // swallowRate
    2,                             // swallowAttribute
    100,                           // shotDamageRate
    20,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },          // starPointDispOffset
    "SFX_BOSS_SATELLITE_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0x00000004,                    // attributeFlags
    &regist,                       // vulnStatus
    1,                             // numParts
    parts,                         // mParts
    init_event,                    // initEvtScript
    data_table,                    // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    75,  // stopRate
    90,  // dizzyRate
    0,   // poisonRate
    10,  // confuseRate
    100, // electricRate
    100, // burnRate
    75,  // freezeRate
    100, // hugeRate
    75,  // tinyRate
    100, // attackUpRate
    75,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    75,  // allergicRate
    0,   // frightRate
    90,  // galeForceRate
    100, // fastRate
    75,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    90,  // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "K_1" },
    { 0x1C, "S_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "D_1" },
    { 0x27, "D_1" },
    { 0x32, "A_1" },
    { 0x2A, "S_1" },
    { 0x28, "S_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "A_1" },
    { 0x45, "S_1" },
};

static BattleDataEntry data_table[15] = {
    { 0x18, dmg_counter_event },
    { 0x19, dmg_counter_event },
    { 0x1A, dmg_counter_event },
    { 0x1B, dmg_counter_event },
    { 0x1C, dmg_counter_event },
    { 0x1D, dmg_counter_event },
    { 0x1E, dmg_counter_event },
    { 0x1F, dmg_counter_event },
    { 0x20, dmg_counter_event },
    { 0x21, dmg_counter_event },
    { 0x22, dmg_counter_event },
    { 0x23, dmg_counter_event },
    { 0x24, dmg_counter_event },
    { 0x32, dead_event },
    { 0x00, NULL },
};
USER_FUNC(bts_wariai_enzan_1) {
    s32* args = event->args;
    f32 f29 = evtGetFloat(event, args[0]) - 1.0f;
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f1 = evtGetFloat(event, args[3]);
    evtSetValue(event, args[4], (s32)((f29 * f31 + f1 * (f30 - f29)) / f30));
    return EVT_RETURN_DONE;
}

EVT_BEGIN(dmg_counter_event)
    CALL(btlevtcmd_OnUnitFlag, LW(10), 1)
    SWITCH(LW(0))
        CASE_EQUAL(43)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_ATTACK_MISS3"), 0)
        SET_VALUE(LW(14), 36)
        CASE_OR_EQUAL(46)
        CASE_OR_EQUAL(49)
        CASE_OR_EQUAL(50)
        CASE_OR_EQUAL(54)
        SET_VALUE(LW(14), 29)
        CASE_END
        CASE_DEFAULT
        SET_VALUE(LW(14), 39)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, LW(10), LW(2), LW(3), LW(4))
    CALL(btlevtcmd_FaceDirectionSub, LW(10), LW(2), 25)
    ADD(LW(3), 25)
    CALL(btlevtcmd_DivePosition, LW(10), LW(2), LW(3), LW(4), 10, 0, 0, 0, -1)
    WAIT_MS(500)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, LW(10), 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SATELLITE_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHomePos, LW(10), LW(5), LW(6), LW(7))
    WAIT_MS(80)
    SET_VALUE(LW(8), 42)
    LOOP(LW(8))
        CALL(bts_wariai_enzan_1, LW(8), 42, LW(2), LW(5), LW(12))
        CALL(bts_wariai_enzan_1, LW(8), 42, LW(3), LW(6), LW(13))
        CALL(bts_wariai_enzan_1, LW(8), 42, LW(4), LW(7), LW(14))
        CALL(btlevtcmd_SetPos, -2, LW(12), LW(13), LW(14))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static BattleUnitKindPart parts[1] = {
    {
        1,                         // partNum
        "btl_un_batten_satellite", // partName
        "c_shuryo_p",              // modelName
        { 0.0f, 0.0f, 0.0f },      // positionOffset
        { -4.0f, -2.0f, 0.0f },    // hitBaseOffset
        { -6.0f, 1.0f, 0.0f },     // hitCursorBaseOffset
        14,                        // shadowSize
        14,                        // field_0x32
        255,                       // baseAlpha
        &defence,                  // defense
        &defence_attr,             // defenseAttr
        0x00600009,                // attributes
        0,                         // counterAttributes
        pose_table,                // poseTable
    },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetHpGaugeOffset, -2, -5, -13)
    CALL(btlevtcmd_OnUnitFlag, -2, 3145728)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    SET_VALUE(LW(9), PTR(&weapon_batten_satellite_attack))
    CALL_CHILD(batten_satellite_attack_attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(batten_satellite_attack_attack_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("F_1"))
    WAIT_MS(400)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SATELLITE_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(13), LW(6), LW(7))
    SET_VALUE(LW(8), 9)
    LOOP(LW(8))
        CALL(bts_wariai_enzan_1, LW(8), 9, LW(0), LW(13), LW(10))
        CALL(bts_wariai_enzan_1, LW(8), 9, LW(1), LW(6), LW(11))
        CALL(bts_wariai_enzan_1, LW(8), 9, LW(2), LW(7), LW(12))
        CALL(btlevtcmd_SetPos, -2, LW(10), LW(11), LW(12))
        WAIT_FRAMES(1)
    END_LOOP
    SUBTRACT(LW(13), LW(0))
    DIVIDE(LW(13), 2)
    SUBTRACT(LW(6), LW(1))
    DIVIDE(LW(6), 2)
    SUBTRACT(LW(7), LW(2))
    DIVIDE(LW(7), 2)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(13), LW(0))
    ADD(LW(6), LW(1))
    ADD(LW(7), LW(2))
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(8), 4)
        LOOP(LW(8))
            CALL(bts_wariai_enzan_1, LW(8), 4, LW(0), LW(13), LW(10))
            CALL(bts_wariai_enzan_1, LW(8), 4, LW(1), LW(6), LW(11))
            CALL(bts_wariai_enzan_1, LW(8), 4, LW(2), LW(7), LW(12))
            CALL(btlevtcmd_SetPos, -2, LW(10), LW(11), LW(12))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(98)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_SATELLITE_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    WAIT_MS(80)
    SET_VALUE(LW(8), 42)
    LOOP(LW(8))
        CALL(bts_wariai_enzan_1, LW(8), 42, LW(13), LW(0), LW(10))
        CALL(bts_wariai_enzan_1, LW(8), 42, LW(6), LW(1), LW(11))
        CALL(bts_wariai_enzan_1, LW(8), 42, LW(7), LW(2), LW(12))
        CALL(btlevtcmd_SetPos, -2, LW(10), LW(11), LW(12))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: type
// Uninitialised so it lands in .bss like the original (not .data).
u32 bts_x_flag_data[1];
USER_FUNC(bts_wariai_enzan_1f) {
    s32* args = event->args;
    f32 f29 = evtGetFloat(event, args[0]) - 1.0f;
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f1 = evtGetFloat(event, args[3]);
    evtSetFloat(event, args[4], (f29 * f31 + f1 * (f30 - f29)) / f30);
    return EVT_RETURN_DONE;
}

static inline f32 bts_intpl(s32 mode, f32 start, f32 end, s32 step, s32 steps) {
    return intplGetValue(mode, step, steps, start, end);
}
USER_FUNC(bts_wariai_enzan_1f_intpl) {
    s32* args = event->args;
    f32 f28 = evtGetFloat(event, args[0]) - 1.0f;
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    s32 dest = args[4];
    f32 f1 = evtGetFloat(event, args[5]);
    f32 v;
    f32 f2;
    f32 t;
    if (f1 >= 14.0f) {
        f2 = f1 - 14.0f;
        if (f2 >= 14.0f) {
            evtSetFloat(event, dest, f31);
            return EVT_RETURN_DONE;
        }
        v = 1.0f - bts_intpl((s32)f2, 1.0f, 0.0f, (s32)(f29 - f28), (s32)f29);
    } else {
        v = bts_intpl((s32)f1, 0.0f, 1.0f, (s32)(f29 - f28), (s32)f29);
    }
    t = 1.0f - v;
    evtSetFloat(event, dest, t * f31 + v * f30);
    return EVT_RETURN_DONE;
}

// The second if tests f31 but subtracts 14 from f26 into a fresh `f2`, as in the original.
// Three things were needed on top of bts_intpl: declaring `f25` after the evtGetFloat arg loads (so the
// allocator gives the low group f26/f27/f28 and the intermediate f25, not one register lower), and writing
// the final term as `f30 * f1` so MWCC emits `fmuls f0, f30, f2` in the original's operand order.
USER_FUNC(bts_wariai_enzan_1f_intpl_w) {
    s32* args = event->args;
    f32 f28 = evtGetFloat(event, args[0]) - 1.0f;
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f27 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    s32 dest = args[4];
    f32 f26 = evtGetFloat(event, args[5]);
    f32 f31 = evtGetFloat(event, args[6]);
    f32 f25;
    f32 f1;
    f32 f2;
    f32 t;
    if (f26 >= 14.0f) {
        f26 = f26 - 14.0f;
        if (f26 >= 14.0f) {
            evtSetFloat(event, dest, f27);
            return EVT_RETURN_DONE;
        }
        f25 = 1.0f - bts_intpl((s32)f26, 1.0f, 0.0f, (s32)(f29 - f28), (s32)f29);
    } else {
        f25 = bts_intpl((s32)f26, 0.0f, 1.0f, (s32)(f29 - f28), (s32)f29);
    }
    if (f31 >= 14.0f) {
        f2 = f26 - 14.0f;
        if (f2 >= 14.0f) {
            evtSetFloat(event, dest, f27);
            return EVT_RETURN_DONE;
        }
        f1 = 1.0f - bts_intpl((s32)f2, 1.0f, 0.0f, (s32)(f29 - f28), (s32)f29);
    } else {
        f1 = bts_intpl((s32)f31, 0.0f, 1.0f, (s32)(f29 - f28), (s32)f29);
    }
    f1 = f25 * f1;
    t = 1.0f - f1;
    evtSetFloat(event, dest, f27 * t + f30 * f1);
    return EVT_RETURN_DONE;
}

USER_FUNC(bts_wariai_enzan_0) {
    s32* args = event->args;
    f32 f30 = evtGetFloat(event, args[0]) - 1.0f;
    f32 f29 = evtGetFloat(event, args[1]) - 1.0f;
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f1 = evtGetFloat(event, args[3]);
    evtSetValue(event, args[4], (s32)((f30 * f31 + f1 * (f29 - f30)) / f29));
    return EVT_RETURN_DONE;
}

USER_FUNC(bts_wariai_enzan_0f) {
    s32* args = event->args;
    f32 f30 = evtGetFloat(event, args[0]) - 1.0f;
    f32 f29 = evtGetFloat(event, args[1]) - 1.0f;
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f1 = evtGetFloat(event, args[3]);
    evtSetFloat(event, args[4], (f30 * f31 + f1 * (f29 - f30)) / f29);
    return EVT_RETURN_DONE;
}

USER_FUNC(bts_x_flag_set) {
    s32* args = event->args;
    bts_x_flag_data[0] = evtGetValue(event, args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(bts_x_flag_add) {
    s32* args = event->args;
    bts_x_flag_data[0] += evtGetValue(event, args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(bts_x_flag_get) {
    s32* args = event->args;
    evtSetValue(event, args[0], bts_x_flag_data[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(bts_x_flag_wait) {
    s32* args = event->args;
    return (evtGetValue(event, args[0]) - bts_x_flag_data[0]) == 0 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// clang-format on

#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts;
static BattleDispPose pose_table[18];
static BattleDataEntry data_table[2];
static s32 init_event[39];
static s32 damage_event[10];
static s32 attack_event[407];
static s32 wait_event[6];

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_killer = {
    UNIT_BULLET_BILL,           // mUnitType
    "btl_un_killer",            // name
    2,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    1,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    9999,                       // field_0x14
    32,                         // width
    26,                         // height
    { 3, 26 },                  // statusIconOffset
    { 0.0f, 13.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 16.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 16.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 16.0f, 16.9f, 0.0f },     // kissHitOffset
    { 0.0f, 13.0f, 0.0f },      // cutBaseOffset
    32.0f,                      // cutWidth
    26.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_KILLER_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00000004,                 // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    &parts,                     // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 1, 1, 1, 1, 1 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    0,   // stopRate
    105, // dizzyRate
    0,   // poisonRate
    50,  // confuseRate
    150, // electricRate
    100, // burnRate
    70,  // freezeRate
    100, // hugeRate
    95,  // tinyRate
    100, // attackUpRate
    95,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    75,  // allergicRate
    0,   // frightRate
    100, // galeForceRate
    100, // fastRate
    75,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleUnitKindPart parts = {
    1,                     // partNum
    "btl_un_killer",       // partName
    "c_kilr",              // modelName
    { 0.0f, 0.0f, 0.0f },  // positionOffset
    { 0.0f, 25.0f, 0.0f }, // hitBaseOffset
    { 0.0f, 30.0f, 0.0f }, // hitCursorBaseOffset
    20,                    // field_0x30
    30,                    // field_0x32
    255,                   // baseAlpha
    &defence,              // defense
    &defence_attr,         // defenseAttr
    0x00000009,            // attributes
    0,                     // counterAttributes
    pose_table,            // poseTable
};

static BattleWeapon weapon_killer_charge_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
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
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0x000007FF,                               // counterResistanceFlags
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleDispPose pose_table[18] = {
    { 0x01, "KIL_N_1" },
    { 0x02, "KIL_D_1" },
    { 0x09, "KIL_D_1" },
    { 0x05, "KIL_K_1" },
    { 0x04, "KIL_X_1" },
    { 0x03, "KIL_X_1" },
    { 0x1C, "KIL_S_1" },
    { 0x1D, "KIL_Q_1" },
    { 0x1E, "KIL_Q_1" },
    { 0x1F, "KIL_A_1" },
    { 0x27, "KIL_D_1" },
    { 0x32, "KIL_A_1" },
    { 0x28, "KIL_W_1" },
    { 0x2A, "KIL_R_1" },
    { 0x38, "KIL_X_1" },
    { 0x39, "KIL_X_1" },
    { 0x41, "KIL_T_1" },
    { 0x45, "KIL_S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), 10, LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 10, LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(start_avoid)
    IF_INT_EQUAL(LW(5), 3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
    END_IF
    IF_INT_EQUAL(LW(5), 6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
    END_IF
    IF_INT_EQUAL(LW(5), 2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_killer_charge_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_killer_charge_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_killer_charge_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_killer_charge_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_killer_charge_attack), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        GOTO(90)
        CASE_EQUAL(2)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 10, LW(2), 0, -1)
    CALL_CHILD(start_avoid)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 500)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 10, LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_w, 0, 5, 0, 15, 0)
    WAIT_MS(300)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    WAIT_MS(500)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 10, LW(2), 0, -1)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_killer_charge_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_killer_charge_attack), 256, LW(5))
    CALL(evt_btl_camera_shake_w, 0, 5, 0, 15, 0)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    ADD(LW(1), 20)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 17, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetCoin, -2, LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, -2, LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(1), LW(2))
    ADD(LW(2), 10)
    ADD(LW(1), 30)
    CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    WAIT_MS(500)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(200)
    LABEL(99)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

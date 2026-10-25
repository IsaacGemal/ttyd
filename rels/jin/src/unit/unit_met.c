#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static BattleDataEntry data_table[3];
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts;
static s32 init_event[12];
static s32 init_event2[148];
static s32 damage_event[10];
static s32 dmg_turn_event[71];
static s32 attack_event[546];
static s32 wait_event[6];
static s32 damage_event_ceil[49];
static s32 counter_damage_event_ceil[76];
static s32 ceil_fall_ready_event[158];
static s32 attack_event_ceil[467];
static s32 wait_event_ceil[6];

// clang-format off
extend_data(88,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(89,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_met = {
    UNIT_BUZZY_BEETLE,          // mUnitType
    "btl_un_met",               // name
    5,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    16,                         // mLevel
    0,                          // mBonusExp
    2,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    1,                          // baseCoin
    60,                         // escapeRate
    9999,                       // renzokuSoftCap
    28,                         // width
    20,                         // height
    { 5, 20 },                  // statusIconOffset
    { 0.0f, 10.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 14.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 14.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 12.0f, 15.0f, 0.0f },     // kissHitOffset
    { 0.0f, 10.0f, 0.0f },      // cutBaseOffset
    28.0f,                      // cutWidth
    20.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_MET_DAMAGED1",     // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    &parts,                     // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleDispPose pose_table[18] = {
    { 0x01, "MET_N_1" },
    { 0x02, "MET_Y_1" },
    { 0x09, "MET_Y_1" },
    { 0x05, "MET_K_1" },
    { 0x04, "MET_X_1" },
    { 0x03, "MET_X_1" },
    { 0x1C, "MET_S_1" },
    { 0x1D, "MET_Q_1" },
    { 0x1E, "MET_Q_1" },
    { 0x1F, "MET_A_1" },
    { 0x27, "MET_D_1" },
    { 0x32, "MET_A_1" },
    { 0x28, "MET_W_1" },
    { 0x2A, "MET_R_1" },
    { 0x38, "MET_X_1" },
    { 0x39, "MET_X_1" },
    { 0x41, "MET_T_1" },
    { 0x45, "MET_S_1" },
};

static BattleDispPose pose_table_ceil[18] = {
    { 0x01, "SMT_N_1" },
    { 0x02, "SMT_Y_1" },
    { 0x09, "SMT_Y_1" },
    { 0x05, "SMT_K_1" },
    { 0x04, "SMT_X_1" },
    { 0x03, "SMT_X_1" },
    { 0x1C, "SMT_S_1" },
    { 0x1D, "SMT_Q_1" },
    { 0x1E, "SMT_Q_1" },
    { 0x1F, "SMT_A_1" },
    { 0x27, "SMT_D_1" },
    { 0x32, "SMT_A_1" },
    { 0x28, "SMT_W_1" },
    { 0x2A, "SMT_R_1" },
    { 0x38, "SMT_X_1" },
    { 0x39, "SMT_X_1" },
    { 0x41, "SMT_S_1" },
    { 0x45, "SMT_S_1" },
};

static BattleDispPose pose_table_turn[17] = {
    { 0x01, "MET_N_2" },
    { 0x02, "MET_Y_2" },
    { 0x09, "MET_Y_2" },
    { 0x05, "MET_K_2" },
    { 0x04, "MET_X_2" },
    { 0x03, "MET_X_2" },
    { 0x1C, "MET_S_2" },
    { 0x1D, "MET_Q_2" },
    { 0x1E, "MET_Q_2" },
    { 0x1F, "MET_A_1" },
    { 0x27, "MET_D_3" },
    { 0x32, "MET_A_1" },
    { 0x28, "MET_W_1" },
    { 0x38, "MET_X_2" },
    { 0x39, "MET_X_2" },
    { 0x41, "MET_S_2" },
    { 0x45, "MET_S_2" },
};

static BattleDataEntry data_table[3] = {
    { 0x0D, dmg_turn_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleDataEntry data_table_ceil[14] = {
    { 0x1A, counter_damage_event_ceil },
    { 0x19, counter_damage_event_ceil },
    { 0x1B, counter_damage_event_ceil },
    { 0x1C, counter_damage_event_ceil },
    { 0x1D, counter_damage_event_ceil },
    { 0x1E, counter_damage_event_ceil },
    { 0x1F, counter_damage_event_ceil },
    { 0x20, counter_damage_event_ceil },
    { 0x21, counter_damage_event_ceil },
    { 0x22, counter_damage_event_ceil },
    { 0x23, counter_damage_event_ceil },
    { 0x24, counter_damage_event_ceil },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleWeapon weapon_met_attack = {
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
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21004004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000005FF,                               // counterResistanceFlags
    0x00002004,                               // targetWeightingFlags
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

static BattleWeapon weapon_met_ceil_attack = {
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
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00005087,                               // special
    0x000005FF,                               // counterResistanceFlags
    0x80002000,                               // targetWeightingFlags
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

static BattleUnitDefense defence = { { 4, 99, 4, 99, 4 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 2, 0, 2, 0 } };

static BattleUnitDefense defence_turn = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_turn = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    75,  // sleepRate
    90,  // stopRate
    75,  // dizzyRate
    100, // poisonRate
    75,  // confuseRate
    100, // electricRate
    0,   // burnRate
    75,  // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    75,  // allergicRate
    60,  // frightRate
    30,  // galeForceRate
    100, // fastRate
    75,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleUnitKindPart parts = {
    1,                     // partNum
    "btl_un_met",          // partName
    "c_met",               // modelName
    { 0.0f, 0.0f, 0.0f },  // positionOffset
    { 0.0f, 20.0f, 0.0f }, // hitBaseOffset
    { 0.0f, 25.0f, 0.0f }, // hitCursorBaseOffset
    20,                    // shadowSize
    30,                    // field_0x32
    255,                   // baseAlpha
    &defence,              // defense
    &defence_attr,         // defenseAttr
    0x00000009,            // attributes
    0,                     // counterAttributes
    pose_table,            // poseTable
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
    CALL_CHILD(init_event2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(init_event2)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
        CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
        CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
        CALL(btlevtcmd_SetEventCeilFall, -2, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4096)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    ELSE
        CALL(btlevtcmd_OnAttribute, -2, 2)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4194304)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 2097152)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_ceil))
        CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table_ceil))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(1), 130)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_ceil))
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_ceil))
        CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_ceil))
        CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event_ceil))
        CALL(btlevtcmd_SetEventCeilFall, -2, PTR(ceil_fall_ready_event))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, -50, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, -50, 0)
        CALL(btlevtcmd_SetKissHitOffset, -2, 12, -15, 0)
    END_IF
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_MET_MOVE1L"), STRING("SFX_ENM_MET_MOVE1R"), 0, 3, 3)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_MET_MOVE1L"), STRING("SFX_ENM_MET_MOVE1R"), 0, 6, 6)
    CALL(btlevtcmd_SetOverTurnCount, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_turn_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence_turn))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 1)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(4)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 18, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 12, -1)
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 69)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 18, -1)
    CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetOverTurnCount, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(wakeup_event)
        RETURN
    END_IF
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        WAIT_FRAMES(30)
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_met_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_met_attack), LW(3), LW(4))
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
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_met_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_met_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 10)
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 20)
    WAIT_MS(100)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            ADD(LW(1), 10)
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 0)
    DELETE_EVENT(LW(13))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_met_attack), 256, LW(5))
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
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 10)
    WAIT_MS(250)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(10.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_met_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_met_attack), 256, LW(5))
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 75)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_ceil)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_GetTotalDamage, -2, LW(12))
    IF_INT_EQUAL(LW(12), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(ceil_fall_ready_event)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event_ceil)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 12, 15, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    CALL_CHILD(init_event2)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    CALL_CHILD(subsetevt_counter_damage)
    RETURN
EVT_END()

static EVT_BEGIN(ceil_fall_ready_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, 0)
    CALL(btlevtcmd_SetJumpSpeed, -2, 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 15, LW(2), 15, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_turn))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 12, 15, 0)
    CALL_CHILD(init_event2)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence_turn))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 69)
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 18, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 12, -1)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_ceil)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_met_ceil_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_met_ceil_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_met_ceil_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_met_ceil_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(10), LW(11), LW(12))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(11), LW(12), 60, -1, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    WAIT_MS(250)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, 0)
    CALL(btlevtcmd_SetJumpSpeed, -2, 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 18, LW(2), 15, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_met_ceil_attack), 256, LW(5))
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
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_met_ceil_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_met_ceil_attack), 256, LW(5))
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, -18, 0)
    SET_FLOAT_VALUE(LW(12), FLOAT(30.0f))
    SET_FLOAT_VALUE(LW(13), FLOAT(0.0f))
    LOOP(60)
        CALL(evt_sub_get_sincos, LW(13), LW(14), LW(15))
        MULTIPLY_FLOAT(LW(14), LW(12))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(14))
        SUBTRACT_FLOAT(LW(12), FLOAT(0.5f))
        ADD_FLOAT(LW(13), FLOAT(10.0f))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 12, 15, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 25)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 25)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetHomePos, -2, LW(7), 0, LW(9))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(7), 0, LW(9), 0, -1, 0)
    CALL_CHILD(init_event2)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_ceil)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

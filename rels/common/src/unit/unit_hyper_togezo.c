#include "battle/battle_camera.h"
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
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[5];
static BattleUnitKindPart parts[1];
static s32 init_event[52];
static s32 dmg_avoid_counter_thorn_event[61];
static s32 damage_event[10];
static s32 dmg_turn_event[84];
static s32 attack_event[109];
static s32 normal_attack_event[813];
static s32 guard_event[95];
static s32 guard_cancel_event[89];
static s32 charge_event[102];
static s32 wait_event[6];

// clang-format off
extend_data(hyper_togezo_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(hyper_togezo_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_SKY_BLUE_SPINY,        // mUnitType
    "btl_un_hyper_togezo",      // name
    6,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    22,                         // mLevel
    0,                          // mBonusExp
    1,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    9999,                       // field_0x14
    32,                         // width
    30,                         // height
    { 4, 30 },                  // statusIconOffset
    { 0.0f, 15.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 16.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 16.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 12.0f, 16.0f, 0.0f },     // kissHitOffset
    { 0.0f, 15.0f, 0.0f },      // cutBaseOffset
    32.0f,                      // cutWidth
    30.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_TOGEZO_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 4, 4, 4, 4, 4 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    60,  // sleepRate
    70,  // stopRate
    60,  // dizzyRate
    100, // poisonRate
    60,  // confuseRate
    100, // electricRate
    100, // burnRate
    50,  // freezeRate
    100, // hugeRate
    85,  // tinyRate
    100, // attackUpRate
    85,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    70,  // allergicRate
    80,  // frightRate
    10,  // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    50,  // instaKillRate
};

static BattleUnitDefense defence_turn = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_turn = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_turn = {
    100, // sleepRate
    100, // stopRate
    100, // dizzyRate
    100, // poisonRate
    100, // confuseRate
    100, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    100, // tinyRate
    100, // attackUpRate
    100, // attackDownRate
    100, // defenseUpRate
    100, // defenseDownRate
    100, // allergicRate
    100, // frightRate
    100, // galeForceRate
    100, // fastRate
    100, // slowRate
    100, // dodgyRate
    100, // invisibleRate
    100, // instaKillRate
};

static BattleUnitDefense defence_guard = { { 99, 99, 99, 99, 99 } };

static BattleUnitDefenseAttr defence_attr_guard = { { 2, 2, 2, 2, 2 } };

static BattleVulnerableStatus regist_guard = {
    100, // sleepRate
    100, // stopRate
    100, // dizzyRate
    100, // poisonRate
    100, // confuseRate
    100, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    100, // tinyRate
    100, // attackUpRate
    100, // attackDownRate
    100, // defenseUpRate
    100, // defenseDownRate
    100, // allergicRate
    100, // frightRate
    100, // galeForceRate
    100, // fastRate
    100, // slowRate
    100, // dodgyRate
    100, // invisibleRate
    100, // instaKillRate
};

static BattleDispPose pose_table[15] = {
    { 0x01, "TGZ_N_1" },
    { 0x02, "TGZ_Y_1" },
    { 0x09, "TGZ_Y_1" },
    { 0x05, "TGZ_K_1" },
    { 0x04, "TGZ_X_1" },
    { 0x03, "TGZ_X_1" },
    { 0x1C, "TGZ_S_1" },
    { 0x1D, "TGZ_Q_1" },
    { 0x1E, "TGZ_Q_1" },
    { 0x1F, "TGZ_A_1" },
    { 0x27, "TGZ_D_1" },
    { 0x28, "TGZ_W_1" },
    { 0x2A, "TGZ_R_1" },
    { 0x41, "TGZ_T_1" },
    { 0x45, "TGZ_S_1" },
};

static BattleDispPose pose_table_turn[14] = {
    { 0x01, "TGZ_N_2" },
    { 0x02, "TGZ_Y_2" },
    { 0x09, "TGZ_Y_2" },
    { 0x05, "TGZ_K_2" },
    { 0x04, "TGZ_X_2" },
    { 0x03, "TGZ_X_2" },
    { 0x1C, "TGZ_S_2" },
    { 0x1D, "TGZ_Q_2" },
    { 0x1E, "TGZ_Q_2" },
    { 0x1F, "TGZ_S_1" },
    { 0x27, "TGZ_D_3" },
    { 0x28, "TGZ_W_1" },
    { 0x2A, "TGZ_R_1" },
    { 0x45, "TGZ_S_2" },
};

static BattleDispPose pose_table_guard[12] = {
    { 0x01, "TGZ_P_1" },
    { 0x02, "TGZ_P_1" },
    { 0x09, "TGZ_P_1" },
    { 0x05, "TGZ_P_1" },
    { 0x04, "TGZ_P_1" },
    { 0x03, "TGZ_P_1" },
    { 0x1C, "TGZ_P_1" },
    { 0x1D, "TGZ_P_1" },
    { 0x1E, "TGZ_P_1" },
    { 0x1F, "TGZ_P_1" },
    { 0x27, "TGZ_P_1" },
    { 0x45, "TGZ_P_1" },
};

static BattleDataEntry data_table[5] = {
    { 0x0D, dmg_turn_event },
    { 0x25, dmg_avoid_counter_thorn_event },
    { 0x27, dmg_avoid_counter_thorn_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleDataEntry data_table_turn[3] = {
    { 0x0D, dmg_turn_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleDataEntry data_table_guard[4] = {
    { 0x25, dmg_avoid_counter_thorn_event },
    { 0x27, dmg_avoid_counter_thorn_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_hyper_togezo", // partName
        "c_togezo_b",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00001009,            // attributes
        0x00000005,            // counterAttributes
        pose_table,            // poseTable
    },
};

static BattleWeapon weapon_togezo_attack = {
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
    { 6, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
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

static BattleWeapon weapon_togezo_charge = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
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
    6,                                        // chargeStrength
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

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_TOGEZO_MOVE1L"), STRING("SFX_ENM_TOGEZO_MOVE1R"), 0, 10, 10)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_TOGEZO_MOVE1L"), STRING("SFX_ENM_TOGEZO_MOVE1R"), 0, 6, 6)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_ENM_KURI_JUMP1"), STRING("SFX_ENM_KURI_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_event)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 45)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 25)
        GOTO(90)
    END_IF
    ADD(LW(1), 35)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
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
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_OffPartsCounterAttribute, LW(10), LW(11), 1)
    CALL(btlevtcmd_OffPartsCounterAttribute, LW(10), LW(11), 4)
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table_turn))
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 1)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
    CALL(evt_snd_sfxon, STRING("SFX_CONDITION_INSIDE"), 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, LW(10))
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr))
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_OnPartsCounterAttribute, LW(10), LW(11), 1)
    CALL(btlevtcmd_OnPartsCounterAttribute, LW(10), LW(11), 4)
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table))
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 69)
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetOverTurnCount, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(wakeup_event)
        RETURN
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(guard_cancel_event)
        GOTO(99)
    ELSE
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
            CALL_CHILD(normal_attack_event)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_get_turn, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_DrawLots, LW(0), 3, 40, 40, 20)
        ELSE
            CALL(btlevtcmd_DrawLots, LW(0), 3, 40, 40, 6)
        END_IF
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL_CHILD(normal_attack_event)
            CASE_EQUAL(1)
            CALL_CHILD(guard_event)
            CASE_EQUAL(2)
            CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
            SET_VALUE(LW(9), PTR(&weapon_togezo_charge))
            CALL_CHILD(charge_event)
        END_SWITCH
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_togezo_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_togezo_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_togezo_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_togezo_attack))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(14))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(14))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_TURN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 15, 0)
    LOOP(5)
        SET_VALUE(LW(0), -36)
        MULTIPLY(LW(0), LW(14))
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGZ_P_1"))
    LOOP(5)
        SET_VALUE(LW(0), -36)
        MULTIPLY(LW(0), LW(14))
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(5)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    SET_VALUE(LW(0), -36)
    MULTIPLY(LW(0), LW(14))
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
            CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_FRAMES(10)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_ROLL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 30, 0, FLOAT(0.15f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_ROLL2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(9.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_togezo_attack), 0, LW(5))
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
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_ROLL5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(0), -18)
        MULTIPLY(LW(0), LW(14))
        CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
        SET_VALUE(LW(0), 1)
        MULTIPLY(LW(0), LW(14))
        LOOP(18)
            CALL(btlevtcmd_AddUnitWork, -2, 2, LW(0))
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 50)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 1)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 26)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(1.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    WAIT_FRAMES(10)
    GOTO(97)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_ROLL4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_togezo_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_togezo_attack), 256, LW(5))
    SET_VALUE(LW(0), 36)
    MULTIPLY(LW(0), LW(14))
    CALL(btlevtcmd_SetUnitWork, -2, 2, -36)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    ADD(LW(2), 10)
    CALL(btlevtcmd_JumpSetting, -2, 30, 0, FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 30, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    ADD(LW(2), 5)
    CALL(btlevtcmd_JumpSetting, -2, 15, 0, FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 15, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(0), 18)
        MULTIPLY(LW(0), LW(14))
        CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
        SET_VALUE(LW(0), -1)
        MULTIPLY(LW(0), LW(14))
        LOOP(18)
            CALL(btlevtcmd_AddUnitWork, -2, 2, LW(0))
            WAIT_FRAMES(2)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 18)
    ADD(LW(2), 10)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(1.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 9)
    ADD(LW(2), 5)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(0.5f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    WAIT_FRAMES(3)
    CALL(evt_sub_random, 99, LW(0))
    IF_INT_LESS(LW(0), 50)
        GOTO(98)
    ELSE
        GOTO(97)
    END_IF
    LABEL(97)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_TURN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 15, 0)
    LOOP(5)
        SET_VALUE(LW(0), 36)
        MULTIPLY(LW(0), LW(14))
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGZ_S_1"))
    LOOP(5)
        SET_VALUE(LW(0), 36)
        MULTIPLY(LW(0), LW(14))
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(5)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGZ_S_1"))
    WAIT_MS(250)
    GOTO(99)
    LABEL(98)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_ROLL3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    SET_VALUE(LW(0), 36)
    MULTIPLY(LW(0), LW(14))
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
            CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence_guard))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&defence_attr_guard))
    CALL(btlevtcmd_SetRegistStatus, -2, PTR(&regist_guard))
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_guard))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table_guard))
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4096)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    WAIT_MS(250)
    GOTO(99)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(guard_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_TURN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 15, 0)
    LOOP(5)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, 36)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGZ_P_1"))
    LOOP(5)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, 36)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_FRAMES(5)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence_guard))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr_guard))
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist_guard))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_guard))
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table_guard))
    CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(11), 4096)
    CALL(btlevtcmd_SetUnitWork, LW(10), 0, 1)
    RETURN
EVT_END()

static EVT_BEGIN(guard_cancel_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEZO_TURN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 15, 0)
    LOOP(5)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, -36)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGZ_S_1"))
    LOOP(5)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, -36)
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(5)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr))
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table))
    CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(11), 4096)
    CALL(btlevtcmd_SetUnitWork, LW(10), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(charge_event)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    WAIT_FRAMES(40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 15)
    ADD(LW(2), 10)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_eff64, STRING(""), STRING("crystal_n64"), 7, LW(0), LW(1), LW(2), FLOAT(1.5f), 60, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpSetting, -2, 20, 0, FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        WAIT_MS(500)
        CALL(evt_btl_camera_set_mode, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

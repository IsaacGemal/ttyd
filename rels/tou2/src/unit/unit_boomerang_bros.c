#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_unit.h"
#include "battle/battle_database_common.h"

#pragma force_active on
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[4];
static s32 init_event[21];
static s32 damage_event[10];
static s32 attack_event[71];
static s32 normal_attack_event[174];
static s32 renzoku_attack_event[178];
s32 normal_attack_event_confuse[322];
static s32 wait_event[6];

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boomerang_bros = {
    UNIT_BOOMERANG_BRO,         // mUnitType
    "btl_un_boomerang_bros",    // name
    7,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    20,                         // mLevel
    0,                          // mBonusExp
    2,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    2,                          // baseCoin
    50,                         // field_0x13
    9999,                       // field_0x14
    36,                         // width
    44,                         // height
    { 3, 44 },                  // statusIconOffset
    { 0.0f, 22.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 18.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 15.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 10.0f, 28.0f, 0.0f },     // kissHitOffset
    { 0.0f, 22.0f, 0.0f },      // cutBaseOffset
    36.0f,                      // cutWidth
    44.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_BOOME_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                     // vulnStatus
    4,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 1, 1, 1, 1, 1 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    80,  // sleepRate
    80,  // stopRate
    70,  // dizzyRate
    100, // poisonRate
    80,  // confuseRate
    100, // electricRate
    100, // burnRate
    70,  // freezeRate
    100, // hugeRate
    80,  // tinyRate
    100, // attackUpRate
    80,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    80,  // allergicRate
    70,  // frightRate
    60,  // galeForceRate
    100, // fastRate
    80,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "BRO_N_1" },
    { 0x02, "BRO_Y_1" },
    { 0x09, "BRO_Y_1" },
    { 0x05, "BRO_K_1" },
    { 0x04, "BRO_X_1" },
    { 0x03, "BRO_X_1" },
    { 0x1B, "BRO_D_1" },
    { 0x1C, "BRO_S_1" },
    { 0x1D, "BRO_Q_1" },
    { 0x1E, "BRO_Q_1" },
    { 0x1F, "BRO_S_1" },
    { 0x27, "BRO_D_1" },
    { 0x2A, "BRO_R_1" },
    { 0x28, "BRO_W_1" },
    { 0x38, "BRO_X_1" },
    { 0x39, "BRO_X_1" },
    { 0x41, "BRO_T_1" },
    { 0x45, "BRO_S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[4] = {
    {
        1,                       // partNum
        "btl_un_boomerang_bros", // partName
        "c_burosu_b",            // modelName
        { 0.0f, 0.0f, 0.0f },    // positionOffset
        { 0.0f, 35.0f, 0.0f },   // hitBaseOffset
        { 0.0f, 45.0f, 0.0f },   // hitCursorBaseOffset
        20,                      // field_0x30
        30,                      // field_0x32
        255,                     // baseAlpha
        &defence,                 // defense
        &defence_attr,            // defenseAttr
        0x00000009,              // attributes
        0,                       // counterAttributes
        pose_table,              // poseTable
    },
    {
        2,                       // partNum
        "btl_un_boomerang_bros", // partName
        "c_burosu_b",            // modelName
        { 0.0f, 0.0f, 0.0f },    // positionOffset
        { 15.0f, 25.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },   // hitCursorBaseOffset
        20,                      // field_0x30
        30,                      // field_0x32
        255,                     // baseAlpha
        &defence,                 // defense
        &defence_attr,            // defenseAttr
        0x13010000,              // attributes
        0,                       // counterAttributes
        pose_table,              // poseTable
    },
    {
        3,                       // partNum
        "btl_un_boomerang_bros", // partName
        "c_burosu_b",            // modelName
        { 0.0f, 0.0f, 0.0f },    // positionOffset
        { 15.0f, 25.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },   // hitCursorBaseOffset
        20,                      // field_0x30
        30,                      // field_0x32
        255,                     // baseAlpha
        &defence,                 // defense
        &defence_attr,            // defenseAttr
        0x13010000,              // attributes
        0,                       // counterAttributes
        pose_table,              // poseTable
    },
    {
        4,                       // partNum
        "btl_un_boomerang_bros", // partName
        "c_burosu_b",            // modelName
        { 0.0f, 0.0f, 0.0f },    // positionOffset
        { 15.0f, 25.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },   // hitCursorBaseOffset
        20,                      // field_0x30
        30,                      // field_0x32
        255,                     // baseAlpha
        &defence,                 // defense
        &defence_attr,            // defenseAttr
        0x13010000,              // attributes
        0,                       // counterAttributes
        pose_table,              // poseTable
    },
};

BattleWeapon weapon_boomerang_bros_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
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

BattleWeapon weapon_boomerang_bros_renzoku_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
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

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 7, 0)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetHp, -2, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 3)
        SET_VALUE(LW(0), 70)
        ADD(LW(0), 30)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 30)
            CALL_CHILD(renzoku_attack_event)
            GOTO(99)
        END_IF
    END_IF
    CALL_CHILD(normal_attack_event)
    GOTO(99)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_sub)
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(6), 0, LW(5))
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
    GOTO(99)
    LABEL(91)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
    IF_INT_EQUAL(LW(8), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(6))
        CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
    END_IF
    IF_INT_EQUAL(LW(7), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 0, LW(5))
    END_IF
    SET_VALUE(LW(0), 1)
    GOTO(99)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_no_last)
    SET_VALUE(LW(7), 0)
    CALL_CHILD(damage_check_sub)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_last)
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
        IF_INT_EQUAL(LW(8), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(6))
            CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 256, LW(5))
    ELSE
        SET_VALUE(LW(7), 1)
        CALL_CHILD(damage_check_sub)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(boomerang_move_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -180)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 0, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    SET_VALUE(LW(9), 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(10), LW(12), LW(13))
            IF_INT_EQUAL(LW(9), 2)
                CALL(btlevtcmd_SetUnitWork, -2, 7, 0)
            END_IF
            SWITCH(LW(9))
                CASE_EQUAL(0)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 1)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_GREATER_OR_EQUAL(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL_CHILD(damage_check_no_last)
                SET_VALUE(LW(9), 1)
                CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
                CASE_END
                CASE_EQUAL(1)
                CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 4, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 2)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_GREATER_OR_EQUAL(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL_CHILD(damage_check_no_last)
                SET_VALUE(LW(9), 2)
                CALL(btlevtcmd_SetUnitWork, -2, 6, LW(0))
                CASE_END
                CASE_EQUAL(2)
                CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 4, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 3)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_LESS(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetUnitWork, -2, 6, LW(0))
                IF_INT_EQUAL(LW(14), 0)
                    CALL_CHILD(damage_check_no_last)
                ELSE
                    CALL_CHILD(damage_check_last)
                END_IF
                SET_VALUE(LW(9), 3)
                CASE_END
                CASE_EQUAL(3)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 4)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_LESS(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
                IF_INT_EQUAL(LW(14), 0)
                    CALL_CHILD(damage_check_no_last)
                ELSE
                    CALL_CHILD(damage_check_last)
                END_IF
                SET_VALUE(LW(9), 4)
                CASE_END
                CASE_EQUAL(4)
                BREAK_LOOP
                CASE_END
            END_SWITCH
            LABEL(10)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 64)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), -200, LW(1), LW(2), 40, FLOAT(-20.0f), 4, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK3"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), LW(0), LW(1), LW(2), 40, FLOAT(20.0f), 1, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AddUnitWork, -2, 0, 1)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_boomerang_bros_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_boomerang_bros_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event_confuse)
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_boomerang_bros_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_boomerang_bros_attack))
    SET_VALUE(LW(0), 1)
    SET_VALUE(LW(1), 3)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    LABEL(10)
    CALL(btlevtcmd_SetUnitWork, -2, LW(0), LW(3))
    CALL(btlevtcmd_SetUnitWork, -2, LW(1), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    ADD(LW(0), 1)
    ADD(LW(1), 1)
    GOTO(10)
    LABEL(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(14), 1)
    SET_VALUE(LW(6), PTR(&weapon_boomerang_bros_attack))
    SPAWN_EVENT(boomerang_move_event)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_S_1"))
    WAIT_MS(500)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_boomerang_bros_renzoku_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event_confuse)
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_boomerang_bros_renzoku_attack))
    SET_VALUE(LW(0), 1)
    SET_VALUE(LW(1), 3)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    LABEL(10)
    CALL(btlevtcmd_SetUnitWork, -2, LW(0), LW(3))
    CALL(btlevtcmd_SetUnitWork, -2, LW(1), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    ADD(LW(0), 1)
    ADD(LW(1), 1)
    GOTO(10)
    LABEL(20)
    SET_VALUE(LW(6), PTR(&weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(14), 0)
    SPAWN_EVENT(boomerang_move_event)
    WAIT_MS(750)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 4)
    SET_VALUE(LW(14), 1)
    SPAWN_EVENT(boomerang_move_event)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 2)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_S_1"))
    WAIT_MS(500)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(normal_attack_event_confuse)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_boomerang_bros_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_boomerang_bros_attack))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -180)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 0, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 64)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), 250, 40, 0, 15, FLOAT(-20.0f), 4, 0, -1)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), -30, 40, 0, 15, FLOAT(20.0f), 1, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK3"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), LW(0), LW(1), LW(2), 15, FLOAT(-10.0f), 1, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_boomerang_bros_attack), 256, LW(5))
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
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
        IF_INT_EQUAL(LW(8), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_boomerang_bros_attack))
            CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        END_IF
        ADD(LW(10), 1)
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_boomerang_bros_attack), 0, LW(5))
        CALL(evt_sub_random, 10, LW(0))
        ADD(LW(0), 30)
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_boomerang_bros_attack), 256, LW(5))
    END_SIBLING_EVENT
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(0)
    END_IF
    LABEL(99)
    WAIT_FRAMES(100)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(renzoku_attack_event_confuse)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LOOP(2)
        CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
        WAIT_FRAMES(27)
        SET_VALUE(LW(15), 2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 20)
        CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -180)
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 0, 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 64)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(8))
        CALL(btlevtcmd_DivePartsPosition, -2, LW(15), 250, 40, 0, 15, FLOAT(-20.0f), 4, 0, -1)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        CALL(btlevtcmd_DivePartsPosition, -2, LW(15), -30, 40, 0, 15, FLOAT(20.0f), 1, 0, -1)
        CALL(evt_snd_sfxoff, LW(8))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK3"), EVTDAT_POINTER_MAX, 0, LW(8))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 20)
        CALL(btlevtcmd_DivePartsPosition, -2, LW(15), LW(0), LW(1), LW(2), 15, FLOAT(-10.0f), 1, 0, -1)
        CALL(evt_snd_sfxoff, LW(8))
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_LOOP
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_boomerang_bros_renzoku_attack), 256, LW(5))
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
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
        IF_INT_EQUAL(LW(8), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_boomerang_bros_renzoku_attack))
            CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        END_IF
        ADD(LW(10), 1)
    END_IF
    BEGIN_SIBLING_EVENT
        LOOP(3)
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_boomerang_bros_renzoku_attack), 0, LW(5))
            CALL(evt_sub_random, 10, LW(0))
            ADD(LW(0), 30)
            WAIT_FRAMES(LW(0))
        END_LOOP
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_boomerang_bros_renzoku_attack), 256, LW(5))
    END_SIBLING_EVENT
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(0)
    END_IF
    LABEL(99)
    WAIT_FRAMES(160)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

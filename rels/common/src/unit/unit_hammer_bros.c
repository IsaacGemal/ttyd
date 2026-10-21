#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static void hammer_bros_get_next_target(s32* unitId, s32* partId);
static void hammer_bros_get_target(s32* unitId, s32* partId);
static USER_FUNC(hammer_bros_nagemakuri_set_target);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[6];
static s32 init_event[21];
static s32 damage_event[10];
static s32 attack_event[96];
static s32 normal_attack_event[112];
static s32 renzoku_attack_event[111];
static s32 renzoku_attack_event_sub_no_last[13];
static s32 renzoku_attack_event_sub_last[39];
static s32 nagemakuri_attack_event[186];
static s32 nagemakuri_attack_event_sub[49];
static s32 nagemakuri_attack_event_sub_no_last[13];
static s32 nagemakuri_attack_event_sub_last[76];
static s32 wait_event[6];

// clang-format off
extend_data(unit_hammer_bros_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_hammer_bros_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_HAMMER_BRO,            // mUnitType
    "btl_un_hammer_bros",       // name
    7,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    26,                         // mLevel
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
    "SFX_ENM_HUMMER_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    6,                          // numParts
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

static BattleUnitKindPart parts[6] = {
    {
        1,                     // partNum
        "btl_un_hammer_bros",  // partName
        "c_burosu_h",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 35.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 45.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        2,                      // partNum
        "btl_un_hammer_bros",   // partName
        "c_burosu_h",           // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        3,                      // partNum
        "btl_un_hammer_bros",   // partName
        "c_burosu_h",           // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        4,                      // partNum
        "btl_un_hammer_bros",   // partName
        "c_burosu_h",           // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        5,                      // partNum
        "btl_un_hammer_bros",   // partName
        "c_burosu_h",           // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        6,                      // partNum
        "btl_un_hammer_bros",   // partName
        "c_burosu_h",           // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

static BattleWeapon weapon_hammer_bros_attack = {
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
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
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

static BattleWeapon weapon_hammer_bros_renzoku_attack = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
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

static BattleWeapon weapon_hammer_bros_nagemakuri_attack = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
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
    CALL(btlevtcmd_SetUnitWork, -2, 10, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 11, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 12, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 13, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 14, 0)
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
        SET_VALUE(LW(0), 50)
        ADD(LW(0), 50)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 50)
            CALL_CHILD(renzoku_attack_event)
            GOTO(99)
        END_IF
        CALL_CHILD(nagemakuri_attack_event)
        GOTO(99)
    ELSE
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
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

static EVT_BEGIN(hammer_bros_attack_common_event1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
    MULTIPLY(LW(0), 10)
    ADD(LW(6), LW(0))
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -135)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 5, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(10)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 32)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            ELSE
                CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 60, 0, FLOAT(0.2f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 60, -1)
    CALL(btlevtcmd_JumpPartsContinue, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(6), 100)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, FLOAT(4.0f), FLOAT(0.6f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), 0, LW(8), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(6), 150)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, FLOAT(4.0f), FLOAT(0.4f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), 0, LW(8), 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    RETURN
EVT_END()

static EVT_BEGIN(hammer_bros_attack_common_event2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
    MULTIPLY(LW(0), 10)
    ADD(LW(6), LW(0))
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -135)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 5, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(10)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 32)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 60, 0, FLOAT(0.2f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 60, -1)
    RETURN
EVT_END()

static EVT_BEGIN(hammer_bros_attack_common_event3)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 32)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(6), 50)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, FLOAT(2.0f), FLOAT(0.5f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), 0, LW(8), 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_event_sub)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
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
    CALL_CHILD(hammer_bros_attack_common_event1)
    GOTO(99)
    LABEL(91)
    IF_INT_EQUAL(LW(11), 1)
        IF_INT_EQUAL(LW(9), PTR(&weapon_hammer_bros_renzoku_attack))
            CALL(btlevtcmd_SetUnitWork, -2, 10, 1)
        ELSE
            SET_VALUE(LW(13), 0)
            SET_VALUE(LW(14), 5)
            LOOP(0)
                CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(5))
                CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(6))
                IF_INT_EQUAL(LW(3), LW(5))
                    IF_INT_EQUAL(LW(4), LW(6))
                        ADD(LW(13), 10)
                        CALL(btlevtcmd_SetUnitWork, -2, LW(13), 1)
                        BREAK_LOOP
                    END_IF
                END_IF
                ADD(LW(13), 1)
                ADD(LW(14), 1)
            END_LOOP
        END_IF
    END_IF
    CALL_CHILD(hammer_bros_attack_common_event2)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    IF_INT_EQUAL(LW(10), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    END_IF
    CALL_CHILD(hammer_bros_attack_common_event3)
    GOTO(99)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_hammer_bros_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_hammer_bros_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_hammer_bros_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_hammer_bros_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_1A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(9), PTR(&weapon_hammer_bros_attack))
    SET_VALUE(LW(10), 1)
    SET_VALUE(LW(11), 0)
    CALL_CHILD(damage_check_event_sub)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_hammer_bros_renzoku_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_hammer_bros_renzoku_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_hammer_bros_renzoku_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_hammer_bros_renzoku_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(14), 0)
    SET_VALUE(LW(15), 2)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(14), 4)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_1A"))
        WAIT_FRAMES(27)
        IF_INT_EQUAL(LW(14), 3)
            SPAWN_EVENT(renzoku_attack_event_sub_last)
        ELSE
            SPAWN_EVENT(renzoku_attack_event_sub_no_last)
        END_IF
        WAIT_MS(250)
        ADD(LW(14), 1)
        ADD(LW(15), 1)
    END_LOOP
    LABEL(98)
    WAIT_MS(1500)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event_sub_no_last)
    SET_VALUE(LW(9), PTR(&weapon_hammer_bros_renzoku_attack))
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(damage_check_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event_sub_last)
    CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(hammer_bros_attack_common_event2)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_hammer_bros_renzoku_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_hammer_bros_renzoku_attack), 256, LW(5))
        CALL_CHILD(hammer_bros_attack_common_event3)
    ELSE
        SET_VALUE(LW(9), PTR(&weapon_hammer_bros_renzoku_attack))
        SET_VALUE(LW(10), 1)
        SET_VALUE(LW(11), 0)
        CALL_CHILD(damage_check_event_sub)
    END_IF
    RETURN
EVT_END()

typedef struct HammerBrosSamplingEntry {
    s16 unitId; // 0x0
    s16 partId; // 0x2
    u8 pad[0x24 - 4];
} HammerBrosSamplingEntry;

typedef struct HammerBrosSampling {
    u8 pad_0x0[4];
    HammerBrosSamplingEntry entries[1]; // 0x4
} HammerBrosSampling;

// tou2's original calls the two helpers below with `bl`; tik's inlines them. configure.py gives the two
// module copies different UNIT_DATA_SYM values and nothing else, so gate the pragma on that symbol.
#define HAMMER_BROS_CAT_(a, b) a##b
#define HAMMER_BROS_CAT(a, b) HAMMER_BROS_CAT_(a, b)
#define HAMMER_BROS_SYM_unit_hammer_bros_28_data_24A10 1
#if HAMMER_BROS_CAT(HAMMER_BROS_SYM_, UNIT_DATA_SYM)
#define HAMMER_BROS_TOU2 1
#endif
#ifdef HAMMER_BROS_TOU2
#pragma dont_inline on
#endif
static USER_FUNC(hammer_bros_nagemakuri_set_target) {
    BattleWork* wp = _battleWorkPointer;
    EventEntry* evt = event;
    s32* args = evt->args;
    s32 unitIds[20];
    s32 partIds[20];
    s32 i = 0;
    s32 count = 0;

    wp->sampling.selectIdx = 0;
    hammer_bros_get_target(&unitIds[0], &partIds[0]);
    while (1) {
        if (unitIds[i] == -1) {
            count = i;
            break;
        }
        i++;
        hammer_bros_get_next_target(&unitIds[i], &partIds[i]);
    }
    for (i = 0; i < 5; i++) {
        s32 pick = irand(count);
        evtSetValue(evt, *(args++), unitIds[pick]);
        evtSetValue(evt, *(args++), partIds[pick]);
    }
    return EVT_RETURN_DONE;
}

static void hammer_bros_get_target(s32* unitId, s32* partId) {
    BattleWork* wp = _battleWorkPointer;
    BattleSampling* sampling = &wp->sampling;
    s32 idx = sampling->selectIdx;
    BattleSamplingEntry* entry = &sampling->entries[sampling->order[idx]];

    *unitId = entry->unitId;
    *partId = entry->partId;
}

static void hammer_bros_get_next_target(s32* unitId, s32* partId) {
    BattleWork* wp = _battleWorkPointer;
    BattleSampling* sampling = &wp->sampling;
    BattleSamplingEntry* entry;

    sampling->selectIdx++;
    if (sampling->selectIdx < sampling->count) {
        entry = &sampling->entries[sampling->order[sampling->selectIdx]];
        *unitId = entry->unitId;
        *partId = entry->partId;
    } else {
        *unitId = -1;
        *partId = 0;
    }
}
#ifdef HAMMER_BROS_TOU2
#pragma dont_inline reset
#endif


static EVT_BEGIN(nagemakuri_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_hammer_bros_nagemakuri_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_hammer_bros_nagemakuri_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_hammer_bros_nagemakuri_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_hammer_bros_nagemakuri_attack))
    CALL(hammer_bros_nagemakuri_set_target, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(2))
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(6))
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(8))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(1))
    CALL(btlevtcmd_SetUnitWork, -2, 6, LW(3))
    CALL(btlevtcmd_SetUnitWork, -2, 7, LW(5))
    CALL(btlevtcmd_SetUnitWork, -2, 8, LW(7))
    CALL(btlevtcmd_SetUnitWork, -2, 9, LW(9))
    SET_VALUE(LW(12), 0)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 5)
    SET_VALUE(LW(15), 2)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(12), 4)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(3))
        CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(4))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(10))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(10))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_1A"))
        WAIT_FRAMES(27)
        SPAWN_EVENT(nagemakuri_attack_event_sub)
        WAIT_MS(250)
        ADD(LW(12), 1)
        ADD(LW(13), 1)
        ADD(LW(14), 1)
        ADD(LW(15), 1)
    END_LOOP
    LABEL(98)
    WAIT_MS(1500)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(nagemakuri_attack_event_sub)
    CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(3))
    CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(4))
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(13), 3)
            CALL_CHILD(nagemakuri_attack_event_sub_last)
            BREAK_LOOP
        END_IF
        ADD(LW(13), 1)
        ADD(LW(14), 1)
        CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(5))
        CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(6))
        IF_INT_EQUAL(LW(3), LW(5))
            IF_INT_EQUAL(LW(4), LW(6))
                CALL_CHILD(nagemakuri_attack_event_sub_no_last)
                BREAK_LOOP
            END_IF
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(nagemakuri_attack_event_sub_no_last)
    SET_VALUE(LW(9), PTR(&weapon_hammer_bros_nagemakuri_attack))
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(damage_check_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(nagemakuri_attack_event_sub_last)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 5)
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(5))
        CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(6))
        IF_INT_EQUAL(LW(3), LW(5))
            IF_INT_EQUAL(LW(4), LW(6))
                ADD(LW(13), 10)
                CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(0))
                BREAK_LOOP
            END_IF
        END_IF
        ADD(LW(13), 1)
        ADD(LW(14), 1)
    END_LOOP
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(hammer_bros_attack_common_event2)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_hammer_bros_nagemakuri_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_hammer_bros_nagemakuri_attack), 256, LW(5))
        CALL_CHILD(hammer_bros_attack_common_event3)
    ELSE
        SET_VALUE(LW(9), PTR(&weapon_hammer_bros_nagemakuri_attack))
        SET_VALUE(LW(10), 1)
        SET_VALUE(LW(11), 0)
        CALL_CHILD(damage_check_event_sub)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()



#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
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

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static USER_FUNC(honenoko_copy_status);
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts[6];
static BattleDispPose pose_table[19];
static BattleDataEntry data_table[4];
static s32 init_event[46];
static s32 damage_event[10];
static s32 dead_event[190];
static s32 fake_dead_event[165];
static s32 attack_event[196];
static s32 normal_attack_event[428];
static s32 link_attack_event[111];
static s32 link_attack_event_sub[402];
static s32 call_friend_event[311];
static s32 wait_event[6];

// clang-format off
extend_data(unit_karon_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_karon_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_DRY_BONES,              // mUnitType
    "btl_un_karon",              // name
    8,                           // maxHp
    0,                           // maxFp
    1,                           // mDangerHP
    1,                           // mPerilHP
    30,                          // mLevel
    0,                           // mBonusExp
    2,                           // bonusCoinRolls
    70,                          // bonusCoinRate
    2,                           // baseCoin
    50,                          // escapeRate
    9999,                        // renzokuSoftCap
    36,                          // width
    40,                          // height
    { -4, 40 },                  // statusIconOffset
    { 0.0f, 20.0f, 0.0f },       // centerOffset
    { 0, 0 },                    // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },        // talkTogeBaseOffset
    { 18.0f, 0.0f, -10.0f },     // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },        // burnFlameOffset
    0.0f,                        // kagenukeOffsetX
    0.0f,                        // kagenukeOffsetZ
    { 15.0f, 0.0f, 0.0f },       // bintaHitOffset
    { 9.0f, 24.0f, 0.0f },       // kissHitOffset
    { 0.0f, 20.0f, 0.0f },       // cutBaseOffset
    36.0f,                       // cutWidth
    40.0f,                       // cutHeight
    0,                           // turnOrder
    0,                           // turnOrderVariance
    0,                           // swallowRate
    2,                           // swallowAttribute
    100,                         // shotDamageRate
    20,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },        // starPointDispOffset
    "SFX_ENM_HONENOKO_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0x00010000,                  // attributeFlags
    &regist,                     // vulnStatus
    6,                           // numParts
    parts,                       // mParts
    init_event,                  // initEvtScript
    data_table,                  // dataTable
};

static BattleUnitDefense defence = { { 2, 2, 2, 2, 2 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    75,  // stopRate
    30,  // dizzyRate
    0,   // poisonRate
    30,  // confuseRate
    100, // electricRate
    100, // burnRate
    70,  // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    85,  // defenseDownRate
    70,  // allergicRate
    0,   // frightRate
    70,  // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    90,  // instaKillRate
};

static BattleUnitKindPart parts[6] = {
    {
        1,                     // partNum
        "btl_un_karon",        // partName
        "c_karon_h",           // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 35.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 45.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // shadowSize
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
        "btl_un_karon",         // partName
        "c_karon_h",            // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },   // hitCursorBaseOffset
        20,                     // shadowSize
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
        "btl_un_karon",         // partName
        "c_karon_h",            // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },   // hitCursorBaseOffset
        20,                     // shadowSize
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
        "btl_un_karon",         // partName
        "c_karon_h",            // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },   // hitCursorBaseOffset
        20,                     // shadowSize
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
        "btl_un_karon",         // partName
        "c_nagehone",           // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },   // hitCursorBaseOffset
        20,                     // shadowSize
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
        "btl_un_karon",         // partName
        "c_karon_h",            // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },   // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

static BattleWeapon weapon_honenoko_attack = {
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
    { 5, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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

static BattleWeapon weapon_honenoko_link_attack = {
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
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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

static BattleUnitSetup entry_honenoko[1] = {
    { &UNIT_DATA_SYM, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static BattleDispPose pose_table[19] = {
    { 0x01, "KRN_N_1" },
    { 0x02, "KRN_Y_1" },
    { 0x09, "KRN_Y_1" },
    { 0x05, "KRN_K_1" },
    { 0x04, "KRN_X_1" },
    { 0x03, "KRN_X_1" },
    { 0x1B, "KRN_Z_2" },
    { 0x1C, "KRN_S_1" },
    { 0x1D, "KRN_Q_1" },
    { 0x1E, "KRN_Q_1" },
    { 0x1F, "KRN_S_1" },
    { 0x27, "KRN_D_1" },
    { 0x32, "KRN_A_1" },
    { 0x2A, "KRN_R_1" },
    { 0x28, "KRN_W_1" },
    { 0x38, "KRN_X_1" },
    { 0x39, "KRN_X_1" },
    { 0x41, "KRN_T_1" },
    { 0x45, "KRN_S_1" },
};

static BattleDataEntry data_table[4] = {
    { 0x31, dead_event },
    { 0x0F, fake_dead_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KRN_H_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRN_H_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_H_1"))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("0"), STRING("SFX_ENM_HONENOKO_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(0), 48)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LABEL(10)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AddRotate, LW(10), 0, -15, 0)
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_SetRotate, LW(10), 0, 0, 0)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
        CALL(btlevtcmd_GetCoin, LW(10), LW(0))
        CALL(btlevtcmd_StoreCoin, LW(0))
        CALL(btlevtcmd_GetExp, LW(10), LW(0))
        CALL(btlevtcmd_StoreExp, LW(0))
        CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
        ADD(LW(2), 10)
        ADD(LW(1), 30)
        CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
        CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            LOOP(LW(0))
            END_LOOP
        END_IF
    END_IF
    CALL(btlevtcmd_SetRotateOffset, LW(10), 0, 0, 0)
    SET_VALUE(LW(0), 30)
    LABEL(20)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AddRotate, LW(10), 3, 0, 0)
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_GetBackItem, LW(10))
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, LW(10), 0)
    ELSE
        CALL(btlevtcmd_KillUnit, LW(10), 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(fake_dead_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(2), LW(3), LW(4))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(2), LW(3), LW(4), 0, -1, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
        SET_VALUE(LW(10), -2)
        CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        ADD(LW(2), 10)
        CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_GetCoin, LW(10), LW(0))
        CALL(btlevtcmd_StoreCoin, LW(0))
        CALL(btlevtcmd_GetExp, LW(10), LW(0))
        CALL(btlevtcmd_StoreExp, LW(0))
        CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
        ADD(LW(2), 10)
        ADD(LW(1), 30)
        CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
        CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            LOOP(LW(0))
            END_LOOP
        END_IF
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_D_2"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_CRUMBLE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(1000)
    CALL(btlevtcmd_KillUnit, -2, 2)
    CALL(evt_sub_random, 2, LW(0))
    ADD(LW(0), 2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    CALL(btlevtcmd_SetHitOffset, LW(10), LW(11), 0, -20, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(rebirth_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_D_3"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_D_4"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_OffAttribute, -2, 131072)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    CALL(btlevtcmd_SetHp, -2, LW(0))
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(rebirth_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    LABEL(0)
    SET_VALUE(LW(0), 50)
    ADD(LW(0), 30)
    ADD(LW(0), 20)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 50)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 30)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(link_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
        SET_VALUE(LW(4), LW(0))
        CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
        MULTIPLY(LW(4), LW(5))
        IF_INT_LESS(LW(4), 0)
            SUBTRACT(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL_CHILD(call_friend_event)
                WAIT_MS(500)
                GOTO(99)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
        IF_INT_LESS_OR_EQUAL(LW(0), 170)
            ADD(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
                CALL_CHILD(call_friend_event)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                WAIT_MS(500)
                GOTO(99)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    GOTO(0)
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

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_honenoko_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_honenoko_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_honenoko_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_honenoko_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_1"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_2"))
    WAIT_MS(250)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_SEND_GROUP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_BONE_THROW2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KRN_H_1"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetPartsPos, -2, 1, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(14))
            IF_INT_EQUAL(LW(14), 1)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AddPartsRotate, -2, 2, 0, 0, 25)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_honenoko_attack), 256, LW(5))
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
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            ELSE
                CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 14)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(9))
    MULTIPLY(LW(0), LW(9))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(8.0f), FLOAT(0.05f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, 0)
    CALL(evt_snd_sfxoff, LW(15))
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 14)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(8.0f), FLOAT(0.2f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, 0)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_honenoko_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_honenoko_attack), 256, LW(5))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), -50)
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(2.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, 0)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
    WAIT_FRAMES(10)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(start_avoid2)
    IF_INT_EQUAL(LW(11), 3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
    END_IF
    IF_INT_EQUAL(LW(11), 6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
    END_IF
    IF_INT_EQUAL(LW(11), 2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(link_attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_honenoko_link_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_honenoko_link_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_honenoko_link_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_honenoko_link_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT(link_attack_event_sub)
    WAIT_FRAMES(40)
    SET_VALUE(LW(12), 3)
    SPAWN_EVENT(link_attack_event_sub)
    WAIT_FRAMES(35)
    SET_VALUE(LW(12), 4)
    SPAWN_EVENT(link_attack_event_sub)
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
        IF_INT_EQUAL(LW(0), -1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    WAIT_MS(300)
    RETURN
EVT_END()

static EVT_BEGIN(link_attack_event_sub)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_1"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_2"))
    WAIT_MS(250)
    CALL(btlevtcmd_AddUnitWork, -2, 4, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(13))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_SEND_GROUP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_BONE_THROW3"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, LW(12), STRING("KRN_H_1"))
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 33554432)
    CALL(btlevtcmd_GetPartsPos, -2, 1, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(14))
            IF_INT_EQUAL(LW(14), 1)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AddPartsRotate, -2, LW(12), 0, 0, 25)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    IF_INT_EQUAL(LW(13), 3)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_honenoko_link_attack), 256, LW(11))
    ELSE
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_honenoko_link_attack), 256, LW(11))
    END_IF
    IF_INT_EQUAL(LW(13), 3)
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(14))
        IF_INT_EQUAL(LW(14), 1)
            GOTO(91)
        END_IF
    END_IF
    SWITCH(LW(11))
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
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid2)
                    BREAK_LOOP
                END_IF
            ELSE
                CALL(btlevtcmd_GetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid2)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 14)
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(9))
    MULTIPLY(LW(0), LW(9))
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(12), 0, FLOAT(8.0f), FLOAT(0.05f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 0, 0)
    CALL(evt_snd_sfxoff, LW(15))
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 14)
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(12), 0, FLOAT(8.0f), FLOAT(0.2f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 0, 0)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_honenoko_link_attack))
    IF_INT_EQUAL(LW(13), 3)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_honenoko_link_attack), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_honenoko_link_attack), 0, LW(5))
    END_IF
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), -50)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(12), 0, FLOAT(2.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 0, 0)
    GOTO(98)
    LABEL(98)
    IF_INT_EQUAL(LW(13), 3)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        CALL(btlevtcmd_SetUnitWork, -2, 4, -1)
    END_IF
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 33554432)
    LABEL(99)
    RETURN
EVT_END()

#include "rel/common/honenoko_copy_status.h"


static EVT_BEGIN(call_friend_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    SET_VALUE(LW(10), 0)
    LOOP(6)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_3"))
        WAIT_FRAMES(5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_4"))
        WAIT_FRAMES(15)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_SEND_GROUP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SWITCH(LW(10))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRN_H_4"))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRN_H_5"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRN_H_5"))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRN_H_3"))
            CASE_EQUAL(4)
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRN_H_3"))
            CASE_DEFAULT
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRN_H_2"))
        END_SWITCH
        CALL(btlevtcmd_OffPartsAttribute, -2, 5, 50331648)
        CALL(btlevtcmd_GetPartsPos, -2, 1, LW(4), LW(5), LW(6))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(4), 15)
        ADD(LW(5), 14)
        CALL(btlevtcmd_SetPartsPos, -2, 5, LW(4), LW(5), LW(6))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
            SET_VALUE(LW(15), 0)
            LOOP(0)
                CALL(btlevtcmd_GetUnitWork, -2, 0, LW(14))
                IF_INT_EQUAL(LW(14), 1)
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_SetPartsRotate, -2, 5, 0, 0, LW(15))
                ADD(LW(15), 25)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_JumpPartsSetting, -2, 5, 0, FLOAT(6.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPartsPosition, -2, 5, LW(0), LW(1), LW(2), 20, 0)
        CALL(btlevtcmd_GetPartsPos, -2, 5, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, -2, 6, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_SEND_GROUP2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SWITCH(LW(10))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_C_5"))
            CALL(btlevtcmd_OffPartsAttribute, -2, 6, 50331648)
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_C_4"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_C_3"))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_C_2"))
            CASE_EQUAL(4)
            CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_C_1"))
            CASE_DEFAULT
            CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_Z_2"))
        END_SWITCH
        CALL(btlevtcmd_OnPartsAttribute, -2, 5, 50331648)
        ADD(LW(10), 1)
    END_LOOP
    WAIT_FRAMES(15)
    CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_D_3"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRN_D_4"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_S_1"))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_honenoko), 0)
    CALL(btlevtcmd_OnPartsAttribute, -2, 6, 50331648)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(honenoko_copy_status, -2, LW(3))
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on

#include "unit_atmic_teresa.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];
static BattleVulnerableStatus regist;
static s32 init_event[26];
static s32 damage_event[10];
static s32 attack_event[101];
s32 kaokakusi_event[110];
s32 press_event[264];
s32 surprise_event[303];
s32 bikkuri_event[285];
static s32 breath_event[254];
s32 breath_event_sub[102];
static s32 wait_event[6];
s32 teresa_entry[19];

// clang-format off
extend_data(80,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(81,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_atmic_teresa = {
    UNIT_ATOMIC_BOO,             // mUnitType
    "btl_un_atmic_teresa",       // name
    40,                          // maxHp
    0,                           // maxFp
    1,                           // mDangerHP
    1,                           // mPerilHP
    52,                          // mLevel
    0,                           // mBonusExp
    1,                           // bonusCoinRolls
    100,                         // bonusCoinRate
    0,                           // baseCoin
    50,                          // field_0x13
    9999,                        // field_0x14
    120,                         // width
    110,                         // height
    { 30, 105 },                 // statusIconOffset
    { 0.0f, 55.0f, 0.0f },       // centerOffset
    { 0, 0 },                    // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },        // talkTogeBaseOffset
    { 60.0f, 0.0f, -10.0f },     // field_0x3C
    { 0.0f, 0.0f, 0.0f },        // burnFlameOffset
    0.0f,                        // field_0x54
    0.0f,                        // field_0x58
    { 60.0f, 0.0f, 0.0f },       // bintaHitOffset
    { 46.0f, 71.5f, 10.0f },     // kissHitOffset
    { 0.0f, 55.0f, 0.0f },       // cutBaseOffset
    120.0f,                      // cutWidth
    110.0f,                      // cutHeight
    0,                           // turnOrder
    0,                           // turnOrderVariance
    0,                           // swallowRate
    2,                           // swallowAttribute
    100,                         // field_0x8C
    20,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },        // starPointDispOffset
    "SFX_BOSS_ATMTLSA_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0x00000004,                  // attributeFlags
    &regist,                     // vulnStatus
    1,                           // numParts
    parts,                       // mParts
    init_event,                  // initEvtScript
    data_table,                  // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleDispPose pose_table[17] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "X_1" },
    { 0x1C, "S_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

BattleDispPose pose_table_kaokakusi[17] = {
    { 0x01, "A_1" },
    { 0x02, "A_1" },
    { 0x09, "A_1" },
    { 0x05, "A_1" },
    { 0x04, "A_1" },
    { 0x03, "A_1" },
    { 0x1C, "A_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "A_1" },
    { 0x27, "D_1" },
    { 0x2A, "A_1" },
    { 0x28, "A_1" },
    { 0x38, "A_1" },
    { 0x39, "A_1" },
    { 0x41, "T_1" },
    { 0x45, "A_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                      // partNum
        "btl_un_atmic_teresa",  // partName
        "c_atmic_trs",          // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 10.0f, 80.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 110.0f, 0.0f }, // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x00600009,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

static BattleVulnerableStatus regist = {
    40,  // sleepRate
    40,  // stopRate
    40,  // dizzyRate
    100, // poisonRate
    40,  // confuseRate
    100, // electricRate
    100, // burnRate
    30,  // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    80,  // defenseDownRate
    40,  // allergicRate
    0,   // frightRate
    10,  // galeForceRate
    100, // fastRate
    40,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    5,   // instaKillRate
};

BattleWeapon weapon_atmic_teresa_press = {
    NULL,                                       // name
    0,                                          // icon
    ITEM_NULL,                                  // item
    NULL,                                       // description
    100,                                        // baseAccuracy
    0,                                          // baseFpCost
    0,                                          // baseSpCost
    1,                                          // superguardType
    1.0f,                                       // field_0x14
    1,                                          // mStylishCommandMultiplier
    1,                                          // field_0x19
    1,                                          // mBingoSlotChance
    1,                                          // field_0x1B
    weaponGetPowerDefault,                      // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x02101260,                                 // targetClassFlags
    0x20000000,                                 // targetPropertyFlags
    0,                                          // element
    0,                                          // field_0x6D
    3,                                          // actionLevel
    2,                                          // field_0x6F
    NULL,                                       // acHelpMsg
    0x00001087,                                 // special
    0x0000006C,                                 // counterResistanceFlags
    0,                                          // targetWeightingFlags
    0,                                          // sleepChance
    0,                                          // sleepTime
    0,                                          // stopChance
    0,                                          // stopTime
    0,                                          // dizzyChance
    0,                                          // dizzyTime
    0,                                          // poisonChance
    0,                                          // poisonTime
    0,                                          // poisonStrength
    0,                                          // confuseChance
    0,                                          // confuseTime
    0,                                          // electricChance
    0,                                          // electricTime
    0,                                          // dodgyChance
    0,                                          // dodgyTime
    0,                                          // burnChance
    0,                                          // burnTime
    0,                                          // freezeChance
    0,                                          // freezeTime
    0,                                          // sizeChangeChance
    0,                                          // sizeChangeTime
    0,                                          // sizeChangeStrength
    0,                                          // atkChangeChance
    0,                                          // atkChangeTime
    0,                                          // atkChangeStrength
    0,                                          // defChangeChance
    0,                                          // defChangeTime
    0,                                          // defChangeStrength
    0,                                          // allergicChance
    0,                                          // allergicTime
    0,                                          // ohkoChance
    0,                                          // chargeStrength
    0,                                          // fastChance
    0,                                          // fastTime
    0,                                          // slowChance
    0,                                          // slowTime
    0,                                          // frightChance
    0,                                          // galeForceChance
    0,                                          // paybackTime
    0,                                          // holdFastTime
    0,                                          // invisibleChance
    0,                                          // invisibleTime
    0,                                          // hpRegenTime
    0,                                          // hpRegenStrength
    0,                                          // fpRegenTime
    0,                                          // fpRegenStrength
    NULL,                                       // mAttackEvtScript
    { 0, 50, 50, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon weapon_atmic_teresa_bikkuri = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    50,                                       // stopChance
    3,                                        // stopTime
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

BattleWeapon weapon_atmic_teresa_surprise = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000067,                               // special
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
    50,                                       // confuseChance
    3,                                        // confuseTime
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

BattleWeapon weapon_atmic_teresa_breath = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000097,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000004,                               // targetWeightingFlags
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

// TODO: write atmic_teresa_id_set (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(atmic_teresa_id_set, -2)
    CALL_CHILD(teresa_entry)
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
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(LW(1), 30)
        ADD(LW(1), 20)
        ADD(LW(1), 20)
        SUBTRACT(LW(1), 1)
        CALL(evt_sub_random, LW(1), LW(0))
        SET_VALUE(LW(1), 30)
        IF_INT_LESS(LW(0), LW(1))
            CALL_CHILD(kaokakusi_event)
            GOTO(99)
        END_IF
        ADD(LW(1), 20)
        IF_INT_LESS(LW(0), LW(1))
            CALL_CHILD(press_event)
            GOTO(99)
        END_IF
        CALL_CHILD(bikkuri_event)
        GOTO(99)
    ELSE
        SET_VALUE(LW(1), 50)
        ADD(LW(1), 50)
        SUBTRACT(LW(1), 1)
        CALL(evt_sub_random, LW(1), LW(0))
        SET_VALUE(LW(1), 50)
        IF_INT_LESS(LW(0), LW(1))
            CALL_CHILD(surprise_event)
            GOTO(99)
        END_IF
        CALL_CHILD(breath_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(kaokakusi_event)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 60)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 10, 4, 0, -1)
    WAIT_FRAMES(20)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_FACE_HIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1"))
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 20, 0, 5, 0, -1)
    WAIT_FRAMES(10)
    LOOP(15)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 12, 0)
    END_LOOP
    WAIT_FRAMES(20)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_kaokakusi))
    RETURN
EVT_END()

EVT_BEGIN(press_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_press))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_press), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_press))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_press))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 10)
    CALL(btlevtcmd_DivePosition, -2, -125, 100, LW(2), 120, 10, 4, 0, -1)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("Z_1"))
    CALL(btlevtcmd_DivePosition, -2, -125, 0, LW(2), 30, 0, 1, 0, -1)
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_press), 0, LW(5))
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
        CASE_END
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
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_press))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_press), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(0)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(98)
    WAIT_FRAMES(60)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("W_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 50, 6, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    WAIT_MS(250)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(surprise_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_surprise))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_surprise), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_surprise))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_surprise))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_FACE_HIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 10, 4, 0, -1)
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_DivePosition, -2, 0, LW(1), LW(2), 10, 0, 5, 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_SURPRISED1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_w, 0, 10, 0, 60, 0)
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 4)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_surprise), 0, LW(5))
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
        CASE_END
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
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_surprise))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_surprise), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(0)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(98)
    WAIT_FRAMES(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("W_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, -20, 6, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    WAIT_MS(250)
    LABEL(99)
    RETURN
EVT_END()

// TODO: write teresa_wait_bikkuri2 (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

// TODO: write teresa_wait_bikkuri (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

// TODO: write teresa_appear_bikkuri (global, 0x30 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(bikkuri_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_bikkuri))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_bikkuri), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_bikkuri))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_press))
    CALL(teresa_appear_bikkuri)
    WAIT_FRAMES(75)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 5)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_S_ROUND1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(teresa_wait_bikkuri)
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_SURPRISED2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_bikkuri), 0, LW(5))
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
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_bikkuri))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_bikkuri), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(0)
    END_IF
    LABEL(98)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_WHISTLE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(teresa_wait_bikkuri2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_UNION1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        ADD(LW(2), 1)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 8, LW(0), LW(1), LW(2), FLOAT(1.25f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(30)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(250)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    LABEL(99)
    RETURN
EVT_END()

// TODO: write teresa_wait_breath2 (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

// TODO: write teresa_wait_breath (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

// TODO: write teresa_appear_breath (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

// TODO: write target_id_set (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(breath_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_breath), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(target_id_set, LW(3), LW(4))
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_FACE_HIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 10, 4, 0, -1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 10, 0, 5, 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_BLOW2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(teresa_appear_breath, 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(10)
    IF_INT_EQUAL(LW(3), -1)
        GOTO(20)
    END_IF
    SPAWN_EVENT(breath_event_sub)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(10)
    LABEL(20)
    WAIT_FRAMES(120)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(teresa_wait_breath)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_WHISTLE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(teresa_wait_breath2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_UNION1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        ADD(LW(2), 1)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 8, LW(0), LW(1), LW(2), FLOAT(1.25f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(30)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(1500)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    RETURN
EVT_END()

EVT_BEGIN(breath_event_sub)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 0, LW(5))
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
    WAIT_FRAMES(15)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 0, LW(5))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 0, LW(5))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 256, LW(5))
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: type
u32 atmic_teresa_id[1] = {
    0xFFFFFFFF,
};

// TODO: type
u32 target_unit_id[1] = {
    0xFFFFFFFF,
};

// TODO: type
u32 target_parts_id[1] = {
    0xFFFFFFFF,
};

// TODO: type
u32 atmic_teresa_scale[1] = {
    0x3F800000,
};

// TODO: type
u32 teresa_ext_entry_data[51] = {
    (u32)"c_teresa", (u32)"S_1", 0x00000000, (u32)"c_teresa",
    (u32)"S_1", 0x40000000, (u32)"c_teresa", (u32)"S_1",
    0x40800000, (u32)"c_teresa", (u32)"S_1", 0x40C00000,
    (u32)"c_teresa", (u32)"S_1", 0x41000000, (u32)"c_teresa",
    (u32)"S_1", 0x41200000, (u32)"c_teresa", (u32)"S_1",
    0x40C00000, (u32)"c_teresa", (u32)"S_1", 0x40E00000,
    (u32)"c_teresa", (u32)"A_1", 0x00000000, (u32)"c_teresa",
    (u32)"A_2", 0x40C00000, (u32)"c_teresa", (u32)"A_2",
    0x40E00000, (u32)"c_teresa", (u32)"A_2", 0x41000000,
    (u32)"c_teresa", (u32)"A_2", 0x41100000, (u32)"c_teresa",
    (u32)"A_2", 0x41200000, (u32)"c_teresa", (u32)"A_2",
    0x41300000, (u32)"c_teresa", (u32)"A_2", 0x41400000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: write teresa_ext_main (global, 0x10f0 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

// TODO: write teresa_ext_init (global, 0x138 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(teresa_entry)
    CALL(evt_npc_entry, STRING("ext_teresa"), STRING("c_teresa"))
    CALL(evt_npc_set_position, STRING("ext_teresa"), 0, -1000, 0)
    CALL(evt_ext_entry, 30, PTR(teresa_ext_entry_data), PTR(teresa_ext_init), PTR(teresa_ext_main), 0)
    RETURN
EVT_END()

// TODO: type
static u32 jumptable_00016A64[72] = {
    (u32)((u8*)&teresa_ext_main + 0x360), (u32)((u8*)&teresa_ext_main + 0x3bc), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x4a4), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x624), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x798),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x968), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x9ac),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0xaf0), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0xb84),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0xbd4),
    (u32)((u8*)&teresa_ext_main + 0xbd4), (u32)((u8*)&teresa_ext_main + 0xd84), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0xda8),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0xe74),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0xfb8), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c),
    (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x108c), (u32)((u8*)&teresa_ext_main + 0x1040),
};

// TODO: type
u32 teresa_count[1] = {
    0x00000000,
};

// TODO: type
u32 teresa[1] = {
    0x00000000,
};

// TODO: write teresa_ext_disp (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/jin/asm/unit/unit_atmic_teresa.s; this is where the original has it, just before the first script that uses it
// clang-format on

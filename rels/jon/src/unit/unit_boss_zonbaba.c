#include "unit_boss_zonbaba.h"
#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "eff/n64/eff_kemuri9_n64.h"
#include "evt/evt_eff.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(_leg_smoke_effect);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[2];
static s32 init_event[58];
static s32 damage_event[10];
s32 zonbaba_damage_sub_event[45];
static s32 dead_event[264];
static s32 phase_event[94];
static s32 attack_event[282];
static s32 wait_event[6];

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_zonbaba = {
    UNIT_BONETAIL,              // mUnitType
    "btl_un_zonbaba",           // name
    200,                        // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    99,                         // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    7,                          // field_0x14
    230,                        // width
    170,                        // height
    { -210, 45 },               // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { -250, 0 },                // hpGaugeOffset
    { 280.0f, 40.0f, 0.0f },    // talkTogeBaseOffset
    { 115.0f, 0.0f, -10.0f },   // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    20.0f,                      // field_0x54
    0.0f,                       // field_0x58
    { 250.0f, 0.0f, 50.0f },    // bintaHitOffset
    { 380.0f, 110.0f, 50.0f },  // kissHitOffset
    { -290.0f, 85.0f, 0.0f },   // cutBaseOffset
    230.0f,                     // cutWidth
    170.0f,                     // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    0,                          // field_0x8C
    0,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_GNB_DAMAGED1",    // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00200000,                 // attributeFlags
    &regist,                    // vulnStatus
    2,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 2, 2, 2, 2, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    0,   // stopRate
    0,   // dizzyRate
    100, // poisonRate
    0,   // confuseRate
    100, // electricRate
    100, // burnRate
    0,   // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    0,   // defenseDownRate
    0,   // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    0,   // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table[9] = {
    { 0x01, "GNB_N_1" },
    { 0x05, "GNB_X_1" },
    { 0x03, "GNB_X_1" },
    { 0x02, "GNB_Z_1" },
    { 0x1C, "GNB_S_3" },
    { 0x41, "GNB_T_3" },
    { 0x1F, "GNB_S_3" },
    { 0x27, "GNB_D_1" },
    { 0x45, "GNB_S_3" },
};

static BattleDispPose pose_table_weak[9] = {
    { 0x01, "GNB_N_1" },
    { 0x05, "GNB_X_1" },
    { 0x03, "GNB_X_1" },
    { 0x02, "GNB_N_1" },
    { 0x1C, "GNB_S_4" },
    { 0x41, "GNB_T_4" },
    { 0x1F, "GNB_S_4" },
    { 0x27, "GNB_V_1" },
    { 0x45, "GNB_S_4" },
};

static BattleDataEntry data_table[3] = {
    { 0x09, zonbaba_damage_sub_event },
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[2] = {
    {
        1,                        // partNum
        "btl_un_zonbaba",         // partName
        "c_gonbaba_z",            // modelName
        { 0.0f, 0.0f, 0.0f },     // positionOffset
        { 230.0f, 36.0f, 60.0f }, // hitBaseOffset
        { 230.0f, 36.0f, 60.0f }, // hitCursorBaseOffset
        20,                       // field_0x30
        30,                       // field_0x32
        255,                      // baseAlpha
        &defence,                 // defense
        &defence_attr,            // defenseAttr
        0x00000002,               // attributes
        0,                        // counterAttributes
        pose_table,               // poseTable
    },
    {
        2,                         // partNum
        "btl_un_zonbaba",          // partName
        "c_gonbaba_z",             // modelName
        { 100.0f, 0.0f, 0.0f },    // positionOffset
        { 250.0f, 110.0f, 25.0f }, // hitBaseOffset
        { 250.0f, 110.0f, 25.0f }, // hitCursorBaseOffset
        20,                        // field_0x30
        30,                        // field_0x32
        255,                       // baseAlpha
        &defence,                  // defense
        &defence_attr,             // defenseAttr
        0x01600009,                // attributes
        0,                         // counterAttributes
        pose_table,                // poseTable
    },
};

BattleWeapon weapon_zonbaba_fumituke_attack = {
    NULL,                                      // name
    0,                                         // icon
    ITEM_NULL,                                 // item
    NULL,                                      // description
    100,                                       // baseAccuracy
    0,                                         // baseFpCost
    0,                                         // baseSpCost
    0,                                         // superguardsAllowed
    1.0f,                                      // field_0x14
    1,                                         // mStylishCommandMultiplier
    1,                                         // field_0x19
    1,                                         // mBingoSlotChance
    1,                                         // field_0x1B
    weaponGetPowerDefault,                     // hpDamageFn
    { 8, 0, 0, 0, 0, 0, 0, 0 },                // hpDamageWork
    NULL,                                      // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                // fpDamageWork
    0x01101260,                                // targetClassFlags
    0x21002000,                                // targetPropertyFlags
    0,                                         // element
    11,                                        // field_0x6D
    3,                                         // actionLevel
    2,                                         // field_0x6F
    NULL,                                      // acHelpMsg
    0x00000007,                                // special
    0x000007FF,                                // counterResistanceFlags
    0x00002004,                                // targetWeightingFlags
    0,                                         // sleepChance
    0,                                         // sleepTime
    0,                                         // stopChance
    0,                                         // stopTime
    0,                                         // dizzyChance
    0,                                         // dizzyTime
    0,                                         // poisonChance
    0,                                         // poisonTime
    0,                                         // poisonStrength
    0,                                         // confuseChance
    0,                                         // confuseTime
    0,                                         // electricChance
    0,                                         // electricTime
    0,                                         // dodgyChance
    0,                                         // dodgyTime
    0,                                         // burnChance
    0,                                         // burnTime
    0,                                         // freezeChance
    0,                                         // freezeTime
    0,                                         // sizeChangeChance
    0,                                         // sizeChangeTime
    0,                                         // sizeChangeStrength
    0,                                         // atkChangeChance
    0,                                         // atkChangeTime
    0,                                         // atkChangeStrength
    0,                                         // defChangeChance
    0,                                         // defChangeTime
    0,                                         // defChangeStrength
    0,                                         // allergicChance
    0,                                         // allergicTime
    0,                                         // ohkoChance
    0,                                         // chargeStrength
    0,                                         // fastChance
    0,                                         // fastTime
    0,                                         // slowChance
    0,                                         // slowTime
    0,                                         // frightChance
    0,                                         // galeForceChance
    0,                                         // paybackTime
    0,                                         // holdFastTime
    0,                                         // invisibleChance
    0,                                         // invisibleTime
    0,                                         // hpRegenTime
    0,                                         // hpRegenStrength
    0,                                         // fpRegenTime
    0,                                         // fpRegenStrength
    NULL,                                      // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 10, 0, 0, 0 }, // field_B4
};

BattleWeapon weapon_zonbaba_kamituki_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 8, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020007,                               // special
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

BattleWeapon weapon_zonbaba_cold_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 8, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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
    30,                                       // freezeChance
    3,                                        // freezeTime
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

BattleWeapon weapon_zonbaba_confuze_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 8, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    30,                                       // confuseChance
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

BattleWeapon weapon_zonbaba_sleep_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 8, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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
    30,                                       // sleepChance
    5,                                        // sleepTime
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

BattleWeapon weapon_zonbaba_minimini_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 8, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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
    30,                                       // sizeChangeChance
    3,                                        // sizeChangeTime
    -2,                                       // sizeChangeStrength
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

BattleWeapon weapon_zonbaba_recover = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000007,                               // special
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

static EVT_BEGIN(battle_entry_event)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_map_replayanim, 0, STRING("dontyo"))
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 0, 110, 1080, 0, 93, -2, 1, 0)
    CALL(btlevtcmd_GetStageSize, LW(6), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    MULTIPLY(LW(6), -1)
    CALL(btlevtcmd_GetHomePos, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -4, LW(3), LW(4), LW(5))
    CALL(btlevtcmd_SetPos, -3, LW(6), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -4, LW(6), LW(4), LW(5))
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(3), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), LW(3))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 42)
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 42)
    CALL(btlevtcmd_GetHomePos, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -4, LW(3), LW(4), LW(5))
    SET_VALUE(LW(8), LW(0))
    SUBTRACT(LW(8), LW(3))
    IF_INT_LESS(LW(8), 0)
        MULTIPLY(LW(8), -1)
    END_IF
    CALL(btlevtcmd_GetStageSize, LW(6), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(7), LW(6))
    ADD(LW(7), LW(8))
    MULTIPLY(LW(6), -1)
    MULTIPLY(LW(7), -1)
    CALL(btlevtcmd_SetPos, -3, LW(6), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -4, LW(7), LW(4), LW(5))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetMoveSpeed, -4, FLOAT(6.0f))
        CALL(btlevtcmd_MovePosition, -4, LW(3), LW(4), LW(5), 0, -1, 0)
        CALL(btlevtcmd_GetBodyId, -4, LW(0))
        CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 43)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -3, FLOAT(6.0f))
    CALL(btlevtcmd_MovePosition, -3, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_I_Y"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_F_3"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(10)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(5)
        CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_I_O"))
        CALL(btlevtcmd_SetFallAccel, -3, FLOAT(0.3f))
        CALL(btlevtcmd_GetPos, -3, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_snd_se, -3, STRING("SFX_VOICE_MARIO_SURPRISED2_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_JumpPosition, -3, LW(0), LW(1), LW(2), 25, -1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(5)
        CALL(btlevtcmd_SetFallAccel, -4, FLOAT(0.3f))
        CALL(btlevtcmd_GetPos, -4, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPosition, -4, LW(0), LW(1), LW(2), 25, -1)
        CALL(btlevtcmd_GetBodyId, -4, LW(0))
        CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 5)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_GNB_APPEAR1"), 0)
    CALL(evt_btl_camera_shake_h, 1, 8, 0, 20, 13)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_I_Y"))
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 43)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, -233, 45, 452, 56, 125, 37, 60, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_msg_print, 2, STRING("tik_boss_12"), 0, -2)
    WAIT_MS(300)
    CALL(evt_msg_print_battle_party, STRING("tik_boss_13"))
    CALL(btlevtcmd_StartWaitEvent, -3)
    CALL(btlevtcmd_StartWaitEvent, -4)
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 3)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetTalkBodyId, -2, 1)
    CALL(btlevtcmd_SetBaseScale, -2, FLOAT(1.3f), FLOAT(1.3f), FLOAT(0.5f))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

EVT_BEGIN(zonbaba_damage_sub_event)
    CALL(btlevtcmd_GetDamagePartsId, -2, LW(11))
    IF_INT_EQUAL(LW(11), 1)
        CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("GNB_D_2"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("GNB_D_1"))
    END_IF
    CALL(btlevtcmd_GetDamage, LW(10), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_ATTACK_MISS2"), LW(0), LW(1), LW(2), 0)
    END_IF
    WAIT_MS(1000)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_X_1"))
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_DOWN1"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, LW(15))
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_wait_move_end)
    CALL(evt_btl_camera_set_mode, 1, 3)
    SET_VALUE(LW(1), 160)
    SET_VALUE(LW(2), 536)
    CALL(btlevtcmd_GetStageAudienceAreaSize, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    ADD(LW(1), LW(5))
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(5))
    ADD(LW(2), LW(5))
    ADD(LW(2), 80)
    SET_VALUE(LW(5), LW(1))
    SUBTRACT(LW(5), 37)
    SET_VALUE(LW(6), LW(2))
    SUBTRACT(LW(6), 511)
    CALL(evt_btl_camera_set_moveto, 1, 93, LW(1), LW(2), 93, LW(5), LW(6), 30, 0)
    WAIT_FRAMES(90)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("GNB_X_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("GNB_X_1"))
    CALL(evt_msg_print, 2, STRING("tik_boss_16"), 0, -2)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, -38, 33, 1005, -38, 92, 13, 60, 0)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_DOWN2"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_D_3"))
    WAIT_FRAMES(42)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_1"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 25)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_1"))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(5), 100)
        SET_VALUE(LW(4), 120)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, LW(5), 0)
        LOOP(40)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(4))
            SUBTRACT(LW(4), 3)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    SUBTRACT(LW(2), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -1)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_GNB_DOWN3"), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_2"))
    CALL(evt_btl_camera_shake_h, 1, 8, 0, 30, 13)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_3"))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    WAIT_FRAMES(60)
    CALL(btlevtcmd_KillUnit, -2, 1)
    CALL(evt_btl_camera_set_prilimit, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 50)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        END_IF
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 25)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
            CALL(evt_msg_print, 2, STRING("tik_boss_15"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
        END_IF
    END_SWITCH
    LABEL(99)
    RETURN
EVT_END()

static USER_FUNC(_leg_smoke_effect) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    effKemuri9N64Entry(1, 18, 90, x, y, z, 60.0f, 0.0f);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(fumituke_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_W_2"))
    WAIT_FRAMES(57)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -300, 0, -30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -300)
        ADD(LW(2), -30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_FRAMES(46)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -300, 0, 30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -300)
        ADD(LW(2), 30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_3"))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_ARM_UP1"), EVTDAT_POINTER_MAX, 0, -320, 30, 30, EVTDAT_POINTER_MAX)
        WAIT_MS(700)
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_ARM_DOWN1"), EVTDAT_POINTER_MAX, 0, -360, 50, 30, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    WAIT_FRAMES(45)
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
    GOTO(97)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -400, 0, -5, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -400)
        ADD(LW(2), -5)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    WAIT_FRAMES(3)
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_FRAMES(7)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 30, 13)
    WAIT_FRAMES(35)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_W_3"))
    WAIT_FRAMES(51)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -180, 0, 30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -180)
        ADD(LW(2), 30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_FRAMES(46)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -100, 0, 30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -100)
        ADD(LW(2), -30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_FRAMES(7)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(kamituki_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_BITE1"), EVTDAT_POINTER_MAX, 0, -220, 80, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(250)
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_BITE2"), EVTDAT_POINTER_MAX, 0, -260, 40, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_2"))
    WAIT_FRAMES(20)
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
        GOTO(99)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(97)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    LABEL(99)
    WAIT_FRAMES(20)
    RETURN
EVT_END()

EVT_BEGIN(breath1_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_1"))
    SET_VALUE(LW(10), 120)
    SET_VALUE(LW(11), 30)
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(6), 360)
    ADD(LW(7), 30)
    CALL(btlevtcmd_GetPos, -2, LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), 360)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), LW(10))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_NECK_UP1"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(105)
        CALL(btlevtcmd_snd_se_offset, -2, LW(13), EVTDAT_POINTER_MAX, 0, -300, 0, 0, EVTDAT_POINTER_MAX)
        ADD(LW(11), 45)
        SUBTRACT(LW(8), 10)
        SWITCH(LW(9))
            CASE_EQUAL(PTR(&weapon_zonbaba_cold_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 3, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(PTR(&weapon_zonbaba_confuze_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 4, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(PTR(&weapon_zonbaba_sleep_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 5, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(PTR(&weapon_zonbaba_minimini_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 6, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
        END_SWITCH
    END_INLINE_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SUBTRACT(LW(12), LW(6))
    IF_INT_LESS(LW(12), 0)
        MULTIPLY(LW(12), -1)
    END_IF
    ADD(LW(12), 20)
    MULTIPLY(LW(12), LW(11))
    DIVIDE(LW(12), LW(10))
    ADD(LW(12), 105)
    WAIT_FRAMES(LW(12))
    SET_VALUE(LW(10), 0)
    LABEL(10)
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
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        WAIT_FRAMES(20)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    WAIT_FRAMES(84)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(breath2_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A2_1"))
    WAIT_MS(833)
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_NECK_UP1"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A2_3"))
    CALL(btlevtcmd_snd_se_offset, -2, LW(13), EVTDAT_POINTER_MAX, 0, -300, 0, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(10), 120)
    SET_VALUE(LW(11), 30)
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(6), 360)
    ADD(LW(7), 30)
    CALL(btlevtcmd_GetPos, -2, LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), 360)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), LW(10))
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(20)
        ADD(LW(11), 45)
        SUBTRACT(LW(8), 10)
        SWITCH(LW(9))
            CASE_EQUAL(PTR(&weapon_zonbaba_cold_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 3, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(PTR(&weapon_zonbaba_confuze_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 4, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(PTR(&weapon_zonbaba_sleep_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 5, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(PTR(&weapon_zonbaba_minimini_attack))
            CALL(evt_eff, 0, STRING("gonbaba_breath"), 6, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
        END_SWITCH
    END_INLINE_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SUBTRACT(LW(12), LW(6))
    IF_INT_LESS(LW(12), 0)
        MULTIPLY(LW(12), -1)
    END_IF
    ADD(LW(12), 20)
    MULTIPLY(LW(12), LW(11))
    DIVIDE(LW(12), LW(10))
    ADD(LW(12), 20)
    WAIT_FRAMES(LW(12))
    SET_VALUE(LW(10), 0)
    LABEL(10)
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
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        WAIT_FRAMES(20)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    WAIT_FRAMES(84)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(recover_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(btlevtcmd_GetUnitWork, -2, 2, 1)
        CALL(evt_msg_print, 2, STRING("tik_boss_14"), 0, -2)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, -2, 1, LW(0))
    END_IF
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_RecoverHp, -2, 1, 20)
    CALL(btlevtcmd_GetHitPos, -2, 1, LW(0), LW(1), LW(2))
    ADD(LW(0), -27)
    ADD(LW(1), 15)
    ADD(LW(2), 10)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 20, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(40)
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
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_DrawLots, LW(0), 6, 25, 20, 10, 10, 10, 10)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_fumituke_attack))
            CALL_CHILD(fumituke_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_kamituki_attack))
            CALL_CHILD(kamituki_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_COLD1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_cold_attack))
            CALL_CHILD(breath2_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_confuze_attack))
            CALL_CHILD(breath1_attack_event)
            CASE_EQUAL(4)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_sleep_attack))
            CALL_CHILD(breath2_attack_event)
            CASE_EQUAL(5)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_minimini_attack))
            CALL_CHILD(breath1_attack_event)
        END_SWITCH
        CASE_EQUAL(1)
        SET_VALUE(LW(1), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(1), 20)
        END_IF
        CALL(btlevtcmd_DrawLots, LW(0), 7, 25, 20, 10, 10, 10, 10, LW(1))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_fumituke_attack))
            CALL_CHILD(fumituke_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_kamituki_attack))
            CALL_CHILD(kamituki_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_COLD1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_cold_attack))
            CALL_CHILD(breath2_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_confuze_attack))
            CALL_CHILD(breath1_attack_event)
            CASE_EQUAL(4)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_sleep_attack))
            CALL_CHILD(breath2_attack_event)
            CASE_EQUAL(5)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_minimini_attack))
            CALL_CHILD(breath1_attack_event)
            CASE_EQUAL(6)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_recover))
            CALL_CHILD(recover_event)
        END_SWITCH
        CASE_EQUAL(2)
        SET_VALUE(LW(1), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(1), 50)
        END_IF
        CALL(btlevtcmd_DrawLots, LW(0), 7, 25, 20, 10, 10, 10, 10, LW(1))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_fumituke_attack))
            CALL_CHILD(fumituke_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_kamituki_attack))
            CALL_CHILD(kamituki_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_COLD1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_cold_attack))
            CALL_CHILD(breath2_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_confuze_attack))
            CALL_CHILD(breath1_attack_event)
            CASE_EQUAL(4)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_sleep_attack))
            CALL_CHILD(breath2_attack_event)
            CASE_EQUAL(5)
            SET_VALUE(LW(13), STRING("SFX_BOSS_GNB_BREATH_CHANGE1"))
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_minimini_attack))
            CALL_CHILD(breath1_attack_event)
            CASE_EQUAL(6)
            SET_VALUE(LW(9), PTR(&weapon_zonbaba_recover))
            CALL_CHILD(recover_event)
        END_SWITCH
    END_SWITCH
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

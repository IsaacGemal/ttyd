#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "driver/animdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include <string.h>
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static void _fire_point_callback(BattleUnitPart* part, s32 group, void* mtx);
static USER_FUNC(_set_draw_callback);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[2];
static s32 init_event[57];
static s32 damage_event[118];
static s32 phase_event[126];
static s32 attack_event[175];
static s32 wait_event[6];
static s32 dead_event[52];
static s32 explosion_event[247];

// clang-format off
extend_data(200,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(201,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_giant_bomb = {
    UNIT_BOB_ULK,               // mUnitType
    "btl_un_giant_bomb",        // name
    10,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    34,                         // mLevel
    2,                          // mBonusExp
    5,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    9999,                       // field_0x14
    54,                         // width
    70,                         // height
    { 7, 70 },                  // statusIconOffset
    { 0.0f, 35.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 27.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 27.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 0.0f, 0.0f, 0.0f },       // kissHitOffset
    { 0.0f, 35.0f, 0.0f },      // cutBaseOffset
    54.0f,                      // cutWidth
    70.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_HEAVYB_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    2,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleWeapon weapon_heavy_bomb_super_explosion = {
    NULL,                                       // name
    0,                                          // icon
    ITEM_NULL,                                  // item
    NULL,                                       // description
    100,                                        // baseAccuracy
    0,                                          // baseFpCost
    0,                                          // baseSpCost
    0,                                          // superguardType
    1.0f,                                       // field_0x14
    1,                                          // mStylishCommandMultiplier
    1,                                          // field_0x19
    1,                                          // mBingoSlotChance
    1,                                          // field_0x1B
    weaponGetPowerDefault,                      // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x02101020,                                 // targetClassFlags
    0x20000000,                                 // targetPropertyFlags
    3,                                          // element
    0,                                          // field_0x6D
    3,                                          // actionLevel
    2,                                          // field_0x6F
    NULL,                                       // acHelpMsg
    0x00022087,                                 // special
    0x000007FF,                                 // counterResistanceFlags
    0x00000004,                                 // targetWeightingFlags
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

static BattleWeapon weapon_heavy_bomb_guard = {
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
    0x02004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020008,                               // special
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
    100,                                      // defChangeChance
    3,                                        // defChangeTime
    3,                                        // defChangeStrength
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

static BattleWeapon weapon_heavy_bomb_guard_2 = {
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
    0x02004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020008,                               // special
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
    100,                                      // atkChangeChance
    3,                                        // atkChangeTime
    3,                                        // atkChangeStrength
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

static BattleWeapon weapon_heavy_bomb_guard_3 = {
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
    0x02004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020008,                               // special
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
    9,                                        // chargeStrength
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

static BattleUnitDefense defence = { { 2, 2, 2, 2, 2 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    70,  // sleepRate
    55,  // stopRate
    50,  // dizzyRate
    100, // poisonRate
    50,  // confuseRate
    100, // electricRate
    100, // burnRate
    90,  // freezeRate
    100, // hugeRate
    70,  // tinyRate
    100, // attackUpRate
    70,  // attackDownRate
    100, // defenseUpRate
    85,  // defenseDownRate
    50,  // allergicRate
    60,  // frightRate
    10,  // galeForceRate
    100, // fastRate
    50,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    20,  // instaKillRate
};

static BattleDispPose pose_table_3[16] = {
    { 0x01, "N_1" },
    { 0x09, "Y_1" },
    { 0x02, "Y_1" },
    { 0x03, "K_1" },
    { 0x04, "X_1" },
    { 0x05, "K_1" },
    { 0x09, "Z_1" },
    { 0x1C, "S_1" },
    { 0x1D, "D_1" },
    { 0x23, "S_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "S_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_3_fire[16] = {
    { 0x01, "N_1" },
    { 0x09, "Y_1" },
    { 0x02, "Y_1" },
    { 0x03, "K_1" },
    { 0x04, "X_1" },
    { 0x05, "K_1" },
    { 0x09, "Z_1" },
    { 0x1C, "S_1" },
    { 0x1D, "D_1" },
    { 0x23, "S_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "S_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_2[16] = {
    { 0x01, "N_2" },
    { 0x09, "Y_2" },
    { 0x02, "Y_2" },
    { 0x03, "K_2" },
    { 0x04, "X_2" },
    { 0x05, "K_2" },
    { 0x09, "Z_2B" },
    { 0x1C, "S_2B" },
    { 0x1D, "D_2" },
    { 0x23, "S_2B" },
    { 0x1F, "S_2B" },
    { 0x27, "D_2" },
    { 0x38, "X_2" },
    { 0x39, "X_2" },
    { 0x41, "S_2B" },
    { 0x45, "S_2B" },
};

static BattleDispPose pose_table_2_fire[16] = {
    { 0x01, "N_2" },
    { 0x09, "Y_2" },
    { 0x02, "Y_2" },
    { 0x03, "K_2" },
    { 0x04, "X_2" },
    { 0x05, "K_2" },
    { 0x09, "Z_2A" },
    { 0x1C, "S_2A" },
    { 0x1D, "D_2" },
    { 0x23, "S_2A" },
    { 0x1F, "S_2A" },
    { 0x27, "D_2" },
    { 0x38, "X_2" },
    { 0x39, "X_2" },
    { 0x41, "S_2A" },
    { 0x45, "S_2A" },
};

static BattleDispPose pose_table_1[16] = {
    { 0x01, "N_3" },
    { 0x09, "Y_3" },
    { 0x02, "Y_3" },
    { 0x03, "K_3" },
    { 0x04, "X_3" },
    { 0x05, "K_3" },
    { 0x09, "Z_3B" },
    { 0x1C, "S_3B" },
    { 0x1D, "D_3" },
    { 0x23, "S_3B" },
    { 0x1F, "S_3B" },
    { 0x27, "D_3" },
    { 0x38, "X_3" },
    { 0x39, "X_3" },
    { 0x41, "S_3B" },
    { 0x45, "S_3B" },
};

static BattleDispPose pose_table_1_fire[16] = {
    { 0x01, "N_3" },
    { 0x09, "Y_3" },
    { 0x02, "Y_3" },
    { 0x03, "K_3" },
    { 0x04, "X_3" },
    { 0x05, "K_3" },
    { 0x09, "Z_3A" },
    { 0x1C, "S_3A" },
    { 0x1D, "D_3" },
    { 0x23, "S_3A" },
    { 0x1F, "S_3A" },
    { 0x27, "D_3" },
    { 0x38, "X_3" },
    { 0x39, "X_3" },
    { 0x41, "S_3A" },
    { 0x45, "S_3A" },
};

static BattleDispPose pose_table_0_fire[16] = {
    { 0x01, "N_4" },
    { 0x09, "Y_4" },
    { 0x02, "Y_4" },
    { 0x03, "K_4" },
    { 0x04, "X_4" },
    { 0x05, "K_4" },
    { 0x09, "Z_4A" },
    { 0x1C, "S_4A" },
    { 0x1D, "D_4" },
    { 0x23, "S_4A" },
    { 0x1F, "S_4A" },
    { 0x27, "D_4" },
    { 0x38, "X_4" },
    { 0x39, "X_4" },
    { 0x41, "S_4A" },
    { 0x45, "S_4A" },
};

static BattleDispPose pose_table_0[16] = {
    { 0x01, "N_4" },
    { 0x09, "Y_4" },
    { 0x02, "Y_4" },
    { 0x03, "K_4" },
    { 0x04, "X_4" },
    { 0x05, "K_4" },
    { 0x09, "Z_4B" },
    { 0x1C, "S_4B" },
    { 0x1D, "D_4" },
    { 0x23, "S_4B" },
    { 0x1F, "S_4B" },
    { 0x27, "D_4" },
    { 0x38, "X_4" },
    { 0x39, "X_4" },
    { 0x41, "S_4B" },
    { 0x45, "S_4B" },
};

static BattleDispPose pose_table_3_to_2[13] = {
    { 0x01, "B_4" },
    { 0x09, "B_1" },
    { 0x02, "B_1" },
    { 0x03, "B_7" },
    { 0x04, "B_10" },
    { 0x05, "B_7" },
    { 0x1C, "B_1" },
    { 0x1F, "B_1" },
    { 0x27, "B_1" },
    { 0x38, "B_10" },
    { 0x39, "B_10" },
    { 0x41, "B_1" },
    { 0x45, "B_1" },
};

static BattleDispPose pose_table_2_to_1[13] = {
    { 0x01, "B_5" },
    { 0x09, "B_2" },
    { 0x02, "B_2" },
    { 0x03, "B_8" },
    { 0x04, "B_11" },
    { 0x05, "B_8" },
    { 0x1C, "B_2" },
    { 0x1F, "B_2" },
    { 0x27, "B_2" },
    { 0x38, "B_11" },
    { 0x39, "B_11" },
    { 0x41, "B_2" },
    { 0x45, "B_2" },
};

static BattleDispPose pose_table_1_to_0[13] = {
    { 0x01, "B_6" },
    { 0x09, "B_3" },
    { 0x02, "B_3" },
    { 0x03, "B_9" },
    { 0x04, "B_12" },
    { 0x05, "B_9" },
    { 0x1C, "B_3" },
    { 0x1F, "B_3" },
    { 0x27, "B_3" },
    { 0x38, "B_12" },
    { 0x39, "B_12" },
    { 0x41, "B_3" },
    { 0x45, "B_3" },
};

static BattleDataEntry data_table[3] = {
    { 0x31, dead_event },
    { 0x34, explosion_event },
    { 0x00, NULL },
};

// TODO: type
static u32 pose_sound_timing_table[45] = {
    (u32)"S_3A", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3",
    0x00000001, (u32)"S_3A", 0x3E888889, 0x00000000,
    (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_3A", 0x3F088889,
    0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_3A",
    0x3F4CCCCD, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001,
    (u32)"S_4A", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3",
    0x00000001, (u32)"S_4A", 0x3E2AAAAB, 0x00000000,
    (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_4A", 0x3EAAAAAB,
    0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_4A",
    0x3F000000, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001,
};

static BattleUnitKindPart parts[2] = {
    {
        1,                      // partNum
        "btl_un_giant_bomb",    // partName
        "c_giant",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 50.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 65.0f, 0.0f },  // hitCursorBaseOffset
        65,                     // field_0x30
        70,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x00000009,             // attributes
        0,                      // counterAttributes
        pose_table_3,           // poseTable
    },
    {
        2,                     // partNum
        "ジャイアントボム",            // partName
        "c_giant",             // modelName
        { 0.0f, 80.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        15,                    // field_0x30
        15,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x02010000,            // attributes
        0,                     // counterAttributes
        pose_table_3,          // poseTable
    },
};

static USER_FUNC(_set_draw_callback) {
    BattleWork* wp = _battleWorkPointer;
    s32 id = BattleTransID(event, -2);
    BattleUnit* unit = BattleGetUnitPtr(wp, id);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, 1);

    animPoseSetDispCallBack(part->poseId, (void*)_fire_point_callback, part);
    part = BtlUnit_GetPartsPtr(unit, 2);
    part->attributes |= 0x01000000;
    return 2;
}

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 3)
    CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("F_1"))
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
    CALL(_set_draw_callback)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
            CALL(evt_snd_sfxoff, LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(2)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2))
                CASE_EQUAL(1)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1))
                CASE_EQUAL(0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_0))
            END_SWITCH
        END_IF
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
        SWITCH(LW(0))
            CASE_OR_EQUAL(27)
            CASE_OR_EQUAL(24)
            CASE_OR_EQUAL(25)
            CASE_OR_EQUAL(52)
            CASE_OR_EQUAL(46)
            CASE_OR_EQUAL(50)
            CASE_OR_EQUAL(54)
            GOTO(50)
            CASE_END
        END_SWITCH
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
    LABEL(50)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 29)
    CALL_CHILD(dead_event)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_PhaseEventStartDeclare, -2)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("K_4"))
                CALL(btlevtcmd_AfterReactionEntry, -2, 52)
                RETURN
            END_IF
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_WAIT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(3)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_3_to_2))
                CASE_EQUAL(2)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2_to_1))
                CASE_EQUAL(1)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1_to_0))
            END_SWITCH
            CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
            WAIT_FRAMES(40)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            SUBTRACT(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 1, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(2)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2_fire))
                CASE_EQUAL(1)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1_fire))
                CASE_EQUAL(0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_0_fire))
            END_SWITCH
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_1"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_2"))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_3"))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_4"))
        END_SWITCH
        WAIT_FRAMES(40)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_WAIT1"), EVTDAT_POINTER_MAX, 0, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        CALL(btlevtcmd_OnUnitFlag, -2, 16)
    ELSE
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_1"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_2"))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_3"))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_4"))
        END_SWITCH
        WAIT_FRAMES(40)
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(12), PTR(&weapon_heavy_bomb_guard))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
            CASE_EQUAL(1)
            SET_VALUE(LW(12), PTR(&weapon_heavy_bomb_guard_2))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 2)
            CASE_EQUAL(2)
            SET_VALUE(LW(12), PTR(&weapon_heavy_bomb_guard_3))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
        END_SWITCH
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(12), 256, LW(5))
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(evt_snd_sfxoff, LW(0))
    END_IF
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    SWITCH(LW(0))
        CASE_OR_EQUAL(27)
        CASE_OR_EQUAL(24)
        CASE_OR_EQUAL(25)
        CASE_OR_EQUAL(52)
        CASE_OR_EQUAL(46)
        CASE_OR_EQUAL(50)
        CASE_OR_EQUAL(54)
        GOTO(50)
        CASE_END
    END_SWITCH
    LABEL(10)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core)
    RETURN
    LABEL(50)
    CALL(btlevtcmd_AfterReactionEntry, -2, 52)
    RETURN
EVT_END()

static EVT_BEGIN(explosion_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    CALL(evt_snd_sfxoff, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    DIVIDE(LW(5), 2)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    ADD(LW(1), LW(5))
    CALL(evt_eff, 0, STRING("bomb"), 5, LW(0), LW(1), LW(2), FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_StageDispellFog)
    WAIT_MS(300)
    CALL(evt_eff, 0, STRING("bomb"), 4, -40, 30, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, 40, 90, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, 40, 30, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, -40, 90, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, -100, 60, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, 100, 60, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_heavy_bomb_super_explosion))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_heavy_bomb_super_explosion), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_heavy_bomb_super_explosion))
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_heavy_bomb_super_explosion))
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_heavy_bomb_super_explosion), 256, LW(5))
    IF_INT_EQUAL(LW(5), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_heavy_bomb_super_explosion), 256, LW(5))
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(99)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static void _fire_point_callback(BattleUnitPart* part, s32 group, void* mtx) {
    BattleUnit* unit = part->mOwner;
    s32 pose;
    const char* name;

    if (unit->work[0] == 0) {
        return;
    }
    pose = part->poseId;
    switch (unit->work[1]) {
        case 0:
            name = "locator11";
            break;
        case 1:
            name = "locator10";
            break;
        case 2:
            name = "locator9";
            break;
        case 3:
        default:
            name = "locator8";
            break;
    }
    if (strcmp(name, animPoseGetGroupName(pose, group)) != 0) {
        return;
    }
    {
        BattleUnitPart* part2 = BtlUnit_GetPartsPtr(unit, 2);
        s32 pose2 = part2->poseId;
        s32 shapeId = animPoseGetShapeIdx(pose2, "pPlaneShape3");
        animPoseDrawShape(pose2, shapeId);
    }
}
// clang-format on
#pragma force_active off

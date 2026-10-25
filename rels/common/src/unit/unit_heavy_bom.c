#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "driver/animdrv.h"
#include "manager/evtmgr_cmd.h"
#include <string.h>
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static void _fire_point_callback(void* userData, s32 group, Mtx mtx);
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
extend_data(unit_heavy_bom_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_heavy_bom_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_BULKY_BOB_OMB,         // mUnitType
    "btl_un_heavy_bom",         // name
    6,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    22,                         // mLevel
    0,                          // mBonusExp
    4,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    0,                          // baseCoin
    40,                         // escapeRate
    9999,                       // renzokuSoftCap
    54,                         // width
    70,                         // height
    { 7, 70 },                  // statusIconOffset
    { 0.0f, 35.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 27.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 27.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 27.0f, 45.5f, 0.0f },     // kissHitOffset
    { 0.0f, 35.0f, 0.0f },      // cutBaseOffset
    54.0f,                      // cutWidth
    70.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // shotDamageRate
    0,                          // itemStealParam
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x02101020,                                 // targetClassFlags
    0x20000000,                                 // targetPropertyFlags
    3,                                          // element
    0,                                          // damagePattern
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
    { 0, 50, 50, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
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
    0,                                        // damagePattern
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
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
    0,                                        // damagePattern
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
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
    0,                                        // damagePattern
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
    3,                                        // chargeStrength
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

static BattleUnitDefense defence = { { 1, 1, 1, 1, 1 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    90,  // sleepRate
    75,  // stopRate
    70,  // dizzyRate
    0,   // poisonRate
    70,  // confuseRate
    150, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    80,  // tinyRate
    100, // attackUpRate
    80,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    70,  // allergicRate
    80,  // frightRate
    20,  // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
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

static BtlUnitPoseSound pose_sound_timing_table[9] = {
    { "S_3A", 0.01f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { "S_3A", 0.26666668f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { "S_3A", 0.53333336f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { "S_3A", 0.8f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { "S_4A", 0.01f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { "S_4A", 0.16666667f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { "S_4A", 0.33333334f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { "S_4A", 0.5f, NULL, "SFX_ENM_HEAVYB_WAIT3", 0, 1 },
    { NULL, 0.0f, NULL, NULL, 0, 1 },
};

static BattleUnitKindPart parts[2] = {
    {
        1,                      // partNum
        "btl_un_heavy_bom",     // partName
        "c_heavy",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 50.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 65.0f, 0.0f },  // hitCursorBaseOffset
        65,                     // shadowSize
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
        "ヘビーボム",               // partName
        "c_heavy",             // modelName
        { 0.0f, 80.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        15,                    // shadowSize
        15,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x02010000,            // attributes
        0,                     // counterAttributes
        pose_table_3,          // poseTable
    },
};

#include "rel/common/unit_heavy_bom/_set_draw_callback.h"


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

#include "rel/common/unit_heavy_bom/_fire_point_callback.h"


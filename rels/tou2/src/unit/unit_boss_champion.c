#include "unit_boss_champion.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_icon.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_weapon_power.h"
#include "eff/eff_fall.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(down_ceil);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[52];
static s32 entry_event[354];
static s32 damage_event[75];
static s32 dead_event[90];
static s32 phase_event[450];
static s32 attack_event[221];
s32 drop_kick_attack_event[299];
s32 screw_attack_event[295];
s32 flying_chop_attack_event[326];
s32 foot_stamp_attack_event[467];
s32 healing_event[209];
s32 roof_shake_attack_event[386];
static s32 wait_event[6];

// clang-format off
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// BattleUnitKind (0xC4). The header's field_0x4 and field_0x10..0xB8 are unnamed u8[] regions that
// hold the name string, floats, the SFX names and &regist, so it cannot be written as a struct
BattleUnitKind unit_boss_champion = {
    UNIT_RAWK_HAWK,             // mUnitType
    "btl_un_champion",          // name
    40,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    34,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    8,                          // field_0x14
    50,                         // width
    64,                         // height
    { 3, 64 },                  // statusIconOffset
    { 0.0f, 32.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 8.0f, 36.0f, 0.0f },      // talkTogeBaseOffset
    { 25.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 25.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 10.0f, 48.0f, 10.0f },    // kissHitOffset
    { 0.0f, 32.0f, 0.0f },      // cutBaseOffset
    50.0f,                      // cutWidth
    64.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_BLED_DAMAGED1",   // damageSfxName
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

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

// BattleVulnerableStatus (22 bytes); the extra 2 bytes are word-alignment padding, so the object is 0x18.
static BattleVulnerableStatus regist = {
    60,  // sleepRate
    50,  // stopRate
    60,  // dizzyRate
    100, // poisonRate
    60,  // confuseRate
    100, // electricRate
    100, // burnRate
    40,  // freezeRate
    0,   // hugeRate
    85,  // tinyRate
    100, // attackUpRate
    85,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    60,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "K_1" },
    { 0x1C, "S_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "D_1" },
    { 0x27, "D_1" },
    { 0x32, "Z_6" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

BattleDispPose pose_table_roof[18] = {
    { 0x01, "N_2" },
    { 0x02, "Y_2" },
    { 0x09, "Y_2" },
    { 0x05, "K_2" },
    { 0x04, "X_2" },
    { 0x03, "K_2" },
    { 0x1C, "S_8" },
    { 0x1D, "Q_2" },
    { 0x1E, "Q_2" },
    { 0x1F, "D_2" },
    { 0x27, "D_2" },
    { 0x32, "Z_8" },
    { 0x2A, "W_2" },
    { 0x28, "W_2" },
    { 0x38, "X_2" },
    { 0x39, "X_2" },
    { 0x41, "T_8" },
    { 0x45, "S_8" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_champion",     // partName
        "c_bred_h",            // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 50.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 55.0f, 0.0f }, // hitCursorBaseOffset
        35,                    // field_0x30
        70,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

BattleWeapon weapon_champion_drop_kick_attack = {
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
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0,                                        // counterResistanceFlags
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

BattleWeapon weapon_champion_screw_attack = {
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
    0x02101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0,                                        // counterResistanceFlags
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

BattleWeapon weapon_champion_flying_chop_attack = {
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
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0,                                        // counterResistanceFlags
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

BattleWeapon weapon_champion_foot_stamp_attack = {
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
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0,                                        // counterResistanceFlags
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

BattleWeapon weapon_champion_healing = {
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
    { 5, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0,                                        // counterResistanceFlags
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

BattleWeapon weapon_champion_roof_shake_attack = {
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
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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

BattleWeapon weapon_champion_roof_shake_last_attack = {
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
    NULL,                                       // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x02101260,                                 // targetClassFlags
    0x20000000,                                 // targetPropertyFlags
    0,                                          // element
    27,                                         // field_0x6D
    3,                                          // actionLevel
    2,                                          // field_0x6F
    NULL,                                       // acHelpMsg
    0x00020087,                                 // special
    0x000007FF,                                 // counterResistanceFlags
    0x80002004,                                 // targetWeightingFlags
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
    { 0, 25, 25, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

__declspec(section ".rodata") Vec vec3_tou2_0000b738 = {0.0f, 250.0f, 0.0f};

EVT_BEGIN(to_ceiling)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1D"))
    SET_VALUE(LW(1), 60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_8"))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_GRASP_CAILING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_roof))
    CALL(btlevtcmd_SetTalkPoseType, -2, 65)
    CALL(btlevtcmd_SetStayPoseType, -2, 43)
    CALL(btlevtcmd_OnAttribute, -2, 2)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
    CALL(btlevtcmd_SetKissHitOffset, -2, 16, 48, 10)
    CALL(btlevtcmd_SetCutBaseOffset, -2, -2, -38, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, -44, 0)
    RETURN
EVT_END()

EVT_BEGIN(to_floor)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_2"))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 50, 13)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_SetTalkPoseType, -2, 65)
    CALL(btlevtcmd_SetStayPoseType, -2, 43)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 6291456)
    CALL(btlevtcmd_SetKissHitOffset, -2, 10, 48, 10)
    CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 32, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    RETURN
EVT_END()

USER_FUNC(ceil_exist) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;

    if (wp->field_180C0.stageFlags & 0x20) {
        evtSetValue(event, args[0], 0);
    } else {
        evtSetValue(event, args[0], 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(init_ceil) {
    u8* wp = &_battleWorkPointer->field_180C0.stageFlags;
    BattleStageObject* body = BattleSearchObjectPtr("if_body");
    BattleStageObject* wire = BattleSearchObjectPtr("if_wire");

    wire->mPosition = vec3_tou2_0000b738;
    body->mPosition = wire->mPosition;
    *wp |= 0x20;
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(init_ceil)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_BLED_MOVE1L"), STRING("SFX_BOSS_BLED_MOVE1R"), 0, 15, 15)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_BLED_MOVE1L"), STRING("SFX_BOSS_BLED_MOVE1R"), 0, 8, 8)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    IF_INT_LESS(GSW(0), 158)
        IF_INT_EQUAL(GSWF(2513), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_6"))
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_6"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_6"))
            WAIT_FRAMES(120)
            SET_VALUE(GSWF(2513), 1)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, -102, 83, 334, -102, 59, 0, 30, 11)
            WAIT_FRAMES(37)
            CALL(evt_msg_print_battle_party, STRING("stg3_tou_432"))
            WAIT_MS(300)
            CALL(evt_btl_camera_set_moveto, 0, 81, 83, 334, 81, 59, 0, 30, 11)
            WAIT_FRAMES(37)
            CALL(evt_msg_print, 2, STRING("stg3_tou_433"), 0, -2)
            WAIT_MS(200)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_MS(200)
            CALL(evt_msg_print_battle_party, STRING("stg3_tou_434"))
            WAIT_MS(200)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 81, 83, 334, 81, 59, 0, 1, 11)
            WAIT_FRAMES(7)
            CALL(evt_msg_print, 2, STRING("stg3_tou_435"), 0, -2)
            WAIT_MS(200)
            CALL(evt_btl_camera_set_moveto, 0, -111, 67, 275, -111, 49, 0, 1, 11)
            WAIT_FRAMES(7)
            CALL(evt_msg_print_battle_party, STRING("stg3_tou_435_01"))
            WAIT_MS(200)
            CALL(evt_btl_camera_set_moveto, 0, 74, 67, 275, 74, 49, 0, 30, 11)
            WAIT_FRAMES(37)
            CALL(evt_msg_print, 2, STRING("stg3_tou_435_02"), 0, -2)
            WAIT_MS(200)
        ELSE
            WAIT_FRAMES(60)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
            WAIT_FRAMES(60)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, -102, 83, 334, -102, 59, 0, 30, 11)
            WAIT_FRAMES(37)
            CALL(evt_msg_print_battle_party, STRING("stg3_tou_436"))
            WAIT_MS(200)
            CALL(evt_btl_camera_set_moveto, 0, 81, 83, 334, 81, 59, 0, 30, 11)
            WAIT_FRAMES(37)
            CALL(evt_msg_print, 2, STRING("stg3_tou_436_01"), 0, -2)
            WAIT_MS(200)
        END_IF
    ELSE
        IF_INT_LESS(GSW(0), 172)
        ELSE
            SET_VALUE(GSWF(2514), 0)
            DEBUG_PUT_MSG(STRING("チャンピオン　再挑戦"))
            DEBUG_PUT_REG(GSWF(2515))
            IF_INT_EQUAL(GSWF(2503), 0)
                WAIT_FRAMES(60)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
                WAIT_FRAMES(60)
                CALL(evt_btl_camera_set_mode, 0, 3)
                CALL(evt_btl_camera_set_moveto, 0, -102, 83, 334, -102, 59, 0, 30, 11)
                WAIT_FRAMES(37)
                CALL(evt_msg_print_battle_party, STRING("stg3_tou_re_116"))
                WAIT_MS(200)
                CALL(evt_btl_camera_set_moveto, 0, 81, 83, 334, 81, 59, 0, 30, 11)
                WAIT_FRAMES(37)
                CALL(evt_msg_print, 2, STRING("stg3_tou_re_123"), 0, -2)
                WAIT_MS(200)
            ELSE
                WAIT_FRAMES(60)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
                WAIT_FRAMES(60)
                CALL(evt_btl_camera_set_mode, 0, 3)
                CALL(evt_btl_camera_set_moveto, 0, -102, 83, 334, -102, 59, 0, 30, 11)
                WAIT_FRAMES(37)
                CALL(evt_msg_print_battle_party, STRING("stg3_tou_re_124"))
                WAIT_MS(200)
                CALL(evt_btl_camera_set_moveto, 0, 81, 83, 334, 81, 59, 0, 30, 11)
                WAIT_FRAMES(37)
                CALL(evt_msg_print, 2, STRING("stg3_tou_re_131"), 0, -2)
                WAIT_MS(200)
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseFromTable, -4, LW(0))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(200)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(btlevtcmd_SetTalkPoseType, -2, 65)
    CALL(btlevtcmd_SetStayPoseType, -2, 43)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(2), 0)
        CALL(btlevtcmd_GetDamageCode, -2, LW(0))
        SWITCH(LW(0))
            CASE_OR_EQUAL(23)
            CASE_OR_EQUAL(24)
            CASE_OR_EQUAL(25)
            CASE_OR_EQUAL(26)
            CASE_OR_EQUAL(27)
            CALL(btlevtcmd_GetTotalDamage, -2, LW(1))
            IF_INT_GREATER_OR_EQUAL(LW(1), 1)
                SET_VALUE(LW(2), 1)
            END_IF
            CASE_END
        END_SWITCH
        CALL(btlevtcmd_CheckStatus, -2, 11, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(2), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL_CHILD(to_floor)
            CALL(btlevtcmd_StartWaitEvent, -2)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(ceil_fall_ready_event)
    CALL(btlevtcmd_SetEventCeilFall, -2, 0)
    CALL_CHILD(to_floor)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(to_floor)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_3"))
    WAIT_MS(200)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_BLED_DIE1"), 0)
    WAIT_MS(100)
    IF_INT_LESS(GSW(0), 158)
        CALL(btlevtcmd_WaitAttackEnd)
        CALL(evt_btl_camera_set_prilimit, 1)
        CALL(evt_btl_camera_wait_move_end)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_btl_camera_set_mode, 1, 3)
        CALL(evt_btl_camera_set_moveto, 1, 81, 83, 334, 81, 59, 0, 30, 11)
        WAIT_FRAMES(37)
        CALL(btlevtcmd_SetTalkPose, -2, STRING("T_10"))
        CALL(btlevtcmd_SetStayPose, -2, STRING("S_10"))
        CALL(evt_msg_print, 2, STRING("stg3_tou_436_06"), 0, -2)
        WAIT_MS(300)
        CALL(evt_btl_camera_set_mode, 1, 0)
        WAIT_MS(300)
        CALL(evt_btl_camera_set_prilimit, 0)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
    END_IF
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    RETURN
EVT_END()

static USER_FUNC(down_ceil) {
    u8* wp = &_battleWorkPointer->field_180C0.stageFlags;
    BattleStageObject* body = BattleSearchObjectPtr("if_body");
    BattleStageObject* wire = BattleSearchObjectPtr("if_wire");
    f32 y;

    if (isFirstCall) {
        event->userdata[0] = 120;
        *wp &= ~0x20;
    }
    y = intplGetValue(4, 120 - event->userdata[0], 120, 250.0f, 130.0f);
    wire->mPosition.y = y;
    body->mPosition.y = y;
    event->userdata[0]--;
    if (event->userdata[0] < 0) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

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
        DEBUG_PUT_REG(LW(0))
        MULTIPLY(LW(0), 67)
        DEBUG_PUT_REG(LW(0))
        DIVIDE(LW(0), 100)
        DEBUG_PUT_REG(LW(0))
        CALL(btlevtcmd_GetHp, -2, LW(1))
        DEBUG_PUT_REG(LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
            IF_INT_EQUAL(GSWF(2514), 0)
                SET_VALUE(GSWF(2514), 1)
                CALL(btlevtcmd_StopWaitEvent, -2)
                IF_INT_LESS(GSW(0), 158)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_btl_camera_set_mode, 0, 3)
                    CALL(evt_btl_camera_set_moveto, 0, 73, 78, 322, 73, 57, 0, 30, 11)
                    WAIT_FRAMES(37)
                    CALL(evt_msg_print, 2, STRING("stg3_tou_436_02"), 0, -2)
                ELSE
                    CALL(evt_btl_camera_set_mode, 0, 3)
                    CALL(evt_btl_camera_set_moveto, 0, 73, 78, 322, 73, 57, 0, 30, 11)
                    WAIT_FRAMES(37)
                END_IF
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_2"))
                WAIT_MS(200)
                CALL(evt_btl_camera_set_moveto, 0, 73, 82, 379, 73, 57, 0, 30, 11)
                CALL(evt_env_blur_on, 0, 500)
                WAIT_FRAMES(37)
                IF_INT_LESS(GSW(0), 158)
                    CALL(btlevtcmd_SetTalkPose, -2, STRING("T_2"))
                    CALL(btlevtcmd_SetStayPose, -2, STRING("S_2"))
                    CALL(evt_msg_print, 2, STRING("stg3_tou_436_03"), 0, -2)
                    CALL(btlevtcmd_SetTalkPoseType, -2, 65)
                    CALL(btlevtcmd_SetStayPoseType, -2, 43)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                END_IF
                CALL(evt_btl_camera_set_mode, 0, 0)
            END_IF
        END_IF
        CASE_EQUAL(2)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        DEBUG_PUT_REG(LW(0))
        MULTIPLY(LW(0), 34)
        DEBUG_PUT_REG(LW(0))
        DIVIDE(LW(0), 100)
        DEBUG_PUT_REG(LW(0))
        CALL(btlevtcmd_GetHp, -2, LW(1))
        DEBUG_PUT_REG(LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            SET_VALUE(LW(0), 0)
            CALL(btlevtcmd_CheckStatus, -2, 11, LW(1))
            IF_INT_EQUAL(LW(1), 0)
                CALL(btlevtcmd_SetUnitWork, -2, 0, 3)
                IF_INT_LESS(GSW(0), 158)
                    IF_INT_EQUAL(GSWF(2515), 0)
                        SET_VALUE(GSWF(2515), 1)
                        CALL(btlevtcmd_StatusWindowOnOff, 0)
                        CALL(evt_btl_camera_set_mode, 0, 3)
                        CALL(evt_btl_camera_set_moveto, 0, 73, 78, 322, 73, 57, 0, 30, 11)
                        WAIT_FRAMES(37)
                        CALL(evt_msg_print, 2, STRING("stg3_tou_436_04"), 0, -2)
                        WAIT_MS(300)
                        CALL(btlevtcmd_StatusWindowOnOff, 1)
                        CALL(evt_btl_camera_set_mode, 0, 0)
                    END_IF
                END_IF
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
                WAIT_MS(200)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3C"))
                CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
                CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 25, -1)
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(evt_btl_camera_shake_h, 0, 3, 0, 50, 13)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
                BEGIN_INLINE_EVENT
                    WAIT_MS(600)
                    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
                END_INLINE_EVENT
                CALL(evt_snd_sfxon, STRING("SFX_BOSS_BLED_MOVE_CAILING1"), LW(15))
                CALL(down_ceil)
                CALL(evt_snd_sfxoff, LW(15))
                CALL(btlevtcmd_SetEventCeilFall, -2, PTR(ceil_fall_ready_event))
            END_IF
        END_IF
        CASE_EQUAL(4)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        DEBUG_PUT_REG(LW(0))
        MULTIPLY(LW(0), 25)
        DEBUG_PUT_REG(LW(0))
        DIVIDE(LW(0), 100)
        DEBUG_PUT_REG(LW(0))
        CALL(btlevtcmd_GetHp, -2, LW(1))
        DEBUG_PUT_REG(LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            IF_INT_LESS(GSW(0), 158)
                IF_INT_EQUAL(GSWF(2516), 0)
                    SET_VALUE(GSWF(2516), 1)
                    CALL(btlevtcmd_StopWaitEvent, -2)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_btl_camera_set_mode, 0, 3)
                    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
                    IF_INT_EQUAL(LW(0), 0)
                        CALL(evt_btl_camera_set_moveto, 0, 73, 78, 322, 73, 57, 0, 30, 11)
                    ELSE
                        CALL(evt_btl_camera_set_moveto, 0, 73, 138, 322, 73, 117, 0, 30, 11)
                    END_IF
                    WAIT_FRAMES(37)
                    CALL(evt_msg_print, 2, STRING("stg3_tou_436_05"), 0, -2)
                    WAIT_MS(300)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    CALL(btlevtcmd_StartWaitEvent, -2)
                END_IF
            END_IF
        END_IF
    END_SWITCH
    LABEL(99)
    RETURN
EVT_END()

USER_FUNC(check_ceil) {
    event->lwData[1] = _battleWorkPointer->field_180C0.stageFlags & 0x20;
    return EVT_RETURN_DONE;
}

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
        CALL(btlevtcmd_DrawLots, LW(0), 2, 10, 15)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_champion_drop_kick_attack))
            CALL_CHILD(drop_kick_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_champion_screw_attack))
            CALL_CHILD(screw_attack_event)
        END_SWITCH
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
        SET_VALUE(LW(9), PTR(&weapon_champion_flying_chop_attack))
        CALL_CHILD(flying_chop_attack_event)
        CASE_EQUAL(2)
        CALL(btlevtcmd_DrawLots, LW(0), 3, 10, 15, 20)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_champion_drop_kick_attack))
            CALL_CHILD(drop_kick_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_champion_screw_attack))
            CALL_CHILD(screw_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(&weapon_champion_flying_chop_attack))
            CALL_CHILD(flying_chop_attack_event)
        END_SWITCH
        CASE_EQUAL(3)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 4)
        CALL_CHILD(to_ceiling)
        SET_VALUE(LW(9), PTR(&weapon_champion_roof_shake_attack))
        CALL_CHILD(roof_shake_attack_event)
        CASE_EQUAL(4)
        CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LW(2), 30)
            CALL(btlevtcmd_CheckStatus, -2, 11, LW(1))
            IF_INT_NOT_EQUAL(LW(1), 0)
                SET_VALUE(LW(2), 0)
            END_IF
            CALL(check_ceil)
            IF_INT_NOT_EQUAL(LW(1), 0)
                SET_VALUE(LW(2), 0)
            END_IF
            CALL(btlevtcmd_DrawLots, LW(0), 4, 10, 15, 20, LW(2))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                SET_VALUE(LW(9), PTR(&weapon_champion_drop_kick_attack))
                CALL_CHILD(drop_kick_attack_event)
                CASE_EQUAL(1)
                SET_VALUE(LW(9), PTR(&weapon_champion_screw_attack))
                CALL_CHILD(screw_attack_event)
                CASE_EQUAL(2)
                SET_VALUE(LW(9), PTR(&weapon_champion_flying_chop_attack))
                CALL_CHILD(flying_chop_attack_event)
                CASE_EQUAL(3)
                CALL_CHILD(to_ceiling)
                SET_VALUE(LW(9), PTR(&weapon_champion_roof_shake_attack))
                CALL_CHILD(roof_shake_attack_event)
            END_SWITCH
        ELSE
            CALL(btlevtcmd_DrawLots, LW(0), 3, 10, 20, 20)
            SWITCH(LW(0))
                CASE_EQUAL(0)
                SET_VALUE(LW(9), PTR(&weapon_champion_foot_stamp_attack))
                CALL_CHILD(foot_stamp_attack_event)
                CASE_EQUAL(1)
                SET_VALUE(LW(9), PTR(&weapon_champion_healing))
                CALL_CHILD(healing_event)
                CASE_EQUAL(2)
                SET_VALUE(LW(9), PTR(&weapon_champion_roof_shake_attack))
                CALL_CHILD(roof_shake_attack_event)
            END_SWITCH
        END_IF
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(drop_kick_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(0), 150)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2A"))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.01f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(0), 30)
    ADD(LW(2), 5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_DROP_KICK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
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
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
    WAIT_MS(300)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 50, 13)
    WAIT_MS(60)
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(98)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_CHOP_ROUND1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(3.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    WAIT_MS(250)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1C"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    RETURN
EVT_END()

EVT_BEGIN(screw_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1F"))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(5.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_CHOP_ROUND1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
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
    CALL(btlevtcmd_JumpContinue, -2)
    WAIT_MS(150)
    GOTO(98)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(1.5f), FLOAT(0.7f))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(98)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_CHOP_ROUND1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(3.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    WAIT_MS(250)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1C"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    RETURN
EVT_END()

EVT_BEGIN(flying_chop_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.15f))
    ADD(LW(0), 82)
    ADD(LW(1), 80)
    ADD(LW(2), 140)
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    MULTIPLY_FLOAT(LW(5), 30)
    SUBTRACT(LW(0), LW(5))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_CHOP_ROUND1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_CHOP_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(10.0f), FLOAT(0.01f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(0), 30)
    ADD(LW(1), 5)
    ADD(LW(2), 5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_CHOP_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
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
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
    CALL(btlevtcmd_JumpContinue, -2)
    WAIT_MS(150)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 50, 13)
    WAIT_MS(60)
    LABEL(98)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(3.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    WAIT_MS(250)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1C"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    RETURN
EVT_END()

EVT_BEGIN(foot_stamp_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("W_2"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_1"))
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3A"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.75f))
    ADD(LW(1), 20)
    ADD(LW(2), 5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_FUMI_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_SetTalkPoseType, -2, 65)
    CALL(btlevtcmd_SetStayPoseType, -2, 43)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 6291456)
    CALL(btlevtcmd_SetKissHitOffset, -2, 10, 48, 10)
    CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 32, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_FUMI_STEPON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
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
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
    WAIT_MS(300)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3C"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 25, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_FUMI_STEPON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3C"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 35, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_FUMI_STEPON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 14, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 50, 13)
    WAIT_MS(300)
    LABEL(98)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(3.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
    WAIT_MS(250)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1C"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    RETURN
EVT_END()

EVT_BEGIN(healing_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
    WAIT_FRAMES(16)
    LOOP(3)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_SHAKE_CAILING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
        WAIT_FRAMES(18)
    END_LOOP
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
        ADD(LW(5), -30)
        ADD(LW(6), 80)
        ADD(LW(7), 40)
        CALL(btlevtcmd_BtlIconEntryItemId, 148, LW(5), LW(6), LW(7), LW(8))
        SET_VALUE(LW(0), LW(6))
        ADD(LW(0), -62)
        CALL(evt_sub_intpl_msec_init, 1, LW(6), LW(0), 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(btlevtcmd_BtlIconSetPosition, LW(8), LW(5), LW(0), LW(7))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_BtlIconDelete, LW(8))
    END_INLINE_EVENT
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_8"))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_EAT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_2"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_2"))
    WAIT_MS(700)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 5)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    ADD(LW(2), 10)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 5, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(40)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

USER_FUNC(unk_tou2_000027b8) {
    s32* args = event->args;
    s32 dest = args[0];
    s32 type = evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    f32 c = evtGetFloat(event, args[5]);
    f32 d = evtGetFloat(event, args[6]);
    f32 e = evtGetFloat(event, args[7]);
    s32 a = evtGetValue(event, args[8]);
    s32 b = evtGetValue(event, args[9]);
    EffectEntry* effect = effFallEntry(type, x, y, a, z, b, c, d, e);

    evtSetValue(event, dest, (s32)effect);
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_tou2_00002770) {
    EffectEntry* effect = (EffectEntry*)evtGetValue(event, event->args[0]);

    if (effect != NULL && effFallCheckHit(effect) == 0) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(roof_shake_attack_event)
    SET_VALUE(LW(15), 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 0, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 1, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 2, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 3, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 4, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 7, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 8, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 9, 0)
    SET_VALUE(LW(13), 0)
    LOOP(10)
        SET_VALUE(LW(0), 0)
        ADD(LW(0), LW(13))
        CALL(btlevtcmd_SetPartsWork, -2, 1, LW(0), 0)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL_CHILD(subsetevt_confuse_flustered)
            END_IF
            RETURN
        END_IF
        CALL(btlevtcmd_WeaponAftereffect, LW(9))
        CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
        CALL(btlevtcmd_WaitGuardMove)
        CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
        WAIT_FRAMES(16)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_BLED_SHAKE_CAILING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 10)
        SET_VALUE(LW(8), LW(1))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
        IF_INT_NOT_EQUAL(LW(5), 1)
            SET_VALUE(LW(8), 0)
        END_IF
        CALL(evt_sub_random, 2, LW(6))
        SWITCH(LW(6))
            CASE_EQUAL(0)
            CALL(unk_tou2_000027b8, LW(14), 4, LW(0), LW(1), LW(2), 20, LW(8), 0, 1, 60)
            CASE_EQUAL(1)
            CALL(unk_tou2_000027b8, LW(14), 5, LW(0), LW(1), LW(2), 20, LW(8), 0, 1, 60)
            CASE_EQUAL(2)
            CALL(unk_tou2_000027b8, LW(14), 8, LW(0), LW(1), LW(2), 20, LW(8), 0, 1, 60)
        END_SWITCH
        BEGIN_INLINE_EVENT_GET_ID(LW(15))
            SWITCH(LW(5))
                CASE_EQUAL(1)
                CALL(unk_tou2_00002770, LW(14))
                CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
                CASE_EQUAL(3)
                CALL(unk_tou2_00002770, LW(14))
                CALL(btlevtcmd_StartAvoid, LW(3), 38)
                CASE_EQUAL(6)
                WAIT_FRAMES(10)
                CALL(btlevtcmd_StartAvoid, LW(3), 39)
                CASE_EQUAL(2)
                CALL(unk_tou2_00002770, LW(14))
                CALL(btlevtcmd_StartAvoid, LW(3), 40)
                CASE_DEFAULT
                CALL(unk_tou2_00002770, LW(14))
            END_SWITCH
        END_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        ADD(LW(0), LW(13))
        CALL(btlevtcmd_SetPartsWork, -2, 1, LW(0), LW(15))
        WAIT_FRAMES(4)
        LABEL(90)
        ADD(LW(13), 1)
    END_LOOP
    WAIT_FRAMES(10)
    LABEL(92)
    WAIT_FRAMES(1)
    SET_VALUE(LW(13), 0)
    LABEL(93)
    SET_VALUE(LW(0), 0)
    ADD(LW(0), LW(13))
    CALL(btlevtcmd_GetPartsWork, -2, 1, LW(0), LW(15))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(btlevtcmd_SetPartsWork, -2, 1, LW(0), 0)
        GOTO(92)
    END_IF
    ADD(LW(13), 1)
    IF_INT_LESS(LW(13), 10)
        GOTO(93)
    END_IF
    CALL(btlevtcmd_GetGuardKouraId, -3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_CheckDamage, -2, LW(0), 1, PTR(&weapon_champion_roof_shake_last_attack), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, -3, 1, PTR(&weapon_champion_roof_shake_last_attack), 256, LW(5))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, -4, 1, PTR(&weapon_champion_roof_shake_last_attack), 256, LW(5))
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on

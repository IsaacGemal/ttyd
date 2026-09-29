#include "battle/battle.h"
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
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(kanbu_get_id);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[2];
static s32 init_event[21];
static s32 damage_event[10];
static s32 attack_event[235];
static s32 fire_bottle_event[56];
static s32 hunyahunya_bottle_event[43];
static s32 yokeyoke_bottle_event[74];
static s32 minimini_bottle_event[314];
static s32 one_recover_event[128];
static s32 all_recover_event[155];
static s32 yokeyoke_bottle_event_with_kanbu[15];
static s32 ziwaziwa_bottle_event[15];
static s32 wait_event[6];

// clang-format off
extend_data(unit_gundan_zako_magician_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_gundan_zako_magician_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_X_NAUT_PHD,               // mUnitType
    "btl_un_gundan_zako_magician", // name
    9,                             // maxHp
    0,                             // maxFp
    1,                             // mDangerHP
    1,                             // mPerilHP
    27,                            // mLevel
    0,                             // mBonusExp
    3,                             // bonusCoinRolls
    70,                            // bonusCoinRate
    1,                             // baseCoin
    50,                            // field_0x13
    9999,                          // field_0x14
    30,                            // width
    36,                            // height
    { 0, 36 },                     // statusIconOffset
    { 0.0f, 18.0f, 0.0f },         // centerOffset
    { 0, 0 },                      // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },          // talkTogeBaseOffset
    { 15.0f, 0.0f, -10.0f },       // field_0x3C
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // field_0x54
    0.0f,                          // field_0x58
    { 15.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 12.0f, 27.0f, 0.0f },        // kissHitOffset
    { 0.0f, 18.0f, 0.0f },         // cutBaseOffset
    30.0f,                         // cutWidth
    36.0f,                         // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    0,                             // swallowRate
    2,                             // swallowAttribute
    100,                           // field_0x8C
    20,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },          // starPointDispOffset
    "SFX_ENM_3RD2_DAMAGED1",       // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0,                             // attributeFlags
    &regist,                       // vulnStatus
    2,                             // numParts
    parts,                         // mParts
    init_event,                    // initEvtScript
    data_table,                    // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    80,  // sleepRate
    80,  // stopRate
    80,  // dizzyRate
    100, // poisonRate
    80,  // confuseRate
    100, // electricRate
    100, // burnRate
    70,  // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    70,  // allergicRate
    90,  // frightRate
    70,  // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleDispPose pose_table[16] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "X_1" },
    { 0x1B, "Z_1" },
    { 0x1C, "S_1" },
    { 0x1D, "D_1" },
    { 0x1E, "D_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[2] = {
    {
        1,                             // partNum
        "btl_un_gundan_zako_magician", // partName
        "c_zako_m",                    // modelName
        { 0.0f, 0.0f, 0.0f },          // positionOffset
        { 0.0f, 28.0f, 0.0f },         // hitBaseOffset
        { 0.0f, 38.0f, 0.0f },         // hitCursorBaseOffset
        20,                            // field_0x30
        30,                            // field_0x32
        255,                           // baseAlpha
        &defence,                      // defense
        &defence_attr,                 // defenseAttr
        0x00000009,                    // attributes
        0,                             // counterAttributes
        pose_table,                    // poseTable
    },
    {
        2,                             // partNum
        "btl_un_gundan_zako_magician", // partName
        "c_zako_m",                    // modelName
        { 0.0f, 0.0f, 0.0f },          // positionOffset
        { 0.0f, 0.0f, 0.0f },          // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },          // hitCursorBaseOffset
        20,                            // field_0x30
        30,                            // field_0x32
        255,                           // baseAlpha
        &defence,                      // defense
        &defence_attr,                 // defenseAttr
        0x13018000,                    // attributes
        0,                             // counterAttributes
        pose_table,                    // poseTable
    },
};

static BattleWeapon weapon_gundan_zako_magician_fire_bottle = {
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
    1,                                        // element
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
    75,                                       // burnChance
    3,                                        // burnTime
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

static BattleWeapon weapon_gundan_zako_magician_hunyahunya_bottle = {
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
    75,                                       // defChangeChance
    3,                                        // defChangeTime
    -3,                                       // defChangeStrength
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

static BattleWeapon weapon_gundan_zako_magician_yokeyoke_bottle = {
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
    100,                                      // dodgyChance
    3,                                        // dodgyTime
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

static BattleWeapon weapon_gundan_zako_magician_minimini_bottle = {
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
    0x00000007,                               // special
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
    75,                                       // sizeChangeChance
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

static BattleWeapon weapon_gundan_zako_magician_one_recover = {
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
    0x01100160,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0,                                        // special
    0x000007FF,                               // counterResistanceFlags
    0x80001500,                               // targetWeightingFlags
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

static BattleWeapon weapon_gundan_zako_magician_all_recover = {
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
    0x02100160,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0,                                        // special
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

static BattleWeapon weapon_gundan_zako_magician_ziwaziwa_bottle = {
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
    0x01100160,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80001500,                               // targetWeightingFlags
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
    5,                                        // hpRegenTime
    2,                                        // hpRegenStrength
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

static USER_FUNC(kanbu_get_id) {
    s32* args;
    s32 id;
    s32 i;
    BattleWork* wp;
    BattleUnit* unit;

    id = -1;
    args = event->args;
    wp = _battleWorkPointer;
    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(wp, i);
        if (unit && unit->initialKind == UNIT_LORD_CRUMP_CHAPTER5) {
            id = i;
            break;
        }
    }
    evtSetValue(event, args[0], id);
    if (id != -1) {
        evtSetValue(event, args[1], BtlUnit_GetBodyPartsId(unit));
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(fire_bottle_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(kanbu_get_id, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(10)
    ELSE
        GOTO(20)
    END_IF
    LABEL(10)
    SET_VALUE(LW(0), 0)
    ADD(LW(0), 20)
    ADD(LW(0), 10)
    ADD(LW(0), 10)
    ADD(LW(0), 20)
    ADD(LW(0), 10)
    ADD(LW(0), 5)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(fire_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(hunyahunya_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        CALL(btlevtcmd_CheckStatus, -2, 7, LW(2))
        IF_INT_EQUAL(LW(2), 0)
            CALL_CHILD(yokeyoke_bottle_event)
            GOTO(99)
        END_IF
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(minimini_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(one_recover_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 5)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(all_recover_event)
        GOTO(99)
    END_IF
    GOTO(10)
    LABEL(20)
    SET_VALUE(LW(0), 0)
    ADD(LW(0), 60)
    ADD(LW(0), 20)
    ADD(LW(0), 20)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 60)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(fire_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL(kanbu_get_id, LW(3), LW(4))
        CALL(btlevtcmd_CheckStatus, LW(3), 7, LW(2))
        IF_INT_EQUAL(LW(2), 0)
            CALL_CHILD(yokeyoke_bottle_event_with_kanbu)
            GOTO(99)
        END_IF
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL(kanbu_get_id, LW(3), LW(4))
        CALL(btlevtcmd_CheckStatus, LW(3), 23, LW(2))
        IF_INT_EQUAL(LW(2), 0)
            CALL_CHILD(ziwaziwa_bottle_event)
            GOTO(99)
        END_IF
    END_IF
    GOTO(20)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_common_event1)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_minimini_bottle))
        CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(10))
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LW(10), 125)
        ELSE
            SET_VALUE(LW(10), -125)
        END_IF
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(10), 0, 1, LW(14))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(14))
    ELSE
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(14))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(14))
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_fire_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_hunyahunya_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_minimini_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_yokeyoke_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_one_recover))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_all_recover))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_ziwaziwa_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
    END_IF
    WAIT_FRAMES(80)
    RETURN
EVT_END()

static EVT_BEGIN(attack_common_event2)
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_fire_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_hunyahunya_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1B"))
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_fire_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_hunyahunya_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
    END_IF
    WAIT_FRAMES(24)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(24)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(6.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, -1)
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
        LABEL(90)
        CALL(btlevtcmd_JumpPartsContinue, -2, 2)
        CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
        LOOP(12)
            CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
        END_LOOP
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
        GOTO(98)
        LABEL(91)
        IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_fire_bottle))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 16, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        END_IF
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        LOOP(12)
            CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
        END_LOOP
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
        GOTO(98)
        LABEL(98)
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_FRAMES(30)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(attack_common_event3)
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_yokeyoke_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1D"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_ziwaziwa_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1C"))
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_yokeyoke_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5C"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_magician_ziwaziwa_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4C"))
    END_IF
    WAIT_FRAMES(24)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(24)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(1.0f), FLOAT(0.5f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), 0, LW(2), 0, -1)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    LOOP(12)
        CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(0), 0, LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
    END_LOOP
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
        LABEL(90)
        GOTO(98)
        LABEL(91)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(98)
        LABEL(98)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_FRAMES(30)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(fire_bottle_event)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_fire_bottle))
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event2)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(hunyahunya_bottle_event)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_hunyahunya_bottle))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event2)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(yokeyoke_bottle_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_yokeyoke_bottle))
    CALL_CHILD(attack_common_event1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_BOTTLE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        LOOP(3)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_BOTTLE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(20)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5B"))
    WAIT_FRAMES(120)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(5)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_BOTTLE5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(55)
        CALL(evt_btl_camera_set_mode, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    RETURN
EVT_END()

static EVT_BEGIN(minimini_bottle_event)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_minimini_bottle))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1E"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3C"))
    WAIT_FRAMES(24)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(24)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(10))
    IF_INT_EQUAL(LW(10), 0)
        SET_VALUE(LW(10), 125)
    ELSE
        SET_VALUE(LW(10), -125)
    END_IF
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(6.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(10), 0, 0, 0, -1)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 20, LW(10), 50, 0, FLOAT(2.4f), 0, 0, 0, 0, 0, 0, 0)
    LOOP(12)
        CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(10), 0, 0, FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
    END_INLINE_EVENT
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
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
            ADD(LW(10), 1)
            GOTO(0)
        END_IF
        LABEL(98)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_FRAMES(60)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(one_recover_event)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_one_recover))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4B"))
    WAIT_FRAMES(80)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 10)
    CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, LW(3), LW(5))
    CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    ADD(LW(1), LW(5))
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 10, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(all_recover_event)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_all_recover))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4B"))
    WAIT_FRAMES(80)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(0)
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_random, 10, LW(10))
        ADD(LW(10), 5)
        WAIT_FRAMES(LW(10))
        CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 5)
        CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, LW(3), LW(5))
        CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
        MULTIPLY_FLOAT(LW(5), LW(6))
        ADD(LW(1), LW(5))
        CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 5, 0, 0, 0, 0, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(0)
    END_IF
    WAIT_FRAMES(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(yokeyoke_bottle_event_with_kanbu)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_yokeyoke_bottle))
    CALL(kanbu_get_id, LW(3), LW(4))
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event3)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(ziwaziwa_bottle_event)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_magician_ziwaziwa_bottle))
    CALL(kanbu_get_id, LW(3), LW(4))
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event3)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on

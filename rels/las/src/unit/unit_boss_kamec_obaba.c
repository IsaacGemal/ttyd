#include "unit_boss_kamec_obaba.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_damage.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_weapon_power.h"
#include "driver/effdrv.h"
#include "eff/eff_stardust.h"
#include "eff/n64/eff_magic1_n64.h"
#include "eff/n64/eff_thunderflare_n64.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(eff_magic_get_posX);
static USER_FUNC(eff_magic);
static USER_FUNC(eff_staff);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[2];
static s32 init_event[56];
static s32 damage_event[74];
static s32 dmg_fall_event[149];
static s32 dead_event[155];
static s32 phase_event[216];
static s32 attack_event[352];
static s32 attack_magic_event[269];
s32 magic_select_pri_obaba[52];
s32 magic_select_pri_koopa[52];
static s32 dekadeka_magic_event[73];
static s32 katikati_magic_event[4];
static s32 biribiri_magic_event[4];
static s32 trans_magic_event[4];
static s32 recover_magic_event[90];
static s32 wait_event[6];

// clang-format off
extend_data(132,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(133,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BattleWeapon weapon_kamec_obaba_attack_magic = {
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
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
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

static BattleWeapon weapon_kamec_obaba_dekadeka_magic = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
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
    100,                                      // sizeChangeChance
    3,                                        // sizeChangeTime
    2,                                        // sizeChangeStrength
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

static BattleWeapon weapon_kamec_obaba_biribiri_magic = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
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
    100,                                      // electricChance
    3,                                        // electricTime
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

static BattleWeapon weapon_kamec_obaba_katikati_magic = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
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

static BattleWeapon weapon_kamec_obaba_trans_magic = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
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
    100,                                      // invisibleChance
    3,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_kamec_obaba_recover_magic = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00001500,                               // targetWeightingFlags
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

BattleUnitKind unit_boss_kamec_obaba = {
    UNIT_KAMMY_KOOPA,             // mUnitType
    "btl_un_kamec_obaba",         // name
    50,                           // maxHp
    0,                            // maxFp
    1,                            // mDangerHP
    1,                            // mPerilHP
    48,                           // mLevel
    0,                            // mBonusExp
    0,                            // bonusCoinRolls
    100,                          // bonusCoinRate
    0,                            // baseCoin
    50,                           // field_0x13
    6,                            // field_0x14
    42,                           // width
    42,                           // height
    { 7, 42 },                    // statusIconOffset
    { 0.0f, 21.0f, 0.0f },        // centerOffset
    { 0, 0 },                     // hpGaugeOffset
    { 20.0f, 20.0f, 0.0f },       // talkTogeBaseOffset
    { 21.0f, 0.0f, -10.0f },      // field_0x3C
    { 0.0f, 0.0f, 0.0f },         // burnFlameOffset
    0.0f,                         // field_0x54
    0.0f,                         // field_0x58
    { 18.0f, 0.0f, 0.0f },        // bintaHitOffset
    { 21.0f, 27.3f, 0.0f },       // kissHitOffset
    { 0.0f, 21.0f, 0.0f },        // cutBaseOffset
    42.0f,                        // cutWidth
    42.0f,                        // cutHeight
    0,                            // turnOrder
    0,                            // turnOrderVariance
    0,                            // swallowRate
    2,                            // swallowAttribute
    100,                          // field_0x8C
    20,                           // itemStealParam
    { 0.0f, 0.0f, 0.0f },         // starPointDispOffset
    "SFX_BOSS_KAMEBABA_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",       // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",        // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",   // electricDamageSfxName
    0x00000004,                   // attributeFlags
    &regist,                      // vulnStatus
    2,                            // numParts
    parts,                        // mParts
    init_event,                   // initEvtScript
    data_table,                   // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    40,  // sleepRate
    40,  // stopRate
    60,  // dizzyRate
    100, // poisonRate
    40,  // confuseRate
    100, // electricRate
    100, // burnRate
    40,  // freezeRate
    100, // hugeRate
    60,  // tinyRate
    100, // attackUpRate
    60,  // attackDownRate
    100, // defenseUpRate
    80,  // defenseDownRate
    40,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    40,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    5,   // instaKillRate
};

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
    { 0x1F, "D_1" },
    { 0x27, "D_1" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_sky[17] = {
    { 0x01, "N_2" },
    { 0x02, "Y_2" },
    { 0x09, "Y_2" },
    { 0x05, "K_2" },
    { 0x04, "X_2" },
    { 0x03, "X_2" },
    { 0x1C, "S_2" },
    { 0x1D, "Q_2" },
    { 0x1E, "Q_2" },
    { 0x1F, "D_2" },
    { 0x27, "D_2" },
    { 0x2A, "R_2" },
    { 0x28, "W_2" },
    { 0x38, "X_2" },
    { 0x39, "X_2" },
    { 0x41, "T_2" },
    { 0x45, "S_2" },
};

static BattleDispPose pose_table_dead[3] = {
    { 0x1C, "D_3" },
    { 0x27, "D_3" },
    { 0x45, "D_3" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[2] = {
    {
        1,                     // partNum
        "btl_un_kamec_obaba",  // partName
        "c_kamek_bb",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 30.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 40.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00600009,            // attributes
        0,                     // counterAttributes
        pose_table_sky,        // poseTable
    },
    {
        2,                    // partNum
        "btl_un_kamec_obaba", // partName
        "c_kamek_bb",         // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
};

static EVT_BEGIN(kamec_obaba_zoom)
    CALL(btlevtcmd_GetUnitWork, GW(9), 0, LW(1))
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_EQUAL(LW(1), 1)
        CALL(evt_btl_camera_set_moveto, 0, 135, 106, 296, 135, 55, -402, 30, 11)
    ELSE
        CALL(evt_btl_camera_set_moveto, 0, 140, 34, 296, 140, 50, -402, 30, 11)
    END_IF
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_KAMEBABA_MOVE1L"), STRING("SFX_BOSS_KAMEBABA_MOVE1R"), 0, 15, 15)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_KAMEBABA_MOVE1L"), STRING("SFX_BOSS_KAMEBABA_MOVE1R"), 0, 8, 8)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckDamageCode, -2, 256, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(10), 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetTotalDamage, LW(10), LW(1))
        CALL(btlevtcmd_GetUnitWork, LW(10), 1, LW(0))
        ADD(LW(0), LW(1))
        CALL(btlevtcmd_SetUnitWork, LW(10), 1, LW(0))
        IF_INT_GREATER(LW(0), 1)
            CALL(btlevtcmd_WaitAttackEnd)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL_CHILD(dmg_fall_event)
            CALL(btlevtcmd_StartWaitEvent, -2)
        END_IF
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_fall_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_OffAttribute, -2, 4)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_2"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_SetTalkPoseType, -2, 65)
    CALL(btlevtcmd_SetStayPoseType, -2, 43)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("H_1"))
        CALL(btlevtcmd_SetAlpha, -2, 2, 255)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        WAIT_FRAMES(80)
        CALL(evt_sub_intpl_init, 0, 255, 0, 30)
        LOOP(30)
            CALL(evt_sub_intpl_get_value_para, LW(1), LW(2))
            CALL(btlevtcmd_SetAlpha, -2, 2, LW(1))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
    END_INLINE_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 10, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    WAIT_MS(500)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    DEBUG_PUT_MSG(STRING("カメックオババ　死亡"))
    SET_VALUE(GW(11), 1)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_WaitAttackEnd)
    DEBUG_PUT_MSG(STRING("カメックオババ　死亡イベント開始"))
    CALL(btlevtcmd_GetUnitWork, LW(10), 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(dmg_fall_event)
    END_IF
    SWITCH(GW(10))
        CASE_EQUAL(0)
        DEBUG_PUT_MSG(STRING("カメックオババ　死亡イベント　クッパ生存"))
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL_CHILD(kamec_obaba_zoom)
        CALL(btlevtcmd_SetTalkPose, -2, STRING("T_3"))
        CALL(btlevtcmd_SetStayPose, -2, STRING("D_3"))
        CALL(evt_msg_print, 2, STRING("stg8_las_100_01"), 0, -2)
        WAIT_MS(200)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_3"))
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_dead))
        SET_VALUE(GW(11), 2)
        SET_VALUE(LW(10), -2)
        CALL_CHILD(subsetevt_dead_core_nospin_norotate)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        CASE_EQUAL(1)
        DEBUG_PUT_MSG(STRING("カメックオババ　死亡イベント　同時に死亡"))
        LOOP(0)
            IF_INT_EQUAL(GW(10), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_3"))
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_dead))
        SET_VALUE(GW(11), 2)
        SET_VALUE(LW(10), -2)
        CALL_CHILD(subsetevt_dead_core_nospin_norotate)
        CASE_EQUAL(2)
        DEBUG_PUT_MSG(STRING("カメックオババ　死亡イベント　クッパ死亡"))
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL_CHILD(kamec_obaba_zoom)
        CALL(btlevtcmd_SetTalkPose, -2, STRING("D_1"))
        CALL(btlevtcmd_SetStayPose, -2, STRING("D_1"))
        CALL(evt_msg_print, 2, STRING("stg8_las_100_03"), 0, -2)
        WAIT_MS(200)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_3"))
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_dead))
        SET_VALUE(GW(11), 2)
        SET_VALUE(LW(10), -2)
        CALL_CHILD(subsetevt_dead_core_nospin_norotate)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
    END_SWITCH
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AddUnitWork, -2, 2, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_LESS(LW(0), 3)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_MovePosition, -2, 300, LW(1), LW(2), 0, -1, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_OnAttribute, -2, 4)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_sky))
    CALL(btlevtcmd_SetTalkPoseType, -2, 65)
    CALL(btlevtcmd_SetStayPoseType, -2, 43)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_MS(166)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_ChangeFaceDirection, -2, -1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), 40, LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), 40, LW(2), 60, 0, 4, 0, -1)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 40, LW(2))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    DELETE_EVENT(LW(15))
    WAIT_MS(500)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    DEBUG_PUT_REG(LW(0))
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_attack_magic))
        CALL_CHILD(attack_magic_event)
        GOTO(99)
    END_IF
    SET_VALUE(LW(5), 0)
    CALL(btlevtcmd_GetHp, -2, LW(0))
    CALL(btlevtcmd_GetMaxHp, -2, LW(1))
    MULTIPLY(LW(0), 100)
    DIVIDE(LW(0), LW(1))
    SWITCH(LW(0))
        CASE_LESS(25)
        SET_VALUE(LW(5), 25)
        CASE_LESS(50)
        SET_VALUE(LW(5), 10)
        CASE_LESS(75)
        SET_VALUE(LW(5), 5)
    END_SWITCH
    SET_VALUE(LW(6), 0)
    IF_INT_EQUAL(GW(10), 0)
        CALL(btlevtcmd_GetHp, GW(8), LW(0))
        CALL(btlevtcmd_GetMaxHp, GW(8), LW(1))
        MULTIPLY(LW(0), 100)
        DIVIDE(LW(0), LW(1))
        SWITCH(LW(0))
            CASE_LESS(25)
            SET_VALUE(LW(6), 25)
            CASE_LESS(50)
            SET_VALUE(LW(6), 10)
            CASE_LESS(75)
            SET_VALUE(LW(6), 5)
        END_SWITCH
    END_IF
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_dekadeka_magic))
    CALL_CHILD(magic_select_pri_obaba)
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckStatus, LW(3), 6, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LW(8), 10)
        END_IF
        CALL(btlevtcmd_CheckStatus, LW(3), 18, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LW(10), 10)
        END_IF
    END_IF
    SET_VALUE(LW(13), LW(3))
    SET_VALUE(LW(14), LW(4))
    CALL_CHILD(magic_select_pri_koopa)
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckStatus, LW(3), 14, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LW(7), 10)
        END_IF
        SET_VALUE(LW(11), 10)
    END_IF
    LABEL(10)
    SET_VALUE(LW(1), LW(5))
    ADD(LW(1), LW(6))
    ADD(LW(1), LW(7))
    ADD(LW(1), LW(8))
    ADD(LW(1), LW(10))
    ADD(LW(1), LW(11))
    ADD(LW(1), 50)
    SUBTRACT(LW(1), 1)
    CALL(evt_sub_random, LW(1), LW(0))
    SET_VALUE(LW(1), LW(5))
    ADD(LW(1), LW(6))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_recover_magic))
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
        CALL(btlevtcmd_GetHp, LW(3), LW(0))
        CALL(btlevtcmd_GetMaxHp, LW(3), LW(1))
        IF_INT_EQUAL(LW(0), LW(1))
            GOTO(10)
        END_IF
        CALL_CHILD(recover_magic_event)
        GOTO(99)
    END_IF
    ADD(LW(1), LW(7))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_katikati_magic))
        CALL_CHILD(katikati_magic_event)
        GOTO(99)
    END_IF
    ADD(LW(1), LW(8))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_biribiri_magic))
        SET_VALUE(LW(3), LW(13))
        SET_VALUE(LW(4), LW(14))
        CALL_CHILD(biribiri_magic_event)
        GOTO(99)
    END_IF
    ADD(LW(1), LW(10))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_trans_magic))
        SET_VALUE(LW(3), LW(13))
        SET_VALUE(LW(4), LW(14))
        CALL_CHILD(trans_magic_event)
        GOTO(99)
    END_IF
    ADD(LW(1), LW(11))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_dekadeka_magic))
        CALL_CHILD(dekadeka_magic_event)
        GOTO(99)
    ELSE
        SET_VALUE(LW(9), PTR(&weapon_kamec_obaba_attack_magic))
        CALL_CHILD(attack_magic_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static USER_FUNC(eff_staff) {
    s32* args = event->args;
    s32 id = BattleTransID(event, evtGetValue(event, args[0]));
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    EffectEntry* eff;
    f32* data;
    f32 x, y, z;

    if (isFirstCall) {
        unit->work[3] = (s32)effThunderflareN64Entry(0, 0x3C, 0.0f, -1000.0f, 0.0f, 1.0f);
    }
    eff = (EffectEntry*)unit->work[3];
    if ((eff->flags & 1) == 0) {
        return 2;
    }
    data = eff->userdata;
    data[0x28 / 4] = unit->mSizeMultiplier;
    BtlUnit_GetPos(unit, &x, &y, &z);
    data[0x8 / 4] = 30.0f * unit->mSizeMultiplier * unit->faceDirection + x;
    data[0xC / 4] = 35.0f * unit->mSizeMultiplier + y;
    data[0x10 / 4] = z + 5.0f;
    return 0;
}

static USER_FUNC(eff_magic) {
    s32* args = event->args;
    s32 id = BattleTransID(event, evtGetValue(event, args[0]));
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    f32 a = evtGetFloat(event, args[1]);
    f32 b = evtGetFloat(event, args[2]);
    f32 c = evtGetFloat(event, args[3]);
    s32 type = evtGetValue(event, args[4]);
    EffectEntry* eff;
    f32* data;
    f32 x, y, z;

    BtlUnit_GetPos(unit, &x, &y, &z);
    x = (28.0f * unit->mSizeMultiplier) * unit->faceDirection + x;
    y = 20.0f * unit->mSizeMultiplier + y;
    z += 5.0f;
    eff = effMagic1N64Entry(0, type, x, y, z, a, b, c);
    unit->work[4] = (s32)eff;
    data = eff->userdata;
    data[0x28 / 4] = unit->mSizeMultiplier;
    return 2;
}

static EVT_BEGIN(ground_magic_common_event1)
    BEGIN_SIBLING_EVENT
        LOOP(2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(40)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(eff_staff, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
    WAIT_FRAMES(30)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC8"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1D"))
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1E"))
    IF_INT_NOT_EQUAL(LW(9), PTR(&weapon_kamec_obaba_attack_magic))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(eff_magic_support, -2, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(ground_magic_common_event2)
    BEGIN_SIBLING_EVENT
        LOOP(2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(40)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(eff_staff, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2A"))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC7"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2C"))
    IF_INT_NOT_EQUAL(LW(9), PTR(&weapon_kamec_obaba_attack_magic))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(eff_magic_support, -2, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(sky_magic_common_event)
    BEGIN_SIBLING_EVENT
        LOOP(2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(40)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(eff_staff, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3A"))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC8"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
    WAIT_FRAMES(35)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC7"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3C"))
    WAIT_FRAMES(15)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3D"))
    IF_INT_NOT_EQUAL(LW(9), PTR(&weapon_kamec_obaba_attack_magic))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(eff_magic_support, -2, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(magic_common_event1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitId, -2, LW(13))
        IF_INT_NOT_EQUAL(LW(3), LW(13))
            CALL_CHILD(ground_magic_common_event1)
        ELSE
            CALL_CHILD(ground_magic_common_event2)
        END_IF
    ELSE
        CALL_CHILD(sky_magic_common_event)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(magic_common_event2)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(ground_magic_common_event2)
    ELSE
        CALL_CHILD(sky_magic_common_event)
    END_IF
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

static USER_FUNC(eff_magic_get_posX) {
    s32* args = event->args;
    s32 id = BattleTransID(event, evtGetValue(event, args[0]));
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    EffectEntry* eff = (EffectEntry*)unit->work[4];
    f32* data = eff->userdata;

    evtSetFloat(event, args[1], data[0x10 / 4]);
    return 2;
}

USER_FUNC(eff_magic_support) {
    s32* args = event->args;
    s32 id = BattleTransID(event, evtGetValue(event, args[0]));
    s32 mode = evtGetValue(event, args[1]);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    f32 x, y, z;

    BtlUnit_GetPos(unit, &x, &y, &z);
    switch (mode) {
        case 0:
            x = (30.0f * unit->mSizeMultiplier) * unit->faceDirection + x;
            y = 10.0f * unit->mSizeMultiplier + y;
            z += 5.0f;
            effStardustEntry(6, 5, 0x1E, x, y, z, 40.0f, 40.0f);
            break;
        case 1:
            x = (30.0f * unit->mSizeMultiplier) * unit->faceDirection + x;
            y = 50.0f * unit->mSizeMultiplier + y;
            z += 5.0f;
            effStardustEntry(6, 5, 0x1E, x, y, z, 40.0f, 40.0f);
            break;
    }
    return 2;
}

static EVT_BEGIN(attack_magic_event)
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
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL_CHILD(magic_common_event1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KAMEBABA_MAGIC3_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
        LABEL(90)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(1)
            LOOP(0)
                CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
                IF_INT_EQUAL(LW(0), -1)
                    CALL(eff_magic_get_posX, -2, LW(0))
                    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                    IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                        CALL_CHILD(start_avoid)
                        BREAK_LOOP
                    END_IF
                ELSE
                    CALL(eff_magic_get_posX, -2, LW(0))
                    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                    IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                        CALL_CHILD(start_avoid)
                        BREAK_LOOP
                    END_IF
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -57)
        ADD(LW(1), 10)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
        CALL(btlevtcmd_GetStageSize, LW(10), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
        MULTIPLY(LW(10), LW(0))
        SUBTRACT(LW(11), 5)
        CALL(eff_magic, -2, LW(10), LW(11), LW(12), 60)
        WAIT_FRAMES(60)
        GOTO(98)
        LABEL(91)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -57)
        ADD(LW(1), 10)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
        CALL(eff_magic, -2, LW(10), LW(11), LW(12), 45)
        WAIT_FRAMES(45)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(98)
        LABEL(98)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

EVT_BEGIN(magic_select_pri_obaba)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    IF_INT_EQUAL(GW(10), 0)
        SET_VALUE(LW(5), 25)
    END_IF
    IF_INT_EQUAL(GW(11), 0)
        SET_VALUE(LW(6), 75)
    END_IF
    SET_VALUE(LW(1), LW(5))
    ADD(LW(1), LW(6))
    SUBTRACT(LW(1), 1)
    CALL(evt_sub_random, LW(1), LW(0))
    IF_INT_LESS(LW(0), LW(5))
        SET_VALUE(LW(3), GW(8))
        SET_VALUE(LW(4), 1)
    ELSE
        SET_VALUE(LW(3), -2)
        SET_VALUE(LW(4), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(magic_select_pri_koopa)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    IF_INT_EQUAL(GW(10), 0)
        SET_VALUE(LW(5), 75)
    END_IF
    IF_INT_EQUAL(GW(11), 0)
        SET_VALUE(LW(6), 25)
    END_IF
    SET_VALUE(LW(1), LW(5))
    ADD(LW(1), LW(6))
    SUBTRACT(LW(1), 1)
    CALL(evt_sub_random, LW(1), LW(0))
    IF_INT_LESS(LW(0), LW(5))
        SET_VALUE(LW(3), GW(8))
        SET_VALUE(LW(4), 1)
    ELSE
        SET_VALUE(LW(3), -2)
        SET_VALUE(LW(4), 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(dekadeka_magic_event)
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL_CHILD(magic_common_event1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(katikati_magic_event)
    CALL_CHILD(dekadeka_magic_event)
    RETURN
EVT_END()

static EVT_BEGIN(biribiri_magic_event)
    CALL_CHILD(dekadeka_magic_event)
    RETURN
EVT_END()

static EVT_BEGIN(trans_magic_event)
    CALL_CHILD(dekadeka_magic_event)
    RETURN
EVT_END()

static EVT_BEGIN(recover_magic_event)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL_CHILD(magic_common_event1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 8)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    IF_INT_EQUAL(LW(3), GW(8))
        ADD(LW(0), -30)
        ADD(LW(1), 100)
        ADD(LW(2), 10)
    ELSE
        ADD(LW(1), 50)
        ADD(LW(2), 10)
    END_IF
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 8, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(40)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on

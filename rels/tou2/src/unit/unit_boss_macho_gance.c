#include "unit_boss_macho_gance.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(x_flag_wait);
static USER_FUNC(x_flag_get);
static USER_FUNC(x_flag_add);
static USER_FUNC(x_flag_set);
static USER_FUNC(wariai_enzan_0f);
static USER_FUNC(wariai_enzan_0);
static USER_FUNC(wariai_enzan_1f_intpl);
static USER_FUNC(wariai_enzan_1f);
static USER_FUNC(wariai_enzan_1);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[15];
static BattleUnitKindPart parts[1];
static s32 init_event[79];
static s32 entry_event[62];
static s32 damage_event[97];
static s32 phase_event[482];
static s32 dead_event[64];
s32 unk_evt_tou2_000219a8[238];
static s32 attack_event[151];
s32 gance_macho_speed_attack_event[119];
s32 gance_build_up_attack_event[119];
s32 gance_body_attack_event[126];
s32 gance_footwork_attack_event[119];
s32 gance_attack_attack_event[541];
s32 gance_posing_attack_event[129];
s32 gance_rariat_attack_attack_event[446];
s32 gance_body_attack_attack_event[515];
static s32 wait_event[6];

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_macho_gance = {
    UNIT_MACHO_GRUBBA,          // mUnitType
    "btl_un_macho_gance",       // name
    60,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    71,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    6,                          // renzokuSoftCap
    116,                        // width
    124,                        // height
    { 30, 100 },                // statusIconOffset
    { 0.0f, 62.0f, 0.0f },      // centerOffset
    { 0, -3 },                  // hpGaugeOffset
    { 40.0f, 80.0f, 0.0f },     // talkTogeBaseOffset
    { 58.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 58.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 42.0f, 80.0f, 0.0f },     // kissHitOffset
    { 0.0f, 62.0f, 0.0f },      // cutBaseOffset
    116.0f,                     // cutWidth
    124.0f,                     // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_GANSU_DAMAGED1",  // damageSfxName
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

static BattleVulnerableStatus regist = {
    50,  // sleepRate
    40,  // stopRate
    50,  // dizzyRate
    100, // poisonRate
    50,  // confuseRate
    100, // electricRate
    100, // burnRate
    30,  // freezeRate
    0,   // hugeRate
    75,  // tinyRate
    100, // attackUpRate
    75,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    60,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    60,  // slowRate
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
    { 0x32, "E_1A" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_furafura[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "K_1" },
    { 0x1C, "X_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "D_1" },
    { 0x27, "D_1" },
    { 0x32, "E_1A" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "X_1" },
};

static BattleDataEntry data_table[15] = {
    { 0x31, dead_event },
    { 0x24, unk_evt_tou2_000219a8 },
    { 0x18, unk_evt_tou2_000219a8 },
    { 0x19, unk_evt_tou2_000219a8 },
    { 0x1A, unk_evt_tou2_000219a8 },
    { 0x1B, unk_evt_tou2_000219a8 },
    { 0x1C, unk_evt_tou2_000219a8 },
    { 0x1D, unk_evt_tou2_000219a8 },
    { 0x1E, unk_evt_tou2_000219a8 },
    { 0x1F, unk_evt_tou2_000219a8 },
    { 0x20, unk_evt_tou2_000219a8 },
    { 0x21, unk_evt_tou2_000219a8 },
    { 0x22, unk_evt_tou2_000219a8 },
    { 0x23, unk_evt_tou2_000219a8 },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                      // partNum
        "btl_un_macho_gance",   // partName
        "c_m_gansu",            // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 26.0f, 77.0f, 0.0f }, // hitBaseOffset
        { 25.0f, 81.0f, 0.0f }, // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        kMainPart | kUnknown_0x8, // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

BattleWeapon weapon_gance_macho_speed = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
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
    100,                                      // fastChance
    4,                                        // fastTime
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

BattleWeapon weapon_gance_build_up = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
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
    100,                                      // atkChangeChance
    1,                                        // atkChangeTime
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

BattleWeapon weapon_gance_body = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
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
    100,                                      // defChangeChance
    2,                                        // defChangeTime
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

BattleWeapon weapon_gance_footwork = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
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
    100,                                      // dodgyChance
    2,                                        // dodgyTime
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_gance_attack = {
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
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_gance_posing = {
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
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000088,                               // special
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
    4,                                        // chargeStrength
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

BattleWeapon weapon_gance_rariat_attack = {
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
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_gance_body_attack = {
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
    0x01101260,                                 // targetClassFlags
    0x20001000,                                 // targetPropertyFlags
    0,                                          // element
    10,                                         // damagePattern
    3,                                          // actionLevel
    2,                                          // field_0x6F
    NULL,                                       // acHelpMsg
    0x00001087,                                 // special
    0,                                          // counterResistanceFlags
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
    { 0, 25, 25, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

EVT_BEGIN(macho_gance_zoom)
    CALL(btlevtcmd_GetStatusMg, -2, LW(2))
    MULTIPLY_FLOAT(LW(2), FLOAT(10.0f))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(1))
    MULTIPLY_FLOAT(LW(2), LW(1))
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(LW(1), 30)
    ELSE
        SET_VALUE(LW(1), 1)
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(2), FLOAT(10.0f))
        CALL(evt_btl_camera_set_moveto, 0, 83, 124, 512, 83, 89, 0, LW(1), 11)
    ELSE
        CALL(evt_btl_camera_set_moveto, 0, 71, 159, 609, 71, 118, 0, LW(1), 11)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(600)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(macho_gance_blur_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(LW(1), 48)
        SET_VALUE(LW(2), 800)
    ELSE
        SET_VALUE(LW(1), 1)
        SET_VALUE(LW(2), 16)
    END_IF
    CALL(evt_btl_camera_set_moveto, 0, 83, 137, 701, 83, 89, 0, LW(1), 11)
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 0, LW(2))
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(900)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(mario_party_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(LW(1), 30)
    ELSE
        SET_VALUE(LW(1), 1)
    END_IF
    CALL(evt_btl_camera_set_moveto, 0, -111, 67, 275, -111, 49, 0, LW(1), 11)
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(600)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_GANSU_MOVE1L"), STRING("SFX_BOSS_GANSU_MOVE1R"), 0, 10, 10)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_GANSU_MOVE1L"), STRING("SFX_BOSS_GANSU_MOVE1R"), 0, 20, 20)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    // Unit work: 0 = attack counter, 1 = phase index, 2 = attack id, 3 (float) = size scale.
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetUnitWorkFloat, -2, 3, FLOAT(1.0f))
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    SET_VALUE(LW(0), 2)
    CALL_CHILD(macho_gance_zoom)
    WAIT_MS(1000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_msg_print, 2, STRING("stg3_tou_469"), 0, -2)
    WAIT_MS(300)
    SET_VALUE(LW(0), 2)
    CALL_CHILD(mario_party_zoom)
    CALL(evt_msg_print_battle_party, STRING("stg3_tou_470"))
    WAIT_MS(300)
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseFromTable, -4, LW(0))
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SWITCH(LW(0))
            CASE_EQUAL(1)
            WAIT_MS(250)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            WAIT_MS(300)
            CASE_EQUAL(2)
            WAIT_MS(250)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            WAIT_MS(300)
            CASE_EQUAL(3)
            WAIT_MS(250)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            WAIT_MS(300)
            CASE_EQUAL(4)
            WAIT_MS(250)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            WAIT_MS(300)
            CASE_EQUAL(6)
            WAIT_MS(250)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            WAIT_MS(300)
        END_SWITCH
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    END_IF
    RETURN
EVT_END()

// Rescales the hit/cursor offsets and HP gauge by the new size / old size ratio.
EVT_BEGIN(base_size_change)
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(1))
    CALL(btlevtcmd_SetUnitWorkFloat, -2, 3, LW(0))
    DIVIDE_FLOAT(LW(0), LW(1))
    SET_FLOAT_VALUE(LW(5), LW(0))
    MULTIPLY_FLOAT(LW(5), FLOAT(26.0f))
    SUBTRACT_FLOAT(LW(5), FLOAT(26.0f))
    SET_FLOAT_VALUE(LW(6), LW(0))
    MULTIPLY_FLOAT(LW(6), FLOAT(77.0f))
    SUBTRACT_FLOAT(LW(6), FLOAT(77.0f))
    SET_FLOAT_VALUE(LW(7), LW(0))
    MULTIPLY_FLOAT(LW(7), FLOAT(0.0f))
    SUBTRACT_FLOAT(LW(7), FLOAT(0.0f))
    CALL(btlevtcmd_SetHitOffset, -2, 1, LW(5), LW(6), LW(7))
    SET_FLOAT_VALUE(LW(5), LW(0))
    MULTIPLY_FLOAT(LW(5), FLOAT(25.0f))
    SUBTRACT_FLOAT(LW(5), FLOAT(25.0f))
    SET_FLOAT_VALUE(LW(6), LW(0))
    MULTIPLY_FLOAT(LW(6), FLOAT(81.0f))
    SUBTRACT_FLOAT(LW(6), FLOAT(81.0f))
    SET_FLOAT_VALUE(LW(7), LW(0))
    MULTIPLY_FLOAT(LW(7), FLOAT(0.0f))
    SUBTRACT_FLOAT(LW(7), FLOAT(0.0f))
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_GetHpGaugeOffset, -2, LW(5), LW(6))
    MULTIPLY_FLOAT(LW(5), LW(0))
    MULTIPLY_FLOAT(LW(6), LW(0))
    CALL(btlevtcmd_SetHpGaugeOffset, -2, LW(5), LW(6))
    RETURN
EVT_END()

// Advances the phase index at 75%, 50%, and 25% remaining HP.
static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 75)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            SET_VALUE(LW(0), 0)
            CALL_CHILD(macho_gance_zoom)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
            WAIT_MS(1500)
            CALL(evt_snd_sfxoff, LW(15))
            SET_VALUE(LW(0), -900)
            IF_INT_GREATER(LW(0), 0)
                WAIT_MS(LW(0))
            END_IF
            CALL(evt_msg_print, 2, STRING("stg3_tou_471_17"), 0, -2)
            WAIT_MS(300)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_MS(1000)
        END_IF
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 50)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            SET_VALUE(LW(0), 1)
            CALL_CHILD(macho_gance_zoom)
            CALL(evt_msg_print, 2, STRING("stg3_tou_471_18"), 0, -2)
            WAIT_MS(300)
            SET_VALUE(LW(0), 0)
            CALL_CHILD(macho_gance_blur_zoom)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_BIG3"), EVTDAT_POINTER_MAX, 0, LW(15))
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_1"))
            WAIT_MS(800)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_BIG1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetRotate, -2, LW(0), LW(1), LW(2))
            SET_VALUE(LW(5), 36)
            LOOP(LW(5))
                CALL(wariai_enzan_1f, LW(5), 36, FLOAT(0.0f), FLOAT(180.0f), LW(6))
                CALL(wariai_enzan_1f, LW(5), 36, FLOAT(1.0f), FLOAT(1.15f), LW(7))
                CALL(btlevtcmd_SetBaseScale, -2, LW(7), LW(7), 1)
                ADD(LW(6), LW(1))
                CALL(btlevtcmd_SetRotate, -2, 0, LW(6), 0)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            WAIT_MS(400)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_BIG2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetRotate, -2, LW(0), LW(1), LW(2))
            SET_VALUE(LW(5), 36)
            LOOP(LW(5))
                CALL(wariai_enzan_1f, LW(5), 36, FLOAT(0.0f), FLOAT(-180.0f), LW(6))
                CALL(wariai_enzan_1f, LW(5), 36, FLOAT(1.15f), FLOAT(1.3f), LW(7))
                CALL(btlevtcmd_SetBaseScale, -2, LW(7), LW(7), 1)
                ADD(LW(6), LW(1))
                CALL(btlevtcmd_SetRotate, -2, 0, LW(6), 0)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_ResetFaceDirection, -2)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            SET_FLOAT_VALUE(LW(0), FLOAT(1.3f))
            CALL_CHILD(base_size_change)
            WAIT_MS(400)
            CALL(evt_snd_sfxoff, LW(15))
            CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.6f))
            SET_VALUE(LW(8), 2)
            LOOP(LW(8))
                BEGIN_SIBLING_EVENT
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
                    CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
                    LOOP(0)
                        CALL(btlevtcmd_GetPosFloat, -2, LW(9), LW(10), LW(11))
                        SUBTRACT_FLOAT(LW(6), LW(10))
                        IF_INT_GREATER(LW(6), FLOAT(0.0f))
                            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
                            BREAK_LOOP
                        END_IF
                        SET_FLOAT_VALUE(LW(6), LW(10))
                        WAIT_FRAMES(1)
                    END_LOOP
                END_SIBLING_EVENT
                CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 36, -1)
                BEGIN_SIBLING_EVENT
                    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_HIPDROP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 800)
                END_SIBLING_EVENT
                WAIT_MS(200)
            END_LOOP
            CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
            WAIT_MS(400)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_MS(1000)
        END_IF
        CASE_EQUAL(2)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 25)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 1, 3)
            CALL(btlevtcmd_StopWaitEvent, -2)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_furafura))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            SET_VALUE(LW(0), 1)
            CALL_CHILD(macho_gance_zoom)
            CALL(btlevtcmd_SetTalkPoseType, -2, 65)
            CALL(btlevtcmd_SetStayPoseType, -2, 43)
            CALL(evt_msg_print, 2, STRING("stg3_tou_471_19"), 0, -2)
            WAIT_MS(300)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_MS(1000)
        END_IF
    END_SWITCH
    LABEL(0)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(macho_gance_zoom)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_2"))
    WAIT_MS(170)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 500)
    WAIT_MS(200)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("T_2"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("S_2"))
    CALL(evt_msg_print, 2, STRING("stg3_tou_471_20"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(500)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_tou2_000219a8)
    CALL(btlevtcmd_ResetFaceDirection, LW(10))
    CALL(btlevtcmd_OnUnitFlag, LW(10), 1)
    BEGIN_SIBLING_EVENT
        SWITCH(LW(0))
            CASE_OR_EQUAL(43)
            CASE_OR_EQUAL(44)
            CASE_OR_EQUAL(45)
            CALL(btlevtcmd_GetUnitKind, LW(10), LW(2))
            IF_INT_EQUAL(LW(2), 222)
                CALL(btlevtcmd_GetPos, LW(10), LW(2), LW(3), LW(4))
                CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_DAMAGED1"), LW(2), LW(3), LW(4), 0)
            END_IF
            CASE_END
        END_SWITCH
    END_SIBLING_EVENT
    SWITCH(LW(0))
        CASE_EQUAL(43)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_ATTACK_MISS3"), 0)
        SET_VALUE(LW(14), 36)
        CASE_EQUAL(46)
        CALL(btlevtcmd_GetUnitKind, LW(10), LW(2))
        IF_INT_EQUAL(LW(2), 222)
            CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_VOICE_MARIO_DAMAGED1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_IF
        SET_VALUE(LW(14), 29)
        CASE_OR_EQUAL(49)
        CASE_OR_EQUAL(50)
        CASE_OR_EQUAL(54)
        SET_VALUE(LW(14), 29)
        CASE_END
        CASE_DEFAULT
        SET_VALUE(LW(14), 39)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, LW(10), LW(2), LW(3), LW(4))
    CALL(btlevtcmd_FaceDirectionSub, LW(10), LW(2), 25)
    ADD(LW(3), 25)
    CALL(btlevtcmd_DivePosition, LW(10), LW(2), LW(3), LW(4), 10, 0, 0, 0, -1)
    WAIT_MS(500)
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, LW(10), LW(2), LW(3), LW(4))
    CALL(btlevtcmd_GetHomePos, LW(10), LW(5), LW(6), LW(7))
    MULTIPLY(LW(2), 3)
    MULTIPLY(LW(5), 4)
    ADD(LW(2), LW(5))
    DIVIDE(LW(2), 7)
    CALL(btlevtcmd_TransStageFloorPosition, LW(2), LW(3), LW(4))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(2), LW(3), LW(4), 40, -1)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 250)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHomePos, LW(10), LW(5), LW(6), LW(7))
    MULTIPLY(LW(5), 2)
    ADD(LW(2), LW(5))
    DIVIDE(LW(2), 3)
    CALL(btlevtcmd_JumpPosition, LW(10), LW(2), LW(3), LW(4), 20, -1)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 166)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHomePos, LW(10), LW(2), LW(3), LW(4))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(2), LW(3), LW(4), 10, -1)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 116)
    END_SIBLING_EVENT
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
    ADD(LW(1), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(1))
    CALL(btlevtcmd_CheckStatus, -2, STATUS_FAST, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(LW(9), PTR(&weapon_gance_macho_speed))
        CALL_CHILD(gance_macho_speed_attack_event)
    ELSE
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(2))
        IF_INT_LESS(LW(2), 2)
            IF_INT_LESS_OR_EQUAL(LW(1), 1)
                CALL(btlevtcmd_DrawLots, LW(0), 3, 30, 20, 10)
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    SET_VALUE(LW(9), PTR(&weapon_gance_build_up))
                    CALL_CHILD(gance_build_up_attack_event)
                    CASE_EQUAL(1)
                    SET_VALUE(LW(9), PTR(&weapon_gance_body))
                    CALL_CHILD(gance_body_attack_event)
                    CASE_EQUAL(2)
                    SET_VALUE(LW(9), PTR(&weapon_gance_footwork))
                    CALL_CHILD(gance_footwork_attack_event)
                END_SWITCH
            ELSE
                SET_VALUE(LW(9), PTR(&weapon_gance_attack))
                CALL_CHILD(gance_attack_attack_event)
            END_IF
        ELSE
            IF_INT_LESS_OR_EQUAL(LW(1), 1)
                CALL(btlevtcmd_DrawLots, LW(0), 4, 25, 20, 10, 10)
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    SET_VALUE(LW(9), PTR(&weapon_gance_build_up))
                    CALL_CHILD(gance_build_up_attack_event)
                    CASE_EQUAL(1)
                    SET_VALUE(LW(9), PTR(&weapon_gance_body))
                    CALL_CHILD(gance_body_attack_event)
                    CASE_EQUAL(2)
                    SET_VALUE(LW(9), PTR(&weapon_gance_footwork))
                    CALL_CHILD(gance_footwork_attack_event)
                    CASE_EQUAL(3)
                    SET_VALUE(LW(9), PTR(&weapon_gance_posing))
                    CALL_CHILD(gance_posing_attack_event)
                END_SWITCH
            ELSE
                CALL(btlevtcmd_DrawLots, LW(0), 2, 10, 10)
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    SET_VALUE(LW(9), PTR(&weapon_gance_rariat_attack))
                    CALL_CHILD(gance_rariat_attack_attack_event)
                    CASE_EQUAL(1)
                    SET_VALUE(LW(9), PTR(&weapon_gance_body_attack))
                    CALL_CHILD(gance_body_attack_attack_event)
                END_SWITCH
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(gance_macho_speed_attack_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(60.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_3"))
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(gance_build_up_attack_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(50.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING3"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_5"))
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 2)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(gance_body_attack_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(50.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING4"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_4"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_7"))
    WAIT_MS(500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 3)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(gance_footwork_attack_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(50.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING5"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_6"))
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 4)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(gance_attack_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(50.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING7"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1B"))
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, kConfusedAction, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_PartsFaceDirectionSub, -2, 1, LW(0), 250)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 1166)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 70, -1, 1)
    CALL(x_flag_wait, 0, 0)
    WAIT_MS(500)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 500)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 30, -1, 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1A"))
        CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
        LOOP(0)
            CALL(btlevtcmd_GetPosFloat, -2, LW(9), LW(10), LW(11))
            SUBTRACT_FLOAT(LW(6), LW(10))
            IF_INT_GREATER(LW(6), FLOAT(0.0f))
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
                BREAK_LOOP
            END_IF
            SET_FLOAT_VALUE(LW(6), LW(10))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 36, -1)
    CALL(x_flag_set, 0, 0)
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
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_HIPDROP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 800)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 18)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 9, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(300)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_HIPDROP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 800)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 48)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 24)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 12)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
    LABEL(98)
    CALL(x_flag_wait, 0, 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 666)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 40, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(x_flag_wait, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(gance_posing_attack_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(50.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_SIBLING_EVENT
        WAIT_MS(50)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_FIST1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(180)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_FIST1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_8"))
    WAIT_MS(1000)
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 6)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(gance_rariat_attack_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(90.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(20.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(0), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING8"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_2"))
    WAIT_MS(1000)
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, kConfusedAction, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2A"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 200)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    CALL(x_flag_set, 0, 0)
    CALL(x_flag_set, 1, 0)
    LOOP(0)
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStatusMg, -2, LW(7))
        MULTIPLY_FLOAT(LW(7), 35)
        CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
        MULTIPLY_FLOAT(LW(7), LW(6))
        ADD_FLOAT(LW(7), 15)
        ADD(LW(0), LW(7))
        ADD(LW(2), 6)
        IF_INT_EQUAL(LW(11), 0)
            CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
            CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
        ELSE
            CALL(x_flag_set, 1, 1)
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
                CALL(x_flag_set, 1, 0)
            END_SIBLING_EVENT
        END_IF
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        SWITCH(LW(5))
            CASE_OR_EQUAL(4)
            SET_VALUE(LW(6), 90)
            CASE_END
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            SET_VALUE(LW(6), 90)
            CASE_END
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            SET_VALUE(LW(6), 90)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
            SET_VALUE(LW(6), 90)
            CASE_EQUAL(1)
            SET_VALUE(LW(6), 91)
            CASE_END
            CASE_DEFAULT
            SET_VALUE(LW(6), 97)
            CASE_END
        END_SWITCH
        GOTO(LW(6))
        LABEL(90)
        GOTO(97)
        LABEL(91)
        IF_INT_EQUAL(LW(11), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LW(10), 1)
            CALL(x_flag_set, 0, 1)
            BEGIN_SIBLING_EVENT
                CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 800)
                CALL(x_flag_set, 0, 0)
            END_SIBLING_EVENT
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        LABEL(97)
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        ADD(LW(11), 1)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(x_flag_wait, 1, 0)
    SUBTRACT(LW(0), LW(7))
    ADD(LW(2), 6)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    WAIT_MS(500)
    CALL(x_flag_wait, 0, 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 666)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 40, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(x_flag_wait, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(gance_body_attack_attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("E_1A"))
    WAIT_MS(1500)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetPosFloat, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(50.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT_FLOAT(LW(2), FLOAT(40.0f))
    CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_PAUSING9"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_1"))
    WAIT_MS(1000)
    CALL(evt_eff_delete, STRING("hikari"))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetRotate, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 12)
    LOOP(LW(5))
        CALL(wariai_enzan_1f, LW(5), 12, FLOAT(0.0f), FLOAT(180.0f), LW(6))
        ADD(LW(6), LW(1))
        CALL(btlevtcmd_SetRotate, -2, 0, LW(6), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_MS(500)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, kConfusedAction, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(300)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
        CALL(btlevtcmd_GetStatusMg, -2, LW(0))
        MULTIPLY_FLOAT(LW(0), FLOAT(70.0f))
        CALL(btlevtcmd_GetUnitWorkFloat, -2, 3, LW(1))
        MULTIPLY_FLOAT(LW(0), LW(1))
        CALL(btlevtcmd_SetRotateOffset, -2, FLOAT(0.0f), LW(0), FLOAT(0.0f))
        SET_VALUE(LW(5), FLOAT(28.8f))
        LOOP(LW(5))
            CALL(wariai_enzan_1f_intpl, LW(5), FLOAT(28.8f), FLOAT(0.0f), FLOAT(320.0f), LW(6), 5)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(6))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_SetRotateOffset, -2, FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 36, -1)
    CALL(x_flag_set, 0, 0)
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
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_HIPDROP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 800)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1D"))
    WAIT_MS(300)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GANSU_HIPDROP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 800)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1D"))
    CALL(btlevtcmd_JumpContinue, -2)
    LABEL(98)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 18)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 9, -1)
    CALL(x_flag_wait, 0, 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(x_flag_set, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 666)
        CALL(x_flag_set, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 40, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(x_flag_wait, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// Event flags: sibling events set/wait on a flag so the script can resume once they finish.
static s32 x_flag_data[2];

#include "rel/common/wariai_enzan_1.h"

static inline f32 gance_intpl(s32 mode, f32 start, f32 end, s32 step, s32 steps) {
    return intplGetValue(mode, step, steps, start, end);
}
// args[5] is an intplGetValue mode; 14..27 run mode - 14 mirrored (1 - its curve from 1 to 0), 28 and up
// output start.
static USER_FUNC(wariai_enzan_1f_intpl) {
    s32* args = event->args;
    f32 remaining = evtGetFloat(event, args[0]) - 1.0f;
    f32 steps = evtGetFloat(event, args[1]);
    f32 start = evtGetFloat(event, args[2]);
    f32 end = evtGetFloat(event, args[3]);
    s32 out = args[4];
    f32 mode = evtGetFloat(event, args[5]);
    f32 ratio;

    if (mode >= 14.0f) {
        mode -= 14.0f;
        if (mode >= 14.0f) {
            evtSetFloat(event, out, start);
            return EVT_RETURN_DONE;
        }
        ratio = 1.0f - gance_intpl((s32)mode, 1.0f, 0.0f, (s32)(steps - remaining), (s32)steps);
    } else {
        ratio = gance_intpl((s32)mode, 0.0f, 1.0f, (s32)(steps - remaining), (s32)steps);
    }
    evtSetFloat(event, out, (1.0f - ratio) * start + ratio * end);
    return EVT_RETURN_DONE;
}

USER_FUNC(wariai_enzan_1f_intpl_w) {
    s32* args = event->args;
    f32 remaining = evtGetFloat(event, args[0]) - 1.0f;
    f32 steps = evtGetFloat(event, args[1]);
    f32 start = evtGetFloat(event, args[2]);
    f32 end = evtGetFloat(event, args[3]);
    s32 out = args[4];
    f32 mode = evtGetFloat(event, args[5]);
    f32 mode2 = evtGetFloat(event, args[6]);
    f32 ratio1;
    f32 ratio2;
    f32 ratio;

    if (mode >= 14.0f) {
        mode -= 14.0f;
        if (mode >= 14.0f) {
            evtSetFloat(event, out, start);
            return EVT_RETURN_DONE;
        }
        ratio1 = 1.0f - gance_intpl((s32)mode, 1.0f, 0.0f, (s32)(steps - remaining), (s32)steps);
    } else {
        ratio1 = gance_intpl((s32)mode, 0.0f, 1.0f, (s32)(steps - remaining), (s32)steps);
    }
    if (mode2 >= 14.0f) {
        // Retail derives this from the first mode, not mode2.
        f32 reverseMode2 = mode - 14.0f;
        if (reverseMode2 >= 14.0f) {
            evtSetFloat(event, out, start);
            return EVT_RETURN_DONE;
        }
        ratio2 = 1.0f - gance_intpl((s32)reverseMode2, 1.0f, 0.0f, (s32)(steps - remaining), (s32)steps);
    } else {
        ratio2 = gance_intpl((s32)mode2, 0.0f, 1.0f, (s32)(steps - remaining), (s32)steps);
    }
    ratio = ratio1 * ratio2;
    evtSetFloat(event, out, start * (1.0f - ratio) + end * ratio);
    return EVT_RETURN_DONE;
}

static USER_FUNC(wariai_enzan_0) {
    s32* args = event->args;
    f32 remaining = evtGetFloat(event, args[0]) - 1.0f;
    f32 lastStep = evtGetFloat(event, args[1]) - 1.0f;
    f32 start = evtGetFloat(event, args[2]);
    f32 end = evtGetFloat(event, args[3]);

    evtSetValue(event, args[4], (s32)((remaining * start + end * (lastStep - remaining)) / lastStep));
    return EVT_RETURN_DONE;
}

static USER_FUNC(wariai_enzan_0f) {
    s32* args = event->args;
    f32 remaining = evtGetFloat(event, args[0]) - 1.0f;
    f32 lastStep = evtGetFloat(event, args[1]) - 1.0f;
    f32 start = evtGetFloat(event, args[2]);
    f32 end = evtGetFloat(event, args[3]);

    evtSetFloat(event, args[4], (remaining * start + end * (lastStep - remaining)) / lastStep);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_set) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[index] = value;
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_add) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[index] += value;
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_get) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], x_flag_data[index]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_wait) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    return (value == x_flag_data[index]) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
// clang-format on

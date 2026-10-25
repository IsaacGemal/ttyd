#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_sub.h"
#include "battle/battle_disp.h"
#include "battle/battle_weapon_power.h"
#include "eff/n64/eff_magic1_n64.h"
#include "eff/n64/eff_pokopi_pcharge_n64.h"
#include "eff/n64/eff_thunderflare_n64.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"
#include <string.h>
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(eff_magic_get_posX);
static USER_FUNC(eff_magic);
static USER_FUNC(eff_staff2);
static USER_FUNC(eff_staff1);
static USER_FUNC(kamec_copy_status);
static USER_FUNC(kamec_shuffle);
static USER_FUNC(kamec_get_bunsin);
static USER_FUNC(kamec_get_live_num);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[5];
static BattleDataEntry data_table_bunsin[3];
static BattleUnitKindPart parts[2];
static s32 init_event[22];
static s32 init_event2[87];
static s32 damage_event[66];
static s32 kamec_subset_bunsin_all_kill[68];
static s32 kamec_subset_dead_event[203];
static s32 kamec_subset_blow_dead_event[78];
static s32 avoid_miss_event[16];
static s32 bunsin_kill_event[37];
static s32 dmg_fall_event[159];
static s32 dmg_first_attack_pos[19];
static s32 attack_event[296];
static s32 attack_magic_event[325];
static s32 attack_magic_event_sub_no_hit[45];
static s32 attack_magic_event_sub_hit[30];
static s32 dekadeka_magic_event[7];
static s32 katikati_magic_event[7];
static s32 biribiri_magic_event[7];
static s32 trans_magic_event[7];
static s32 recover_magic_event[142];
static s32 all_recover_magic_event[180];
static s32 bunsin_magic_event[160];
static s32 spawn_event[66];
static s32 wait_event[55];

// clang-format off
extend_data(124,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(125,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// field_0xBC[4] (the init event pointer) are u8[] regions holding pointers, which cannot be written
// as bytes at compile time. Everything else (unit type 0x31, parts/data table at 0xB8/0xC0) fits.
BattleUnitKind unit_kamec_red = {
    UNIT_RED_MAGIKOOPA,         // mUnitType
    "btl_un_kamec_red",         // name
    7,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    17,                         // mLevel
    0,                          // mBonusExp
    2,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    2,                          // baseCoin
    50,                         // escapeRate
    9999,                       // renzokuSoftCap
    42,                         // width
    42,                         // height
    { 0, 42 },                  // statusIconOffset
    { 0.0f, 21.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 21.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 21.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 21.0f, 27.3f, 0.0f },     // kissHitOffset
    { 0.0f, 21.0f, 0.0f },      // cutBaseOffset
    42.0f,                      // cutWidth
    42.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_KAMEKU_DAMAGED1",  // damageSfxName
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

BattleUnitKind unit_kamec_red_bunsin = {
    UNIT_RED_MAGIKOOPA_CLONE,   // mUnitType
    "btl_un_kamec_red_bunsin",  // name
    7,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    9999,                       // renzokuSoftCap
    42,                         // width
    42,                         // height
    { 0, 42 },                  // statusIconOffset
    { 0.0f, 21.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 21.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 21.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 21.0f, 27.3f, 0.0f },     // kissHitOffset
    { 0.0f, 21.0f, 0.0f },      // cutBaseOffset
    42.0f,                      // cutWidth
    42.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    0,                          // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_KAMEKU_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00400000,                 // attributeFlags
    &regist,                    // vulnStatus
    2,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table_bunsin,          // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    70,  // sleepRate
    75,  // stopRate
    80,  // dizzyRate
    100, // poisonRate
    70,  // confuseRate
    100, // electricRate
    100, // burnRate
    70,  // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    80,  // allergicRate
    70,  // frightRate
    80,  // galeForceRate
    100, // fastRate
    80,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleVulnerableStatus regist_bunsin = {
    0, // sleepRate
    0, // stopRate
    0, // dizzyRate
    0, // poisonRate
    0, // confuseRate
    0, // electricRate
    0, // burnRate
    0, // freezeRate
    0, // hugeRate
    0, // tinyRate
    0, // attackUpRate
    0, // attackDownRate
    0, // defenseUpRate
    0, // defenseDownRate
    0, // allergicRate
    0, // frightRate
    0, // galeForceRate
    0, // fastRate
    0, // slowRate
    0, // dodgyRate
    0, // invisibleRate
    0, // instaKillRate
};

static BattleDispPose pose_table[17] = {
    { 0x01, "KMR_N_1" },
    { 0x02, "KMR_Y_1" },
    { 0x09, "KMR_Y_1" },
    { 0x05, "KMR_K_1" },
    { 0x04, "KMR_X_1" },
    { 0x03, "KMR_X_1" },
    { 0x1C, "KMR_S_1" },
    { 0x1D, "KMR_Q_1" },
    { 0x1E, "KMR_Q_1" },
    { 0x1F, "KMR_S_1" },
    { 0x27, "KMR_D_1" },
    { 0x2A, "KMR_R_1" },
    { 0x28, "KMR_W_1" },
    { 0x38, "KMR_X_1" },
    { 0x39, "KMR_X_1" },
    { 0x41, "KMR_T_1" },
    { 0x45, "KMR_S_1" },
};

static BattleDispPose pose_table_sky[17] = {
    { 0x01, "KMR_N_2" },
    { 0x02, "KMR_Y_2" },
    { 0x09, "KMR_Y_2" },
    { 0x05, "KMR_K_2" },
    { 0x04, "KMR_X_2" },
    { 0x03, "KMR_X_2" },
    { 0x1C, "KMR_S_2" },
    { 0x1D, "KMR_Q_2" },
    { 0x1E, "KMR_Q_2" },
    { 0x1F, "KMR_S_2" },
    { 0x27, "KMR_D_2" },
    { 0x2A, "KMR_R_2" },
    { 0x28, "KMR_W_2" },
    { 0x38, "KMR_X_2" },
    { 0x39, "KMR_X_2" },
    { 0x41, "KMR_T_2" },
    { 0x45, "KMR_S_2" },
};

static BattleDataEntry data_table[5] = {
    { 0x31, kamec_subset_dead_event },
    { 0x3A, kamec_subset_blow_dead_event },
    { 0x2E, avoid_miss_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleDataEntry data_table_sky[6] = {
    { 0x31, kamec_subset_dead_event },
    { 0x3A, kamec_subset_blow_dead_event },
    { 0x2E, avoid_miss_event },
    { 0x0E, dmg_fall_event },
    { 0x30, dmg_first_attack_pos },
    { 0x00, NULL },
};

static BattleDataEntry data_table_bunsin[3] = {
    { 0x2E, avoid_miss_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleDataEntry data_table_sky_bunsin[4] = {
    { 0x2E, avoid_miss_event },
    { 0x0E, dmg_fall_event },
    { 0x30, dmg_first_attack_pos },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[2] = {
    {
        1,                     // partNum
        "btl_un_kamec_red",    // partName
        "c_kamek_r",           // modelName
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
        2,                    // partNum
        "btl_un_kamec_red",   // partName
        "c_kamek_r",          // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // shadowSize
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
};

static BattleWeapon weapon_kamec_attack_magic = {
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
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x000000C7,                               // special
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

static BattleWeapon weapon_kamec_dekadeka_magic = {
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
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000108,                               // special
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

static BattleWeapon weapon_kamec_biribiri_magic = {
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
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000108,                               // special
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

static BattleWeapon weapon_kamec_trans_magic = {
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
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000108,                               // special
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

static BattleWeapon weapon_kamec_katikati_magic = {
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
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000108,                               // special
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

static BattleWeapon weapon_kamec_recover_magic = {
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
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0,                                        // special
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

static BattleWeapon weapon_kamec_all_recover_magic = {
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
    0,                                        // damagePattern
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

static BattleUnitSetup entry_kamec[1] = {
    { &unit_kamec_red_bunsin, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    CALL_CHILD(init_event2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(init_event2)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_sky))
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_ChangeDataTable, -2, PTR(&data_table_sky))
        ELSE
            CALL(btlevtcmd_ChangeDataTable, -2, PTR(&data_table_sky_bunsin))
        END_IF
        CALL(btlevtcmd_OnAttribute, -2, 4)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 2097152)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 2048)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4194304)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPos, -2, LW(0), 40, LW(2))
        CALL(btlevtcmd_SetHomePos, -2, LW(0), 40, LW(2))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(alpha_down_event)
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 254)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    CALL(evt_sub_intpl_init, 0, 254, 0, 30)
    LOOP(30)
        CALL(evt_sub_intpl_get_value_para, LW(1), LW(2))
        CALL(btlevtcmd_SetAlpha, LW(0), 1, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_OnPartsAttribute, LW(0), 1, 33554432)
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 0)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

static EVT_BEGIN(alpha_up_event)
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_OffPartsAttribute, LW(0), 1, 33554432)
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 0)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    CALL(evt_sub_intpl_init, 0, 0, 254, 30)
    LOOP(30)
        CALL(evt_sub_intpl_get_value_para, LW(1), LW(2))
        CALL(btlevtcmd_SetAlpha, LW(0), 1, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 255)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(kamec_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
        END_IF
    ELSE
        SET_VALUE(LW(0), -2)
        CALL_CHILD(bunsin_kill_event)
        RETURN
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL_CHILD(kamec_subset_bunsin_all_kill)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(kamec_subset_bunsin_all_kill)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(kamec_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_WaitAttackEnd)
            CALL(kamec_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
            SET_VALUE(LW(0), LW(1))
            SPAWN_EVENT(bunsin_kill_event)
            SET_VALUE(LW(0), LW(2))
            SPAWN_EVENT(bunsin_kill_event)
            SET_VALUE(LW(0), LW(3))
            SPAWN_EVENT(bunsin_kill_event)
            SET_VALUE(LW(0), LW(4))
            SPAWN_EVENT(bunsin_kill_event)
            LOOP(0)
                CALL(kamec_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(kamec_subset_dead_event)
    CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(0), 50)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 50)
    END_IF
    SET_VALUE(LW(0), 48)
    CALL(btlevtcmd_OnUnitFlag, LW(10), 134217728)
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
    CALL(btlevtcmd_ClearAllStatus, LW(10))
    CALL(btlevtcmd_OnAttribute, LW(10), 16777216)
    CALL(btlevtcmd_OnAttribute, LW(10), 536870912)
    CALL_CHILD(kamec_subset_bunsin_all_kill)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, LW(10), 0)
    ELSE
        CALL(btlevtcmd_KillUnit, LW(10), 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(kamec_subset_blow_dead_event)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_CLAUD_BREATH3"), 0)
    CALL(btlevtcmd_GetBodyId, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 39)
    BEGIN_SIBLING_EVENT
        LABEL(0)
        CALL(btlevtcmd_AddRotate, -2, 0, 20, 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    ADD(LW(1), 40)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -10, 2, 0, -1)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL_CHILD(kamec_subset_bunsin_all_kill)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(avoid_miss_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(0), -2)
        CALL_CHILD(bunsin_kill_event)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(bunsin_kill_event)
    CALL(btlevtcmd_GetUnitId, LW(0), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(0), 3, LW(1))
    IF_INT_EQUAL(LW(1), 1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_SetUnitWork, LW(0), 3, 1)
    CALL_CHILD(alpha_down_event)
    LABEL(10)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_KillUnit, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_fall_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_OffAttribute, -2, 4)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2048)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_ChangeDataTable, -2, PTR(&data_table))
    ELSE
        CALL(btlevtcmd_ChangeDataTable, -2, PTR(&data_table_bunsin))
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KMR_H_1"))
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        WAIT_FRAMES(20)
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
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    WAIT_MS(500)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL_CHILD(init_event2)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_first_attack_pos)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 10)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(9), PTR(&weapon_kamec_attack_magic))
        CALL_CHILD(attack_magic_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        GOTO(99)
    END_IF
    CALL(kamec_get_live_num, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        LABEL(10)
        SET_VALUE(LW(1), 40)
        ADD(LW(1), 0)
        ADD(LW(1), 40)
        SUBTRACT(LW(1), 1)
        CALL(evt_sub_random, LW(1), LW(0))
        SET_VALUE(LW(1), 40)
        IF_INT_LESS(LW(0), LW(1))
            SET_VALUE(LW(9), PTR(&weapon_kamec_attack_magic))
            CALL_CHILD(attack_magic_event)
            GOTO(99)
        END_IF
        ADD(LW(1), 0)
        IF_INT_LESS(LW(0), LW(1))
            SET_VALUE(LW(9), PTR(&weapon_kamec_recover_magic))
            CALL_CHILD(recover_magic_event)
            IF_INT_EQUAL(LW(0), 0)
                GOTO(10)
            END_IF
            GOTO(99)
        ELSE
            CALL_CHILD(bunsin_magic_event)
            GOTO(99)
        END_IF
    ELSE
        CALL(kamec_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL_CHILD(subsetevt_confuse_flustered)
                GOTO(99)
            END_IF
            SET_VALUE(LW(9), PTR(&weapon_kamec_attack_magic))
            CALL_CHILD(attack_magic_event)
            GOTO(99)
        ELSE
            LABEL(20)
            SET_VALUE(LW(1), 50)
            ADD(LW(1), 20)
            ADD(LW(1), 20)
            ADD(LW(1), 0)
            ADD(LW(1), 0)
            ADD(LW(1), 0)
            ADD(LW(1), 0)
            CALL(evt_sub_random, LW(1), LW(0))
            SET_VALUE(LW(1), 50)
            IF_INT_LESS(LW(0), LW(1))
                SET_VALUE(LW(9), PTR(&weapon_kamec_attack_magic))
                CALL_CHILD(attack_magic_event)
                GOTO(99)
            END_IF
            ADD(LW(1), 20)
            IF_INT_LESS(LW(0), LW(1))
                SET_VALUE(LW(9), PTR(&weapon_kamec_dekadeka_magic))
                CALL_CHILD(dekadeka_magic_event)
                IF_INT_EQUAL(LW(0), 0)
                    GOTO(20)
                END_IF
                GOTO(99)
            END_IF
            ADD(LW(1), 20)
            IF_INT_LESS(LW(0), LW(1))
                SET_VALUE(LW(9), PTR(&weapon_kamec_katikati_magic))
                CALL_CHILD(katikati_magic_event)
                IF_INT_EQUAL(LW(0), 0)
                    GOTO(20)
                END_IF
                GOTO(99)
            END_IF
            ADD(LW(1), 0)
            IF_INT_LESS(LW(0), LW(1))
                SET_VALUE(LW(9), PTR(&weapon_kamec_biribiri_magic))
                CALL_CHILD(biribiri_magic_event)
                IF_INT_EQUAL(LW(0), 0)
                    GOTO(20)
                END_IF
                GOTO(99)
            END_IF
            ADD(LW(1), 0)
            IF_INT_LESS(LW(0), LW(1))
                SET_VALUE(LW(9), PTR(&weapon_kamec_trans_magic))
                CALL_CHILD(trans_magic_event)
                IF_INT_EQUAL(LW(0), 0)
                    GOTO(20)
                END_IF
                GOTO(99)
            END_IF
            ADD(LW(1), 0)
            IF_INT_LESS(LW(0), LW(1))
                SET_VALUE(LW(9), PTR(&weapon_kamec_recover_magic))
                CALL_CHILD(recover_magic_event)
                IF_INT_EQUAL(LW(0), 0)
                    GOTO(20)
                END_IF
                GOTO(99)
            ELSE
                SET_VALUE(LW(9), PTR(&weapon_kamec_all_recover_magic))
                CALL_CHILD(all_recover_magic_event)
                IF_INT_EQUAL(LW(0), 0)
                    GOTO(20)
                END_IF
                GOTO(99)
            END_IF
        END_IF
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(ground_magic_common_event1)
    BEGIN_SIBLING_EVENT
        LOOP(2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(40)
        END_LOOP
    END_SIBLING_EVENT
    CALL(kamec_get_bunsin, LW(11), LW(12), LW(13), LW(14), LW(0))
    IF_INT_EQUAL(LW(15), 0)
        BEGIN_SIBLING_EVENT
            CALL(eff_staff1, -2)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(11), -1)
                CALL(eff_staff1, LW(11))
            END_IF
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(12), -1)
                CALL(eff_staff1, LW(12))
            END_IF
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(13), -1)
                CALL(eff_staff1, LW(13))
            END_IF
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(14), -1)
                CALL(eff_staff1, LW(14))
            END_IF
        END_SIBLING_EVENT
    ELSE
        BEGIN_SIBLING_EVENT
            CALL(eff_staff2, -2)
        END_SIBLING_EVENT
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_1A"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_1A"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_1A"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_1A"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_1A"))
    END_IF
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_1B"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_1B"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_1B"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_1B"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_1B"))
    END_IF
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_1C"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_1C"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_1C"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_1C"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_1C"))
    END_IF
    WAIT_FRAMES(30)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC8"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_1D"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_1D"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_1D"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_1D"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_1D"))
    END_IF
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_1E"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_1E"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_1E"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_1E"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_1E"))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(ground_magic_common_event2)
    BEGIN_SIBLING_EVENT
        LOOP(2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(40)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(eff_staff1, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_1A"))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_2A"))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC7"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_2B"))
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_2C"))
    RETURN
EVT_END()

static EVT_BEGIN(sky_magic_common_event)
    BEGIN_SIBLING_EVENT
        LOOP(2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(40)
        END_LOOP
    END_SIBLING_EVENT
    CALL(kamec_get_bunsin, LW(11), LW(12), LW(13), LW(14), LW(0))
    IF_INT_EQUAL(LW(15), 0)
        BEGIN_SIBLING_EVENT
            CALL(eff_staff1, -2)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(11), -1)
                CALL(eff_staff1, LW(11))
            END_IF
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(12), -1)
                CALL(eff_staff1, LW(12))
            END_IF
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(13), -1)
                CALL(eff_staff1, LW(13))
            END_IF
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            IF_INT_NOT_EQUAL(LW(14), -1)
                CALL(eff_staff1, LW(14))
            END_IF
        END_SIBLING_EVENT
    ELSE
        BEGIN_SIBLING_EVENT
            CALL(eff_staff2, -2)
        END_SIBLING_EVENT
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_3A"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_3A"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_3A"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_3A"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_3A"))
    END_IF
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC8"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_3B"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_3B"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_3B"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_3B"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_3B"))
    END_IF
    WAIT_FRAMES(35)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC7"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_3C"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_3C"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_3C"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_3C"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_3C"))
    END_IF
    WAIT_FRAMES(15)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KMR_A_3D"))
    IF_INT_NOT_EQUAL(LW(11), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(11), 1, STRING("KMR_A_3D"))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(12), 1, STRING("KMR_A_3D"))
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(13), 1, STRING("KMR_A_3D"))
    END_IF
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_AnimeChangePose, LW(14), 1, STRING("KMR_A_3D"))
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

static EVT_BEGIN(attack_magic_event)
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
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(15), 0)
    CALL_CHILD(magic_common_event1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_MAGIC3_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_mode, 0, 0)
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
        CALL(kamec_get_bunsin, LW(6), LW(7), LW(8), LW(13), LW(0))
        SET_VALUE(LW(14), -2)
        CALL_CHILD(attack_magic_event_sub_no_hit)
        SET_VALUE(LW(14), LW(6))
        CALL_CHILD(attack_magic_event_sub_no_hit)
        SET_VALUE(LW(14), LW(7))
        CALL_CHILD(attack_magic_event_sub_no_hit)
        SET_VALUE(LW(14), LW(8))
        CALL_CHILD(attack_magic_event_sub_no_hit)
        SET_VALUE(LW(14), LW(13))
        CALL_CHILD(attack_magic_event_sub_no_hit)
        WAIT_FRAMES(60)
        GOTO(98)
        LABEL(91)
        CALL(kamec_get_bunsin, LW(6), LW(7), LW(8), LW(13), LW(0))
        SET_VALUE(LW(14), -2)
        CALL_CHILD(attack_magic_event_sub_hit)
        SET_VALUE(LW(14), LW(6))
        CALL_CHILD(attack_magic_event_sub_hit)
        SET_VALUE(LW(14), LW(7))
        CALL_CHILD(attack_magic_event_sub_hit)
        SET_VALUE(LW(14), LW(8))
        CALL_CHILD(attack_magic_event_sub_hit)
        SET_VALUE(LW(14), LW(13))
        CALL_CHILD(attack_magic_event_sub_hit)
        WAIT_FRAMES(45)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(98)
        LABEL(98)
        WAIT_FRAMES(30)
        CALL(kamec_get_bunsin, LW(6), LW(7), LW(8), LW(9), LW(0))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        IF_INT_NOT_EQUAL(LW(6), -1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(6), 1, 43)
        END_IF
        IF_INT_NOT_EQUAL(LW(7), -1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(7), 1, 43)
        END_IF
        IF_INT_NOT_EQUAL(LW(8), -1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(8), 1, 43)
        END_IF
        IF_INT_NOT_EQUAL(LW(9), -1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(9), 1, 43)
        END_IF
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(attack_magic_event_sub_no_hit)
    IF_INT_EQUAL(LW(14), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetPos, LW(14), LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
    CALL(btlevtcmd_GetStageSize, LW(10), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, LW(14), LW(0))
    MULTIPLY(LW(10), LW(0))
    SUBTRACT(LW(11), 5)
    CALL(eff_magic, LW(14), LW(10), LW(11), LW(12), 60)
    RETURN
EVT_END()

static EVT_BEGIN(attack_magic_event_sub_hit)
    IF_INT_EQUAL(LW(14), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetPos, LW(14), LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
    CALL(eff_magic, LW(14), LW(10), LW(11), LW(12), 45)
    RETURN
EVT_END()

static EVT_BEGIN(support_magic_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            SET_VALUE(LW(0), 1)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    IF_INT_EQUAL(LW(15), 10)
        CALL(btlevtcmd_CheckStatus, LW(3), 10, LW(0))
    END_IF
    IF_INT_EQUAL(LW(15), 14)
        CALL(btlevtcmd_CheckStatus, LW(3), 14, LW(0))
    END_IF
    IF_INT_EQUAL(LW(15), 6)
        CALL(btlevtcmd_CheckStatus, LW(3), 6, LW(0))
    END_IF
    IF_INT_EQUAL(LW(15), 18)
        CALL(btlevtcmd_CheckStatus, LW(3), 18, LW(0))
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(15), 0)
    CALL_CHILD(magic_common_event1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(30)
    LABEL(99)
    SET_VALUE(LW(0), 1)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dekadeka_magic_event)
    SET_VALUE(LW(15), 10)
    CALL_CHILD(support_magic_event)
    RETURN
EVT_END()

static EVT_BEGIN(katikati_magic_event)
    SET_VALUE(LW(15), 14)
    CALL_CHILD(support_magic_event)
    RETURN
EVT_END()

static EVT_BEGIN(biribiri_magic_event)
    SET_VALUE(LW(15), 6)
    CALL_CHILD(support_magic_event)
    RETURN
EVT_END()

static EVT_BEGIN(trans_magic_event)
    SET_VALUE(LW(15), 18)
    CALL_CHILD(support_magic_event)
    RETURN
EVT_END()

static EVT_BEGIN(recover_magic_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            SET_VALUE(LW(0), 1)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetHp, LW(3), LW(0))
    CALL(btlevtcmd_GetMaxHp, LW(3), LW(1))
    IF_INT_EQUAL(LW(0), LW(1))
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(15), 0)
    CALL_CHILD(magic_common_event1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 8)
    CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, LW(3), LW(5))
    CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    ADD(LW(1), LW(5))
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 8, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(40)
    LABEL(99)
    SET_VALUE(LW(0), 1)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(all_recover_magic_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            SET_VALUE(LW(0), 1)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(5)
    CALL(btlevtcmd_GetHp, LW(3), LW(0))
    CALL(btlevtcmd_GetMaxHp, LW(3), LW(1))
    IF_INT_LESS(LW(0), LW(1))
        GOTO(8)
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    GOTO(5)
    LABEL(8)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL_CHILD(magic_common_event2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(10)
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_random, 10, LW(0))
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 4)
        CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, LW(3), LW(5))
        CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
        MULTIPLY_FLOAT(LW(5), LW(6))
        ADD(LW(1), LW(5))
        CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 4, 0, 0, 0, 0, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(10)
    END_IF
    SET_VALUE(LW(0), 1)
    WAIT_FRAMES(120)
    RETURN
EVT_END()

static EVT_BEGIN(bunsin_magic_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(15), 1)
    CALL_CHILD(magic_common_event1)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_OFFSHOOT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(0), -2)
    CALL_CHILD(alpha_down_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
        SET_VALUE(LW(4), LW(0))
        IF_INT_GREATER(LW(4), 0)
            SUBTRACT(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL_CHILD(spawn_event)
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
                CALL_CHILD(spawn_event)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(kamec_shuffle)
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KAMEKU_OFFSHOOT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(0), -2)
    SPAWN_EVENT(alpha_up_event)
    CALL(kamec_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
    SET_VALUE(LW(0), LW(1))
    SPAWN_EVENT(alpha_up_event)
    SET_VALUE(LW(0), LW(2))
    SPAWN_EVENT(alpha_up_event)
    SET_VALUE(LW(0), LW(3))
    SPAWN_EVENT(alpha_up_event)
    SET_VALUE(LW(0), LW(4))
    SPAWN_EVENT(alpha_up_event)
    WAIT_FRAMES(60)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(spawn_event)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_kamec), 0)
    CALL(btlevtcmd_SetAlpha, LW(3), 1, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(10))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(13))
    CALL(kamec_copy_status, LW(13), LW(3))
    CALL(btlevtcmd_OffAttribute, LW(3), 33554432)
    CALL(btlevtcmd_SetUnitWork, LW(3), 1, 1)
    CALL(btlevtcmd_SetEventAttack, LW(3), PTR(attack_event))
    CALL(btlevtcmd_SetRegistStatus, LW(3), PTR(&regist_bunsin))
    CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 16384)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(kamec_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    IF_INT_NOT_EQUAL(LW(1), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(1), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(2), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(4), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(4), 1)
    END_IF
    RETURN
EVT_END()

#include "rel/common/kamec_get_live_num.h"

static USER_FUNC(kamec_get_bunsin) {
    s32* args = event->args;
    s32 ids[4];
    s32 num = 0;
    s32 i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = wp->units[i];

        if (unit != NULL && unit->currentKind == UNIT_RED_MAGIKOOPA_CLONE) {
            ids[num++] = i;
        }
    }
    for (i = 0; i < 4; i++) {
        if (i < num) {
            evtSetValue(event, *args++, ids[i]);
        } else {
            evtSetValue(event, *args++, -1);
        }
    }
    evtSetValue(event, *args, num);
    return EVT_RETURN_DONE;
}

static USER_FUNC(kamec_shuffle) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* bunsin[6];
    s32 num = 0;
    s32 i;

    for (i = 0; i < 64; i++) {
        bunsin[num] = wp->units[i];
        if (bunsin[num] != NULL &&
            (bunsin[num]->currentKind == UNIT_RED_MAGIKOOPA || bunsin[num]->currentKind == UNIT_RED_MAGIKOOPA_CLONE)) {
            num++;
        }
    }
    for (i = 0; i < 10; i++) {
        s32 idx1 = irand(num);
        s32 idx2 = irand(num);
        BattleUnit* a = bunsin[idx1];
        BattleUnit* b = bunsin[idx2];
        f32 tmp;

        tmp = a->mPosition.x;
        a->mPosition.x = b->mPosition.x;
        b->mPosition.x = tmp;
        tmp = a->mHomePosition.x;
        a->mHomePosition.x = b->mHomePosition.x;
        b->mHomePosition.x = tmp;
        tmp = a->mPosition.z;
        a->mPosition.z = b->mPosition.z;
        b->mPosition.z = tmp;
        tmp = a->mHomePosition.z;
        a->mHomePosition.z = b->mHomePosition.z;
        b->mHomePosition.z = tmp;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(kamec_copy_status) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 b = evtGetValue(event, args[1]);
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* src = BattleGetUnitPtr(wp, a);
    BattleUnit* dst = BattleGetUnitPtr(wp, b);
    f32 ratio;

    dst->currentHp = src->currentHp;
    memcpy(&dst->mSleepTurns, &src->mSleepTurns, 0x1E);
    dst->hpGauge.fromHp = dst->currentHp;
    dst->hpGauge.toHp = dst->currentHp;
    ratio = (f32)dst->currentHp / (f32)dst->maxHp;
    dst->hpGauge.drainRatio = ratio;
    dst->hpGauge.fillRatio = ratio;
    return EVT_RETURN_DONE;
}

static USER_FUNC(eff_staff1) {
    s32* args = event->args;
    s32 id = BattleTransID(event, evtGetValue(event, args[0]));
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    EffectEntry* eff;
    EffThunderflareData* data;
    f32 x, y, z;

    if (isFirstCall) {
        unit->work[4] = (s32)effThunderflareN64Entry(0, 0x3C, 0.0f, -1000.0f, 0.0f, 1.0f);
    }
    eff = (EffectEntry*)unit->work[4];
    if ((eff->flags & 1) == 0) {
        return EVT_RETURN_DONE;
    }
    data = eff->userdata;
    data->scale = unit->mSizeMultiplier;
    BtlUnit_GetPos(unit, &x, &y, &z);
    data->x = 22.5f * unit->mSizeMultiplier * unit->faceDirection + x;
    data->y = 40.0f * unit->mSizeMultiplier + y;
    data->z = z + 5.0f;
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(eff_staff2) {
    s32* args = event->args;
    s32 id = BattleTransID(event, evtGetValue(event, args[0]));
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    EffectEntry* eff;
    effPokopiPchargeData* data;
    f32 x, y, z;

    if (isFirstCall) {
        unit->work[4] = (s32)effPokopiPchargeN64Entry(0.0f, -1000.0f, 0.0f, 1.0f, 0, 0x3C);
    }
    eff = (EffectEntry*)unit->work[4];
    if ((eff->flags & 1) == 0) {
        return EVT_RETURN_DONE;
    }
    data = eff->userdata;
    data->scale = unit->mSizeMultiplier;
    BtlUnit_GetPos(unit, &x, &y, &z);
    data->x = 22.5f * unit->mSizeMultiplier * unit->faceDirection + x;
    data->y = 40.0f * unit->mSizeMultiplier + y;
    data->z = z + 5.0f;
    return EVT_RETURN_BLOCK;
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
    effMagic1Data* data;
    f32 x, y, z;

    BtlUnit_GetPos(unit, &x, &y, &z);
    x = 25.0f * unit->mSizeMultiplier * unit->faceDirection + x;
    y = 20.0f * unit->mSizeMultiplier + y;
    z += 5.0f;
    eff = effMagic1N64Entry(0, type, x, y, z, a, b, c);
    unit->work[5] = (s32)eff;
    data = eff->userdata;
    data->scale = unit->mSizeMultiplier;
    return EVT_RETURN_DONE;
}

static USER_FUNC(eff_magic_get_posX) {
    s32* args = event->args;
    s32 id = BattleTransID(event, evtGetValue(event, args[0]));
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);
    EffectEntry* eff = (EffectEntry*)unit->work[5];
    effMagic1Data* data = eff->userdata;

    evtSetFloat(event, args[1], data->offsetX);
    return EVT_RETURN_DONE;
}

// clang-format on

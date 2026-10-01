#include "battle/battle_camera.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle_database_common.h"
#include "manager/evtmgr_cmd.h"
#include "driver/mapdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "eff/eff_syuryou.h"
#include "memory.h"
#include "system.h"

typedef struct BarriernZanzouScale {
    f32 value[5];
} BarriernZanzouScale;

typedef struct BarriernSatelliteTo {
    s32 type;         // 0x00
    s32 unitId;       // 0x04
    s32 points[5];    // 0x08
    s32 field_1C[15]; // 0x1C
    s32 field_58;     // 0x58
} BarriernSatelliteTo;

typedef struct BarriernSatelliteToData {
    s32 field_0; // 0x00
    f32 x;       // 0x04
    f32 y;       // 0x08
    f32 z;       // 0x0C
    f32 scale;   // 0x10
} BarriernSatelliteToData;

#pragma pool_data off // match the original: float literals are emitted as separate constants

#pragma force_active on // keep static event/standalone helpers (Init/Main) emitted like the original
extern BattleUnitKind UNIT_REF_BARRIERN_SATELLITE;
static USER_FUNC(battleGetBarriernSatelliteInitPos);
static USER_FUNC(_check_satellite);
static void barriern_wawawa_get_next_target(s32* out1, s32* out2);
static void barriern_wawawa_get_target(s32* out1, s32* out2);
static USER_FUNC(barriern_wawawa_set_target);
static void outSplineTableSet_Sub(BattleUnit* unit, s32 index, void* spline);
static USER_FUNC(outSplineTableSet);
static USER_FUNC(barriernSatelliteToOutside);
static USER_FUNC(barriernSatelliteToInside);
static u8 barriernSatelliteTo_Main(BattleUnit* unit, s32 index);
static u8 barriernSatelliteTo_Init(BattleUnit* unit, s32 index, s32 id);
static s32 barriern_satellite_zanzou_sub(BOOL isFirstCall, BattleUnit* sat, BarriernZanzouScale scale,
                                         s32 index, s32 mode);
static USER_FUNC(barriern_satellite_zanzou2);
static USER_FUNC(barriern_satellite_zanzou);
static s32 barriern_zanzou_sub(BOOL isFirstCall, BattleUnit* unit, BarriernZanzouScale scale, s32 mode);
static USER_FUNC(barriern_zanzou3);
static USER_FUNC(barriern_zanzou2);
static USER_FUNC(barriern_zanzou);
static BattleUnitKindPart parts[11];
static BattleVulnerableStatus regist;
static BattleDispPose pose_table[18];
static BattleDataEntry data_table[2];
static s32 init_event[143];
static s32 damage_event[17];
static s32 barriern_dmg_first_attack_pos[19];
static s32 unison_phase_event[190];
static s32 attack_event[106];
static s32 recovery_event[69];
static s32 wakka_attack_event[255];
static s32 wawawa_attack_event[548];
static s32 wawawa_attack_event_sub[87];
static s32 wawawa_attack_event_sub_no_last[10];
static s32 wawawa_attack_event_sub_last[93];
static s32 wait_event[116];
static s32 zanzou_count[3];

// clang-format off
extend_data(unit_barriern_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_barriern_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_YUX,                   // mUnitType
    "btl_un_barriern",          // name
    3,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    12,                         // mLevel
    0,                          // mBonusExp
    1,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    1,                          // baseCoin
    60,                         // field_0x13
    9999,                       // field_0x14
    40,                         // width
    40,                         // height
    { 0, 20 },                  // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { 0, -40 },                 // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 20.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 20.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 10.0f, 10.0f, 0.0f },     // kissHitOffset
    { 0.0f, 0.0f, 0.0f },       // cutBaseOffset
    40.0f,                      // cutWidth
    40.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_BARRI_DAMAGED2",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00000004,                 // attributeFlags
    &regist,                    // vulnStatus
    11,                         // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleUnitKindPart parts[11] = {
    {
        1,                     // partNum
        "btl_un_barriern",     // partName
        "c_baria",             // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 12.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 22.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00600009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        2,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
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
    {
        3,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
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
    {
        4,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
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
    {
        5,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
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
    {
        6,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
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
    {
        7,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        10,                   // field_0x30
        10,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
    {
        8,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        10,                   // field_0x30
        10,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
    {
        9,                    // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        10,                   // field_0x30
        10,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
    {
        10,                   // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        10,                   // field_0x30
        10,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
    {
        11,                   // partNum
        "btl_un_barriern",    // partName
        "c_baria",            // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        10,                   // field_0x30
        10,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
};

static BattleVulnerableStatus regist = {
    30,  // sleepRate
    80,  // stopRate
    100, // dizzyRate
    0,   // poisonRate
    70,  // confuseRate
    100, // electricRate
    0,   // burnRate
    0,   // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    70,  // allergicRate
    0,   // frightRate
    90,  // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleWeapon weapon_wakka_attack = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00002004,                               // targetWeightingFlags
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

static BattleWeapon weapon_wawawa_attack = {
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

static BattleUnitSetup entry_barriern_satellite[1] = {
    { &UNIT_REF_BARRIERN_SATELLITE, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static BattleDispPose pose_table[18] = {
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
    { 0x29, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x30, barriern_dmg_first_attack_pos },
    { 0x00, NULL },
};

static const BarriernZanzouScale zanzou_scale = { { 1.7f, 2.4f, 3.1f, 3.8f, 4.5f } };
static const BarriernZanzouScale zanzou2_scale = { { 1.7f, 2.4f, 3.1f, 3.8f, 4.5f } };
static const BarriernZanzouScale zanzou3_scale = { { 1.7f, 2.4f, 3.1f, 3.8f, 4.5f } };
static const BarriernZanzouScale satellite_zanzou_scale = { { 1.7f, 2.4f, 3.1f, 3.8f, 4.5f } };
static const BarriernZanzouScale satellite_zanzou2_scale = { { 1.7f, 2.4f, 3.1f, 3.8f, 4.5f } };

// TODO: type
static u32 splineTableIn1_1[15] = {
    0x41C80000, 0x00000000, 0x00000000, 0x41A00000,
    0xC1F00000, 0xBF800000, 0x41480000, 0xC2200000,
    0xBF800000, 0x40A00000, 0xC1F00000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn2_1[15] = {
    0x41C80000, 0x00000000, 0x00000000, 0x41A00000,
    0xC1F00000, 0xBF800000, 0x41480000, 0xC2200000,
    0xBF800000, 0x40A00000, 0xC1F00000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn2_2[15] = {
    0xC1C80000, 0x00000000, 0x00000000, 0xC1A00000,
    0x41F00000, 0xBF800000, 0xC1480000, 0x41F00000,
    0xBF800000, 0xC0A00000, 0x41F00000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn3_1[15] = {
    0x00000000, 0x41C80000, 0x00000000, 0x00000000,
    0x42340000, 0xBF800000, 0x00000000, 0x42480000,
    0xBF800000, 0x00000000, 0x41C80000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn3_2[15] = {
    0x41C80000, 0xC1C80000, 0x00000000, 0x42200000,
    0xC2200000, 0xBF800000, 0x41F00000, 0xC20C0000,
    0xBF800000, 0x41A00000, 0xC1C80000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn3_3[15] = {
    0xC1C80000, 0xC1C80000, 0x00000000, 0xC2200000,
    0xC2200000, 0xBF800000, 0xC1F00000, 0xC20C0000,
    0xBF800000, 0xC1A00000, 0xC1C80000, 0xBF800000,
    0x80000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn4_1[15] = {
    0x41C80000, 0x41C80000, 0x00000000, 0x41F00000,
    0x420C0000, 0xBF800000, 0x42200000, 0x42340000,
    0xBF800000, 0x41A00000, 0x41C80000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn4_2[15] = {
    0xC1C80000, 0x41C80000, 0x00000000, 0xC1F00000,
    0x420C0000, 0xBF800000, 0xC2200000, 0x42340000,
    0xBF800000, 0xC1A00000, 0x41C80000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn4_3[15] = {
    0x41C80000, 0xC1C80000, 0x00000000, 0x41F00000,
    0xC20C0000, 0xBF800000, 0x42200000, 0xC2340000,
    0xBF800000, 0x41A00000, 0xC1C80000, 0xBF800000,
    0x00000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableIn4_4[15] = {
    0xC1C80000, 0xC1C80000, 0x00000000, 0xC1F00000,
    0xC20C0000, 0xBF800000, 0xC2200000, 0xC2340000,
    0xBF800000, 0xC1A00000, 0xC1C80000, 0xBF800000,
    0x80000000, 0x00000000, 0xBF800000,
};

// TODO: type
static u32 splineTableOut1_1[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0xC20C0000,
    0xC20C0000, 0xBF800000, 0xC2200000, 0x00000000,
    0xBF800000, 0x41F00000, 0x42200000, 0xBF800000,
    0x41C80000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 splineTableOut2_1[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0x40A00000,
    0x41F00000, 0xBF800000, 0x41480000, 0x42200000,
    0xBF800000, 0x41A00000, 0x41F00000, 0xBF800000,
    0x41C80000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 splineTableOut2_2[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0xC0A00000,
    0xC1F00000, 0xBF800000, 0xC1480000, 0xC2200000,
    0xBF800000, 0xC1A00000, 0xC1F00000, 0xBF800000,
    0xC1C80000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 splineTableOut3_1[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0x00000000,
    0x41C80000, 0xBF800000, 0x00000000, 0x42480000,
    0xBF800000, 0x00000000, 0x42340000, 0xBF800000,
    0x00000000, 0x41C80000, 0x00000000,
};

// TODO: type
static u32 splineTableOut3_2[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0x41C80000,
    0xC1A00000, 0xBF800000, 0x42340000, 0xC1F00000,
    0xBF800000, 0x41F00000, 0xC1C80000, 0xBF800000,
    0x41C80000, 0xC1C80000, 0x00000000,
};

// TODO: type
static u32 splineTableOut3_3[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0xC1C80000,
    0xC1A00000, 0xBF800000, 0xC2340000, 0xC1F00000,
    0xBF800000, 0xC1F00000, 0xC1C80000, 0xBF800000,
    0xC1C80000, 0xC1C80000, 0x00000000,
};

// TODO: type
static u32 splineTableOut4_1[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0x41C80000,
    0x41A00000, 0xBF800000, 0x42340000, 0x41F00000,
    0xBF800000, 0x41F00000, 0x41C80000, 0xBF800000,
    0x41C80000, 0x41C80000, 0x00000000,
};

// TODO: type
static u32 splineTableOut4_2[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0xC1C80000,
    0x41A00000, 0xBF800000, 0xC2340000, 0x41F00000,
    0xBF800000, 0xC1F00000, 0x41C80000, 0xBF800000,
    0xC1C80000, 0x41C80000, 0x00000000,
};

// TODO: type
static u32 splineTableOut4_3[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0x41C80000,
    0xC1A00000, 0xBF800000, 0x42340000, 0xC1F00000,
    0xBF800000, 0x41F00000, 0xC1C80000, 0xBF800000,
    0x41C80000, 0xC1C80000, 0x00000000,
};

// TODO: type
static u32 splineTableOut4_4[15] = {
    0x00000000, 0x00000000, 0xBF800000, 0xC1C80000,
    0xC1A00000, 0xBF800000, 0xC2340000, 0xC1F00000,
    0xBF800000, 0xC1F00000, 0xC1C80000, 0xBF800000,
    0xC1C80000, 0xC1C80000, 0x00000000,
};

static USER_FUNC(barriern_zanzou) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    barriern_zanzou_sub(isFirstCall, unit, zanzou_scale, 0);
}

static USER_FUNC(barriern_zanzou2) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    barriern_zanzou_sub(isFirstCall, unit, zanzou2_scale, 1);
}

static USER_FUNC(barriern_zanzou3) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    barriern_zanzou_sub(isFirstCall, unit, zanzou3_scale, 2);
}

// NONMATCHING (98.57%): the always-body's first intplGetValue(4, zanzou_count[0], ...) puts the
// zanzou_count address in a scratch register and materialises the 0.8f address into a full
// register, where the original keeps the count address in r4 and folds the 0.8f @l into the
// load. The second loop's scheduling is fixed by the volatile-comma below in MUST_MATCH builds.
static s32 barriern_zanzou_sub(BOOL isFirstCall, BattleUnit* unit, BarriernZanzouScale scale, s32 mode) {
    BattleUnitPart* part;
    f32 temp1[5];
    f32 temp2[5];
    f32 x;
    f32 y;
    f32 z;
    f32 base;
    f32 step;
    f32 s;
    s32 num;
    u8 alpha1;
    u8 alpha2;
    s32 a = 4;
    s32 i;
    s32 j;

    switch (mode) {
    case 0:
        num = 0x24;
        base = 0.6f;
        for (i = 0; i < 5; i++) {
            temp1[i] = 0.8f * scale.value[i];
            temp2[i] = 0.6f;
        }
        step = 0.5f;
        alpha1 = 0;
        alpha2 = 0x40;
        a = 4;
        break;
    case 1:
    case 2:
        num = 8;
        base = 0.8f;
        for (i = 0; i < 5; i++) {
            temp1[i] = 0.8f;
            temp2[i] = 0.8f * scale.value[i];
        }
        step = -0.5f;
        alpha1 = 0x40;
        alpha2 = 0;
        a = 1;
        break;
    }
    if (mode == 2) {
        num = 0x24;
    }
    if (isFirstCall) {
        zanzou_count[0] = 0;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetPartsScale(BtlUnit_GetPartsPtr(unit, 1), 0.8f, 0.8f, 1.0f);
        for (i = 2, j = 0; i <= 6; i++, j++) {
            part = BtlUnit_GetPartsPtr(unit, i);
            BtlUnit_SetPartsPos(part, x, y, z + (f32)i * step);
            BtlUnit_SetPartsScale(part, temp1[j], temp1[j], 1.0f);
            part->color.a = alpha2;
            part->attributes &= ~0x01000000;
        }
    }
    part = BtlUnit_GetPartsPtr(unit, 1);
    s = intplGetValue(a, zanzou_count[0], num, 0.8f, base);
    BtlUnit_SetPartsScale(part, s, s, 1.0f);
    for (i = 2, j = 0; i <= 6; i++, j++) {
        part = BtlUnit_GetPartsPtr(unit, i);
#ifdef MUST_MATCH
        // volatile-comma forces both temp float loads before the count load, as in the original.
        s = intplGetValue(a, ((volatile f32)temp1[j], (volatile f32)temp2[j], zanzou_count[0]), num,
                          temp1[j], temp2[j]);
#else
        s = intplGetValue(a, zanzou_count[0], num, temp1[j], temp2[j]);
#endif
        BtlUnit_SetPartsScale(part, s, s, 1.0f);
        part->color.a = (u8)intplGetValue(0, zanzou_count[0], num, (f32)alpha1, (f32)alpha2);
    }
    if (zanzou_count[0] >= num) {
        part = BtlUnit_GetPartsPtr(unit, 1);
        BtlUnit_SetPartsScale(part, 0.8f, 0.8f, 1.0f);
        part->color.a = 0xff;
        for (i = 2; i <= 6; i++) {
            BtlUnit_GetPartsPtr(unit, i)->attributes |= 0x01000000;
        }
        return 2;
    }
    zanzou_count[0]++;
    return 0;
}

static USER_FUNC(barriern_satellite_zanzou) {
    s32 index;
    BattleUnit* unit;
#ifdef MUST_MATCH
    BattleUnit* p;
#endif
    s32* args;
    s32 id;
    BattleWork* wp;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    index = evtGetValue(event, args[1]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, id);
#ifdef MUST_MATCH
    return barriern_satellite_zanzou_sub(isFirstCall, (p = BattleGetUnitPtr(wp, unit->work[4 + index]), p),
                                         satellite_zanzou_scale, index, 0);
#else
    return barriern_satellite_zanzou_sub(isFirstCall, BattleGetUnitPtr(wp, unit->work[4 + index]),
                                         satellite_zanzou_scale, index, 0);
#endif
}

static USER_FUNC(barriern_satellite_zanzou2) {
    s32 index;
    BattleUnit* unit;
#ifdef MUST_MATCH
    BattleUnit* p;
#endif
    s32* args;
    s32 id;
    BattleWork* wp;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    index = evtGetValue(event, args[1]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, id);
#ifdef MUST_MATCH
    return barriern_satellite_zanzou_sub(isFirstCall, (p = BattleGetUnitPtr(wp, unit->work[4 + index]), p),
                                         satellite_zanzou2_scale, index, 1);
#else
    return barriern_satellite_zanzou_sub(isFirstCall, BattleGetUnitPtr(wp, unit->work[4 + index]),
                                         satellite_zanzou2_scale, index, 1);
#endif
}

// NONMATCHING (98.65%): in the always-body the compiler materialises the 0.8f address into a full
// register and puts the zanzou_count address in a scratch register in the first intplGetValue, where
// the original keeps the count address in r4 and folds the 0.8f @l into the load. The second loop's
// scheduling is fixed by the volatile-comma below in MUST_MATCH builds.
static s32 barriern_satellite_zanzou_sub(BOOL isFirstCall, BattleUnit* unit, BarriernZanzouScale scale,
                                         s32 index, s32 mode) {
    BattleUnitPart* part;
    f32 temp1[5];
    f32 temp2[5];
    f32 x;
    f32 y;
    f32 z;
    f32 base;
    f32 step;
    f32 s;
    s32 num;
    u8 alpha1;
    u8 alpha2;
    s32 a = 4;
    s32 i;
    s32 j;

    switch (mode) {
    case 0:
        num = 0x24;
        base = 0.6f;
        for (i = 0; i < 5; i++) {
            temp1[i] = 0.8f * scale.value[i];
            temp2[i] = 0.6f;
        }
        step = 0.5f;
        alpha1 = 0;
        alpha2 = 0x40;
        a = 4;
        break;
    case 1:
        num = 8;
        base = 0.8f;
        for (i = 0; i < 5; i++) {
            temp1[i] = 0.8f;
            temp2[i] = 0.8f * scale.value[i];
        }
        step = -0.5f;
        alpha1 = 0x40;
        alpha2 = 0;
        a = 1;
        break;
    }
    index = index + 1;
    if (isFirstCall) {
        zanzou_count[index] = 0;
        BtlUnit_GetPos(unit, &x, &y, &z);
        BtlUnit_SetPartsScale(BtlUnit_GetPartsPtr(unit, 1), 0.8f, 0.8f, 1.0f);
        for (i = 2, j = 0; i <= 6; i++, j++) {
            part = BtlUnit_GetPartsPtr(unit, i);
            BtlUnit_SetPartsPos(part, x, y, z + (f32)i * step);
            BtlUnit_SetPartsScale(part, temp1[j], temp1[j], 1.0f);
            BtlUnit_SetPartsRotate(part, 0.0f, 0.0f, 0.0f);
            part->color.a = alpha1;
            BtlUnit_SetAnim(part, "Z_2");
            part->attributes &= ~0x01000000;
        }
    }
    part = BtlUnit_GetPartsPtr(unit, 1);
    s = intplGetValue(4, zanzou_count[0], num, 0.8f, base);
    BtlUnit_SetPartsScale(part, s, s, 1.0f);
    for (i = 2, j = 0; i <= 6; i++, j++) {
        part = BtlUnit_GetPartsPtr(unit, i);
#ifdef MUST_MATCH
        // volatile-comma forces both temp float loads before the count load, as in the original.
        s = intplGetValue(a, ((volatile f32)temp1[j], (volatile f32)temp2[j], zanzou_count[index]), num,
                          temp1[j], temp2[j]);
#else
        s = intplGetValue(a, zanzou_count[index], num, temp1[j], temp2[j]);
#endif
        BtlUnit_SetPartsScale(part, s, s, 1.0f);
        part->color.a = (u8)intplGetValue(0, zanzou_count[index], num, (f32)alpha1, (f32)alpha2);
    }
    if (zanzou_count[index] >= num) {
        part = BtlUnit_GetPartsPtr(unit, 1);
        BtlUnit_SetPartsScale(part, 0.8f, 0.8f, 1.0f);
        part->color.a = 0xff;
        for (i = 2; i <= 6; i++) {
            BtlUnit_GetPartsPtr(unit, i)->attributes |= 0x01000000;
        }
        return 2;
    }
    zanzou_count[index]++;
    return 0;
}

// Separate inline copy of barriernSatelliteTo_Init used by Inside/Outside. Storing unitId before
// type changes the inliner's register allocation to match the original; the standalone Init keeps
// its own order (see its NONMATCHING note). Natural build calls the standalone directly.
#ifdef MUST_MATCH
static inline u8 barriernSatelliteTo_InitInline(BattleUnit* unit, s32 index, s32 id) {
    BarriernSatelliteTo* work;

    if (unit->work[4 + index] == -1) {
        return 0;
    }
    work = __memAlloc(4, 0x5c);
    unit->work[10 + index] = (s32)work;
    work->unitId = id;
    work->type = 5;
    work->field_58 = 0;
    spline_maketable(5, work->unitId, work->points, work->field_1C);
    BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]), 1)->work[2] = id;
    return 1;
}
#else
#define barriernSatelliteTo_InitInline(unit, index, id) barriernSatelliteTo_Init(unit, index, id)
#endif
static u8 barriernSatelliteTo_Init(BattleUnit* unit, s32 index, s32 id) {
    BarriernSatelliteTo* work;

    if (unit->work[4 + index] == -1) {
        return 0;
    }
    work = __memAlloc(4, 0x5c);
    unit->work[10 + index] = (s32)work;
    work->type = 5;
    work->unitId = id;
    work->field_58 = 0;
    spline_maketable(5, work->unitId, work->points, work->field_1C);
    BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]), 1)->work[2] = id;
    return 1;
}

static u8 barriernSatelliteTo_Main(BattleUnit* unit, s32 index) {
    BarriernSatelliteTo* work;
    BattleUnit* satellite;
    f32 pos[3];
    f32 out[3];
    f32 t;

    if (unit->work[4 + index] == -1) {
        return 0;
    }
    work = (BarriernSatelliteTo*)unit->work[10 + index];
    if (work == NULL) {
        return 0;
    }
    if (work->field_58 >= 0x3c) {
        if (work != NULL) {
            __memFree(4, work);
            unit->work[10 + index] = 0;
        }
        return 0;
    }
    t = intplGetValue(4, work->field_58, 0x3c, 0.0f, 1.0f);
    spline_getvalue(out, work->type, work->unitId, (s32)work->points, (s32)work->field_1C, t);
    satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]);
    if (satellite == NULL) {
        return 0;
    }
    BtlUnit_GetPos(unit, &pos[0], &pos[1], &pos[2]);
    BtlUnit_SetPos(satellite, pos[0] + out[0] * unit->faceDirection, pos[1] + out[1], pos[2] + out[2]);
    BtlUnit_SetHomePos(satellite, pos[0] + out[0] * unit->faceDirection, pos[1] + out[1], pos[2] + out[2]);
    work->field_58++;
    return 1;
}

// NONMATCHING (89.36%): the inlined barriernSatelliteTo_InitInline (see above) now gets `work` in r7
// like the original, but still keeps the id in the caller's cached table register for `work->unitId`
// where the original rematerialises it (`lis/addi` into r4), permuting registers through the function.
// `found` is a `u8`, which matches the original's `clrlwi` on the return value, but the compiler still
// chooses `subfic` where the original used `subic` for the found->0/2 conversion.
static USER_FUNC(barriernSatelliteToInside) {
    s32* args;
    s32 id;
    s32 type;
    BattleUnit* unit;
    s32 i;
    u8 found;

    args = event->args;
    id = evtGetValue(event, args[0]);
    type = evtGetValue(event, args[1]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        for (i = 0; i < 4; i++) {
            s32 table;

            if (unit->work[4 + i] == -1) {
                continue;
            }
            table = BtlUnit_GetPartsPtr(BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + i]), 1)->work[2];
            if (table == (s32)splineTableOut1_1) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn1_1);
            }
            if (table == (s32)splineTableOut2_1) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn2_1);
            }
            if (table == (s32)splineTableOut2_2) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn2_2);
            }
            if (table == (s32)splineTableOut3_1) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn3_1);
            }
            if (table == (s32)splineTableOut3_2) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn3_2);
            }
            if (table == (s32)splineTableOut3_3) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn3_3);
            }
            if (table == (s32)splineTableOut4_1) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn4_1);
            }
            if (table == (s32)splineTableOut4_2) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn4_2);
            }
            if (table == (s32)splineTableOut4_3) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn4_3);
            }
            if (table == (s32)splineTableOut4_4) {
                barriernSatelliteTo_InitInline(unit, i, (s32)splineTableIn4_4);
            }
        }
    }
    found = 0;
    for (i = 0; i < 4; i++) {
        if (barriernSatelliteTo_Main(unit, i) == 1) {
            found = 1;
        }
    }
    return found ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// NONMATCHING (93.02%): same inlined barriernSatelliteTo_InitInline rematerialisation as
// barriernSatelliteToInside; the original materialises the table address for `work->unitId = id`
// instead of reusing the caller's cached register. `found` is a `u8` (matching the original's
// `clrlwi`), with the same `subic`/`subfic` peephole difference on the return value.
static USER_FUNC(barriernSatelliteToOutside) {
    s32* args;
    s32 id;
    s32 type;
    BattleUnit* unit;
    s32 i;
    u8 found;

    args = event->args;
    id = evtGetValue(event, args[0]);
    type = evtGetValue(event, args[1]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    if (isFirstCall) {
        switch (type) {
        case 1:
            for (i = 0;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut1_1) == 1) {
                    break;
                }
            }
            break;
        case 2:
            for (i = 0;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut2_1) == 1) {
                    break;
                }
            }
            i++;
            for (;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut2_2) == 1) {
                    break;
                }
            }
            break;
        case 3:
            for (i = 0;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut3_1) == 1) {
                    break;
                }
            }
            i++;
            for (;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut3_2) == 1) {
                    break;
                }
            }
            i++;
            for (;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut3_3) == 1) {
                    break;
                }
            }
            break;
        case 4:
            for (i = 0;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut4_1) == 1) {
                    break;
                }
            }
            i++;
            for (;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut4_2) == 1) {
                    break;
                }
            }
            i++;
            for (;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut4_3) == 1) {
                    break;
                }
            }
            i++;
            for (;; i++) {
                if (barriernSatelliteTo_InitInline(unit, i, (s32)splineTableOut4_4) == 1) {
                    break;
                }
            }
            break;
        default:
            return EVT_RETURN_DONE;
        }
    }
    found = 0;
    for (i = 0; i < 4; i++) {
        if (barriernSatelliteTo_Main(unit, i) == 1) {
            found = 1;
        }
    }
    return found ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

static USER_FUNC(outSplineTableSet) {
    s32* args;
    BattleUnit* unit;
    BattleUnit* satellite;
    BattleUnitPart* part;
    s32 type;
    s32 id;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    type = evtGetValue(event, args[1]);
    switch (type) {
    case 1:
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut1_1;
        break;
    case 2:
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut2_1;
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[5]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut2_2;
        break;
    case 3:
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut3_1;
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[5]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut3_2;
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[6]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut3_3;
        break;
    case 4:
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut4_1;
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[5]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut4_2;
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[6]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut4_3;
        satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[7]);
        part = BtlUnit_GetPartsPtr(satellite, 1);
        part->work[2] = (s32)splineTableOut4_4;
        break;
    }
    return EVT_RETURN_DONE;
}

static void outSplineTableSet_Sub(BattleUnit* unit, s32 index, void* spline) {
    BattleUnit* satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[4 + index]);

    BtlUnit_GetPartsPtr(satellite, 1)->work[2] = (s32)spline;
}

// The args increment is written before the second call so the scheduler emits the addi there,
// as the original does; args[-1] is the pre-increment args[1].
static USER_FUNC(barriern_wawawa_set_target) {
    s32* args;
    s32 xs[12];
    s32 ys[10];
    s32 i;
    s32 count;
    s32 n;

    args = event->args;
    i = 0;
    _battleWorkPointer->field_428[0xab7] = 0;
    barriern_wawawa_get_target(&xs[0], &ys[0]);
    for (;;) {
        if (xs[i] == -1) {
            count = i;
            break;
        }
        i++;
        barriern_wawawa_get_next_target(&xs[i], &ys[i]);
    }
    i = 0;
    while (i < 5) {
        n = irand(count);

        evtSetValue(event, args[0], xs[n]);
#ifdef MUST_MATCH
        args += 2;
        evtSetValue(event, args[-1], ys[n]);
#else
        evtSetValue(event, args[1], ys[n]);
        args += 2;
#endif
        i++;
    }
    return EVT_RETURN_DONE;
}

static void barriern_wawawa_get_target(s32* out1, s32* out2) {
    u8* sampling = _battleWorkPointer->field_428;
    u8* entry = sampling + (s8)sampling[0xa6d + (s8)sampling[0xab7]] * 0x24 + 4;

    *out1 = *(s16*)(entry + 0);
    *out2 = *(s16*)(entry + 2);
}

static void barriern_wawawa_get_next_target(s32* out1, s32* out2) {
    u8* sampling = _battleWorkPointer->field_428;
    s32 idx;

    sampling[0xab7]++;
    idx = (s8)sampling[0xab7];
    if (idx < (s8)sampling[0xa6c]) {
        u8* entry = sampling + (s8)sampling[0xa6d + idx] * 0x24 + 4;
        *out1 = *(s16*)(entry + 0);
        *out2 = *(s16*)(entry + 2);
    } else {
        *out1 = -1;
        *out2 = 0;
    }
}

static USER_FUNC(_check_satellite) {
    BattleUnit* unit;
    s32* args;
    BattleUnit* satellite;
    BattleUnitPart* part;
    s32 id;
    s32 i;
    s32 hasSat;
    f32 x;
    f32 y;
    f32 z;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    args = event->args;
    hasSat = 0;
    if (unit == NULL) {
        return EVT_RETURN_DONE;
    }
    for (i = 4; i < 6; i++) {
        if (unit->work[i] != -1) {
            satellite = BattleGetUnitPtr(_battleWorkPointer, unit->work[i]);
            if (satellite != NULL) {
                hasSat = 1;
            } else {
                unit->work[i] = -1;
            }
        }
    }
    if (hasSat == 0) {
        s32 j;
        s32 w;
        s32 h;
        s32 dx;
        s32 dy;

        part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        if (part->attributes & 0x20000000) {
            effDelete((EffectEntry*)unit->work[3]);
            unit->work[3] = 0;
            BtlUnit_snd_se(unit, "SFX_ENM_BARRI_BARRIER_BREAK1", -250000000, 0);
            BtlUnit_GetPos(unit, &x, &y, &z);
            for (j = 0; j < 20; j++) {
                w = BtlUnit_GetWidth(unit) + 0xf;
                h = BtlUnit_GetHeight(unit) + 0xf;
                dx = irand(w) - w / 2;
                dy = irand(h) - h / 2;
                effGlassN64Entry(2, 0x3c, x + dx, y + dy, z, 1.0f);
            }
        }
        part->attributes &= ~0x20000000;
        evtSetValue(event, args[0], 0);
    } else {
        f32 ex;
        f32 ey;
        f32 ez;

        part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        if (!(part->attributes & 0x20000000)) {
            BtlUnit_snd_se(unit, "SFX_ENM_BARRI_BARRIER1", -250000000, 0);
            BtlUnit_GetPos(unit, &ex, &ey, &ez);
            unit->work[3] = (s32)effSyuryouEntry(1, 0x3e8, 0.0f, -1000.0f, 0.0f);
        }
        part->attributes |= 0x20000000;
        evtSetValue(event, args[0], 1);
        if (unit->work[3] != 0) {
            BarriernSatelliteToData* data;

            data = (BarriernSatelliteToData*)((EffectEntry*)unit->work[3])->userdata;
            BtlUnit_GetPos(unit, &x, &y, &z);
            data->x = x;
            data->y = y - 32.0f * unit->mSizeMultiplier * 0.8f;
            data->z = z + 10.0f;
            data->scale = 0.8f * unit->mSizeMultiplier * 1.8f;
        }
    }
    return EVT_RETURN_DONE;
}




static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventUnisonPhase, -2, PTR(unison_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetPartsScale, -2, 1, FLOAT(0.8f), FLOAT(0.8f), FLOAT(1.0f))
    CALL(btlevtcmd_OnUnitFlag, -2, 3145728)
    SET_VALUE(LW(0), 4)
    LOOP(2)
        CALL(btlevtcmd_SetUnitWork, -2, LW(0), -1)
        ADD(LW(0), 1)
    END_LOOP
    SET_VALUE(LW(6), 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(7))
    LABEL(0)
    IF_INT_LESS(LW(6), LW(7))
        CALL(btlevtcmd_GetUnitId, -2, LW(5))
        SET_VALUE(LW(8), LW(6))
        ADD(LW(8), 4)
        CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_barriern_satellite), 0)
        CALL(btlevtcmd_SetUnitWork, -2, LW(8), LW(3))
        CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(5))
        CALL(btlevtcmd_SetUnitWork, LW(3), 1, 1)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(battleGetBarriernSatelliteInitPos, -2, LW(7), LW(6), LW(10), LW(11), LW(12))
        ADD(LW(0), LW(10))
        ADD(LW(1), LW(11))
        ADD(LW(2), LW(12))
        CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetScale, LW(3), FLOAT(0.8f), FLOAT(0.8f), FLOAT(1.0f))
        ADD(LW(6), 1)
        GOTO(0)
    END_IF
    LABEL(10)
    CALL(outSplineTableSet, -2, LW(7))
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_SetRGB, -2, 1, 255, 255, 255)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(barriern_dmg_first_attack_pos)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 20)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    RETURN
EVT_END()

static EVT_BEGIN(satellite_blur_on)
    SET_VALUE(LW(2), 4)
    LOOP(2)
        CALL(btlevtcmd_GetUnitWork, -2, LW(2), LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_SetPartsBlur, LW(3), LW(4), 255, 255, 255, 255, 255, 255, 255, 100, 0)
            CALL(btlevtcmd_OnPartsAttribute, LW(3), LW(4), 67108864)
        END_IF
        ADD(LW(2), 1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(satellite_blur_off)
    SET_VALUE(LW(2), 4)
    LOOP(2)
        CALL(btlevtcmd_GetUnitWork, -2, LW(2), LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_OffPartsAttribute, LW(3), LW(4), 67108864)
        END_IF
        ADD(LW(2), 1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(unison_phase_event)
    CALL(btlevtcmd_StopWaitEvent, -2)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(evt_sub_random, 10, LW(0))
    WAIT_FRAMES(LW(0))
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 4)
    LOOP(2)
        CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            ADD(LW(0), 1)
        END_IF
        ADD(LW(1), 1)
    END_LOOP
    IF_INT_GREATER_OR_EQUAL(LW(0), 2)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK9"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(barriern_zanzou3, -2)
    CALL_CHILD(satellite_blur_on)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
    IF_INT_GREATER_OR_EQUAL(LW(1), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_SPLIT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    CALL(barriernSatelliteToInside, -2, LW(1))
    CALL_CHILD(satellite_blur_off)
    CALL(btlevtcmd_GetUnitId, -2, LW(4))
    SET_VALUE(LW(1), 4)
    SET_VALUE(LW(5), 0)
    LOOP(2)
        CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(2))
        IF_INT_EQUAL(LW(2), -1)
            CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_barriern_satellite), 0)
            CALL(btlevtcmd_AddPos, LW(3), 0, 0, -1)
            CALL(btlevtcmd_SetUnitWork, -2, LW(1), LW(3))
            CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
            ADD(LW(0), 1)
            ADD(LW(5), 1)
            IF_INT_GREATER_OR_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        ADD(LW(1), 1)
    END_LOOP
    WAIT_FRAMES(5)
    CALL_CHILD(satellite_blur_on)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_SPLIT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(barriernSatelliteToOutside, -2, LW(0))
    WAIT_FRAMES(60)
    CALL_CHILD(satellite_blur_off)
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_SetRGB, -2, 1, 255, 255, 255)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(wakka_attack_event)
        RETURN
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(_check_satellite)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 4)
    LOOP(2)
        CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_CheckActStatus, LW(3), LW(2))
            IF_INT_NOT_EQUAL(LW(2), 0)
                CALL(btlevtcmd_CheckStatus, LW(3), 5, LW(2))
                IF_INT_EQUAL(LW(2), 0)
                    ADD(LW(0), 1)
                END_IF
            END_IF
        END_IF
        ADD(LW(1), 1)
    END_LOOP
    IF_INT_EQUAL(LW(0), 2)
        CALL_CHILD(wawawa_attack_event)
    ELSE
        SET_VALUE(LW(0), 100)
        ADD(LW(0), 0)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 100)
            CALL_CHILD(wakka_attack_event)
        ELSE
            CALL_CHILD(recovery_event)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(recovery_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_3"))
    WAIT_FRAMES(120)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_RecoverHp, -2, 1, 0)
    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
    ADD(LW(6), 30)
    ADD(LW(7), 10)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(5), LW(6), LW(7), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(30)
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

static EVT_BEGIN(wakka_attack_common_event_no_hit)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(6))
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 67108864)
    ELSE
        CALL(btlevtcmd_SetPartsBlur, -2, LW(15), 255, 255, 255, 255, 255, 255, 255, 100, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 67108864)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("Z_3"))
    CALL(btlevtcmd_GetPos, LW(12), LW(6), LW(7), LW(8))
    ADD(LW(8), 1)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_CheckPartsAttribute, -2, LW(15), 16777216, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5"))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            ELSE
                CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(6))
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
        CALL(btlevtcmd_GetStageSize, LW(6), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        MULTIPLY(LW(6), 1)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
        MULTIPLY(LW(6), LW(0))
        SUBTRACT(LW(7), 5)
        CALL(btlevtcmd_SetPartsScale, -2, LW(15), 1, 0, 1)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                LOOP(10)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(0.125f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                LOOP(10)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(-0.125f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, 4, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 0, -1)
    ELSE
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
        CALL(btlevtcmd_GetStageSize, LW(6), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        MULTIPLY(LW(6), 1)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
        MULTIPLY(LW(6), LW(0))
        SUBTRACT(LW(7), 5)
        CALL(btlevtcmd_SetPartsScale, -2, LW(15), 1, 0, 1)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                LOOP(5)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(0.25f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                LOOP(5)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(-0.25f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, 7, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 0, -1)
        CALL(btlevtcmd_SetPartsPos, -2, LW(15), 0, -1000, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wakka_attack_common_event_hit)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(6))
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 67108864)
    ELSE
        CALL(btlevtcmd_SetPartsBlur, -2, LW(15), 255, 255, 255, 255, 255, 255, 255, 100, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 67108864)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("Z_3"))
    CALL(btlevtcmd_GetPos, LW(12), LW(6), LW(7), LW(8))
    ADD(LW(8), 1)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_CheckPartsAttribute, -2, LW(15), 16777216, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5"))
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(6))
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_SetPartsScale, -2, LW(15), 1, 0, 1)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                LOOP(10)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(0.125f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                LOOP(10)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(-0.125f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
        CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, 4, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 0, -1)
    ELSE
        CALL(btlevtcmd_SetPartsScale, -2, LW(15), 1, 0, 1)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                LOOP(5)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(0.25f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                LOOP(5)
                    CALL(btlevtcmd_AddPartsScale, -2, LW(15), 0, FLOAT(-0.25f), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
        CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, 7, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 0, -1)
        CALL(btlevtcmd_SetPartsPos, -2, LW(15), 0, -1000, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_event_sub)
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
    END_SWITCH
    LABEL(90)
    CALL_CHILD(wakka_attack_common_event_no_hit)
    GOTO(98)
    LABEL(91)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 5)
    LOOP(0)
        CALL(btlevtcmd_GetPartsWork, -2, 1, LW(13), LW(5))
        CALL(btlevtcmd_GetPartsWork, -2, 1, LW(14), LW(6))
        IF_INT_EQUAL(LW(3), LW(5))
            IF_INT_EQUAL(LW(4), LW(6))
                ADD(LW(13), 10)
                CALL(btlevtcmd_SetPartsWork, -2, 1, LW(13), 1)
                BREAK_LOOP
            END_IF
        END_IF
        ADD(LW(13), 1)
        ADD(LW(14), 1)
    END_LOOP
    CALL_CHILD(wakka_attack_common_event_hit)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    IF_INT_EQUAL(LW(10), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    END_IF
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), 0, -1000, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wakka_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_wakka_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_wakka_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 0, LW(3))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 5, LW(4))
    SET_VALUE(LW(9), PTR(&weapon_wakka_attack))
    SET_VALUE(LW(10), 1)
    SET_VALUE(LW(12), -2)
    SET_VALUE(LW(15), 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_wakka_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_wakka_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(14))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(14))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(barriern_zanzou, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK9"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(barriern_zanzou2, -2)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(0), 6)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(0), LW(14))
    LOOP(10)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
    END_LOOP
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK7"), EVTDAT_POINTER_MAX, 0, LW(0))
    CALL(btlevtcmd_SetPartsWork, -2, 7, 15, LW(0))
    CALL_CHILD(damage_check_event_sub)
    CALL(btlevtcmd_GetPartsWork, -2, 7, 15, LW(0))
    CALL(evt_snd_sfxoff, LW(0))
    SET_VALUE(LW(0), -6)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(0), LW(14))
    LOOP(10)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
    END_LOOP
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wawawa_attack_event_satellite_blur)
    CALL(barriern_satellite_zanzou, -2, LW(0))
    CALL(barriern_satellite_zanzou2, -2, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(wawawa_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_wawawa_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_wawawa_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(barriern_wawawa_set_target, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 0, LW(0))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 1, LW(2))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 2, LW(4))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 3, LW(6))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 4, LW(8))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 5, LW(1))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 6, LW(3))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 7, LW(5))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 8, LW(7))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 9, LW(9))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 10, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 11, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 12, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 13, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 14, 0)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_wawawa_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_wawawa_attack))
    CALL(btlevtcmd_GetPartsWork, -2, 1, 0, LW(3))
    CALL(btlevtcmd_GetPartsWork, -2, 1, 5, LW(4))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(0), 0)
    LOOP(2)
        SPAWN_EVENT(wawawa_attack_event_satellite_blur)
        ADD(LW(0), 1)
        WAIT_FRAMES(10)
    END_LOOP
    CALL(barriern_zanzou, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK10"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(barriern_zanzou2, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("F_1"))
    WAIT_FRAMES(20)
    SET_VALUE(LW(11), 4)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(11), 6)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(12))
        CALL(btlevtcmd_GetBodyId, LW(12), LW(13))
        CALL(btlevtcmd_AnimeChangePose, LW(12), LW(13), STRING("F_2"))
        ADD(LW(11), 1)
    END_LOOP
    WAIT_FRAMES(20)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(11), 4)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(11), 6)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(12))
        CALL(btlevtcmd_SetRotate, LW(12), 0, 0, 0)
        ADD(LW(11), 1)
    END_LOOP
    SET_VALUE(LW(0), 6)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(0), LW(14))
    LOOP(10)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
        SET_VALUE(LW(11), 4)
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(11), 6)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(12))
            CALL(btlevtcmd_AddRotate, LW(12), 0, LW(0), 0)
            ADD(LW(11), 1)
        END_LOOP
    END_LOOP
    SET_VALUE(LW(11), 4)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(11), 6)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(12))
        CALL(btlevtcmd_GetBodyId, LW(12), LW(13))
        CALL(btlevtcmd_AnimeChangePose, LW(12), LW(13), STRING("S_2"))
        ADD(LW(11), 1)
    END_LOOP
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
    SET_VALUE(LW(12), -2)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 5)
    SET_VALUE(LW(15), 7)
    SPAWN_EVENT(wawawa_attack_event_sub)
    WAIT_FRAMES(30)
    SET_VALUE(LW(11), 4)
    ADD(LW(13), 1)
    ADD(LW(14), 1)
    ADD(LW(15), 1)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(11), 6)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(12))
        SPAWN_EVENT(wawawa_attack_event_sub)
        WAIT_FRAMES(20)
        ADD(LW(11), 1)
        ADD(LW(13), 1)
        ADD(LW(14), 1)
        ADD(LW(15), 1)
    END_LOOP
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 3)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(10)
    CALL(_check_satellite)
    SET_VALUE(LW(0), -6)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(0), LW(14))
    LOOP(10)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
        SET_VALUE(LW(11), 4)
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(11), 6)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(12))
            CALL(btlevtcmd_AddRotate, LW(12), 0, LW(0), 0)
            ADD(LW(11), 1)
        END_LOOP
    END_LOOP
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wawawa_attack_event_sub)
    CALL(btlevtcmd_GetPartsWork, -2, 1, LW(13), LW(3))
    CALL(btlevtcmd_GetPartsWork, -2, 1, LW(14), LW(4))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BARRI_ATTACK8"), EVTDAT_POINTER_MAX, 0, LW(0))
    CALL(btlevtcmd_SetPartsWork, -2, LW(15), 15, LW(0))
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(13), 1)
            CALL_CHILD(wawawa_attack_event_sub_last)
            BREAK_LOOP
        END_IF
        ADD(LW(13), 1)
        ADD(LW(14), 1)
        CALL(btlevtcmd_GetPartsWork, -2, 1, LW(13), LW(5))
        CALL(btlevtcmd_GetPartsWork, -2, 1, LW(14), LW(6))
        IF_INT_EQUAL(LW(3), LW(5))
            IF_INT_EQUAL(LW(4), LW(6))
                CALL_CHILD(wawawa_attack_event_sub_no_last)
                BREAK_LOOP
            END_IF
        END_IF
    END_LOOP
    CALL(btlevtcmd_AddUnitWork, -2, 2, 1)
    CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(0))
    CALL(evt_snd_sfxoff, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(wawawa_attack_event_sub_no_last)
    SET_VALUE(LW(9), PTR(&weapon_wawawa_attack))
    SET_VALUE(LW(10), 0)
    CALL_CHILD(damage_check_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(wawawa_attack_event_sub_last)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 5)
    LOOP(0)
        CALL(btlevtcmd_GetPartsWork, -2, 1, LW(13), LW(5))
        CALL(btlevtcmd_GetPartsWork, -2, 1, LW(14), LW(6))
        IF_INT_EQUAL(LW(3), LW(5))
            IF_INT_EQUAL(LW(4), LW(6))
                ADD(LW(13), 10)
                CALL(btlevtcmd_GetPartsWork, -2, 1, LW(13), LW(0))
                BREAK_LOOP
            END_IF
        END_IF
        ADD(LW(13), 1)
        ADD(LW(14), 1)
    END_LOOP
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(wakka_attack_common_event_hit)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_wawawa_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_wawawa_attack), 256, LW(5))
        CALL(btlevtcmd_SetPartsPos, -2, LW(15), 0, -1000, 0)
        WAIT_FRAMES(20)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 67108864)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    ELSE
        SET_VALUE(LW(9), PTR(&weapon_wawawa_attack))
        SET_VALUE(LW(10), 1)
        CALL_CHILD(damage_check_event_sub)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(_check_satellite)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    SET_VALUE(LW(11), 4)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(11), 6)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, LW(11), LW(12))
        IF_INT_NOT_EQUAL(LW(12), -1)
            CALL(btlevtcmd_SetUnitWork, LW(12), 1, 1)
        END_IF
        ADD(LW(11), 1)
    END_LOOP
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(1), 255)
        SET_VALUE(LW(2), 255)
        SET_VALUE(LW(3), 255)
        SET_VALUE(LW(4), -8)
        LABEL(0)
        CALL(_check_satellite, LW(0))
        WAIT_FRAMES(1)
        GOTO(0)
    END_SIBLING_EVENT
    LABEL(1)
    CALL(evt_sub_random, 120, LW(0))
    ADD(LW(0), 30)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckStatus, -2, 5, LW(0))
        CALL(btlevtcmd_CheckStatus, -2, 3, LW(1))
        CALL(btlevtcmd_CheckStatus, -2, 4, LW(2))
        OR(LW(0), LW(1))
        OR(LW(0), LW(2))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_3"))
            WAIT_FRAMES(180)
            CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
        END_IF
    END_IF
    GOTO(1)
    RETURN
EVT_END()

static s32 zanzou_count[3];

static USER_FUNC(battleGetBarriernSatelliteInitPos) {
    s32* args;
    BattleUnit* unit;
    s32 id;
    s32 n;
    s32 m;
    s32 i;
    s32 j;
    s32 indexX;
    s32 indexY;
    s32 indexZ;
    const Vec* p;
    BattleWork* wp;
    static const Vec VecData[] = {
        { 25.f, 0.f, 0.f },   { 25.f, 0.f, 0.f },   { -25.f, 0.f, 0.f },  { 0.f, 25.f, 0.f },
        { 25.f, -25.f, 0.f }, { -25.f, -25.f, 0.f }, { 25.f, 25.f, 0.f },  { -25.f, 25.f, 0.f },
        { 25.f, -25.f, 0.f }, { -25.f, -25.f, 0.f },
    };

    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    n = evtGetValue(event, args[1]);
    m = evtGetValue(event, args[2]);
    indexX = args[3];
    indexY = args[4];
    indexZ = args[5];
    p = VecData;
    for (i = 0; i < n; i++) {
        for (j = 0; j < i; j++) {
            p++;
        }
    }
    for (j = 0; j < m; j++) {
        p++;
    }
    evtSetValue(event, indexX, (s32)(p->x * unit->faceDirection));
    evtSetValue(event, indexY, (s32)p->y);
    evtSetValue(event, indexZ, (s32)p->z);
    return EVT_RETURN_DONE;
}


// clang-format on

#include "unit_boss_black_peach.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_batstage.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_ext.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "unit/party/unit_party_vivian.h"
#include "battle/battle_audience.h"
#include "battle/battle_break_slot.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_queen.h"
#include "eff/n64/eff_pokopi_thunder_n64.h"
#include "eff/n64/eff_shock_n64.h"
#include "driver/extdrv.h"
#include "driver/lightdrv.h"
#include "driver/npcdrv.h"
#include "eff/n64/eff_pokopi_thunder_n64.h"
#include "eff/n64/eff_shock_n64.h"
#include "eff/n64/eff_stardust2_n64.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static void hand_init(void);
static void pose_main(void);
static void pose_init(void);
BattleVulnerableStatus peach_regist;
BattleDataEntry peach_data_table[2];
BattleUnitKindPart peach_parts[7];
BattleVulnerableStatus queen_regist;
BattleDataEntry queen_data_table[2];
BattleUnitKindPart queen_parts[5];
BattleVulnerableStatus queen_hand_1_regist;
BattleDataEntry queen_hand_1_data_table[2];
BattleUnitKindPart queen_hand_1_parts[1];
BattleVulnerableStatus queen_hand_2_regist;
BattleDataEntry queen_hand_2_data_table[2];
BattleUnitKindPart queen_hand_2_parts[1];
BattleVulnerableStatus queen_hand_small_regist;
BattleDataEntry queen_hand_small_data_table[2];
BattleUnitKindPart queen_hand_small_parts[1];
s32 peach_init_event[170];
s32 peach_entry_event[62];
s32 peach_damage_event[17];
s32 peach_phase_event[46];
s32 peach_dead_event[11];
s32 peach_attack_event[85];
s32 peach_wait_event[6];
s32 queen_init_event[225];
s32 queen_phase_event[208];
s32 queen_attack_event[163];
s32 queen_damage_event[63];
s32 queen_dead_event[347];
s32 queen_wait_event[10];
s32 queen_entry_event[62];
s32 queen_hand_1_init_event[107];
s32 queen_hand_1_attack_event[41];
s32 queen_hand_1_damage_event[24];
s32 queen_hand_1_dead_event[41];
s32 queen_hand_1_wait_event[6];
s32 queen_hand_2_init_event[113];
s32 queen_hand_2_attack_event[41];
s32 queen_hand_2_damage_event[24];
s32 queen_hand_2_dead_event[41];
s32 queen_hand_2_wait_event[6];
s32 queen_hand_small_init_event[96];
s32 queen_hand_small_attack_event[35];
s32 queen_hand_small_damage_event[48];
s32 queen_hand_small_dead_event[32];
s32 queen_hand_small_wait_event[6];
u32 hand_data[96];

// clang-format off
extend_data(140,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(141,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_black_peach = {
    UNIT_SHADOW_QUEEN_PEACH,    // mUnitType
    "btl_un_black_peach",       // name
    150,                        // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    6,                          // field_0x14
    40,                         // width
    54,                         // height
    { 5, 54 },                  // statusIconOffset
    { 0.0f, 27.0f, 0.0f },      // centerOffset
    { 0, -3 },                  // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },      // talkTogeBaseOffset
    { 20.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 15.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 15.0f, 45.0f, 0.0f },     // kissHitOffset
    { 0.0f, 27.0f, 0.0f },      // cutBaseOffset
    40.0f,                      // cutWidth
    54.0f,                      // cutHeight
    80,                         // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    0,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_BPCH_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &peach_regist,              // vulnStatus
    7,                          // numParts
    peach_parts,                // mParts
    peach_init_event,           // initEvtScript
    peach_data_table,           // dataTable
};

BattleUnitDefense peach_defence = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr peach_defence_attr = { { 0, 0, 0, 0, 0 } };

BattleVulnerableStatus peach_regist = {
    30,  // sleepRate
    30,  // stopRate
    30,  // dizzyRate
    100, // poisonRate
    30,  // confuseRate
    100, // electricRate
    100, // burnRate
    0,   // freezeRate
    100, // hugeRate
    60,  // tinyRate
    100, // attackUpRate
    60,  // attackDownRate
    100, // defenseUpRate
    75,  // defenseDownRate
    30,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    30,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    0,   // instaKillRate
};

BattleDispPose peach_pose_table[18] = {
    { 0x01, "KRT_N_1" },
    { 0x02, "KRT_Z_1" },
    { 0x09, "KRT_Z_1" },
    { 0x05, "KRT_K_1" },
    { 0x04, "KRT_S_1" },
    { 0x03, "KRT_K_1" },
    { 0x1C, "KRT_S_1" },
    { 0x1D, "KRT_D_1" },
    { 0x1E, "KRT_Q_1" },
    { 0x1F, "KRT_Q_1" },
    { 0x27, "KRT_D_1" },
    { 0x32, "KRT_A_1" },
    { 0x2A, "KRT_W_1" },
    { 0x28, "KRT_W_1" },
    { 0x38, "KRT_X_1" },
    { 0x39, "KRT_X_1" },
    { 0x41, "KRT_T_1" },
    { 0x45, "KRT_S_1" },
};

BattleDataEntry peach_data_table[2] = {
    { 0x31, peach_dead_event },
    { 0x00, NULL },
};

BattleUnitKindPart peach_parts[7] = {
    {
        1,                      // partNum
        "btl_un_shadow_queen",  // partName
        "c_b_peach_b",          // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { -2.0f, 44.0f, 0.0f }, // hitBaseOffset
        { -2.0f, 44.0f, 0.0f }, // hitCursorBaseOffset
        41,                     // field_0x30
        61,                     // field_0x32
        255,                    // baseAlpha
        &peach_defence,         // defense
        &peach_defence_attr,    // defenseAttr
        0x00000009,             // attributes
        0,                      // counterAttributes
        peach_pose_table,       // poseTable
    },
    {
        2,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_b",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &peach_defence,        // defense
        &peach_defence_attr,   // defenseAttr
        0x03018000,            // attributes
        0,                     // counterAttributes
        peach_pose_table,      // poseTable
    },
    {
        3,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_f",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &peach_defence,        // defense
        &peach_defence_attr,   // defenseAttr
        0x13018000,            // attributes
        0,                     // counterAttributes
        peach_pose_table,      // poseTable
    },
    {
        4,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_h",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &peach_defence,        // defense
        &peach_defence_attr,   // defenseAttr
        0x13018000,            // attributes
        0,                     // counterAttributes
        peach_pose_table,      // poseTable
    },
    {
        5,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_f",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &peach_defence,        // defense
        &peach_defence_attr,   // defenseAttr
        0x13018000,            // attributes
        0,                     // counterAttributes
        peach_pose_table,      // poseTable
    },
    {
        6,                          // partNum
        "btl_un_shadow_hand_big_1", // partName
        "c_b_hand",                 // modelName
        { 15.0f, 0.0f, -50.0f },    // positionOffset
        { 0.0f, 38.0f, 5.0f },      // hitBaseOffset
        { 0.0f, 38.0f, 5.0f },      // hitCursorBaseOffset
        41,                         // field_0x30
        61,                         // field_0x32
        255,                        // baseAlpha
        &peach_defence,             // defense
        &peach_defence_attr,        // defenseAttr
        0x13018000,                 // attributes
        0,                          // counterAttributes
        peach_pose_table,           // poseTable
    },
    {
        7,                          // partNum
        "btl_un_shadow_hand_big_2", // partName
        "c_b_hand",                 // modelName
        { 160.0f, 0.0f, 50.0f },    // positionOffset
        { 0.0f, 38.0f, 5.0f },      // hitBaseOffset
        { 0.0f, 38.0f, 5.0f },      // hitCursorBaseOffset
        41,                         // field_0x30
        61,                         // field_0x32
        255,                        // baseAlpha
        &peach_defence,             // defense
        &peach_defence_attr,        // defenseAttr
        0x13018000,                 // attributes
        0,                          // counterAttributes
        peach_pose_table,           // poseTable
    },
};

BattleWeapon weapon_peach_black_thunder_attack = {
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
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    4,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
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

BattleWeapon weapon_peach_black_energy_attack = {
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
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00005087,                               // special
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
    3,                                        // atkChangeTime
    3,                                        // atkChangeStrength
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

BattleWeapon weapon_peach_drain_attack = {
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
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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

BattleWeapon weapon_peach_dark_drag_attack = {
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
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
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

BattleWeapon weapon_peach_dark_drag_last_attack = {
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
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    27,                                       // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x0002004F,                               // special
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

BattleWeapon weapon_peach_dark_charge_attack = {
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
    7,                                        // chargeStrength
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

BattleWeapon weapon_peach_black_wave_attack = {
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
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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

BattleWeapon weapon_peach_black_sigh_attack = {
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
    60,                                       // allergicChance
    3,                                        // allergicTime
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

BattleWeapon weapon_peach_black_temptation_attack = {
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

BattleWeapon weapon_peach_black_poison_attack = {
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
    0x00002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    50,                                       // poisonChance
    10,                                       // poisonTime
    1,                                        // poisonStrength
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

BattleWeapon weapon_peach_black_press_attack = {
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
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000007,                               // special
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

BattleWeapon weapon_peach_black_1000hand_attack = {
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
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000007,                               // special
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

BattleUnitKind unit_boss_shadow_queen = {
    UNIT_SHADOW_QUEEN_DEMON,    // mUnitType
    "btl_un_shadow_queen",      // name
    150,                        // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    6,                          // field_0x14
    40,                         // width
    54,                         // height
    { 0, 54 },                  // statusIconOffset
    { 0.0f, 90.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },      // talkTogeBaseOffset
    { 20.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 25.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 50.0f, 40.0f, 0.0f },     // kissHitOffset
    { 0.0f, 27.0f, 0.0f },      // cutBaseOffset
    40.0f,                      // cutWidth
    54.0f,                      // cutHeight
    80,                         // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    0,                          // field_0x8C
    0,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_QWEN_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &queen_regist,              // vulnStatus
    5,                          // numParts
    queen_parts,                // mParts
    queen_init_event,           // initEvtScript
    queen_data_table,           // dataTable
};



// TODO: type
u32 hand_wp[1];

// TODO: type
u32 hand_data[96];

// TODO: write queen_hand_small_drag_4_evt (global, 0x180 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

typedef struct HandElem {
    /*0x00*/ u16 mirror;          // nonzero flips the pose matrix x scale
    /*0x02*/ u8 pad_0x02[2];
    /*0x04*/ s32 poseIdx;         // index into HandWork.poses
    /*0x08*/ OSTime startTime;    // renderTime the pose animation started at
    /*0x10*/ s16 stagger;         // random 0..20 subtracted from phase * speed to stagger posY
    /*0x12*/ s16 wavePhase;       // 0..0x870, advanced and wrapped by the wave hand events
    /*0x14*/ f32 speed;           // random scale on phase
    /*0x18*/ f32 posX;            // stored into pose->matrix[0][3]
    /*0x1C*/ f32 posY;            // stored into pose->matrix[1][3]
    /*0x20*/ f32 posZ;            // stored into pose->matrix[2][3]
    /*0x24*/ u8 pad_0x24[4];
} HandElem;

typedef struct HandPose {
    /*0x00*/ char name[0x10];
    /*0x10*/ char name2[8];
    /*0x18*/ f32 duration;        // HandData.value, divides the elapsed time in pose_main
    /*0x1C*/ s32 firstFrame;      // first frame of the run of same-name entries
    /*0x20*/ s32 frameCount;      // number of entries in the run
} HandPose;

typedef struct HandWork {
    /*0x000*/ HandElem* elems;
    /*0x004*/ s32 count;
    /*0x008*/ HandPose poses[32];
    /*0x488*/ u8 pad_0x488[0x14];
    /*0x49C*/ f32 phase;          // scroll position the hand setup/scroll events step by 1 per frame
    /*0x4A0*/ u8 pad_0x4A0[4];
    /*0x4A4*/ s32 poseCount;      // pose_init counter, then the total number of poses
    /*0x4A8*/ s32 frameTimer;     // counts down a 30-frame hold, or up to 200 in queen_hand_small_1000hand_1_evt
    /*0x4AC*/ s32 state;          // switch selector of the hand scroll and setup events
} HandWork;

typedef struct HandData {
    /*0x0*/ const char* str1;
    /*0x4*/ const char* str2;
    /*0x8*/ f32 value;
} HandData;

#define HAND ((HandWork*)hand_wp[0])

typedef struct ShadowQueenData {
    s32 pose2Id;                // handle from part 2 (poseId), animPoseWorldPositionEvalOn
    s32 pose3Id;                // handle from part 3
    s32 pose4Id;                // handle from part 4
    u8 pad_0x0C[0x34];
    s32 locator2Group;          // group index of "locator12" in pose2Id
    s32 locator3Group;          // group index of "locator12" in pose3Id
} ShadowQueenData;


USER_FUNC(shadow_queen_connect) {
    ShadowQueenData* data;
    BattleUnitPart* part;
    AnimationPose* pose;
    f32* verts;
    if (isFirstCall) {
        data = __memAlloc(4, 0x48);
        event->userdata[0] = (s32)data;
        data->pose2Id = BattleGetUnitPartsPtr(BattleTransID(event, -2), 2)->poseId;
        animPoseWorldPositionEvalOn(data->pose2Id);
        data->locator2Group = animPoseGetGroupIdx(data->pose2Id, "locator12");
        data->pose3Id = BattleGetUnitPartsPtr(BattleTransID(event, -2), 3)->poseId;
        animPoseWorldPositionEvalOn(data->pose3Id);
        data->locator3Group = animPoseGetGroupIdx(data->pose3Id, "locator12");
        animPosePeraOff(data->pose2Id);
        animPosePeraOff(data->pose3Id);
        data->pose4Id = BattleGetUnitPartsPtr(BattleTransID(event, -2), 4)->poseId;
        animPosePeraOff(data->pose4Id);
    }
    data = (ShadowQueenData*)event->userdata[0];
    pose = animPoseGetAnimPosePtr(data->pose2Id);
    verts = *(f32**)((void*)pose->groupWorldPos);
    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 3);
    BtlUnit_SetPartsPos(part, verts[data->locator2Group * 3], verts[data->locator2Group * 3 + 1], verts[data->locator2Group * 3 + 2]);
    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 5);
    part->field_0x4F8 = 7.0f;
    BtlUnit_SetPartsPos(part, verts[data->locator2Group * 3], verts[data->locator2Group * 3 + 1], verts[data->locator2Group * 3 + 2]);
    pose = animPoseGetAnimPosePtr(data->pose3Id);
    verts = *(f32**)((void*)pose->groupWorldPos);
    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 4);
    part->field_0x4F8 = 1.0f;
    BtlUnit_SetPartsPos(part, verts[data->locator3Group * 3], verts[data->locator3Group * 3 + 1], verts[data->locator3Group * 3 + 2]);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(peach_talk_synchro_shadow_head_evt) {
    BattleUnitPart* part;
    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 1);
    if (strcmp(part->poseName, "KRT_T_1") == 0 || strcmp(part->poseName, "KRT_T_2") == 0 ||
        strcmp(part->poseName, "KRT_T_3") == 0) {
        part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 3);
        if (strcmp(part->poseName, "KRT_T_1") != 0) {
            BtlUnit_SetAnim(part, "KRT_T_1");
            return EVT_RETURN_BLOCK;
        }
    }
    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 1);
    if (strcmp(part->poseName, "KRT_T_1") != 0 && strcmp(part->poseName, "KRT_T_2") != 0 &&
        strcmp(part->poseName, "KRT_T_3") != 0) {
        part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 3);
        if (strcmp(part->poseName, "KRT_T_1") == 0) {
            BtlUnit_SetAnim(part, "KRT_S_1");
            return EVT_RETURN_BLOCK;
        }
    }
    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 1);
    if (strcmp(part->poseName, "KRT_D_1") == 0) {
        part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 3);
        if (strcmp(part->poseName, "KRT_D_1") != 0) {
            BtlUnit_SetAnim(part, "KRT_D_1");
            return EVT_RETURN_BLOCK;
        }
    }
    part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 1);
    if (strcmp(part->poseName, "KRT_D_1") != 0) {
        part = BattleGetUnitPartsPtr(BattleTransID(event, -2), 3);
        if (strcmp(part->poseName, "KRT_D_1") == 0) {
            BtlUnit_SetAnim(part, "KRT_S_1");
            return EVT_RETURN_BLOCK;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(make_pattern) {
    HandData* p;
    NpcEntry* npc;
    AnimData* data;
    s32 i;
    p = (HandData*)hand_data;
    npc = npcNameToPtr("dummy");
    animPoseSetAnim(npc->poseId, "KRT_S_1", 1);
    animPoseGetAnimPosePtr(npc->poseId);
    data = animPoseGetAnimDataPtr(npc->poseId);
    for (i = 0; i < (s32)*(f32*)((u8*)data->clip + 0x8); i += 3) {
        p->str1 = "c_s_hand";
        p->str2 = "KRT_S_1";
        p->value = 3.0f * (f32)i;
        p++;
    }
    p->str1 = NULL;
    return EVT_RETURN_DONE;
}

// dont_inline keeps pose_init a real `bl` from hand_init as in retail.
#pragma dont_inline on
static void pose_init(void) {
    char name1[0x20];
    char name2[0x20];
    Mtx mtx;
    LightEntry* light;
    HandData* data;
    s32 i;
    s32 start;
    data = (HandData*)hand_data;
    i = 0;
    light = lightGetPaperCraft();
    light->position = (Vec){-200.0f, 100.0f, 500.0f};
    PSMTXScale(mtx, 3.0f, 3.0f, 3.0f);
    PSMTXMultVec(mtx, &light->position, &light->position);
    strcpy(name1, "");
    strcpy(name2, "");
    HAND->poseCount = -1;
    do {
        if (strcmp(data->str1, name1) != 0) {
            strcpy(name1, data->str1);
            strcpy(name2, "");
        }
        if (strcmp(data->str2, name2) != 0) {
            strcpy(name2, data->str2);
            start = i;
            HAND->poseCount++;
        }
        strcpy(HAND->poses[HAND->poseCount].name, name1);
        strcpy(HAND->poses[HAND->poseCount].name2, name2);
        HAND->poses[HAND->poseCount].firstFrame = start;
        HAND->poses[HAND->poseCount].frameCount = i - start + 1;
        HAND->poses[HAND->poseCount].duration = data->value;
        i++;
        data++;
    } while (data->str1 != NULL);
    HAND->poseCount++;
}
#pragma dont_inline reset

BattleUnitDefense queen_defence = { { 1, 1, 1, 1, 1 } };

BattleUnitDefenseAttr queen_defence_attr = { { 0, 0, 0, 0, 0 } };


BattleVulnerableStatus queen_regist = {
    30,  // sleepRate
    30,  // stopRate
    30,  // dizzyRate
    100, // poisonRate
    30,  // confuseRate
    100, // electricRate
    100, // burnRate
    0,   // freezeRate
    100, // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    60,  // defenseDownRate
    30,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    30,  // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

BattleUnitDefense queen_muteki_defence = { { 99, 99, 99, 99, 99 } };

BattleUnitDefenseAttr queen_muteki_defence_attr = { { 2, 2, 2, 2, 2 } };

BattleVulnerableStatus queen_muteki_regist = {
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

BattleDispPose queen_pose_table[18] = {
    { 0x01, "KRT_S_1" },
    { 0x02, "KRT_S_1" },
    { 0x09, "KRT_S_1" },
    { 0x05, "KRT_S_1" },
    { 0x04, "KRT_S_1" },
    { 0x03, "KRT_S_1" },
    { 0x1C, "KRT_S_1" },
    { 0x1D, "KRT_S_1" },
    { 0x1E, "KRT_S_1" },
    { 0x1F, "KRT_S_1" },
    { 0x27, "KRT_S_1" },
    { 0x32, "KRT_S_1" },
    { 0x2A, "KRT_S_1" },
    { 0x28, "KRT_S_1" },
    { 0x38, "KRT_S_1" },
    { 0x39, "KRT_S_1" },
    { 0x41, "KRT_T_1" },
    { 0x45, "KRT_S_1" },
};

BattleDispPose queen_hand_pose_table[4] = {
    { 0x1C, "KRT_S_1" },
    { 0x1F, "KRT_S_1" },
    { 0x27, "KRT_S_1" },
    { 0x45, "KRT_S_1" },
};

BattleDataEntry queen_data_table[2] = {
    { 0x31, queen_dead_event },
    { 0x00, NULL },
};

BattleUnitKindPart queen_parts[5] = {
    {
        1,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_b_peach_b",         // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 47.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 47.0f, 0.0f }, // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &queen_defence,        // defense
        &queen_defence_attr,   // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        peach_pose_table,      // poseTable
    },
    {
        2,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_b",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &queen_defence,        // defense
        &queen_defence_attr,   // defenseAttr
        0x02018000,            // attributes
        0,                     // counterAttributes
        queen_pose_table,      // poseTable
    },
    {
        3,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_f",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &queen_defence,        // defense
        &queen_defence_attr,   // defenseAttr
        0x12018000,            // attributes
        0,                     // counterAttributes
        queen_pose_table,      // poseTable
    },
    {
        4,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_h",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &queen_defence,        // defense
        &queen_defence_attr,   // defenseAttr
        0x12018000,            // attributes
        0,                     // counterAttributes
        queen_pose_table,      // poseTable
    },
    {
        5,                     // partNum
        "btl_un_shadow_queen", // partName
        "c_q_kage_f",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 0.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },  // hitCursorBaseOffset
        41,                    // field_0x30
        61,                    // field_0x32
        255,                   // baseAlpha
        &queen_defence,        // defense
        &queen_defence_attr,   // defenseAttr
        0x13018000,            // attributes
        0,                     // counterAttributes
        queen_pose_table,      // poseTable
    },
};

// TODO: type
u32 hair_offset[36] = {
    0xC1500000, 0x427C0000, 0x40A00000, 0x42640000,
    0xC1C00000, 0xC2200000, 0xC1200000, 0x42740000,
    0xC0E00000, 0x41C00000, 0x41880000, 0xC1800000,
    0xC1200000, 0x42700000, 0xC1100000, 0xC1D80000,
    0x41E80000, 0x40E00000, 0xC0800000, 0x427C0000,
    0xC0A00000, 0x41A80000, 0xC0400000, 0x41600000,
    0xC0C00000, 0x427C0000, 0x3F800000, 0x41B00000,
    0xC1900000, 0xC1900000, 0xC0E00000, 0x42740000,
    0x00000000, 0x41500000, 0x00000000, 0xC1A80000,
};

EVT_BEGIN(shadow_queen_1000)
    CALL(evt_npc_entry, STRING("dummy"), STRING("c_s_hand"))
    CALL(evt_npc_set_anim, STRING("dummy"), STRING("KRT_S_1"))
    CALL(evt_npc_set_position, STRING("dummy"), 0, -1000, 0)
    CALL(make_pattern)
    CALL(evt_ext_entry, 100, PTR(hand_data), PTR(hand_init), PTR(hand_main), 0)
    RETURN
EVT_END()

BattleUnitKind unit_boss_shadow_hand_1 = {
    UNIT_SHADOW_QUEEN_LEFT_RIGHT_HAND, // mUnitType
    "btl_un_shadow_queen",             // name
    5,                                 // maxHp
    0,                                 // maxFp
    1,                                 // mDangerHP
    1,                                 // mPerilHP
    0,                                 // mLevel
    0,                                 // mBonusExp
    0,                                 // bonusCoinRolls
    100,                               // bonusCoinRate
    0,                                 // baseCoin
    50,                                // field_0x13
    7,                                 // field_0x14
    90,                                // width
    110,                               // height
    { 0, 54 },                         // statusIconOffset
    { 0.0f, 27.0f, 0.0f },             // centerOffset
    { 0, 0 },                          // hpGaugeOffset
    { 40.0f, 40.0f, 0.0f },            // talkTogeBaseOffset
    { 45.0f, 0.0f, -10.0f },           // field_0x3C
    { 0.0f, 0.0f, 0.0f },              // burnFlameOffset
    0.0f,                              // field_0x54
    0.0f,                              // field_0x58
    { 25.0f, 0.0f, 0.0f },             // bintaHitOffset
    { 45.0f, 71.5f, 0.0f },            // kissHitOffset
    { -30.0f, 55.0f, 0.0f },           // cutBaseOffset
    75.0f,                             // cutWidth
    110.0f,                            // cutHeight
    100,                               // turnOrder
    0,                                 // turnOrderVariance
    -1,                                // swallowRate
    0,                                 // swallowAttribute
    0,                                 // field_0x8C
    0,                                 // itemStealParam
    { 0.0f, 0.0f, 0.0f },              // starPointDispOffset
    "SFX_BOSS_QWEN_DAMAGED1",          // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",            // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",             // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",        // electricDamageSfxName
    0x02000000,                        // attributeFlags
    &queen_hand_1_regist,              // vulnStatus
    1,                                 // numParts
    queen_hand_1_parts,                // mParts
    queen_hand_1_init_event,           // initEvtScript
    queen_hand_1_data_table,           // dataTable
};

BattleUnitDefense queen_hand_1_defence = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr queen_hand_1_defence_attr = { { 0, 0, 0, 0, 0 } };

BattleVulnerableStatus queen_hand_1_regist = {
    30,  // sleepRate
    30,  // stopRate
    30,  // dizzyRate
    0,   // poisonRate
    30,  // confuseRate
    0,   // electricRate
    100, // burnRate
    0,   // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    0,   // attackUpRate
    0,   // attackDownRate
    0,   // defenseUpRate
    80,  // defenseDownRate
    30,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    0,   // fastRate
    0,   // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

BattleDispPose queen_hand_1_pose_table[8] = {
    { 0x1C, "KRT_S_1" },
    { 0x1D, "KRT_Q_1" },
    { 0x1E, "KRT_Q_1" },
    { 0x1F, "KRT_D_1" },
    { 0x27, "KRT_D_1" },
    { 0x38, "KRT_S_1" },
    { 0x39, "KRT_S_1" },
    { 0x45, "KRT_S_1" },
};

BattleDataEntry queen_hand_1_data_table[2] = {
    { 0x31, queen_hand_1_dead_event },
    { 0x00, NULL },
};

BattleUnitKindPart queen_hand_1_parts[1] = {
    {
        1,                          // partNum
        "btl_un_shadow_hand_big_1", // partName
        "c_b_hand",                 // modelName
        { 0.0f, 0.0f, 0.0f },       // positionOffset
        { 16.0f, 47.0f, 30.0f },    // hitBaseOffset
        { 16.0f, 47.0f, 30.0f },    // hitCursorBaseOffset
        41,                         // field_0x30
        61,                         // field_0x32
        255,                        // baseAlpha
        &queen_hand_1_defence,      // defense
        &queen_hand_1_defence_attr, // defenseAttr
        0x03018009,                 // attributes
        0,                          // counterAttributes
        queen_hand_1_pose_table,    // poseTable
    },
};

BattleUnitKind unit_boss_shadow_hand_2 = {
    UNIT_SHADOW_QUEEN_LEFT_RIGHT_HAND, // mUnitType
    "btl_un_shadow_queen",             // name
    5,                                 // maxHp
    0,                                 // maxFp
    1,                                 // mDangerHP
    1,                                 // mPerilHP
    0,                                 // mLevel
    0,                                 // mBonusExp
    0,                                 // bonusCoinRolls
    100,                               // bonusCoinRate
    0,                                 // baseCoin
    50,                                // field_0x13
    7,                                 // field_0x14
    90,                                // width
    110,                               // height
    { 0, 54 },                         // statusIconOffset
    { 0.0f, 27.0f, 0.0f },             // centerOffset
    { 0, 0 },                          // hpGaugeOffset
    { 40.0f, 40.0f, 0.0f },            // talkTogeBaseOffset
    { 45.0f, 0.0f, -10.0f },           // field_0x3C
    { 0.0f, 0.0f, 0.0f },              // burnFlameOffset
    0.0f,                              // field_0x54
    0.0f,                              // field_0x58
    { 25.0f, 0.0f, 0.0f },             // bintaHitOffset
    { 45.0f, 71.5f, 0.0f },            // kissHitOffset
    { -30.0f, 55.0f, 0.0f },           // cutBaseOffset
    75.0f,                             // cutWidth
    110.0f,                            // cutHeight
    100,                               // turnOrder
    0,                                 // turnOrderVariance
    -1,                                // swallowRate
    0,                                 // swallowAttribute
    0,                                 // field_0x8C
    0,                                 // itemStealParam
    { 0.0f, 0.0f, 0.0f },              // starPointDispOffset
    "SFX_BOSS_QWEN_DAMAGED1",          // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",            // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",             // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",        // electricDamageSfxName
    0x02000000,                        // attributeFlags
    &queen_hand_2_regist,              // vulnStatus
    1,                                 // numParts
    queen_hand_2_parts,                // mParts
    queen_hand_2_init_event,           // initEvtScript
    queen_hand_2_data_table,           // dataTable
};

BattleUnitDefense queen_hand_2_defence = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr queen_hand_2_defence_attr = { { 0, 0, 0, 0, 0 } };

BattleVulnerableStatus queen_hand_2_regist = {
    30,  // sleepRate
    30,  // stopRate
    30,  // dizzyRate
    0,   // poisonRate
    30,  // confuseRate
    0,   // electricRate
    100, // burnRate
    0,   // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    0,   // attackUpRate
    0,   // attackDownRate
    0,   // defenseUpRate
    80,  // defenseDownRate
    30,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    0,   // fastRate
    0,   // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

BattleDispPose queen_hand_2_pose_table[8] = {
    { 0x1C, "KRT_S_1" },
    { 0x1D, "KRT_Q_1" },
    { 0x1E, "KRT_Q_1" },
    { 0x1F, "KRT_D_1" },
    { 0x27, "KRT_D_1" },
    { 0x38, "KRT_S_1" },
    { 0x39, "KRT_S_1" },
    { 0x45, "KRT_S_1" },
};

BattleDataEntry queen_hand_2_data_table[2] = {
    { 0x31, queen_hand_2_dead_event },
    { 0x00, NULL },
};

BattleUnitKindPart queen_hand_2_parts[1] = {
    {
        1,                          // partNum
        "btl_un_shadow_hand_big_2", // partName
        "c_b_hand",                 // modelName
        { 0.0f, 0.0f, 0.0f },       // positionOffset
        { 28.0f, 56.0f, 20.0f },    // hitBaseOffset
        { 28.0f, 56.0f, 20.0f },    // hitCursorBaseOffset
        41,                         // field_0x30
        61,                         // field_0x32
        255,                        // baseAlpha
        &queen_hand_2_defence,      // defense
        &queen_hand_2_defence_attr, // defenseAttr
        0x03018009,                 // attributes
        0,                          // counterAttributes
        queen_hand_2_pose_table,    // poseTable
    },
};

BattleUnitKind unit_boss_shadow_hand_small = {
    UNIT_SHADOW_QUEEN_DEAD_HANDS, // mUnitType
    "btl_un_shadow_queen",        // name
    8,                            // maxHp
    0,                            // maxFp
    1,                            // mDangerHP
    1,                            // mPerilHP
    0,                            // mLevel
    0,                            // mBonusExp
    0,                            // bonusCoinRolls
    100,                          // bonusCoinRate
    0,                            // baseCoin
    50,                           // field_0x13
    7,                            // field_0x14
    220,                          // width
    45,                           // height
    { 0, 54 },                    // statusIconOffset
    { 0.0f, 27.0f, 0.0f },        // centerOffset
    { 0, 0 },                     // hpGaugeOffset
    { 40.0f, 40.0f, 0.0f },       // talkTogeBaseOffset
    { 110.0f, 0.0f, -10.0f },     // field_0x3C
    { 0.0f, 0.0f, 0.0f },         // burnFlameOffset
    0.0f,                         // field_0x54
    0.0f,                         // field_0x58
    { 110.0f, 0.0f, 0.0f },       // bintaHitOffset
    { 110.0f, 29.25f, 0.0f },     // kissHitOffset
    { 0.0f, 22.0f, 0.0f },        // cutBaseOffset
    180.0f,                       // cutWidth
    45.0f,                        // cutHeight
    90,                           // turnOrder
    0,                            // turnOrderVariance
    -1,                           // swallowRate
    0,                            // swallowAttribute
    0,                            // field_0x8C
    0,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },         // starPointDispOffset
    "SFX_BOSS_QWEN_DAMAGED1",     // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",       // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",        // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",   // electricDamageSfxName
    0,                            // attributeFlags
    &queen_hand_small_regist,     // vulnStatus
    1,                            // numParts
    queen_hand_small_parts,       // mParts
    queen_hand_small_init_event,  // initEvtScript
    queen_hand_small_data_table,  // dataTable
};

BattleUnitDefense queen_hand_small_defence = { { 0, 0, 0, 0, 0 } };

BattleUnitDefenseAttr queen_hand_small_defence_attr = { { 0, 0, 0, 0, 0 } };

BattleVulnerableStatus queen_hand_small_regist = {
    30,  // sleepRate
    30,  // stopRate
    30,  // dizzyRate
    0,   // poisonRate
    30,  // confuseRate
    0,   // electricRate
    100, // burnRate
    0,   // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    0,   // attackUpRate
    0,   // attackDownRate
    0,   // defenseUpRate
    80,  // defenseDownRate
    30,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    0,   // fastRate
    0,   // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

BattleDispPose queen_hand_small_pose_table[8] = {
    { 0x1C, "KRT_S_1" },
    { 0x1D, "KRT_S_1" },
    { 0x1E, "KRT_S_1" },
    { 0x1F, "KRT_S_1" },
    { 0x27, "KRT_S_1" },
    { 0x38, "KRT_S_1" },
    { 0x39, "KRT_S_1" },
    { 0x45, "KRT_S_1" },
};

BattleDataEntry queen_hand_small_data_table[2] = {
    { 0x31, queen_hand_small_dead_event },
    { 0x00, NULL },
};

BattleUnitKindPart queen_hand_small_parts[1] = {
    {
        1,                              // partNum
        "btl_un_shadow_hand_small",     // partName
        "c_b_hand",                     // modelName
        { 0.0f, 0.0f, 0.0f },           // positionOffset
        { 70.0f, 18.0f, 0.0f },         // hitBaseOffset
        { 70.0f, 18.0f, 0.0f },         // hitCursorBaseOffset
        188,                            // field_0x30
        40,                             // field_0x32
        255,                            // baseAlpha
        &queen_hand_small_defence,      // defense
        &queen_hand_small_defence_attr, // defenseAttr
        0x03008009,                     // attributes
        0,                              // counterAttributes
        queen_hand_small_pose_table,    // poseTable
    },
};

BattleUnitSetup entry_shadow_queen[1] = {
    { &unit_boss_shadow_queen, 1, 0x04000004, { 75.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_shadow_hand_1[1] = {
    { &unit_boss_shadow_hand_1, 1, 0x04000004, { 15.0f, 0.0f, -50.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_shadow_hand_2[1] = {
    { &unit_boss_shadow_hand_2, 1, 0x04000004, { 160.0f, 0.0f, 50.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_shadow_hand_small[1] = {
    { &unit_boss_shadow_hand_small, 1, 0x04000004, { 95.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

EVT_BEGIN(peach_init_event)
    SET_VALUE(GW(8), -1)
    SET_VALUE(GW(9), -1)
    SET_VALUE(GW(10), -1)
    SET_VALUE(GW(14), 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    ELSE
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_queen), 0)
        CALL(btlevtcmd_SetPos, -2, 0, -1000, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 50397184)
        CALL(btlevtcmd_OnAttribute, -2, 301989888)
        CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    SET_VALUE(GW(13), LW(0))
    BEGIN_INLINE_EVENT
        CALL(shadow_queen_connect)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(peach_talk_synchro_shadow_head_evt)
    END_INLINE_EVENT
    CALL(btlevtcmd_SetPartsBaseScale, -2, 7, FLOAT(-1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_SetPartsBaseRotate, -2, 6, 0, 315, 0)
    CALL(btlevtcmd_SetPartsBaseRotate, -2, 7, 0, 315, 0)
    CALL(btlevtcmd_SetPartsPos, -2, 6, 15, 0, -50)
    CALL(btlevtcmd_SetPartsPos, -2, 7, 160, 0, 50)
    SPAWN_EVENT(shadow_queen_1000)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(360.0f))
        LOOP(0)
            CALL(btlevtcmd_SetPartsRotate, -2, 2, FLOAT(0.0f), LW(0), FLOAT(3.7f))
            SUBTRACT_FLOAT(LW(0), FLOAT(0.72f))
            IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRT_S_1"))
    CALL(btlevtcmd_SetEventWait, -2, PTR(peach_wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(peach_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(peach_attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(peach_damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(peach_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(peach_entry_event)
    WAIT_MS(2000)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 83, 69, 332, 83, 46, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_140_02"), 0, -2)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_moveto, 0, -122, 69, 332, -122, 46, 0, 15, 11)
    WAIT_MS(550)
    CALL(evt_msg_print_battle_party, STRING("stg8_las_141"))
    CALL(btlevtcmd_StartWaitEvent, -4)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(unk_las_0000af38)
    RETURN
EVT_END()

EVT_BEGIN(peach_damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(peach_change_evt)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 83, 69, 332, 83, 46, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_141_01"), 0, -2)
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_L_2"))
    WAIT_MS(200)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("KRT_T_3"))
    CALL(evt_msg_print, 2, STRING("stg8_las_141_02"), 0, -2)
    WAIT_MS(200)
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 0, 500)
    END_INLINE_EVENT
    CALL(evt_btl_camera_set_moveto, 0, 83, 125, 565, 83, 86, 0, 30, 11)
    WAIT_MS(400)
    CALL(btlevtcmd_ClearAllStatus, -2)
    WAIT_MS(400)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("m_q"), STRING("mist2"), 0, LW(0), 15, LW(2), 100, 0, -1, 0, 0, 0, 0, 0)
    ADD(LW(2), 30)
    CALL(evt_eff, STRING("m_q2"), STRING("queen"), 0, LW(0), 0, LW(2), FLOAT(1.0f), 330, 0, 0, 0, 0, 0, 0)
    CALL(eff_queen_set_alpha_evt, 90)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_QWEN_APPEAR2"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), 0, LW(2))
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_O_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KRT_S_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRT_S_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRT_O_2"))
        CALL(btlevtcmd_SetAlpha, -2, 2, 0)
        CALL(btlevtcmd_SetAlpha, -2, 3, 0)
        CALL(btlevtcmd_SetAlpha, -2, 4, 0)
        CALL(btlevtcmd_SetAlpha, -2, 5, 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 4, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 5, 16777216)
        SET_VALUE(LW(0), 0)
        LOOP(63)
            CALL(btlevtcmd_SetAlpha, -2, 2, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 3, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 4, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 5, LW(0))
            ADD(LW(0), 4)
            WAIT_MS(50)
        END_LOOP
        CALL(btlevtcmd_SetAlpha, -2, 2, 255)
        CALL(btlevtcmd_SetAlpha, -2, 3, 255)
        CALL(btlevtcmd_SetAlpha, -2, 4, 255)
        CALL(btlevtcmd_SetAlpha, -2, 5, 255)
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
        CALL(btlevtcmd_OnPartsAttribute, -2, 5, 16777216)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_1), 0)
        SET_VALUE(GW(8), LW(0))
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_2), 0)
        SET_VALUE(GW(9), LW(0))
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff_f, 513, 2000)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_FLOAT_VALUE(LW(1), LW(1))
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    LOOP(100)
        ADD_FLOAT(LW(1), FLOAT(0.41f))
        ADD_FLOAT(LW(3), FLOAT(0.41f))
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        SET_FLOAT_VALUE(LW(4), LW(1))
        SUBTRACT_FLOAT(LW(4), LW(3))
        CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(4), LW(2))
        WAIT_MS(20)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_L_1"))
    WAIT_MS(800)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_T_1"))
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 83, 141, 412, 83, 111, 0, 30, 11)
    CALL(evt_snd_bgmon, 513, STRING("BGM_BOSS_STG8_LAST_BOSS1"))
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_141_03"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsHomePos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&queen_muteki_defence))
    CALL(btlevtcmd_SetPartsDefenceTable, GW(8), 1, PTR(&queen_muteki_defence))
    CALL(btlevtcmd_SetPartsDefenceTable, GW(9), 1, PTR(&queen_muteki_defence))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&queen_muteki_defence_attr))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, GW(8), 1, PTR(&queen_muteki_defence_attr))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, GW(9), 1, PTR(&queen_muteki_defence_attr))
    CALL(btlevtcmd_SetRegistStatus, -2, PTR(&queen_muteki_regist))
    CALL(btlevtcmd_SetRegistStatus, GW(8), PTR(&queen_muteki_regist))
    CALL(btlevtcmd_SetRegistStatus, GW(9), PTR(&queen_muteki_regist))
    CALL(btlevtcmd_OffPartsAttribute, GW(8), 1, 65536)
    CALL(btlevtcmd_OffPartsAttribute, GW(9), 1, 65536)
    CALL(btlevtcmd_OffAttribute, GW(8), 33554432)
    CALL(btlevtcmd_OffAttribute, GW(9), 33554432)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 34, -15, 30)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 34, -15, 30)
    CALL(btlevtcmd_SetBintaHitOffset, -2, 25, 0, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 50, 40, 0)
    CALL(btlevtcmd_SetEventPhase, -2, PTR(queen_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(queen_attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(queen_damage_event))
    CALL(btlevtcmd_SetEventWait, -2, PTR(queen_wait_event))
    SET_VALUE(GW(11), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    SET_VALUE(GW(10), -1)
    SET_VALUE(GW(12), 0)
    CALL(btlevtcmd_GetHp, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetHp, -2, 112)
    END_IF
    CALL(btlevtcmd_GetHp, -2, LW(0))
    SET_VALUE(GSW(1124), LW(0))
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_phase_event)
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 4)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetHp, -2, LW(0))
    IF_INT_LESS(LW(0), 112)
        CALL_CHILD(peach_change_evt)
        CALL(btlevtcmd_reset_turn)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(peach_dead_event)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL_CHILD(peach_change_evt)
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_reset_turn)
    RETURN
EVT_END()

EVT_BEGIN(peack_attack_black_thunder)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_thunder_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_thunder_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_thunder_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_1"))
    WAIT_MS(500)
    WAIT_MS(700)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(queen_thunder, LW(0), LW(1), LW(2))
    WAIT_MS(80)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_thunder_attack), 256, LW(5))
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
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_thunder_attack))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_thunder_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_thunder_attack), 256, LW(5))
    LABEL(95)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(peack_attack_black_energy)
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    WAIT_MS(200)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_2"))
    WAIT_MS(750)
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 0, 1000)
    END_INLINE_EVENT
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    ADD(LW(1), LW(5))
    ADD(LW(1), -15)
    CALL(evt_eff, STRING(""), STRING("charge"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 60, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_DARK_POWER1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_energy_attack))
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, PTR(&weapon_peach_black_energy_attack), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(peack_attack_dark_drag)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_dark_drag_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    WAIT_MS(200)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_QWEN_APPEAR1"), LW(0), LW(1), LW(2), LW(10))
        CALL(btlevtcmd_SetAlpha, -2, 2, 0)
        CALL(btlevtcmd_SetAlpha, -2, 3, 0)
        CALL(btlevtcmd_SetAlpha, -2, 4, 0)
        CALL(btlevtcmd_SetAlpha, -2, 6, 0)
        CALL(btlevtcmd_SetAlpha, -2, 7, 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 4, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 6, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 7, 16777216)
        SET_VALUE(LW(0), 0)
        LOOP(6)
            CALL(btlevtcmd_SetAlpha, -2, 2, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 3, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 4, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 6, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 7, LW(0))
            ADD(LW(0), 16)
            WAIT_MS(5)
        END_LOOP
    END_INLINE_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_2"))
    WAIT_MS(500)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(500)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_APPEAR1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(peach_hand1000_dark_drag_1_evt)
    END_INLINE_EVENT
    CALL(evt_eff, STRING("m_h_s1"), STRING("mist2"), 0, -100, 15, 0, 100, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s2"), STRING("mist2"), 0, -100, 15, 0, 100, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s3"), STRING("mist2"), 0, -100, 15, 0, 100, 0, -1, 0, 0, 0, 0, 0)
    WAIT_MS(1250)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 256, LW(5))
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
    WAIT_MS(2250)
    CALL(evt_eff_softdelete, STRING("m_h_s1"))
    CALL(evt_eff_softdelete, STRING("m_h_s2"))
    CALL(evt_eff_softdelete, STRING("m_h_s3"))
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_dark_drag_attack))
    CALL(battle_evt_majo_disp_off, LW(3), LW(4), 0, 0)
    CALL(btlevtcmd_SetScale, LW(3), 0, 0, 0)
    CALL(btlevtcmd_GetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 0, LW(5))
    WAIT_MS(750)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 0, LW(5))
    WAIT_MS(750)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 0, LW(5))
    WAIT_MS(750)
    CALL(evt_eff_softdelete, STRING("m_h_s1"))
    CALL(evt_eff_softdelete, STRING("m_h_s2"))
    CALL(evt_eff_softdelete, STRING("m_h_s3"))
    CALL(btlevtcmd_GetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetScale, LW(3), 1, 1, 1)
    CALL(battle_evt_majo_disp_on, LW(3), LW(4), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_last_attack), 256, LW(5))
    WAIT_MS(750)
    LABEL(95)
    LABEL(98)
    CALL(evt_snd_sfxoff, LW(10))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 100)
        LOOP(6)
            CALL(btlevtcmd_SetAlpha, -2, 2, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 3, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 4, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 6, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 7, LW(0))
            ADD(LW(0), -16)
            WAIT_MS(5)
        END_LOOP
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 4, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 6, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 7, 16777216)
    END_INLINE_EVENT
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(peack_attack_drain)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_drain_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_drain_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_drain_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    WAIT_MS(200)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_QWEN_APPEAR1"), LW(0), LW(1), LW(2), LW(10))
        CALL(btlevtcmd_SetAlpha, -2, 2, 0)
        CALL(btlevtcmd_SetAlpha, -2, 3, 0)
        CALL(btlevtcmd_SetAlpha, -2, 4, 0)
        CALL(btlevtcmd_SetAlpha, -2, 7, 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 4, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 6, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 7, 16777216)
        SET_VALUE(LW(0), 0)
        LOOP(6)
            CALL(btlevtcmd_SetAlpha, -2, 2, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 3, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 4, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 7, LW(0))
            ADD(LW(0), 16)
            WAIT_MS(5)
        END_LOOP
    END_INLINE_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_2"))
    WAIT_MS(500)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_MS(500)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRT_S_1"))
    CALL(btlevtcmd_SetAlpha, -2, 6, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, 6, 16777216)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("m_h_1"), STRING("mist2"), 0, LW(0), 15, LW(2), 50, 0, -1, 0, 0, 0, 0, 0)
    ADD(LW(0), 20)
    SET_VALUE(LW(1), 0)
    ADD(LW(2), -30)
    CALL(btlevtcmd_SetPartsPos, -2, 6, LW(0), LW(1), LW(2))
    SET_VALUE(LW(0), 0)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 6, LW(0))
        ADD(LW(0), 16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 6, 255)
    WAIT_MS(500)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_DRAIN1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 6, STRING("KRT_A2_1"))
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        CALL(queen_drain, LW(0), LW(1), LW(2))
        WAIT_MS(600)
        CALL(queen_drain, LW(0), LW(1), LW(2))
        WAIT_MS(600)
        CALL(queen_drain, LW(0), LW(1), LW(2))
        WAIT_MS(600)
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_drain_attack), 256, LW(5))
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
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_drain_attack))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_drain_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_drain_attack), 256, LW(5))
    CALL(btlevtcmd_GetHpDamage, LW(3), LW(0))
    CALL(btlevtcmd_RecoverHp, -2, 1, LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_GetHeight, -2, LW(8))
    ADD(LW(6), LW(8))
    ADD(LW(7), 30)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(5), LW(6), LW(7), LW(0), 0, 0, 0, 0, 0, 0, 0)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_eff_softdelete, STRING("m_h_1"))
    CALL(evt_snd_sfxoff, LW(10))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 100)
        LOOP(6)
            CALL(btlevtcmd_SetAlpha, -2, 2, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 3, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 4, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 7, LW(0))
            ADD(LW(0), -16)
            WAIT_MS(5)
        END_LOOP
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 4, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 7, 16777216)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 255)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 6, LW(0))
        ADD(LW(0), -16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_OnPartsAttribute, -2, 6, 16777216)
    CALL(btlevtcmd_SetPartsPos, -2, 6, 15, 0, -50)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(peach_attack_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        CALL_CHILD(peack_attack_black_thunder)
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
        CALL_CHILD(peack_attack_black_energy)
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 3)
        CALL_CHILD(peack_attack_dark_drag)
        CASE_EQUAL(3)
        CALL(btlevtcmd_DrawLots, LW(0), 4, 100, 30, 30, 30)
        CALL(btlevtcmd_CheckStatus, -2, 14, LW(1))
        IF_INT_EQUAL(LW(1), 1)
            IF_INT_EQUAL(LW(0), 1)
                SET_VALUE(LW(0), 0)
            END_IF
        END_IF
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL_CHILD(peack_attack_black_thunder)
            CASE_EQUAL(1)
            CALL_CHILD(peack_attack_black_energy)
            CASE_EQUAL(2)
            CALL_CHILD(peack_attack_drain)
            CASE_EQUAL(3)
            CALL_CHILD(peack_attack_dark_drag)
        END_SWITCH
    END_SWITCH
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(peach_wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_init_event)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    CALL(btlevtcmd_SetHp, -2, LW(0))
    CALL(btlevtcmd_SetEventWait, -2, PTR(queen_wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(queen_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(queen_attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(queen_damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(queen_entry_event))
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    SET_VALUE(GW(11), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 3)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    SET_VALUE(GW(10), -1)
    SET_VALUE(GW(12), 300)
    SET_VALUE(GW(18), 0)
    CALL(queen_kankyaku_delete_evt)
    CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_1), 0)
    SET_VALUE(GW(8), LW(0))
    CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_2), 0)
    SET_VALUE(GW(9), LW(0))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    SET_VALUE(GW(13), LW(0))
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(360.0f))
        LOOP(0)
            CALL(btlevtcmd_SetPartsRotate, -2, 2, FLOAT(0.0f), LW(0), FLOAT(3.7f))
            SUBTRACT_FLOAT(LW(0), FLOAT(0.72f))
            IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(shadow_queen_connect)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(peach_talk_synchro_shadow_head_evt)
    END_INLINE_EVENT
    SPAWN_EVENT(shadow_queen_1000)
    CALL(btlevtcmd_OffPartsAttribute, GW(8), 1, 65536)
    CALL(btlevtcmd_OffPartsAttribute, GW(9), 1, 65536)
    CALL(btlevtcmd_OffAttribute, GW(8), 33554432)
    CALL(btlevtcmd_OffAttribute, GW(9), 33554432)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 34, -15, 30)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 34, -15, 30)
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRT_S_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("m_q"), STRING("mist2"), 0, LW(0), 15, LW(2), 100, 0, -1, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(queen_first_attack_message_evt)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_142"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    SET_VALUE(GW(12), 100)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(queen_next_attack_message_evt)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_143"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
    SET_VALUE(GW(12), 200)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(queen_last_attack_message_evt)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        RETURN
    END_IF
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_moveto, 0, -113, 69, 332, -113, 46, 0, 15, 11)
    WAIT_MS(550)
    CALL(evt_msg_print_battle_party, STRING("stg8_las_145"))
    CALL(btlevtcmd_StartWaitEvent, -4)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 3)
    SET_VALUE(GW(12), 300)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_InterruptStop, 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_change_hand_small_evt)
    CALL(btlevtcmd_GetUnitWork, GW(13), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        RETURN
    END_IF
    SET_VALUE(GF(1), 1)
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(8), -1)
            SET_VALUE(GF(1), 0)
            CALL_CHILD(queen_hand_1_dead_event)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(9), -1)
            SET_VALUE(GF(1), 0)
            CALL_CHILD(queen_hand_2_dead_event)
        END_IF
    END_INLINE_EVENT
    WAIT_MS(300)
    SET_VALUE(GF(0), 0)
    CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_small), 0)
    SET_VALUE(GW(10), LW(0))
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(0)
        IF_INT_EQUAL(GF(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    SET_VALUE(GW(11), 2)
    RETURN
EVT_END()

EVT_BEGIN(queen_change_hand_big_evt)
    CALL(btlevtcmd_GetUnitWork, GW(13), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        RETURN
    END_IF
    SET_VALUE(GF(1), 1)
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(10), -1)
            SET_VALUE(GF(1), 0)
            CALL_CHILD(queen_hand_small_dead_event)
        END_IF
    END_INLINE_EVENT
    WAIT_MS(300)
    SET_VALUE(GF(0), 0)
    CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_1), 0)
    SET_VALUE(GW(8), LW(0))
    CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_2), 0)
    SET_VALUE(GW(9), LW(0))
    CALL(btlevtcmd_OffPartsAttribute, GW(8), 1, 65536)
    CALL(btlevtcmd_OffPartsAttribute, GW(9), 1, 65536)
    CALL(btlevtcmd_OffAttribute, GW(8), 33554432)
    CALL(btlevtcmd_OffAttribute, GW(9), 33554432)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(0)
        IF_INT_EQUAL(GF(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    SET_VALUE(GW(11), 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_big_life_check_evt)
    CALL(btlevtcmd_GetUnitWork, GW(13), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        RETURN
    END_IF
    SET_VALUE(GF(0), 1)
    IF_INT_EQUAL(GW(8), -1)
        SET_VALUE(GF(0), 0)
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_1), 0)
        SET_VALUE(GW(8), LW(0))
        CALL(btlevtcmd_OffPartsAttribute, GW(8), 1, 65536)
        CALL(btlevtcmd_OffAttribute, GW(8), 33554432)
    END_IF
    IF_INT_EQUAL(GW(9), -1)
        SET_VALUE(GF(0), 0)
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_2), 0)
        SET_VALUE(GW(9), LW(0))
        CALL(btlevtcmd_OffPartsAttribute, GW(9), 1, 65536)
        CALL(btlevtcmd_OffAttribute, GW(9), 33554432)
    END_IF
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_small_life_check_evt)
    CALL(btlevtcmd_GetUnitWork, GW(13), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        RETURN
    END_IF
    IF_INT_NOT_EQUAL(GW(10), -1)
        RETURN
    END_IF
    SET_VALUE(GF(0), 0)
    CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_small), 0)
    SET_VALUE(GW(10), LW(0))
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    RETURN
EVT_END()

// TODO: write queen_hp_drain_5_evt (global, 0x370 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

// TODO: write queen_hp_drain_3_evt (global, 0x180 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

// TODO: write queen_hp_drain_1_evt (global, 0x180 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(queen_hp_drain_evt)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_144"), 0, -2)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_mode, 0, 0)
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(8), -1)
            CALL_CHILD(queen_hand_1_dead_event)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(9), -1)
            CALL_CHILD(queen_hand_2_dead_event)
        END_IF
    END_INLINE_EVENT
    IF_INT_EQUAL(GW(10), -1)
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_small), 0)
        SET_VALUE(GW(10), LW(0))
        WAIT_MS(1000)
    END_IF
    CALL(evt_eff_softdelete, STRING("m_h_1"))
    CALL(evt_eff_softdelete, STRING("m_h_2"))
    CALL(evt_eff_softdelete, STRING("m_h_3"))
    WAIT_MS(1000)
    CALL(queen_get_aud_num, LW(10))
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HAND_GOOUT1"), 0)
    CALL(queen_hp_drain_1_evt)
    CALL(evt_eff, STRING("m_h_s1_"), STRING("mist2"), 0, 0, -40, 0, 400, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s2_"), STRING("mist2"), 0, 0, -25, 0, 400, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s3_"), STRING("mist2"), 0, 0, -35, 0, 400, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s4_"), STRING("mist2"), 0, 0, -15, 0, 400, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 240, 0)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWENH_HAND_APPEAR2"), 0)
    CALL(evt_snd_sfxon, STRING("SFX_AUDIENCE_PANIC_SCREAM1"), LW(11))
    CALL(queen_hp_drain_2_evt)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(queen_hp_drain_3_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 127)
        LOOP(63)
            SUBTRACT(LW(0), 2)
            CALL(evt_snd_sfx_vol, LW(11), LW(0))
            WAIT_MS(31)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(11))
    END_INLINE_EVENT
    CALL(queen_hp_drain_4_evt)
    IF_INT_GREATER(LW(10), 7)
        SET_VALUE(LW(10), 7)
    END_IF
    IF_INT_GREATER(LW(10), 99)
        SET_VALUE(LW(10), 99)
    END_IF
    CALL(btlevtcmd_GetMaxHp, -2, LW(10))
    CALL(btlevtcmd_RecoverHp, GW(13), 1, LW(10))
    CALL(btlevtcmd_GetPos, GW(13), LW(5), LW(6), LW(7))
    CALL(btlevtcmd_GetHeight, GW(13), LW(8))
    ADD(LW(6), LW(8))
    ADD(LW(7), 30)
    CALL(evt_eff, STRING(""), STRING("recovery"), 4, LW(5), LW(6), LW(7), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_batstage_return_aud_dark_base, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HAND_APPEAR1_2"), 0)
    CALL(queen_hp_drain_5_evt)
    CALL(evt_eff_softdelete, STRING("m_h_s1_"))
    CALL(evt_eff_softdelete, STRING("m_h_s2_"))
    CALL(evt_eff_softdelete, STRING("m_h_s3_"))
    CALL(evt_eff_softdelete, STRING("m_h_s4_"))
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_142_01"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    SET_VALUE(GW(11), 2)
    CALL(btlevtcmd_reset_turn)
    RETURN
EVT_END()

EVT_BEGIN(queen_hp_drain_evt_miss)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_144"), 0, -2)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    SET_VALUE(GW(11), 2)
    CALL(btlevtcmd_reset_turn)
    RETURN
EVT_END()

EVT_BEGIN(queen_first_damage_evt)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_163"), 0, -2)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, -113, 69, 332, -113, 46, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print_battle_party, STRING("stg8_las_164"))
    CALL(btlevtcmd_StartWaitEvent, -4)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 1, 11)
    WAIT_MS(350)
    CALL(evt_msg_print, 2, STRING("stg8_las_165"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(queen_2_hp_msg_evt)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_166"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(queen_4_hp_msg_evt)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 2, STRING("stg8_las_167"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(queen_dead_hp_msg_evt)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 30, 11)
    WAIT_MS(800)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("KRT_D_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("KRT_D_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KRT_S_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_D_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRT_S_1"))
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_D_1"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_D_1"))
    END_IF
    CALL(evt_msg_print, 2, STRING("stg8_las_168"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(queen_phase_event)
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    IF_INT_NOT_EQUAL(GW(11), 0)
        IF_INT_EQUAL(GW(18), 1)
            SET_VALUE(GW(18), 2)
            CALL_CHILD(queen_first_damage_evt)
        END_IF
        CALL(btlevtcmd_GetMaxHp, GW(13), LW(0))
        CALL(btlevtcmd_GetHp, GW(13), LW(1))
        DIVIDE(LW(0), 4)
        IF_INT_LESS(LW(1), LW(0))
            CALL(btlevtcmd_GetUnitWork, GW(13), 2, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), 1)
                CALL(btlevtcmd_SetUnitWork, GW(13), 2, 2)
                CALL_CHILD(queen_4_hp_msg_evt)
            END_IF
        END_IF
        CALL(btlevtcmd_GetMaxHp, GW(13), LW(0))
        CALL(btlevtcmd_GetHp, GW(13), LW(1))
        DIVIDE(LW(0), 2)
        IF_INT_LESS(LW(1), LW(0))
            CALL(btlevtcmd_GetUnitWork, GW(13), 2, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), 0)
                CALL(btlevtcmd_SetUnitWork, GW(13), 2, 1)
                CALL_CHILD(queen_2_hp_msg_evt)
            END_IF
        END_IF
    END_IF
    SWITCH(GW(11))
        CASE_EQUAL(0)
        IF_INT_GREATER(GW(12), 200)
            CALL(queen_get_aud_num, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 1)
                SET_VALUE(GW(11), 3)
                SET_VALUE(GW(14), 1)
                CALL_CHILD(queen_hp_drain_evt)
            ELSE
                CALL_CHILD(queen_hp_drain_evt_miss)
            END_IF
            CALL_CHILD(queen_last_attack_message_evt)
            BREAK_SWITCH
        END_IF
        IF_INT_GREATER(GW(12), 100)
            CALL_CHILD(queen_next_attack_message_evt)
            BREAK_SWITCH
        END_IF
        IF_INT_GREATER(GW(12), 0)
            CALL_CHILD(queen_first_attack_message_evt)
            BREAK_SWITCH
        END_IF
        CASE_EQUAL(1)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
        IF_INT_EQUAL(LW(0), 1)
            CALL_CHILD(queen_hand_big_life_check_evt)
        END_IF
        CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_sub_random, 100, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), 50)
                CALL_CHILD(queen_change_hand_small_evt)
                BREAK_SWITCH
            END_IF
        END_IF
        CASE_EQUAL(2)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
        IF_INT_EQUAL(LW(0), 1)
            CALL_CHILD(queen_hand_small_life_check_evt)
        END_IF
        CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_sub_random, 100, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), 50)
                CALL_CHILD(queen_change_hand_big_evt)
                BREAK_SWITCH
            END_IF
        END_IF
        CASE_EQUAL(3)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108868)
        IF_INT_EQUAL(LW(0), 1)
            SET_VALUE(GW(11), 2)
        END_IF
    END_SWITCH
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_black_thunder)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_thunder_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_thunder_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_thunder_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
    WAIT_MS(500)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), -33)
    ADD(LW(1), 82)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 4, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_EYE_SHINE1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_2"))
    WAIT_MS(700)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(queen_thunder, LW(0), LW(1), LW(2))
    WAIT_MS(80)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_thunder_attack), 256, LW(5))
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
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_thunder_attack))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_thunder_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_thunder_attack), 256, LW(5))
    LABEL(95)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_charge)
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_dark_charge_attack))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_2"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_A_3"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_A_3"))
    END_IF
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 0, 1000)
    END_INLINE_EVENT
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    ADD(LW(1), LW(5))
    ADD(LW(2), 30)
    CALL(evt_eff, STRING(""), STRING("charge"), 2, LW(0), LW(1), LW(2), FLOAT(3.0f), 60, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_DARK_POWER1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_S_1"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_S_1"))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, PTR(&weapon_peach_dark_charge_attack), 256, LW(5))
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_black_wave)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_wave_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_wave_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_wave_attack))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    IF_INT_NOT_EQUAL(GW(10), -1)
        CALL_CHILD(queen_hand_small_dead_event)
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_1), 0)
        SET_VALUE(GW(8), LW(0))
        CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_shadow_hand_2), 0)
        SET_VALUE(GW(9), LW(0))
        WAIT_MS(1500)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_L_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_T_3"))
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_A_3"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_A_3"))
    END_IF
    WAIT_MS(1000)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_3"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), -33)
    ADD(LW(1), 82)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 4, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_EYE_SHINE1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("m_q2"), STRING("queen2"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_DARK_WAVE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1500)
        LOOP(3)
            CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_DARK_WAVE2"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(1000)
        END_LOOP
        WAIT_MS(500)
        CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_DARK_WAVE3"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(500)
        CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_DARK_WAVE3"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 0, 1000)
    END_INLINE_EVENT
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 120, 0)
    WAIT_MS(3500)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 120, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_2"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_A_4"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_A_4"))
    END_IF
    WAIT_MS(1500)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_S_1"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_S_1"))
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_wave_attack))
    SET_VALUE(LF(0), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_wave_attack), 256, LW(5))
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
        GOTO(95)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(95)
    LABEL(91)
    CALL(evt_btl_camera_set_mode, 0, 0)
    IF_INT_EQUAL(LF(0), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_wave_attack))
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_wave_attack), 256, LW(5))
    LABEL(95)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(98)
    END_IF
    GOTO(0)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(99)
    IF_INT_NOT_EQUAL(GW(11), 0)
        SET_VALUE(GW(11), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_black_energy)
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_2"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_A_3"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_A_3"))
    END_IF
    WAIT_MS(750)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    IF_INT_NOT_EQUAL(GW(8), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(8), 1, STRING("KRT_S_1"))
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        CALL(btlevtcmd_AnimeChangePose, GW(9), 1, STRING("KRT_S_1"))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 0, 1000)
    END_INLINE_EVENT
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    ADD(LW(1), LW(5))
    ADD(LW(1), -15)
    CALL(evt_eff, STRING(""), STRING("charge"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 60, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_DARK_POWER1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_energy_attack))
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, PTR(&weapon_peach_black_energy_attack), 256, LW(5))
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_bress1)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_sigh_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_sigh_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_sigh_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
    WAIT_MS(500)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), -33)
    ADD(LW(1), 82)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 4, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_EYE_SHINE1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_2"))
    CALL(evt_eff, STRING(""), STRING("toiki"), 0, 73, 112, 0, 180, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_BREATH1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(2500)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_BREATH2"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_3"))
    WAIT_MS(80)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_sigh_attack))
    SET_VALUE(LF(0), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_sigh_attack), 256, LW(5))
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
        GOTO(95)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(95)
    LABEL(91)
    CALL(evt_btl_camera_set_mode, 0, 0)
    IF_INT_EQUAL(LF(0), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_sigh_attack))
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_sigh_attack), 256, LW(5))
    LABEL(95)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(98)
    END_IF
    GOTO(0)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_bress2)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_temptation_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_temptation_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_temptation_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
    WAIT_MS(500)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), -33)
    ADD(LW(1), 82)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 4, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_EYE_SHINE1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_2"))
    CALL(evt_eff, STRING(""), STRING("toiki"), 1, 73, 112, 0, 180, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_BREATH1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(2500)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_BREATH2"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_3"))
    WAIT_MS(80)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_temptation_attack))
    SET_VALUE(LF(0), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_temptation_attack), 256, LW(5))
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
        GOTO(95)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(95)
    LABEL(91)
    CALL(evt_btl_camera_set_mode, 0, 0)
    IF_INT_EQUAL(LF(0), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_temptation_attack))
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_temptation_attack), 256, LW(5))
    LABEL(95)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(98)
    END_IF
    GOTO(0)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_bress3)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_poison_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_poison_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_poison_attack))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A_1"))
    WAIT_MS(500)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), -33)
    ADD(LW(1), 82)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 4, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_EYE_SHINE1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_2"))
    CALL(evt_eff, STRING(""), STRING("toiki"), 2, 73, 112, 0, 180, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_BREATH1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(2500)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_BREATH2"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_A2_3"))
    WAIT_MS(80)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_S_1"))
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_poison_attack))
    SET_VALUE(LF(0), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_poison_attack), 256, LW(5))
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
        GOTO(95)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(95)
    LABEL(91)
    CALL(evt_btl_camera_set_mode, 0, 0)
    IF_INT_EQUAL(LF(0), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_poison_attack))
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_poison_attack), 256, LW(5))
    LABEL(95)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(98)
    END_IF
    GOTO(0)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(queen_attack_event)
    IF_INT_EQUAL(GW(11), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 3)
            CALL_CHILD(queen_attack_black_thunder)
            CASE_EQUAL(1)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 3)
            CALL_CHILD(queen_attack_black_energy)
            CASE_EQUAL(3)
            CALL(btlevtcmd_DrawLots, LW(0), 4, 100, 20, 30, 30)
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL_CHILD(queen_attack_black_thunder)
                CASE_EQUAL(1)
                CALL_CHILD(queen_attack_bress1)
                CASE_EQUAL(2)
                CALL_CHILD(queen_attack_bress2)
                CASE_EQUAL(3)
                CALL_CHILD(queen_attack_bress3)
            END_SWITCH
        END_SWITCH
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        CALL_CHILD(queen_attack_black_thunder)
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
        CALL_CHILD(queen_attack_charge)
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 3)
        CALL_CHILD(queen_attack_black_wave)
        CASE_EQUAL(3)
        CALL(btlevtcmd_DrawLots, LW(0), 6, 100, 50, 30, 20, 30, 30)
        CALL(btlevtcmd_CheckStatus, -2, 14, LW(1))
        IF_INT_EQUAL(LW(1), 1)
            IF_INT_EQUAL(LW(0), 1)
                SET_VALUE(LW(0), 0)
            END_IF
        END_IF
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL_CHILD(queen_attack_black_thunder)
            CASE_EQUAL(1)
            CALL_CHILD(queen_attack_black_energy)
            CASE_EQUAL(2)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
            CALL_CHILD(queen_attack_charge)
            CASE_EQUAL(3)
            CALL_CHILD(queen_attack_bress1)
            CASE_EQUAL(4)
            CALL_CHILD(queen_attack_bress2)
            CASE_EQUAL(5)
            CALL_CHILD(queen_attack_bress3)
        END_SWITCH
    END_SWITCH
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(queen_damage_event)
    CALL(btlevtcmd_GetDamageCode, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 22)
        ADD(GW(12), 1)
        IF_INT_EQUAL(GW(18), 0)
            SET_VALUE(GW(18), 1)
        END_IF
        BEGIN_INLINE_EVENT
            LOOP(5)
                CALL(btlevtcmd_AddPartsPos, -2, 2, 2, 0, 0)
                WAIT_MS(70)
                CALL(btlevtcmd_AddPartsPos, -2, 2, -2, 0, 0)
                WAIT_MS(70)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(queen_dead_event)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL_CHILD(queen_dead_hp_msg_evt)
    CALL(evt_eff_softdelete, STRING("m_q"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_SetTalkPose, -2, STRING("KRT_S_1"))
        CALL(btlevtcmd_SetStayPose, -2, STRING("KRT_S_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRT_O_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KRT_S_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRT_S_1"))
        CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("KRT_O_2"))
        CALL(btlevtcmd_SetAlpha, -2, 2, 254)
        CALL(btlevtcmd_SetAlpha, -2, 3, 254)
        CALL(btlevtcmd_SetAlpha, -2, 4, 254)
        CALL(btlevtcmd_SetAlpha, -2, 5, 254)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 4, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 5, 16777216)
        SET_VALUE(LW(0), 254)
        LOOP(63)
            CALL(btlevtcmd_SetAlpha, -2, 2, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 3, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 4, LW(0))
            CALL(btlevtcmd_SetAlpha, -2, 5, LW(0))
            SUBTRACT(LW(0), 4)
            WAIT_MS(50)
        END_LOOP
        CALL(btlevtcmd_SetAlpha, -2, 2, 0)
        CALL(btlevtcmd_SetAlpha, -2, 3, 0)
        CALL(btlevtcmd_SetAlpha, -2, 4, 0)
        CALL(btlevtcmd_SetAlpha, -2, 5, 0)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_BPCH_DIE1"), LW(0), LW(1), LW(2), LW(10))
        CALL(evt_eff, STRING(""), STRING("mist"), 6, 80, 50, 20, 0, 1500, 0, 0, 0, 0, 0, 0)
        WAIT_MS(200)
        CALL(evt_eff, STRING(""), STRING("mist"), 6, 100, 30, 10, 0, 1500, 0, 0, 0, 0, 0, 0)
        WAIT_MS(100)
        CALL(evt_eff, STRING(""), STRING("mist"), 6, 60, 40, 0, 0, 1500, 0, 0, 0, 0, 0, 0)
        WAIT_MS(50)
        CALL(evt_eff, STRING(""), STRING("mist"), 6, 70, 10, -20, 0, 1500, 0, 0, 0, 0, 0, 0)
        WAIT_MS(4500)
        SET_VALUE(LW(0), 127)
        LOOP(16)
            SUBTRACT(LW(0), 7)
            CALL(evt_snd_sfx_vol, LW(10), LW(0))
            WAIT_MS(125)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(8), -1)
            CALL_CHILD(queen_hand_1_dead_event)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(9), -1)
            CALL_CHILD(queen_hand_2_dead_event)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(GW(10), -1)
            CALL_CHILD(queen_hand_small_dead_event)
        END_IF
    END_INLINE_EVENT
    LABEL(0)
    WAIT_MS(100)
    IF_INT_NOT_EQUAL(GW(8), -1)
        GOTO(0)
    END_IF
    IF_INT_NOT_EQUAL(GW(9), -1)
        GOTO(0)
    END_IF
    IF_INT_NOT_EQUAL(GW(10), -1)
        GOTO(0)
    END_IF
    IF_INT_NOT_EQUAL(GF(0), 1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_FLOAT_VALUE(LW(1), LW(1))
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    LOOP(50)
        SUBTRACT_FLOAT(LW(1), FLOAT(0.82f))
        SUBTRACT_FLOAT(LW(3), FLOAT(0.82f))
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        SET_FLOAT_VALUE(LW(4), LW(1))
        SUBTRACT_FLOAT(LW(4), LW(3))
        CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(4), LW(2))
        WAIT_MS(10)
    END_LOOP
    CALL(btlevtcmd_KillUnit, -2, 1)
    CALL(evt_btl_camera_set_prilimit, 0)
    RETURN
EVT_END()

EVT_BEGIN(queen_wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 3)
    RETURN
EVT_END()

EVT_BEGIN(queen_entry_event)
    WAIT_MS(2000)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, -113, 69, 332, -113, 46, 0, 15, 11)
    WAIT_MS(550)
    CALL(evt_msg_print_battle_party, STRING("stg8_las_161"))
    CALL(btlevtcmd_StartWaitEvent, -4)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_moveto, 0, 63, 141, 412, 63, 111, 0, 1, 11)
    WAIT_MS(350)
    CALL(evt_msg_print, 2, STRING("stg8_las_162"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(slot_init)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_1_init_event)
    CALL(btlevtcmd_SetBaseRotate, -2, 0, 315, 0)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
        SET_VALUE(LW(0), 0)
        LOOP(15)
            CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
            ADD(LW(0), 16)
            WAIT_MS(5)
        END_LOOP
        CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    END_INLINE_EVENT
    CALL(btlevtcmd_SetEventWait, -2, PTR(queen_hand_1_wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(queen_hand_1_attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(queen_hand_1_damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("m_h_1"), STRING("mist2"), 0, LW(0), 15, LW(2), 50, 0, -1, 0, 0, 0, 0, 0)
    IF_INT_EQUAL(GW(11), 0)
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&queen_muteki_defence))
        CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&queen_muteki_defence_attr))
        CALL(btlevtcmd_SetRegistStatus, -2, PTR(&queen_muteki_regist))
    END_IF
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 65536)
    CALL(btlevtcmd_OffAttribute, -2, 33554432)
    CALL(btlevtcmd_StartWaitEvent, -2)
    SET_VALUE(GF(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_1_press_attack)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_press_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_press_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_press_attack))
    CALL(evt_eff_softdelete, LW(10))
    SET_VALUE(LW(0), 255)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), -16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 0)
    WAIT_MS(500)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    SET_VALUE(LW(1), 0)
    ADD(LW(2), -30)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    SET_VALUE(LW(0), 0)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), 16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, LW(10), STRING("mist2"), 0, LW(0), 15, LW(2), 50, 0, -1, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A1_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_HAND_UP1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(1100)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 15, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_press_attack), 256, LW(5))
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
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_press_attack))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_press_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_press_attack), 256, LW(5))
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(98)
    LABEL(99)
    CALL(evt_eff_softdelete, LW(10))
    SET_VALUE(LW(0), 255)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), -16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    SET_VALUE(LW(0), 0)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), 16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, LW(10), STRING("mist2"), 0, LW(0), 15, LW(2), 50, 0, -1, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_1_drain_attack)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_drain_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_drain_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_drain_attack))
    CALL(evt_eff_softdelete, LW(10))
    SET_VALUE(LW(0), 255)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), -16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 0)
    WAIT_MS(500)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    ADD(LW(2), -30)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    SET_VALUE(LW(0), 0)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), 16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, LW(10), STRING("mist2"), 0, LW(0), 15, LW(2), 50, 0, -1, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_A2_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWEN_DRAIN1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        CALL(queen_drain, LW(0), LW(1), LW(2))
        WAIT_MS(600)
        CALL(queen_drain, LW(0), LW(1), LW(2))
        WAIT_MS(600)
        CALL(queen_drain, LW(0), LW(1), LW(2))
        WAIT_MS(600)
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_drain_attack), 256, LW(5))
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
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_drain_attack))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_drain_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_drain_attack), 256, LW(5))
    CALL(btlevtcmd_GetHpDamage, LW(3), LW(0))
    CALL(btlevtcmd_RecoverHp, GW(13), 1, LW(0))
    CALL(btlevtcmd_GetPos, GW(13), LW(5), LW(6), LW(7))
    CALL(btlevtcmd_GetHeight, GW(13), LW(8))
    ADD(LW(6), LW(8))
    ADD(LW(7), 30)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(5), LW(6), LW(7), LW(0), 0, 0, 0, 0, 0, 0, 0)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(98)
    LABEL(99)
    CALL(evt_eff_softdelete, LW(10))
    SET_VALUE(LW(0), 255)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), -16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRT_S_1"))
    SET_VALUE(LW(0), 0)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
        ADD(LW(0), 16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, LW(10), STRING("mist2"), 0, LW(0), 15, LW(2), 50, 0, -1, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_1_attack_event)
    CALL(btlevtcmd_GetUnitWork, GW(13), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_DrawLots, LW(0), 2, 100, 20)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(10), STRING("m_h_1"))
        CALL_CHILD(queen_hand_1_press_attack)
        CASE_EQUAL(1)
        SET_VALUE(LW(10), STRING("m_h_1"))
        CALL_CHILD(queen_hand_1_drain_attack)
    END_SWITCH
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_1_damage_event)
    IF_INT_EQUAL(GW(18), 0)
        SET_VALUE(GW(18), 1)
    END_IF
    ADD(GW(12), 1)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_1_dead_event)
    SET_VALUE(LW(0), 255)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, GW(8), 1, LW(0))
        ADD(LW(0), -16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, GW(8), 1, 0)
    CALL(evt_eff_softdelete, STRING("m_h_1"))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_ClearAllStatus, GW(8))
    CALL(btlevtcmd_KillUnit, GW(8), 0)
    SET_VALUE(GW(8), -1)
    SET_VALUE(GF(1), 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_1_wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_2_init_event)
    CALL(btlevtcmd_SetBaseScale, -2, FLOAT(-1.1f), FLOAT(1.1f), FLOAT(1.1f))
    CALL(btlevtcmd_SetBaseRotate, -2, 0, 315, 0)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
        SET_VALUE(LW(0), 0)
        LOOP(15)
            CALL(btlevtcmd_SetAlpha, -2, 1, LW(0))
            ADD(LW(0), 16)
            WAIT_MS(5)
        END_LOOP
        CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    END_INLINE_EVENT
    CALL(btlevtcmd_SetEventWait, -2, PTR(queen_hand_2_wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(queen_hand_2_attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(queen_hand_2_damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("m_h_2"), STRING("mist2"), 0, LW(0), 15, LW(2), 50, 0, -1, 0, 0, 0, 0, 0)
    IF_INT_EQUAL(GW(11), 0)
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&queen_muteki_defence))
        CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&queen_muteki_defence_attr))
        CALL(btlevtcmd_SetRegistStatus, -2, PTR(&queen_muteki_regist))
    END_IF
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 65536)
    CALL(btlevtcmd_OffAttribute, -2, 33554432)
    CALL(btlevtcmd_StartWaitEvent, -2)
    SET_VALUE(GF(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_2_attack_event)
    CALL(btlevtcmd_GetUnitWork, GW(13), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_DrawLots, LW(0), 2, 100, 20)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(10), STRING("m_h_2"))
        CALL_CHILD(queen_hand_1_press_attack)
        CASE_EQUAL(1)
        SET_VALUE(LW(10), STRING("m_h_2"))
        CALL_CHILD(queen_hand_1_drain_attack)
    END_SWITCH
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_2_damage_event)
    IF_INT_EQUAL(GW(18), 0)
        SET_VALUE(GW(18), 1)
    END_IF
    ADD(GW(12), 1)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_2_dead_event)
    SET_VALUE(LW(0), 255)
    LOOP(15)
        CALL(btlevtcmd_SetAlpha, GW(9), 1, LW(0))
        ADD(LW(0), -16)
        WAIT_MS(5)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, GW(9), 1, 0)
    CALL(evt_eff_softdelete, STRING("m_h_2"))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_ClearAllStatus, GW(9))
    CALL(btlevtcmd_KillUnit, GW(9), 0)
    SET_VALUE(GW(9), -1)
    SET_VALUE(GF(1), 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_2_wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: write queen_hand_small_entry_evt (global, 0x370 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(queen_hand_small_init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(queen_hand_small_wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(queen_hand_small_attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(queen_hand_small_damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(evt_eff, STRING("m_h_s1"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s2"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s3"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HAND_APPEAR1_2"), 0)
    CALL(queen_hand_small_entry_evt)
    IF_INT_EQUAL(GW(11), 0)
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&queen_muteki_defence))
        CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(&queen_muteki_defence_attr))
        CALL(btlevtcmd_SetRegistStatus, -2, PTR(&queen_muteki_regist))
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    SET_VALUE(GF(0), 1)
    RETURN
EVT_END()

// TODO: write queen_hand_small_drag_2_evt (global, 0x180 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

// TODO: write queen_hand_small_drag_3_evt (global, 0x370 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(queen_hand_small_drag_attack)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_dark_drag_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_dark_drag_attack))
    CALL(evt_eff_softdelete, STRING("m_h_s1"))
    CALL(evt_eff_softdelete, STRING("m_h_s2"))
    CALL(evt_eff_softdelete, STRING("m_h_s3"))
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HAND_GOOUT1"), 0)
    CALL(queen_hand_small_drag_2_evt)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BOSS_QWENH_HAND_APPEAR1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(queen_hand_small_drag_1_evt)
    END_INLINE_EVENT
    CALL(evt_eff, STRING("m_h_s1_"), STRING("mist2"), 0, -100, 15, 0, 100, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s2_"), STRING("mist2"), 0, -100, 15, 0, 100, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s3_"), STRING("mist2"), 0, -100, 15, 0, 100, 0, -1, 0, 0, 0, 0, 0)
    WAIT_MS(1250)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 256, LW(5))
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
    WAIT_MS(2250)
    CALL(evt_eff_softdelete, STRING("m_h_s1_"))
    CALL(evt_eff_softdelete, STRING("m_h_s2_"))
    CALL(evt_eff_softdelete, STRING("m_h_s3_"))
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_dark_drag_attack))
    CALL(battle_evt_majo_disp_off, LW(3), LW(4), 0, 0)
    CALL(btlevtcmd_SetScale, LW(3), 0, 0, 0)
    CALL(btlevtcmd_GetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 0, LW(5))
    WAIT_MS(750)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 0, LW(5))
    WAIT_MS(750)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 0, LW(5))
    WAIT_MS(750)
    CALL(evt_eff_softdelete, STRING("m_h_s1_"))
    CALL(evt_eff_softdelete, STRING("m_h_s2_"))
    CALL(evt_eff_softdelete, STRING("m_h_s3_"))
    CALL(btlevtcmd_GetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetScale, LW(3), 1, 1, 1)
    CALL(battle_evt_majo_disp_on, LW(3), LW(4), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_dark_drag_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_last_attack), 256, LW(5))
    WAIT_MS(750)
    LABEL(95)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_eff, STRING("m_h_s1"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s2"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s3"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HAND_APPEAR1_2"), 0)
    CALL(queen_hand_small_drag_3_evt)
    RETURN
EVT_END()

// TODO: write queen_hand_small_1000hand_2_evt (global, 0x370 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(queen_hand_small_1000hand_attack)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_peach_black_1000hand_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_peach_black_1000hand_attack), LW(3), LW(4))
    CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subsetevt_confuse_flustered)
        RETURN
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_peach_black_1000hand_attack))
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 180, 0)
    CALL(evt_eff_softdelete, STRING("m_h_s1"))
    CALL(evt_eff_softdelete, STRING("m_h_s2"))
    CALL(evt_eff_softdelete, STRING("m_h_s3"))
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HANDS_MOVE1"), 0)
    BEGIN_INLINE_EVENT
        CALL(queen_hand_small_1000hand_1_evt)
    END_INLINE_EVENT
    WAIT_MS(400)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_dark_drag_attack), 256, LW(5))
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
    LOOP(4)
        SWITCH(LW(5))
            CASE_OR_EQUAL(4)
            CASE_END
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            CASE_END
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_SWITCH
        WAIT_MS(500)
    END_LOOP
    WAIT_MS(1000)
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_peach_black_1000hand_attack))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_1000hand_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_1000hand_attack), 0, LW(5))
    WAIT_MS(500)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_1000hand_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_1000hand_attack), 0, LW(5))
    WAIT_MS(500)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_1000hand_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_1000hand_attack), 0, LW(5))
    WAIT_MS(500)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_peach_black_1000hand_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_peach_black_1000hand_attack), 256, LW(5))
    WAIT_MS(500)
    WAIT_MS(1000)
    GOTO(98)
    LABEL(95)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_eff, STRING("m_h_s1"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s2"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("m_h_s3"), STRING("mist2"), 0, 100, 15, 0, 150, 0, -1, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HAND_APPEAR1_2"), 0)
    CALL(queen_hand_small_1000hand_2_evt)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_small_attack_event)
    CALL(btlevtcmd_GetUnitWork, GW(13), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_DrawLots, LW(0), 2, 60, 40)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL_CHILD(queen_hand_small_1000hand_attack)
        CASE_EQUAL(1)
        CALL_CHILD(queen_hand_small_drag_attack)
    END_SWITCH
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_small_damage_event)
    IF_INT_EQUAL(GW(18), 0)
        SET_VALUE(GW(18), 1)
    END_IF
    ADD(GW(12), 1)
    CALL(btlevtcmd_GetDamageCode, -2, LW(0))
    BEGIN_INLINE_EVENT
        LOOP(5)
            CALL(queen_hand_small_1000hand_set_offset_evt, 2, 0, 0)
            WAIT_MS(70)
            CALL(queen_hand_small_1000hand_set_offset_evt, -2, 0, 0)
            WAIT_MS(70)
        END_LOOP
    END_INLINE_EVENT
END_IF
SET_VALUE(LW(10), -2)
SET_VALUE(LW(11), 1)
CALL_CHILD(btldefaultevt_Damage)
CALL(evt_btl_camera_set_mode, 0, 0)
RETURN
EVT_END()

// TODO: write queen_hand_small_dead_evt (global, 0x180 bytes) here:
// its asm is in build/G8MJ01/las/asm/unit/unit_boss_black_peach.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(queen_hand_small_dead_event)
    CALL(evt_eff_softdelete, STRING("m_h_s1"))
    CALL(evt_eff_softdelete, STRING("m_h_s2"))
    CALL(evt_eff_softdelete, STRING("m_h_s3"))
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_QWEN_HAND_GOOUT1"), 0)
    CALL(queen_hand_small_dead_evt)
    WAIT_FRAMES(40)
    CALL(btlevtcmd_ClearAllStatus, GW(10))
    CALL(btlevtcmd_KillUnit, GW(10), 0)
    SET_VALUE(GW(10), -1)
    SET_VALUE(GF(1), 1)
    RETURN
EVT_END()

EVT_BEGIN(queen_hand_small_wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
#define DEF_QUEEN_HAND_SMALL_SCROLL_EVT(name, startval, limitval)                                        \
    USER_FUNC(name) {                                                                                     \
        if (isFirstCall) {                                                                                \
            HAND->state = 0;                                                                              \
        }                                                                                                 \
        switch (HAND->state) {                                                                            \
        case 0:                                                                                           \
            HAND->phase = startval;                                                                       \
            HAND->state++;                                                                                \
        case 1:                                                                                           \
            if (HAND->phase <= limitval) {                                                                \
                HAND->state++;                                                                            \
                break;                                                                                    \
            }                                                                                             \
            HAND->phase -= 1.0f;                                                                          \
            extGetPosePtr();                                                                              \
            {                                                                                             \
                s32 i;                                                                                    \
                HandElem* elem = HAND->elems;                                                             \
                for (i = 0; i < HAND->count; i++) {                                                       \
                    elem->posY = HAND->phase * elem->speed - (f32)elem->stagger;                            \
                    elem++;                                                                               \
                }                                                                                         \
            }                                                                                             \
            break;                                                                                        \
        case 2:                                                                                           \
        case 0x63:                                                                                        \
            {                                                                                             \
                s32 i;                                                                                    \
                ExtPose* pose = extGetPosePtr();                                                          \
                for (i = 0; i < HAND->count; i++) {                                                       \
                    pose->flags &= ~EXT_POSE_VISIBLE;                                                     \
                    pose++;                                                                               \
                }                                                                                         \
            }                                                                                             \
            return EVT_RETURN_YIELD;                                                                      \
        default:                                                                                          \
            break;                                                                                        \
        }                                                                                                 \
        return EVT_RETURN_BLOCK;                                                                          \
    }
#define DEF_QUEEN_HAND_SMALL_SETUP_EVT(name)                                                             \
    USER_FUNC(name) {                                                                                     \
        if (isFirstCall) {                                                                                \
            HAND->state = 0;                                                                              \
        }                                                                                                 \
        switch (HAND->state) {                                                                            \
        case 0:                                                                                           \
            {                                                                                             \
                s32 i;                                                                                    \
                ExtPose* pose;                                                                            \
                HandElem* elem;                                                                           \
                HAND->phase = -80.0f;                                                                     \
                pose = extGetPosePtr();                                                                   \
                elem = HAND->elems;                                                                       \
                for (i = 0; i < HAND->count; i++, pose++, elem++) {                                         \
                    pose->flags |= EXT_POSE_VISIBLE;                                                      \
                    pose->flags &= ~EXT_POSE_BILLBOARD;                                                   \
                    PSMTXIdentity(*pose->matrix);                                                          \
                    elem->stagger = irand(0x14);                                                            \
                    elem->speed = 1.0f + (f32)irand(0xc8) / 100.0f;                                       \
                    elem->posX = (f32)((i % 10) * 0x14 + irand(0xf));                                     \
                    elem->posY = HAND->phase - (f32)elem->stagger;                                          \
                    {                                                                                     \
                        s32 v = irand(0xf) + (i / 10) * -0xf;                                             \
                        elem->posZ = (f32)(v + 0x46);                                                     \
                    }                                                                                     \
                    pose->heading = (f32)(irand(0x14) - 0xa);                                             \
                    ((f32*)pose->matrix)[0] = 50.0f / (50.0f + (f32)irand(0x32));                         \
                    ((f32*)pose->matrix)[5] = 50.0f / (50.0f + (f32)irand(0x32));                         \
                    if (elem->mirror != 0) {                                                              \
                        ((f32*)pose->matrix)[0] *= -1.0f;                                                 \
                    }                                                                                     \
                }                                                                                         \
                HAND->state++;                                                                            \
            }                                                                                             \
        case 1:                                                                                           \
            if (HAND->phase >= 0.0f) {                                                                    \
                HAND->state++;                                                                            \
                break;                                                                                    \
            }                                                                                             \
            HAND->phase += 1.0f;                                                                          \
            extGetPosePtr();                                                                              \
            {                                                                                             \
                HandElem* elem = HAND->elems;                                                             \
                s32 i;                                                                                    \
                for (i = 0; i < HAND->count; i++) {                                                       \
                    elem->posY = HAND->phase * elem->speed - (f32)elem->stagger;                            \
                    elem++;                                                                               \
                }                                                                                         \
            }                                                                                             \
            break;                                                                                        \
        case 2:                                                                                           \
        case 0x63:                                                                                        \
            return EVT_RETURN_YIELD;                                                                      \
        default:                                                                                          \
            break;                                                                                        \
        }                                                                                                 \
        return EVT_RETURN_BLOCK;                                                                          \
    }
static void pose_main(void) {
    ExtPose* pose;
    HandElem* elem;
    s32 i;
    pose = extGetPosePtr();
    elem = HAND->elems;
    i = 0;
    for (i = 0; i < HAND->count; i++, pose++, elem++) {
        HandPose* hp = &HAND->poses[elem->poseIdx];
        s32 unitId;
        f32 f1 = (f32)OSTicksToMilliseconds((u64)(gp->renderTime - elem->startTime));
        f1 = (f32)gp->framerate * f1 / 1000.0f / hp->duration;
        if (f1 >= 1.0f) {
            elem->startTime = gp->renderTime;
            f1 -= 1.0f;
        }
        pose->texId = hp->firstFrame + (s32)((hp->frameCount - 1) * f1);
        unitId = evtGetValue(NULL, 0xFD050F8A);
        if (unitId == -1) {
            ((f32*)pose->matrix)[3] = elem->posX;
            ((f32*)pose->matrix)[7] = elem->posY;
            ((f32*)pose->matrix)[11] = elem->posZ;
        } else {
            BattleUnit* unit;
            f32 x, y, z, hx, hy, hz;
            BattleGetUnitPtr(_battleWorkPointer, unitId);
            unit = BattleGetUnitPtr(_battleWorkPointer, unitId);
            BtlUnit_GetPos(unit, &x, &y, &z);
            unit = BattleGetUnitPtr(_battleWorkPointer, unitId);
            BtlUnit_GetHomePos(unit, &hx, &hy, &hz);
            ((f32*)pose->matrix)[3] = elem->posX + x - hx;
            ((f32*)pose->matrix)[7] = elem->posY + y - hy;
            ((f32*)pose->matrix)[11] = elem->posZ + z - hz;
        }
    }
}

// The `prod` local and `#pragma opt_propagation off` are both needed: with propagation on MWCC folds
// the product back into the subtraction and materialises &gp before the 10000000 constant, giving
// gp=r31 / constant=r26 instead of the target's gp=r26 / constant=r31.  Keeping the product in its
// own statement (not propagated) makes MWCC materialise the constant first, matching the target.
#pragma opt_propagation off
static void hand_init(void) {
    s32 i;
    ExtPose* pose;
    HandElem* elem;
    hand_wp[0] = (u32)__memAlloc(4, 0x4B0);
    memset(HAND, 0, 0x4B0);
    HAND->count = extGetPoseNum();
    HAND->elems = __memAlloc(4, HAND->count * 0x28);
    memset(HAND->elems, 0, HAND->count * 0x28);
    pose_init();
    pose = extGetPosePtr();
    i = 0;
    elem = HAND->elems;
    for (i = 0; i < HAND->count; i++, pose++, elem++) {
        pose->flags |= 6;
        PSMTXIdentity(*pose->matrix);
        elem->poseIdx = 0;
        {
            OSTime t = (OSTime)irand(0x64);
            OSTime prod = t * (OSTime)10000000;
            elem->startTime = gp->renderTime - prod;
        }
        elem->mirror = irand(1);
    }
}
#pragma opt_propagation reset

void hand_draw(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

// Matching: the original hand_main is `pose_main(); dispEntry(...)`; with -inline auto,deferred
// MWCC inlines pose_main's loop here, producing the target's exact code (including the r22/r26
// callee-saved colouring that an explicitly written-out loop could not reproduce).
void hand_main(void) {
    pose_main();
    dispEntry(4, 0.0f, 1, (DispCallback)hand_draw, 0);
}

USER_FUNC(change_pose) {
    s32 value = evtGetValue(event, event->args[0]);
    HandElem* elem = HAND->elems;
    s32 i;
    for (i = 0; i < HAND->count; i++) {
        elem->poseIdx = value;
        elem->startTime = gp->renderTime;
        elem++;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(queen_thunder) {
    s32* args = event->args;
    f32 a;
    f32 b;
    f32 c;
    Vec pos;
    EffectEntry* eff;
    a = evtGetFloat(event, args[0]);
    b = evtGetFloat(event, args[1]);
    c = evtGetFloat(event, args[2]);
    pos.x = a;
    pos.y = b;
    pos.z = c;
    psndSFXOn_3D((const char*)0xf1e, &pos);
    eff = effPokopiThunderN64Entry(0, 0x28, a + 20.0f + (f32)irand(0x14), 150.0f, c + 20.0f, a, 0.0f, c, 1.0f);
    ((EffPokopiThunderData*)eff->userdata)->tev1R = 0xff;
    ((EffPokopiThunderData*)eff->userdata)->tev1G = 0xa0;
    ((EffPokopiThunderData*)eff->userdata)->tev1B = 0xc0;
    ((EffPokopiThunderData*)eff->userdata)->tev2R = 0x14;
    ((EffPokopiThunderData*)eff->userdata)->tev2G = 0x14;
    ((EffPokopiThunderData*)eff->userdata)->tev2B = 0x14;
    eff = effShockN64Entry(3, a, 0.0f, c, 80.0f, 80.0f);
    ((EffShockData*)eff->userdata)->tev1R = 0x14;
    ((EffShockData*)eff->userdata)->tev1G = 0x14;
    ((EffShockData*)eff->userdata)->tev1B = 0x14;
    ((EffShockData*)eff->userdata)->tev2R = 0xff;
    ((EffShockData*)eff->userdata)->tev2G = 0xa0;
    ((EffShockData*)eff->userdata)->tev2B = 0xc0;
    btl_camera_shake_w(0, 8, 0, 1.0f, 1.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(queen_drain) {
    Vec position;
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    position.x = x;
    position.y = y;
    position.z = z;
    psndSFXOn_3D((const char*)0xF20, &position);
    effStardust2N64Entry(5, 10, 30, x, y, z, 20.0f, 40.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_las_0000af38) {
    BattleBreakSlotGetPtr()->flags |= 2;
    return EVT_RETURN_DONE;
}

USER_FUNC(eff_queen_set_alpha_evt) {
    s32 value = evtGetValue(event, event->args[0]);
    EffectEntry* eff = effNameToPtr("m_q2");
    ((effQueenData*)eff->userdata)->alphaCap = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(peach_hand1000_dark_drag_1_evt) {
    if (isFirstCall) {
        HAND->state = 0;
    }
    switch (HAND->state) {
    case 0:
        {
            s32 i;
            ExtPose* pose;
            HandElem* elem;
            HAND->phase = -80.0f;
            pose = extGetPosePtr();
            elem = HAND->elems;
            for (i = 0; i < HAND->count; i++, pose++, elem++) {
                pose->flags |= EXT_POSE_VISIBLE;
                pose->flags &= ~EXT_POSE_BILLBOARD;
                PSMTXIdentity(*pose->matrix);
                elem->stagger = irand(0x14);
                elem->speed = 1.0f + (f32)irand(0xc8) / 100.0f;
                {
                    s32 v = irand(0xf) + (i % 10) * 0x14;
                    elem->posX = (f32)(v - 0xc8);
                }
                elem->posY = HAND->phase - (f32)elem->stagger;
                {
                    s32 v = irand(0xf) + (i / 10) * -0xf;
                    elem->posZ = (f32)(v + 0x46);
                }
                pose->heading = (f32)(irand(0x14) - 0xa);
                ((f32*)pose->matrix)[0] = 50.0f / (50.0f + (f32)irand(0x32));
                ((f32*)pose->matrix)[5] = 50.0f / (50.0f + (f32)irand(0x32));
                if (elem->mirror != 0) {
                    ((f32*)pose->matrix)[0] *= -1.0f;
                }
            }
            HAND->state++;
        }
    case 1:
        if (HAND->phase >= 0.0f) {
            HAND->state++;
            break;
        }
        HAND->phase += 1.0f;
        extGetPosePtr();
        {
            HandElem* elem = HAND->elems;
            s32 i;
            for (i = 0; i < HAND->count; i++) {
                elem->posY = HAND->phase * elem->speed - (f32)elem->stagger;
                elem++;
            }
        }
        break;
    case 2:
        HAND->frameTimer = 30;
        HAND->state++;
        break;
    case 3:
        if (HAND->frameTimer != 0) {
            HAND->frameTimer--;
        } else {
            if (HAND->phase <= -80.0f) {
                HAND->state++;
            } else {
                HAND->phase -= 1.0f;
                extGetPosePtr();
                {
                    HandElem* elem = HAND->elems;
                    s32 i;
                    for (i = 0; i < HAND->count; i++) {
                        elem->posY = HAND->phase * elem->speed - (f32)elem->stagger;
                        elem++;
                    }
                }
            }
        }
        break;
    case 4:
    case 0x63:
        return EVT_RETURN_YIELD;
    default:
        break;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(queen_kankyaku_delete_evt) {
    s32 i;
    for (i = 0; i < 200; i++) {
        if ((u8)BattleAudience_GetSysCtrl(i)) {
            BattleAudience_Delete(i);
            BattleAudienceAddTargetNum(0.0f, -1.0f);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(queen_get_aud_num) {
    s32* args = event->args;
    s32 num;
    BattleAudience_GetAudienceNum();
    num = BattleAudience_GetAudienceNum();
    evtSetValue(event, args[0], num);
    return EVT_RETURN_DONE;
}

DEF_QUEEN_HAND_SMALL_SCROLL_EVT(queen_hp_drain_1_evt, 0.0f, -80.0f)
USER_FUNC(queen_hp_drain_2_evt) {
    if (isFirstCall) {
        HAND->state = 0;
    }
    switch (HAND->state) {
    case 0:
        {
            s32 i;
            ExtPose* pose;
            HandElem* elem;
            event->userdata[0]++;
            HAND->phase = -110.0f;
            pose = extGetPosePtr();
            elem = HAND->elems;
            for (i = 0; i < HAND->count; i++, pose++, elem++) {
                pose->flags |= EXT_POSE_VISIBLE;
                pose->flags &= ~EXT_POSE_BILLBOARD;
                PSMTXIdentity(*pose->matrix);
                elem->stagger = irand(0x14);
                elem->speed = 1.0f + (f32)irand(0xc8) / 100.0f;
                {
                    s32 v = irand(0xf) + (i % 10) * 0x28;
                    elem->posX = (f32)(v - 0xc8);
                }
                elem->posY = HAND->phase - (f32)elem->stagger;
                {
                    s32 v = irand(0xf) + (i / 10) * -0xf;
                    elem->posZ = (f32)(v + 0x10e);
                }
                pose->heading = (f32)(irand(0x14) - 0xa);
                ((f32*)pose->matrix)[0] = 50.0f / (50.0f + (f32)irand(0x32));
                ((f32*)pose->matrix)[5] = 50.0f / (50.0f + (f32)irand(0x32));
                if (elem->mirror != 0) {
                    ((f32*)pose->matrix)[0] *= -1.0f;
                }
            }
            HAND->state++;
        }
    case 1:
        if (HAND->phase >= -30.0f) {
            HAND->state++;
            break;
        }
        HAND->phase += 1.0f;
        extGetPosePtr();
        {
            HandElem* elem = HAND->elems;
            s32 i;
            for (i = 0; i < HAND->count; i++) {
                elem->posY = HAND->phase * elem->speed - (f32)elem->stagger;
                elem++;
            }
        }
        break;
    case 2:
    case 0x63:
        return EVT_RETURN_YIELD;
    default:
        break;
    }
    return EVT_RETURN_BLOCK;
}

DEF_QUEEN_HAND_SMALL_SCROLL_EVT(queen_hp_drain_3_evt, -30.0f, -110.0f)
USER_FUNC(queen_hp_drain_4_evt) {
    s32 i;
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    switch (event->userdata[0]) {
    case 0:
        event->userdata[1] = 0;
        event->userdata[0]++;
        for (i = 0; i < 200; i++) {
            if ((u8)BattleAudience_GetSysCtrl(i)) {
                BattleAudience_SetAnim(i, 9, 1);
            }
        }
    case 1:
        for (i = 0; i < 200; i++) {
            if ((u8)BattleAudience_GetSysCtrl(i)) {
                f32 x, y, z;
                BattleAudience_GetPosition(i, &x, &y, &z);
                BattleAudience_SetPosition(i, x, y - 1.0f, z);
            }
        }
        event->userdata[1]--;
        if (event->userdata[1] < -80) {
            event->userdata[0]++;
        }
        break;
    case 2:
        for (i = 0; i < 200; i++) {
            if ((u8)BattleAudience_GetSysCtrl(i)) {
                BattleAudience_Delete(i);
                BattleAudienceAddTargetNum(0.0f, -1.0f);
            }
        }
        event->userdata[0] = 0x63;
    case 0x63:
        return EVT_RETURN_YIELD;
    default:
        break;
    }
    return EVT_RETURN_BLOCK;
}

DEF_QUEEN_HAND_SMALL_SETUP_EVT(queen_hp_drain_5_evt)
USER_FUNC(peach_shock_evt) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 b = evtGetValue(event, args[1]);
    s32 c = evtGetValue(event, args[2]);
    EffectEntry* eff = effShockN64Entry(3, (f32)a, (f32)b, (f32)c, 200.0f, 150.0f);
    ((EffShockData*)eff->userdata)->tev1R = 10;
    ((EffShockData*)eff->userdata)->tev1G = 10;
    ((EffShockData*)eff->userdata)->tev1B = 10;
    ((EffShockData*)eff->userdata)->tev2R = irand(50) + 155;
    ((EffShockData*)eff->userdata)->tev2G = irand(50) + 155;
    ((EffShockData*)eff->userdata)->tev2B = irand(50) + 155;
    return EVT_RETURN_DONE;
}

USER_FUNC(slot_init) {
    BattleBreakSlotGetPtr()->flags |= 4;
    return EVT_RETURN_DONE;
}

DEF_QUEEN_HAND_SMALL_SETUP_EVT(queen_hand_small_entry_evt)

DEF_QUEEN_HAND_SMALL_SCROLL_EVT(queen_hand_small_dead_evt, 0.0f, -80.0f)
DEF_QUEEN_HAND_SMALL_SCROLL_EVT(queen_hand_small_drag_4_evt, 0.0f, -80.0f)
DEF_QUEEN_HAND_SMALL_SETUP_EVT(queen_hand_small_drag_3_evt)
DEF_QUEEN_HAND_SMALL_SCROLL_EVT(queen_hand_small_drag_2_evt, 0.0f, -80.0f)
USER_FUNC(queen_hand_small_drag_1_evt) {
    if (isFirstCall) {
        HAND->state = 0;
    }
    switch (HAND->state) {
    case 0:
        {
            s32 i;
            ExtPose* pose;
            HandElem* elem;
            HAND->phase = -80.0f;
            pose = extGetPosePtr();
            elem = HAND->elems;
            for (i = 0; i < HAND->count; i++, pose++, elem++) {
                pose->flags |= EXT_POSE_VISIBLE;
                pose->flags &= ~EXT_POSE_BILLBOARD;
                PSMTXIdentity(*pose->matrix);
                elem->stagger = irand(0x14);
                elem->speed = 1.0f + (f32)irand(0xc8) / 100.0f;
                {
                    s32 v = irand(0xf) + (i % 10) * 0x14;
                    elem->posX = (f32)(v - 0xc8);
                }
                elem->posY = HAND->phase - (f32)elem->stagger;
                {
                    s32 v = irand(0xf) + (i / 10) * -0xf;
                    elem->posZ = (f32)(v + 0x46);
                }
                pose->heading = (f32)(irand(0x14) - 0xa);
                ((f32*)pose->matrix)[0] = 50.0f / (50.0f + (f32)irand(0x32));
                ((f32*)pose->matrix)[5] = 50.0f / (50.0f + (f32)irand(0x32));
                if (elem->mirror != 0) {
                    ((f32*)pose->matrix)[0] *= -1.0f;
                }
            }
            HAND->state++;
        }
    case 1:
        if (HAND->phase >= 0.0f) {
            HAND->state++;
            break;
        }
        HAND->phase += 1.0f;
        extGetPosePtr();
        {
            HandElem* elem = HAND->elems;
            s32 i;
            for (i = 0; i < HAND->count; i++) {
                elem->posY = HAND->phase * elem->speed - (f32)elem->stagger;
                elem++;
            }
        }
        break;
    case 2:
        HAND->frameTimer = 30;
        HAND->state++;
        break;
    case 3:
        if (HAND->frameTimer != 0) {
            HAND->frameTimer--;
            if (HAND->frameTimer == 0) {
                Vec pos;
                pos.x = -100.0f;
                pos.y = 0.0f;
                pos.z = 0.0f;
                psndSFXOn_3D((const char*)0xf3c, &pos);
            }
        } else {
            if (HAND->phase <= -80.0f) {
                HAND->state++;
            } else {
                HAND->phase -= 1.0f;
                extGetPosePtr();
                {
                    HandElem* elem = HAND->elems;
                    s32 i;
                    for (i = 0; i < HAND->count; i++) {
                        elem->posY = HAND->phase * elem->speed - (f32)elem->stagger;
                        elem++;
                    }
                }
            }
        }
        break;
    case 4:
    case 0x63:
        return EVT_RETURN_YIELD;
    default:
        break;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(queen_hand_small_1000hand_1_evt) {
    if (isFirstCall) {
        HAND->state = 0;
    }
    switch (HAND->state) {
    case 0:
        HAND->frameTimer = 0;
        HAND->phase = 0.0f;
        HAND->frameTimer = 0;
        extGetPosePtr();
        {
            s32 i;
            HandElem* elem = HAND->elems;
            for (i = 0; i < HAND->count; i++, elem++) {
                elem->wavePhase = 0;
            }
        }
        HAND->state++;
    case 1:
        if (HAND->frameTimer > 200) {
            HAND->state++;
            break;
        }
        HAND->frameTimer++;
        extGetPosePtr();
        {
            HandElem* elem = HAND->elems;
            s32 i;
            for (i = 0; i < HAND->count; i++, elem++) {
                if ((i % 10) <= HAND->frameTimer / 3) {
                    elem->posX -= 3.0f;
                    { f32 t = (f32)cos((f32)elem->wavePhase / 6.0f); elem->posY -= t * 0.3f; }
                    elem->wavePhase++;
                    if (elem->wavePhase >= 0x870) {
                        elem->wavePhase -= 0x870;
                    }
                }
            }
        }
        break;
    case 2:
    case 0x63:
        return EVT_RETURN_YIELD;
    default:
        break;
    }
    return EVT_RETURN_BLOCK;
}

DEF_QUEEN_HAND_SMALL_SETUP_EVT(queen_hand_small_1000hand_2_evt)
USER_FUNC(queen_hand_small_1000hand_set_offset_evt) {
    s32* args = event->args;
    s32 off18;
    s32 off1C;
    s32 off20;
    s32 i;
    HandElem* elem;
    off18 = evtGetValue(event, args[0]);
    off1C = evtGetValue(event, args[1]);
    off20 = evtGetValue(event, args[2]);
    extGetPosePtr();
    elem = HAND->elems;
    for (i = 0; i < HAND->count; i++) {
        elem->posX += (f32)off18;
        elem->posY += (f32)off1C;
        elem->posZ += (f32)off20;
        elem++;
    }
    return EVT_RETURN_DONE;
}

// clang-format on

#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_04_000097A4_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

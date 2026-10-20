#include "unit_boss_magnum_battender_mkII.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_audience.h"
#include "evt/evt_batstage.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_sub.h"
#include "driver/effdrv.h"
#include "driver/animdrv.h"
#include "system.h"
#include "eff/eff_batten.h"
#include "eff/eff_kemutest.h"
#include "eff/n64/eff_akari_charge_n64.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "eff/n64/eff_kumokumo_n64.h"
#include "manager/evtmgr_cmd.h"
#include "math_trig.h"
#include "pmario_sound.h"
#include <stdlib.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(arm_end);
static USER_FUNC(arm_main);
static USER_FUNC(arm_init2);
static USER_FUNC(arm_init);
static USER_FUNC(eff_dead_delete);
static USER_FUNC(eff_dead);
static USER_FUNC(arm_check);
static USER_FUNC(waku_main);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[4];
static s32 init_event[92];
static s32 waku_event[23];
static s32 damage_event[111];
static s32 dead_event[234];
static s32 entry_event[78];
static s32 attack_event[320];
s32 drill_attack_event[392];
s32 boomerang_attack_event[324];
s32 aud_attack_event[823];
s32 aud_attack_event_sub[139];
static s32 arm_bunri_event[199];
static s32 arm_return_event[151];
static s32 wait_event[6];
BattleUnitKind unit_boss_rocket_punch_mkII;
static BattleVulnerableStatus regist_r;
static BattleDataEntry data_table_r[1];
static BattleUnitKindPart parts_r[1];
static s32 init_event_r[191];
static s32 arm_event[59];
static s32 arm_event_end[35];
static s32 damage_event_r[30];
static s32 attack_event_r[368];
static s32 wait_event_r[18];
EffectEntry* eff_tor[1];
s32 torCount[1];
EffectEntry* eff_jet[2];
s32 jetCount[1];
static u32 list[1];
EffectEntry* eff_suikomi[1];
s32 screamId[1];
s32 unk_aji_bss00000168[1];
u8 unk_aji_bss00000165;
u8 unk_aji_bss00000164;
TargetWork* wp_target[1];
u32 targetSoundId[1];
static EffectEntry* eff_charge[1];
static ArmWork* work[2];

// clang-format off
extend_data(76,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(77,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_magnum_battender_mkII = {
    UNIT_MAGNUS2_0,                 // mUnitType
    "btl_un_magnum_battender_mkii", // name
    70,                             // maxHp
    0,                              // maxFp
    1,                              // mDangerHP
    1,                              // mPerilHP
    84,                             // mLevel
    0,                              // mBonusExp
    0,                              // bonusCoinRolls
    100,                            // bonusCoinRate
    0,                              // baseCoin
    50,                             // field_0x13
    6,                              // field_0x14
    120,                            // width
    160,                            // height
    { 30, 100 },                    // statusIconOffset
    { 0.0f, 80.0f, 0.0f },          // centerOffset
    { 0, 0 },                       // hpGaugeOffset
    { 30.0f, 115.0f, 0.0f },        // talkTogeBaseOffset
    { 60.0f, 0.0f, -10.0f },        // field_0x3C
    { 0.0f, 0.0f, 0.0f },           // burnFlameOffset
    0.0f,                           // field_0x54
    0.0f,                           // field_0x58
    { 52.0f, 0.0f, 0.0f },          // bintaHitOffset
    { 30.0f, 110.0f, 20.0f },       // kissHitOffset
    { 0.0f, 80.0f, 0.0f },          // cutBaseOffset
    120.0f,                         // cutWidth
    160.0f,                         // cutHeight
    0,                              // turnOrder
    0,                              // turnOrderVariance
    -1,                             // swallowRate
    0,                              // swallowAttribute
    100,                            // field_0x8C
    20,                             // itemStealParam
    { 0.0f, 0.0f, 0.0f },           // starPointDispOffset
    "SFX_BOSS_MBTN_DAMAGED1",       // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",         // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",          // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",     // electricDamageSfxName
    0,                              // attributeFlags
    &regist,                        // vulnStatus
    4,                              // numParts
    parts,                          // mParts
    init_event,                     // initEvtScript
    data_table,                     // dataTable
};

static BattleUnitDefense defence = { { 2, 2, 2, 2, 2 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    40,  // stopRate
    40,  // dizzyRate
    0,   // poisonRate
    40,  // confuseRate
    0,   // electricRate
    100, // burnRate
    20,  // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    80,  // defenseDownRate
    40,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    30,  // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table[17] = {
    { 0x01, "MGN_S_1" },
    { 0x02, "MGN_2D_1" },
    { 0x09, "MGN_2D_1" },
    { 0x05, "MGN_X_1" },
    { 0x04, "MGN_X_1" },
    { 0x03, "MGN_X_1" },
    { 0x1C, "MGN_S_1" },
    { 0x1D, "MGN_Q_1" },
    { 0x1E, "MGN_Q_1" },
    { 0x1F, "MGN_S_1" },
    { 0x27, "MGN_D_1" },
    { 0x28, "MGN_W_1" },
    { 0x29, "MGN_W_2" },
    { 0x38, "MGN_S_1" },
    { 0x39, "MGN_S_1" },
    { 0x41, "MGN_T_1" },
    { 0x45, "MGN_S_1" },
};

static BattleDispPose pose_table_no_arm[16] = {
    { 0x01, "MGN_S_2" },
    { 0x02, "MGN_2D_2" },
    { 0x09, "MGN_2D_2" },
    { 0x05, "MGN_X_2" },
    { 0x04, "MGN_X_2" },
    { 0x03, "MGN_X_2" },
    { 0x1C, "MGN_S_2" },
    { 0x1D, "MGN_Q_2" },
    { 0x1E, "MGN_Q_2" },
    { 0x1F, "MGN_S_2" },
    { 0x27, "MGN_D_2" },
    { 0x28, "MGN_W_3" },
    { 0x38, "MGN_S_2" },
    { 0x39, "MGN_S_2" },
    { 0x41, "MGN_T_2" },
    { 0x45, "MGN_S_2" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[4] = {
    {
        1,                              // partNum
        "btl_un_magnum_battender_mkii", // partName
        "c_mb_robo_2b",                 // modelName
        { 0.0f, 0.0f, 0.0f },           // positionOffset
        { 37.0f, 103.0f, 30.0f },       // hitBaseOffset
        { 37.0f, 113.0f, 61.0f },       // hitCursorBaseOffset
        106,                            // field_0x30
        174,                            // field_0x32
        255,                            // baseAlpha
        &defence,                       // defense
        &defence_attr,                  // defenseAttr
        0x00600009,                     // attributes
        0,                              // counterAttributes
        pose_table,                     // poseTable
    },
    {
        2,                              // partNum
        "btl_un_magnum_battender_mkii", // partName
        "c_mb_robo_2b",                 // modelName
        { 0.0f, 0.0f, 0.0f },           // positionOffset
        { 10.0f, 10.0f, 30.0f },        // hitBaseOffset
        { 10.0f, 20.0f, 61.0f },        // hitCursorBaseOffset
        106,                            // field_0x30
        174,                            // field_0x32
        255,                            // baseAlpha
        &defence,                       // defense
        &defence_attr,                  // defenseAttr
        0x03100002,                     // attributes
        0,                              // counterAttributes
        pose_table,                     // poseTable
    },
    {
        3,                              // partNum
        "btl_un_magnum_battender_mkii", // partName
        "c_mb_robo_2b",                 // modelName
        { 0.0f, 0.0f, 0.0f },           // positionOffset
        { 0.0f, 0.0f, 0.0f },           // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },           // hitCursorBaseOffset
        30,                             // field_0x30
        30,                             // field_0x32
        255,                            // baseAlpha
        &defence,                       // defense
        &defence_attr,                  // defenseAttr
        0x12010000,                     // attributes
        0,                              // counterAttributes
        pose_table,                     // poseTable
    },
    {
        4,                              // partNum
        "btl_un_magnum_battender_mkii", // partName
        "c_mb_robo_2b",                 // modelName
        { 0.0f, 0.0f, 0.0f },           // positionOffset
        { 0.0f, 0.0f, 0.0f },           // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },           // hitCursorBaseOffset
        30,                             // field_0x30
        30,                             // field_0x32
        255,                            // baseAlpha
        &defence,                       // defense
        &defence_attr,                  // defenseAttr
        0x13010000,                     // attributes
        0,                              // counterAttributes
        pose_table,                     // poseTable
    },
};

// The original defines these two zero vectors as global .rodata objects; the compiler
// would otherwise pool them with the function literals at the end of .rodata.
#ifdef MUST_MATCH
__declspec(section ".rodata") Vec vec3_aji_00016210 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_aji_0001621c = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_aji_00016210 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_aji_0001621c = { 0.0f, 0.0f, 0.0f };
#endif

BattleWeapon weapon_batten_drill_attack = {
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

BattleWeapon weapon_batten_boomerang_attack = {
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
    { 6, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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

BattleWeapon weapon_batten_aud_attack = {
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

static BattleUnitSetup entry_rocket_punch[1] = {
    { &unit_boss_rocket_punch_mkII, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

EVT_BEGIN(aud_hit_event)
    CALL(evt_audience_move_position_frame, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
    CALL(evt_audience_jump_position_gravity, LW(0), LW(6), LW(7), LW(8), LW(9), LW(10))
    CALL(evt_audience_delete, LW(0))
    CALL(evt_audience_num_updown_in_event, FLOAT(-1.0f))
    RETURN
EVT_END()

EVT_BEGIN(aud_no_hit_event)
    CALL(evt_audience_move_position_frame, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
    CALL(evt_audience_delete, LW(0))
    CALL(evt_audience_num_updown_in_event, FLOAT(-1.0f))
    RETURN
EVT_END()


static EVT_BEGIN(init_event)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MGN_L_1"))
    SPAWN_EVENT(waku_event)
    CALL(arm_init)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetHomePos, -2, 125, 0, 0)
    CALL(btlevtcmd_SetPos, -2, 125, 0, 0)
    CALL(btlevtcmd_SetBaseRotate, -2, 0, -55, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_MBTN_MOVE1"), STRING("SFX_BOSS_MBTN_MOVE1"), 26, 26, 26)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_MBTN_MOVE1"), STRING("SFX_BOSS_MBTN_MOVE1"), 20, 20, 20)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()


static EVT_BEGIN(waku_event)
    LABEL(0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        GOTO(99)
    END_IF
    CALL(waku_main, -2)
    GOTO(0)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_CheckDamagePattern, -2, 10, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
    CALL(btlevtcmd_GetUnitWork, -2, 6, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 14)
            CALL(btlevtcmd_WaitAttackEnd)
            CALL(btlevtcmd_SetUnitWork, -2, 6, 1)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(evt_btl_camera_set_prilimit, 1)
            CALL(evt_btl_camera_set_mode, 1, 3)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 20)
            CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
            WAIT_FRAMES(20)
            CALL(btlevtcmd_SetTalkPoseType, -2, 65)
            CALL(btlevtcmd_SetStayPoseType, -2, 69)
            CALL(evt_msg_print, 2, STRING("stg7_aji_41_06"), 0, -2)
            CALL(evt_btl_camera_set_prilimit, 0)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            WAIT_MS(500)
        END_IF
    END_IF
    RETURN
EVT_END()





static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 70)
    ADD(LW(2), 10)
    CALL(eff_dead, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_SetTalkPoseType, -2, 39)
    CALL(btlevtcmd_SetStayPoseType, -2, 39)
    CALL(evt_msg_print, 2, STRING("stg7_aji_41_07"), 0, -2)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    WAIT_MS(500)
    CALL(arm_check, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_CheckDamageCode, LW(0), 512, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            SET_VALUE(LW(10), LW(0))
            BEGIN_INLINE_EVENT
                CALL_CHILD(subsetevt_dead_core)
                CALL(arm_end, 0)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 16777216)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 33554432)
            END_INLINE_EVENT
        END_IF
    END_IF
    CALL(arm_check, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_CheckDamageCode, LW(0), 512, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            SET_VALUE(LW(10), LW(0))
            BEGIN_INLINE_EVENT
                CALL_CHILD(subsetevt_dead_core)
                CALL(arm_end, 1)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 16777216)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 33554432)
            END_INLINE_EVENT
        END_IF
    END_IF
    SET_VALUE(LW(10), -2)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
    ADD(LW(2), 50)
    ADD(LW(1), 30)
    CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    WAIT_FRAMES(120)
    CALL(btlevtcmd_GetBackItem, LW(10))
    CALL(btlevtcmd_KillUnit, LW(10), 1)
    CALL(eff_dead_delete)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    WAIT_FRAMES(120)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
    WAIT_FRAMES(20)
    CALL(evt_msg_print, 2, STRING("stg7_aji_41_00"), 0, -2)
    CALL(evt_btl_camera_set_moveto, 1, -102, 83, 334, -102, 59, 0, 20, 11)
    WAIT_FRAMES(20)
    CALL(evt_msg_print_battle_party, STRING("stg7_aji_41_01"))
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    WAIT_MS(500)
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseFromTable, -4, LW(0))
    RETURN
EVT_END()


static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_check, 0, LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(arm_check, 1, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_AddUnitWork, -2, 1, -1)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
                IF_INT_LESS_OR_EQUAL(LW(0), 0)
                    CALL_CHILD(arm_return_event)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL_CHILD(drill_attack_event)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_GREATER(LW(0), 63)
            CALL_CHILD(drill_attack_event)
            GOTO(99)
        ELSE
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL_CHILD(arm_bunri_event)
                GOTO(99)
            END_IF
            CALL(btlevtcmd_GetHp, -2, LW(0))
            IF_INT_GREATER(LW(0), 35)
                SET_VALUE(LW(0), 0)
                ADD(LW(0), 40)
                ADD(LW(0), 40)
                SUBTRACT(LW(0), 1)
                CALL(evt_sub_random, LW(0), LW(1))
                SET_VALUE(LW(0), 40)
                IF_INT_LESS(LW(1), LW(0))
                    CALL_CHILD(drill_attack_event)
                    GOTO(99)
                ELSE
                    CALL_CHILD(arm_bunri_event)
                    GOTO(99)
                END_IF
            ELSE
                CALL(btlevtcmd_GetUnitWork, -2, 7, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(get_aud_num, LW(0))
                    IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                        CALL_CHILD(aud_attack_event)
                        GOTO(99)
                    END_IF
                END_IF
                LABEL(10)
                SET_VALUE(LW(0), 0)
                ADD(LW(0), 40)
                ADD(LW(0), 40)
                ADD(LW(0), 25)
                SUBTRACT(LW(0), 1)
                CALL(evt_sub_random, LW(0), LW(1))
                SET_VALUE(LW(0), 40)
                IF_INT_LESS(LW(1), LW(0))
                    CALL_CHILD(drill_attack_event)
                    GOTO(99)
                END_IF
                ADD(LW(0), 40)
                IF_INT_LESS(LW(1), LW(0))
                    CALL_CHILD(arm_bunri_event)
                    GOTO(99)
                END_IF
                ADD(LW(0), 25)
                IF_INT_LESS(LW(1), LW(0))
                    CALL(get_aud_num, LW(0))
                    IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                        CALL_CHILD(aud_attack_event)
                        GOTO(99)
                    END_IF
                END_IF
                GOTO(10)
            END_IF
        END_IF
    ELSE
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_GREATER(LW(0), 35)
            CALL_CHILD(boomerang_attack_event)
            GOTO(99)
        ELSE
            CALL(btlevtcmd_GetUnitWork, -2, 7, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(get_aud_num, LW(0))
                IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                    CALL_CHILD(aud_attack_event)
                    GOTO(99)
                END_IF
            END_IF
            LABEL(20)
            SET_VALUE(LW(0), 0)
            ADD(LW(0), 75)
            ADD(LW(0), 20)
            SUBTRACT(LW(0), 1)
            CALL(evt_sub_random, LW(0), LW(1))
            SET_VALUE(LW(0), 75)
            IF_INT_LESS(LW(1), LW(0))
                CALL_CHILD(boomerang_attack_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 20)
            IF_INT_LESS(LW(1), LW(0))
                CALL(get_aud_num, LW(0))
                IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                    CALL_CHILD(aud_attack_event)
                    GOTO(99)
                END_IF
            END_IF
            GOTO(20)
        END_IF
    END_IF
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()



EVT_BEGIN(drill_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_batten_drill_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_batten_drill_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 5, 1)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_btl_camera_set_prilimit, 1)
        CALL(evt_btl_camera_set_mode, 1, 3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 20)
        CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
        WAIT_FRAMES(20)
        CALL(evt_msg_print, 2, STRING("stg7_aji_41_02"), 0, -2)
        CALL(evt_btl_camera_set_prilimit, 0)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        WAIT_MS(500)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_batten_drill_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_batten_drill_attack))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_DRILL_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A3_1"))
    WAIT_MS(800)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_DRILL_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A3_2"))
    BEGIN_SIBLING_EVENT
        CALL(drill_jet, -2)
    END_SIBLING_EVENT
    WAIT_MS(300)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 300)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 0, 1, 0, -1)
    WAIT_MS(300)
    CALL(btlevtcmd_SetPos, -2, -125, 500, 0)
    CALL(btlevtcmd_SetRotate, -2, 180, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A3_3"))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_DRILL_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, -125, -200, 0, 60, 0, 0, 0, -1)
    END_SIBLING_EVENT
    BEGIN_INLINE_EVENT
        CALL(drill_tor, -2)
    END_INLINE_EVENT
    LOOP(0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(1), 200)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_btl_camera_shake_h, 0, 7, 0, 80, 0)
    SET_VALUE(LW(6), 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(10)
    IF_INT_EQUAL(LW(3), -1)
        GOTO(97)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_drill_attack), 256, LW(5))
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
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_batten_drill_attack))
        SET_VALUE(LW(6), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_drill_attack), 256, LW(5))
    GOTO(95)
    LABEL(95)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(10)
    LABEL(97)
    LOOP(0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(1), -200)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(200)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, 400, LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 60, -1, 0)
    RETURN
EVT_END()

EVT_BEGIN(boomerang_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_batten_boomerang_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_batten_boomerang_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_batten_boomerang_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_batten_boomerang_attack))
    BEGIN_SIBLING_EVENT
        WAIT_MS(900)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_BOOME_DIVIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A6_1"))
    WAIT_MS(1160)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_BOOME_ROUND1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 70)
    ADD(LW(1), 70)
    CALL(btlevtcmd_SetPartsPos, -2, 4, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, 4, 0, 55, 65)
    BEGIN_SIBLING_EVENT
        WAIT_MS(1600)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_BOOME_ACTIVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MGN_A4_2"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 4, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 4, 33554432)
    BEGIN_SIBLING_EVENT
        WAIT_MS(2500)
        CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MGN_A4_3"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPartsPos, -2, 4, LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
    CALL(btlevtcmd_DivePartsPosition, -2, 4, LW(0), LW(1), LW(2), 80, -40, 0, 0, -1)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
    CALL(btlevtcmd_DivePartsPosition, -2, 4, LW(0), LW(1), LW(2), 80, 40, 0, 0, -1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_DivePartsPosition, -2, 4, -400, 120, LW(2), 40, -20, 1, 0, -1)
    END_SIBLING_EVENT
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_BOOME_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(20)
    SET_VALUE(LW(6), 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(10)
    IF_INT_EQUAL(LW(3), -1)
        GOTO(97)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_boomerang_attack), 256, LW(5))
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
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_batten_boomerang_attack))
        SET_VALUE(LW(6), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_boomerang_attack), 256, LW(5))
    GOTO(95)
    LABEL(95)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(10)
    LABEL(97)
    WAIT_MS(700)
    CALL(btlevtcmd_OnPartsAttribute, -2, 4, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 4, 33554432)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(200)
    RETURN
EVT_END()






EVT_BEGIN(aud_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_batten_aud_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_batten_aud_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_btl_camera_set_prilimit, 1)
        CALL(evt_btl_camera_set_mode, 1, 3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 20)
        CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
        WAIT_FRAMES(20)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("MGN_T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("MGN_S_1"))
        ELSE
            CALL(btlevtcmd_SetTalkPose, -2, STRING("MGN_T_2"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("MGN_S_2"))
        END_IF
        CALL(evt_msg_print, 2, STRING("stg7_aji_41_04"), 0, -2)
        CALL(evt_btl_camera_set_prilimit, 0)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        WAIT_MS(500)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_batten_aud_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_batten_aud_attack))
    WAIT_FRAMES(30)
    BEGIN_SIBLING_EVENT
        WAIT_MS(200)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
        WAIT_MS(300)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
        WAIT_MS(300)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_MOTOR_ACTIVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
        WAIT_MS(700)
        CALL(evt_snd_sfxoff, LW(15))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_1"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_1"))
    END_IF
    WAIT_MS(1600)
    BEGIN_SIBLING_EVENT
        CALL(drill_jet, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_2"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_2"))
    END_IF
    WAIT_MS(300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 300)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 0, 1, 0, -1)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_mode, 0, 6)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_batstage_set_aud_dark, 0, 30, 0)
    BEGIN_SIBLING_EVENT
        CALL(aud_jet, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_7"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_7"))
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(2), 150)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 300)
    CALL(btlevtcmd_DivePosition, -2, LW(0), 25, LW(2), 60, 0, 4, 0, -1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_3"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_3"))
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_INHALE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(aud_suikomi, -2)
    BEGIN_SIBLING_EVENT
        CALL(unk_aji_00007668)
    END_SIBLING_EVENT
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_7"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_7"))
    END_IF
    WAIT_MS(300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 300)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 0, 1, 0, -1)
    CALL(aud_jet_end)
    WAIT_MS(300)
    CALL(evt_batstage_return_aud_dark_base, 30, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    BEGIN_SIBLING_EVENT
        CALL(drill_jet, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_2"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_2"))
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 300)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 0, 4, 0, -1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_MOTOR_ACTIVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
        WAIT_MS(600)
        CALL(evt_snd_sfxoff, LW(15))
        WAIT_MS(400)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_CHEST_OPEN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_4"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_4"))
    END_IF
    WAIT_MS(1200)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_btl_camera_set_prilimit, 1)
        CALL(evt_btl_camera_set_mode, 1, 3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 20)
        CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
        WAIT_FRAMES(20)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("MGN_T_3"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("MGN_A2_8"))
        ELSE
            CALL(btlevtcmd_SetTalkPose, -2, STRING("MGN_T_4"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("MGN_A5_8"))
        END_IF
        CALL(evt_msg_print, 2, STRING("stg7_aji_41_05"), 0, -2)
        CALL(btlevtcmd_SetTalkPoseType, -2, 65)
        CALL(btlevtcmd_SetStayPoseType, -2, 69)
        CALL(evt_btl_camera_set_prilimit, 0)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        WAIT_MS(500)
    END_IF
    CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(aud_target_mark, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_5"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_5"))
    END_IF
    SET_VALUE(LW(10), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 8, 0)
    LOOP(10)
        WAIT_MS(250)
        CALL_CHILD(aud_attack_event_sub)
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), -10)
    ADD(LW(1), 78)
    WAIT_MS(250)
    CALL(evt_eff, STRING(""), STRING("machingegun"), 0, LW(0), LW(1), LW(2), 180, FLOAT(8.0f), FLOAT(2.1f), 0, 0, 0, 0, 0)
    WAIT_MS(250)
    CALL(evt_eff, STRING(""), STRING("machingegun"), 0, LW(0), LW(1), LW(2), 170, FLOAT(8.0f), FLOAT(2.1f), 0, 0, 0, 0, 0)
    WAIT_MS(250)
    CALL(evt_eff, STRING(""), STRING("machingegun"), 0, LW(0), LW(1), LW(2), 185, FLOAT(8.0f), FLOAT(2.1f), 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_8"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_8"))
    END_IF
    WAIT_MS(1000)
    BEGIN_SIBLING_EVENT
        WAIT_MS(100)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_CHEST_OPEN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(300)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(300)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_6"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A5_6"))
    END_IF
    WAIT_MS(1000)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(200)
    RETURN
EVT_END()



EVT_BEGIN(aud_attack_event_sub)
    IF_INT_EQUAL(LW(10), 9)
        CALL(btlevtcmd_GetUnitWork, -2, 8, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            GOTO(91)
        END_IF
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_MACHINEGUN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_aud_attack), 256, LW(5))
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
    SET_VALUE(LW(11), LW(10))
    MULTIPLY(LW(11), 2)
    CALL(aud_fire_no_hit, -2, LW(3), LW(11))
    GOTO(95)
    LABEL(91)
    SET_VALUE(LW(11), LW(10))
    MULTIPLY(LW(11), 2)
    CALL(aud_fire_hit, -2, LW(3), LW(11))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_batten_aud_attack))
    IF_INT_EQUAL(LW(10), 9)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_aud_attack), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_aud_attack), 0, LW(5))
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 8, 1)
    GOTO(95)
    LABEL(95)
    ADD(LW(10), 1)
    RETURN
EVT_END()

static EVT_BEGIN(arm_bunri_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_btl_camera_set_prilimit, 1)
        CALL(evt_btl_camera_set_mode, 1, 3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 20)
        CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
        WAIT_FRAMES(20)
        CALL(evt_msg_print, 2, STRING("stg7_aji_41_03"), 0, -2)
        CALL(evt_btl_camera_set_prilimit, 0)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        WAIT_MS(500)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_ACTIVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_1"))
    WAIT_MS(600)
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_STOP1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_2"))
    WAIT_MS(500)
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_3"))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_IGNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_w, 0, 3, 0, 20, 0)
    WAIT_MS(100)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, 0)
    CALL(btlevtcmd_SetUnitWork, LW(3), 1, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(3))
    CALL(btlevtcmd_AnimeChangePoseType, LW(3), 1, 69)
    CALL(btlevtcmd_SpawnUnit, LW(4), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(4), 0, 1)
    CALL(btlevtcmd_SetUnitWork, LW(4), 1, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(4))
    CALL(btlevtcmd_AnimeChangePoseType, LW(4), 1, 69)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_no_arm))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(1500)
    RETURN
EVT_END()

static EVT_BEGIN(arm_return_event)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_6"))
    WAIT_MS(600)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, 2)
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(3))
    CALL(btlevtcmd_AnimeChangePoseType, LW(3), 1, 69)
    CALL(btlevtcmd_SpawnUnit, LW(4), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(4), 0, 3)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(4))
    CALL(btlevtcmd_AnimeChangePoseType, LW(4), 1, 69)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetPos, LW(3), 300, 40, -100)
        CALL(btlevtcmd_DivePosition, LW(3), 30, 100, -40, 30, -10, 0, 0, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetPos, LW(4), 350, 40, 100)
    CALL(btlevtcmd_DivePosition, LW(4), 80, 100, 40, 30, -10, 0, 0, -1)
    CALL(arm_end, 0)
    CALL(arm_end, 1)
    CALL(btlevtcmd_KillUnit, LW(3), 0)
    CALL(btlevtcmd_KillUnit, LW(4), 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_IGNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_w, 0, 3, 0, 20, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_7"))
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_8"))
    WAIT_MS(600)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(500)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

BattleUnitKind unit_boss_rocket_punch_mkII = {
    UNIT_X_PUNCH,               // mUnitType
    "btl_un_rocket_punch_mkii", // name
    5,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    10,                         // field_0x14
    50,                         // width
    34,                         // height
    { 0, 15 },                  // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { 0, -20 },                 // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 25.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 25.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 24.0f, 16.0f, 0.0f },     // kissHitOffset
    { -5.0f, 0.0f, 0.0f },      // cutBaseOffset
    50.0f,                      // cutWidth
    34.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_MBTN_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00000004,                 // attributeFlags
    &regist_r,                  // vulnStatus
    1,                          // numParts
    parts_r,                    // mParts
    init_event_r,               // initEvtScript
    data_table_r,               // dataTable
};

static BattleUnitDefense defence_r = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_r = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_r = {
    0,   // sleepRate
    70,  // stopRate
    70,  // dizzyRate
    100, // poisonRate
    70,  // confuseRate
    100, // electricRate
    100, // burnRate
    70,  // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    85,  // defenseDownRate
    70,  // allergicRate
    0,   // frightRate
    100, // galeForceRate
    100, // fastRate
    70,  // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table_r[14] = {
    { 0x01, "MGN_A1_9" },
    { 0x02, "MGN_A1_4" },
    { 0x09, "MGN_A1_4" },
    { 0x05, "MGN_A1_9" },
    { 0x04, "MGN_A1_9" },
    { 0x03, "MGN_A1_9" },
    { 0x1C, "MGN_A1_9" },
    { 0x1D, "MGN_A1_9" },
    { 0x1E, "MGN_A1_9" },
    { 0x1F, "MGN_A1_9" },
    { 0x27, "MGN_A1_9" },
    { 0x38, "MGN_A1_9" },
    { 0x39, "MGN_A1_9" },
    { 0x45, "MGN_A1_9" },
};

static BattleDataEntry data_table_r[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts_r[1] = {
    {
        1,                          // partNum
        "btl_un_rocket_punch_mkii", // partName
        "c_mb_robo_2b",             // modelName
        { 0.0f, 0.0f, 0.0f },       // positionOffset
        { 0.0f, 10.0f, 0.0f },      // hitBaseOffset
        { 0.0f, 10.0f, 0.0f },      // hitCursorBaseOffset
        106,                        // field_0x30
        174,                        // field_0x32
        255,                        // baseAlpha
        &defence_r,                 // defense
        &defence_attr_r,            // defenseAttr
        0x00600009,                 // attributes
        0x00000002,                 // counterAttributes
        pose_table_r,               // poseTable
    },
};

static BattleWeapon weapon_batten_rocket_attack = {
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
    { 9, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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


static EVT_BEGIN(init_event_r)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_4"))
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_r))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_r))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_r))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetBaseRotate, -2, 0, -90, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(arm_init2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        CALL(arm_init2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_init2, 1)
    END_IF
    IF_INT_EQUAL(LW(0), 3)
        CALL(arm_init2, 1)
    END_IF
    SPAWN_EVENT(arm_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetPos, -2, 30, 100, -40)
        CALL(btlevtcmd_DivePosition, -2, -100, 150, -60, 20, 0, 4, 0, -1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, 350, 110, -80, 30, 50, 0, 0, -1)
        CALL(btlevtcmd_DivePosition, -2, 30, 60, -40, 25, -30, 4, 0, -1)
        CALL(btlevtcmd_SetHomePos, -2, 30, 60, -40)
        CALL(btlevtcmd_StartWaitEvent, -2)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetPos, -2, 80, 100, 40)
        CALL(btlevtcmd_DivePosition, -2, -50, 150, 60, 20, 0, 4, 0, -1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, 300, 120, 80, 30, 50, 0, 0, -1)
        CALL(btlevtcmd_DivePosition, -2, 80, 60, 40, 30, -30, 4, 0, -1)
        CALL(btlevtcmd_SetHomePos, -2, 80, 60, 40)
        CALL(btlevtcmd_StartWaitEvent, -2)
    END_IF
    RETURN
EVT_END()


static EVT_BEGIN(arm_event)
    LABEL(0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL_CHILD(arm_event_end)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(arm_main, -2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        CALL(arm_main, -2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_main, -2, 1)
    END_IF
    IF_INT_EQUAL(LW(0), 3)
        CALL(arm_main, -2, 1)
    END_IF
    GOTO(0)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(arm_event_end)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(arm_end, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        CALL(arm_end, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_end, 1)
    END_IF
    IF_INT_EQUAL(LW(0), 3)
        CALL(arm_end, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_r)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL_CHILD(arm_event_end)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_r)
    CALL(btlevtcmd_AddUnitWork, -2, 1, -1)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_batten_rocket_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_batten_rocket_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(evt_sub_random, 1, LW(6))
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_batten_rocket_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_batten_rocket_attack))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_4"))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_rocket_attack), 256, LW(5))
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
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_GetPos, -2, LW(10), LW(11), LW(12))
            IF_INT_LESS_OR_EQUAL(LW(10), LW(0))
                CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_batten_rocket_attack))
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_rocket_attack), 256, LW(5))
                CALL(evt_btl_camera_shake_w, 0, 3, 0, 10, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    GOTO(95)
    LABEL(95)
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 110)
        SET_VALUE(LW(1), 120)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 10, 4, 0, -1)
        WAIT_FRAMES(5)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), -20)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, -250, 0, LW(2), 20, -10, 0, 0, -1)
    ELSE
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), -200)
        ADD(LW(1), 140)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, -80, 1, 0, -1)
    END_IF
    WAIT_MS(250)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_SetPos, -2, 300, LW(1), -100)
    ELSE
        CALL(btlevtcmd_SetPos, -2, 300, LW(1), 100)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 0, 4, 0, -1)
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_r)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    LABEL(0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

// clang-format on
static USER_FUNC(waku_main) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnitPart* part1;
    BattleUnitPart* part3;
    AnimationPose* pose;
    Vec* positions;
    u8* tbl;
    s32 id;
    s32 poseId;
    s32 group;

    id = evtGetValue(event, *args++);
    animGetPtr();
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    part1 = BtlUnit_GetPartsPtr(unit, 1);
    part3 = BtlUnit_GetPartsPtr(unit, 3);
    poseId = part1->poseId;
    animPoseWorldPositionEvalOn(poseId);
    animPoseGetAnimBaseDataPtr(poseId);
    pose = animPoseGetAnimPosePtr(poseId);
    group = animPoseGetGroupIdx(poseId, "locator17");
    tbl = *(u8**)((u8*)pose + 0x15C);
    positions = (Vec*)(*(u8**)tbl);
    BtlUnit_SetPartsPos(part3, positions[group].x, positions[group].y, positions[group].z);
    {
        f32 yaw = camGetPtr(4)->viewYaw;
        BtlUnit_SetPartsRotate(part3, 0.0f, -yaw, 0.0f);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_check) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 value = evtGetValue(event, args[0]);
    s32 result = -1;
    s32 i;

    for (i = 0; i < 64; i++) {
        unit = wp->units[i];
        if (unit != NULL && (unit->currentKind == UNIT_X_FIST || unit->currentKind == UNIT_X_PUNCH)) {
            if (value == 0) {
                if (unit->work[0] == 0 || unit->work[0] == 2) {
                    result = i;
                    break;
                }
            }
            if (value == 1) {
                if (unit->work[0] == 1 || unit->work[0] == 3) {
                    result = i;
                    break;
                }
            }
        }
    }
    evtSetValue(event, args[1], result);
    return EVT_RETURN_DONE;
}

USER_FUNC(drill_tor) {
    f32 x;
    f32 y;
    f32 z;
    s32 transId = BattleTransID(event, evtGetValue(event, event->args[0]));
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = BattleGetUnitPtr(wp, transId);

    BtlUnit_GetPos(unit, &x, &y, &z);
    if (isFirstCall) {
        eff_tor[0] = effKamekiTornadeN64Entry(0, 1000, 0.0f, -1000.0f, 0.0f, 3.5f);
        torCount[0] = 0;
    }
    if (y <= 300.0f) {
        ((BattenData*)eff_tor[0]->userdata)->position.x = x;
        ((BattenData*)eff_tor[0]->userdata)->position.y = y - 240.0f;
        ((BattenData*)eff_tor[0]->userdata)->position.z = z;
    }
    if (++torCount[0] >= 70) {
        effDelete(eff_tor[0]);
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(drill_jet) {
    f32 x;
    f32 y;
    f32 z;
    s32 i;
    s32 transId = BattleTransID(event, evtGetValue(event, event->args[0]));
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = BattleGetUnitPtr(wp, transId);
    BattenData* data;

    BtlUnit_GetPos(unit, &x, &y, &z);
    if (isFirstCall) {
        for (i = 0; i < 2; i++) {
            eff_jet[i] = effBattenEntry(3, 1000, 0.0f, -1000.0f, 0.0f, 3.0f);
        }
        jetCount[0] = 0;
    }
    data = eff_jet[0]->userdata;
    data->position.x = x - 20.0f;
    data->position.y = y;
    data->position.z = z - 20.0f;
    data = eff_jet[1]->userdata;
    data->position.x = x + 20.0f;
    data->position.y = y;
    data->position.z = z + 20.0f;
    if (++jetCount[0] >= 90) {
        for (i = 0; i < 2; i++) {
            effDelete(eff_jet[i]);
        }
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(aud_jet) {
    Mtx mtx;
    f32 x;
    f32 y;
    f32 z;
    s32 i;
    s32 transId = BattleTransID(event, evtGetValue(event, event->args[0]));
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, transId);
    BattenData* data;

    BtlUnit_GetPos(unit, &x, &y, &z);
    if (isFirstCall) {
        for (i = 0; i < 2; i++) {
            eff_jet[i] = effBattenEntry(3, 1000, 0.0f, -1000.0f, 0.0f, 3.0f);
        }
        jetCount[0] = 0;
    }
    PSMTXRotRad(mtx, 'z', -0.34906584f);
    data = eff_jet[0]->userdata;
    data->position.x = x - 60.0f;
    data->position.y = y + 22.5f;
    data->position.z = z;
    PSMTXCopy(mtx, ((BattenData*)eff_jet[0]->userdata)->matrix);
    data = eff_jet[1]->userdata;
    data->position.x = x - 15.0f;
    data->position.y = y + 22.5f;
    data->position.z = z + 70.0f;
    PSMTXCopy(mtx, ((BattenData*)eff_jet[1]->userdata)->matrix);
    if (jetCount[0] == 1) {
        for (i = 0; i < 2; i++) {
            effDelete(eff_jet[i]);
        }
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(aud_jet_end) {
    jetCount[0] = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(get_aud_num) {
    s32* args = event->args;
    s32 num = BattleAudience_GetAudienceNum();

    evtSetValue(event, args[0], num);
    return EVT_RETURN_DONE;
}

USER_FUNC(aud_suikomi) {
    s32* args = event->args;
    s32 transId = BattleTransID(event, evtGetValue(event, args[0]));
    s32 count = 0;
    s32 temp[200];
    s32 i, found;
    BattleUnit* unit;
    BattleAudienceMember* aud;
    f32 x;
    f32 y;
    f32 z;
    f32 v;
    f32 px;
    f32 py;
    f32 pz;

    if (isFirstCall) {
        found = 0;
        if (list[0] != 0) {
            BattleFree((void*)list[0]);
        }
        list[0] = (u32)BattleAlloc(0x50);
        for (i = 0; i < 200; i++) {
            if ((u8)BattleAudience_GetExist(i) == 1) {
                temp[found] = i;
                found++;
            }
        }
        for (i = 0; i < 1000; i++) {
            s32 a = irand(found);
            s32 b = irand(found);
            s32 tmp = temp[a];
            temp[a] = temp[b];
            temp[b] = tmp;
        }
        for (i = 0; i < 20; i++) {
            aud = BattleAudienceGetPtr(temp[i]);
            aud->flags |= 0x10;
            BattleAudience_SetAnim(temp[i], 9, 0);
            aud->interpFramesTotal = 60 + i * 3;
            aud->counter = 0;
            aud->mRotation.y = 0.0f;
            ((s32*)list[0])[i] = temp[i];
        }
        unit = BattleGetUnitPtr(_battleWorkPointer, transId);
        BtlUnit_GetPos(unit, &x, &y, &z);
        eff_suikomi[0] = effKumokumoN64Entry(2, 120, x - 10.0f, y + 10.0f, z, -40.0f, 1.0f, 1.5f);
        screamId[0] = psndSFXOn("SFX_AUDIENCE_PANIC_SCREAM1");
    }
    unit = BattleGetUnitPtr(_battleWorkPointer, transId);
    BtlUnit_GetPos(unit, &px, &py, &pz);
    py += 5.0f;
    for (i = 0; i < 200; i++) {
        s32 j;
        for (j = 0; j < 20; j++) {
            if (i != ((s32*)list[0])[j]) {
                continue;
            }
            aud = BattleAudienceGetPtr(i);
            aud->mRotation.y += 19.0f;
            if (aud->interpFramesLeft < aud->interpFramesTotal) {
                aud->interpFramesLeft++;
            } else {
                aud->flags &= ~4;
                count++;
                aud->flags |= 2;
            }
#ifdef MUST_MATCH
            aud->position.x = intplGetValue(4, aud->interpFramesLeft, aud->interpFramesTotal, *(volatile f32*)&aud->mHomePosition.x, *(volatile f32*)&px);
            aud->position.y = intplGetValue(2, aud->interpFramesLeft, aud->interpFramesTotal, *(volatile f32*)&aud->mHomePosition.y, *(volatile f32*)&py);
            aud->position.z = intplGetValue(4, aud->interpFramesLeft, aud->interpFramesTotal, *(volatile f32*)&aud->mHomePosition.z, *(volatile f32*)&pz);
            v = intplGetValue(4, aud->interpFramesLeft, aud->interpFramesTotal, 1.0f, 0.65f);
#else
            aud->position.x = intplGetValue(4, aud->interpFramesLeft, aud->interpFramesTotal, aud->mHomePosition.x, px);
            aud->position.y = intplGetValue(2, aud->interpFramesLeft, aud->interpFramesTotal, aud->mHomePosition.y, py);
            aud->position.z = intplGetValue(4, aud->interpFramesLeft, aud->interpFramesTotal, aud->mHomePosition.z, pz);
            v = intplGetValue(4, aud->interpFramesLeft, aud->interpFramesTotal, 1.0f, 0.65f);
#endif
            aud->baseSize.y = v;
            aud->baseSize.x = v;
            break;
        }
    }
    return (count < 20) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

// Retail stores the truncated interpolation straight to the byte global and then passes
// the global back by name; the compiler folds that store into the stb and reuses the
// converted value. Storing through a `vol` local (the natural form) instead makes it
// materialise the global's address into a temp (addi + stb 0(base)).
USER_FUNC(unk_aji_00007668) {
    s32 timer;
    f32 v;
    u8 vol;

    if (isFirstCall) {
        unk_aji_bss00000168[0] = 120;
        unk_aji_bss00000165 = psndSFX_get_vol(screamId[0]);
    }
    timer = --unk_aji_bss00000168[0];
    v = intplGetValue(0, 120 - timer, 120, unk_aji_bss00000165, 0.0f);
#ifdef MUST_MATCH
    unk_aji_bss00000164 = (u8)v;
    psndSFX_vol(screamId[0], unk_aji_bss00000164);
#else
    vol = (u8)(s32)v;
    unk_aji_bss00000164 = vol;
    psndSFX_vol(screamId[0], vol);
#endif
    if (unk_aji_bss00000168[0] <= 0) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

void aud_get_pod_pos(BattleUnit* unit, f32* x, f32* y, f32* z) {
    BtlUnit_GetPos(unit, x, y, z);
    *x += 10.0f;
    *y += 78.0f;
}

void aud_fire_eff(f32 x, f32 y, f32 z) {
    EffectEntry* effect = effKemuTestEntry(11, x - 50.0f, y + 20.0f, z + 50.0f, 0.75f);
    effKemuTestSetRxRz(effect, 40.0f, 110.0f);
}

USER_FUNC(aud_fire_hit) {
    s32* args = event->args;
    s32 transId1;
    s32 transId2;
    s32 start;
    s32 i;
    BattleUnit* unit;
    BattleUnit* unit2;
    BattleUnitPart* parts;
    EffectEntry* effect;
    BattleAudienceMember* aud;
    EventEntry* ev;
    Vec position;
    f32 posx;
    f32 posy;
    f32 posz;
    f32 hx;
    f32 hy;
    f32 hz;

    transId1 = BattleTransID(event, evtGetValue(event, args[0]));
    transId2 = BattleTransID(event, evtGetValue(event, args[1]));
    start = evtGetValue(event, args[2]);
    if (isFirstCall) {
    unit = BattleGetUnitPtr(_battleWorkPointer, transId1);
    BtlUnit_GetPos(unit, &posx, &posy, &posz);
    posx += 10.0f;
    posy += 78.0f;
    effect = effKemuTestEntry(11, posx - 50.0f, posy + 20.0f, posz + 50.0f, 0.75f);
    effKemuTestSetRxRz(effect, 40.0f, 110.0f);
    unit2 = BattleGetUnitPtr(_battleWorkPointer, transId2);
    parts = BtlUnit_GetPartsPtr(unit2, BtlUnit_GetBodyPartsId(unit2));
    BtlUnit_GetHitPos(unit2, parts, &hx, &hy, &hz);
    for (i = start; i < start + 2; i++) {
        aud = BattleAudienceGetPtr(((s32*)list[0])[i]);
        BattleAudience_SetAnim(((s32*)list[0])[i], 9, 0);
        position = vec3_aji_00016210;
        position.x = posx;
        position.y = posy;
        position.z = posz;
        aud->position = position;
        aud->interpFramesTotal = 0;
        aud->counter = 0;
        aud->mRotation.y = 0.0f;
        aud->baseSize.y = 1.0f;
        aud->baseSize.x = 1.0f;
        aud->flags |= 4;
        ev = evtEntry(aud_hit_event, 0, 0x20);
        ev->lwData[0] = ((s32*)list[0])[i];
        ev->lwData[1] = FLOAT(hx);
        ev->lwData[2] = FLOAT(hy);
        ev->lwData[3] = FLOAT(hz);
        ev->lwData[4] = 0;
        ev->lwData[5] = irand(3) + 10;
        ev->lwData[6] = FLOAT((hx + 200.0f * rand() / 32767.0f - 100.0f));
        ev->lwData[7] = FLOAT(-100.0f);
        ev->lwData[8] = FLOAT((hz + 100.0f * rand() / 32767.0f - 50.0f));
        ev->lwData[9] = irand(30) + 30;
        ev->lwData[10] = FLOAT(-0.3f);
    }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(aud_fire_no_hit) {
    s32* args = event->args;
    s32 transId1;
    s32 transId2;
    s32 start;
    s32 i;
    BattleUnit* unit;
    BattleUnit* unit2;
    BattleUnitPart* parts;
    EffectEntry* effect;
    BattleAudienceMember* aud;
    EventEntry* ev;
    Vec position;
    f32 posx;
    f32 posy;
    f32 posz;
    f32 hx;
    f32 hy;
    f32 hz;

    transId1 = BattleTransID(event, evtGetValue(event, args[0]));
    transId2 = BattleTransID(event, evtGetValue(event, args[1]));
    start = evtGetValue(event, args[2]);
    if (isFirstCall) {
    unit = BattleGetUnitPtr(_battleWorkPointer, transId1);
    BtlUnit_GetPos(unit, &posx, &posy, &posz);
    posx += 10.0f;
    posy += 78.0f;
    effect = effKemuTestEntry(11, posx - 50.0f, posy + 20.0f, posz + 50.0f, 0.75f);
    effKemuTestSetRxRz(effect, 40.0f, 110.0f);
    unit2 = BattleGetUnitPtr(_battleWorkPointer, transId2);
    parts = BtlUnit_GetPartsPtr(unit2, BtlUnit_GetBodyPartsId(unit2));
    BtlUnit_GetHitPos(unit2, parts, &hx, &hy, &hz);
    for (i = start; i < start + 2; i++) {
        aud = BattleAudienceGetPtr(((s32*)list[0])[i]);
        BattleAudience_SetAnim(((s32*)list[0])[i], 9, 0);
        position = vec3_aji_0001621c;
        position.x = posx;
        position.y = posy;
        position.z = posz;
        aud->position = position;
        aud->interpFramesTotal = 0;
        aud->counter = 0;
        aud->mRotation.y = 0.0f;
        aud->baseSize.y = 1.0f;
        aud->baseSize.x = 1.0f;
        aud->flags |= 4;
        ev = evtEntry(aud_hit_event, 0, 0x20);
        ev->lwData[0] = ((s32*)list[0])[i];
        ev->lwData[1] = FLOAT(-275.0f);
        ev->lwData[2] = FLOAT((hy - 20.0f));
        ev->lwData[3] = FLOAT(hz);
        ev->lwData[4] = 0;
        ev->lwData[5] = irand(3) + 10;
    }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(aud_target_mark) {
    s32* args = event->args;
    f32 f31 = (f32)evtGetValue(event, args[0]);
    f32 f30 = (f32)evtGetValue(event, args[1]);
    f32 f29 = (f32)evtGetValue(event, args[2]);
    BattenData* data;

    if (isFirstCall) {
        wp_target[0] = BattleAlloc(0xC);
        wp_target[0]->effect = effBattenEntry(1, 1000, 0.0f, -1000.0f, 0.0f, 1.0f);
        wp_target[0]->field_0x0 = 0;
        wp_target[0]->field_0x4 = 0;
        targetSoundId[0] = psndSFXOn("SFX_BOSS_MBTN_SEARCH1");
    }
    data = wp_target[0]->effect->userdata;
    switch (wp_target[0]->field_0x0) {
    case 0:
        wp_target[0]->field_0x4 += 1;
        data->position.x = intplGetValue(4, wp_target[0]->field_0x4, 60, 60.0f + f31, f31 - 5.0f);
        data->position.y = intplGetValue(4, wp_target[0]->field_0x4, 60, 20.0f + f30, 60.0f + f30);
        data->position.z = 20.0f + f29;
        if (wp_target[0]->field_0x4 >= 60) {
            wp_target[0]->field_0x0 = 5;
            wp_target[0]->field_0x4 = 0;
        }
        break;
    case 5:
        wp_target[0]->field_0x4 += 1;
        data->position.x = intplGetValue(4, wp_target[0]->field_0x4, 40, f31 - 5.0f, f31 - 25.0f);
        data->position.y = intplGetValue(4, wp_target[0]->field_0x4, 40, 60.0f + f30, 20.0f + f30);
        data->position.z = 20.0f + f29;
        if (wp_target[0]->field_0x4 >= 40) {
            wp_target[0]->field_0x0 = 10;
            wp_target[0]->field_0x4 = 0;
        }
        break;
    case 10:
        wp_target[0]->field_0x4 += 1;
        data->position.x = intplGetValue(4, wp_target[0]->field_0x4, 20, f31 - 25.0f, f31);
        data->position.y = intplGetValue(4, wp_target[0]->field_0x4, 20, 20.0f + f30, f30);
        data->position.z = 20.0f + f29;
        if (wp_target[0]->field_0x4 >= 20) {
            wp_target[0]->field_0x0 = 15;
            wp_target[0]->field_0x4 = 0;
            effDelete(wp_target[0]->effect);
            wp_target[0]->effect = effBattenEntry(2, 1000, f31, f30, f29 + 10.0f, 1.0f);
            psndSFXOff(targetSoundId[0]);
            psndSFXOn("SFX_BOSS_MBTN_LOCKON1");
        }
        break;
    case 15:
        wp_target[0]->field_0x4 += 1;
        if (wp_target[0]->field_0x4 >= 90) {
            effDelete(wp_target[0]->effect);
            BattleFree(wp_target[0]);
            return EVT_RETURN_DONE;
        }
        break;
    }
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(eff_dead) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    eff_charge[0] = effAkariChargeN64Entry(0, 1000, x, y, z, 1.5f);
    return EVT_RETURN_DONE;
}

static USER_FUNC(eff_dead_delete) {
    effDelete(eff_charge[0]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_init) {
    s32 i;

    for (i = 0; i < 2; i++) {
        ArmWork* w;

        work[i] = BattleAlloc(sizeof(ArmWork));
        w = work[i];
        w->field_0xC = (Vec){ 0.0f, 0.0f, 0.0f };
        work[i]->field_0x0 = w->field_0xC;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_init2) {
    s32 idx = evtGetValue(event, event->args[0]);
    ArmWork* w = work[idx];

    w->effect = effBattenEntry(3, 1000, 0.0f, -1000.0f, 0.0f, 1.0f);
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_main) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    ArmWork* w;
    BattenData* data;
    s32 id;
    Mtx mtx74;
    Mtx mtx44;
    Vec offset;
    Vec pos;
    f32 x;
    f32 y;
    f32 z;
    f32 rx;
    f32 ry;
    f32 rz;
    f32 bx;
    f32 by;
    f32 bz;

    id = evtGetValue(event, args[0]);
    w = work[evtGetValue(event, args[1])];
    if (w->effect == NULL) {
        return EVT_RETURN_DONE;
    }
    data = w->effect->userdata;
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    w->field_0x0 = w->field_0xC;
    pos = (Vec){ 0.0f, 0.0f, 0.0f };
    pos.x = x;
    pos.y = y;
    pos.z = z;
    w->field_0xC = pos;
    if (evtCheckID(unit->waitEventId) != 1) {
        f32 dx = w->field_0xC.x - w->field_0x0.x;
        f32 dy = w->field_0xC.y - w->field_0x0.y;

        if (dx != 0.0f) {
            w->field_0x1C = 360.0f * (f32)atan(dy / dx) / 6.283184f;
            w->field_0x18 = w->field_0x18 + (w->field_0x1C - w->field_0x18) / 3.0f;
            BtlUnit_GetRotate(unit, &rx, &ry, &rz);
            BtlUnit_SetRotate(unit, rx, ry, w->field_0x18);
        } else {
            w->field_0x1C = 0.0f;
        }
    }
    BtlUnit_GetBaseRotate(unit, &bx, &by, &bz);
    BtlUnit_GetRotate(unit, &rx, &ry, &rz);
    rx += bx;
    ry += by;
    rz += bz;
    PSMTXRotRad(mtx74, 'x', 1.5707964f);
    PSMTXRotRad(mtx44, 'y', 0.017453292f * ry);
    PSMTXConcat(mtx44, mtx74, mtx74);
    PSMTXRotRad(mtx44, 'x', 0.017453292f * rx);
    PSMTXConcat(mtx44, mtx74, mtx74);
    PSMTXRotRad(mtx44, 'z', 0.017453292f * rz);
    PSMTXConcat(mtx44, mtx74, mtx74);
    PSMTXCopy(mtx74, data->matrix);
    offset = (Vec){ 0.0f, -18.0f, 0.0f };
    PSMTXMultVec(mtx74, &offset, &offset);
    data->position.x = w->field_0xC.x + offset.x;
    data->position.y = w->field_0xC.y + offset.y;
    data->position.z = w->field_0xC.z + offset.z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_end) {
    s32 idx = evtGetValue(event, event->args[0]);
    ArmWork* w = work[idx];
    if (w->effect != NULL) {
        effDelete(w->effect);
        w->effect = NULL;
    }
    return EVT_RETURN_DONE;
}

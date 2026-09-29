#include "unit_boss_koopa.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_message.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"

#include <dolphin/mtx.h>
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern BattleUnitKind unit_boss_kamec_obaba;
static void _disp_icon(CameraId cameraId, void* param);
static USER_FUNC(_icon_jump_icon);
static USER_FUNC(party_cannot_use_command);
static USER_FUNC(mario_cannot_use_command);
static char* cmd_disable_string[5];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[124];
static s32 entry_event[55];
static s32 damage_event[22];
static s32 dead_event[172];
static s32 phase_event[134];
static s32 harf_hp_event[94];
static s32 attack_event[110];
static s32 hip_attack_event[431];
static s32 bite_attack_event[278];
static s32 critical_bite_attack_event[261];
static s32 fire_attack_event[356];
s32 first_mario_damage_check[33];
s32 first_mario_damage_event[114];
static s32 wait_event[6];

// clang-format off
extend_data(128,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(129,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_koopa = {
    UNIT_BOWSER_CHAPTER8,       // mUnitType
    "btl_un_koopa",             // name
    70,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    68,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    6,                          // field_0x14
    86,                         // width
    80,                         // height
    { 0, 70 },                  // statusIconOffset
    { 0.0f, 40.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 40.0f, 40.0f, 0.0f },     // talkTogeBaseOffset
    { 43.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 21.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 36.0f, 72.0f, 0.0f },     // kissHitOffset
    { 0.0f, 40.0f, 0.0f },      // cutBaseOffset
    86.0f,                      // cutWidth
    80.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_KOOPA_DAMAGED1",  // damageSfxName
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

static BattleUnitDefense defence = { { 2, 2, 2, 2, 2 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    30,  // sleepRate
    30,  // stopRate
    30,  // dizzyRate
    100, // poisonRate
    40,  // confuseRate
    100, // electricRate
    100, // burnRate
    25,  // freezeRate
    100, // hugeRate
    75,  // tinyRate
    100, // attackUpRate
    75,  // attackDownRate
    100, // defenseUpRate
    80,  // defenseDownRate
    30,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    30,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    5,   // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "KPA_N_3" },
    { 0x02, "KPA_Z_6" },
    { 0x09, "KPA_Z_6" },
    { 0x05, "KPA_K_2" },
    { 0x04, "KPA_S_2" },
    { 0x03, "KPA_S_2" },
    { 0x1C, "KPA_S_1" },
    { 0x1D, "KPA_Q_2" },
    { 0x1E, "KPA_Q_2" },
    { 0x1F, "KPA_D_1" },
    { 0x27, "KPA_D_1" },
    { 0x32, "KPA_S_1" },
    { 0x2A, "KPA_R_1" },
    { 0x28, "KPA_W_1" },
    { 0x38, "KPA_S_1" },
    { 0x39, "KPA_S_1" },
    { 0x41, "KPA_T_1" },
    { 0x45, "KPA_S_1" },
};

static BattleDispPose pose_table_forward_leaning[18] = {
    { 0x01, "KPA_N_2" },
    { 0x02, "KPA_Z_5" },
    { 0x09, "KPA_Z_5" },
    { 0x05, "KPA_K_1" },
    { 0x04, "KPA_X_1" },
    { 0x03, "KPA_X_1" },
    { 0x1C, "KPA_S_8" },
    { 0x1D, "KPA_D_4" },
    { 0x1E, "KPA_D_4" },
    { 0x1F, "KPA_D_4" },
    { 0x27, "KPA_D_4" },
    { 0x32, "KPA_S_8" },
    { 0x2A, "KPA_R_1" },
    { 0x28, "KPA_W_1" },
    { 0x38, "KPA_S_8" },
    { 0x39, "KPA_S_8" },
    { 0x41, "KPA_T_9" },
    { 0x45, "KPA_S_8" },
};

static BattleDispPose pose_table_dead[3] = {
    { 0x1C, "KPA_Z_3" },
    { 0x27, "KPA_Z_3" },
    { 0x45, "KPA_Z_3" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                      // partNum
        "btl_un_koopa",         // partName
        "c_koopa",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 34.0f, 62.0f, 0.0f }, // hitBaseOffset
        { 22.0f, 78.0f, 0.0f }, // hitCursorBaseOffset
        70,                     // field_0x30
        80,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x00000009,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

static BattleWeapon weapon_koopa_hip_attack = {
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
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    10,                                       // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0x0000000C,                               // counterResistanceFlags
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

static BattleWeapon weapon_koopa_bite_attack = {
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
    { 7, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x00000002,                               // counterResistanceFlags
    0x00000004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    30,                                       // poisonChance
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

static BattleWeapon weapon_koopa_critical_bite_attack = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x00000002,                               // counterResistanceFlags
    0x00000004,                               // targetWeightingFlags
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

static BattleWeapon weapon_koopa_fire_attack = {
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
    1,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
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

BattleUnitSetup entry_kamec_obaba[1] = {
    { &unit_boss_kamec_obaba, 1, 0x04000004, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(koopa_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_btl_camera_set_moveto, 0, LW(0), 95, 350, LW(0), 45, -300, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

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
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btlevtcmd_SpawnUnit, GW(9), PTR(entry_kamec_obaba), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    ADD(LW(1), 40)
    ADD(LW(2), 10)
    CALL(btlevtcmd_SetPos, GW(9), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, GW(9), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    SET_VALUE(GW(8), LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    DIVIDE(LW(0), 2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    SET_VALUE(GW(10), 0)
    SET_VALUE(GW(11), 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_KOOPA_MOVE1L"), STRING("SFX_BOSS_KOOPA_MOVE1R"), 0, 15, 15)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_KOOPA_MOVE1L"), STRING("SFX_BOSS_KOOPA_MOVE1R"), 0, 8, 8)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
    WAIT_FRAMES(60)
    CALL_CHILD(koopa_zoom)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_LAUGH2_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_Y_2"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 2, STRING("stg8_las_96"), 0, -2)
    WAIT_MS(300)
    CALL_CHILD(kamec_obaba_zoom)
    CALL(evt_msg_print, 2, STRING("stg8_las_97"), 0, GW(9))
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GW(10), 1)
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(4), LW(5))
    SET_VALUE(LW(6), 1)
    IF_INT_EQUAL(LW(0), LW(3))
        IF_INT_EQUAL(LW(1), LW(4))
            IF_INT_EQUAL(LW(2), LW(5))
                SET_VALUE(LW(6), 0)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        CALL(btlevtcmd_ResetFaceDirection, -2)
    END_IF
    WAIT_FRAMES(1)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL_CHILD(koopa_zoom)
    SWITCH(GW(11))
        CASE_EQUAL(0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_SATIATED2_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_D_2"))
        WAIT_MS(200)
        CALL(btlevtcmd_SetTalkPose, -2, STRING("KPA_T_8"))
        CALL(btlevtcmd_SetStayPose, -2, STRING("KPA_Z_3"))
        CALL(evt_msg_print, 2, STRING("stg8_las_100_02"), 0, -2)
        WAIT_MS(300)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_dead))
        SET_VALUE(GW(10), 2)
        SET_VALUE(LW(10), -2)
        CALL_CHILD(subsetevt_dead_core_nospin_norotate)
        CASE_DEFAULT
        CALL(btlevtcmd_SetTalkPose, -2, STRING("KPA_D_1"))
        CALL(btlevtcmd_SetStayPose, -2, STRING("KPA_D_1"))
        CALL(evt_msg_print, 2, STRING("stg8_las_100_04"), 0, -2)
        WAIT_MS(300)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_SATIATED2_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_D_2"))
        WAIT_MS(700)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_dead))
        SET_VALUE(GW(10), 2)
        SET_VALUE(LW(10), -2)
        CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    END_SWITCH
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckActStatus, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_PhaseEventStartDeclare, -2)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_sub_random, 99, LW(1))
                IF_INT_LESS(LW(1), 0)
                    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_U_2"))
                    WAIT_MS(200)
                    CALL(btlevtcmd_OnPartsCounterAttribute, -2, 1, 1)
                    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_forward_leaning))
                END_IF
            ELSE
                CALL(evt_sub_random, 99, LW(1))
                IF_INT_LESS(LW(1), 100)
                    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_U_3"))
                    WAIT_MS(200)
                    CALL(btlevtcmd_OffPartsCounterAttribute, -2, 1, 1)
                    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
                END_IF
            END_IF
            CALL(btlevtcmd_StartWaitEvent, -2)
        END_IF
    END_IF
    LABEL(90)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_PhaseEventStartDeclare, -2)
        CALL_CHILD(harf_hp_event)
        CALL(btlevtcmd_StartWaitEvent, -2)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(harf_hp_event)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(1))
    IF_INT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
        IF_INT_LESS(LW(0), LW(1))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_U_3"))
                WAIT_MS(200)
            END_IF
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL_CHILD(koopa_zoom)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("KPA_T_3"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("KPA_S_4"))
            CALL(evt_msg_print, 2, STRING("stg8_las_100"), 0, -2)
            WAIT_MS(300)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_U_2"))
                WAIT_MS(200)
            END_IF
            CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(9), PTR(&weapon_koopa_fire_attack))
        CALL_CHILD(fire_attack_event)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 1)
        CASE_EQUAL(1)
        SET_VALUE(LW(9), PTR(&weapon_koopa_bite_attack))
        CALL_CHILD(bite_attack_event)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 2)
        CASE_EQUAL(2)
        SET_VALUE(LW(9), PTR(&weapon_koopa_hip_attack))
        CALL_CHILD(hip_attack_event)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 3)
        CASE_DEFAULT
        CALL(btlevtcmd_DrawLots, LW(0), 4, 20, 20, 0, 40)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(&weapon_koopa_hip_attack))
            CALL_CHILD(hip_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(&weapon_koopa_bite_attack))
            CALL_CHILD(bite_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(&weapon_koopa_critical_bite_attack))
            CALL_CHILD(critical_bite_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(9), PTR(&weapon_koopa_fire_attack))
            CALL_CHILD(fire_attack_event)
        END_SWITCH
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(first_mario_damage_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static USER_FUNC(mario_cannot_use_command) {
    BattleWork* work = _battleWorkPointer;
    s32 type = irand(2);
    s32* args = event->args;
    s32 arg = args[0];
    s32 value = 0;

    switch (type) {
        case 0:
            *(u32*)((u8*)work + 0x19038) = 3;
            value = 0x146;
            break;
        case 1:
            *(u32*)((u8*)work + 0x1903C) = 3;
            value = 0x149;
            break;
        case 2:
            *(u32*)((u8*)work + 0x19040) = 3;
            value = 0x14C;
            break;
    }
    event->lwData[0] = (s32)cmd_disable_string[type];
    evtSetValue(event, arg, value);
    return EVT_RETURN_DONE;
}

static USER_FUNC(party_cannot_use_command) {
    BattleWork* work = _battleWorkPointer;
    s32 type = irand(1);
    s32* args = event->args;
    s32 arg = args[0];
    s32 value = 0;

    switch (type) {
        case 0:
            *(u32*)((u8*)work + 0x19044) = 3;
            value = 0x150;
            break;
        case 1:
            *(u32*)((u8*)work + 0x19048) = 3;
            value = 0x14C;
            break;
    }
    event->lwData[0] = (s32)cmd_disable_string[type + 3];
    evtSetValue(event, arg, value);
    return EVT_RETURN_DONE;
}

static USER_FUNC(_icon_jump_icon) {
    IconJump* obj;
    s32* args;
    f32 x1;
    f32 y1;
    f32 z1;
    f32 x2;
    f32 y2;
    f32 z2;
    f32 accel;
    s32 frames;
    s32 icon;

    args = event->args;
    icon = evtGetValue(event, args[0]);
    x1 = evtGetFloat(event, args[1]);
    y1 = evtGetFloat(event, args[2]);
    z1 = evtGetFloat(event, args[3]);
    x2 = evtGetFloat(event, args[4]);
    y2 = evtGetFloat(event, args[5]);
    z2 = evtGetFloat(event, args[6]);
    accel = evtGetFloat(event, args[7]);
    frames = evtGetValue(event, args[8]);

    if (isFirstCall) {
        Vec pos;

        event->userdata[0] = (s32)BattleAlloc(sizeof(IconJump));
        obj = (IconJump*)event->userdata[0];
        obj->target.x = x2;
        obj->target.y = y2;
        obj->target.z = z2;
        pos.x = x1;
        pos.y = y1;
        pos.z = z1;
        obj->pos = pos;
        obj->start = pos;
        obj->fallAccel = accel;
        obj->frames = frames;
#ifdef MUST_MATCH
        // /2.0f (not *0.5f) makes MWCC put the float constant in f3 and the
        // int->float bias in f4, matching the original's register allocation.
        obj->vy = obj->fallAccel * obj->frames / 2.0f + (obj->target.y - obj->start.y) / obj->frames;
#else
        obj->vy = obj->fallAccel * obj->frames * 0.5f + (obj->target.y - obj->start.y) / obj->frames;
#endif
        obj->speed = distABf(obj->start.x, obj->start.z, obj->target.x, obj->target.z) / obj->frames;
        event->userdata[1] = icon;
        event->userdata[2] = 0;
    }

    obj = (IconJump*)event->userdata[0];
    if (event->userdata[2] != 0) {
        BattleFree(obj);
        return EVT_RETURN_DONE;
    }

    obj->pos.y += obj->vy;
    obj->vy -= obj->fallAccel;
    if (obj->vy < 0.0f && obj->pos.y < obj->target.y) {
        obj->pos.y = obj->target.y;
    }
    btlMovePos(&obj->pos.x, &obj->pos.z, obj->speed, angleABf(obj->start.x, obj->start.z, obj->target.x, obj->target.z));
    obj->frames--;
    if (obj->frames <= 0) {
        event->userdata[2] = 1;
        obj->pos = obj->target;
    }
    dispEntry(CAMERA_3D, 900.0f, 1, _disp_icon, event);
    return 0;
}

static EVT_BEGIN(hip_attack_event)
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
    CALL(btlevtcmd_GetUnitKind, LW(3), LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2A"))
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2B"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(25)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2C"))
        WAIT_FRAMES(15)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2D"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.25f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 50, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_HIP_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2E"))
    WAIT_MS(500)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 50, 13)
    CALL(btlevtcmd_JumpContinue, -2)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2E"))
    WAIT_MS(300)
    CALL(btlevtcmd_GetResultACDefence, LW(7))
    IF_INT_LESS_OR_EQUAL(LW(7), 3)
        SET_VALUE(LW(15), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(222)
            CALL(mario_cannot_use_command, LW(15))
            CASE_BETWEEN(224, 230)
            CALL(party_cannot_use_command, LW(15))
            CASE_DEFAULT
            SET_VALUE(LW(0), -1)
        END_SWITCH
        IF_INT_NOT_EQUAL(LW(0), -1)
            IF_INT_NOT_EQUAL(LW(15), 0)
                BEGIN_INLINE_EVENT
                    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
                    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(5), 60)
                    CALL(btlevtcmd_TransStageFloorPosition, LW(5), LW(6), LW(7))
                    CALL(_icon_jump_icon, LW(15), LW(0), LW(1), LW(2), LW(5), LW(6), LW(7), FLOAT(0.5f), 30)
                    SET_VALUE(LW(0), LW(5))
                    SET_VALUE(LW(1), LW(6))
                    SET_VALUE(LW(2), LW(7))
                    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(5), 40)
                    CALL(_icon_jump_icon, LW(15), LW(0), LW(1), LW(2), LW(5), LW(6), LW(7), FLOAT(0.5f), 20)
                    SET_VALUE(LW(0), LW(5))
                    SET_VALUE(LW(1), LW(6))
                    SET_VALUE(LW(2), LW(7))
                    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(5), 25)
                    CALL(_icon_jump_icon, LW(15), LW(0), LW(1), LW(2), LW(5), LW(6), LW(7), FLOAT(0.5f), 12)
                END_INLINE_EVENT
            END_IF
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, LW(0), 120)
            END_SIBLING_EVENT
        END_IF
    END_IF
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
    CALL_CHILD(first_mario_damage_check)
    RETURN
EVT_END()

static EVT_BEGIN(bite_attack_event)
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
    CALL(btlevtcmd_GetUnitKind, LW(3), LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 65)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3A"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3B"))
    WAIT_MS(150)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    GOTO(98)
    LABEL(91)
    WAIT_MS(100)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
    CALL_CHILD(first_mario_damage_check)
    RETURN
EVT_END()

static EVT_BEGIN(critical_bite_attack_event)
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
    CALL(btlevtcmd_GetUnitKind, LW(3), LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 65)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3A"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3B"))
    WAIT_MS(150)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_4"))
    WAIT_MS(1500)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL_CHILD(first_mario_damage_check)
    RETURN
EVT_END()

static EVT_BEGIN(fire_attack_event)
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
    CALL(btlevtcmd_GetUnitKind, LW(3), LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 120)
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1A"))
    WAIT_MS(300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_FIRE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_FIRE_BREATH1"), EVTDAT_POINTER_MAX, 0, LW(15))
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1B"))
    WAIT_MS(300)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    SET_FLOAT_VALUE(LW(6), FLOAT(60.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(22.5f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT(LW(2), 5)
    MULTIPLY_FLOAT(LW(5), FLOAT(3.5f))
    CALL(evt_eff, STRING(""), STRING("gonbaba_breath"), 7, LW(0), LW(1), LW(2), LW(5), 90, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1C"))
    WAIT_MS(300)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(10)
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
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(97)
    CALL_CHILD(first_mario_damage_check)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(98)
    WAIT_MS(1200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1D"))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    RETURN
EVT_END()

EVT_BEGIN(first_mario_damage_check)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        IF_INT_EQUAL(LW(0), 222)
            CALL(btlevtcmd_GetTotalDamage, LW(3), LW(1))
            IF_INT_NOT_EQUAL(LW(1), 0)
                CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(first_mario_damage_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL_CHILD(koopa_zoom)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_U_3"))
            WAIT_MS(200)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_LAUGH2_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_Y_2"))
        WAIT_MS(1000)
        CALL(btlevtcmd_SetTalkPose, -2, STRING("KPA_T_2"))
        CALL(btlevtcmd_SetStayPose, -2, STRING("KPA_S_3"))
        CALL(evt_msg_print, 2, STRING("stg8_las_98"), 0, -2)
        WAIT_MS(300)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
        IF_INT_EQUAL(GW(11), 0)
            CALL_CHILD(kamec_obaba_zoom)
            CALL(evt_msg_print, 2, STRING("stg8_las_99"), 0, GW(9))
            WAIT_MS(300)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_U_2"))
            WAIT_MS(200)
        END_IF
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        CALL(evt_btl_camera_set_mode, 0, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static char* cmd_disable_string[5] = {
    "btl_jump_cmd_disable", "btl_hammer_cmd_disable", "btl_item_cmd_disable", "btl_party_tech_cmd_disable",
    "btl_party_item_cmd_disable",
};

static void _disp_icon(CameraId cameraId, void* param) {
    EventEntry* event = param;
    IconJump* obj = (IconJump*)event->userdata[0];

    // icondrv.h declares iconDispGx's iconId as s16, but the original zero-extends
    // the argument (clrlwi), so it is really u16. Report, don't change the header.
#ifdef MUST_MATCH
    ((void (*)(Vec*, s16, u16, f32))iconDispGx)(&obj->pos, 0, (u16)event->userdata[1], 1.0f);
#else
    iconDispGx(&obj->pos, 0, event->userdata[1], 1.0f);
#endif
}
// clang-format on

#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle_database_common.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(shadow_scale_return);
static USER_FUNC(shadow_scale_small);
static USER_FUNC(shadow_scale_memory);
static USER_FUNC(pakkun_ceil_exist);
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts[1];
static BattleDispPose pose_table[18];
static BattleDataEntry data_table[15];
static s32 init_event[31];
static s32 dmg_avoid_counter_thorn_head_event[71];
static s32 damage_event[10];
static s32 counter_damage_event[30];
static s32 return_event[256];
static s32 normal_attack_event[311];
static s32 wait_event[6];

// clang-format off
extend_data(unit_monochrome_pakkun_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_monochrome_pakkun_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_PALE_PIRANHA,          // mUnitType
    "btl_un_monochrome_pakkun", // name
    4,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    11,                         // mLevel
    0,                          // mBonusExp
    1,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    1,                          // baseCoin
    60,                         // escapeRate
    9999,                       // renzokuSoftCap
    40,                         // width
    48,                         // height
    { 4, 48 },                  // statusIconOffset
    { 0.0f, 24.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 20.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 20.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 16.0f, 32.0f, 0.0f },     // kissHitOffset
    { 0.0f, 24.0f, 0.0f },      // cutBaseOffset
    40.0f,                      // cutWidth
    48.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_PAKKUN_DAMAGED1",  // damageSfxName
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
    110, // sleepRate
    90,  // stopRate
    60,  // dizzyRate
    100, // poisonRate
    100, // confuseRate
    100, // electricRate
    100, // burnRate
    80,  // freezeRate
    100, // hugeRate
    95,  // tinyRate
    100, // attackUpRate
    95,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    90,  // allergicRate
    30,  // frightRate
    60,  // galeForceRate
    100, // fastRate
    90,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleUnitKindPart parts[1] = {
    {
        1,                          // partNum
        "btl_un_monochrome_pakkun", // partName
        "c_pakflwr_t",              // modelName
        { 0.0f, 0.0f, 0.0f },       // positionOffset
        { 0.0f, 40.0f, 0.0f },      // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },       // hitCursorBaseOffset
        20,                         // shadowSize
        30,                         // field_0x32
        255,                        // baseAlpha
        &defence,                   // defense
        &defence_attr,              // defenseAttr
        0x00000009,                 // attributes
        0x00000001,                 // counterAttributes
        pose_table,                 // poseTable
    },
};

static BattleWeapon weapon_pakkun_flower_attack = {
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
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x00000002,                               // counterResistanceFlags
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

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(9), PTR(&weapon_pakkun_flower_attack))
        CALL_CHILD(normal_attack_event)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(9), PTR(&weapon_pakkun_flower_attack))
    CALL_CHILD(normal_attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static BattleDispPose pose_table[18] = {
    { 0x01, "PKF_N_1" },
    { 0x02, "PKF_Y_1" },
    { 0x09, "PKF_Y_1" },
    { 0x05, "PKF_K_1" },
    { 0x04, "PKF_X_1" },
    { 0x03, "PKF_X_1" },
    { 0x1C, "PKF_S_1" },
    { 0x1D, "PKF_Q_1" },
    { 0x1E, "PKF_Q_1" },
    { 0x1F, "PKF_S_2" },
    { 0x27, "PKF_D_1" },
    { 0x32, "PKF_A_1B" },
    { 0x28, "PKF_S_1" },
    { 0x2A, "PKF_S_1" },
    { 0x38, "PKF_X_1" },
    { 0x39, "PKF_X_1" },
    { 0x41, "PKF_T_1" },
    { 0x45, "PKF_S_1" },
};

static BattleDispPose pose_table_ceil[18] = {
    { 0x01, "PKF_N_2" },
    { 0x02, "PKF_Y_2" },
    { 0x09, "PKF_Y_2" },
    { 0x05, "PKF_K_2" },
    { 0x04, "PKF_X_2" },
    { 0x03, "PKF_X_2" },
    { 0x1C, "PKF_S_3" },
    { 0x1D, "PKF_Q_2" },
    { 0x1E, "PKF_Q_2" },
    { 0x1F, "PKF_S_4" },
    { 0x27, "PKF_D_2" },
    { 0x32, "PKF_A_5" },
    { 0x28, "PKF_S_3" },
    { 0x2A, "PKF_S_3" },
    { 0x38, "PKF_X_2" },
    { 0x39, "PKF_X_2" },
    { 0x41, "PKF_S_3" },
    { 0x45, "PKF_S_3" },
};

static BattleDataEntry data_table[15] = {
    { 0x25, dmg_avoid_counter_thorn_head_event },
    { 0x1A, counter_damage_event },
    { 0x19, counter_damage_event },
    { 0x1B, counter_damage_event },
    { 0x1C, counter_damage_event },
    { 0x1D, counter_damage_event },
    { 0x1E, counter_damage_event },
    { 0x1F, counter_damage_event },
    { 0x20, counter_damage_event },
    { 0x21, counter_damage_event },
    { 0x22, counter_damage_event },
    { 0x23, counter_damage_event },
    { 0x24, counter_damage_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

#include "rel/common/pakkun_ceil_exist.h"
#include "rel/common/shadow_scale_small.h"

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 50, 0)
    CALL(shadow_scale_memory, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_head_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 31)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 63)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 37)
        GOTO(90)
    END_IF
    ADD(LW(1), 50)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_D_1"))
    WAIT_MS(1000)
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL_CHILD(return_event)
    RETURN
EVT_END()

static EVT_BEGIN(ceil_fall_ready_event)
    CALL(btlevtcmd_SetEventCeilFall, -2, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_small, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_4"))
    WAIT_MS(300)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_return, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffAttribute, -2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_1"))
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 50, 0)
    CALL(btlevtcmd_SetStatusIconOffset, -2, 4, 48)
    CALL(btlevtcmd_SetKissHitOffset, -2, 16, 32, 0)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(return_event)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(pakkun_ceil_exist, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_sub_random, 1, LW(15))
    ELSE
        SET_VALUE(LW(15), 0)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_small, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_2"))
    WAIT_MS(300)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_SetEventCeilFall, -2, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
        CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
        WAIT_MS(500)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        BEGIN_SIBLING_EVENT
            CALL(shadow_scale_return, -2)
        END_SIBLING_EVENT
        CALL(btlevtcmd_OffAttribute, -2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_1"))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 50, 0)
        CALL(btlevtcmd_SetStatusIconOffset, -2, 4, 48)
        CALL(btlevtcmd_SetKissHitOffset, -2, 16, 32, 0)
        CALL(btlevtcmd_OffAttribute, -2, 2)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        WAIT_MS(500)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_1"))
    ELSE
        CALL(btlevtcmd_SetEventCeilFall, -2, PTR(ceil_fall_ready_event))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPos, -2, LW(0), 130, LW(2))
        CALL(btlevtcmd_SetHomePos, -2, LW(0), 130, LW(2))
        WAIT_MS(500)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        BEGIN_SIBLING_EVENT
            CALL(shadow_scale_return, -2)
        END_SIBLING_EVENT
        CALL(btlevtcmd_OffAttribute, -2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_3"))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, -80, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, -50, 0)
        CALL(btlevtcmd_SetStatusIconOffset, -2, 4, 10)
        CALL(btlevtcmd_SetKissHitOffset, -2, 16, -32, 0)
        CALL(btlevtcmd_OnAttribute, -2, 2)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4194304)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 2097152)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_ceil))
        WAIT_MS(500)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_3"))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
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
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_small, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(15))
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_2"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_4"))
    END_IF
    WAIT_MS(300)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 45)
    CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(500)
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_return, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffAttribute, -2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_1"))
    WAIT_MS(150)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_A_1A"))
    WAIT_MS(200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_A_1B"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_A_1C"))
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
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(98)
    LABEL(98)
    WAIT_MS(500)
    CALL_CHILD(return_event)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on

#include "unit_boss_kanbu1.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 btldefaultevt_Dummy[];
extern BattleUnitKind unit_npc_christine;
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[1];
static s32 init_event[82];
static s32 phase_event[282];
static s32 damage_event[10];
static s32 attack_event[24];
static s32 wait_event[6];
static s32 dead_event[276];
static BattleDispPose pose_table[18];
static s32 normal_attack_event[365];

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_kanbu1 = {
    UNIT_LORD_CRUMP_PROLOGUE,   // mUnitType
    "btl_un_kanbu1",            // name
    5,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    20,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    9,                          // renzokuSoftCap
    56,                         // width
    59,                         // height
    { 0, 56 },                  // statusIconOffset
    { 0.0f, 26.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 28.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 28.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 28.0f, 38.35f, 0.0f },    // kissHitOffset
    { 0.0f, 26.0f, 0.0f },      // cutBaseOffset
    42.0f,                      // cutWidth
    52.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_KANBU_DAMAGED1",  // damageSfxName
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
    100, // sleepRate
    100, // stopRate
    100, // dizzyRate
    100, // poisonRate
    100, // confuseRate
    100, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    100, // tinyRate
    100, // attackUpRate
    100, // attackDownRate
    100, // defenseUpRate
    100, // defenseDownRate
    100, // allergicRate
    0,   // frightRate
    100, // galeForceRate
    100, // fastRate
    100, // slowRate
    100, // dodgyRate
    100, // invisibleRate
    0,   // instaKillRate
};

static BattleDataEntry data_table[3] = {
    { 0x30, btldefaultevt_Dummy },
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_kanbu1",       // partName
        "c_kanbu",             // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 45.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 50.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // shadowSize
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

BattleWeapon weapon_kanbu1_attack = {
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
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0,                                        // counterResistanceFlags
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

BattleUnitSetup entry_npc_christine[1] = {
    { &unit_npc_christine, 2, 0x04000001, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_onoff_battleflag, 1, 4096)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_npc_christine), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_SetPos, LW(3), -140, 0, 5)
    CALL(btlevtcmd_SetHomePos, LW(3), -140, 0, 5)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_KANBU_MOVE1L"), STRING("SFX_BOSS_KANBU_MOVE1R"), 0, 10, 10)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_KANBU_MOVE1L"), STRING("SFX_BOSS_KANBU_MOVE1R"), 0, 6, 6)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

USER_FUNC(_mario_ac_success_check) {
    BattleWork* wp = _battleWorkPointer;

    evtSetValue(event, event->args[0], wp->marioAcSuccess);
    return EVT_RETURN_DONE;
}

USER_FUNC(_mario_ac_failed_check) {
    BattleWork* wp = _battleWorkPointer;

    evtSetValue(event, event->args[0], wp->marioAcFailed);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, -117, 62, 284, -115, 43, 0, 30, 11)
            WAIT_MS(500)
            BEGIN_SIBLING_EVENT_GET_ID(LW(5))
                CALL(evt_sub_intpl_init, 0, 0, 180, 15)
                LOOP(16)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -3, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(5))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
            CALL(btlevtcmd_SetTalkPose, LW(3), STRING("PKR_T_1"))
            CALL(btlevtcmd_SetStayPose, LW(3), STRING("PKR_S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t1_000"), 0, LW(3))
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(btlevtcmd_snd_se, -3, STRING("SFX_VOICE_MARIO_HAND_UP1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_N_2"))
            CALL(btlevtcmd_AnimeWaitPlayComplete, -3, 1)
            CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 43)
            BEGIN_SIBLING_EVENT_GET_ID(LW(5))
                CALL(evt_sub_intpl_init, 0, 180, 0, 15)
                LOOP(16)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -3, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(5))
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_StartWaitEvent, -3)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetMaxHp, -2, LW(1))
            DIVIDE(LW(1), 2)
            CALL(btlevtcmd_GetHp, -2, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), LW(1))
                CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
                CALL(evt_msg_print, 2, STRING("mac_0_b_t1_003"), 0, LW(3))
                CALL(btlevtcmd_StatusWindowOnOff, 1)
                RETURN
            END_IF
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(_mario_ac_success_check, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
                CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
                CALL(evt_msg_print, 2, STRING("mac_0_b_t1_001"), 0, LW(3))
                CALL(btlevtcmd_StatusWindowOnOff, 1)
            END_IF
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(_mario_ac_failed_check, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
                CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
                CALL(evt_msg_print, 2, STRING("mac_0_b_t1_002"), 0, LW(3))
                CALL(btlevtcmd_StatusWindowOnOff, 1)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(9), PTR(&weapon_kanbu1_attack))
    CALL_CHILD(normal_attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(0), 50)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 50)
    END_IF
    SET_VALUE(LW(0), 48)
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
    CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 50331648)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StopWaitEvent, -3)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
    BEGIN_SIBLING_EVENT_GET_ID(LW(5))
        CALL(evt_sub_intpl_init, 0, 0, 180, 15)
        LOOP(16)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -3, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(5))
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_SetTalkPose, LW(3), STRING("PKR_T_1"))
    CALL(btlevtcmd_SetStayPose, LW(3), STRING("PKR_S_1"))
    CALL(evt_msg_print, 2, STRING("mac_0_b_t1_004"), 0, LW(3))
    CALL(evt_btl_camera_set_mode, 0, 17)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_msg_print, 2, STRING("mac_0_b_t1_005"), 0, LW(3))
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(5))
        CALL(evt_sub_intpl_init, 0, 180, 0, 15)
        LOOP(16)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -3, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(5))
    CALL(btlevtcmd_KillUnit, LW(10), 0)
    RETURN
EVT_END()

static BattleDispPose pose_table[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "X_1" },
    { 0x1C, "S_1" },
    { 0x41, "T_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x2A, "R_2" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_guard[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "X_1" },
    { 0x1C, "G_2" },
    { 0x41, "T_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "G_2" },
    { 0x27, "D_1" },
    { 0x2A, "R_2" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "G_2" },
};

static EVT_BEGIN(normal_attack_event)
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
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(2.0f), FLOAT(0.2f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
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
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_HIP_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
        CALL(btlevtcmd_JumpSetting, -2, 0, 0, FLOAT(0.2f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
        CALL(btlevtcmd_JumpSetting, -2, 0, 0, FLOAT(0.2f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
        WAIT_FRAMES(25)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_HIP_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_HIP_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_1"))
        WAIT_FRAMES(15)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpSetting, -2, 15, 0, FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(7.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
        RETURN
EVT_END()
// clang-format on
#pragma force_active off

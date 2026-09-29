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

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 btldefaultevt_Dummy[];
extern s32 unit_npc_christine[];
static u32 regist[6];
static u32 data_table[6];
static u32 parts[19];
static s32 init_event[82];
static s32 phase_event[282];
static s32 damage_event[10];
static s32 attack_event[24];
static s32 wait_event[6];
static s32 dead_event[276];
static u32 pose_table[36];
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

// TODO: type
u32 unit_boss_kanbu1[49] = {
    0x00000006, (u32)"btl_un_kanbu1", 0x00050000, 0x01011400,
    0x00640032, 0x00090038, 0x003B0000, 0x00380000,
    0x00000000, 0x41D00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41E00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41E00000,
    0x00000000, 0x00000000, 0x41E00000, 0x42196666,
    0x00000000, 0x00000000, 0x41D00000, 0x00000000,
    0x42280000, 0x42500000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_KANBU_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x64646464, 0x64646464, 0x64646464, 0x64646400,
    0x64646464, 0x64000000,
};

// TODO: type
static u32 data_table[6] = {
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000031, (u32)&dead_event,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_kanbu1", (u32)"c_kanbu", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42340000,
    0x00000000, 0x00000000, 0x42480000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
u32 weapon_kanbu1_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    0x00000000, 0x00001087, 0x00000000, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 entry_npc_christine[12] = {
    (u32)unit_npc_christine, 0x02000000, 0x04000001, 0x447A0000,
    0x00000000, 0x447A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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

    evtSetValue(event, event->args[0], wp->field_16F1C[0x3C - 0x1C]);
    return EVT_RETURN_DONE;
}

USER_FUNC(_mario_ac_failed_check) {
    BattleWork* wp = _battleWorkPointer;

    evtSetValue(event, event->args[0], wp->field_16F1C[0x3D - 0x1C]);
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
    SET_VALUE(LW(9), PTR(weapon_kanbu1_attack))
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

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"N_1", 0x00000002, (u32)"Y_1",
    0x00000009, (u32)"Y_1", 0x00000005, (u32)"K_1",
    0x00000004, (u32)"X_1", 0x00000003, (u32)"X_1",
    0x0000001C, (u32)"S_1", 0x00000041, (u32)"T_1",
    0x0000001D, (u32)"Q_1", 0x0000001E, (u32)"Q_1",
    0x0000001F, (u32)"S_1", 0x00000027, (u32)"D_1",
    0x0000002A, (u32)"R_2", 0x00000028, (u32)"W_1",
    0x00000038, (u32)"X_1", 0x00000039, (u32)"X_1",
    0x00000041, (u32)"T_1", 0x00000045, (u32)"S_1",
};

// TODO: type
static u32 pose_table_guard[36] = {
    0x00000001, (u32)"N_1", 0x00000002, (u32)"Y_1",
    0x00000009, (u32)"Y_1", 0x00000005, (u32)"K_1",
    0x00000004, (u32)"X_1", 0x00000003, (u32)"X_1",
    0x0000001C, (u32)"G_2", 0x00000041, (u32)"T_1",
    0x0000001D, (u32)"Q_1", 0x0000001E, (u32)"Q_1",
    0x0000001F, (u32)"G_2", 0x00000027, (u32)"D_1",
    0x0000002A, (u32)"R_2", 0x00000028, (u32)"W_1",
    0x00000038, (u32)"X_1", 0x00000039, (u32)"X_1",
    0x00000041, (u32)"T_1", 0x00000045, (u32)"G_2",
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

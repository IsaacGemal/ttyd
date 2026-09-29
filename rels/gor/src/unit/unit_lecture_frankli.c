#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_default.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_audience.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 weaponGetPowerFromMarioJumpLv[];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[4];
static BattleUnitKindPart parts[1];
static s32 init_event[38];
static s32 damage_event[10];
static s32 phase_event[681];
static s32 attack_event[5];
static s32 wait_event[6];
s32 lecAttackEvent_Jump[735];
s32 lecAttackEvent_Hammer[651];
s32 lecture_end_event[51];

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

// TODO: type
u32 unit_lecture_frankli[49] = {
    0x000000B2, (u32)"btl_un_frankli", 0x000A0000, 0x01010100,
    0x00000064, 0x0009001C, 0x00180005, 0x00190000,
    0x00000000, 0x41600000, 0x00000000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x41600000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0x00000000, 0x41600000, 0x4179999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_KURI_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x12000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

static BattleUnitDefense defence = { { 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0 } };

static BattleVulnerableStatus regist = { 0 };

static BattleDispPose pose_table[5] = {
    { 0x1B, "D_1" }, { 0x1C, "S_1" }, { 0x1F, "D_2" }, { 0x27, "D_2" }, { 0x45, "S_1" },
};

// TODO: type
u32 _lec_weapon_jump[48] = {
    (u32)"btl_wn_mario_normal_jump", 0x009F0000, 0x00000000, (u32)"msg_normal_jump",
    0x64000000, 0x3F800000, 0x01016432, (u32)weaponGetPowerFromMarioJumpLv,
    0x00000001, 0x00000001, 0x00000002, 0x00000002,
    0x00000003, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01001260, 0x20001004, 0x00000302,
    (u32)"msg_ac_jump", 0x00035807, 0x0000000C, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&lecAttackEvent_Jump, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 _lec_weapon_hammer[48] = {
    (u32)"btl_wn_mario_normal_hammer", 0x009C0000, 0x00000000, (u32)"msg_normal_hammer",
    0x64000000, 0x3F800000, 0x01016432, (u32)weaponGetPowerFromMarioHammerLv,
    0x00000001, 0x00000002, 0x00000002, 0x00000004,
    0x00000003, 0x00000006, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01001260, 0x21002000, 0x00000302,
    (u32)"msg_ac_hammer", 0x00030887, 0x000007FB, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&lecAttackEvent_Hammer, 0x00000064, 0x00000000, 0x00000000,
};

static BattleDataEntry data_table[4] = {
    { 0x44, &_lec_weapon_jump }, { 0x45, &_lec_weapon_hammer },
    { 0x46, &lecture_end_event }, { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,
        "btl_un_frankli",
        "c_kuri_h",
        { 0.0f, 0.0f, 0.0f },
        { 15.0f, 25.0f, 0.0f },
        // 0x24..0x38 is unnamed in BattleUnitKindPart; bytes kept literal.
        { 0x00, 0x00, 0x00, 0x00, 0x41, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x14, 0x00, 0x1E, 0x00, 0xFF, 0x00, 0x00 },
        &defence,
        &defence_attr,
        kMainPart | kUnknown_0x8 | kNotTargetable | kUntattleable,
        0,
        pose_table,
    },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_onoff_battleflag, 1, 4096)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
            CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 0, 58, 256, 0, 41, 0, 30, 11)
            WAIT_MS(500)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_000"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_onoff_battleflag, 1, 16777216)
            CALL(btlevtcmd_StartWaitEvent, -3)
            RETURN
        END_IF
        CASE_EQUAL(2)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_SetUnitWork, -2, 1, 3)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_003"), 0, -2)
            BEGIN_SIBLING_EVENT_GET_ID(LW(0))
                CALL(evt_sub_intpl_init, 5, 0, 180, 20)
                LOOP(21)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(0))
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_004"), 0, -2)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
            CALL(btlevtcmd_StopWaitEvent, LW(3))
            CALL(btlevtcmd_SetTalkPose, LW(3), STRING("PKR_T_1"))
            CALL(btlevtcmd_SetStayPose, LW(3), STRING("PKR_S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_005"), 0, LW(3))
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(btlevtcmd_StartWaitEvent, -3)
            BEGIN_INLINE_EVENT
                CALL(evt_sub_intpl_init, 5, 180, 0, 20)
                LOOP(21)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            END_INLINE_EVENT
            RETURN
        END_IF
        CASE_EQUAL(4)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_SetUnitWork, -2, 1, 5)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_007"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(btlevtcmd_onoff_battleflag, 0, 16777216)
            CALL(btlevtcmd_onoff_battleflag, 1, 33554432)
            RETURN
        END_IF
        CASE_EQUAL(6)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_SetUnitWork, -2, 1, 7)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
            BEGIN_SIBLING_EVENT_GET_ID(LW(0))
                CALL(evt_sub_intpl_init, 5, 180, 0, 20)
                LOOP(21)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -6, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -6, 0, 0, 0)
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(0))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_011"), 0, -2)
            BEGIN_SIBLING_EVENT_GET_ID(LW(0))
                CALL(evt_sub_intpl_init, 5, 0, 180, 20)
                LOOP(21)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(0))
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_012"), 0, -2)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
            CALL(btlevtcmd_StopWaitEvent, LW(3))
            CALL(btlevtcmd_SetTalkPose, LW(3), STRING("PKR_T_1"))
            CALL(btlevtcmd_SetStayPose, LW(3), STRING("PKR_S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_013"), 0, LW(3))
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(btlevtcmd_StartWaitEvent, -3)
            BEGIN_INLINE_EVENT
                CALL(evt_sub_intpl_init, 5, 180, 0, 20)
                LOOP(21)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            END_INLINE_EVENT
            RETURN
        END_IF
        CASE_EQUAL(8)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_SetUnitWork, -2, 1, 9)
            CALL(btlevtcmd_onoff_battleflag, 0, 33554432)
            CALL(btlevtcmd_onoff_battleflag, 1, 134217728)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_016"), 0, -2)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 0, 58, 256, 0, 41, 0, 30, 11)
            WAIT_MS(500)
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_017"), 0, -2)
            CALL(evt_msg_select, 0, STRING("mac_0_b_t2_018"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print_add, 0, STRING("mac_0_b_t2_019"))
                CALL(evt_btl_camera_set_mode, 0, 0)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
                RETURN
            ELSE
                CALL(evt_msg_print_add, 0, STRING("mac_0_b_t2_020"))
                CALL(evt_btl_camera_set_mode, 0, 0)
                CALL(btlevtcmd_onoff_battleflag, 1, 32)
                RETURN
            END_IF
        END_IF
        CASE_EQUAL(9)
        CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_GetUnitWork, -6, 3, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(4)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t2_021"), 0, -2)
                CASE_EQUAL(5)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t2_022"), 0, -2)
                CASE_EQUAL(6)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t2_023"), 0, -2)
                CASE_EQUAL(7)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t2_021"), 0, -2)
            END_SWITCH
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            RETURN
        END_IF
        CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_sub_intpl_init, 5, 180, 0, 20)
            LOOP(21)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetRotate, -6, 0, LW(0), 0)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_SetRotate, -6, 0, 0, 0)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_GetUnitWork, -6, 2, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(1)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t2_021"), 0, -2)
                CASE_EQUAL(2)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t2_022"), 0, -2)
                CASE_EQUAL(3)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t2_023"), 0, -2)
            END_SWITCH
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            RETURN
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

EVT_BEGIN(lecAttackEvent_Jump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
        CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
        CALL(evt_msg_print, 2, STRING("mac_0_b_t2_001"), 0, -6)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 3, -1)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        IF_INT_LESS_OR_EQUAL(LW(10), 0)
            BEGIN_SIBLING_EVENT_GET_ID(LW(0))
                CALL(evt_sub_intpl_init, 5, 0, 180, 20)
                LOOP(21)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -6, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -6, 0, 180, 0)
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(0))
        END_IF
        CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
        CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
        DIVIDE(LW(0), 2)
        WAIT_FRAMES(LW(0))
        IF_INT_LESS_OR_EQUAL(LW(10), 0)
            CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
            CALL(evt_msg_print, 2, STRING("mac_0_b_t2_002"), 0, -6)
            WAIT_FRAMES(1)
            LABEL(11)
            CALL(btlevtcmd_PadCheckTrigger, 0, 256, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                WAIT_FRAMES(1)
                GOTO(11)
            END_IF
            WAIT_MS(150)
            CALL(evt_msg_continue)
        END_IF
    ELSE
        CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
        CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
        DIVIDE(LW(0), 2)
        WAIT_FRAMES(LW(0))
    END_IF
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_SetUnitWork, -6, 2, 0)
        CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
        IF_INT_EQUAL(LW(0), 9)
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(3)
                CALL(btlevtcmd_SetUnitWork, -6, 2, 1)
                CASE_EQUAL(1)
                CALL(btlevtcmd_SetUnitWork, -6, 2, 2)
                CASE_EQUAL(2)
                CALL(btlevtcmd_SetUnitWork, -6, 2, 3)
            END_SWITCH
        END_IF
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        IF_INT_EQUAL(LW(10), 0)
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        END_IF
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_WeaponAftereffect, LW(0))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_snd_voice, -2, 2)
    CALL(btlevtcmd_GetResultCountAC, LW(6))
    IF_INT_LESS_OR_EQUAL(LW(6), 1)
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
            CALL(evt_btl_camera_set_zoom, 0, 200)
            WAIT_FRAMES(10)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
            CALL(evt_btl_camera_set_zoom, 0, 250)
        END_INLINE_EVENT
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131072, LW(5))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        WAIT_FRAMES(2)
        ADD(LW(10), 1)
        GOTO(10)
    ELSE
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        WAIT_FRAMES(2)
    END_IF
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        BEGIN_SIBLING_EVENT_GET_ID(LW(14))
            CALL(evt_sub_intpl_init, 5, 180, 0, 20)
            LOOP(21)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetRotate, -6, 0, LW(0), 0)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_SetRotate, -6, 0, 0, 0)
        END_SIBLING_EVENT
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, -6, 1, 2)
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(lecAttackEvent_Hammer)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 5)
        CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
        CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
        CALL(evt_msg_print, 2, STRING("mac_0_b_t2_008"), 0, -6)
        LABEL(1)
        CALL(btlevtcmd_PadCheckNow, 0, 262144, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            WAIT_FRAMES(1)
            GOTO(1)
        END_IF
        CALL(evt_msg_continue)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    BEGIN_SIBLING_EVENT
        LABEL(2)
        CALL(btlevtcmd_GetPos, -3, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(0), 0)
            WAIT_FRAMES(1)
            GOTO(2)
        END_IF
        CALL(evt_sub_intpl_init, 5, 0, 180, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -6, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -6, 0, 180, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
        CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
        CALL(btlevtcmd_AcSetParamAll, 4, 25, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
        IF_INT_EQUAL(LW(0), 5)
            CALL(btlevtcmd_AcSetFlag, 2)
        ELSE
            CALL(btlevtcmd_AcSetFlag, 0)
        END_IF
        CALL(btlevtcmd_SetupAC, -2, 3, 1, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 35)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        ADD(LW(2), 5)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 350)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_H_1A"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_H_1B"))
        CALL(btlevtcmd_StartAC, 1)
        CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
        IF_INT_EQUAL(LW(0), 5)
            LABEL(5)
            CALL(btlevtcmd_AcGetParam, 4, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                WAIT_FRAMES(1)
                GOTO(5)
            END_IF
        END_IF
    END_SIBLING_EVENT
    SET_VALUE(LW(14), 0)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 5)
        BEGIN_SIBLING_EVENT_GET_ID(LW(14))
            SET_VALUE(LW(5), 5)
            LOOP(LW(5))
                CALL(btlevtcmd_PadCheckNow, 0, 262144, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    IS_EVENT_RUNNING(LW(15), LW(0))
                    IF_INT_EQUAL(LW(0), 0)
                    ELSE
                        SUSPEND_EVENT(LW(15))
                        CALL(btlevtcmd_AcSetParam, 2, 1)
                        CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
                        CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
                        CALL(evt_msg_print, 2, STRING("mac_0_b_t2_009"), 0, -6)
                        LABEL(6)
                        CALL(btlevtcmd_PadCheckNow, 0, 262144, LW(0))
                        IF_INT_EQUAL(LW(0), 0)
                            WAIT_FRAMES(1)
                            GOTO(6)
                        END_IF
                        WAIT_MS(200)
                        CALL(evt_msg_continue)
                        CALL(btlevtcmd_AcSetParam, 2, 0)
                        RESUME_EVENT(LW(15))
                    END_IF
                END_IF
                WAIT_FRAMES(1)
                SET_VALUE(LW(5), 5)
            END_LOOP
        END_SIBLING_EVENT
    END_IF
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    IF_INT_NOT_EQUAL(LW(14), 0)
        DELETE_EVENT(LW(14))
    END_IF
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 5)
        CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
        CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
        CALL(evt_msg_print, 2, STRING("mac_0_b_t2_010"), 0, -6)
        LABEL(10)
        CALL(btlevtcmd_PadCheckNow, 0, 262144, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            WAIT_FRAMES(1)
            GOTO(10)
        END_IF
        CALL(btlevtcmd_ResultAC)
        WAIT_MS(200)
        CALL(evt_msg_continue)
    ELSE
        CALL(btlevtcmd_ResultAC)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -6, 2, 0)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 9)
        CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(3)
            CALL(btlevtcmd_SetUnitWork, -6, 2, 1)
            CASE_EQUAL(1)
            CALL(btlevtcmd_SetUnitWork, -6, 2, 2)
            CASE_EQUAL(2)
            CALL(btlevtcmd_SetUnitWork, -6, 2, 3)
        END_SWITCH
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_H_1C"))
    WAIT_FRAMES(7)
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_HIT_GROUND2"), 0)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_GetDamage, LW(3), LW(0))
    CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
    WAIT_FRAMES(10)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 5)
        CALL(btlevtcmd_SetUnitWork, -6, 1, 6)
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(lecture_end_event)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 0, 58, 256, 0, 41, 0, 30, 11)
    WAIT_MS(500)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
    CALL(evt_msg_print, 2, STRING("mac_0_b_t2_024"), 0, -2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(500)
    CALL(btlevtcmd_onoff_battleflag, 1, 32)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

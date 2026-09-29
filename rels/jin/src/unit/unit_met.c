#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static u32 data_table[6];
static u32 regist[6];
static u32 parts[19];
static s32 init_event[12];
static s32 init_event2[148];
static s32 damage_event[10];
static s32 dmg_turn_event[71];
static s32 attack_event[546];
static s32 wait_event[6];
static s32 damage_event_ceil[49];
static s32 counter_damage_event_ceil[76];
static s32 ceil_fall_ready_event[158];
static s32 attack_event_ceil[467];
static s32 wait_event_ceil[6];

// clang-format off
extend_data(88,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(89,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_met[49] = {
    0x00000048, (u32)"btl_un_met", 0x00050000, 0x01011000,
    0x0246013C, 0x270F001C, 0x00140005, 0x00140000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0x00000000, 0x41400000, 0x41700000,
    0x00000000, 0x00000000, 0x41200000, 0x00000000,
    0x41E00000, 0x41A00000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_MET_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"MET_N_1", 0x00000002, (u32)"MET_Y_1",
    0x00000009, (u32)"MET_Y_1", 0x00000005, (u32)"MET_K_1",
    0x00000004, (u32)"MET_X_1", 0x00000003, (u32)"MET_X_1",
    0x0000001C, (u32)"MET_S_1", 0x0000001D, (u32)"MET_Q_1",
    0x0000001E, (u32)"MET_Q_1", 0x0000001F, (u32)"MET_A_1",
    0x00000027, (u32)"MET_D_1", 0x00000032, (u32)"MET_A_1",
    0x00000028, (u32)"MET_W_1", 0x0000002A, (u32)"MET_R_1",
    0x00000038, (u32)"MET_X_1", 0x00000039, (u32)"MET_X_1",
    0x00000041, (u32)"MET_T_1", 0x00000045, (u32)"MET_S_1",
};

// TODO: type
static u32 pose_table_ceil[36] = {
    0x00000001, (u32)"SMT_N_1", 0x00000002, (u32)"SMT_Y_1",
    0x00000009, (u32)"SMT_Y_1", 0x00000005, (u32)"SMT_K_1",
    0x00000004, (u32)"SMT_X_1", 0x00000003, (u32)"SMT_X_1",
    0x0000001C, (u32)"SMT_S_1", 0x0000001D, (u32)"SMT_Q_1",
    0x0000001E, (u32)"SMT_Q_1", 0x0000001F, (u32)"SMT_A_1",
    0x00000027, (u32)"SMT_D_1", 0x00000032, (u32)"SMT_A_1",
    0x00000028, (u32)"SMT_W_1", 0x0000002A, (u32)"SMT_R_1",
    0x00000038, (u32)"SMT_X_1", 0x00000039, (u32)"SMT_X_1",
    0x00000041, (u32)"SMT_S_1", 0x00000045, (u32)"SMT_S_1",
};

// TODO: type
static u32 pose_table_turn[34] = {
    0x00000001, (u32)"MET_N_2", 0x00000002, (u32)"MET_Y_2",
    0x00000009, (u32)"MET_Y_2", 0x00000005, (u32)"MET_K_2",
    0x00000004, (u32)"MET_X_2", 0x00000003, (u32)"MET_X_2",
    0x0000001C, (u32)"MET_S_2", 0x0000001D, (u32)"MET_Q_2",
    0x0000001E, (u32)"MET_Q_2", 0x0000001F, (u32)"MET_A_1",
    0x00000027, (u32)"MET_D_3", 0x00000032, (u32)"MET_A_1",
    0x00000028, (u32)"MET_W_1", 0x00000038, (u32)"MET_X_2",
    0x00000039, (u32)"MET_X_2", 0x00000041, (u32)"MET_S_2",
    0x00000045, (u32)"MET_S_2",
};

// TODO: type
static u32 data_table[6] = {
    0x0000000D, (u32)&dmg_turn_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 data_table_ceil[28] = {
    0x0000001A, (u32)&counter_damage_event_ceil, 0x00000019, (u32)&counter_damage_event_ceil,
    0x0000001B, (u32)&counter_damage_event_ceil, 0x0000001C, (u32)&counter_damage_event_ceil,
    0x0000001D, (u32)&counter_damage_event_ceil, 0x0000001E, (u32)&counter_damage_event_ceil,
    0x0000001F, (u32)&counter_damage_event_ceil, 0x00000020, (u32)&counter_damage_event_ceil,
    0x00000021, (u32)&counter_damage_event_ceil, 0x00000022, (u32)&counter_damage_event_ceil,
    0x00000023, (u32)&counter_damage_event_ceil, 0x00000024, (u32)&counter_damage_event_ceil,
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_met_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21004004, 0x00000302,
    0x00000000, 0x00000087, 0x000005FF, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_met_ceil_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00005087, 0x000005FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 defence[2] = {
    0x04630463, 0x04000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00020002, 0x00000000,
};

// TODO: type
static u32 defence_turn[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr_turn[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x4B5A4B64, 0x4B64004B, 0x645A645A, 0x645F4B3C,
    0x1E644B64, 0x645F0000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_met", (u32)"c_met", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41A00000,
    0x00000000, 0x00000000, 0x41C80000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
    CALL_CHILD(init_event2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(init_event2)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
        CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
        CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
        CALL(btlevtcmd_SetEventCeilFall, -2, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4096)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    ELSE
        CALL(btlevtcmd_OnAttribute, -2, 2)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4194304)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 2097152)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_ceil))
        CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table_ceil))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(1), 130)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_ceil))
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_ceil))
        CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_ceil))
        CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event_ceil))
        CALL(btlevtcmd_SetEventCeilFall, -2, PTR(ceil_fall_ready_event))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, -50, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, -50, 0)
        CALL(btlevtcmd_SetKissHitOffset, -2, 12, -15, 0)
    END_IF
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_MET_MOVE1L"), STRING("SFX_ENM_MET_MOVE1R"), 0, 3, 3)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_MET_MOVE1L"), STRING("SFX_ENM_MET_MOVE1R"), 0, 6, 6)
    CALL(btlevtcmd_SetOverTurnCount, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_turn_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence_turn))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(defence_attr_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 1)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(4)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 18, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 12, -1)
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(defence_attr))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 69)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 18, -1)
    CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetOverTurnCount, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(wakeup_event)
        RETURN
    END_IF
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        WAIT_FRAMES(30)
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_met_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_met_attack), LW(3), LW(4))
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
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_met_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_met_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 10)
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 20)
    WAIT_MS(100)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            ADD(LW(1), 10)
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 0)
    DELETE_EVENT(LW(13))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_met_attack), 256, LW(5))
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
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 10)
    WAIT_MS(250)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(10.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_met_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_met_attack), 256, LW(5))
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 75)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_ceil)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_GetTotalDamage, -2, LW(12))
    IF_INT_EQUAL(LW(12), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(ceil_fall_ready_event)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event_ceil)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 12, 15, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    CALL_CHILD(init_event2)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    CALL_CHILD(subsetevt_counter_damage)
    RETURN
EVT_END()

static EVT_BEGIN(ceil_fall_ready_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, 0)
    CALL(btlevtcmd_SetJumpSpeed, -2, 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 15, LW(2), 15, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_turn))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 12, 15, 0)
    CALL_CHILD(init_event2)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence_turn))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(defence_attr_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 69)
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 18, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 12, -1)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_ceil)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_met_ceil_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_met_ceil_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_met_ceil_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_met_ceil_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(10), LW(11), LW(12))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(11), LW(12), 60, -1, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    WAIT_MS(250)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, 0)
    CALL(btlevtcmd_SetJumpSpeed, -2, 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 18, LW(2), 15, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_met_ceil_attack), 256, LW(5))
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
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_met_ceil_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_met_ceil_attack), 256, LW(5))
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, -18, 0)
    SET_FLOAT_VALUE(LW(12), FLOAT(30.0f))
    SET_FLOAT_VALUE(LW(13), FLOAT(0.0f))
    LOOP(60)
        CALL(evt_sub_get_sincos, LW(13), LW(14), LW(15))
        MULTIPLY_FLOAT(LW(14), LW(12))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(14))
        SUBTRACT_FLOAT(LW(12), FLOAT(0.5f))
        ADD_FLOAT(LW(13), FLOAT(10.0f))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 12, 15, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 25)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 25)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MET_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetHomePos, -2, LW(7), 0, LW(9))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(7), 0, LW(9), 0, -1, 0)
    CALL_CHILD(init_event2)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_ceil)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

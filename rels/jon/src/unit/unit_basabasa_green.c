#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"

#pragma force_active on
static u32 data_table[28];
static u32 parts[19];
static u32 regist[6];
static s32 init_event[78];
static s32 dmg_first_attack_pos[12];
static s32 damage_event[10];
static s32 damage_event_ceil[51];
static s32 counter_damage_event[16];
static s32 ceil_fall_ready_event[23];
static s32 attack_event[683];
static s32 wait_event[6];

// clang-format off
extend_data(224,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(225,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_basabasa_green[49] = {
    0x000000A7, (u32)"btl_un_basabasa_green", 0x00140000, 0x01012302,
    0x02460232, 0x270F0020, 0x00200006, 0x00200000,
    0x00000000, 0x41800000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41800000,
    0x00000000, 0xC1700000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41800000,
    0x00000000, 0x00000000, 0x41800000, 0x41A66666,
    0x00000000, 0x00000000, 0x41800000, 0x00000000,
    0x42000000, 0x42000000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_BASA_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x04040002,
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
static u32 weapon_basabasa_drain_attack_biribiri_counterd[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x00000000, 0x00002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"BSA_N_1", 0x00000002, (u32)"BSA_Y_1",
    0x00000009, (u32)"BSA_Y_1", 0x00000005, (u32)"BSA_K_1",
    0x00000004, (u32)"BSA_X_1", 0x00000003, (u32)"BSA_X_1",
    0x0000001C, (u32)"BSA_S_1", 0x0000001D, (u32)"BSA_Q_1",
    0x0000001E, (u32)"BSA_Q_1", 0x0000001F, (u32)"BSA_S_1",
    0x00000027, (u32)"BSA_D_1", 0x00000032, (u32)"BSA_A_1",
    0x0000002A, (u32)"BSA_R_1", 0x00000028, (u32)"BSA_W_1",
    0x00000038, (u32)"BSA_X_1", 0x00000039, (u32)"BSA_X_1",
    0x00000041, (u32)"BSA_T_1", 0x00000045, (u32)"BSA_S_1",
};

// TODO: type
static u32 pose_table_ceil[36] = {
    0x00000001, (u32)"BSA_N_2", 0x00000002, (u32)"BSA_Y_2",
    0x00000009, (u32)"BSA_Y_2", 0x00000005, (u32)"BSA_K_2",
    0x00000004, (u32)"BSA_X_2", 0x00000003, (u32)"BSA_X_2",
    0x0000001C, (u32)"BSA_S_2", 0x0000001D, (u32)"BSA_Q_2",
    0x0000001E, (u32)"BSA_Q_2", 0x0000001F, (u32)"BSA_S_2",
    0x00000027, (u32)"BSA_D_2", 0x00000032, (u32)"BSA_A_1",
    0x0000002A, (u32)"BSA_S_1", 0x00000028, (u32)"BSA_S_1",
    0x00000038, (u32)"BSA_X_2", 0x00000039, (u32)"BSA_X_2",
    0x00000041, (u32)"BSA_T_2", 0x00000045, (u32)"BSA_S_2",
};

// TODO: type
static u32 data_table[28] = {
    0x0000001A, (u32)&counter_damage_event, 0x00000019, (u32)&counter_damage_event,
    0x0000001B, (u32)&counter_damage_event, 0x0000001C, (u32)&counter_damage_event,
    0x0000001D, (u32)&counter_damage_event, 0x0000001E, (u32)&counter_damage_event,
    0x0000001F, (u32)&counter_damage_event, 0x00000020, (u32)&counter_damage_event,
    0x00000021, (u32)&counter_damage_event, 0x00000022, (u32)&counter_damage_event,
    0x00000023, (u32)&counter_damage_event, 0x00000024, (u32)&counter_damage_event,
    0x00000030, (u32)&dmg_first_attack_pos, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_basabasa_green", (u32)"c_basabasa_g", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00600009, 0x00000000, (u32)&pose_table_ceil,
};

// TODO: type
static u32 regist[6] = {
    0x32375A64, 0x32646432, 0x644B644B, 0x64503232,
    0x4B643264, 0x64140000,
};

// TODO: type
static u32 weapon_basabasa_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000006, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000047, 0x00000000, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_sound_timing_table[45] = {
    (u32)"BSA_S_1", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_BASA_WAIT1",
    0x00000001, (u32)"BSA_W_1", 0x3C23D70A, 0x00000000,
    (u32)"SFX_ENM_BASA_MOVE1", 0x00000001, (u32)"BSA_R_1", 0x3C23D70A,
    0x00000000, (u32)"SFX_ENM_BASA_MOVE1", 0x00000001, (u32)"BSA_T_1",
    0x3C23D70A, 0x00000000, (u32)"SFX_ENM_BASA_WAIT1", 0x00000001,
    (u32)"BSA_A_1", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_BASA_WAIT1",
    0x00000001, (u32)"BSA_N_1", 0x3C23D70A, 0x00000000,
    (u32)"SFX_ENM_BASA_WAIT1", 0x00000001, (u32)"BSA_K_1", 0x3C23D70A,
    0x00000000, (u32)"SFX_ENM_BASA_WAIT1", 0x00000001, (u32)"BSA_X_1",
    0x3C23D70A, 0x00000000, (u32)"SFX_ENM_BASA_WAIT1", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001,
};

static EVT_BEGIN(ceil_sky_event)
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OnAttribute, -2, 4)
    CALL(btlevtcmd_SetPossessionItemOffset, -2, 0, 0, 0)
    CALL(btlevtcmd_SetEventCeilFall, -2, 0)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AddPos, -2, 0, -30, 0)
    CALL(btlevtcmd_AddHomePos, -2, 0, -30, 0)
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 30, 0)
    CALL(btlevtcmd_SetKissHitOffset, -2, 16, 21, 0)
    CALL(btlevtcmd_SetStatusIconOffset, -2, 6, 32)
    CALL(btlevtcmd_AddHomePos, -2, 0, -40, 0)
    LABEL(99)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_ceil))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventCeilFall, -2, PTR(ceil_fall_ready_event))
    CALL(btlevtcmd_SetPossessionItemOffset, -2, 0, -20, 0)
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, -50, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, -30, 0)
    CALL(btlevtcmd_SetStatusIconOffset, -2, 6, 10)
    CALL(btlevtcmd_SetKissHitOffset, -2, 16, -18, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(ceil_sky_event)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_first_attack_pos)
    CALL_CHILD(ceil_sky_event)
    CALL(btlevtcmd_SetPos, -2, LW(0), 10, LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
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
    CALL(btlevtcmd_GetTotalDamage, LW(10), LW(12))
    IF_INT_EQUAL(LW(12), 0)
        RETURN
    END_IF
    CALL_CHILD(ceil_sky_event)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 0, 0, 0, -1)
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event)
    CALL_CHILD(ceil_sky_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    CALL_CHILD(subsetevt_counter_damage)
    RETURN
EVT_END()

static EVT_BEGIN(ceil_fall_ready_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL_CHILD(ceil_sky_event)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 0, 0, 0, -1)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_basabasa_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_basabasa_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_basabasa_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_basabasa_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddPos, -2, 0, -30, 0)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.5f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 30)
        SUBTRACT(LW(1), 40)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -16, 0, 0, -1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), -6)
    LOOP(2)
        ADD(LW(1), 13)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 10, 10, 0, 0, -1)
        ADD(LW(1), -13)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 10, -10, 0, 0, -1)
    END_LOOP
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.5f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    SUBTRACT(LW(1), 10)
    SUBTRACT(LW(2), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -16, 0, 0, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_basabasa_attack), 256, LW(5))
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
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.5f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 50)
    ADD(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0, 0, -1)
    WAIT_MS(200)
    GOTO(98)
    LABEL(91)
    SET_VALUE(LF(0), 0)
    CALL(btlevtcmd_PreCheckCounter, -2, LW(3), LW(4), PTR(weapon_basabasa_attack), 256, LW(5))
    IF_INT_EQUAL(LW(5), 17)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LW(5), 14)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_basabasa_attack))
        SET_VALUE(LW(9), PTR(weapon_basabasa_drain_attack_biribiri_counterd))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        RETURN
    END_IF
    CALL(btlevtcmd_SetRotate, -2, 0, 0, -45)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BASA_SUCK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BSA_A_2"))
    LOOP(2)
        LOOP(10)
            CALL(btlevtcmd_AddScale, -2, 0, FLOAT(0.05f), 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(10)
            CALL(btlevtcmd_AddScale, -2, 0, FLOAT(-0.05f), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_basabasa_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_basabasa_attack), 256, LW(5))
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 15)
    ADD(LW(2), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetDamage, LW(3), LW(10))
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BSA_N_1"))
        WAIT_MS(300)
        CALL(btlevtcmd_RecoverHp, -2, 1, LW(10))
        CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
        ADD(LW(6), 20)
        ADD(LW(7), 10)
        CALL(evt_eff, STRING(""), STRING("recovery"), 7, LW(5), LW(6), LW(7), LW(10), 0, 0, 0, 0, 0, 0, 0)
        SET_VALUE(LW(0), 0)
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(0), 5)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AddScale, -2, FLOAT(-0.1f), FLOAT(0.1f), 0)
            ADD(LW(0), 1)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(0), 0)
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(0), 10)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AddScale, -2, FLOAT(0.1f), FLOAT(-0.1f), 0)
            ADD(LW(0), 1)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(0), 0)
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(0), 5)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AddScale, -2, FLOAT(-0.1f), FLOAT(0.1f), 0)
            ADD(LW(0), 1)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetScale, -2, 1, 1, 1)
        WAIT_MS(1000)
    END_IF
    WAIT_MS(500)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(7))
    IF_INT_EQUAL(LW(7), 1)
        SUBTRACT(LW(1), 30)
    END_IF
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -10, 0, 0, -1)
    LABEL(99)
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BSA_S_2"))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddPos, -2, 0, 30, 0)
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BSA_S_1"))
        WAIT_FRAMES(1)
    END_IF
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

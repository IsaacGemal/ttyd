#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
extern s32 weaponGetFPPowerDefault[];
static u32 regist[6];
static u32 parts[19];
static s32 magic_attack_event[269];
static u32 pose_table[36];
static u32 data_table[28];
static s32 init_event[39];
static s32 damage_event[10];
static s32 counter_damage_event[41];
static s32 return_event[24];
static s32 drain_attack_event[911];
static s32 wait_event[6];

// clang-format off
extend_data(flower_chorobon_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(flower_chorobon_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x00000057, (u32)"btl_un_flower_chorobon", 0x00060000, 0x01011300,
    0x0346003C, 0x270F001C, 0x00200005, 0x00200000,
    0x00000000, 0x41800000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0x00000000, 0x41100000, 0x41A00000,
    0x00000000, 0x00000000, 0x41800000, 0x00000000,
    0x41E00000, 0x42000000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_CHORO2_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00040000,
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
    0x55555064, 0x55646446, 0x645A645A, 0x645F3C55,
    0x50645064, 0x645F0000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_flower_chorobon", (u32)"c_chorobon_f", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_chorobon_drain_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00000047, 0x00000000, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_chorobon_drain_attack_fp[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)weaponGetFPPowerDefault, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00000047, 0x00000000, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_chorobon_magic_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000047, 0x000007FF, 0x00002000,
    0x00050000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetFp, -2, LW(0))
    CALL(btlevtcmd_GetMaxFp, -2, LW(1))
    IF_INT_LESS(LW(0), 2)
        GOTO(10)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(0), LW(1))
        GOTO(20)
    END_IF
    SET_VALUE(LW(0), 50)
    ADD(LW(0), 10)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    IF_INT_LESS(LW(1), 50)
        LABEL(10)
        CALL_CHILD(drain_attack_event)
        GOTO(99)
    END_IF
    LABEL(20)
    CALL_CHILD(magic_attack_event)
    GOTO(99)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(magic_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_chorobon_magic_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_chorobon_magic_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_chorobon_magic_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_chorobon_magic_attack))
    SET_VALUE(LW(0), 2)
    MULTIPLY(LW(0), -1)
    CALL(btlevtcmd_RecoverFp, -2, 1, LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, 0, FLOAT(0.1f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), FLOAT(50.0f), -1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING(""), STRING("shock_n64"), 1, LW(0), LW(1), LW(2), 20, 20, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(10)
    CALL(evt_eff64, STRING(""), STRING("shock_n64"), 1, LW(0), LW(1), LW(2), 20, 20, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(10)
    CALL(evt_eff64, STRING(""), STRING("shock_n64"), 1, LW(0), LW(1), LW(2), 20, 20, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(20)
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 30, 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_chorobon_magic_attack), 256, LW(5))
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
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_chorobon_magic_attack))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_chorobon_magic_attack), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(0)
    END_IF
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

// TODO: type
static u32 weapon_chorobon_drain_attack_biribiri_counterd[48] = {
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
    0x00000001, (u32)"CBN_N_1", 0x00000002, (u32)"CBN_Y_1",
    0x00000009, (u32)"CBN_Y_1", 0x00000005, (u32)"CBN_K_1",
    0x00000004, (u32)"CBN_X_1", 0x00000003, (u32)"CBN_X_1",
    0x0000001C, (u32)"CBN_S_1", 0x0000001D, (u32)"CBN_Q_1",
    0x0000001E, (u32)"CBN_Q_1", 0x0000001F, (u32)"CBN_S_1",
    0x00000027, (u32)"CBN_D_1", 0x00000032, (u32)"CBN_A_1",
    0x0000002A, (u32)"CBN_R_1", 0x00000028, (u32)"CBN_W_1",
    0x00000038, (u32)"CBN_X_1", 0x00000039, (u32)"CBN_X_1",
    0x00000041, (u32)"CBN_T_1", 0x00000045, (u32)"CBN_S_1",
};

// TODO: type
static u32 data_table[28] = {
    0x0000001A, (u32)&counter_damage_event, 0x00000019, (u32)&counter_damage_event,
    0x0000001B, (u32)&counter_damage_event, 0x0000001C, (u32)&counter_damage_event,
    0x0000001D, (u32)&counter_damage_event, 0x0000001E, (u32)&counter_damage_event,
    0x0000001F, (u32)&counter_damage_event, 0x00000020, (u32)&counter_damage_event,
    0x00000021, (u32)&counter_damage_event, 0x00000022, (u32)&counter_damage_event,
    0x00000023, (u32)&counter_damage_event, 0x00000024, (u32)&counter_damage_event,
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000000, 0x00000000,
};

static EVT_BEGIN(rabbit_move)
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    LABEL(0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(11), LW(5))
    SUBTRACT(LW(11), LW(0))
    SET_VALUE(LW(12), LW(11))
    IF_INT_LESS(LW(12), 0)
        MULTIPLY(LW(12), -1)
    END_IF
    IF_INT_GREATER(LW(12), LW(8))
        IF_INT_GREATER_OR_EQUAL(LW(11), 0)
            ADD(LW(0), LW(8))
        ELSE
            SUBTRACT(LW(0), LW(8))
        END_IF
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(6), LW(2), 0, -1)
        LOOP(2)
            CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
            WAIT_FRAMES(1)
        END_LOOP
        GOTO(0)
    ELSE
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(5), LW(6), LW(7), 0, -1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(2)
            CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_IF
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.16f), FLOAT(0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 pose_sound_timing_table[20] = {
    (u32)"CBN_S_1", 0x3F155555, 0x00000000, (u32)"SFX_ENM_CHORO2_MOVE1",
    0x00000001, (u32)"CBN_S_1", 0x3F555555, 0x00000000,
    (u32)"SFX_ENM_CHORO2_WAIT2", 0x00000001, (u32)"CBN_S_1", 0x3FCAAAAB,
    0x00000000, (u32)"SFX_ENM_CHORO2_WAIT1", 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_ENM_CHORO2_MOVE1"), 0)
    CALL(btlevtcmd_SetMaxFp, -2, 3)
    CALL(btlevtcmd_SetFp, -2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    CALL_CHILD(subsetevt_counter_damage)
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

static EVT_BEGIN(return_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetHomePos, -2, LW(5), LW(6), LW(7))
    SET_VALUE(LW(8), 40)
    CALL_CHILD(rabbit_move)
    RETURN
EVT_END()

static EVT_BEGIN(drain_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_chorobon_drain_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_chorobon_drain_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_chorobon_drain_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_chorobon_drain_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(5), LW(6), LW(7))
    SET_VALUE(LW(6), 0)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(5), 50)
    SET_VALUE(LW(8), 50)
    CALL_CHILD(rabbit_move)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_JumpSetting, -2, 60, 0, FLOAT(0.7f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 5)
    SUBTRACT(LW(1), 10)
    SUBTRACT(LW(2), 2)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_chorobon_drain_attack), 256, LW(5))
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
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 5)
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(1.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 30)
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_N_1"))
    WAIT_MS(1000)
    GOTO(98)
    LABEL(91)
    SET_VALUE(LF(0), 0)
    CALL(btlevtcmd_PreCheckCounter, -2, LW(3), LW(4), PTR(weapon_chorobon_drain_attack), 256, LW(5))
    IF_INT_EQUAL(LW(5), 17)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LW(5), 14)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_chorobon_drain_attack))
        SET_VALUE(LW(9), PTR(weapon_chorobon_drain_attack_biribiri_counterd))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        RETURN
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_ATTACK1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetBodyId, LW(3), LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(0), 37)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 89)
        SET_VALUE(LW(1), 1)
        SET_VALUE(LW(2), 2)
        SET_VALUE(LW(5), LW(2))
        LABEL(20)
        SUBTRACT(LW(5), 1)
        IF_INT_LESS_OR_EQUAL(LW(5), 0)
            SET_VALUE(LW(5), LW(2))
            IF_INT_EQUAL(LW(1), 0)
                SET_VALUE(LW(1), 1)
            ELSE
                SET_VALUE(LW(1), 0)
            END_IF
        END_IF
        CALL(btlevtcmd_SetDispOffset, LW(3), LW(1), 0, 0)
        WAIT_FRAMES(1)
        SUBTRACT(LW(0), 1)
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            GOTO(20)
        END_IF
        CALL(btlevtcmd_SetDispOffset, LW(3), 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_ATTACK1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
    SET_VALUE(LW(14), 20)
    IF_INT_EQUAL(LW(15), -1)
        SET_VALUE(LW(14), -20)
    END_IF
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(14))
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(1.5f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_A_2"))
    WAIT_FRAMES(90)
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(1.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_R_2"))
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    LOOP(4)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddScale, -2, 0, FLOAT(-0.2f), 0)
    END_LOOP
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(0.2f), FLOAT(1.0f))
    LOOP(20)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.01f), FLOAT(0.05f), 0)
    END_LOOP
    WAIT_FRAMES(1)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_chorobon_drain_attack))
    CALL(btlevtcmd_GetFp, LW(3), LW(0))
    IF_INT_GREATER(LW(0), 0)
        SET_VALUE(LW(9), PTR(weapon_chorobon_drain_attack_fp))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        CALL(btlevtcmd_GetFpDamage, LW(3), LW(10))
    ELSE
        SET_VALUE(LW(9), PTR(weapon_chorobon_drain_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        CALL(btlevtcmd_GetHpDamage, LW(3), LW(10))
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.5f), FLOAT(0.5f), 0)
    END_IF
    LOOP(4)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.08f), FLOAT(0.12f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_AddScale, -2, FLOAT(0.32f), FLOAT(-0.48f), 0)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_AddScale, -2, FLOAT(-0.32f), FLOAT(0.48f), 0)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 10, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHORO2_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.16f), FLOAT(0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_W_1"))
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        LOOP(20)
            CALL(btlevtcmd_AddScale, -2, FLOAT(-0.0244141f), FLOAT(-0.0244141f), 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_GetBodyId, -2, LW(1))
        CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
        ADD(LW(6), 45)
        ADD(LW(7), 10)
        IF_INT_EQUAL(LW(9), PTR(weapon_chorobon_drain_attack))
            CALL(btlevtcmd_RecoverHp, -2, LW(1), LW(10))
            CALL(evt_eff, STRING(""), STRING("recovery"), 7, LW(5), LW(6), LW(7), LW(10), 0, 0, 0, 0, 0, 0, 0)
        ELSE
            CALL(btlevtcmd_RecoverFp, -2, LW(1), LW(10))
            CALL(evt_eff, STRING(""), STRING("recovery"), 8, LW(5), LW(6), LW(7), LW(10), 0, 0, 0, 0, 0, 0, 0)
        END_IF
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    END_IF
    LOOP(2)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 30)
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_ENM_CHORO2_ATTACK3"), 0, 0, -1, -1, 0, 0)
        LOOP(2)
            CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -2)
        WAIT_FRAMES(16)
        LOOP(2)
            CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, 0, 0, 0, -1, -1, 0, 0)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 6, -2)
    END_LOOP
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.16f), FLOAT(0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 0)
    CALL_CHILD(return_event)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    LABEL(99)
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

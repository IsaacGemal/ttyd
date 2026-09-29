#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static USER_FUNC(eff_aura);
static u32 data_table[6];
static u32 regist[6];
static u32 parts[19];
static s32 init_event[41];
static s32 damage_event[10];
static s32 dmg_turn_event[78];
static s32 attack_event[548];
static s32 ura_attack_event[560];
static s32 wait_event[6];

// clang-format off
extend_data(112,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(113,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_ura_noko_15_data_2E060[49] = {
    0x0000002F, (u32)"btl_un_ura_noko", 0x00080000, 0x01011100,
    0x02460132, 0x270F0024, 0x0028FFFC, 0x00280000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41100000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41900000,
    0x00000000, 0x00000000, 0x41300000, 0x41C00000,
    0x00000000, 0x00000000, 0x41A00000, 0x00000000,
    0x42100000, 0x42200000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_NOKO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00040000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"NKT_N_1", 0x00000002, (u32)"NKT_Y_1",
    0x00000009, (u32)"NKT_Y_1", 0x00000005, (u32)"NKT_K_1",
    0x00000004, (u32)"NKT_X_1", 0x00000003, (u32)"NKT_X_1",
    0x0000001C, (u32)"NKT_S_1", 0x0000001D, (u32)"NKT_Q_1",
    0x0000001E, (u32)"NKT_Q_1", 0x0000001F, (u32)"NKT_S_1",
    0x00000027, (u32)"NKT_D_1", 0x00000032, (u32)"NKT_A_1",
    0x0000002A, (u32)"NKT_R_1", 0x00000028, (u32)"NKT_W_1",
    0x00000038, (u32)"NKT_X_1", 0x00000039, (u32)"NKT_X_1",
    0x00000041, (u32)"NKT_T_1", 0x00000045, (u32)"NKT_S_1",
};

// TODO: type
static u32 pose_table_turn[34] = {
    0x00000001, (u32)"NKT_N_2", 0x00000002, (u32)"NKT_B_1",
    0x00000009, (u32)"NKT_B_1", 0x00000005, (u32)"NKT_K_2",
    0x00000004, (u32)"NKT_K_2", 0x00000003, (u32)"NKT_K_2",
    0x0000001C, (u32)"NKT_S_2", 0x0000001D, (u32)"NKT_Q_1",
    0x0000001E, (u32)"NKT_Q_1", 0x0000001F, (u32)"NKT_D_1",
    0x00000027, (u32)"NKT_D_1", 0x0000002A, (u32)"NKT_R_1",
    0x00000028, (u32)"NKT_W_1", 0x00000038, (u32)"NKT_K_2",
    0x00000039, (u32)"NKT_K_2", 0x00000041, (u32)"NKT_S_2",
    0x00000045, (u32)"NKT_S_2",
};

// TODO: type
static u32 data_table[6] = {
    0x0000000D, (u32)&dmg_turn_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence[2] = {
    0x01010101, 0x01000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_turn[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x555A5064, 0x50646450, 0x645A645A, 0x645A5055,
    0x50645064, 0x645F0000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_ura_noko", (u32)"c_uranoko", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x00000000, 0x42200000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00001009, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_nokonoko_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00080302,
    0x00000000, 0x00000087, 0x000005FF, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_ura_noko_ura_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000006, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20004004, 0x00080302,
    0x00000000, 0x00000087, 0x000005FF, 0x00002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetOverTurnCount, -2, 0)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3R"), 0, 3, 3)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3L"), 0, 6, 6)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_turn_event)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, -15, 0)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 2)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("NKT_D_1"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 15, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("NKT_S_2"))
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(ura_attack_event)
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("NKT_S_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(first_attack_event)
    CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_nokonoko_attack))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("NKT_A_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 20)
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 40)
    WAIT_MS(100)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_nokonoko_attack), 256, LW(5))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 75)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
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
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_nokonoko_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_nokonoko_attack), LW(3), LW(4))
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
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("NKT_A_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 20)
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 40)
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
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 1)
    DELETE_EVENT(LW(13))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_nokonoko_attack), 256, LW(5))
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
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 1)
    WAIT_MS(250)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 10)
    WAIT_MS(250)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(10.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_nokonoko_attack), 256, LW(5))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 75)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: write eff_aura (static, 0xe0 bytes) here:
// its asm is in build/G8MJ01/jon/asm/unit/unit_ura_noko.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(ura_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_ura_noko_ura_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_ura_noko_ura_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_ura_noko_ura_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_ura_noko_ura_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.5f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(27)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("NKT_A_2"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    LOOP(36)
        CALL(btlevtcmd_AddRotate, -2, 0, 10, 0)
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(30)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 5)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_FRAMES(20)
    BEGIN_SIBLING_EVENT
        WAIT_MS(200)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(2), 10)
        CALL(eff_aura, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_URANOKO_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 10)
    WAIT_MS(400)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 20)
    WAIT_MS(400)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 40)
    WAIT_MS(400)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(evt_btl_camera_set_mode, 0, 0)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            ADD(LW(1), 10)
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_URANOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 1)
    SET_VALUE(LW(6), 0)
    LABEL(10)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_ura_noko_ura_attack), 256, LW(5))
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
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 1)
    GOTO(97)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_ura_noko_ura_attack))
        SET_VALUE(LW(6), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_ura_noko_ura_attack), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 500)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 1)
    WAIT_MS(500)
    CALL(btlevtcmd_SetPos, -2, 250, LW(1), LW(2))
    LABEL(98)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_URANOKO_MOVE3"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 1)
    DELETE_EVENT(LW(13))
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_URANOKO_MOVE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(10.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    WAIT_FRAMES(3)
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

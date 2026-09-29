#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
static u32 regist[6];
static u32 data_table[2];
static u32 parts[76];
static s32 init_event[21];
static s32 damage_event[10];
static s32 attack_event[71];
static s32 normal_attack_event[174];
static s32 renzoku_attack_event[178];
s32 normal_attack_event_confuse[322];
static s32 wait_event[6];

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_boomerang_bros[49] = {
    0x00000039, (u32)"btl_un_boomerang_bros", 0x00070000, 0x01011400,
    0x02460232, 0x270F0024, 0x002C0003, 0x002C0000,
    0x00000000, 0x41B00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41900000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x00000000, 0x00000000, 0x41200000, 0x41E00000,
    0x00000000, 0x00000000, 0x41B00000, 0x00000000,
    0x42100000, 0x42300000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_BOOME_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x04000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
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
static u32 regist[6] = {
    0x50504664, 0x50646446, 0x64506450, 0x645F5046,
    0x3C645064, 0x645F0000,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"BRO_N_1", 0x00000002, (u32)"BRO_Y_1",
    0x00000009, (u32)"BRO_Y_1", 0x00000005, (u32)"BRO_K_1",
    0x00000004, (u32)"BRO_X_1", 0x00000003, (u32)"BRO_X_1",
    0x0000001B, (u32)"BRO_D_1", 0x0000001C, (u32)"BRO_S_1",
    0x0000001D, (u32)"BRO_Q_1", 0x0000001E, (u32)"BRO_Q_1",
    0x0000001F, (u32)"BRO_S_1", 0x00000027, (u32)"BRO_D_1",
    0x0000002A, (u32)"BRO_R_1", 0x00000028, (u32)"BRO_W_1",
    0x00000038, (u32)"BRO_X_1", 0x00000039, (u32)"BRO_X_1",
    0x00000041, (u32)"BRO_T_1", 0x00000045, (u32)"BRO_S_1",
};

// TODO: type
static u32 data_table[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[76] = {
    0x00000001, (u32)"btl_un_boomerang_bros", (u32)"c_burosu_b", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x420C0000,
    0x00000000, 0x00000000, 0x42340000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_boomerang_bros", (u32)"c_burosu_b", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_boomerang_bros",
    (u32)"c_burosu_b", 0x00000000, 0x00000000, 0x00000000,
    0x41700000, 0x41C80000, 0x00000000, 0x00000000,
    0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table, 0x00000004, (u32)"btl_un_boomerang_bros", (u32)"c_burosu_b",
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x41C80000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x13010000, 0x00000000, (u32)&pose_table,
};

// TODO: type
u32 weapon_boomerang_bros_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 weapon_boomerang_bros_renzoku_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80000000,
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
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 7, 0)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetHp, -2, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 3)
        SET_VALUE(LW(0), 70)
        ADD(LW(0), 30)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 30)
            CALL_CHILD(renzoku_attack_event)
            GOTO(99)
        END_IF
    END_IF
    CALL_CHILD(normal_attack_event)
    GOTO(99)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_sub)
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(6), 0, LW(5))
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
    GOTO(99)
    LABEL(91)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
    IF_INT_EQUAL(LW(8), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(6))
        CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
    END_IF
    IF_INT_EQUAL(LW(7), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 0, LW(5))
    END_IF
    SET_VALUE(LW(0), 1)
    GOTO(99)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_no_last)
    SET_VALUE(LW(7), 0)
    CALL_CHILD(damage_check_sub)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_last)
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
        IF_INT_EQUAL(LW(8), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(6))
            CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 256, LW(5))
    ELSE
        SET_VALUE(LW(7), 1)
        CALL_CHILD(damage_check_sub)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(boomerang_move_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -180)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 0, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    SET_VALUE(LW(9), 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(10), LW(12), LW(13))
            IF_INT_EQUAL(LW(9), 2)
                CALL(btlevtcmd_SetUnitWork, -2, 7, 0)
            END_IF
            SWITCH(LW(9))
                CASE_EQUAL(0)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 1)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_GREATER_OR_EQUAL(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL_CHILD(damage_check_no_last)
                SET_VALUE(LW(9), 1)
                CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
                CASE_END
                CASE_EQUAL(1)
                CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 4, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 2)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_GREATER_OR_EQUAL(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL_CHILD(damage_check_no_last)
                SET_VALUE(LW(9), 2)
                CALL(btlevtcmd_SetUnitWork, -2, 6, LW(0))
                CASE_END
                CASE_EQUAL(2)
                CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 4, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 3)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_LESS(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetUnitWork, -2, 6, LW(0))
                IF_INT_EQUAL(LW(14), 0)
                    CALL_CHILD(damage_check_no_last)
                ELSE
                    CALL_CHILD(damage_check_last)
                END_IF
                SET_VALUE(LW(9), 3)
                CASE_END
                CASE_EQUAL(3)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(4))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(9), 4)
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_LESS(LW(10), LW(11))
                    BREAK_SWITCH
                END_IF
                CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
                IF_INT_EQUAL(LW(14), 0)
                    CALL_CHILD(damage_check_no_last)
                ELSE
                    CALL_CHILD(damage_check_last)
                END_IF
                SET_VALUE(LW(9), 4)
                CASE_END
                CASE_EQUAL(4)
                BREAK_LOOP
                CASE_END
            END_SWITCH
            LABEL(10)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 64)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), -200, LW(1), LW(2), 40, FLOAT(-20.0f), 4, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK3"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), LW(0), LW(1), LW(2), 40, FLOAT(20.0f), 1, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AddUnitWork, -2, 0, 1)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_boomerang_bros_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_boomerang_bros_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event_confuse)
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_boomerang_bros_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_boomerang_bros_attack))
    SET_VALUE(LW(0), 1)
    SET_VALUE(LW(1), 3)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    LABEL(10)
    CALL(btlevtcmd_SetUnitWork, -2, LW(0), LW(3))
    CALL(btlevtcmd_SetUnitWork, -2, LW(1), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    ADD(LW(0), 1)
    ADD(LW(1), 1)
    GOTO(10)
    LABEL(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(14), 1)
    SET_VALUE(LW(6), PTR(weapon_boomerang_bros_attack))
    SPAWN_EVENT(boomerang_move_event)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_S_1"))
    WAIT_MS(500)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_boomerang_bros_renzoku_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event_confuse)
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_boomerang_bros_renzoku_attack))
    SET_VALUE(LW(0), 1)
    SET_VALUE(LW(1), 3)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    LABEL(10)
    CALL(btlevtcmd_SetUnitWork, -2, LW(0), LW(3))
    CALL(btlevtcmd_SetUnitWork, -2, LW(1), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    ADD(LW(0), 1)
    ADD(LW(1), 1)
    GOTO(10)
    LABEL(20)
    SET_VALUE(LW(6), PTR(weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(14), 0)
    SPAWN_EVENT(boomerang_move_event)
    WAIT_MS(750)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 4)
    SET_VALUE(LW(14), 1)
    SPAWN_EVENT(boomerang_move_event)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 2)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_S_1"))
    WAIT_MS(500)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(normal_attack_event_confuse)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_boomerang_bros_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_boomerang_bros_attack))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -180)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 0, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 64)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), 250, 40, 0, 15, FLOAT(-20.0f), 4, 0, -1)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), -30, 40, 0, 15, FLOAT(20.0f), 1, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK3"), EVTDAT_POINTER_MAX, 0, LW(8))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(15), LW(0), LW(1), LW(2), 15, FLOAT(-10.0f), 1, 0, -1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_boomerang_bros_attack), 256, LW(5))
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
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
        IF_INT_EQUAL(LW(8), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_boomerang_bros_attack))
            CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        END_IF
        ADD(LW(10), 1)
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_boomerang_bros_attack), 0, LW(5))
        CALL(evt_sub_random, 10, LW(0))
        ADD(LW(0), 30)
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_boomerang_bros_attack), 256, LW(5))
    END_SIBLING_EVENT
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(0)
    END_IF
    LABEL(99)
    WAIT_FRAMES(100)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(renzoku_attack_event_confuse)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_boomerang_bros_renzoku_attack))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LOOP(2)
        CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
        WAIT_FRAMES(27)
        SET_VALUE(LW(15), 2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 20)
        CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -180)
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 0, 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 64)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(8))
        CALL(btlevtcmd_DivePartsPosition, -2, LW(15), 250, 40, 0, 15, FLOAT(-20.0f), 4, 0, -1)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        CALL(btlevtcmd_DivePartsPosition, -2, LW(15), -30, 40, 0, 15, FLOAT(20.0f), 1, 0, -1)
        CALL(evt_snd_sfxoff, LW(8))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK3"), EVTDAT_POINTER_MAX, 0, LW(8))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 20)
        CALL(btlevtcmd_DivePartsPosition, -2, LW(15), LW(0), LW(1), LW(2), 15, FLOAT(-10.0f), 1, 0, -1)
        CALL(evt_snd_sfxoff, LW(8))
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOOME_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_LOOP
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_boomerang_bros_renzoku_attack), 256, LW(5))
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
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(8))
        IF_INT_EQUAL(LW(8), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_boomerang_bros_renzoku_attack))
            CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
        END_IF
        ADD(LW(10), 1)
    END_IF
    BEGIN_SIBLING_EVENT
        LOOP(3)
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_boomerang_bros_renzoku_attack), 0, LW(5))
            CALL(evt_sub_random, 10, LW(0))
            ADD(LW(0), 30)
            WAIT_FRAMES(LW(0))
        END_LOOP
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_boomerang_bros_renzoku_attack), 256, LW(5))
    END_SIBLING_EVENT
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(0)
    END_IF
    LABEL(99)
    WAIT_FRAMES(160)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

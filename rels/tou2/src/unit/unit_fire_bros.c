#include "unit_fire_bros.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on
static u32 regist[6];
static u32 data_table[2];
static u32 parts[133];
static s32 init_event[21];
static s32 damage_event[10];
static s32 attack_event[92];
static s32 normal_attack_event[125];
static s32 renzoku_attack_event[141];
static s32 nagemakuri_attack_event[316];
s32 nagemakuri_judge_last_attack_event[73];
static s32 wait_event[6];

// clang-format off
extend_data(44,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(45,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_fire_bros[49] = {
    0x0000003A, (u32)"btl_un_fire_bros", 0x00070000, 0x01011400,
    0x02460232, 0x270F0024, 0x002C0003, 0x002C0000,
    0x00000000, 0x41B00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41900000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x00000000, 0x00000000, 0x41200000, 0x41E00000,
    0x00000000, 0x00000000, 0x41B00000, 0x00000000,
    0x42100000, 0x42300000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_FIREB_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x07000000, (u32)&parts, (u32)&init_event,
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
static u32 parts[133] = {
    0x00000001, (u32)"btl_un_fire_bros", (u32)"c_burosu_f", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x420C0000,
    0x00000000, 0x00000000, 0x42340000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_fire_bros", (u32)"c_burosu_f", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_fire_bros",
    (u32)"c_burosu_f", 0x00000000, 0x00000000, 0x00000000,
    0x41700000, 0x41C80000, 0x00000000, 0x00000000,
    0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table, 0x00000004, (u32)"btl_un_fire_bros", (u32)"c_burosu_f",
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x41C80000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x13010000, 0x00000000, (u32)&pose_table,
    0x00000005, (u32)"btl_un_fire_bros", (u32)"c_burosu_f", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x13010000, 0x00000000, (u32)&pose_table, 0x00000006,
    (u32)"btl_un_fire_bros", (u32)"c_burosu_f", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table, 0x00000007, (u32)"btl_un_fire_bros",
    (u32)"c_burosu_f", 0x00000000, 0x00000000, 0x00000000,
    0x41700000, 0x41C80000, 0x00000000, 0x00000000,
    0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table,
};

// TODO: type
u32 weapon_fire_bros_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x01000302,
    0x00000000, 0x00000007, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 weapon_fire_bros_renzoku_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x01000302,
    0x00000000, 0x00000007, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 weapon_fire_bros_nagemakuri_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x01000302,
    0x00000000, 0x00000007, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
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
        SET_VALUE(LW(0), 50)
        ADD(LW(0), 50)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 30)
            CALL_CHILD(renzoku_attack_event)
            GOTO(99)
        END_IF
        CALL_CHILD(nagemakuri_attack_event)
        GOTO(99)
    ELSE
        SET_VALUE(LW(0), 70)
        ADD(LW(0), 30)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 70)
            CALL_CHILD(normal_attack_event)
            GOTO(99)
        END_IF
        CALL_CHILD(renzoku_attack_event)
        GOTO(99)
    END_IF
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
    SET_VALUE(LW(0), 1)
    IF_INT_EQUAL(LW(6), PTR(weapon_fire_bros_renzoku_attack))
        CALL(btlevtcmd_SetPartsWork, -2, 2, 10, 1)
    END_IF
    IF_INT_EQUAL(LW(8), 1)
        SET_VALUE(LW(13), 0)
        SET_VALUE(LW(14), 5)
        LOOP(0)
            SET_VALUE(LW(12), 2)
            LOOP(0)
                CALL(btlevtcmd_GetPartsWork, -2, LW(12), LW(13), LW(1))
                CALL(btlevtcmd_GetPartsWork, -2, LW(12), LW(14), LW(2))
                IF_INT_EQUAL(LW(3), LW(1))
                    IF_INT_EQUAL(LW(4), LW(2))
                        ADD(LW(13), 10)
                        CALL(btlevtcmd_SetPartsWork, -2, LW(12), LW(13), 1)
                        GOTO(98)
                    END_IF
                END_IF
                ADD(LW(12), 1)
                IF_INT_GREATER(LW(12), 7)
                    BREAK_LOOP
                END_IF
            END_LOOP
            ADD(LW(13), 1)
            ADD(LW(14), 1)
        END_LOOP
    END_IF
    LABEL(98)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(6))
    IF_INT_EQUAL(LW(7), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 0, LW(5))
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_no_last)
    SET_VALUE(LW(7), 0)
    CALL_CHILD(damage_check_sub)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_last)
    IF_INT_EQUAL(LW(6), PTR(weapon_fire_bros_renzoku_attack))
        CALL(btlevtcmd_GetPartsWork, -2, 2, 10, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            SET_VALUE(LW(0), 1)
        ELSE
            SET_VALUE(LW(0), 0)
        END_IF
        GOTO(10)
    END_IF
    IF_INT_EQUAL(LW(8), 1)
        SET_VALUE(LW(13), 0)
        SET_VALUE(LW(14), 5)
        LOOP(0)
            SET_VALUE(LW(12), 2)
            LOOP(0)
                CALL(btlevtcmd_GetPartsWork, -2, LW(12), LW(13), LW(1))
                CALL(btlevtcmd_GetPartsWork, -2, LW(12), LW(14), LW(2))
                IF_INT_EQUAL(LW(3), LW(1))
                    IF_INT_EQUAL(LW(4), LW(2))
                        ADD(LW(13), 10)
                        CALL(btlevtcmd_GetPartsWork, -2, LW(12), LW(13), LW(0))
                        GOTO(10)
                    END_IF
                END_IF
                ADD(LW(12), 1)
                IF_INT_GREATER(LW(12), 7)
                    BREAK_LOOP
                END_IF
            END_LOOP
            ADD(LW(13), 1)
            ADD(LW(14), 1)
        END_LOOP
    END_IF
    LABEL(10)
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(6))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(6), 256, LW(5))
    ELSE
        SET_VALUE(LW(7), 1)
        CALL_CHILD(damage_check_sub)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(fire_move_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_SetPartsWork, -2, LW(15), 15, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 15)
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -180)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 0, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(9), LW(10), LW(11))
    SUBTRACT(LW(9), LW(0))
    SET_VALUE(LW(10), 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(12), LW(13))
            SWITCH(LW(10))
                CASE_EQUAL(0)
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), LW(12), LW(13))
                IF_INT_GREATER_OR_EQUAL(LW(9), 0)
                    IF_INT_LESS_OR_EQUAL(LW(0), LW(11))
                        BREAK_SWITCH
                    END_IF
                ELSE
                    IF_INT_GREATER_OR_EQUAL(LW(0), LW(11))
                        BREAK_SWITCH
                    END_IF
                END_IF
                IF_INT_EQUAL(LW(14), 0)
                    CALL_CHILD(damage_check_no_last)
                ELSE
                    CALL_CHILD(damage_check_last)
                END_IF
                IF_INT_EQUAL(LW(0), 1)
                    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
                    SET_VALUE(LW(10), 2)
                ELSE
                    SET_VALUE(LW(10), 1)
                END_IF
                CASE_END
                CASE_EQUAL(1)
                CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(11), LW(12), LW(13))
                IF_INT_GREATER_OR_EQUAL(LW(11), 250)
                    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
                    SET_VALUE(LW(10), 2)
                END_IF
                IF_INT_LESS_OR_EQUAL(LW(11), -250)
                    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
                    SET_VALUE(LW(10), 2)
                END_IF
                CASE_END
                CASE_EQUAL(2)
                CALL(btlevtcmd_SetPartsWork, -2, LW(15), 15, 1)
                BREAK_LOOP
                CASE_END
            END_SWITCH
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 16)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_sub_random, 1, LW(11))
    IF_INT_EQUAL(LW(11), 0)
        CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, 6, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    ELSE
        CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, 5, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        IF_INT_EQUAL(LW(10), 2)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(0), 0, LW(2), 0, -1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(15), 15, LW(10))
        IF_INT_EQUAL(LW(10), 1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FIREB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    LABEL(99)
    WAIT_FRAMES(6)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    CALL(btlevtcmd_AddUnitWork, -2, 1, 1)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_fire_bros_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_fire_bros_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_fire_bros_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_fire_bros_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_1A"))
    WAIT_FRAMES(40)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(14), 1)
    SET_VALUE(LW(6), PTR(weapon_fire_bros_attack))
    SET_VALUE(LW(8), 0)
    SPAWN_EVENT(fire_move_event)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_FRAMES(20)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_fire_bros_renzoku_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_fire_bros_renzoku_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_fire_bros_renzoku_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_fire_bros_renzoku_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 2, 10, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(20)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(14), 0)
    SET_VALUE(LW(6), PTR(weapon_fire_bros_renzoku_attack))
    SET_VALUE(LW(8), 0)
    SPAWN_EVENT(fire_move_event)
    WAIT_FRAMES(12)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
    WAIT_FRAMES(20)
    SET_VALUE(LW(15), 3)
    SET_VALUE(LW(14), 1)
    SET_VALUE(LW(6), PTR(weapon_fire_bros_renzoku_attack))
    SET_VALUE(LW(8), 0)
    SPAWN_EVENT(fire_move_event)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 2)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_FRAMES(20)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

// TODO: write fire_bros_nagemakuri_set_target (global, 0xd0 bytes) here:
// its asm is in build/G8MJ01/tou2/asm/unit/unit_fire_bros.s; this is where the original has it, just before the first script that uses it
USER_FUNC(fire_bros_nagemakuri_set_target) {
    BattleWork* wp = _battleWorkPointer;
    EventEntry* evt = event;
    s32* args = evt->args;
    s32 ids[20];
    s32 parts[20];
    s32 i = 0;
    s32 count = 0;

    ((s8*)wp)[0xEDF] = 0;
    fire_bros_get_target(&ids[0], &parts[0]);
    while (1) {
        if (ids[i] == -1) { count = i; break; }
        i++;
        fire_bros_get_next_target(&ids[i], &parts[i]);
    }
    for (i = 0; i < 5; i++) {
        s32 idx = irand(count);
        evtSetValue(evt, *(args++), ids[idx]);
        evtSetValue(evt, *(args++), parts[idx]);
    }
    return 2;
}

static EVT_BEGIN(nagemakuri_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_fire_bros_nagemakuri_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_fire_bros_nagemakuri_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_fire_bros_nagemakuri_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_fire_bros_nagemakuri_attack))
    SET_VALUE(LW(10), 2)
    LOOP(0)
        IF_INT_GREATER(LW(10), 7)
            BREAK_LOOP
        END_IF
        CALL(fire_bros_nagemakuri_set_target, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 0, LW(0))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 1, LW(2))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 2, LW(4))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 3, LW(6))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 4, LW(8))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 5, LW(1))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 6, LW(3))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 7, LW(5))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 8, LW(7))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 9, LW(9))
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 10, 0)
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 11, 0)
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 12, 0)
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 13, 0)
        CALL(btlevtcmd_SetPartsWork, -2, LW(10), 14, 0)
        ADD(LW(10), 1)
    END_LOOP
    WAIT_FRAMES(12)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 5)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    LOOP(0)
        SET_VALUE(LW(15), 2)
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
            IF_INT_GREATER_OR_EQUAL(LW(1), 6)
                LOOP(0)
                    WAIT_FRAMES(1)
                    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
                    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(2))
                    SUBTRACT(LW(1), 6)
                    IF_INT_GREATER(LW(2), LW(1))
                        WAIT_FRAMES(3)
                        BREAK_LOOP
                    END_IF
                END_LOOP
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_2A"))
            WAIT_FRAMES(20)
            CALL(btlevtcmd_GetPartsWork, -2, LW(15), LW(10), LW(3))
            CALL(btlevtcmd_GetPartsWork, -2, LW(15), LW(11), LW(4))
            CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(14))
            CALL(btlevtcmd_ChangeFaceDirection, -2, LW(14))
            CALL_CHILD(nagemakuri_judge_last_attack_event)
            SET_VALUE(LW(6), PTR(weapon_fire_bros_nagemakuri_attack))
            SET_VALUE(LW(8), 1)
            SPAWN_EVENT(fire_move_event)
            CALL(btlevtcmd_AddUnitWork, -2, 0, 1)
            WAIT_FRAMES(15)
            ADD(LW(15), 1)
            IF_INT_GREATER(LW(15), 7)
                BREAK_LOOP
            END_IF
        END_LOOP
        ADD(LW(10), 1)
        ADD(LW(11), 1)
        IF_INT_GREATER(LW(10), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 6)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

EVT_BEGIN(nagemakuri_judge_last_attack_event)
    SET_VALUE(LW(0), LW(10))
    SET_VALUE(LW(1), LW(11))
    SET_VALUE(LW(13), LW(15))
    ADD(LW(13), 1)
    LOOP(0)
        IF_INT_GREATER(LW(0), 0)
            SET_VALUE(LW(14), 1)
            GOTO(99)
        END_IF
        LOOP(0)
            IF_INT_GREATER(LW(13), 7)
                SET_VALUE(LW(13), 2)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_GetPartsWork, -2, LW(13), LW(0), LW(5))
            CALL(btlevtcmd_GetPartsWork, -2, LW(13), LW(1), LW(6))
            IF_INT_EQUAL(LW(3), LW(5))
                IF_INT_EQUAL(LW(4), LW(6))
                    SET_VALUE(LW(14), 0)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(13), 1)
        END_LOOP
        ADD(LW(0), 1)
        ADD(LW(1), 1)
    END_LOOP
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// deferred inlining emits these in reverse source order, so get_target is defined before get_next
// to land get_next_target first in .text, matching the original.
#pragma dont_inline on  // the original keeps these as real calls in fire_bros_nagemakuri_set_target, not inlined
void fire_bros_get_target(s32* outUnit, s32* outPart) {
    BattleWork* wp = _battleWorkPointer;
    u8* sampling = wp->field_428;
    s32 idx = (s8)sampling[0xAB7];
    s16* entry = (s16*)(sampling + (s8)sampling[0xA6D + idx] * 0x24 + 4);

    *outUnit = entry[0];
    *outPart = entry[1];
}

void fire_bros_get_next_target(s32* outUnit, s32* outPart) {
    BattleWork* wp = _battleWorkPointer;
    u8* sampling = wp->field_428;
    s16* entry;

    sampling[0xAB7]++;
    if ((s8)sampling[0xAB7] < (s8)sampling[0xA6C]) {
        entry = (s16*)(sampling + (s8)sampling[0xA6D + (s8)sampling[0xAB7]] * 0x24 + 4);
        *outUnit = entry[0];
        *outPart = entry[1];
    } else {
        *outUnit = -1;
        *outPart = 0;
    }
}
#pragma dont_inline reset
// clang-format on
#pragma force_active off

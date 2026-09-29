#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static void hammer_bros_get_next_target(s32* unitId, s32* partId);
static void hammer_bros_get_target(s32* unitId, s32* partId);
static USER_FUNC(hammer_bros_nagemakuri_set_target);
static u32 regist[6];
static u32 data_table[2];
static u32 parts[114];
static s32 init_event[21];
static s32 damage_event[10];
static s32 attack_event[96];
static s32 normal_attack_event[112];
static s32 renzoku_attack_event[111];
static s32 renzoku_attack_event_sub_no_last[13];
static s32 renzoku_attack_event_sub_last[39];
static s32 nagemakuri_attack_event[186];
static s32 nagemakuri_attack_event_sub[49];
static s32 nagemakuri_attack_event_sub_no_last[13];
static s32 nagemakuri_attack_event_sub_last[76];
static s32 wait_event[6];

// clang-format off
extend_data(unit_hammer_bros_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_hammer_bros_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x00000038, (u32)"btl_un_hammer_bros", 0x00070000, 0x01011A00,
    0x02460232, 0x270F0024, 0x002C0003, 0x002C0000,
    0x00000000, 0x41B00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41900000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x00000000, 0x00000000, 0x41200000, 0x41E00000,
    0x00000000, 0x00000000, 0x41B00000, 0x00000000,
    0x42100000, 0x42300000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_HUMMER_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x06000000, (u32)&parts, (u32)&init_event,
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
static u32 parts[114] = {
    0x00000001, (u32)"btl_un_hammer_bros", (u32)"c_burosu_h", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x420C0000,
    0x00000000, 0x00000000, 0x42340000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_hammer_bros", (u32)"c_burosu_h", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_hammer_bros",
    (u32)"c_burosu_h", 0x00000000, 0x00000000, 0x00000000,
    0x41700000, 0x41C80000, 0x00000000, 0x00000000,
    0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table, 0x00000004, (u32)"btl_un_hammer_bros", (u32)"c_burosu_h",
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x41C80000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x13010000, 0x00000000, (u32)&pose_table,
    0x00000005, (u32)"btl_un_hammer_bros", (u32)"c_burosu_h", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x13010000, 0x00000000, (u32)&pose_table, 0x00000006,
    (u32)"btl_un_hammer_bros", (u32)"c_burosu_h", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_hammer_bros_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_hammer_bros_renzoku_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_hammer_bros_nagemakuri_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20001000, 0x00000302,
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
    CALL(btlevtcmd_SetUnitWork, -2, 10, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 11, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 12, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 13, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 14, 0)
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
        IF_INT_LESS(LW(1), 50)
            CALL_CHILD(renzoku_attack_event)
            GOTO(99)
        END_IF
        CALL_CHILD(nagemakuri_attack_event)
        GOTO(99)
    ELSE
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(start_avoid)
    IF_INT_EQUAL(LW(5), 3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
    END_IF
    IF_INT_EQUAL(LW(5), 6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
    END_IF
    IF_INT_EQUAL(LW(5), 2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(hammer_bros_attack_common_event1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
    MULTIPLY(LW(0), 10)
    ADD(LW(6), LW(0))
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -135)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 5, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(10)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 32)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            ELSE
                CALL(btlevtcmd_GetPartsPos, -2, LW(15), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 60, 0, FLOAT(0.2f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 60, -1)
    CALL(btlevtcmd_JumpPartsContinue, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(6), 100)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, FLOAT(4.0f), FLOAT(0.6f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), 0, LW(8), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(6), 150)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, FLOAT(4.0f), FLOAT(0.4f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), 0, LW(8), 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    RETURN
EVT_END()

static EVT_BEGIN(hammer_bros_attack_common_event2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
    MULTIPLY(LW(0), 10)
    ADD(LW(6), LW(0))
    CALL(btlevtcmd_SetPartsPos, -2, LW(15), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_SetPartsRotate, -2, LW(15), 0, 0, -135)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(15), 0, 5, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(15), 33554432)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HUMMER_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(10)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 32)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 60, 0, FLOAT(0.2f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), LW(7), LW(8), 60, -1)
    RETURN
EVT_END()

static EVT_BEGIN(hammer_bros_attack_common_event3)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(15), 0, 0, 32)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, LW(15), STRING("BRO_A_1B"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(6), 50)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(15), 0, FLOAT(2.0f), FLOAT(0.5f))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(15), LW(6), 0, LW(8), 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(15), 33554432)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_event_sub)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        GOTO(90)
        CASE_EQUAL(2)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL_CHILD(hammer_bros_attack_common_event1)
    GOTO(99)
    LABEL(91)
    IF_INT_EQUAL(LW(11), 1)
        IF_INT_EQUAL(LW(9), PTR(weapon_hammer_bros_renzoku_attack))
            CALL(btlevtcmd_SetUnitWork, -2, 10, 1)
        ELSE
            SET_VALUE(LW(13), 0)
            SET_VALUE(LW(14), 5)
            LOOP(0)
                CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(5))
                CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(6))
                IF_INT_EQUAL(LW(3), LW(5))
                    IF_INT_EQUAL(LW(4), LW(6))
                        ADD(LW(13), 10)
                        CALL(btlevtcmd_SetUnitWork, -2, LW(13), 1)
                        BREAK_LOOP
                    END_IF
                END_IF
                ADD(LW(13), 1)
                ADD(LW(14), 1)
            END_LOOP
        END_IF
    END_IF
    CALL_CHILD(hammer_bros_attack_common_event2)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    IF_INT_EQUAL(LW(10), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    END_IF
    CALL_CHILD(hammer_bros_attack_common_event3)
    GOTO(99)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_hammer_bros_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_hammer_bros_attack), LW(3), LW(4))
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
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_hammer_bros_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_hammer_bros_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_1A"))
    WAIT_FRAMES(27)
    SET_VALUE(LW(15), 2)
    SET_VALUE(LW(9), PTR(weapon_hammer_bros_attack))
    SET_VALUE(LW(10), 1)
    SET_VALUE(LW(11), 0)
    CALL_CHILD(damage_check_event_sub)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_hammer_bros_renzoku_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_hammer_bros_renzoku_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_hammer_bros_renzoku_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_hammer_bros_renzoku_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(14), 0)
    SET_VALUE(LW(15), 2)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(14), 4)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_1A"))
        WAIT_FRAMES(27)
        IF_INT_EQUAL(LW(14), 3)
            SPAWN_EVENT(renzoku_attack_event_sub_last)
        ELSE
            SPAWN_EVENT(renzoku_attack_event_sub_no_last)
        END_IF
        WAIT_MS(250)
        ADD(LW(14), 1)
        ADD(LW(15), 1)
    END_LOOP
    LABEL(98)
    WAIT_MS(1500)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event_sub_no_last)
    SET_VALUE(LW(9), PTR(weapon_hammer_bros_renzoku_attack))
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(damage_check_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event_sub_last)
    CALL(btlevtcmd_GetUnitWork, -2, 10, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(hammer_bros_attack_common_event2)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_hammer_bros_renzoku_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_hammer_bros_renzoku_attack), 256, LW(5))
        CALL_CHILD(hammer_bros_attack_common_event3)
    ELSE
        SET_VALUE(LW(9), PTR(weapon_hammer_bros_renzoku_attack))
        SET_VALUE(LW(10), 1)
        SET_VALUE(LW(11), 0)
        CALL_CHILD(damage_check_event_sub)
    END_IF
    RETURN
EVT_END()

typedef struct HammerBrosSamplingEntry {
    s16 unitId; // 0x0
    s16 partId; // 0x2
    u8 pad[0x24 - 4];
} HammerBrosSamplingEntry;

typedef struct HammerBrosSampling {
    u8 pad_0x0[4];
    HammerBrosSamplingEntry entries[1]; // 0x4
} HammerBrosSampling;

#pragma dont_inline on
// NONMATCHING (68.52%): the register assignment of the locals and the loop shape differ; the helpers below carry the same
// "addi before lbz" scheduling difference.
static USER_FUNC(hammer_bros_nagemakuri_set_target) {
    s32* args;
    s32 unitIds[20];
    s32 partIds[20];
    s32 i;
    s32 count;
    s32 total;
    s32 n;
    s32 pick;

    i = 0;
    count = 0;
    args = event->args;
    *((u8*)_battleWorkPointer + 0xEDF) = i;
    hammer_bros_get_target(unitIds, partIds);
    while (unitIds[i] != -1) {
        i++;
        count++;
        hammer_bros_get_next_target(&unitIds[i], &partIds[i]);
    }
    total = count;
    for (n = 0; n < 5; n++) {
        pick = irand(total);
        evtSetValue(event, args[n * 2], unitIds[pick]);
        evtSetValue(event, args[n * 2 + 1], partIds[pick]);
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (88.24%): the original computes wp+0x428 before loading the selected index (lbz 0xedf); ours loads first.
static void hammer_bros_get_target(s32* unitId, s32* partId) {
    BattleWork* wp = _battleWorkPointer;
    u8* base = (u8*)wp + 0x428;
    HammerBrosSamplingEntry* entry;

    entry = (HammerBrosSamplingEntry*)(base + 4 + (s8)base[0xA6D + (s8)((u8*)wp)[0xEDF]] * 0x24);
    *unitId = entry->unitId;
    *partId = entry->partId;
}

// NONMATCHING (89.31%): same scheduling difference, and the original loads _battleWorkPointer with lis/addi/lwz.
static void hammer_bros_get_next_target(s32* unitId, s32* partId) {
    BattleWork* wp = _battleWorkPointer;
    u8* base = (u8*)wp + 0x428;
    HammerBrosSamplingEntry* entry;

    base[0xAB7] = ((u8*)wp)[0xEDF] + 1;
    if ((s8)base[0xAB7] < (s8)base[0xA6C]) {
        entry = (HammerBrosSamplingEntry*)(base + 4 + (s8)base[0xA6D + (s8)base[0xAB7]] * 0x24);
        *unitId = entry->unitId;
        *partId = entry->partId;
    } else {
        *unitId = -1;
        *partId = 0;
    }
}
#pragma dont_inline reset


static EVT_BEGIN(nagemakuri_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_hammer_bros_nagemakuri_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_hammer_bros_nagemakuri_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_hammer_bros_nagemakuri_attack))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_hammer_bros_nagemakuri_attack))
    CALL(hammer_bros_nagemakuri_set_target, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(2))
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(6))
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(8))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(1))
    CALL(btlevtcmd_SetUnitWork, -2, 6, LW(3))
    CALL(btlevtcmd_SetUnitWork, -2, 7, LW(5))
    CALL(btlevtcmd_SetUnitWork, -2, 8, LW(7))
    CALL(btlevtcmd_SetUnitWork, -2, 9, LW(9))
    SET_VALUE(LW(12), 0)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 5)
    SET_VALUE(LW(15), 2)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(12), 4)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(3))
        CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(4))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(10))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(10))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BRO_A_1A"))
        WAIT_FRAMES(27)
        SPAWN_EVENT(nagemakuri_attack_event_sub)
        WAIT_MS(250)
        ADD(LW(12), 1)
        ADD(LW(13), 1)
        ADD(LW(14), 1)
        ADD(LW(15), 1)
    END_LOOP
    LABEL(98)
    WAIT_MS(1500)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(nagemakuri_attack_event_sub)
    CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(3))
    CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(4))
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(13), 3)
            CALL_CHILD(nagemakuri_attack_event_sub_last)
            BREAK_LOOP
        END_IF
        ADD(LW(13), 1)
        ADD(LW(14), 1)
        CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(5))
        CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(6))
        IF_INT_EQUAL(LW(3), LW(5))
            IF_INT_EQUAL(LW(4), LW(6))
                CALL_CHILD(nagemakuri_attack_event_sub_no_last)
                BREAK_LOOP
            END_IF
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(nagemakuri_attack_event_sub_no_last)
    SET_VALUE(LW(9), PTR(weapon_hammer_bros_nagemakuri_attack))
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(damage_check_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(nagemakuri_attack_event_sub_last)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 5)
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(5))
        CALL(btlevtcmd_GetUnitWork, -2, LW(14), LW(6))
        IF_INT_EQUAL(LW(3), LW(5))
            IF_INT_EQUAL(LW(4), LW(6))
                ADD(LW(13), 10)
                CALL(btlevtcmd_GetUnitWork, -2, LW(13), LW(0))
                BREAK_LOOP
            END_IF
        END_IF
        ADD(LW(13), 1)
        ADD(LW(14), 1)
    END_LOOP
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(hammer_bros_attack_common_event2)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_hammer_bros_nagemakuri_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_hammer_bros_nagemakuri_attack), 256, LW(5))
        CALL_CHILD(hammer_bros_attack_common_event3)
    ELSE
        SET_VALUE(LW(9), PTR(weapon_hammer_bros_nagemakuri_attack))
        SET_VALUE(LW(10), 1)
        SET_VALUE(LW(11), 0)
        CALL_CHILD(damage_check_event_sub)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()



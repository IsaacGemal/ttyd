#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
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

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
extern s32 UNIT_REF_HONENOKO[];
static USER_FUNC(honenoko_copy_status);
static u32 regist[6];
static u32 parts[76];
static u32 pose_table[38];
static u32 data_table[8];
static s32 init_event[31];
static s32 damage_event[10];
static s32 dead_event[190];
static s32 fake_dead_event[165];
static s32 attack_event[185];
static s32 normal_attack_event[428];
static s32 call_friend_event[311];
static s32 wait_event[6];

// clang-format off
extend_data(unit_red_honenoko_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_red_honenoko_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x00000016, (u32)"btl_un_red_honenoko", 0x00050000, 0x01010E00,
    0x01640232, 0x00090024, 0x0028FFFC, 0x00280000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41100000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41900000,
    0x00000000, 0x00000000, 0x41100000, 0x41C00000,
    0x00000000, 0x00000000, 0x41A00000, 0x00000000,
    0x42100000, 0x42200000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_HONENOKO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00010000,
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
    0x00552800, 0x28645A50, 0x645A645A, 0x645A5000,
    0x50645064, 0x64000000,
};

// TODO: type
static u32 parts[76] = {
    0x00000001, (u32)"btl_un_red_honenoko", (u32)"c_karon_r_h", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x420C0000,
    0x00000000, 0x00000000, 0x42340000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_red_honenoko", (u32)"c_karon_r_h", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_red_honenoko",
    (u32)"c_nagehone_g", 0x00000000, 0x00000000, 0x00000000,
    0x41700000, 0x41C80000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table, 0x00000004, (u32)"btl_un_red_honenoko", (u32)"c_karon_g_h",
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x41C80000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x13010000, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_honenoko_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 entry_honenoko[12] = {
    (u32)UNIT_REF_HONENOKO, 0x01000000, 0x04000004, 0xC47A0000,
    0xC47A0000, 0xC47A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[38] = {
    0x00000001, (u32)"KRN_N_1", 0x00000002, (u32)"KRN_Y_1",
    0x00000009, (u32)"KRN_Y_1", 0x00000005, (u32)"KRN_K_1",
    0x00000004, (u32)"KRN_X_1", 0x00000003, (u32)"KRN_X_1",
    0x0000001B, (u32)"KRN_Z_2", 0x0000001C, (u32)"KRN_S_1",
    0x0000001D, (u32)"KRN_Q_1", 0x0000001E, (u32)"KRN_Q_1",
    0x0000001F, (u32)"KRN_S_1", 0x00000027, (u32)"KRN_D_1",
    0x00000032, (u32)"KRN_A_1", 0x0000002A, (u32)"KRN_R_1",
    0x00000028, (u32)"KRN_W_1", 0x00000038, (u32)"KRN_X_1",
    0x00000039, (u32)"KRN_X_1", 0x00000041, (u32)"KRN_T_1",
    0x00000045, (u32)"KRN_S_1",
};

// TODO: type
static u32 data_table[8] = {
    0x00000031, (u32)&dead_event, 0x0000000F, (u32)&fake_dead_event,
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000000, 0x00000000,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("0"), STRING("SFX_ENM_HONENOKO_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    SET_VALUE(LW(10), -2)
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
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
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
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, LW(10), 0)
    ELSE
        CALL(btlevtcmd_KillUnit, LW(10), 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(fake_dead_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(2), LW(3), LW(4))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(2), LW(3), LW(4), 0, -1, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
        SET_VALUE(LW(10), -2)
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
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_D_2"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_CRUMBLE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(1000)
    CALL(btlevtcmd_KillUnit, -2, 2)
    CALL(evt_sub_random, 2, LW(0))
    ADD(LW(0), 2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    CALL(btlevtcmd_SetHitOffset, LW(10), LW(11), 0, -20, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(rebirth_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_D_3"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_D_4"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_OffAttribute, -2, 131072)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    CALL(btlevtcmd_SetHp, -2, LW(0))
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(rebirth_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    LABEL(0)
    SET_VALUE(LW(0), 60)
    ADD(LW(0), 0)
    ADD(LW(0), 40)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 60)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(1)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
        SET_VALUE(LW(4), LW(0))
        CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
        MULTIPLY(LW(4), LW(5))
        IF_INT_LESS(LW(4), 0)
            SUBTRACT(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL_CHILD(call_friend_event)
                WAIT_MS(500)
                GOTO(99)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
        IF_INT_LESS_OR_EQUAL(LW(0), 170)
            ADD(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
                CALL_CHILD(call_friend_event)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                WAIT_MS(500)
                GOTO(99)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    GOTO(0)
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

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_honenoko_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_honenoko_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_honenoko_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_honenoko_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_1"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_2"))
    WAIT_MS(250)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_SEND_GROUP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_BONE_THROW1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KRN_H_1"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetPartsPos, -2, 1, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(14))
            IF_INT_EQUAL(LW(14), 1)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AddPartsRotate, -2, 2, 0, 0, 25)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_honenoko_attack), 256, LW(5))
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
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            ELSE
                CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                    CALL_CHILD(start_avoid)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 14)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(9))
    MULTIPLY(LW(0), LW(9))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(6.0f), FLOAT(0.05f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, 0)
    CALL(evt_snd_sfxoff, LW(15))
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 14)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(6.0f), FLOAT(0.2f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, 0)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_honenoko_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_honenoko_attack), 256, LW(5))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), -50)
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(2.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, 0)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
    WAIT_FRAMES(10)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static USER_FUNC(honenoko_copy_status) {
    s32* args;
    s32 srcId;
    s32 dstId;
    BattleWork* wp;
    BattleUnit* src;
    BattleUnit* dst;

    args = event->args;
    srcId = BattleTransID(event, evtGetValue(event, args[0]));
    dstId = BattleTransID(event, evtGetValue(event, args[1]));
    wp = _battleWorkPointer;
    src = BattleGetUnitPtr(wp, srcId);
    dst = BattleGetUnitPtr(wp, dstId);
    dst->mSizeChangeTurns = src->mSizeChangeTurns;
    dst->mSizeChangeStrength = src->mSizeChangeStrength;
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(call_friend_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    SET_VALUE(LW(10), 0)
    LOOP(6)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_3"))
        WAIT_FRAMES(5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_A_4"))
        WAIT_FRAMES(15)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_SEND_GROUP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SWITCH(LW(10))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRN_H_4"))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRN_H_5"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRN_H_5"))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRN_H_3"))
            CASE_EQUAL(4)
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRN_H_3"))
            CASE_DEFAULT
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KRN_H_2"))
        END_SWITCH
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 50331648)
        CALL(btlevtcmd_GetPartsPos, -2, 1, LW(4), LW(5), LW(6))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(4), 15)
        ADD(LW(5), 14)
        CALL(btlevtcmd_SetPartsPos, -2, 3, LW(4), LW(5), LW(6))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
            SET_VALUE(LW(15), 0)
            LOOP(0)
                CALL(btlevtcmd_GetUnitWork, -2, 0, LW(14))
                IF_INT_EQUAL(LW(14), 1)
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_SetPartsRotate, -2, 3, 0, 0, LW(15))
                ADD(LW(15), 25)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_JumpPartsSetting, -2, 3, 0, FLOAT(6.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPartsPosition, -2, 3, LW(0), LW(1), LW(2), 20, 0)
        CALL(btlevtcmd_GetPartsPos, -2, 3, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, -2, 4, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_SEND_GROUP2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SWITCH(LW(10))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_C_5"))
            CALL(btlevtcmd_OffPartsAttribute, -2, 4, 50331648)
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_C_4"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_C_3"))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_C_2"))
            CASE_EQUAL(4)
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_C_1"))
            CASE_DEFAULT
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_Z_2"))
        END_SWITCH
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 50331648)
        ADD(LW(10), 1)
    END_LOOP
    WAIT_FRAMES(15)
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_D_3"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("KRN_D_4"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HONENOKO_UNITE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KRN_S_1"))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_honenoko), 0)
    CALL(btlevtcmd_OnPartsAttribute, -2, 4, 50331648)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(honenoko_copy_status, -2, LW(3))
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on

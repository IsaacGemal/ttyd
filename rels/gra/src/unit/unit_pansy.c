#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static u32 regist[6];
static u32 parts[19];
static u32 pose_table[36];
static u32 data_table[4];
static s32 init_event[21];
static s32 damage_event[10];
static s32 song_event[300];
static s32 escape_event[96];
static s32 wait_event[6];

// clang-format off
extend_data(68,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(69,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_pansy[49] = {
    0x00000045, (u32)"btl_un_pansy", 0x00070000, 0x01011000,
    0x02460146, 0x270F0016, 0x00200005, 0x00200000,
    0x00000000, 0x41800000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41300000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41300000,
    0x00000000, 0x00000000, 0x40C00000, 0x41A66666,
    0x00000000, 0x00000000, 0x41800000, 0x00000000,
    0x41B00000, 0x42000000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_PANSY_DAMAGED1",
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
    0x5F1E1E64, 0x3264641E, 0x641E641E, 0x641E1E64,
    0x50641E64, 0x641E0000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_pansy", (u32)"c_pansy", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41E80000,
    0x00000000, 0x00000000, 0x42080000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
u32 weapon_pansy_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000047, 0x000007FF, 0x80000000,
    0x4B050000, 0x00000000, 0x00000000, 0x00000000,
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
    CALL(btlevtcmd_GetHp, -2, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        GOTO(10)
    END_IF
    SET_VALUE(LW(0), 70)
    ADD(LW(0), 30)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    IF_INT_LESS(LW(1), 70)
        SET_VALUE(LW(9), PTR(weapon_pansy_attack))
        CALL_CHILD(song_event)
        GOTO(99)
    END_IF
    LABEL(10)
    CALL_CHILD(escape_event)
    GOTO(99)
    LABEL(99)
    RETURN
EVT_END()

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"PAN_N_1", 0x00000002, (u32)"PAN_Y_1",
    0x00000009, (u32)"PAN_Y_1", 0x00000005, (u32)"PAN_K_1",
    0x00000004, (u32)"PAN_X_1", 0x00000003, (u32)"PAN_X_1",
    0x0000001C, (u32)"PAN_S_1", 0x0000001D, (u32)"PAN_Q_1",
    0x0000001E, (u32)"PAN_Q_1", 0x0000001F, (u32)"PAN_S_1",
    0x00000027, (u32)"PAN_D_1", 0x00000032, (u32)"PAN_A_1",
    0x0000002A, (u32)"PAN_R_1", 0x00000028, (u32)"PAN_W_1",
    0x00000038, (u32)"PAN_X_1", 0x00000039, (u32)"PAN_X_1",
    0x00000041, (u32)"PAN_T_1", 0x00000045, (u32)"PAN_S_1",
};

// TODO: type
static u32 data_table[4] = {
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000000, 0x00000000,
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

static EVT_BEGIN(song_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(0), 255)
    SET_VALUE(LW(1), 255)
    LOOP(0)
        IF_INT_LESS(LW(0), 192)
            BREAK_LOOP
        END_IF
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(1), 255)
        SUBTRACT(LW(0), 2)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PANSY_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    BEGIN_SIBLING_EVENT
        LOOP(10)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            IF_INT_LESS(LW(15), 0)
                CALL(evt_eff64, STRING(""), STRING("onpu_n64"), 1, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
            ELSE
                CALL(evt_eff64, STRING(""), STRING("onpu_n64"), 2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
            END_IF
            WAIT_FRAMES(12)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_FRAMES(120)
    SET_VALUE(LW(6), 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(10)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
        GOTO(97)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        SET_VALUE(LW(6), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(98)
    END_IF
    GOTO(10)
    LABEL(98)
    SET_VALUE(LW(0), 192)
    SET_VALUE(LW(1), 255)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(1), 255)
        ADD(LW(0), 2)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_map_blend_off, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(500)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(escape_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PANSY_ESCAPE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PAN_E_1"))
    CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_SetPartsBlur, -2, 1, 255, 255, 255, 255, 100, 100, 100, 100, 0)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 67108864)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PANSY_ESCAPE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), FLOAT(20.0f), -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PANSY_ESCAPE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(9.0f))
    CALL(btlevtcmd_MovePosition, -2, 300, LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 67108864)
    WAIT_MS(500)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

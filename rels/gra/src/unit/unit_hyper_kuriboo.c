#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_unit.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "system.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(krb_get_dir);
static BattleDataEntry data_table[2];
static u32 regist[6];
static u32 parts[19];
static s32 init_event[47];
static s32 damage_event[10];
static s32 attack_event[638];
static s32 wait_event[6];

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_hyper_kuriboo[49] = {
    0x00000042, (u32)"btl_un_hyper_kuriboo", 0x00080000, 0x01010F00,
    0x02460146, 0x270F0017, 0x00190005, 0x00190000,
    0x00000000, 0x41400000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41400000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41380000,
    0x00000000, 0x00000000, 0x41400000, 0x41900000,
    0x00000000, 0x00000000, 0x41400000, 0x00000000,
    0x41B80000, 0x41C80000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_KURI_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00040000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

static BattleDispPose pose_table[20] = {
    { 0x00000001, "KUR_N_1" }, { 0x00000002, "KUR_Y_1" },
    { 0x00000009, "KUR_Y_1" }, { 0x00000005, "KUR_K_1" },
    { 0x00000004, "KUR_I_1" }, { 0x00000003, "KUR_I_1" },
    { 0x0000001C, "KUR_S_1" }, { 0x0000001D, "KUR_Q_1" },
    { 0x0000001E, "KUR_Q_1" }, { 0x0000001F, "KUR_S_1" },
    { 0x00000027, "KUR_D_1" }, { 0x00000032, "KUR_A_1" },
    { 0x00000033, "KUR_A_2" }, { 0x0000002A, "KUR_R_1" },
    { 0x00000029, "KUR_R_2" }, { 0x00000028, "KUR_W_1" },
    { 0x00000038, "KUR_I_1" }, { 0x00000039, "KUR_I_1" },
    { 0x00000041, "KUR_T_1" }, { 0x00000045, "KUR_S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x00000030, btldefaultevt_Dummy }, { 0x00000000, NULL },
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
    0x505A5064, 0x50646450, 0x645F645F, 0x645F505A,
    0x50645064, 0x645F0000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_hyper_kuriboo", (u32)"c_kuribo_h", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41B00000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_kuriboo_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00005087, 0x00000000, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_kuriboo_charge[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01004020, 0x20000000, 0x00000302,
    0x00000000, 0x00000108, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000006,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_KURI_MOVE1"), STRING("SFX_ENM_KURI_MOVE2"), 0, 6, 6)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_KURI_MOVE1"), STRING("SFX_ENM_KURI_MOVE2"), 0, 6, 6)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_ENM_KURI_JUMP1"), STRING("SFX_ENM_KURI_LANDING1"))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
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
    RETURN
EVT_END()

static USER_FUNC(krb_get_dir) {
    s32* args = event->args;
    f32 x1 = evtGetFloat(event, args[0]);
    f32 y1 = evtGetFloat(event, args[1]);
    f32 x2 = evtGetFloat(event, args[2]);
    f32 y2 = evtGetFloat(event, args[3]);
    s32 out = evtGetValue(event, args[4]);
    f32 dx = x2 - x1;
    f32 dy = y2 - y1;

    if (dx == 0.0f && dy == 0.0f) {
        evtSetValue(event, args[4], out);
        return 2;
    }
    out = (s32)angleABf(dx, dy, 0.0f, 0.0f);
    evtSetValue(event, args[4], out);
    return 2;
}

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_kuriboo_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_kuriboo_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 16, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_DrawLots, LW(0), 2, 25, 75)
    ELSE
        CALL(btlevtcmd_DrawLots, LW(0), 2, 25, 25)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(9), PTR(weapon_kuriboo_charge))
        CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        CALL(evt_btl_camera_set_mode, 0, 7)
        CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 300)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 15)
        ADD(LW(2), 10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_eff64, STRING(""), STRING("crystal_n64"), 7, LW(0), LW(1), LW(2), FLOAT(1.5f), 60, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1000)
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpSetting, -2, 20, 0, FLOAT(1.0f))
            CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
            WAIT_MS(500)
            CALL(evt_btl_camera_set_mode, 0, 0)
        END_INLINE_EVENT
        CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
        RETURN
    END_IF
    LABEL(10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.6f))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_kuriboo_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_kuriboo_attack))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 100)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
        SET_VALUE(LW(8), 0)
        LOOP(32)
            CALL(btlevtcmd_GetPosFloat, -2, LW(9), LW(10), LW(11))
            CALL(krb_get_dir, LW(5), LW(6), LW(9), LW(10), LW(8))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(8))
            SET_FLOAT_VALUE(LW(5), LW(9))
            SET_FLOAT_VALUE(LW(6), LW(10))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(12)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(15), 50)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, LW(15))
    END_INLINE_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 32, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_kuriboo_attack), 256, LW(5))
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
        GOTO(91)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(15), 69)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
    WAIT_MS(500)
    GOTO(95)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KURI_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_kuriboo_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_kuriboo_attack), 256, LW(5))
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(15), 69)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 10)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
    GOTO(95)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_ENM_KURI_MOVE1"), STRING("SFX_ENM_KURI_MOVE2"), 0, 3, 3, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 41)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
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

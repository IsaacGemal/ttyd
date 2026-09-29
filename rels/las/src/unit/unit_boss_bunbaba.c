#include "unit_boss_bunbaba.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_msg.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_snd.h"
#include "eff/n64/eff_kemuri9_n64.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
static USER_FUNC(_leg_smoke_effect);
static u32 regist[6];
static u32 data_table[6];
static u32 parts[38];
static s32 init_event[53];
static s32 damage_event[10];
s32 bunbaba_damage_sub_event[45];
static s32 dead_event[264];
static s32 phase_event[160];
static s32 attack_event[237];
static s32 wait_event[6];

// clang-format off
extend_data(164,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(165,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_boss_bunbaba[49] = {
    0x00000084, (u32)"btl_un_bunbaba", 0x00500000, 0x01014C00,
    0x00640032, 0x000600E6, 0x00AAFF2E, 0x002D0000,
    0x00000000, 0x00000000, 0x00000000, 0xFF060000,
    0x438C0000, 0x42200000, 0x00000000, 0x42E60000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x41A00000, 0x00000000, 0x437A0000,
    0x00000000, 0x42480000, 0x43BE0000, 0x42DC0000,
    0x42480000, 0xC3910000, 0x42AA0000, 0x00000000,
    0x43660000, 0x432A0000, 0x0000FF00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_GNB_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00200000,
    (u32)&regist, 0x02000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x02020202, 0x02000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x1E1E1E64, 0x1E646400, 0x00006400, 0x64461E00,
    0x00641E00, 0x00010000,
};

// TODO: type
static u32 pose_table[18] = {
    0x00000001, (u32)"GNB_N_1", 0x00000005, (u32)"GNB_X_1",
    0x00000003, (u32)"GNB_X_1", 0x00000002, (u32)"GNB_D2_1",
    0x0000001C, (u32)"GNB_S_3", 0x00000041, (u32)"GNB_T_3",
    0x0000001F, (u32)"GNB_S_3", 0x00000027, (u32)"GNB_D_1",
    0x00000045, (u32)"GNB_S_3",
};

// TODO: type
static u32 data_table[6] = {
    0x00000009, (u32)&bunbaba_damage_sub_event, 0x00000031, (u32)&dead_event,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[38] = {
    0x00000001, (u32)"btl_un_bunbaba", (u32)"c_gonbaba_b", 0x00000000,
    0x00000000, 0x00000000, 0x43660000, 0x42100000,
    0x42700000, 0x43660000, 0x42100000, 0x42700000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000002, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_bunbaba", (u32)"c_gonbaba_b", 0x42C80000, 0x00000000,
    0x00000000, 0x437A0000, 0x42DC0000, 0x41C80000,
    0x437A0000, 0x42DC0000, 0x41C80000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x01600009,
    0x00000000, (u32)&pose_table,
};

// TODO: type
u32 weapon_bunbaba_fumituke_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x000B0302,
    0x00000000, 0x00000007, 0x000007FF, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x0A000000,
};

// TODO: type
u32 weapon_bunbaba_kamituki_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    0x00000000, 0x00020007, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 weapon_bunbaba_poison_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000047, 0x000007FF, 0x00000000,
    0x00000000, 0x0000320A, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 weapon_bunbaba_eq_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x0000000A, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000008, 0x00000302,
    0x00000000, 0x00001047, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 weapon_bunbaba_charge[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01004020, 0x10000000, 0x00000302,
    0x00000000, 0x00000008, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000008,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 weapon_bunbaba_mega_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000047, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(battle_entry_event)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_map_replayanim, 0, STRING("dontyo"))
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 0, 110, 1080, 0, 93, -2, 1, 0)
    CALL(btlevtcmd_GetStageSize, LW(6), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    MULTIPLY(LW(6), -1)
    CALL(btlevtcmd_GetHomePos, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -4, LW(3), LW(4), LW(5))
    CALL(btlevtcmd_SetPos, -3, LW(6), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -4, LW(6), LW(4), LW(5))
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(3), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), LW(3))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 42)
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 42)
    CALL(btlevtcmd_GetHomePos, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -4, LW(3), LW(4), LW(5))
    SET_VALUE(LW(8), LW(0))
    SUBTRACT(LW(8), LW(3))
    IF_INT_LESS(LW(8), 0)
        MULTIPLY(LW(8), -1)
    END_IF
    CALL(btlevtcmd_GetStageSize, LW(6), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(7), LW(6))
    ADD(LW(7), LW(8))
    MULTIPLY(LW(6), -1)
    MULTIPLY(LW(7), -1)
    CALL(btlevtcmd_SetPos, -3, LW(6), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -4, LW(7), LW(4), LW(5))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetMoveSpeed, -4, FLOAT(6.0f))
        CALL(btlevtcmd_MovePosition, -4, LW(3), LW(4), LW(5), 0, -1, 0)
        CALL(btlevtcmd_GetBodyId, -4, LW(0))
        CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 43)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -3, FLOAT(6.0f))
    CALL(btlevtcmd_MovePosition, -3, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_I_Y"))
    WAIT_MS(33)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_F_3"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_MS(166)
    BEGIN_INLINE_EVENT
        WAIT_MS(83)
        CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_I_O"))
        CALL(btlevtcmd_SetFallAccel, -3, FLOAT(0.3f))
        CALL(btlevtcmd_GetPos, -3, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_snd_se, -3, STRING("SFX_VOICE_MARIO_SURPRISED2_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_JumpPosition, -3, LW(0), LW(1), LW(2), 25, -1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(83)
        CALL(btlevtcmd_SetFallAccel, -4, FLOAT(0.3f))
        CALL(btlevtcmd_GetPos, -4, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPosition, -4, LW(0), LW(1), LW(2), 25, -1)
        CALL(btlevtcmd_GetBodyId, -4, LW(0))
        CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 5)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_GNB_APPEAR1"), 0)
    CALL(evt_btl_camera_shake_h, 1, 8, 0, 20, 13)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_I_Y"))
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -4, LW(0), 43)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, -233, 45, 452, 56, 125, 37, 60, 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("GNB_T_3"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("GNB_S_3"))
    CALL(evt_msg_print, 2, STRING("stg8_las_49_01"), 0, -2)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(btlevtcmd_StartWaitEvent, -3)
    CALL(btlevtcmd_StartWaitEvent, -4)
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 3)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetTalkBodyId, -2, 1)
    CALL(btlevtcmd_SetBaseScale, -2, FLOAT(1.3f), FLOAT(1.3f), FLOAT(0.5f))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

EVT_BEGIN(bunbaba_damage_sub_event)
    CALL(btlevtcmd_GetDamagePartsId, -2, LW(11))
    IF_INT_EQUAL(LW(11), 1)
        CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("GNB_D_2"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("GNB_D_1"))
    END_IF
    CALL(btlevtcmd_GetDamage, LW(10), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHitPos, LW(10), LW(11), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_ATTACK_MISS2"), LW(0), LW(1), LW(2), 0)
    END_IF
    WAIT_MS(1000)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_X_1"))
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_DOWN1"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, LW(15))
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_wait_move_end)
    CALL(evt_btl_camera_set_mode, 1, 3)
    SET_VALUE(LW(1), 160)
    SET_VALUE(LW(2), 536)
    CALL(btlevtcmd_GetStageAudienceAreaSize, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    ADD(LW(1), LW(5))
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(5))
    ADD(LW(2), LW(5))
    ADD(LW(2), 80)
    SET_VALUE(LW(5), LW(1))
    SUBTRACT(LW(5), 37)
    SET_VALUE(LW(6), LW(2))
    SUBTRACT(LW(6), 511)
    CALL(evt_btl_camera_set_moveto, 1, 93, LW(1), LW(2), 93, LW(5), LW(6), 30, 0)
    WAIT_MS(1500)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("GNB_X_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("GNB_X_1"))
    CALL(evt_msg_print, 2, STRING("stg8_las_49_08"), 0, -2)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, -38, 33, 1005, -38, 92, 13, 60, 0)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_DOWN2"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_D_3"))
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_1"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 25)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_1"))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(5), 100)
        SET_VALUE(LW(4), 120)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, LW(5), 0)
        LOOP(40)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(4))
            SUBTRACT(LW(4), 3)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    SUBTRACT(LW(2), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -1)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_GNB_DOWN3"), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_2"))
    CALL(evt_btl_camera_shake_h, 1, 8, 0, 30, 13)
    WAIT_MS(166)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_H_3"))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    WAIT_MS(1000)
    CALL(btlevtcmd_KillUnit, -2, 1)
    CALL(evt_btl_camera_set_prilimit, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 60)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
            CALL(evt_msg_print, 2, STRING("stg8_las_49_03"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
        END_IF
        CASE_EQUAL(3)
        CALL(btlevtcmd_GetMaxHp, -2, LW(0))
        MULTIPLY(LW(0), 40)
        DIVIDE(LW(0), 100)
        CALL(btlevtcmd_GetHp, -2, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 4)
            CALL(evt_msg_print, 2, STRING("stg8_las_49_04"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
        END_IF
        CASE_EQUAL(5)
        CALL(evt_msg_print, 2, STRING("stg8_las_49_05"), 0, -2)
        CASE_GREATER_OR_EQUAL(6)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetMaxHp, -2, LW(0))
            MULTIPLY(LW(0), 20)
            DIVIDE(LW(0), 100)
            CALL(btlevtcmd_GetHp, -2, LW(1))
            IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
                CALL(evt_msg_print, 2, STRING("stg8_las_49_07"), 0, -2)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
            END_IF
        END_IF
    END_SWITCH
    LABEL(99)
    RETURN
EVT_END()

// Spawns a Kemuri9 smoke effect at the script-supplied position (60 unit spread).
static USER_FUNC(_leg_smoke_effect) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    effKemuri9N64Entry(1, 0x12, 0x5A, x, y, z, 60.0f, 0.0f);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(fumituke_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_W_2"))
    WAIT_MS(950)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -300, 0, -30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -300)
        ADD(LW(2), -30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_MS(766)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -300, 0, 30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -300)
        ADD(LW(2), 30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_MS(16)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_3"))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_ARM_UP1"), EVTDAT_POINTER_MAX, 0, -320, 30, 30, EVTDAT_POINTER_MAX)
        WAIT_MS(700)
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_ARM_DOWN1"), EVTDAT_POINTER_MAX, 0, -360, 50, 30, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    WAIT_MS(750)
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
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -400, 0, -5, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -400)
        ADD(LW(2), -5)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    WAIT_MS(50)
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_MS(116)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 30, 13)
    WAIT_MS(583)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_W_3"))
    WAIT_MS(850)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -180, 0, 30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -180)
        ADD(LW(2), 30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_MS(766)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MOVE1"), EVTDAT_POINTER_MAX, 0, -100, 0, 30, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), -100)
        ADD(LW(2), -30)
        CALL(_leg_smoke_effect, LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_MS(116)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(kamituki_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_BITE1"), EVTDAT_POINTER_MAX, 0, -220, 80, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(250)
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_BITE2"), EVTDAT_POINTER_MAX, 0, -260, 40, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_2"))
    WAIT_MS(333)
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
        GOTO(99)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(97)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    LABEL(99)
    WAIT_MS(333)
    RETURN
EVT_END()

EVT_BEGIN(eq_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_5"))
    WAIT_MS(666)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_GNB_NECK_UP1"), 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_6"))
    WAIT_MS(233)
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_GNB_DOWN3"), 0)
    WAIT_MS(50)
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    WAIT_MS(250)
    SET_VALUE(LW(10), 0)
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
        GOTO(99)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 30, 13)
    WAIT_MS(583)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(poison_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A2_1"))
    WAIT_MS(833)
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_NECK_UP1"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A2_3"))
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_BREATH_POISON1"), EVTDAT_POINTER_MAX, 0, -300, 0, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(10), 120)
    SET_VALUE(LW(11), 30)
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(6), 360)
    ADD(LW(7), 30)
    CALL(btlevtcmd_GetPos, -2, LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), 360)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), LW(10))
    BEGIN_INLINE_EVENT
        WAIT_MS(333)
        ADD(LW(11), 45)
        SUBTRACT(LW(8), 10)
        CALL(evt_eff, 0, STRING("gonbaba_breath"), 1, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SUBTRACT(LW(12), LW(6))
    IF_INT_LESS(LW(12), 0)
        MULTIPLY(LW(12), -1)
    END_IF
    ADD(LW(12), 20)
    MULTIPLY(LW(12), LW(11))
    DIVIDE(LW(12), LW(10))
    ADD(LW(12), 20)
    WAIT_FRAMES(LW(12))
    SET_VALUE(LW(10), 0)
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
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        WAIT_MS(333)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    WAIT_MS(1400)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(mega_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GNB_A_1"))
    SET_VALUE(LW(10), 120)
    SET_VALUE(LW(11), 30)
    CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(6), 360)
    ADD(LW(7), 30)
    CALL(btlevtcmd_GetPos, -2, LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), 360)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(12), LW(10))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_NECK_UP1"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1750)
        CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_MEGABREATH1"), EVTDAT_POINTER_MAX, 0, -300, 0, 0, EVTDAT_POINTER_MAX)
        ADD(LW(11), 45)
        SUBTRACT(LW(8), 10)
        CALL(evt_eff, 0, STRING("gonbaba_breath"), 2, LW(6), LW(7), LW(8), FLOAT(3.0f), LW(11), 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(12), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SUBTRACT(LW(12), LW(6))
    IF_INT_LESS(LW(12), 0)
        MULTIPLY(LW(12), -1)
    END_IF
    ADD(LW(12), 20)
    MULTIPLY(LW(12), LW(11))
    DIVIDE(LW(12), LW(10))
    ADD(LW(12), 105)
    WAIT_FRAMES(LW(12))
    SET_VALUE(LW(10), 0)
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
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        WAIT_MS(333)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    WAIT_MS(1400)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(charge_event)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_snd_se_offset, -2, STRING("SFX_BOSS_GNB_CHARGE1"), EVTDAT_POINTER_MAX, 0, -250, 0, 0, LW(15))
    CALL(btlevtcmd_GetHitPos, -2, 2, LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    ADD(LW(1), -25)
    CALL(evt_eff, STRING(""), STRING("charge"), 0, LW(0), LW(1), LW(2), FLOAT(2.0f), 60, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        SET_VALUE(LW(9), PTR(weapon_bunbaba_poison_attack))
        CALL_CHILD(poison_attack_event)
        CASE_EQUAL(1)
        CALL(btlevtcmd_DrawLots, LW(0), 3, 10, 10, 20)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_fumituke_attack))
            CALL_CHILD(fumituke_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_kamituki_attack))
            CALL_CHILD(kamituki_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_poison_attack))
            CALL_CHILD(poison_attack_event)
        END_SWITCH
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 3)
        SET_VALUE(LW(9), PTR(weapon_bunbaba_eq_attack))
        CALL_CHILD(eq_attack_event)
        CASE_EQUAL(3)
        CALL(btlevtcmd_DrawLots, LW(0), 4, 10, 10, 20, 15)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_fumituke_attack))
            CALL_CHILD(fumituke_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_kamituki_attack))
            CALL_CHILD(kamituki_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_poison_attack))
            CALL_CHILD(poison_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_eq_attack))
            CALL_CHILD(eq_attack_event)
        END_SWITCH
        CASE_EQUAL(4)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 5)
        SET_VALUE(LW(9), PTR(weapon_bunbaba_charge))
        CALL_CHILD(charge_event)
        CASE_EQUAL(5)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 7)
        SET_VALUE(LW(9), PTR(weapon_bunbaba_mega_attack))
        CALL_CHILD(mega_attack_event)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_msg_print, 2, STRING("stg8_las_49_06"), 0, -2)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        CASE_EQUAL(6)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 7)
        SET_VALUE(LW(9), PTR(weapon_bunbaba_mega_attack))
        CALL_CHILD(mega_attack_event)
        CASE_EQUAL(7)
        CALL(btlevtcmd_DrawLots, LW(0), 6, 10, 10, 20, 10, 20, 0)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_fumituke_attack))
            CALL_CHILD(fumituke_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_kamituki_attack))
            CALL_CHILD(kamituki_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_poison_attack))
            CALL_CHILD(poison_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_eq_attack))
            CALL_CHILD(eq_attack_event)
            CASE_EQUAL(4)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 6)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_charge))
            CALL_CHILD(charge_event)
            CASE_EQUAL(5)
            SET_VALUE(LW(9), PTR(weapon_bunbaba_mega_attack))
            CALL_CHILD(mega_attack_event)
        END_SWITCH
    END_SWITCH
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

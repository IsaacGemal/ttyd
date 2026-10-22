#include "battle/battle_event_default.h"
#include "battle/battle.h"
#include "battle/battle_status_effect.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "forcedata.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_database_common.h"
#include "battle/battle_camera.h"
#include "evt/evt_sub.h"
#include "battle/battle_weapon_power.h"

// These floats are not referenced by any decompiled code, but the original object carries them
// (MSL's sqrtf inlined them here).  Mark them force_active so the REL linker keeps the
// unreferenced locals; the pragma is scoped off again before the rest of .data.
#pragma force_active on
// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
// clang-format on

// Referenced (by address) from the unit's event scripts, but no header declares them.
extern s32 subsetevt_confuse_flustered[];
#pragma force_active off

#include "rel/common/teresa_check/teresa_check_trans.h"
#include "rel/common/teresa_check/teresa_check_move.h"
#include "rel/common/teresa_check/teresa_check_teresa.h"


/* ==== generated strings (.rodata) ==== */
static const char str_btl_un_purple_teresa[24] = {0x62,0x74,0x6C,0x5F,0x75,0x6E,0x5F,0x70,0x75,0x72,0x70,0x6C,0x65,0x5F,0x74,0x65,0x72,0x65,0x73,0x61,0x00,0x00,0x00,0x00};
static const char str_SFX_ENM_TELESA_DAMAGED1[24] = {0x53,0x46,0x58,0x5F,0x45,0x4E,0x4D,0x5F,0x54,0x45,0x4C,0x45,0x53,0x41,0x5F,0x44,0x41,0x4D,0x41,0x47,0x45,0x44,0x31,0x00};
static const char str_SFX_BTL_DAMAGE_FIRE1[24] = {0x53,0x46,0x58,0x5F,0x42,0x54,0x4C,0x5F,0x44,0x41,0x4D,0x41,0x47,0x45,0x5F,0x46,0x49,0x52,0x45,0x31,0x00,0x00,0x00,0x00};
static const char str_SFX_BTL_DAMAGE_ICE1[20] = {0x53,0x46,0x58,0x5F,0x42,0x54,0x4C,0x5F,0x44,0x41,0x4D,0x41,0x47,0x45,0x5F,0x49,0x43,0x45,0x31,0x00};
static const char str_SFX_BTL_DAMAGE_BIRIBIRI1[28] = {0x53,0x46,0x58,0x5F,0x42,0x54,0x4C,0x5F,0x44,0x41,0x4D,0x41,0x47,0x45,0x5F,0x42,0x49,0x52,0x49,0x42,0x49,0x52,0x49,0x31,0x00,0x00,0x00,0x00};
static const char str_c_teresa_p[12] = {0x63,0x5F,0x74,0x65,0x72,0x65,0x73,0x61,0x5F,0x70,0x00,0x00};
static const char str_N_1[4] = {0x4E,0x5F,0x31,0x00};
static const char str_Y_1[4] = {0x59,0x5F,0x31,0x00};
static const char str_K_1[4] = {0x4B,0x5F,0x31,0x00};
static const char str_X_1[4] = {0x58,0x5F,0x31,0x00};
static const char str_000026BC[4] = {0x53,0x5F,0x31,0x00};
static const char str_Q_1[4] = {0x51,0x5F,0x31,0x00};
static const char str_A_2[4] = {0x41,0x5F,0x32,0x00};
static const char str_D_1[4] = {0x44,0x5F,0x31,0x00};
static const char str_W_1[4] = {0x57,0x5F,0x31,0x00};
static const char str_R_1[4] = {0x52,0x5F,0x31,0x00};
static const char str_T_1[4] = {0x54,0x5F,0x31,0x00};
static const char str_SFX_ENM_TELESA_MOVE3[24] = {0x53,0x46,0x58,0x5F,0x45,0x4E,0x4D,0x5F,0x54,0x45,0x4C,0x45,0x53,0x41,0x5F,0x4D,0x4F,0x56,0x45,0x33,0x00,0x00,0x00,0x00};
static const char str_SFX_ENM_TELESA_MOVE4[24] = {0x53,0x46,0x58,0x5F,0x45,0x4E,0x4D,0x5F,0x54,0x45,0x4C,0x45,0x53,0x41,0x5F,0x4D,0x4F,0x56,0x45,0x34,0x00,0x00,0x00,0x00};
static const char str_A_1[4] = {0x41,0x5F,0x31,0x00};
static const char str_SFX_ENM_TELESA_ATTACK1[24] = {0x53,0x46,0x58,0x5F,0x45,0x4E,0x4D,0x5F,0x54,0x45,0x4C,0x45,0x53,0x41,0x5F,0x41,0x54,0x54,0x41,0x43,0x4B,0x31,0x00,0x00};
static const char str_SFX_ENM_TELESA_MOVE2[24] = {0x53,0x46,0x58,0x5F,0x45,0x4E,0x4D,0x5F,0x54,0x45,0x4C,0x45,0x53,0x41,0x5F,0x4D,0x4F,0x56,0x45,0x32,0x00,0x00,0x00,0x00};
static const char str_SFX_ENM_TELESA_MOVE1[21] = {0x53,0x46,0x58,0x5F,0x45,0x4E,0x4D,0x5F,0x54,0x45,0x4C,0x45,0x53,0x41,0x5F,0x4D,0x4F,0x56,0x45,0x31,0x00};

/* ==== forward declarations (.data) ==== */
u32 unit_purple_teresa_22_data_E620[49];
u32 unit_purple_teresa_15_data_493B8[49];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static u32 parts[19];
static u32 weapon_teresa_attack[48];
static u32 weapon_teresa_trans_myself[48];
static u32 weapon_teresa_trans[48];
static u32 weapon_teresa_move[48];
static u32 pose_table[34];
static u32 data_table[2];
static s32 init_event[47];
static s32 damage_event[10];
static s32 attack_event[153];
static s32 normal_attack_event[354];
static s32 trans_myself_event[196];
static s32 trans_event[289];
static s32 move_event[84];
static s32 move_event_sub[103];
static s32 wait_event[6];

/* ==== generated data (.data) ==== */
u32 unit_purple_teresa_22_data_E620[49] = {0x0000006A, (u32)(char*)str_btl_un_purple_teresa, 0x00080000, 0x01011800, 0x04460032, 0x270F0022, 0x00200000, 0x00200000, 0x00000000, 0x41800000, 0x00000000, 0x0000FFFD, 0x00000000, 0x00000000, 0x00000000, 0x41880000, 0x00000000, 0xC1200000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41880000, 0x00000000, 0x00000000, 0x41880000, 0x41A66666, 0x00000000, 0x00000000, 0x41800000, 0x00000000, 0x42080000, 0x42000000, 0x00000002, 0x64140000, 0x00000000, 0x00000000, 0x00000000, (u32)(char*)str_SFX_ENM_TELESA_DAMAGED1, (u32)(char*)str_SFX_BTL_DAMAGE_FIRE1, (u32)(char*)str_SFX_BTL_DAMAGE_ICE1, (u32)(char*)str_SFX_BTL_DAMAGE_BIRIBIRI1, 0x00000004, (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event, (u32)&data_table};
u32 unit_purple_teresa_15_data_493B8[49] = {0x0000006A, (u32)(char*)str_btl_un_purple_teresa, 0x00080000, 0x01011800, 0x04460032, 0x270F0022, 0x00200000, 0x00200000, 0x00000000, 0x41800000, 0x00000000, 0x0000FFFD, 0x00000000, 0x00000000, 0x00000000, 0x41880000, 0x00000000, 0xC1200000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41880000, 0x00000000, 0x00000000, 0x41880000, 0x41A66666, 0x00000000, 0x00000000, 0x41800000, 0x00000000, 0x42080000, 0x42000000, 0x00000002, 0x64140000, 0x00000000, 0x00000000, 0x00000000, (u32)(char*)str_SFX_ENM_TELESA_DAMAGED1, (u32)(char*)str_SFX_BTL_DAMAGE_FIRE1, (u32)(char*)str_SFX_BTL_DAMAGE_ICE1, (u32)(char*)str_SFX_BTL_DAMAGE_BIRIBIRI1, 0x00000004, (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event, (u32)&data_table};
static u32 defence[2] = {0x00000000, 0x00000000};
static u32 defence_attr[2] = {0x00000000, 0x00000000};
static u32 regist[6] = {0x28286400, 0x28646428, 0x64556455, 0x645A3228, 0x55640064, 0x640A0000};
static u32 parts[19] = {0x00000001, (u32)(char*)str_btl_un_purple_teresa, (u32)(char*)str_c_teresa_p, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41C80000, 0x00000000, 0x00000000, 0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x00000009, 0x00000000, (u32)&pose_table};
static u32 weapon_teresa_attack[48] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64000001, 0x3F800000, 0x01010101, (u32)&weaponGetPowerDefault, 0x00000005, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01101260, 0x20000000, 0x00000302, 0x00000000, 0x00000087, 0x000007FF, 0x80002004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000064, 0x00000000, 0x00000000};
static u32 weapon_teresa_trans_myself[48] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64000000, 0x3F800000, 0x01010101, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01101260, 0x20000000, 0x00000302, 0x00000000, 0x00000108, 0x000007FF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64020000, 0x00000000, 0x00000000, 0x00000064, 0x00000000, 0x00000000};
static u32 weapon_teresa_trans[48] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64000000, 0x3F800000, 0x01010101, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01101160, 0x20000000, 0x00000302, 0x00000000, 0x00000108, 0x000007FF, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64020000, 0x00000000, 0x00000000, 0x00000064, 0x00000000, 0x00000000};
static u32 weapon_teresa_move[48] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64000000, 0x3F800000, 0x01010101, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01100160, 0x20000000, 0x00000302, 0x00000000, 0x00000008, 0x000007FF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000064, 0x00000000, 0x00000000};
static u32 pose_table[34] = {0x00000001, (u32)(char*)str_N_1, 0x00000002, (u32)(char*)str_Y_1, 0x00000009, (u32)(char*)str_Y_1, 0x00000005, (u32)(char*)str_K_1, 0x00000004, (u32)(char*)str_X_1, 0x00000003, (u32)(char*)str_X_1, 0x0000001C, (u32)(char*)str_000026BC, 0x0000001D, (u32)(char*)str_Q_1, 0x0000001E, (u32)(char*)str_Q_1, 0x0000001F, (u32)(char*)str_A_2, 0x00000027, (u32)(char*)str_D_1, 0x00000028, (u32)(char*)str_W_1, 0x0000002A, (u32)(char*)str_R_1, 0x00000038, (u32)(char*)str_X_1, 0x00000039, (u32)(char*)str_X_1, 0x00000041, (u32)(char*)str_T_1, 0x00000045, (u32)(char*)str_000026BC};
static u32 data_table[2] = {0x00000000, 0x00000000};
static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 15)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
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
    CALL(btlevtcmd_CheckStatus, -2, 18, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(13), 0)
    ELSE
        SET_VALUE(LW(13), 1)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), 0)
    ELSE
        CALL(teresa_check_trans, -2, LW(14))
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(15), 0)
    ELSE
        CALL(teresa_check_move, LW(15))
    END_IF
    SET_VALUE(LW(0), 0)
    ADD(LW(0), 50)
    ADD(LW(0), 25)
    ADD(LW(0), 25)
    ADD(LW(0), 15)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 25)
    IF_INT_LESS(LW(1), LW(0))
        IF_INT_EQUAL(LW(13), 1)
            CALL_CHILD(trans_myself_event)
            GOTO(99)
        END_IF
    END_IF
    ADD(LW(0), 25)
    IF_INT_LESS(LW(1), LW(0))
        IF_INT_EQUAL(LW(14), 1)
            CALL_CHILD(trans_event)
            GOTO(99)
        END_IF
    END_IF
    ADD(LW(0), 15)
    IF_INT_LESS(LW(1), LW(0))
        IF_INT_EQUAL(LW(15), 1)
            CALL_CHILD(move_event)
            GOTO(99)
        END_IF
    END_IF
    CALL_CHILD(normal_attack_event)
    GOTO(99)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_teresa_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_teresa_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_teresa_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_teresa_attack))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_snd_se, -2, PTR((char*)str_SFX_ENM_TELESA_MOVE3), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetAlpha, -2, 1, 255)
        SET_VALUE(LW(7), 256)
        LOOP(16)
            WAIT_FRAMES(1)
            SUBTRACT(LW(7), 16)
            CALL(btlevtcmd_SetAlpha, -2, 1, LW(7))
        END_LOOP
        WAIT_FRAMES(20)
        CALL(btlevtcmd_snd_se, -2, PTR((char*)str_SFX_ENM_TELESA_MOVE4), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_A_1))
        CALL(btlevtcmd_SetAlpha, -2, 1, 0)
        SET_VALUE(LW(7), -1)
        LOOP(8)
            WAIT_FRAMES(1)
            ADD(LW(7), 32)
            CALL(btlevtcmd_SetAlpha, -2, 1, LW(7))
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 15)
    ADD(LW(2), 5)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, -5, 4, 0, -1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    WAIT_FRAMES(15)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(9)
        CALL(btlevtcmd_snd_se, -2, PTR((char*)str_SFX_ENM_TELESA_ATTACK1), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_A_2))
    WAIT_FRAMES(15)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_teresa_attack), 0, LW(5))
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
        LABEL(90)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_X_1))
        WAIT_FRAMES(60)
        GOTO(98)
        LABEL(91)
        CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_add_zoom, 0, -50)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_teresa_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_teresa_attack), 256, LW(5))
        WAIT_FRAMES(30)
        GOTO(98)
        LABEL(98)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 180, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, 0, 4, 0, -1)
        CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(trans_myself_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_CheckStatus, -2, 18, LW(8))
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_teresa_trans_myself))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_teresa_trans_myself))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 30)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 10, 4, 0, -1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, PTR((char*)str_SFX_ENM_TELESA_MOVE2), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    IF_INT_EQUAL(LW(8), 0)
        CALL(btlevtcmd_SetAlpha, -2, 1, 255)
        SET_VALUE(LW(7), 256)
        BEGIN_SIBLING_EVENT
            LOOP(16)
                WAIT_FRAMES(1)
                SUBTRACT(LW(7), 8)
                CALL(btlevtcmd_SetAlpha, -2, 1, LW(7))
            END_LOOP
        END_SIBLING_EVENT
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_A_1))
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 10, 0, 5, 0, -1)
    WAIT_FRAMES(10)
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 15, 0)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_000026BC))
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 15, 0)
    END_LOOP
    WAIT_FRAMES(12)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_000026BC))
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_teresa_trans_myself))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_teresa_trans_myself), 256, LW(5))
    RETURN
EVT_END()

static EVT_BEGIN(trans_event)
    LABEL(0)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_teresa_trans))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_teresa_trans), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(teresa_check_teresa, LW(3), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_CheckStatus, LW(3), 18, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            GOTO(1)
        END_IF
    END_IF
    GOTO(0)
    LABEL(1)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_teresa_trans))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_teresa_trans))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(10), LW(11), LW(12))
    IF_INT_LESS_OR_EQUAL(LW(0), LW(10))
        CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
    ELSE
        CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    END_IF
    CALL(btlevtcmd_snd_se, -2, PTR((char*)str_SFX_ENM_TELESA_MOVE3), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    SET_VALUE(LW(7), 256)
    BEGIN_SIBLING_EVENT
        LOOP(16)
            WAIT_FRAMES(1)
            SUBTRACT(LW(7), 16)
            CALL(btlevtcmd_SetAlpha, -2, 1, LW(7))
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 40)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, -5, 4, 0, -1)
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, PTR((char*)str_SFX_ENM_TELESA_MOVE4), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetAlpha, -2, 1, 0)
    SET_VALUE(LW(7), -1)
    LOOP(16)
        WAIT_FRAMES(1)
        ADD(LW(7), 16)
        CALL(btlevtcmd_SetAlpha, -2, 1, LW(7))
    END_LOOP
    CALL(btlevtcmd_SetAlpha, -2, 1, 255)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, 0, 0, 0, -1)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_teresa_trans_myself))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_teresa_trans_myself), 256, LW(5))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, -5, 4, 0, -1)
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_000026BC))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(move_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_teresa_move))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_teresa_move), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    SET_VALUE(LW(12), 80)
    LABEL(0)
    CALL(teresa_check_teresa, LW(3), LW(5))
    IF_INT_EQUAL(LW(5), 1)
        CALL(btlevtcmd_GetUnitWork, LW(3), 0, LW(6))
        IF_INT_EQUAL(LW(6), 0)
            SPAWN_EVENT(move_event_sub)
            ADD(LW(12), 20)
        END_IF
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(0)
    END_IF
    WAIT_FRAMES(LW(12))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_000026BC))
    RETURN
EVT_END()

static EVT_BEGIN(move_event_sub)
    CALL(btlevtcmd_GetUnitId, -2, LW(1))
    IF_INT_EQUAL(LW(3), LW(1))
        SET_VALUE(LW(0), 0)
        SET_VALUE(LW(1), 0)
    ELSE
        CALL(evt_sub_random, 20, LW(0))
        ADD(LW(0), 60)
        SET_VALUE(LW(1), 12)
    END_IF
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, 1)
    CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 6291456)
    BEGIN_SIBLING_EVENT
        LOOP(60)
            CALL(btlevtcmd_AddRotate, LW(3), 0, LW(1), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_GetUnitId, -2, LW(0))
        IF_INT_EQUAL(LW(3), LW(0))
            CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((char*)str_T_1))
        END_IF
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, PTR((char*)str_SFX_ENM_TELESA_MOVE1), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPartsPos, LW(3), 1, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 40)
    CALL(btlevtcmd_DivePosition, LW(3), LW(0), LW(1), LW(2), 60, 10, 4, 0, -1)
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

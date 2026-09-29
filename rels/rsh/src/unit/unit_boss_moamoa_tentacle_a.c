#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 boss_moamoa_check_zenei_kouei[];
extern s32 btldefaultevt_Confuse[];
extern s32 moamoa1_weakness_pose_table[];
extern s32 moamoa_defence_no_parts[];
static u32 regist[6];
static u32 data_table[4];
static u32 parts[19];
static s32 init_event[26];
static s32 damage_event[10];
static s32 dead_event[115];
static s32 attack_event[21];
static s32 evt_attack[387];
static s32 wait_event[32];

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_boss_moamoa_tentacle_a[49] = {
    0x0000006E, (u32)"btl_un_moamoa_tentacle_c", 0x00040000, 0x01010000,
    0x00640032, 0x000A005A, 0x00640032, 0x00460000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x438C0000, 0x42200000, 0x00000000, 0x42340000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42340000,
    0x00000000, 0x00000000, 0x41200000, 0x428C0000,
    0x41A00000, 0x428C0000, 0x42C80000, 0x00000000,
    0x42B40000, 0x42C80000, 0x0000FF00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_MOA_G_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00200000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[8] = {
    0x0000001C, (u32)"S_1A_1", 0x0000001F, (u32)"D_1A_1",
    0x00000027, (u32)"D_1A_1", 0x00000045, (u32)"S_1A_1",
};

// TODO: type
static u32 data_table[4] = {
    0x00000031, (u32)&dead_event, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_moamoa_tentacle_a", (u32)"c_g_moamoa_b", 0x00000000,
    0x00000000, 0x00000000, 0xC2940000, 0x429A0000,
    0x41F00000, 0xC2980000, 0x42B80000, 0x41F00000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00600001, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_moamoa_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
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
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetHpGaugeOffset, -2, 50, 62)
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
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1A"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
    CALL(btlevtcmd_SetUnitWork, LW(1), 5, -1)
    CALL(btlevtcmd_GetUnitWork, LW(1), 5, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_GetUnitWork, LW(1), 4, LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(btlevtcmd_GetUnitWork, LW(1), 3, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_SetPartsDefenceTable, LW(1), 1, PTR(moamoa_defence_no_parts))
                CALL(btlevtcmd_OffPartsAttribute, LW(1), 1, 536870912)
                CALL(btlevtcmd_AnimeSetPoseTable, LW(1), 1, PTR(moamoa1_weakness_pose_table))
                CALL(btlevtcmd_SetHitOffset, LW(1), 1, 0, 32, 0)
                CALL(btlevtcmd_SetHitCursorOffset, LW(1), 1, 0, 41, 0)
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_BIG1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_BIG2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_AnimeChangePose, LW(1), 1, STRING("V_1"))
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, LW(1), 1)
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL_CHILD(evt_attack)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(evt_attack_other_arm)
    SET_INT_READ_ADDR(LW(1))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, LW(0), LW(5), LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(0), LW(5))
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, LW(6), 0, LW(10))
        IF_INT_NOT_EQUAL(LW(10), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, LW(0), LW(5), LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(0), LW(5))
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    RETURN
EVT_END()

// TODO: type
u32 A_of_A_Attack_front[10] = {
    (u32)"M_1A_1", (u32)"A_1A_1", (u32)"A_1A_2", 0x00000000,
    (u32)"A_1A_3", 0x00000000, (u32)"A_1A_4", (u32)"M_1A_2",
    (u32)"S_1A_1", 0x00000000,
};

// TODO: type
u32 A_of_A_Attack_back[10] = {
    (u32)"M_1A_1", (u32)"A_1A_5", (u32)"A_1A_6", 0x00000000,
    (u32)"A_1A_7", 0x00000000, (u32)"A_1A_8", (u32)"M_1A_2",
    (u32)"S_1A_1", 0x00000000,
};

// TODO: type
u32 B_of_A_Attack[6] = {
    (u32)"M_1B_1", (u32)"S_1B_2", 0x00000000, (u32)"M_1B_2",
    (u32)"S_1B_1", 0x00000000,
};

// TODO: type
u32 C_of_A_Attack[6] = {
    (u32)"M_1C_1", (u32)"S_1C_2", 0x00000000, (u32)"M_1C_2",
    (u32)"S_1C_1", 0x00000000,
};

static EVT_BEGIN(evt_attack)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_moamoa_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_moamoa_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        RETURN
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_moamoa_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_moamoa_attack))
    CALL(boss_moamoa_check_zenei_kouei, LW(3), LW(1))
    IF_INT_EQUAL(LW(1), 0)
        SET_VALUE(LF(0), 0)
    ELSE
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(6))
    CALL(btlevtcmd_SetUnitWork, LW(6), 0, 0)
    CALL(btlevtcmd_AnimeChangePose, LW(6), 1, STRING("S_1_1"))
    SET_VALUE(LW(15), 0)
    CALL(btlevtcmd_GetUnitWork, LW(6), 4, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        SET_VALUE(LW(1), PTR(B_of_A_Attack))
        SET_VALUE(LW(5), 1)
        SPAWN_EVENT(evt_attack_other_arm)
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(6), 3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        SET_VALUE(LW(1), PTR(C_of_A_Attack))
        SET_VALUE(LW(5), 1)
        SPAWN_EVENT(evt_attack_other_arm)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        SET_INT_READ_ADDR(PTR(A_of_A_Attack_front))
        SET_VALUE(LW(15), 1)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(14))
            WAIT_MS(1800)
            CALL(evt_snd_sfxoff, LW(14))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
    ELSE
        SET_INT_READ_ADDR(PTR(A_of_A_Attack_back))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(14))
            WAIT_MS(2300)
            CALL(evt_snd_sfxoff, LW(14))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE2_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
    END_IF
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    END_LOOP
    WAIT_MS(1000)
    IF_INT_EQUAL(LF(0), 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE3_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    IF_INT_EQUAL(LW(15), 0)
        LOOP(0)
            READ_INT(LW(2))
            IF_INT_EQUAL(LW(2), 0)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        END_LOOP
    ELSE
        BEGIN_SIBLING_EVENT
            LOOP(0)
                READ_INT(LW(2))
                IF_INT_EQUAL(LW(2), 0)
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            END_LOOP
            LOOP(0)
                READ_INT(LW(2))
                IF_INT_EQUAL(LW(2), 0)
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            END_LOOP
            SET_VALUE(GF(9), 0)
        END_SIBLING_EVENT
        WAIT_MS(70)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_moamoa_attack), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(1)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_moamoa_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_moamoa_attack), 256, LW(5))
        CASE_EQUAL(4)
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
    END_SWITCH
    CALL(btlevtcmd_SetUnitWork, LW(6), 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    IF_INT_EQUAL(LW(15), 0)
        LOOP(0)
            READ_INT(LW(2))
            IF_INT_EQUAL(LW(2), 0)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        END_LOOP
    ELSE
        SET_VALUE(GF(9), 1)
        WAIT_WHILE(GF(9))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(btlevtcmd_GetUnitWork, LW(6), 4, LW(10))
    IF_INT_NOT_EQUAL(LW(10), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(10), 1)
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(6), 3, LW(10))
    IF_INT_NOT_EQUAL(LW(10), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(10), 1)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

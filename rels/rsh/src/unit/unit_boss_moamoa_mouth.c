#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 moamoa1_weakness_pose_table[];
extern s32 moamoa_defence_no_parts[];
static u32 regist[6];
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[26];
static s32 damage_event[10];
static s32 dead_event[88];
static s32 attack_event[21];
static s32 evt_attack[302];
static s32 wait_event[6];

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_boss_moamoa_mouth[49] = {
    0x0000006F, (u32)"btl_un_moamoa_mouth", 0x00060000, 0x01010000,
    0x00640032, 0x000A00C8, 0x00780028, 0x00640000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x438C0000, 0x42200000, 0x00000000, 0x42C80000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42C80000,
    0x00000000, 0x00000000, 0x42A00000, 0x42A00000,
    0x41A00000, 0x00000000, 0x42F00000, 0x00000000,
    0x43480000, 0x42F00000, 0x0000FF00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_MOA_G_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00240000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

static BattleUnitDefense defence = { {1, 1, 1, 1, 1} };

static BattleUnitDefenseAttr defence_attr = { {0, 0, 0, 0, 0} };

// TODO: type
static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static BattleDispPose pose_table[] = {
    {0x1C, "S_2"}, {0x1F, "D_2_1"}, {0x27, "D_2_1"}, {0x45, "S_2"},
};

static BattleDataEntry data_table[2] = {
    {0x31, dead_event}, {0, NULL},
};

static BattleUnitKindPart parts[1] = {
    {
        1, "btl_un_moamoa_mouth", "c_g_moamoa_b",
        {0.0f, 0.0f, 0.0f},
        {67.0f, 82.0f, 10.0f},
        {0x42, 0x7C, 0x00, 0x00, 0x42, 0xBE, 0x00, 0x00, 0x41, 0x20, 0x00, 0x00, 0x00, 0x14, 0x00, 0x1E, 0x00, 0xFF,
         0x00, 0x00},
        &defence,
        &defence_attr,
        kMainPart | kHammerlikeCannotTarget | kShellTosslikeCannotTarget,
        0,
        pose_table,
    },
};

static BattleWeapon weapon_moamoa_attack[1] = {
    {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x64, 0, 0, 0, 0x3F, 0x80, 0, 0},
    1, 1, 1, 1,
    (void*)weaponGetPowerDefault,
    {0x0A, 0, 0, 0, 0, 0, 0, 0},
    NULL,
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0x02, 0x10, 0x12, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00},
    (BattleWeaponSpecialFlags)0x87,
    {0x00, 0x00, 0x07, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    NULL,
    {0, 0, 0, 0x64, 0, 0, 0, 0, 0, 0, 0, 0},
    },
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
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_DOWN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("L_2_6"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
    CALL(btlevtcmd_SetUnitWork, LW(1), 6, -1)
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

char* mouth_Attack_front[7] = {
    "A_2_1", "A_2_2", NULL, "A_2_3", NULL, "A_2_4", NULL,
};

static EVT_BEGIN(evt_attack)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_moamoa_attack))
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
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(6))
    CALL(btlevtcmd_SetUnitWork, LW(6), 0, 0)
    CALL(btlevtcmd_AnimeChangePose, LW(6), 1, STRING("S_1_1"))
    SET_VALUE(LW(15), 0)
    SET_INT_READ_ADDR(PTR(mouth_Attack_front))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LOOP(0)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_moamoa_attack), 256, LW(5))
        SWITCH(LW(5))
            CASE_EQUAL(1)
            IF_INT_EQUAL(LW(10), 0)
                CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_moamoa_attack))
            END_IF
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_moamoa_attack), 256, LW(5))
            CASE_EQUAL(4)
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_SWITCH
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            BREAK_LOOP
        END_IF
        ADD(LW(10), 1)
    END_LOOP
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
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

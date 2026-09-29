#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"

static u32 parts[19];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static u32 pose_table_3[8];
static s32 init_event[70];
static s32 phase_event[67];
static s32 wait_event[57];
static s32 explosion_event[173];
static u32 data_table[8];
static s32 damage_event[10];
static s32 attack_event[5];
static s32 dead_event[6];

// clang-format off
// TODO: type
u32 unit_bomzou[49] = {
    0x000000DB, (u32)"btl_un_bomzou", 0x00010000, 0x01010100,
    0x00000064, 0x00090018, 0x00180000, 0x00140000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41400000,
    0x00000000, 0x00000000, 0x41400000, 0x4179999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BTL_DAMAGE1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x12400000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
u32 weapon_bomzou_explosion[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x00000000, 0x00000000, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02200020, 0x20000000, 0x03000302,
    0x00000000, 0x00020080, 0x000007FF, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x000A0A64, 0x1E050505, 0x05000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_bomzou", (u32)"c_bakudan", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00080009, 0x00000000, (u32)&pose_table_3,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table_3[8] = {
    0x0000001C, (u32)"B_2", 0x0000001F, (u32)"B_2",
    0x00000027, (u32)"B_2", 0x00000045, (u32)"B_2",
};

// TODO: type
static u32 pose_table_2[8] = {
    0x0000001C, (u32)"B_3", 0x0000001F, (u32)"B_3",
    0x00000027, (u32)"B_3", 0x00000045, (u32)"B_3",
};

// TODO: type
static u32 pose_table_1[8] = {
    0x0000001C, (u32)"B_4", 0x0000001F, (u32)"B_4",
    0x00000027, (u32)"B_4", 0x00000045, (u32)"B_4",
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_GREATER_OR_EQUAL(3)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_3))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2))
        CASE_GREATER_OR_EQUAL(1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_1"))
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
        SWITCH(LW(0))
            CASE_GREATER_OR_EQUAL(3)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_3))
            CASE_GREATER_OR_EQUAL(2)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2))
            CASE_GREATER_OR_EQUAL(1)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1))
        END_SWITCH
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            CALL(btlevtcmd_PhaseEventStartDeclare, -2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_2"))
            WAIT_FRAMES(36)
            CALL_CHILD(explosion_event)
        ELSE
            CALL(btlevtcmd_StartWaitEvent, -2)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    LABEL(0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_GREATER(LW(0), 1)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(evt_eff64, 0, STRING("sweat_n64"), 1, LW(0), LW(1), LW(2), 15, -45, 15, 0, 0, 0, 0, 0)
    CALL(evt_sub_random, 45, LW(0))
    ADD(LW(0), 45)
    WAIT_FRAMES(LW(0))
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(end_battle_evt)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_snd_sfxoff, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(explosion_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_snd_sfxoff, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("hibashira"), 2, LW(0), LW(1), LW(2), LW(0), LW(1), LW(2), 1, FLOAT(1.0f), 60, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_THUNDERS_BOMB2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_StageDispellFog)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 50331648)
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 4)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_2"))
    CALL(btlevtcmd_SamplingEnemy, -2, 1, PTR(weapon_bomzou_explosion))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_bomzou_explosion), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), LW(3))
        GOTO(80)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), -1)
        MULTIPLY(LW(0), -1)
    END_IF
    IF_INT_GREATER(LW(0), 50)
        GOTO(80)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_bomzou_explosion), 256, LW(5))
    IF_INT_EQUAL(LW(5), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_bomzou_explosion), 256, LW(5))
    END_IF
    LABEL(80)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

// TODO: type
__declspec(section ".data") u32 lbl_80398960[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static u32 data_table[8] = {
    0x00000031, (u32)&dead_event, 0x00000035, (u32)&explosion_event,
    0x0000003F, (u32)&end_battle_evt, 0x00000000, 0x00000000,
};

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_AfterReactionEntry, -2, 53)
    RETURN
EVT_END()
// clang-format on

#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"

static u32 data_table[14];
static u32 regist[6];
static u32 parts[38];
static s32 init_event[46];
static s32 phase_event[38];
s32 koura_open[72];
s32 koura_close[79];
s32 damage_core[92];
s32 pose_tbl_reset[58];
static s32 dead_event[243];
s32 destroy_event[47];
static s32 damage_event[10];
static s32 wait_event[6];
static s32 attack_event[2];

// clang-format off
// TODO: type
u32 unit_koura[49] = {
    0x000000DF, (u32)"btl_un_koura", 0x00080000, 0x05010100,
    0x00000064, 0x00090018, 0x00180018, 0x00180000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41C80000, 0x4179999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BTL_DAMAGE1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x10000000,
    (u32)&regist, 0x02000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 data_table[14] = {
    0x0000003B, (u32)&koura_open, 0x0000003C, (u32)&koura_close,
    0x00000009, (u32)&damage_core, 0x0000003D, (u32)&pose_tbl_reset,
    0x0000003E, (u32)&destroy_event, 0x00000031, (u32)&dead_event,
    0x00000000, 0x00000000,
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
static u32 pose_table[10] = {
    0x0000001C, (u32)"PKO_S_1", 0x0000001F, (u32)"PKO_S_1",
    0x00000027, (u32)"PKO_D_1", 0x00000044, (u32)"PKO_Z_1",
    0x00000045, (u32)"PKO_S_1",
};

// TODO: type
u32 pose_table_crack_lv1[10] = {
    0x0000001C, (u32)"PKO_S_2", 0x0000001F, (u32)"PKO_S_2",
    0x00000027, (u32)"PKO_D_2", 0x00000044, (u32)"PKO_Z_2",
    0x00000045, (u32)"PKO_S_2",
};

// TODO: type
u32 pose_table_crack_lv2[10] = {
    0x0000001C, (u32)"PKO_S_3", 0x0000001F, (u32)"PKO_S_3",
    0x00000027, (u32)"PKO_D_3", 0x00000044, (u32)"PKO_Z_3",
    0x00000045, (u32)"PKO_S_3",
};

// TODO: type
static u32 parts[38] = {
    0x00000001, (u32)"btl_un_koura", (u32)"c_koura", 0x00000000,
    0x00000000, 0x00000000, 0x41A00000, 0x42200000,
    0x00000000, 0x00000000, 0x42200000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00080009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_koura", (u32)"c_koura", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x03010000,
    0x00000000, (u32)&pose_table,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetBaseScale, -2, FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.5f))
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_GREATER(LW(0), 0)
            CALL_CHILD(koura_open)
        END_IF
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_GREATER(LW(0), 0)
            CALL_CHILD(koura_close)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(koura_open)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_PhaseEventStartDeclare, -2)
    CALL(btlevtcmd_SetRotateOffset, -2, -23, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    LOOP(30)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, 2)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_OffAttribute, LW(3), 64)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 65536)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 68)
    RETURN
EVT_END()

EVT_BEGIN(koura_close)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_PhaseEventStartDeclare, -2)
    CALL(btlevtcmd_SetRotateOffset, -2, -23, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 60)
    LOOP(30)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, -2)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OnAttribute, LW(3), 64)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 65536)
    CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(damage_core)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(1))
    CALL(btlevtcmd_GetHp, -2, LW(0))
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(3)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv2))
        IF_INT_NOT_EQUAL(LW(1), 2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 3, 2)
        CASE_LESS_OR_EQUAL(6)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv1))
        IF_INT_NOT_EQUAL(LW(1), 1)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    WAIT_MS(500)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(pose_tbl_reset)
    CALL(btlevtcmd_GetHp, -2, LW(0))
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(3)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv2))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetUnitWork, -2, 3, 2)
        CASE_LESS_OR_EQUAL(6)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv1))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OffAttribute, LW(3), 96)
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKO_H_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("PKO_H_2"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, 1, 23, 0, 0)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, 2, -23, 0, 0)
    BEGIN_SIBLING_EVENT
        LOOP(5)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(2)
        END_LOOP
        LOOP(6)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(2)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(2)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(10)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, -1, 0, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 1, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(5)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, -1, 0, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 1, 0, 0)
            WAIT_FRAMES(2)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(10)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(2)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(14)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 1, -1, 0, 0)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 2, 1, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    LOOP(20)
        CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 0, 2)
        CALL(btlevtcmd_AddPartsRotate, -2, 2, 0, 0, -2)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(8)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        WAIT_FRAMES(2)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        WAIT_FRAMES(2)
    END_LOOP
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

EVT_BEGIN(destroy_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OffAttribute, LW(3), 96)
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

// TODO: type
__declspec(section ".data") u32 lbl_80399940[2] = {
    0xBF800000, 0x3F800000,
};

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()
// clang-format on

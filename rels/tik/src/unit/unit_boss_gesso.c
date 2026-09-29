#include "unit_boss_gesso.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_pad.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_weapon_power.h"
#include "driver/effdrv.h"
#include "eff/n64/eff_opuku_jetw_n64.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"

#pragma pool_data off
// REL modules are built without -fp_contract on, but this function needs the target's fmadds/fnmsubs fusion.
#pragma fp_contract on

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 unit_boss_gesso_left_arm[];
extern s32 unit_boss_gesso_right_arm[];
static u32 regist[6];
static u32 data_table[4];
static u32 parts[19];
static s32 init_event[121];
u32 entry_gesso_left_arm_entry_data[12];
u32 entry_gesso_right_arm_entry_data[12];
static s32 damage_event[34];
static s32 phase_event[488];
static s32 attack_event[48];
s32 gesso_ikasumi_attack[193];
static s32 wait_event[6];
s32 dead_subevent[48];

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
u32 unit_boss_gesso[49] = {
    0x00000008, (u32)"btl_un_gesso", 0x000C0000, 0x01012900,
    0x00640032, 0x00090078, 0x0078002E, 0xFFB50000,
    0x00000000, 0x00000000, 0x00000000, 0x0005FF51,
    0x00000000, 0x00000000, 0x00000000, 0x42700000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42700000,
    0x00000000, 0x00000000, 0x42700000, 0x429C0000,
    0x00000000, 0x00000000, 0x42F00000, 0x00000000,
    0x42F00000, 0x42DC0000, 0x0000FF00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_GESSO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00200000,
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
    0x64646464, 0x64646464, 0x64646464, 0x64646400,
    0x00646400, 0x00000000,
};

// TODO: type
u32 defence_ceil[2] = {
    0x02020202, 0x02000000,
};

// TODO: type
u32 defence_attr_ceil[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
u32 regist_ceil[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 R_defence_turn[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
u32 R_defence_attr_turn[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
u32 R_regist_turn[6] = {
    0x64646464, 0x64646464, 0x64646464, 0x64646400,
    0x00646400, 0x00000000,
};

// TODO: type
static u32 pose_table[22] = {
    0x00000001, (u32)"GSO_N2_1", 0x00000002, (u32)"GSO_D2_2",
    0x00000009, (u32)"GSO_D2_2", 0x00000005, (u32)"GSO_K2_1",
    0x00000004, (u32)"GSO_X2_1", 0x00000003, (u32)"GSO_K2_1",
    0x0000001C, (u32)"GSO_S2_1", 0x0000001D, (u32)"GSO_Q2_1",
    0x0000001F, (u32)"GSO_S2_1", 0x00000027, (u32)"GSO_D2_1",
    0x00000045, (u32)"GSO_S2_1",
};

// TODO: type
static u32 pose_table_ceil[24] = {
    0x00000001, (u32)"GSO_N1_1", 0x00000002, (u32)"GSO_D1_3",
    0x00000009, (u32)"GSO_D1_3", 0x00000005, (u32)"GSO_K1_1",
    0x00000004, (u32)"GSO_X1_1", 0x00000003, (u32)"GSO_K1_1",
    0x0000001C, (u32)"GSO_S1_1", 0x0000001D, (u32)"GSO_Q1_2",
    0x0000001F, (u32)"GSO_S1_1", 0x00000027, (u32)"GSO_D1_2",
    0x00000041, (u32)"GSO_K1_1", 0x00000045, (u32)"GSO_S1_1",
};

// TODO: type
u32 gesso_pose_table_turn[20] = {
    0x00000001, (u32)"GSO_N3_1", 0x00000002, (u32)"GSO_D3_2",
    0x00000009, (u32)"GSO_D3_2", 0x00000005, (u32)"GSO_K3_1",
    0x00000004, (u32)"GSO_K3_1", 0x00000003, (u32)"GSO_K3_1",
    0x0000001C, (u32)"GSO_V_1", 0x0000001F, (u32)"GSO_D3_1",
    0x00000027, (u32)"GSO_D3_1", 0x00000045, (u32)"GSO_V_1",
};

// TODO: type
static u32 data_table[4] = {
    0x00000032, (u32)&dead_subevent, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_gesso", (u32)"c_gesso", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0067FF51, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

USER_FUNC(_test_rtn) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnit* target;
    s32 id;
    s32 transId;
    s32 padTrigger;

    id = BattleTransID(event, -2);
    if (id == -1) {
        return EVT_RETURN_DONE;
    }
    unit = BattleGetUnitPtr(wp, id);
    if (unit == NULL) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    padTrigger = BattlePadMultiCheckTrigger(2, 1);
    if (padTrigger) {
        event->userdata[0]++;
        if (event->userdata[0] >= 3) {
            event->userdata[0] = 0;
        }
    }
    padTrigger = BattlePadMultiCheckTrigger(2, 2);
    if (padTrigger) {
        event->userdata[0]--;
        if (event->userdata[0] < 0) {
            event->userdata[0] = 2;
        }
    }
    switch (event->userdata[0]) {
        case 0:
            transId = BattleTransID(event, -2);
            break;
        case 1:
            transId = unit->work[1];
            break;
        case 2:
            transId = unit->work[2];
            break;
    }
    target = BattleGetUnitPtr(wp, transId);
    BattlePadMultiCheckTrigger(2, 3);
    padTrigger = BattlePadMultiCheckNow(2, 0x40);
    if (padTrigger) {
        target->mScale.x += 0.02f;
        target->mScale.y += 0.02f;
        target->mScale.z += 0.02f;
    }
    padTrigger = BattlePadMultiCheckNow(2, 0x20);
    if (padTrigger) {
        target->mScale.x -= 0.02f;
        target->mScale.y -= 0.02f;
        target->mScale.z -= 0.02f;
    }
    target->mPosition.x = 0.1f * keyGetStickX(2) + target->mPosition.x;
    target->mPosition.y = 0.1f * keyGetSubStickY(2) + target->mPosition.y;
    target->mPosition.z = target->mPosition.z - 0.1f * keyGetStickY(2);
    BattlePadMultiCheckTrigger(2, 0x100);
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(unk_evt_tik_000243c8)
    CALL(_test_rtn)
    RETURN
EVT_END()

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    WAIT_MS(1000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_APPEAR1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GSO_F_4"))
    CALL(btlevtcmd_OffAttribute, -2, 16777216)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("GSO_X1_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("GSO_X1_2"))
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 84, 177, 558, 84, 140, 10, 30, 11)
    WAIT_FRAMES(30)
    CALL(evt_msg_print, 2, STRING("tik_02_gs_01"), 0, -2)
    CALL(evt_snd_sfxon_3d, STRING("SFX_CONDITION_FIND1"), 95, 65, 0, 0)
    CALL(evt_eff_fukidashi, 2, STRING(""), 0, 0, 0, 95, 65, 0, 0, 135, 60)
    WAIT_FRAMES(60)
    CALL(evt_msg_print, 2, STRING("tik_02_gs_02"), 0, -2)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 63, 143, 646, 63, 100, 0, 30, 11)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetUnitId, -2, LW(10))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_gesso_left_arm_entry_data), 0)
    CALL(btlevtcmd_StopWaitEvent, LW(3))
    CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(10))
    CALL(btlevtcmd_SetPos, LW(3), 320, 6, 53)
    CALL(btlevtcmd_SetHomePos, LW(3), 320, 6, 53)
    BEGIN_INLINE_EVENT_GET_ID(LW(14))
        WAIT_FRAMES(35)
        CALL(btlevtcmd_snd_se, LW(3), STRING("SFX_BOSS_GESSO_ARM_APPEAR1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("GSO_F2_1"))
        CALL(btlevtcmd_OffAttribute, LW(3), 16777216)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(3), 1)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_INLINE_EVENT
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_gesso_right_arm_entry_data), 0)
    CALL(btlevtcmd_StopWaitEvent, LW(3))
    CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(10))
    CALL(btlevtcmd_SetPos, LW(3), 59, 166, -18)
    CALL(btlevtcmd_SetHomePos, LW(3), 59, 166, -18)
    BEGIN_INLINE_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(5)
        CALL(btlevtcmd_snd_se, LW(3), STRING("SFX_BOSS_GESSO_ARM_APPEAR1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("GSO_F2_1"))
        CALL(btlevtcmd_OffAttribute, LW(3), 16777216)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(3), 1)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_INLINE_EVENT
    SPAWN_EVENT(unk_evt_tik_000243c8)
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 84, 177, 558, 84, 140, 10, 30, 11)
    WAIT_FRAMES(30)
    CALL(evt_msg_print, 2, STRING("tik_02_gs_03"), 0, -2)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, -117, 62, 284, -115, 43, 0, 30, 11)
    WAIT_MS(500)
    CALL(btlevtcmd_SetTalkPose, -4, STRING("PKR_T_1"))
    CALL(btlevtcmd_SetStayPose, -4, STRING("PKR_S_1"))
    CALL(evt_msg_print, 2, STRING("tik_02_gs_03_01"), 0, -4)
    CALL(btlevtcmd_StartWaitEvent, -4)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 220)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OnAttribute, -2, 2)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_ceil))
    CALL(btlevtcmd_SetHpGaugeOffset, -2, 5, -175)
    CALL(btlevtcmd_SetStatusIconOffset, -2, 46, -75)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 9, -122, 30)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 9, -122, 30)
    CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(defence_ceil))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(defence_attr_ceil))
    CALL(btlevtcmd_SetRegistStatus, -2, PTR(regist_ceil))
    CALL(btlevtcmd_SetStatusIconOffset, -2, 46, -75)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
u32 entry_gesso_left_arm_entry_data[12] = {
    (u32)unit_boss_gesso_left_arm, 0x01000000, 0x04000004, 0x00000000,
    0xC47A0000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 entry_gesso_right_arm_entry_data[12] = {
    (u32)unit_boss_gesso_right_arm, 0x01000000, 0x04000004, 0x00000000,
    0xC47A0000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 1)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 3)
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
            ADD(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 3, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 2)
                CALL(btlevtcmd_PhaseEventStartDeclare, -2)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GSO_U_1"))
                CALL(btlevtcmd_ftof, 75, LW(0))
                WAIT_FRAMES(LW(0))
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_WAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_ftof, 45, LW(0))
                WAIT_FRAMES(LW(0))
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_WAKE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
                CALL(btlevtcmd_SetHpGaugeOffset, -2, 0, 0)
                CALL(btlevtcmd_SetStatusIconOffset, -2, 46, 118)
                CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(defence))
                CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(defence_attr))
                CALL(btlevtcmd_SetRegistStatus, -2, PTR(regist))
                CALL(btlevtcmd_SetStatusIconOffset, -2, 55, 113)
                CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
                CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
                CALL(btlevtcmd_StartWaitEvent, -2)
                RETURN
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108869)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_EQUAL(LW(0), 2)
            CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
            ADD(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 3, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 2)
                CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
                CALL(btlevtcmd_PhaseEventStartDeclare, -2)
                CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
                CALL(btlevtcmd_OnAttribute, -2, 2)
                CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_ceil))
                CALL(btlevtcmd_SetHpGaugeOffset, -2, 5, -175)
                CALL(btlevtcmd_SetStatusIconOffset, -2, 46, -75)
                CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
                CALL(btlevtcmd_SetHitOffset, -2, 1, 9, -122, 30)
                CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 9, -122, 30)
                CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(defence_ceil))
                CALL(btlevtcmd_SetPartsDefenceAttrTable, -2, 1, PTR(defence_attr_ceil))
                CALL(btlevtcmd_SetRegistStatus, -2, PTR(regist_ceil))
                CALL(btlevtcmd_SetStatusIconOffset, -2, 46, -75)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GSO_J_1"))
                WAIT_FRAMES(48)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GSO_J_2"))
                SET_VALUE(LW(5), 0)
                LOOP(4)
                    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                    SWITCH(LW(5))
                        CASE_EQUAL(0)
                        ADD(LW(1), 50)
                        CASE_EQUAL(1)
                        ADD(LW(1), 75)
                        CASE_DEFAULT
                        ADD(LW(1), 100)
                    END_SWITCH
                    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_UP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 0, 5, 0, -1)
                    ADD(LW(5), 1)
                    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                    SUBTRACT(LW(1), 30)
                    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 16, 0, 11, 0, -1)
                END_LOOP
                CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                SET_VALUE(LW(1), 220)
                CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_APPEAR1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GSO_F_4"))
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                WAIT_MS(333)
                CALL(btlevtcmd_GetUnitId, -2, LW(10))
                CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_gesso_left_arm_entry_data), 0)
                CALL(btlevtcmd_StopWaitEvent, LW(3))
                CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
                CALL(btlevtcmd_SetUnitWork, -2, 1, LW(3))
                CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(10))
                CALL(btlevtcmd_SetPos, LW(3), 320, 6, 53)
                CALL(btlevtcmd_SetHomePos, LW(3), 320, 6, 53)
                BEGIN_INLINE_EVENT_GET_ID(LW(14))
                    WAIT_FRAMES(35)
                    CALL(btlevtcmd_snd_se, LW(3), STRING("SFX_BOSS_GESSO_ARM_APPEAR1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                    CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("GSO_F2_1"))
                    CALL(btlevtcmd_OffAttribute, LW(3), 16777216)
                    CALL(btlevtcmd_AnimeWaitPlayComplete, LW(3), 1)
                    CALL(btlevtcmd_StartWaitEvent, LW(3))
                END_INLINE_EVENT
                CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_gesso_right_arm_entry_data), 0)
                CALL(btlevtcmd_StopWaitEvent, LW(3))
                CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
                CALL(btlevtcmd_SetUnitWork, -2, 2, LW(3))
                CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(10))
                CALL(btlevtcmd_SetPos, LW(3), 59, 166, -18)
                CALL(btlevtcmd_SetHomePos, LW(3), 59, 166, -18)
                BEGIN_INLINE_EVENT_GET_ID(LW(15))
                    WAIT_FRAMES(5)
                    CALL(btlevtcmd_snd_se, LW(3), STRING("SFX_BOSS_GESSO_ARM_APPEAR1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                    CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("GSO_F2_1"))
                    CALL(btlevtcmd_OffAttribute, LW(3), 16777216)
                    CALL(btlevtcmd_AnimeWaitPlayComplete, LW(3), 1)
                    CALL(btlevtcmd_StartWaitEvent, LW(3))
                END_INLINE_EVENT
                CALL(btlevtcmd_StartWaitEvent, -2)
                CALL(btlevtcmd_WaitEventEnd, LW(14))
                CALL(btlevtcmd_WaitEventEnd, LW(15))
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        GOTO(99)
    END_IF
    CALL_CHILD(gesso_ikasumi_attack)
    RETURN
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
u32 weapon_gesso_ikasumi_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x000D0302,
    0x00000000, 0x00000087, 0x000007FF, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

USER_FUNC(_ikasumi_effect) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    Vec pos;
    s32 value0, value1;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.x -= 95.0f;
    pos.y += 75.0f;
    effect = effOpukuJetwN64Entry(0, value1, pos.x, pos.y, pos.z, 2.0f);
    *(f32*)((u8*)effect->userdata + 0x38) = 135.0f;
    *(s32*)((u8*)effect->userdata + 0x18) = 0;
    *(s32*)((u8*)effect->userdata + 0x1C) = 0;
    *(s32*)((u8*)effect->userdata + 0x20) = 0;
    *(s32*)((u8*)effect->userdata + 0x28) = 0;
    *(s32*)((u8*)effect->userdata + 0x2C) = 0;
    *(s32*)((u8*)effect->userdata + 0x30) = 0;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(gesso_ikasumi_attack)
    SET_VALUE(LW(12), PTR(weapon_gesso_ikasumi_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GSO_A2_1"))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ftof, 16, LW(0))
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_UP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_ftof, 55, LW(0))
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_UP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ftof, 120, LW(0))
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ftof, 90, LW(7))
    CALL(_ikasumi_effect, -2, LW(7))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_SUMI1"), EVTDAT_POINTER_MAX, 0, LW(15))
        WAIT_FRAMES(LW(7))
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    CALL(btlevtcmd_ftof, 10, LW(0))
    WAIT_FRAMES(LW(0))
    SET_VALUE(LW(10), 0)
    LABEL(50)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 1)
        SWITCH(LW(5))
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_SWITCH
    ELSE
        IF_INT_EQUAL(LW(10), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    ADD(LW(10), 1)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(50)
    END_IF
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

EVT_BEGIN(dead_subevent)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
    CALL(btlevtcmd_GetUnitId, LW(3), LW(3))
    BEGIN_SIBLING_EVENT_GET_ID(LW(1))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_KillUnit, LW(3), 0)
        END_IF
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
    CALL(btlevtcmd_GetUnitId, LW(3), LW(3))
    BEGIN_SIBLING_EVENT_GET_ID(LW(2))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_KillUnit, LW(3), 0)
        END_IF
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(1))
    CALL(btlevtcmd_WaitEventEnd, LW(2))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "manager/evtmgr_cmd.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(lecture_init);
static USER_FUNC(_battle_set_lecture_unit_id);
static u32 regist[6];
static u32 data_table[2];
static u32 parts[19];
static s32 init_event[53];
static s32 entry_event[29];
static s32 phase_event[160];
static s32 damage_event[10];
static s32 wait_event[6];

// clang-format off
extend_data(152,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(153,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_lecture_frankli_kuriboo[49] = {
    0x000000B4, (u32)"btl_un_frankli", 0x000A0000, 0x01010100,
    0x00000064, 0x0009001C, 0x00180005, 0x00190000,
    0x00000000, 0x41600000, 0x00000000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x41600000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0x00000000, 0x41600000, 0x4179999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_KURI_DAMAGED11",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x12000000,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[10] = {
    0x0000001B, (u32)"D_1", 0x0000001C, (u32)"S_1",
    0x0000001F, (u32)"D_2", 0x00000027, (u32)"D_2",
    0x00000045, (u32)"S_1",
};

// TODO: type
static u32 data_table[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_frankli", (u32)"c_kuri_h", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00890009, 0x00000000, (u32)&pose_table,
};

static USER_FUNC(_battle_set_lecture_unit_id) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;

    work->field_424 = evtGetValue(event, *args++);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_init) {
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(init_event)
    CALL(lecture_init)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(_battle_set_lecture_unit_id, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetPos, -2, 0, -1000, 0)
    CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    WAIT_MS(900)
    CALL(btlevtcmd_SetPos, -2, -250, 0, -30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    CALL(btlevtcmd_MovePosition, -2, -60, 0, -30, 45, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_get_turn, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_S_1"))
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(evt_msg_print, 2, STRING("mac_0_092_01"), 0, -2)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 71, 49, 213, 71, 35, 0, 30, 11)
            WAIT_FRAMES(30)
            CALL(evt_msg_toge, 1, 0, 0, 0)
            CALL(evt_msg_print, 2, STRING("mac_0_092_02"), 0, -2)
            CALL(evt_btl_camera_set_moveto, 0, 111, 85, 213, 111, 71, 0, 30, 11)
            WAIT_FRAMES(30)
            CALL(evt_msg_toge, 1, 0, 0, 0)
            CALL(evt_msg_print, 2, STRING("mac_0_092_03"), 0, -2)
            CALL(btlevtcmd_ChangeFaceDirection, -2, -1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_FRAMES(60)
            CALL(evt_msg_print, 2, STRING("mac_0_092_04"), 0, -2)
            CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
            WAIT_MS(1500)
            CALL(btlevtcmd_ChangeFaceDirection, -2, -1)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, -60, 82, 340, -60, 60, 13, 30, 11)
            WAIT_FRAMES(30)
            CALL(evt_msg_print, 2, STRING("mac_0_092_05"), 0, -2)
            CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            GOTO(99)
        END_IF
    END_IF
    LABEL(99)
    RETURN
EVT_END()

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
// clang-format on
#pragma force_active off

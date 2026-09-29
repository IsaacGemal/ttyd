#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(oshimodo_event_sub)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_MS(750)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("tik_08_01"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_ctrl_onoff, 4, 1)
    RETURN
EVT_END()

static EVT_BEGIN(oshimodo_event)
    SPAWN_EVENT(oshimodo_event_sub)
    SET_VALUE(GSWF(1341), 1)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_oshimodo[] = {
    {"dokan", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, oshimodo_event, NULL, "dokan", 1, 1},
};

static BeroEntry bero_entry_data[] = {
    {"dokan", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "usu_00", "dokan_1", 1, 1},
    {"s_bero", 1, 1, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_07", "n_bero", 2, 2,
     (s32)"door", 0},
    {NULL},
};

static BeroEntry bero_entry_data2[] = {
    {"dokan", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "sys_01", "stg_4", 1, 1},
    {NULL},
};

static EVT_BEGIN(evt_kanban)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_msg_print, 0, STRING("tik_08_02"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tik_08_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_LESS_OR_EQUAL(GSW(0), 175)
        SET_VALUE(LW(0), STRING("dokan"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan"), PTR(bero_oshimodo))
        SET_VALUE(LW(0), STRING("dokan"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 176)
        IF_INT_LESS(GSW(0), 178)
            SET_VALUE(LW(0), STRING("dokan"))
            CALL_CHILD(bero_case_switch_off)
            CALL(evt_bero_overwrite, STRING("dokan"), PTR(bero_entry_data2))
            SET_VALUE(LW(0), STRING("dokan"))
            CALL_CHILD(bero_case_switch_on)
        END_IF
    END_IF
    CALL(evt_mobj_signboard, STRING("board"), 85, 0, 0, PTR(evt_kanban), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

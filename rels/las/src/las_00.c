#include "las_00.h"
#include <stdio.h>
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(make_name);

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_05", "n_bero", 5, 5,
     w_door_close, w_door_open},
    {"e_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_01", "w_bero", 5, 5,
     e_door_close, e_door_open},
    {NULL},
};

static EVT_BEGIN(first_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_set_party, 0, 1)
    END_IF
    CALL(evt_mario_set_pos, -420, 10, 0)
    CALL(evt_party_set_pos, 0, -470, 10, 0)
    CALL(evt_cam3d_evt_set, -385, 1441, 763, -37, 642, 100, 0, 11)
    WAIT_FRAMES(1)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 577, 35, 520, -155, 135, 99, 7000, 0)
        WAIT_MS(7000)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    CALL(evt_mario_normalize)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_hitobj_onoff, STRING("w_bero"), 1, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -350, 0, FLOAT(150.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -300, 0, FLOAT(150.0f))
    CALL(evt_hitobj_onoff, STRING("w_bero"), 1, 1)
    LOOP(0)
        IF_INT_NOT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -18, 20, 205, -453, 80, -131, 2400, 11)
    WAIT_MS(2400)
    CALL(evt_party_stop, 0)
    CALL(evt_party_set_dir, 0, 270, 300)
    WAIT_MS(500)
    CALL(evt_party_set_dir, 0, 90, 300)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg8_las_01"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    SET_VALUE(GSW(0), 382)
    CALL_CHILD(bero_case_entry)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_snd_bgm_scope, 0, 0)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_telop_entry, 17)
    RETURN
EVT_END()

static USER_FUNC(make_name) {
    static char name[0x10];
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    sprintf(name, "S_shan_rou_0%d", value);
    evtSetValue(event, args[1], (s32)name);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(shan_init)
    SET_VALUE(LW(0), 1)
    LOOP(12)
        CALL(make_name, LW(0), LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
        CALL(evt_mapobj_get_position, LW(1), LW(2), LW(3), LW(4))
        SUBTRACT_FLOAT(LW(3), FLOAT(9.0f))
        CALL(evt_eff, 0, STRING("fire"), 3, LW(2), LW(3), LW(4), FLOAT(1.0f), 1000, 0, 0, 0, 0, 0, 0)
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(las_00_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 382)
        CALL_CHILD(evt_bero_info_run)
    END_IF
    IF_INT_LESS(GSW(0), 382)
        SPAWN_EVENT(first_evt)
    END_IF
    CALL_CHILD(shan_init)
    RETURN
EVT_END()

// clang-format on

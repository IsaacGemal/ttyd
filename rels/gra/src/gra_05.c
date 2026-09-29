#include "gra_05.h"
#include "cam_road.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "rel/gra/event/evt_iwamove.h"
#include "rel/gra/event/evt_kagemario.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(gra_05_in_01)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(gra_05_out_01)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set_now)
    CALL(evt_mario_cam_y_off)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(evt_hit_get_position, STRING("A_ana_1"), LW(0), LW(1), LW(2))
        CALL(evt_party_force_reset_outofscreen, 0)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(150.0f))
    END_INLINE_EVENT
    LOOP(0)
        IS_EVENT_RUNNING(LW(10), LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_set_pos, 250, -50, 0)
    CALL(evt_party_set_pos, 0, 250, -50, 0)
    CALL(evt_party_set_pos, 1, 250, -50, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, -1)
    CALL(evt_party_set_dir, 1, 90, -1)
    CALL(evt_cam3d_evt_off, 500, 0)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_party_jump_pos, 0, 310, 0, 0, 700, 0, -1000)
        CALL(evt_party_run, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_party_jump_pos, 1, 310, 0, 0, 700, 0, -1000)
        CALL(evt_party_run, 1)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP2"), LW(1), LW(2), LW(3), 0)
    CALL(evt_mario_jump_pos, 310, 0, 0, 700, 0, -1000)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_cam_y_on)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(gra_05_in_02)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(gra_05_out_02)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set_now)
    CALL(evt_mario_cam_y_off)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(evt_hit_get_position, STRING("A_ana_2"), LW(0), LW(1), LW(2))
        CALL(evt_party_force_reset_outofscreen, 0)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(150.0f))
    END_INLINE_EVENT
    LOOP(0)
        IS_EVENT_RUNNING(LW(10), LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_set_pos, 0, -50, 0)
    CALL(evt_party_set_pos, 0, 0, -50, 0)
    CALL(evt_party_set_pos, 1, 0, -50, 0)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_party_set_dir, 1, 270, -1)
    CALL(evt_cam3d_evt_off, 500, 0)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_party_jump_pos, 0, -60, 0, 0, 700, 0, -1000)
        CALL(evt_party_run, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_party_jump_pos, 1, -60, 0, 0, 700, 0, -1000)
        CALL(evt_party_run, 1)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP4"), LW(1), LW(2), LW(3), 0)
    CALL(evt_mario_jump_pos, -60, 0, 0, 700, 0, -1000)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_cam_y_off)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static EVT_BEGIN(in_evt)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static EVT_BEGIN(out_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set_now)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(evt_hit_get_position, STRING("ana_1"), LW(0), LW(1), LW(2))
        CALL(evt_party_force_reset_outofscreen, 0)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(150.0f))
    END_INLINE_EVENT
    LOOP(0)
        IS_EVENT_RUNNING(LW(10), LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_run, 0)
    CALL(evt_mario_bgmode_on)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_mario_cam_y_off)
    CALL(evt_cam_road_reset)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 700, 11)
    CALL(evt_hit_get_position, STRING("ana_2"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 50)
    ADD(LW(2), -10)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP5"), LW(1), LW(2), LW(3), 0)
    CALL(evt_mario_jump_pos, -140, 60, -450, 700, 0, -1000)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static EVT_BEGIN(out_evt2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set_now)
    CALL(evt_hit_get_position, STRING("ana_2"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 50)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 500, 0, -1000)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_pos, -260, -60, 0)
    CALL(evt_party_set_pos, 0, -260, -70, -5)
    CALL(evt_party_set_pos, 1, -260, -80, -10)
    CALL(evt_mario_bgmode_off)
    CALL(evt_mario_set_prev_party, 0)
    CALL(evt_mario_set_prev_party, 1)
    CALL(evt_cam_road_reset)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 700, 11)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, -1)
    CALL(evt_party_set_dir, 1, 90, -1)
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_party_jump_pos, 0, -190, 0, -10, 700, 0, -1000)
        CALL(evt_party_run, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_party_jump_pos, 1, -190, 0, -20, 700, 0, -1000)
        CALL(evt_party_run, 1)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP3"), LW(1), LW(2), LW(3), 0)
    CALL(evt_mario_jump_pos, -190, 0, 0, 700, 0, -1000)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[7] = {
    {
        "w_bero",
        0x0000, 0x0000,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        0x00000000,
        0xFFFFFFFF,
        0x00000000,
        "gra_04",
        "e_bero_2",
        0x0001, 0x0001,
        0x00000000,
        0x00000000,
    },
    {
        "e_bero",
        0x0000, 0x0000,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        0x00000000,
        0xFFFFFFFF,
        0x00000000,
        "gra_06",
        "sw_bero",
        0x0001, 0x0001,
        0x00000000,
        0x00000000,
    },
    {
        "ana_1",
        0x0F00, 0x0000,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        in_evt,
        0x00000000,
        out_evt,
        "gra_05",
        "ana_2",
        0x0004, 0x0004,
        0x00000000,
        0x00000000,
    },
    {
        "ana_2",
        0x1F00, 0x0000,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        in_evt,
        0x00000000,
        out_evt2,
        "gra_05",
        "ana_1",
        0x0004, 0x0004,
        0x00000000,
        0x00000000,
    },
    {
        "A_ana_1",
        0x0F00, 0x0000,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        gra_05_in_01,
        0x00000000,
        gra_05_out_01,
        0x00000000,
        0x00000000,
        0x0004, 0x0004,
        0x00000000,
        0x00000000,
    },
    {
        "A_ana_2",
        0x0F00, 0x0000,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        gra_05_in_01,
        0x00000000,
        gra_05_out_02,
        0x00000000,
        0x00000000,
        0x0004, 0x0004,
        0x00000000,
        0x00000000,
    },
    {
        0x00000000,
        0x0000, 0x0000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x0000, 0x0000,
        0x00000000,
        0x00000000,
    },
};

EVT_BEGIN(gra_05_futa_open_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(1000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_hitobj_onoff, STRING("A_futa"), 0, 0)
    SET_VALUE(GSW(0), 190)
    RETURN
EVT_END()

EVT_BEGIN(evt_gra_05iwa_move_init)
    CALL(evt_hit_bind_mapobj, STRING("A_iwa"), STRING("S_iwa"))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 192)
        CALL(evt_mapobj_trans, 0, STRING("S_iwa"), FLOAT(120.0f), 0, 0)
        CALL(evt_hit_bind_update, STRING("A_iwa"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_futa"), 1)
        CALL(evt_hitobj_onoff, STRING("A_futa"), 0, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_gra_05iwa_move_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_get_position, STRING("S_iwa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_MOVE_ROCK1"), LW(0), LW(1), LW(2), LW(9))
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 3000)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
    CALL(evt_mario_set_pose, STRING("M_O_1"))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    BEGIN_INLINE_EVENT
        LOOP(10)
            CALL(evt_mapobj_get_position, STRING("S_iwa"), LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 30)
            LOOP(5)
                CALL(evt_eff, 0, STRING("kemuri"), 2, LW(0), 5, 49, 0, 0, 0, 0, 0, 0, 0, 0)
                ADD(LW(0), 20)
            END_LOOP
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(10)
            CALL(evt_mapobj_get_position, STRING("S_iwa"), LW(0), LW(1), LW(2))
            CALL(evt_eff, 0, STRING("kemuri"), 2, LW(0), 52, -449, 0, 0, 0, 0, 0, 0, 0, 0)
            ADD(LW(0), 20)
            CALL(evt_eff, 0, STRING("kemuri"), 2, LW(0), 52, -449, 0, 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 0, 120, 3000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 0, STRING("S_iwa"), LW(0), 0, 0)
    SET_VALUE(LW(5), LW(2))
    ADD(LW(5), LW(0))
    CALL(evt_mario_set_pos, LW(5), LW(3), LW(4))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_hit_bind_update, STRING("A_iwa"))
    CALL(evt_snd_sfxoff, LW(9))
    SET_VALUE(GSW(0), 192)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// Defined before camera_curve_on so the deferred emitter (mwcc -inline auto,deferred)
// lays it down second, matching the original .text order (on then off).
USER_FUNC(camera_curve_off) {
    camRoadCurveOff("|MarioSt_CameraRoad|curve2");
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_break_floor)
    SET_VALUE(GSW(0), 191)
    CALL(camera_curve_on)
    RETURN
EVT_END()

static char* fukitobashi_tbl[4] = {
    "S_futa", "S_kura", "S_gra04_kusa", NULL,
};

USER_FUNC(camera_curve_on) {
    camRoadCurveOn("|MarioSt_CameraRoad|curve2");
    return EVT_RETURN_DONE;
}

EVT_BEGIN(gra_05_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_GRA1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_GRA1"))
    CALL(evt_mobj_brick, STRING("blk00"), -475, 60, 25, 0, 13, 0, GSWF(2085))
    CALL(evt_mobj_powerupblk, STRING("blk01"), 630, 60, 40, 0, GSWF(5548), 0)
    CALL(evt_kagemario_init_11_text_804)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_11_data_4F98)
    IF_INT_LESS(GSW(0), 190)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 1, PTR(fukitobashi_tbl), STRING("A_futa"), PTR(gra_05_futa_open_evt))
        CALL(evt_cloud_set_cap_size, 256, 256)
        CALL(evt_hit_bind_mapobj, STRING("A_futa"), STRING("S_futa"))
        CALL(evt_mapobj_trans, 1, STRING("S_futa"), 0, FLOAT(0.1f), 0)
        CALL(evt_hit_bind_update, STRING("A_futa"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_futa"), 1)
        CALL(evt_hitobj_onoff, STRING("A_futa"), 0, 0)
    END_IF
    IF_INT_LESS(GSW(0), 191)
        CALL(camera_curve_off)
        CALL(evt_mobj_breaking_floor, STRING("breaking_floor"), -250, -7, 0, 0, PTR(evt_break_floor), GSWF(2080))
    END_IF
    SPAWN_EVENT(evt_gra_05iwa_move_init)
    IF_INT_LESS(GSW(0), 192)
        CALL(evt_iwamove_init)
        CALL(evt_iwamove_ent, PTR(evt_gra_05iwa_move_evt))
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

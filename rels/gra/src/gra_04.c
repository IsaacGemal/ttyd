#include "gra_04.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "motion/mot_roll.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_kagemario_init_11_text_804[];
extern s32 evt_usu_kagemario_party_kill_11_data_4F98[];

// clang-format off
extend_data(16,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(17,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(in_evt)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

USER_FUNC(unk_gra_00000530) {
    s32* args = event->args;

    evtSetValue(event, args[0], !unk_8009fcbc());
    return 2;
}

static EVT_BEGIN(out_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(unk_gra_00000530, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        DEBUG_PUT_MSG(STRING("ロール解除できません"))
        CALL(evt_mario_wait_landon)
        CALL(evt_mario_mov_pos2, FLOAT(-315.0f), FLOAT(-15.0f), FLOAT(80.0f))
    END_IF
    CALL(evt_mario_normalize)
    SET_VALUE(LF(0), 0)
    SET_VALUE(LF(1), 0)
    CALL(evt_mario_get_exparty, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
        BEGIN_INLINE_EVENT_GET_ID(LW(10))
            CALL(evt_party_stop, 1)
            CALL(evt_party_move_pos2, 1, -375, -20, FLOAT(150.0f))
            CALL(evt_mario_kill_party, 1)
        END_INLINE_EVENT
    END_IF
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(1), 1)
        BEGIN_INLINE_EVENT_GET_ID(LW(11))
            CALL(evt_party_stop, 0)
            CALL(evt_party_move_pos2, 0, -375, -25, FLOAT(150.0f))
            CALL(evt_mario_kill_party, 0)
        END_INLINE_EVENT
    END_IF
    CALL(evt_mario_mov_pos2, -375, -30, FLOAT(150.0f))
    WAIT_MS(800)
    CALL(evt_cam3d_evt_set_now)
    IF_INT_EQUAL(LF(0), 1)
        DELETE_EVENT(LW(10))
    END_IF
    IF_INT_EQUAL(LF(1), 1)
        DELETE_EVENT(LW(11))
    END_IF
    WAIT_FRAMES(1)
    CALL(evt_mario_bgmode_on)
    CALL(evt_mario_cam_y_off)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_mario_set_pos, -242, 10, -450)
    CALL(evt_mario_mov_pos2, -190, -450, FLOAT(150.0f))
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_road_reset)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 1000, 11)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static EVT_BEGIN(out_evt2)
    CALL(evt_mario_key_onoff, 0)
    CALL(unk_gra_00000530, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_mario_normalize)
    ELSE
        DEBUG_PUT_MSG(STRING("ロール解除できません"))
    END_IF
    CALL(evt_mario_mov_pos2, -242, -450, FLOAT(150.0f))
    WAIT_MS(800)
    CALL(evt_cam3d_evt_set_now)
    CALL(evt_mario_bgmode_off)
    CALL(evt_mario_cam_y_on)
    CALL(evt_mario_set_pos, -380, 0, -10)
    CALL(evt_mario_set_prev_party_pos, 0, -380, 0, -15)
    CALL(evt_mario_set_prev_party_pos, 1, -380, 0, -20)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    WAIT_FRAMES(1)
    CALL(evt_party_set_pos, 0, -400, 5, -15)
    CALL(evt_party_set_pos, 1, -400, 5, -20)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_party_set_pos, 0, -373, 0, -32)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_party_move_pos2, 0, -295, -15, FLOAT(150.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -280, -10, FLOAT(150.0f))
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_road_reset)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 1000, 11)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[5] = {
    {
        "e_bero", 0, 0, 20000, 100000, 0, 0, -1, NULL, -1, NULL,
        "gra_03", "w_bero", 1, 1, 0, 0,
    },
    {
        "w_bero_1", 0, 0, 6, 100000, 0, 0, -1, in_evt, -1, out_evt,
        "gra_04", "w_bero_2", 1, 1, 0, 0,
    },
    {
        "w_bero_2", 0x1000, 0, 6, 100000, 0, 0, -1, in_evt, 0, out_evt2,
        "gra_04", "w_bero_1", 1, 1, 0, 0,
    },
    {
        "e_bero_2", 0x1000, 0, 2, -132, 63, -450, -1, NULL, 0, NULL,
        "gra_05", "w_bero", 1, 1, 0, 0,
    },
    {
        NULL, 0, 0, 0, 0, 0, 0, 0, NULL, 0, NULL, NULL, NULL, 0, 0, 0, 0,
    },
};

EVT_BEGIN(gra_04_tree_open_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(1000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_mario_party_use_check, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_hitobj_onoff, STRING("w_bero_1"), 0, 1)
    SET_VALUE(GSW(0), 189)
    RETURN
EVT_END()

static char* fukitobashi_tbl[6] = {
    "S_futa", "S_gra04_kideka", "S_gra04_ishi", "S_gra04_kusa",
    "S_gra04_dai2", NULL,
};

EVT_BEGIN(gra_04_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_GRA1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_GRA1"))
    CALL(evt_kagemario_init_11_text_804)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_11_data_4F98)
    IF_INT_LESS(GSW(0), 189)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 1, PTR(fukitobashi_tbl), STRING("A_futa"), PTR(gra_04_tree_open_evt))
        CALL(evt_cloud_set_cap_size, 256, 256)
        CALL(evt_hitobj_onoff, STRING("w_bero_1"), 0, 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_futa"), 1)
        CALL(evt_hitobj_onoff, STRING("A_futa"), 0, 0)
    END_IF
    CALL(evt_mobj_badgeblk, STRING("m"), 250, 60, -20, 249, 0, GSWF(2086), 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

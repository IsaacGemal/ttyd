#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_data[45] = {
    (u32)"e_bero_", 0x00000000, 0x00000006, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"tou_07", (u32)"e_bero",
    0x00000000, 0x00000000, 0x00000000, (u32)"w_bero_",
    0x00000000, 0x00000002, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"tou_05", (u32)"w_bero", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(evt_dakuto)
    CALL(evt_cam_shake, 4, FLOAT(0.05f), FLOAT(0.0f), 500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_DUCT_KOUSHI_HIT1"), 188, 0, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_deguti"), 1)
    CALL(evt_hitobj_onoff, STRING("deguti"), 1, 0)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_on)
    IF_INT_LESS(GSW(0), 162)
        SET_VALUE(GSW(0), 162)
    ELSE
        SET_VALUE(GSWF(2501), 1)
    END_IF
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_nusumi)
    LABEL(0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(0), 75)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_wait_landon)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_toge, 2, 200, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_455_01"), 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("m"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("p"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 60, 0, FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 100, 0, FLOAT(80.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_cam3d_evt_set, 125, 100, 337, 125, 40, -8, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 2, 200, 40, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_455_02"), 0, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_455_05"))
    WAIT_MS(1000)
    CALL(evt_msg_toge, 2, 200, 40, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_455_03"), 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 2, 200, 40, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_455_04"), 0, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 161)
    RETURN
EVT_END()

static EVT_BEGIN(evt_first)
    LABEL(0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(0), -120)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_wait_landon)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_455"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 160)
    RETURN
EVT_END()

EVT_BEGIN(tou_13_init_evt)
    SWITCH(GSW(0))
        CASE_BETWEEN(127, 137)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU7"))
        CALL(evt_snd_bgmoff, 26624)
        CASE_GREATER_OR_EQUAL(165)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU7"))
        CALL(evt_snd_bgmoff, 26624)
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU7"))
        CALL(evt_snd_set_rev_mode, 2)
    END_SWITCH
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SWITCH(GSW(0))
        CASE_LESS(162)
        SET_VALUE(LF(0), 0)
        CASE_LESS(172)
        SET_VALUE(LF(0), 1)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2501), 0)
            SET_VALUE(LF(0), 0)
        ELSE
            SET_VALUE(LF(0), 1)
        END_IF
    END_SWITCH
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_run_case_evt, 14, 1, STRING("deguti"), 0, PTR(evt_dakuto), 0)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(bero_case_switch_off)
    ELSE
        CALL(evt_hitobj_onoff, STRING("deguti"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_deguti"), 1)
    END_IF
    IF_INT_LESS(GSW(0), 161)
        SPAWN_EVENT(evt_nusumi)
    END_IF
    IF_INT_LESS(GSW(0), 160)
        SPAWN_EVENT(evt_first)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

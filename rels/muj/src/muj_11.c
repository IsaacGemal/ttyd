#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "muj_10", "A_b_bero03", 5, 5,
     (s32)&w_close, (s32)&w_open},
    {"e_bero", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "muj_12", "w_bero", 5, 5,
     (s32)&e_close, (s32)&e_open},
    {NULL},
};

EVT_BEGIN(warning_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_VOICE1"), 0)
        WAIT_MS(800)
        CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_VOICE1"), 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg5_dou_20"), 0, 0)
    SET_VALUE(GSW(0), 252)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(muj_11_init_evt)
    IF_INT_LESS(GSW(0), 253)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST1"))
        CALL(evt_snd_envoff, 512)
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ9"))
    END_IF
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_EQUAL(GSW(0), 251)
        SPAWN_EVENT(warning_event)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

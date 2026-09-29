#include "gor_10.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(depth);
static void depthDisp(void);  // TODO: real signature
static void draw(void);  // TODO: real signature

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write tegami (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(tegami_init_evt)
    CALL(evt_snd_envoff, 1024)
    CALL(evt_snd_envon, 257, STRING("ENV_STG0_GOR6"))
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    BEGIN_INLINE_EVENT
        CALL(tegami)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_seq_wait, 2)
        CALL(evt_fade_end_wait)
        CALL(evt_cam_letter_box_camid, 8)
        WAIT_MS(200)
        CALL(evt_msg_print, 1, STRING("<diary><nowinse><once_stop>"), 0, 0)
        CALL(evt_msg_print_add, 0, STRING("pro_04"))
        CALL(evt_cam_letter_box_camid, 10)
        CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
        CALL(evt_bero_mapchange, STRING("aaa_00"), STRING("prologue2"))
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(evt_mezame)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_fade_end_wait)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        WAIT_MS(2000)
        LOOP(0)
            CALL(evt_snd_sfxon, STRING("SFX_VOICE_MARIO_SLEEP1"), GW(2))
            WAIT_MS(3000)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_msg_print, 0, STRING("mac_0_012"), 0, 0)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("mac_0_013"), 0, 0)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR0"))
    DELETE_EVENT(LW(10))
    WAIT_MS(300)
    CALL(evt_snd_sfxchk, GW(2), LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(evt_snd_sfxoff, GW(2))
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_VOICE_MARIO_FIND1_2"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 2400, 1200, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 1, 1200, 2400, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 1, 2400, 1200, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 1, 1200, 2400, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, 500, 0, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 1, 2400, 0, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_msg_print, 0, STRING("mac_0_013_01"), 0, 0)
    WAIT_MS(300)
    WAIT_MS(2000)
    CALL(evt_bero_mapchange, STRING("gor_00"), 0)
    RETURN
EVT_END()

// TODO: write cam_yure (global, 0x110 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it

// TODO: write mabuta (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it

// TODO: write depth (static, 0x44 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(mezame_init_evt)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envoff, 512)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("marukongou"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("densha"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("pukupuku"), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(50.0f))
    BEGIN_INLINE_EVENT
        CALL(depth)
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(1), FLOAT(240.0f))
    BEGIN_INLINE_EVENT
        CALL(mabuta)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_yure)
    END_INLINE_EVENT
    SPAWN_EVENT(evt_mezame)
    RETURN
EVT_END()

EVT_BEGIN(gor_10_init_evt)
    CALL(evt_map_playanim, STRING("anm_umi"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_nami"), 1, 0)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        RETURN
    END_IF
    IF_INT_LESS(GSW(0), 2)
        CALL_CHILD(tegami_init_evt)
    ELSE
        CALL_CHILD(mezame_init_evt)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 wp[1] = {
    0x00000000,
};

// TODO: view_trg$599 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 view_trg$599[1] = {
//     0x00000000,
// };

// TODO: view_dy$598 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 view_dy$598[1] = {
//     0x00000000,
// };

// TODO: write mabutaDisp (global, 0x1f0 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it

// TODO: write depthDisp (static, 0x604 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it

// TODO: write tegami_delete (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it

// TODO: write draw (static, 0xc0 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_10.s; this is where the original has it, just before the first script that uses it
// clang-format on

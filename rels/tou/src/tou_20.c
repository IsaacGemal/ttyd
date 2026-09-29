#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(evt_tou_balloon)
    CALL(evt_mapobj_get_position, STRING("tou_enkei1"), LW(13), LW(14), LW(15))
    SUBTRACT(LW(14), 20)
    SUBTRACT(LW(15), 50)
    LOOP(0)
        CALL(evt_sub_random, 2, LW(0))
        SET_VALUE(LW(1), 50)
        CALL(evt_sub_random, 100, LW(10))
        SUBTRACT(LW(1), LW(10))
        SET_VALUE(LW(2), 20)
        CALL(evt_sub_random, 40, LW(10))
        SUBTRACT(LW(2), LW(10))
        SET_VALUE(LW(3), 30)
        CALL(evt_sub_random, 40, LW(10))
        SUBTRACT(LW(3), LW(10))
        ADD(LW(1), LW(13))
        ADD(LW(2), LW(14))
        ADD(LW(3), LW(15))
        CALL(evt_eff64, 0, STRING("balloon_heiho_n64"), LW(0), LW(1), LW(2), LW(3), FLOAT(0.01f), FLOAT(0.4f), 600, 0, 0, 0, 0, 0)
        CALL(evt_sub_random, 500, LW(0))
        ADD(LW(0), 300)
        WAIT_MS(LW(0))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_anim)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("tou_enkei1"), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(0)
            CALL(evt_sub_get_sincos, LW(0), LW(1), LW(2))
            ADD(LW(0), 1)
            MODULO(LW(0), 360)
            MULTIPLY_FLOAT(LW(1), FLOAT(30.0f))
            SET_FLOAT_VALUE(GW(3), LW(1))
            CALL(evt_mapobj_trans, 1, STRING("Hikousen"), 0, GW(3), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -1017, -239, -331, -21, -22, 53, 10000, 11)
    WAIT_MS(10000)
    WAIT_MS(4000)
    CALL(evt_snd_sfxoff, GW(10))
    CALL(evt_snd_sfxoff, GW(11))
    CALL(evt_bero_mapchange, STRING("sys_01"), STRING("stg_3"))
    RETURN
EVT_END()

// TODO: type
static u32 spline_dt[9] = {
    0xC44A0000, 0x4472C000, 0x45990800, 0xC4484000,
    0x445FC000, 0x45883800, 0xC43A0000, 0x445C0000,
    0x4578E000,
};

EVT_BEGIN(evt_anim2)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("Hikousen"), 1)
    CALL(evt_mapobj_trans, 1, STRING("Hikousen"), FLOAT(-808.0f), FLOAT(971.0f), FLOAT(4897.0f))
    WAIT_MS(2000)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("Hikousen"), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(2), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(3), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        WAIT_MS(20000)
        CALL(evt_snd_sfxoff, GW(10))
        CALL(evt_snd_sfxoff, GW(11))
        CALL(evt_bero_mapchange, STRING("tou_00"), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(0)
            CALL(evt_sub_get_sincos, LW(0), LW(1), LW(2))
            ADD(LW(0), 1)
            MODULO(LW(0), 360)
            IF_INT_EQUAL(GF(0), 0)
                MULTIPLY_FLOAT(LW(1), FLOAT(50.0f))
            ELSE
                MULTIPLY_FLOAT(LW(1), FLOAT(40.0f))
            END_IF
            SET_FLOAT_VALUE(GW(3), LW(1))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_spline_init, 1, 11, PTR(spline_dt), 3, 0, 10000)
    SET_VALUE(LW(3), 1)
    LOOP(0)
        IF_INT_EQUAL(LW(3), 1)
            CALL(evt_sub_spline_get_value, GW(0), GW(1), GW(2), LW(3))
        ELSE
        END_IF
        SET_FLOAT_VALUE(LW(4), GW(1))
        ADD_FLOAT(LW(4), GW(3))
        CALL(evt_mapobj_trans, 1, STRING("Hikousen"), GW(0), LW(4), GW(2))
        CALL(evt_mapobj_rotate, 1, STRING("Hikousen"), -10, 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("Hikousen"), 0, -100, 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(tou_20_init_evt)
    CALL(evt_snd_envoff, 512)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("naka"), LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 36)
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 30)
            CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_mario_adjust_dir)
            CALL(evt_mario_set_dir, 90, 0, 0)
            CALL(evt_party_set_dir, 0, 90, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("lightcoloer"), 1, 0)
    CALL(evt_map_playanim, STRING("jet"), 1, 0)
    CALL(evt_map_playanim, STRING("omori"), 1, 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_rotate, 1, STRING("puro_2"), LW(0), 0, 0)
            ADD(LW(0), 20)
            MODULO(LW(0), 360)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_rotate, 1, STRING("puro_3"), LW(0), 0, 0)
            ADD(LW(0), 20)
            MODULO(LW(0), 360)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("start"))
        CALL(evt_cam3d_evt_set, -3020, 129, -6, -212, 722, 807, 0, 11)
        SPAWN_EVENT(evt_anim)
    ELSE
        CALL(evt_cam3d_evt_set, -700, 866, 4657, -700, 38, -60, 0, 11)
        SPAWN_EVENT(evt_anim2)
    END_IF
    CALL(evt_bg_auto_scroll_onoff, 1)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
        LOOP(0)
            ADD_FLOAT(LW(0), FLOAT(0.002f))
            CALL(evt_bg_set_scrl_offset, LW(0), LW(1))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(GW(10), 0)
    SET_VALUE(GW(11), 0)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(5)
        CALL(evt_mapobj_get_position, STRING("Hikousen"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AIR_SHIP_DRIVE1"), LW(0), LW(1), LW(2), GW(10))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AIR_SHIP_DRIVE1_2"), LW(0), LW(1), LW(2), GW(11))
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("Hikousen"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, GW(10), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, GW(11), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

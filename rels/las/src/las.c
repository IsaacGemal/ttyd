#include "las.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

// TODO: write mobj_set_ry (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/las/asm/las.s; this is where the original has it, just before the first script that uses it

// TODO: write mobj_color_off (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/las/asm/las.s; this is where the original has it, just before the first script that uses it

// TODO: write mobj_set_color (global, 0xb0 bytes) here:
// its asm is in build/G8MJ01/las/asm/las.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(box_syutugen)
    SET_VALUE(LW(2), LW(0))
    CALL(evt_mobj_get_position, LW(2), LW(3), LW(4), LW(5))
    CALL(evt_eff, STRING("mist"), STRING("mist2"), 0, LW(3), 3, LW(5), 25, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_TBOX_APPEAR1"), LW(3), LW(4), LW(5), LW(8))
    BEGIN_INLINE_EVENT_GET_ID(LW(9))
        LOOP(0)
            CALL(evt_mobj_get_y_position, LW(2), LW(1))
            SUBTRACT(LW(1), LW(4))
            SUBTRACT(LW(1), 3)
            CALL(evt_eff, 0, STRING("mist2"), 1, LW(3), 0, LW(5), 15, LW(1), 100, 0, 0, 0, 0, 0)
            WAIT_MS(200)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1750)
        CALL(evt_mobj_get_position, LW(2), LW(3), LW(4), LW(5))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_TBOX_APPEAR2"), LW(3), LW(4), LW(5), 0)
    END_INLINE_EVENT
    CALL(mobj_set_color, LW(2), 0, 0, 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 1000, 3000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(3), FLOAT(30.0f))
    ADD_FLOAT(LW(3), FLOAT(-30.0f))
    CALL(evt_mobj_set_y_position, LW(2), LW(3))
    MULTIPLY_FLOAT(LW(0), FLOAT(360.0f))
    CALL(mobj_set_ry, LW(2), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_eff_softdelete, STRING("mist"))
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(mobj_set_color, LW(2), LW(0), LW(0), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(mobj_color_off, LW(2))
    CALL(mobj_set_ry, LW(2), 0)
    CALL(evt_mobj_set_y_position, LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(move_dai)
    SET_VALUE(LW(7), LW(0))
    SET_VALUE(LW(8), LW(1))
    SET_VALUE(LW(9), LW(2))
    CALL(evt_mapobj_trans, 1, LW(7), 0, 0, -100)
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(7), 1)
    CALL(evt_hitobj_onoff, LW(8), 0, 1)
    CALL(evt_mapobj_get_position, LW(7), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_FLOOR_MOVE1"), LW(0), LW(1), LW(2), LW(6))
    CALL(evt_sub_intpl_msec_init, 11, -100, 0, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, LW(7), 0, 0, LW(0))
    CALL(evt_hit_bind_update, LW(8))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(6))
    WAIT_MS(LW(9))
    CALL(evt_mapobj_get_position, LW(7), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_FLOOR_MOVE1"), LW(0), LW(1), LW(2), LW(6))
    CALL(evt_sub_intpl_msec_init, 11, 0, -100, 1000)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, LW(7), 0, 0, LW(0))
    CALL(evt_hit_bind_update, LW(8))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_snd_sfxoff, LW(6))
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(7), 1)
    CALL(evt_hitobj_onoff, LW(8), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(hikari_evt)
    LOOP(0)
        CALL(evt_sub_random, 5, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(1), STRING("hikari_01"))
            CASE_EQUAL(1)
            SET_VALUE(LW(1), STRING("hikari_02"))
            CASE_EQUAL(2)
            SET_VALUE(LW(1), STRING("hikari_03"))
            CASE_EQUAL(3)
            SET_VALUE(LW(1), STRING("hikari_04"))
            CASE_EQUAL(4)
            SET_VALUE(LW(1), STRING("hikari_05"))
            CASE_EQUAL(5)
            SET_VALUE(LW(1), STRING("hikari_06"))
        END_SWITCH
        CALL(evt_map_playanim, LW(1), 0, 0)
        CALL(evt_map_checkanim, LW(1), LW(2), LW(3))
        WAIT_MS(LW(3))
    END_LOOP
    RETURN
EVT_END()

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/las/asm/las.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/las/asm/las.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x2d4 bytes) here:
// its asm is in build/G8MJ01/las/asm/las.s; this is where the original has it, just before the first script that uses it
// clang-format on

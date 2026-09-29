#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

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

static EVT_BEGIN(w1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e3_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door04"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e3_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door04"), 0, LW(0), 0)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[90] = {
    (u32)"w_bero_1", 0x0001000C, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"gon_03", (u32)"e_bero",
    0x00050005, (u32)&w1_door_close, (u32)&w1_door_open, (u32)"w_bero_2",
    0x0001000C, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"gon_08", (u32)"e_bero_1", 0x00050005,
    (u32)&w2_door_close, (u32)&w2_door_open, (u32)"e_bero_1", 0x0001000C,
    0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)"gon_05", (u32)"w_bero_1", 0x00050005, (u32)&e1_door_close,
    (u32)&e1_door_open, (u32)"e_bero_2", 0x00000000, 0x00004E20,
    0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)"gon_05",
    (u32)"w_bero_2", 0x00010001, 0x00000000, 0x00000000,
    (u32)"e_bero_3", 0x0001000C, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"gon_05", (u32)"w_bero_3",
    0x00050005, (u32)&e3_door_close, (u32)&e3_door_open, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(evt_gon_04flowblock_updown1_init)
    IF_INT_EQUAL(1, 0)
        IF_INT_NOT_EQUAL(LSWF(6), 0)
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl01"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(225.0f))
            CALL(evt_mobj_set_y_position, STRING("mobj_fltbl01"), LW(5))
        END_IF
    ELSE
        IF_INT_EQUAL(LSWF(6), 0)
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl01"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(225.0f))
            CALL(evt_mobj_set_y_position, STRING("mobj_fltbl01"), LW(5))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_gon_04flowblock_updown1)
    SET_TYPE(239)
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    IF_INT_NOT_EQUAL(1, 0)
        SET_VALUE(LW(10), 0)
        IF_INT_EQUAL(1, 0)
            IF_INT_EQUAL(LSWF(6), 0)
                SET_VALUE(LW(10), 0)
            ELSE
                SET_VALUE(LW(10), 1)
            END_IF
        ELSE
            IF_INT_EQUAL(LSWF(6), 0)
                SET_VALUE(LW(10), 1)
            ELSE
                SET_VALUE(LW(10), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl01"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            ADD_FLOAT(LW(3), FLOAT(225.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
            SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
        ELSE
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl01"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            SUBTRACT_FLOAT(LW(3), FLOAT(225.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(-0.2f))
            SET_FLOAT_VALUE(LW(9), 0)
        END_IF
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(10)
            CALL(evt_sub_rumble_onoff, 0, 0)
            WAIT_MS(300)
            CALL(evt_sub_rumble_onoff, 1, 0)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(10)
            CALL(evt_sub_intpl_init, 11, 10, 3, 4)
            LABEL(0)
            CALL(evt_sub_intpl_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            CALL(evt_cam_shake, 4, FLOAT(0.0f), LW(0), 100)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
        END_SIBLING_EVENT
        WAIT_MS(500)
        CALL(evt_mobj_get_position, STRING("mobj_fltbl01"), LW(12), LW(13), LW(14))
        CALL(evt_mobj_get_kindname, STRING("mobj_fltbl01"), LW(11))
        SET_VALUE(LF(0), 0)
        IF_STR_EQUAL(LW(11), STRING("MOBJ_green_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(11), STRING("MOBJ_purple_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(11), STRING("MOBJ_orange_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LF(1), 0)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_MOVE1"), LW(12), LW(13), LW(14), LW(15))
            ELSE
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_S_FLOAT_MOVE1"), LW(12), LW(13), LW(14), LW(15))
            END_IF
        ELSE
            SET_VALUE(LF(1), 1)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_DOWN1"), LW(12), LW(13), LW(14), LW(15))
            ELSE
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_S_FLOAT_DOWN1"), LW(12), LW(13), LW(14), LW(15))
            END_IF
        END_IF
        SET_FLOAT_VALUE(LW(4), LW(2))
        SET_VALUE(LF(0), 0)
        LOOP(0)
            CALL(evt_mario_get_party, LW(5))
            IF_INT_EQUAL(LW(5), 0)
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("mobj_fltbl01"))
                    GOTO(1)
                END_IF
            ELSE
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("MOBJ_mobj_fltbl01"))
                    GOTO(1)
                END_IF
                IF_STR_EQUAL(LW(7), STRING("MOBJ_mobj_fltbl01"))
                    GOTO(1)
                END_IF
            END_IF
            GOTO(2)
            LABEL(1)
            IF_INT_EQUAL(LW(10), 1)
                SET_FLOAT_VALUE(LW(5), LW(2))
                SET_FLOAT_VALUE(LW(2), LW(3))
                SET_FLOAT_VALUE(LW(3), LW(5))
                SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
                SET_FLOAT_VALUE(LW(8), 0)
                SET_FLOAT_VALUE(LW(10), 0)
                IF_INT_EQUAL(LSWF(6), 0)
                    SET_VALUE(LSWF(6), 1)
                ELSE
                    SET_VALUE(LSWF(6), 0)
                END_IF
                CALL(evt_mobj_exec_cancel, STRING("mobj_fltsw01"))
            END_IF
            LABEL(2)
            ADD_FLOAT(LW(4), LW(9))
            ADD_FLOAT(LW(9), LW(8))
            IF_FLOAT_GREATER_OR_EQUAL(LW(9), FLOAT(4.0f))
                SET_FLOAT_VALUE(LW(9), FLOAT(4.0f))
            END_IF
            CALL(evt_mobj_get_position, STRING("mobj_fltbl01"), LW(11), LW(12), LW(13))
            CALL(evt_snd_sfx_pos, LW(15), LW(11), LW(12), LW(13))
            IF_INT_EQUAL(LW(10), 0)
                IF_INT_GREATER_OR_EQUAL(LW(4), LW(3))
                    SET_FLOAT_VALUE(LW(4), LW(3))
                    SET_VALUE(LF(0), 1)
                END_IF
            ELSE
                IF_INT_LESS_OR_EQUAL(LW(4), LW(3))
                    SET_FLOAT_VALUE(LW(4), LW(3))
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            CALL(evt_mobj_set_y_position, STRING("mobj_fltbl01"), LW(4))
            IF_INT_EQUAL(LF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(LF(1), 0)
            CALL(evt_snd_sfxoff, LW(15))
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_mobj_get_position, STRING("mobj_fltbl01"), LW(13), LW(14), LW(15))
            CALL(evt_mobj_get_kindname, STRING("mobj_fltbl01"), LW(12))
            SET_VALUE(LF(0), 0)
            IF_STR_EQUAL(LW(12), STRING("MOBJ_green_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(12), STRING("MOBJ_purple_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(12), STRING("MOBJ_orange_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LW(10), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_STOP1"), LW(13), LW(14), LW(15), 0)
            END_IF
        END_INLINE_EVENT
        IF_INT_EQUAL(LW(10), 1)
            BEGIN_SIBLING_EVENT
                CALL(evt_sub_rumble_onoff, 0, 0)
                WAIT_MS(300)
                CALL(evt_sub_rumble_onoff, 1, 0)
            END_SIBLING_EVENT
            BEGIN_SIBLING_EVENT
                CALL(evt_sub_intpl_init, 11, 10, 3, 10)
                LABEL(2)
                CALL(evt_sub_intpl_get_value)
                DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
                CALL(evt_cam_shake, 4, FLOAT(0.0f), LW(0), 100)
                IF_INT_EQUAL(LW(1), 1)
                    GOTO(2)
                END_IF
            END_SIBLING_EVENT
            WAIT_MS(1000)
        END_IF
        IF_INT_EQUAL(LSWF(6), 0)
            SET_VALUE(LSWF(6), 1)
        ELSE
            SET_VALUE(LSWF(6), 0)
        END_IF
    ELSE
        CALL(evt_mobj_exec_cancel, STRING("mobj_fltsw01"))
        SET_VALUE(LW(0), 0)
        LOOP(0)
            WAIT_FRAMES(2)
            SET_VALUE(LW(1), LW(0))
            MODULO(LW(1), 2)
            ADD(LW(0), 1)
            IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_gon_04flowblock_updown2_init)
    IF_INT_EQUAL(0, 0)
        IF_INT_NOT_EQUAL(LSWF(7), 0)
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl02"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(200.0f))
            CALL(evt_mobj_set_y_position, STRING("mobj_fltbl02"), LW(5))
        END_IF
    ELSE
        IF_INT_EQUAL(LSWF(7), 0)
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl02"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(200.0f))
            CALL(evt_mobj_set_y_position, STRING("mobj_fltbl02"), LW(5))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_gon_04flowblock_updown2)
    SET_TYPE(239)
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    IF_INT_NOT_EQUAL(GSWF(1485), 0)
        SET_VALUE(LW(10), 0)
        IF_INT_EQUAL(0, 0)
            IF_INT_EQUAL(LSWF(7), 0)
                SET_VALUE(LW(10), 0)
            ELSE
                SET_VALUE(LW(10), 1)
            END_IF
        ELSE
            IF_INT_EQUAL(LSWF(7), 0)
                SET_VALUE(LW(10), 1)
            ELSE
                SET_VALUE(LW(10), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl02"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            ADD_FLOAT(LW(3), FLOAT(200.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
            SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
        ELSE
            CALL(evt_mobj_get_y_position, STRING("mobj_fltbl02"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            SUBTRACT_FLOAT(LW(3), FLOAT(200.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(-0.2f))
            SET_FLOAT_VALUE(LW(9), 0)
        END_IF
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(10)
            CALL(evt_sub_rumble_onoff, 0, 0)
            WAIT_MS(300)
            CALL(evt_sub_rumble_onoff, 1, 0)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(10)
            CALL(evt_sub_intpl_init, 11, 10, 3, 4)
            LABEL(0)
            CALL(evt_sub_intpl_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            CALL(evt_cam_shake, 4, FLOAT(0.0f), LW(0), 100)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
        END_SIBLING_EVENT
        WAIT_MS(500)
        CALL(evt_mobj_get_position, STRING("mobj_fltbl02"), LW(12), LW(13), LW(14))
        CALL(evt_mobj_get_kindname, STRING("mobj_fltbl02"), LW(11))
        SET_VALUE(LF(0), 0)
        IF_STR_EQUAL(LW(11), STRING("MOBJ_green_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(11), STRING("MOBJ_purple_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(11), STRING("MOBJ_orange_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LF(1), 0)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_MOVE1"), LW(12), LW(13), LW(14), LW(15))
            ELSE
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_S_FLOAT_MOVE1"), LW(12), LW(13), LW(14), LW(15))
            END_IF
        ELSE
            SET_VALUE(LF(1), 1)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_DOWN1"), LW(12), LW(13), LW(14), LW(15))
            ELSE
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_S_FLOAT_DOWN1"), LW(12), LW(13), LW(14), LW(15))
            END_IF
        END_IF
        SET_FLOAT_VALUE(LW(4), LW(2))
        SET_VALUE(LF(0), 0)
        LOOP(0)
            CALL(evt_mario_get_party, LW(5))
            IF_INT_EQUAL(LW(5), 0)
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("mobj_fltbl02"))
                    GOTO(1)
                END_IF
            ELSE
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("MOBJ_mobj_fltbl02"))
                    GOTO(1)
                END_IF
                IF_STR_EQUAL(LW(7), STRING("MOBJ_mobj_fltbl02"))
                    GOTO(1)
                END_IF
            END_IF
            GOTO(2)
            LABEL(1)
            IF_INT_EQUAL(LW(10), 1)
                SET_FLOAT_VALUE(LW(5), LW(2))
                SET_FLOAT_VALUE(LW(2), LW(3))
                SET_FLOAT_VALUE(LW(3), LW(5))
                SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
                SET_FLOAT_VALUE(LW(8), 0)
                SET_FLOAT_VALUE(LW(10), 0)
                IF_INT_EQUAL(LSWF(7), 0)
                    SET_VALUE(LSWF(7), 1)
                ELSE
                    SET_VALUE(LSWF(7), 0)
                END_IF
                CALL(evt_mobj_exec_cancel, STRING("mobj_fltsw02"))
            END_IF
            LABEL(2)
            ADD_FLOAT(LW(4), LW(9))
            ADD_FLOAT(LW(9), LW(8))
            IF_FLOAT_GREATER_OR_EQUAL(LW(9), FLOAT(4.0f))
                SET_FLOAT_VALUE(LW(9), FLOAT(4.0f))
            END_IF
            CALL(evt_mobj_get_position, STRING("mobj_fltbl02"), LW(11), LW(12), LW(13))
            CALL(evt_snd_sfx_pos, LW(15), LW(11), LW(12), LW(13))
            IF_INT_EQUAL(LW(10), 0)
                IF_INT_GREATER_OR_EQUAL(LW(4), LW(3))
                    SET_FLOAT_VALUE(LW(4), LW(3))
                    SET_VALUE(LF(0), 1)
                END_IF
            ELSE
                IF_INT_LESS_OR_EQUAL(LW(4), LW(3))
                    SET_FLOAT_VALUE(LW(4), LW(3))
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            CALL(evt_mobj_set_y_position, STRING("mobj_fltbl02"), LW(4))
            IF_INT_EQUAL(LF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(LF(1), 0)
            CALL(evt_snd_sfxoff, LW(15))
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_mobj_get_position, STRING("mobj_fltbl02"), LW(13), LW(14), LW(15))
            CALL(evt_mobj_get_kindname, STRING("mobj_fltbl02"), LW(12))
            SET_VALUE(LF(0), 0)
            IF_STR_EQUAL(LW(12), STRING("MOBJ_green_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(12), STRING("MOBJ_purple_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(12), STRING("MOBJ_orange_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LW(10), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_STOP1"), LW(13), LW(14), LW(15), 0)
            END_IF
        END_INLINE_EVENT
        IF_INT_EQUAL(LW(10), 1)
            BEGIN_SIBLING_EVENT
                CALL(evt_sub_rumble_onoff, 0, 0)
                WAIT_MS(300)
                CALL(evt_sub_rumble_onoff, 1, 0)
            END_SIBLING_EVENT
            BEGIN_SIBLING_EVENT
                CALL(evt_sub_intpl_init, 11, 10, 3, 10)
                LABEL(2)
                CALL(evt_sub_intpl_get_value)
                DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
                CALL(evt_cam_shake, 4, FLOAT(0.0f), LW(0), 100)
                IF_INT_EQUAL(LW(1), 1)
                    GOTO(2)
                END_IF
            END_SIBLING_EVENT
            WAIT_MS(1000)
        END_IF
        IF_INT_EQUAL(LSWF(7), 0)
            SET_VALUE(LSWF(7), 1)
        ELSE
            SET_VALUE(LSWF(7), 0)
        END_IF
    ELSE
        CALL(evt_mobj_exec_cancel, STRING("mobj_fltsw02"))
        SET_VALUE(LW(0), 0)
        LOOP(0)
            WAIT_FRAMES(2)
            SET_VALUE(LW(1), LW(0))
            MODULO(LW(1), 2)
            ADD(LW(0), 1)
            IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

// TODO: type
u32 kemuri_pos1[10] = {
    0x00000096, 0x00000014, 0x00000082, 0x00000037,
    0x00000078, 0x00000064, 0x000000DC, 0x00000014,
    0x000000B9, 0x0000004B,
};

// TODO: type
u32 kemuri_pos2[10] = {
    0x0000013B, 0x0000004B, 0x0000010E, 0x0000001E,
    0x0000017C, 0x00000064, 0x00000172, 0x00000037,
    0x00000154, 0x00000014,
};

EVT_BEGIN(evt_kaidan_pera_anime)
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_gon_04_2"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_set_offscreen, 0, STRING("neta_02"), STRING("offscreen"))
        CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_2"))
        CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
        CALL(evt_img_alloc_capture, STRING("img"), 1, STRING("offscreen"), 0, 0, 0, 608, 480)
        CALL(evt_mapobj_flag_onoff, 0, 1, STRING("neta_01"), 1)
        CALL(evt_hitobj_onoff, STRING("a_neta_01"), 0, 0)
        CALL(evt_snd_sfxon, STRING("SFX_STG1_PERA_STEP1"), 0)
    ELSE
        CALL(evt_mapobj_set_offscreen, 0, STRING("neta_02"), STRING("offscreen"))
        CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
        CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
        CALL(evt_img_alloc_capture, STRING("img"), 1, STRING("offscreen"), 0, 0, 0, 608, 480)
        CALL(evt_mapobj_flag_onoff, 0, 1, STRING("neta_02"), 1)
        CALL(evt_hitobj_onoff, STRING("a_neta_02"), 0, 0)
        CALL(evt_snd_sfxon, STRING("SFX_STG1_PERA_STEP2"), 0)
    END_IF
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_snd_sfxon, STRING("SFX_STG1_QUAK_PERA1"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.016f), FLOAT(0.0f), 250)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("neta_02"), 1)
        CALL(evt_hitobj_onoff, STRING("a_neta_02"), 0, 1)
        SET_INT_READ_ADDR(PTR(kemuri_pos1))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("neta_01"), 1)
        CALL(evt_hitobj_onoff, STRING("a_neta_01"), 0, 1)
        SET_INT_READ_ADDR(PTR(kemuri_pos2))
    END_IF
    LOOP(5)
        READ_INT2(LW(0), LW(1))
        CALL(evt_sub_random, 10, LW(2))
        SUBTRACT(LW(2), 5)
        ADD(LW(2), LW(0))
        CALL(evt_sub_random, 20, LW(3))
        SUBTRACT(LW(3), 10)
        ADD(LW(3), LW(1))
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(2), LW(3), 60, FLOAT(1.6f), 0, 0, 0, 0, 0, 0, 0)
    END_LOOP
    CALL(evt_img_release, STRING("img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 0, STRING("neta_02"))
    RETURN
EVT_END()

EVT_BEGIN(evt_kaidan_pera_init)
    IF_INT_EQUAL(GSWF(1482), 0)
        CALL(evt_mapobj_flag_onoff, 0, 1, STRING("neta_02"), 1)
        CALL(evt_hitobj_onoff, STRING("a_neta_02"), 0, 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 0, 1, STRING("neta_01"), 1)
        CALL(evt_hitobj_onoff, STRING("a_neta_01"), 0, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_kaidan_pera)
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    CALL(evt_mario_key_onoff, 0)
    LOOP(0)
        IF_INT_EQUAL(GF(10), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_party_use_check, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        CONVERT_MS_TO_FRAME(LW(0), 1500)
        LOOP(LW(0))
            WAIT_FRAMES(1)
            CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
            CALL(evt_party_get_pos, 0, LW(13), LW(14), LW(15))
            IF_INT_GREATER_OR_EQUAL(LW(10), LW(13))
                SET_VALUE(LW(8), LW(10))
                SET_VALUE(LW(9), LW(13))
            ELSE
                SET_VALUE(LW(8), LW(13))
                SET_VALUE(LW(9), LW(10))
            END_IF
            SUBTRACT(LW(8), LW(9))
            IF_INT_LESS_OR_EQUAL(LW(8), 15)
                BREAK_LOOP
            END_IF
        END_LOOP
    ELSE
        WAIT_MS(300)
    END_IF
    IF_INT_LESS(GSW(0), 40)
        SET_VALUE(GSW(0), 40)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_map_get_fog, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
        CONVERT_MS_TO_FRAME(LW(10), 500)
        CALL(evt_sub_intpl_init, 11, 0, 250, LW(10))
        LOOP(0)
            CALL(evt_sub_intpl_get_value)
            SET_VALUE(LW(2), LW(5))
            ADD(LW(2), LW(0))
            CALL(evt_map_set_fog, LW(4), LW(2), LW(6), LW(7), LW(8), LW(9))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 250, 125, 650, 250, 80, -140, 450, 11)
    WAIT_MS(1500)
    SET_VALUE(LW(0), GSWF(1482))
    CALL_CHILD(evt_kaidan_pera_anime)
    WAIT_MS(1000)
    IF_INT_EQUAL(GSWF(1482), 0)
        SET_VALUE(GSWF(1482), 1)
    ELSE
        SET_VALUE(GSWF(1482), 0)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_map_get_fog, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
        CONVERT_MS_TO_FRAME(LW(10), 500)
        CALL(evt_sub_intpl_init, 11, 0, 250, LW(10))
        LOOP(0)
            CALL(evt_sub_intpl_get_value)
            SET_VALUE(LW(2), LW(5))
            SUBTRACT(LW(2), LW(0))
            CALL(evt_map_set_fog, LW(4), LW(2), LW(6), LW(7), LW(8), LW(9))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_off, 450, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_hashi_pera)
    CALL(evt_mario_key_onoff, 0)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(evt_mario_party_use_check, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_cam3d_evt_set, 280, 1000, 450, 280, 900, -140, 300, 11)
    ELSE
        CALL(evt_cam3d_evt_set, 0, 1000, 450, 0, 900, -140, 300, 11)
    END_IF
    WAIT_MS(500)
    CALL(evt_paper_entry, STRING("OFF_gon_04_3"))
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_gon_04_3"))
    CALL(evt_offscreen_entry, STRING("offscreen2"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_set_offscreen, 1, STRING("tuuro_02"), STRING("offscreen2"))
    ELSE
        CALL(evt_mapobj_set_offscreen, 1, STRING("tuuro_01"), STRING("offscreen2"))
    END_IF
    CALL(evt_offscreen_get_boundingbox, STRING("offscreen2"), LW(10), LW(11), LW(12), LW(13))
    ADD(LW(10), -40)
    ADD(LW(11), -100)
    ADD(LW(12), 80)
    ADD(LW(13), 100)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, 0, 1, LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 2, STRING("offscreen2"), 1, LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_clear_virtual_point, STRING("capture_img"))
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    WAIT_MS(500)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("tuuro_02"), 1)
        CALL(evt_hitobj_onoff, STRING("a_tuuro_02"), 0, 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("tuuro_01"), 1)
        CALL(evt_hitobj_onoff, STRING("a_tuuro_01"), 0, 1)
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_STG1_PERA2"), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(1900)
        CALL(evt_snd_sfxon, STRING("SFX_STG1_PERA2"), 0)
    END_INLINE_EVENT
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_clear_offscreen, 1, STRING("tuuro_02"))
    ELSE
        CALL(evt_mapobj_clear_offscreen, 1, STRING("tuuro_01"))
    END_IF
    CALL(evt_offscreen_delete, STRING("offscreen2"))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 300, 11)
    WAIT_MS(300)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_hashi_pera0)
    SET_VALUE(GF(10), 1)
    SET_VALUE(LW(0), 0)
    CALL_CHILD(evt_hashi_pera)
    SET_VALUE(GSW(0), 48)
    SET_VALUE(GF(10), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_hashi_pera1)
    SET_VALUE(GF(10), 1)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(evt_hashi_pera)
    SET_VALUE(GSW(0), 49)
    SET_VALUE(GF(10), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_hashi)
    IF_INT_LESS(GSW(0), 48)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("tuuro_02"), 1)
        CALL(evt_hitobj_onoff, STRING("a_tuuro_02"), 0, 0)
        CALL(evt_mobj_switch_blue, 0, STRING("mobj_hashi0"), 400, 900, -100, PTR(evt_hashi_pera0), GSWF(1483))
    END_IF
    IF_INT_LESS(GSW(0), 49)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("tuuro_01"), 1)
        CALL(evt_hitobj_onoff, STRING("a_tuuro_01"), 0, 0)
        CALL(evt_mobj_switch_blue, 0, STRING("mobj_hashi1"), -80, 900, -100, PTR(evt_hashi_pera1), GSWF(1484))
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x0000000D, 0xFFFFFFFF,
};

static EVT_BEGIN(evt_chk)
    CALL(evt_pouch_check_item, 13, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock)
    SET_VALUE(LW(0), STRING("e_bero_1"))
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(GSW(0), 42)
    RETURN
EVT_END()

static EVT_BEGIN(key_get)
    SET_VALUE(GSW(0), 41)
    RETURN
EVT_END()

EVT_BEGIN(gon_04_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_cam_road_reset)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_npc_entry, STRING("\"カロン00\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン00\""), -502, 0, -68)
    CALL(evt_npc_set_rotate, STRING("\"カロン00\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン00\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン00\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン00\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン00\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン00\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン00\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン01\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン01\""), -359, 0, 95)
    CALL(evt_npc_set_rotate, STRING("\"カロン01\""), 0, 180, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン01\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン01\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン01\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン01\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン01\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン01\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン02\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン02\""), -191, 0, -1)
    CALL(evt_npc_set_rotate, STRING("\"カロン02\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン02\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン02\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン02\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン02\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン02\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン02\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン03\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン03\""), 9, 0, 75)
    CALL(evt_npc_set_rotate, STRING("\"カロン03\""), 0, 180, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン03\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン03\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン03\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン03\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン03\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン03\""), 7)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_mobj_switch_red, 0, STRING("mobj1"), 250, 150, -120, PTR(evt_kaidan_pera), 0)
    SPAWN_EVENT(evt_kaidan_pera_init)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuyu_box01"), 1)
    CALL(evt_hitobj_onoff, STRING("a_fuyu_box01"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuyu_box02"), 1)
    CALL(evt_hitobj_onoff, STRING("a_fuyu_box02"), 1, 0)
    CALL(evt_mobj_float_blk, STRING("mobj_fltbl01"), FLOAT(582.5f), 150, FLOAT(-137.5f), 1, 0, 0, LSWF(6))
    CALL(evt_mobj_float_blk, STRING("mobj_fltbl02"), 395, 150, -125, 2, 1, 0, LSWF(7))
    CALL_CHILD(evt_gon_04flowblock_updown1_init)
    CALL_CHILD(evt_gon_04flowblock_updown2_init)
    CALL(evt_mobj_switch_float_blk, STRING("mobj_fltsw01"), 550, 75, 137, STRING("mobj_fltbl01"), 1, PTR(evt_gon_04flowblock_updown1), LSWF(6))
    CALL(evt_mobj_switch_float_blk, STRING("mobj_fltsw02"), 450, 75, 60, STRING("mobj_fltbl02"), 2, PTR(evt_gon_04flowblock_updown2), LSWF(7))
    IF_INT_LESS(GSW(0), 41)
        CALL(evt_item_entry, STRING("key00"), 13, -150, 150, -125, 16, -1, PTR(key_get))
    END_IF
    CALL(evt_item_entry, STRING("item00"), 125, 150, 400, -125, 16, GSWF(5604), 0)
    CALL(evt_item_entry, STRING("item01"), 125, 400, 900, -175, 16, GSWF(5605), 0)
    CALL(evt_mobj_lv_blk, STRING("mobj_blk00"), 395, 200, -125, 0, GSWF(1485), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("mobj_blk01"), 445, 150, -125, 0, GSWF(1486), STRING("MOBJ_Lv2BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("mobj_blk02"), FLOAT(545.0f), 150, -125, 0, GSWF(1487), STRING("MOBJ_Lv2BigBlock"))
    SPAWN_EVENT(evt_hashi)
    IF_INT_LESS(GSW(0), 42)
        CALL(evt_mobj_lock, STRING("mobj_lock00"), 13, 595, 160, -50, 270, PTR(evt_chk), PTR(evt_unlock), GSWF(1488))
        SET_VALUE(LW(0), STRING("e_bero_1"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 6, 550, 960, -125, 0, 0)
    CALL(evt_mobj_powerupblk, STRING("pblk1"), -250, 60, -130, 0, GSWF(5538), 0)
    CALL(evt_map_playanim, STRING("hokori"), 1, 0)
    CALL(evt_paper_entry, STRING("OFF_gon_04_2"))
    SET_VALUE(GF(10), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

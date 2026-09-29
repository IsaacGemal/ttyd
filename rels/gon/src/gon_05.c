#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(door01_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door01_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door02_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door02_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door03_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_03"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door03_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_03"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door04_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door04_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door04_2"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door04_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door04_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door04_2"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door05_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_05"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door05_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_05"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero_1", 1, 0xC, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_04", "e_bero_1", 5, 5, &door01_close, &door01_open},
    {"w_bero_2", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_04", "e_bero_2", 1, 1, 0, 0},
    {"w_bero_3", 1, 0xC, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_04", "e_bero_3", 5, 5, &door02_close, &door02_open},
    {"w_bero_4", 1, 0xC, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_09", "e_bero", 5, 5, &door03_close, &door03_open},
    {"w_bero_5", 1, 0xC, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_10", "e_bero", 5, 5, &door04_close, &door04_open},
    {"e_bero", 1, 0xC, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_06", "w_bero", 5, 5, &door05_close, &door05_open},
    {NULL},
};

static EVT_BEGIN(init_0)
    IF_INT_EQUAL(1, 0)
        IF_INT_NOT_EQUAL(LSWF(4), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(525.0f))
            CALL(evt_mobj_set_y_position, STRING("blk_0"), LW(5))
        END_IF
    ELSE
        IF_INT_EQUAL(LSWF(4), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(525.0f))
            CALL(evt_mobj_set_y_position, STRING("blk_0"), LW(5))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(main_0)
    SET_TYPE(239)
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    IF_INT_NOT_EQUAL(1, 0)
        SET_VALUE(LW(10), 0)
        IF_INT_EQUAL(1, 0)
            IF_INT_EQUAL(LSWF(4), 0)
                SET_VALUE(LW(10), 0)
            ELSE
                SET_VALUE(LW(10), 1)
            END_IF
        ELSE
            IF_INT_EQUAL(LSWF(4), 0)
                SET_VALUE(LW(10), 1)
            ELSE
                SET_VALUE(LW(10), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            ADD_FLOAT(LW(3), FLOAT(525.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
            SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
        ELSE
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            SUBTRACT_FLOAT(LW(3), FLOAT(525.0f))
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
        CALL(evt_mobj_get_position, STRING("blk_0"), LW(12), LW(13), LW(14))
        CALL(evt_mobj_get_kindname, STRING("blk_0"), LW(11))
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
                IF_STR_EQUAL(LW(6), STRING("blk_0"))
                    GOTO(1)
                END_IF
            ELSE
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("MOBJ_blk_0"))
                    GOTO(1)
                END_IF
                IF_STR_EQUAL(LW(7), STRING("MOBJ_blk_0"))
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
                IF_INT_EQUAL(LSWF(4), 0)
                    SET_VALUE(LSWF(4), 1)
                ELSE
                    SET_VALUE(LSWF(4), 0)
                END_IF
                CALL(evt_mobj_exec_cancel, STRING("sw_0"))
            END_IF
            LABEL(2)
            ADD_FLOAT(LW(4), LW(9))
            ADD_FLOAT(LW(9), LW(8))
            IF_FLOAT_GREATER_OR_EQUAL(LW(9), FLOAT(4.0f))
                SET_FLOAT_VALUE(LW(9), FLOAT(4.0f))
            END_IF
            CALL(evt_mobj_get_position, STRING("blk_0"), LW(11), LW(12), LW(13))
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
            CALL(evt_mobj_set_y_position, STRING("blk_0"), LW(4))
            IF_INT_EQUAL(LF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(LF(1), 0)
            CALL(evt_snd_sfxoff, LW(15))
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_mobj_get_position, STRING("blk_0"), LW(13), LW(14), LW(15))
            CALL(evt_mobj_get_kindname, STRING("blk_0"), LW(12))
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
        IF_INT_EQUAL(LSWF(4), 0)
            SET_VALUE(LSWF(4), 1)
        ELSE
            SET_VALUE(LSWF(4), 0)
        END_IF
    ELSE
        CALL(evt_mobj_exec_cancel, STRING("sw_0"))
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

EVT_BEGIN(init_1)
    IF_INT_EQUAL(0, 0)
        IF_INT_NOT_EQUAL(LSWF(5), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_1"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(489.0f))
            CALL(evt_mobj_set_y_position, STRING("blk_1"), LW(5))
        END_IF
    ELSE
        IF_INT_EQUAL(LSWF(5), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_1"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(489.0f))
            CALL(evt_mobj_set_y_position, STRING("blk_1"), LW(5))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(main_1)
    SET_TYPE(239)
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    IF_INT_NOT_EQUAL(1, 0)
        SET_VALUE(LW(10), 0)
        IF_INT_EQUAL(0, 0)
            IF_INT_EQUAL(LSWF(5), 0)
                SET_VALUE(LW(10), 0)
            ELSE
                SET_VALUE(LW(10), 1)
            END_IF
        ELSE
            IF_INT_EQUAL(LSWF(5), 0)
                SET_VALUE(LW(10), 1)
            ELSE
                SET_VALUE(LW(10), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_1"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            ADD_FLOAT(LW(3), FLOAT(489.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
            SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
        ELSE
            CALL(evt_mobj_get_y_position, STRING("blk_1"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            SUBTRACT_FLOAT(LW(3), FLOAT(489.0f))
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
        CALL(evt_mobj_get_position, STRING("blk_1"), LW(12), LW(13), LW(14))
        CALL(evt_mobj_get_kindname, STRING("blk_1"), LW(11))
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
                IF_STR_EQUAL(LW(6), STRING("blk_1"))
                    GOTO(1)
                END_IF
            ELSE
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("MOBJ_blk_1"))
                    GOTO(1)
                END_IF
                IF_STR_EQUAL(LW(7), STRING("MOBJ_blk_1"))
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
                IF_INT_EQUAL(LSWF(5), 0)
                    SET_VALUE(LSWF(5), 1)
                ELSE
                    SET_VALUE(LSWF(5), 0)
                END_IF
                CALL(evt_mobj_exec_cancel, STRING("sw_1"))
            END_IF
            LABEL(2)
            ADD_FLOAT(LW(4), LW(9))
            ADD_FLOAT(LW(9), LW(8))
            IF_FLOAT_GREATER_OR_EQUAL(LW(9), FLOAT(4.0f))
                SET_FLOAT_VALUE(LW(9), FLOAT(4.0f))
            END_IF
            CALL(evt_mobj_get_position, STRING("blk_1"), LW(11), LW(12), LW(13))
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
            CALL(evt_mobj_set_y_position, STRING("blk_1"), LW(4))
            IF_INT_EQUAL(LF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(LF(1), 0)
            CALL(evt_snd_sfxoff, LW(15))
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_mobj_get_position, STRING("blk_1"), LW(13), LW(14), LW(15))
            CALL(evt_mobj_get_kindname, STRING("blk_1"), LW(12))
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
        IF_INT_EQUAL(LSWF(5), 0)
            SET_VALUE(LSWF(5), 1)
        ELSE
            SET_VALUE(LSWF(5), 0)
        END_IF
    ELSE
        CALL(evt_mobj_exec_cancel, STRING("sw_1"))
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
u32 key_tbl00[2] = {
    0x0000000C, 0xFFFFFFFF,
};

EVT_BEGIN(evt_chk00)
    CALL(evt_pouch_check_item, 12, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl00), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock00)
    SET_VALUE(LW(0), STRING("w_bero_3"))
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(GSW(0), 47)
    RETURN
EVT_END()

// TODO: type
u32 key_tbl01[2] = {
    0x0000000E, 0xFFFFFFFF,
};

EVT_BEGIN(evt_chk01)
    CALL(evt_pouch_check_item, 14, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl01), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock01)
    SET_VALUE(LW(0), STRING("w_bero_5"))
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(GSW(0), 54)
    RETURN
EVT_END()

static EVT_BEGIN(key_get)
    SET_VALUE(GSW(0), 53)
    RETURN
EVT_END()

EVT_BEGIN(gon_05_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_npc_entry, STRING("\"カロン00\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン00\""), -203, 0, 150)
    CALL(evt_npc_set_rotate, STRING("\"カロン00\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン00\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン00\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン00\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン00\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン00\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン00\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン01\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン01\""), -120, 0, 51)
    CALL(evt_npc_set_rotate, STRING("\"カロン01\""), 0, 180, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン01\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン01\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン01\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン01\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン01\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン01\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン02\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン02\""), 25, 0, 119)
    CALL(evt_npc_set_rotate, STRING("\"カロン02\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン02\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン02\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン02\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン02\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン02\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン02\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン03\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン03\""), 87, 0, -129)
    CALL(evt_npc_set_rotate, STRING("\"カロン03\""), 0, 180, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン03\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン03\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン03\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン03\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン03\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン03\""), 7)
    CALL(evt_mobj_float_blk, STRING("blk_0"), 120, 815, 150, 2, 2, 0, LSWF(4))
    CALL_CHILD(init_0)
    CALL(evt_mobj_switch_float_blk, STRING("sw_0"), 20, 1399, 150, STRING("blk_0"), 2, PTR(main_0), LSWF(4))
    CALL(evt_mobj_flag_onoff, 1, STRING("blk_0"), 128)
    CALL(evt_mobj_float_blk, STRING("blk_1"), 200, 0, -150, 0, 2, 0, LSWF(5))
    CALL_CHILD(init_1)
    CALL(evt_mobj_switch_float_blk, STRING("sw_1"), FLOAT(-2.5f), 100, -175, STRING("blk_1"), 0, PTR(main_1), LSWF(5))
    CALL(evt_mobj_flag_onoff, 1, STRING("blk_1"), 128)
    IF_INT_LESS(GSW(0), 53)
        CALL(evt_item_entry, STRING("key00"), 14, 200, 1340, 150, 16, -1, PTR(key_get))
    END_IF
    CALL(evt_item_entry, STRING("item00"), 125, -200, 240, -150, 16, GSWF(5606), 0)
    CALL(evt_item_entry, STRING("item01"), 273, 210, 915, 150, 16, GSWF(1503), 0)
    IF_INT_LESS(GSW(0), 47)
        CALL(evt_mobj_lock, STRING("mobj_lock00"), 12, -240, 750, 0, 90, PTR(evt_chk00), PTR(evt_unlock00), GSWF(1489))
        SET_VALUE(LW(0), STRING("w_bero_3"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    IF_INT_LESS(GSW(0), 54)
        CALL(evt_mobj_lock, STRING("mobj_lock01"), 14, -240, 1900, 0, 90, PTR(evt_chk01), PTR(evt_unlock01), GSWF(1490))
        SET_VALUE(LW(0), STRING("w_bero_5"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 0, 60, 0, 0, 0)
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -200, 1950, -150, 0, GSWF(5539), 0)
    CALL(evt_map_playanim, STRING("hokori"), 1, 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

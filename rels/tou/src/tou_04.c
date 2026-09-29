#include <stdio.h>
#include <string.h>
#include "tou_04.h"
#include "driver/msgdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_tou_get_ranking_27_text_1ED4[];
static USER_FUNC(make_list);

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

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_w"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_w"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_3_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_3_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_4_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_4"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_4_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_4"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_5_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_5"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_5_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_5"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_6_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_6"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_6_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_6"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_7_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_7"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(_7_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_7"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_e"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_e"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[] = {
    {"w_bero", 1, 4, 0x4, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_02", "n_bero_4", 5, 5, w_door_close, w_door_open},
    {"s_bero_1", 1, 4, 0x2, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_11", "n_bero", 5, 5, _1_door_close, _1_door_open},
    {"s_bero_2", 1, 4, 0x2, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_09", "n_bero", 5, 5, _2_door_close, _2_door_open},
    {"s_bero_3", 1, 4, 0x0, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_07", "n_bero", 5, 5, _3_door_close, _3_door_open},
    {"s_bero_4", 1, 4, 0x0, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_06", "n_bero", 5, 5, _4_door_close, _4_door_open},
    {"s_bero_5", 1, 4, 0x0, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_05", "n_bero", 5, 5, _5_door_close, _5_door_open},
    {"s_bero_6", 1, 4, 0x6, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_08", "n_bero", 5, 5, _6_door_close, _6_door_open},
    {"s_bero_7", 1, 4, 0x6, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_10", "n_bero", 5, 5, _7_door_close, _7_door_open},
    {"e_bero", 1, 4, 0x4, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_02", "n_bero_3", 5, 5, e_door_close, e_door_open},
    {"n_bero_1", 1, 6, 0x6, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_03", "e_bero", 5, 5, n1_door_close, n1_door_open},
    {"n_bero_2", 1, 6, 0x2, 0x186A0, 0, 0, -1, NULL, -1, NULL, "tou_03", "w_bero", 5, 5, n2_door_close, n2_door_open},
    {NULL},
};

EVT_BEGIN(move_g)
    SET_VALUE(LF(0), 0)
    SWITCH(LW(0))
        CASE_EQUAL(3)
        SET_VALUE(LF(0), 1)
        CASE_EQUAL(4)
        SET_VALUE(LF(0), 1)
        CASE_EQUAL(5)
        SET_VALUE(LF(0), 1)
    END_SWITCH
    CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(10), -520)
        SET_VALUE(LW(11), 0)
        SET_VALUE(LW(12), 0)
        GOTO(1)
    END_IF
    LABEL(10)
    LABEL(0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_sub_get_dist, LW(0), LW(2), LW(10), LW(12), LW(6))
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    IF_INT_GREATER(LW(6), 100)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
        SET_VALUE(LW(6), LW(5))
        SET_VALUE(LW(7), LW(5))
        ADD(LW(6), 50)
        ADD(LW(7), -50)
        CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(6), LW(8))
        CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(7), LW(9))
        IF_INT_LESS(LW(8), LW(9))
            CALL(evt_npc_move_position, STRING("me"), LW(3), LW(7), 0, FLOAT(100.0f), 1)
        ELSE
            CALL(evt_npc_move_position, STRING("me"), LW(3), LW(6), 0, FLOAT(100.0f), 1)
        END_IF
    ELSE
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
        SET_VALUE(LW(6), LW(3))
        SET_VALUE(LW(7), LW(3))
        ADD(LW(6), 50)
        ADD(LW(7), -50)
        CALL(evt_sub_get_dist, LW(0), LW(2), LW(6), LW(5), LW(8))
        CALL(evt_sub_get_dist, LW(0), LW(2), LW(7), LW(5), LW(9))
        IF_INT_LESS(LW(8), LW(9))
            CALL(evt_npc_move_position, STRING("me"), LW(7), LW(5), 0, FLOAT(100.0f), 1)
        ELSE
            CALL(evt_npc_move_position, STRING("me"), LW(6), LW(5), 0, FLOAT(100.0f), 1)
        END_IF
    END_IF
    LABEL(1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_sub_get_dist, LW(0), LW(2), LW(10), LW(12), LW(6))
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    IF_INT_LESS(LW(6), 100)
        WAIT_FRAMES(1)
        GOTO(1)
    END_IF
    CALL(evt_npc_move_position, STRING("me"), LW(10), LW(12), 0, FLOAT(100.0f), 1)
    GOTO(10)
    RETURN
EVT_END()

EVT_BEGIN(move_g_1_dokimasen)
    CALL(evt_npc_set_position, STRING("me"), -580, 0, 350)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_move_position, STRING("me"), -580, 300, 0, FLOAT(80.0f), 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(init_g_1)
    IF_INT_EQUAL(GSWF(2379), 0)
        CALL(evt_npc_set_position, STRING("me"), -580, 0, 300)
    ELSE
        CALL(evt_npc_set_position, STRING("me"), -620, 0, 300)
    END_IF
    IF_INT_EQUAL(GSW(0), 141)
        CALL(evt_npc_set_position, STRING("me"), -580, 0, 350)
    END_IF
    IF_INT_EQUAL(GSW(0), 142)
        CALL(evt_npc_set_position, STRING("me"), -580, 0, 350)
    END_IF
    IF_INT_EQUAL(GSWF(2517), 0)
        IF_INT_EQUAL(GSW(0), 143)
            CALL(evt_bero_get_entername, LW(0))
            IF_STR_EQUAL(LW(0), STRING("s_bero_7"))
                SET_VALUE(GSWF(2517), 1)
                SPAWN_EVENT(move_g_1_dokimasen)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(move_g_1_dokimasu)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
    IF_INT_LESS(LW(2), LW(12))
        ADD(LW(12), 50)
    ELSE
        ADD(LW(12), -50)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
    CALL(evt_npc_move_position, STRING("me"), LW(10), LW(12), 0, FLOAT(80.0f), 1)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
    SET_VALUE(LW(0), STRING("s_bero_7"))
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(GSW(0), 141)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_1)
    SWITCH(GSW(0))
        CASE_LESS(133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_39"), 0, 0)
        CASE_BETWEEN(140, 142)
        CALL(evt_msg_print, 0, STRING("stg3_tou_297_13"), 0, 0)
        IF_INT_LESS(GSW(0), 141)
            CALL_CHILD(move_g_1_dokimasu)
        END_IF
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_96_01"), 0, 0)
        CASE_LESS(172)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_96"), 0, 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_95"), 0, 0)
        END_IF
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_39"), 0, 0)
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), 10)
                CALL(evt_msg_print, 0, STRING("stg3_tou_96"), 0, 0)
            ELSE
                CALL(evt_msg_print, 0, STRING("stg3_tou_95"), 0, 0)
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_1)
    IF_INT_EQUAL(GSWF(2379), 1)
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(131)
        IF_INT_EQUAL(GSWF(2376), 1)
            SET_VALUE(LW(0), 0)
            CALL_CHILD(move_g)
        END_IF
        CASE_LESS(172)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        IF_INT_GREATER(LW(0), 10)
            SET_VALUE(LW(0), 0)
            CALL_CHILD(move_g)
        END_IF
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 1)
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            IF_INT_GREATER(LW(0), 10)
                SET_VALUE(LW(0), 0)
                CALL_CHILD(move_g)
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_g_2)
    IF_INT_EQUAL(GSW(0), 163)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("n_bero_2"))
            CALL(evt_npc_set_position, STRING("me"), -520, 0, 70)
            RETURN
        END_IF
    END_IF
    CALL(evt_npc_set_position, STRING("me"), -520, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_2)
    SWITCH(GSW(0))
        CASE_LESS(133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_38"), 0, 0)
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_94_01"), 0, 0)
        CASE_EQUAL(157)
        IF_INT_EQUAL(GSWF(2399), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_423_01"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_I_Y"))
            CALL(evt_msg_print, 0, STRING("stg3_tou_423_02"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_94"), 0, 0)
        END_IF
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_tou_94"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_2)
    IF_INT_EQUAL(GSWF(2379), 1)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(move_g)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2380), 1)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(move_g)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2382), 1)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(move_g)
        RETURN
    END_IF
    IF_INT_EQUAL(GSW(0), 157)
        IF_INT_EQUAL(GSWF(2399), 0)
            SET_VALUE(LW(0), 1)
            CALL_CHILD(move_g)
            RETURN
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 163)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(move_g)
        RETURN
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(init_g_3)
    IF_INT_EQUAL(GSWF(2380), 1)
        CALL(evt_npc_set_position, STRING("me"), -620, 0, -300)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2381), 1)
        CALL(evt_npc_set_position, STRING("me"), -620, 0, -300)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("me"), -580, 0, -300)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_3)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    SWITCH(LW(0))
        CASE_LESS(1)
        CALL(evt_msg_print, 0, STRING("stg3_tou_99"), 0, 0)
        CASE_LESS(11)
        CALL(evt_msg_print, 0, STRING("stg3_tou_98"), 0, 0)
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_99_01"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_tou_40"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_3)
    IF_INT_EQUAL(GSWF(2380), 1)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2381), 1)
        RETURN
    END_IF
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    SWITCH(LW(0))
        CASE_LESS(1)
        IF_INT_EQUAL(GSWF(2378), 1)
            SET_VALUE(LW(0), 2)
            CALL_CHILD(move_g)
        END_IF
        CASE_LESS(11)
        SET_VALUE(LW(0), 2)
        CALL_CHILD(move_g)
        CASE_DEFAULT
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_4_dokimasen)
    CALL(evt_npc_set_position, STRING("me"), -300, 0, -530)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_move_position, STRING("me"), -350, -530, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_set_position, STRING("me"), -350, 0, -530)
    SET_VALUE(LW(0), 5)
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(init_g_4)
    IF_INT_LESS(GSW(0), 172)
        CALL(evt_npc_set_position, STRING("me"), -350, 0, -530)
    ELSE
        SET_VALUE(LF(0), 0)
        SET_VALUE(LF(1), 0)
        SET_VALUE(LF(2), 0)
        SWITCH(GSW(1420))
            CASE_EQUAL(13)
            SET_VALUE(LF(0), 1)
            CASE_EQUAL(23)
            SET_VALUE(LF(0), 1)
        END_SWITCH
        IF_INT_EQUAL(LF(0), 1)
            SET_VALUE(LF(1), GSWF(2499))
        ELSE
            IF_INT_EQUAL(GSWF(2500), 0)
                SET_VALUE(LF(1), GSWF(2498))
            ELSE
                CALL(evt_bero_get_entername, LW(0))
                IF_STR_EQUAL(LW(0), STRING("s_bero_5"))
                    SET_VALUE(LF(1), 1)
                    SET_VALUE(LF(2), 1)
                ELSE
                    SET_VALUE(LF(1), 0)
                END_IF
            END_IF
        END_IF
        IF_INT_EQUAL(LF(1), 0)
            CALL(evt_npc_set_position, STRING("me"), -350, 0, -530)
        ELSE
            CALL(evt_npc_set_position, STRING("me"), -300, 0, -530)
            IF_INT_EQUAL(LF(2), 1)
                SPAWN_EVENT(move_g_4_dokimasen)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

USER_FUNC(memoryRY) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");

    *(f32*)npc->field_148 = npc->direction;
    return 2;
}

EVT_BEGIN(talk_g_4)
    SWITCH(GSW(0))
        CASE_LESS(130)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
        IF_INT_EQUAL(LW(3), -350)
            CALL(evt_msg_print, 0, STRING("stg3_tou_42"), 0, 0)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
            CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
            IF_INT_LESS(LW(0), LW(3))
                ADD(LW(3), 50)
                CALL(evt_npc_move_position, STRING("me"), LW(3), LW(5), 0, FLOAT(80.0f), 1)
                CALL(evt_npc_set_ry, STRING("me"), 270)
            ELSE
                ADD(LW(3), -50)
                CALL(evt_npc_move_position, STRING("me"), LW(3), LW(5), 0, FLOAT(80.0f), 1)
                CALL(evt_npc_set_ry, STRING("me"), 90)
            END_IF
            CALL(memoryRY)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
            SET_VALUE(LW(0), 5)
            CALL_CHILD(bero_case_switch_on)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_42_01"), 0, 0)
        END_IF
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_42_01"), 0, 0)
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_102_02"), 0, 0)
        CASE_LESS(172)
        CALL(evt_msg_print, 0, STRING("stg3_tou_102_01"), 0, 0)
        CASE_DEFAULT
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        IF_INT_EQUAL(LW(0), -350)
            SWITCH(GSW(1420))
                CASE_EQUAL(13)
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_311"), 0, 0)
                SET_VALUE(GSWF(2499), 1)
                CASE_EQUAL(23)
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_311"), 0, 0)
                SET_VALUE(GSWF(2499), 1)
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2500), 0)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_re_310"), 0, 0)
                    SET_VALUE(GSWF(2498), 1)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_tou_re_314"), 0, 0)
                    RETURN
                END_IF
            END_SWITCH
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
            CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
            IF_INT_LESS(LW(0), LW(3))
                ADD(LW(3), 50)
                CALL(evt_npc_move_position, STRING("me"), LW(3), LW(5), 0, FLOAT(80.0f), 1)
                CALL(evt_npc_set_ry, STRING("me"), 270)
            ELSE
                ADD(LW(3), -50)
                CALL(evt_npc_move_position, STRING("me"), LW(3), LW(5), 0, FLOAT(80.0f), 1)
                CALL(evt_npc_set_ry, STRING("me"), 90)
            END_IF
            CALL(memoryRY)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
            SET_VALUE(LW(0), 5)
            CALL_CHILD(bero_case_switch_on)
        ELSE
            SWITCH(GSW(1420))
                CASE_EQUAL(13)
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_313"), 0, 0)
                CASE_EQUAL(23)
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_313"), 0, 0)
                CASE_DEFAULT
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_312"), 0, 0)
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_4)
    IF_INT_EQUAL(GSWF(2375), 1)
        SET_VALUE(LW(0), 3)
        CALL_CHILD(move_g)
    END_IF
    IF_INT_EQUAL(GSWF(2376), 1)
        SET_VALUE(LW(0), 3)
        CALL_CHILD(move_g)
    END_IF
    IF_INT_EQUAL(GSWF(2377), 1)
        SET_VALUE(LW(0), 3)
        CALL_CHILD(move_g)
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(130)
        CASE_LESS(133)
        SET_VALUE(LW(0), 3)
        CALL_CHILD(move_g)
        CASE_EQUAL(162)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("s_bero_5"))
            CALL(evt_npc_set_position, STRING("me"), -400, 0, -530)
            CALL(evt_bero_exec_wait, 65536)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_npc_move_position, STRING("me"), -350, -530, 0, FLOAT(80.0f), 1)
            CALL(evt_mario_key_onoff, 1)
        END_IF
        CASE_BETWEEN(163, 171)
        SET_VALUE(LW(0), 3)
        CALL_CHILD(move_g)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_g_5)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_5)
    RETURN
EVT_END()

EVT_BEGIN(move_g_5)
    RETURN
EVT_END()

EVT_BEGIN(init_g_6)
    IF_INT_EQUAL(GSWF(2382), 1)
        CALL(evt_npc_set_position, STRING("me"), 350, 0, -560)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("me"), 350, 0, -530)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_6)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    SWITCH(LW(0))
        CASE_LESS(1)
        CALL(evt_msg_print, 0, STRING("stg3_tou_101"), 0, 0)
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_102_00"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_tou_102"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_6)
    IF_INT_EQUAL(GSWF(2382), 1)
        RETURN
    END_IF
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    SWITCH(LW(0))
        CASE_LESS(1)
        SET_VALUE(LW(0), 5)
        CALL_CHILD(move_g)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2375), 1)
            SET_VALUE(LW(0), 5)
            CALL_CHILD(move_g)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_g_7)
    CALL(evt_npc_set_position, STRING("me"), 580, 0, -300)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_7)
    SWITCH(GSW(0))
        CASE_LESS(133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_40"), 0, 0)
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_100_01"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_tou_100"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_7)
    RETURN
EVT_END()

EVT_BEGIN(init_g_8)
    CALL(evt_npc_set_position, STRING("me"), 520, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_8)
    SWITCH(GSW(0))
        CASE_LESS(133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_38"), 0, 0)
        CASE_EQUAL(157)
        IF_INT_EQUAL(GSWF(2399), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_423_03"), 0, 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_94"), 0, 0)
        END_IF
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_tou_94"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_8)
    RETURN
EVT_END()

EVT_BEGIN(init_g_9)
    CALL(evt_npc_set_position, STRING("me"), 580, 0, 300)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_9)
    SWITCH(GSW(0))
        CASE_LESS(133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_39"), 0, 0)
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_97_01"), 0, 0)
        CASE_LESS(172)
        CALL(evt_msg_print, 0, STRING("stg3_tou_97"), 0, 0)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_39"), 0, 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_97"), 0, 0)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(move_g_9)
    IF_INT_EQUAL(GSWF(2381), 1)
        SET_VALUE(LW(0), 8)
        CALL_CHILD(move_g)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(init_g_10)
    SWITCH(GSW(0))
        CASE_EQUAL(157)
        IF_INT_EQUAL(GSWF(2399), 0)
            CALL(evt_npc_set_position, STRING("me"), -550, 0, 530)
        END_IF
        CASE_EQUAL(163)
        CALL(evt_npc_set_position, STRING("me"), -550, 0, 530)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_g_10)
    SWITCH(GSW(0))
        CASE_EQUAL(157)
        IF_INT_EQUAL(GSWF(2399), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_102_03"), 0, STRING("me"))
        END_IF
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_102_04"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_g_11)
    SWITCH(GSW(0))
        CASE_EQUAL(157)
        IF_INT_EQUAL(GSWF(2399), 0)
            CALL(evt_npc_set_position, STRING("me"), 550, 0, 530)
        END_IF
        CASE_EQUAL(163)
        CALL(evt_npc_set_position, STRING("me"), 550, 0, 530)
    END_SWITCH
    CALL(evt_npc_set_ry, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(talk_g_11)
    SWITCH(GSW(0))
        CASE_EQUAL(157)
        IF_INT_EQUAL(GSWF(2399), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_102_03"), 0, STRING("me"))
        END_IF
        CASE_EQUAL(163)
        CALL(evt_msg_print, 0, STRING("stg3_tou_102_04"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_champ)
    RETURN
EVT_END()

static EVT_BEGIN(init_brots)
    IF_INT_LESS(GSW(0), 172)
        IF_INT_EQUAL(GSWF(2421), 0)
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_LESS(5)
                CASE_LESS(19)
                CALL(evt_sub_random, 4, LW(1))
                SWITCH(LW(1))
                    CASE_EQUAL(0)
                    CALL(evt_npc_set_position, STRING("me"), -525, 0, 225)
                    CASE_EQUAL(1)
                    CALL(evt_npc_set_position, STRING("me"), -175, 0, -475)
                    CASE_EQUAL(2)
                    CALL(evt_npc_set_position, STRING("me"), 525, 0, -175)
                    CASE_DEFAULT
                    CALL(evt_npc_set_position, STRING("me"), 525, 0, 425)
                END_SWITCH
            END_SWITCH
        END_IF
    END_IF
    RETURN
EVT_END()

static USER_FUNC(make_list) {
    static char* tmp;
    static const char* list[7] = {
        "stg3_tou_hushigi_list_1", "stg3_tou_hushigi_list_2", "stg3_tou_hushigi_list_3", "stg3_tou_hushigi_list_4",
        "stg3_tou_hushigi_list_5", "stg3_tou_hushigi_list_6", "stg3_tou_hushigi_list_7",
    };
    s32* args = event->args;
    s32 i;

    if (tmp == NULL) {
        tmp = _mapAlloc(0x1000);
    }
    sprintf(tmp, "<select %d %d %d %d>", 0, 7, 300, 140);
    for (i = 0; i < 7; i++) {
        strcat(tmp, "\n");
        strcat(tmp, msgSearch(list[i]));
    }
    evtSetValue(event, args[0], (s32)tmp);
    return 2;
}

static EVT_BEGIN(talk_brots)
    IF_INT_EQUAL(GSWF(2444), 0)
        CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_00"), 0, 0)
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(700)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_01"), 0, 0)
        CALL(evt_mario_set_pose, STRING("M_N_B"))
        WAIT_MS(700)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_02"), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_03"), 0, 0)
    END_IF
    CALL(evt_msg_select, 0, STRING("stg3_tou_hushigi_02_yn"))
    IF_INT_EQUAL(LW(0), 1)
        IF_INT_EQUAL(GSWF(2444), 0)
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_hushigi_06"))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_hushigi_07"))
        END_IF
    ELSE
        CALL(evt_msg_print_add, 0, STRING("stg3_tou_hushigi_05"))
        CALL(make_list, LW(0))
        CALL(evt_msg_select, 1, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_11"), 0, 0)
            CASE_EQUAL(1)
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_12"), 0, 0)
            CASE_EQUAL(2)
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_13"), 0, 0)
            CASE_EQUAL(3)
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_14"), 0, 0)
            CASE_EQUAL(4)
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_15"), 0, 0)
            CASE_EQUAL(5)
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_16"), 0, 0)
            CASE_EQUAL(6)
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_17"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_hushigi_08"), 0, 0)
        END_SWITCH
    END_IF
    SET_VALUE(GSWF(2444), 1)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"キノシコワ", 0x40400620},
    {"ガンス", 0x40400620},
    {"チャンピョン", 0x40400620, 0, init_champ},
    {"ブロッツ", 0x40400600, 0, init_brots, NULL, talk_brots},
    {"ガードマン１", 0x40400600, 0, init_g_1, move_g_1, talk_g_1},
    {"ガードマン２", 0x40400600, 0, init_g_2, move_g_2, talk_g_2},
    {"ガードマン３", 0x40400600, 0, init_g_3, move_g_3, talk_g_3},
    {"ガードマン４", 0x40400600, 0, init_g_4, move_g_4, talk_g_4},
    {"ガードマン５", 0x40400600, 0, init_g_5, move_g_5, talk_g_5},
    {"ガードマン６", 0x40400600, 0, init_g_6, move_g_6, talk_g_6},
    {"ガードマン７", 0x40400600, 0, init_g_7, move_g_7, talk_g_7},
    {"ガードマン８", 0x40400600, 0, init_g_8, move_g_8, talk_g_8},
    {"ガードマン９", 0x40400600, 0, init_g_9, move_g_9, talk_g_9},
    {"ガードマン１０", 0x40400600, 0, init_g_10, NULL, talk_g_10},
    {"ガードマン１１", 0x40400600, 0, init_g_11, NULL, talk_g_11},
    {NULL},
};

EVT_BEGIN(evt_drop)
    LOOP(0)
        CALL(evt_sub_random, 5000, LW(0))
        ADD(LW(0), 1000)
        WAIT_MS(LW(0))
        CALL(evt_map_playanim, STRING("anm_mizu"), 0, 0)
        CALL(evt_map_checkanim, STRING("anm_mizu"), LW(0), LW(1))
        WAIT_MS(LW(1))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AMB_WATERDROP1"), 500, 0, -405, 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_gans)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_set_position, STRING("ガンス"), -350, 0, -570)
    CALL(evt_mario_set_pos, -350, 0, -580)
    CALL(evt_party_set_pos, 0, -350, 0, -600)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 5)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_5_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        SET_VALUE(LW(1), 5)
        SPAWN_EVENT(bero_close_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL_CHILD(_5_door_open)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_mov_pos2, -350, -500, FLOAT(90.0f))
        CALL(evt_mario_mov_pos2, 310, -500, FLOAT(90.0f))
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("ガンス"), 2)
    CALL(evt_npc_move_position, STRING("ガンス"), -350, -500, 0, FLOAT(90.0f), 1)
    CALL(evt_npc_move_position, STRING("ガンス"), 320, -500, 0, FLOAT(90.0f), 1)
    CALL(evt_npc_move_position, STRING("ガンス"), 350, -530, 0, FLOAT(90.0f), 1)
    SET_VALUE(LW(1), 3)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_3_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_bero_mapchange, STRING("tou_07"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_kinoshikowa)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_set_position, STRING("キノシコワ"), -350, 0, -570)
    CALL(evt_mario_set_pos, -350, 0, -580)
    CALL(evt_party_set_pos, 0, -350, 0, -590)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 5)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_5_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        SET_VALUE(LW(1), 5)
        SPAWN_EVENT(bero_close_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL_CHILD(_5_door_open)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_mov_pos2, -350, -505, FLOAT(75.0f))
        CALL(evt_mario_mov_pos2, -550, -505, FLOAT(75.0f))
        CALL(evt_mario_mov_pos2, -550, 250, FLOAT(75.0f))
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_msg_print, 0, STRING("stg3_tou_62"), 0, 0)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -350, -470, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -550, -470, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -550, 270, 0, FLOAT(75.0f), 1)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_move_position, STRING("キノシコワ"), -585, 290, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
    WAIT_MS(600)
    SET_VALUE(LW(1), 7)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_7_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GSWF(2376), 0)
    CALL(evt_bero_mapchange, STRING("tou_10"), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_kinoshikowa2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_set_position, STRING("キノシコワ"), -350, 0, -570)
    CALL(evt_mario_set_pos, -350, 0, -580)
    CALL(evt_party_set_pos, 0, -350, 0, -590)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 5)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_5_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        SET_VALUE(LW(1), 5)
        SPAWN_EVENT(bero_close_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL_CHILD(_5_door_open)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_mov_pos2, -350, -505, FLOAT(75.0f))
        CALL(evt_mario_mov_pos2, -550, -505, FLOAT(75.0f))
        CALL(evt_mario_mov_pos2, -550, -350, FLOAT(75.0f))
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -350, -470, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -550, -470, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -550, -333, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -585, -290, 0, FLOAT(75.0f), 1)
    SET_VALUE(LW(1), 6)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_6_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GSWF(2377), 0)
    CALL(evt_bero_mapchange, STRING("tou_08"), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_kinoshikowa3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pos, -625, 0, -300)
    CALL(evt_npc_set_position, STRING("キノシコワ"), -620, 0, -300)
    CALL(evt_npc_set_ry, STRING("キノシコワ"), 270)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 6)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_6_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        SET_VALUE(LW(1), 6)
        SPAWN_EVENT(bero_close_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL_CHILD(_6_door_open)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_mov_pos2, -550, -301, FLOAT(75.0f))
        CALL(evt_mario_mov_pos2, -550, -500, FLOAT(75.0f))
        CALL(evt_mario_mov_pos2, 300, -500, FLOAT(75.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        CALL(evt_party_move_pos2, 0, -550, -301, FLOAT(75.0f))
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -550, -301, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -550, -500, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 340, -500, 0, FLOAT(75.0f), 1)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 340, -530, 0, FLOAT(75.0f), 1)
    SET_VALUE(LW(1), 3)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_3_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GSWF(2378), 0)
    CALL(evt_bero_mapchange, STRING("tou_07"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_gardman)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 7)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_7_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ガードマン１"), 2)
        CALL(evt_npc_move_position, STRING("ガードマン１"), -550, 300, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_move_position, STRING("ガードマン１"), -550, -35, 0, FLOAT(60.0f), 1)
        CALL(evt_set_dir_to_target, STRING("ガードマン１"), STRING("mario"))
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -550, 300, FLOAT(60.0f))
        CALL(evt_mario_mov_pos2, -550, 0, FLOAT(60.0f))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_party_move_pos2, 0, -550, 300, FLOAT(60.0f))
    SET_VALUE(LW(1), 7)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_7_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_76"), 0, STRING("ガードマン１"))
    SET_VALUE(LW(1), 10)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n2_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -400, 0, FLOAT(60.0f))
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(GSWF(2379), 0)
    SWITCH(GSW(0))
        CASE_LESS(132)
        SET_VALUE(GSW(0), 132)
        CASE_DEFAULT
    END_SWITCH
    CALL(evt_bero_mapchange, STRING("tou_03"), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_gardman2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 6)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_6_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ガードマン３"), 2)
        CALL(evt_npc_move_position, STRING("ガードマン３"), -550, -300, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_move_position, STRING("ガードマン３"), -550, 35, 0, FLOAT(60.0f), 1)
        CALL(evt_set_dir_to_target, STRING("ガードマン３"), STRING("mario"))
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -550, -300, FLOAT(60.0f))
        CALL(evt_mario_mov_pos2, -550, 0, FLOAT(60.0f))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_party_move_pos2, 0, -550, -300, FLOAT(60.0f))
    SET_VALUE(LW(1), 6)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_6_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_328"), 0, STRING("ガードマン３"))
    SET_VALUE(LW(1), 10)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n2_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -400, 0, FLOAT(60.0f))
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(GSWF(2380), 0)
    CALL(evt_bero_mapchange, STRING("tou_03"), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_gardman3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 6)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_6_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ガードマン３"), 2)
        CALL(evt_npc_move_position, STRING("ガードマン３"), -550, -300, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_move_position, STRING("ガードマン３"), -550, -400, 0, FLOAT(60.0f), 1)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -550, -300, FLOAT(60.0f))
        CALL(evt_mario_set_dir_npc, STRING("ガードマン３"))
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_party_move_pos2, 0, -550, -300, FLOAT(60.0f))
    CALL(evt_party_move_pos2, 0, -550, -270, FLOAT(60.0f))
    CALL(evt_party_set_dir_npc, 0, STRING("ガードマン３"))
    SET_VALUE(LW(1), 6)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_6_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_407"))
    WAIT_MS(300)
    CALL(evt_party_run, 0)
    CALL(evt_set_dir_to_target, STRING("ガードマン３"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_408"), 0, STRING("ガードマン３"))
    WAIT_MS(300)
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン４"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン５"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン６"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン７"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン８"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ガードマン９"), 32)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_sub_get_dist, LW(0), LW(2), 0, -500, LW(10))
            IF_INT_LESS(LW(10), 100)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print_party, STRING("stg3_tou_408_01"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ガードマン３"), -550, -520, 0, FLOAT(90.0f), 1)
        CALL(evt_npc_move_position, STRING("ガードマン３"), 550, -520, 0, FLOAT(90.0f), 1)
        CALL(evt_npc_move_position, STRING("ガードマン３"), 570, 280, 0, FLOAT(90.0f), 1)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -550, -515, FLOAT(90.0f))
    CALL(evt_mario_mov_pos2, 550, -515, FLOAT(90.0f))
    CALL(evt_mario_mov_pos2, 570, 250, FLOAT(90.0f))
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(LW(1), 1)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_1_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ガードマン３"), 590, 300, 0, FLOAT(60.0f), 1)
        CALL(evt_mario_mov_pos2, 590, 300, FLOAT(90.0f))
    END_INLINE_EVENT
    SET_VALUE(GSWF(2381), 0)
    CALL(evt_bero_mapchange, STRING("tou_11"), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_gardman4)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 3)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_3_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ガードマン６"), 2)
        CALL(evt_npc_move_position, STRING("ガードマン６"), 350, -510, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_move_position, STRING("ガードマン６"), -550, -510, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_move_position, STRING("ガードマン６"), -550, 35, 0, FLOAT(60.0f), 1)
        CALL(evt_set_dir_to_target, STRING("ガードマン６"), STRING("mario"))
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 350, -510, FLOAT(60.0f))
        CALL(evt_mario_mov_pos2, -550, -510, FLOAT(60.0f))
        CALL(evt_mario_mov_pos2, -550, 0, FLOAT(60.0f))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_party_move_pos2, 0, 350, -510, FLOAT(60.0f))
    SET_VALUE(LW(1), 3)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(_3_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_454"), 0, STRING("ガードマン６"))
    SET_VALUE(LW(1), 10)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n2_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -400, 0, FLOAT(60.0f))
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(GSWF(2382), 0)
    CALL(evt_bero_mapchange, STRING("tou_03"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_toile)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 450, 133, -1033, 450, 37, -492, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_DRAINING1"), 450, 0, -500, LW(3))
    BEGIN_INLINE_EVENT_GET_ID(LW(4))
        CALL(evt_sub_intpl_msec_init, 0, 450, 450, 5000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_snd_sfx_pos, LW(3), LW(0), 0, -500)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, -450, 133, -1033, -450, 37, -492, 5000, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(333)
        CALL(evt_eff_fukidashi, 1, STRING("a"), STRING("ガードマン６"), 1, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(4000)
        CALL(evt_eff_fukidashi, 1, STRING("b"), STRING("ガードマン４"), 1, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    WAIT_MS(5000)
    DELETE_EVENT(LW(4))
    CALL(evt_snd_sfxoff, LW(3))
    CALL(evt_bero_mapchange, STRING("tou_08"), STRING("dokan"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_toile2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -450, 133, -1033, -450, 37, -492, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_DRAINING1"), -450, 0, -500, LW(3))
    BEGIN_INLINE_EVENT_GET_ID(LW(4))
        CALL(evt_sub_intpl_msec_init, 0, -450, 450, 5000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_snd_sfx_pos, LW(3), LW(0), 0, -500)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, 450, 133, -1033, 450, 37, -492, 5000, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(333)
        CALL(evt_eff_fukidashi, 1, STRING("a"), STRING("ガードマン４"), 1, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(4000)
        CALL(evt_eff_fukidashi, 1, STRING("b"), STRING("ガードマン６"), 1, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    WAIT_MS(5000)
    DELETE_EVENT(LW(4))
    CALL(evt_snd_sfxoff, LW(3))
    CALL(evt_bero_mapchange, STRING("tou_09"), STRING("dokan"))
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x00000012, 0xFFFFFFFF,
};

static EVT_BEGIN(evt_chk)
    CALL(evt_pouch_check_item, 18, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_lock)
    SET_VALUE(GSW(0), 147)
    SET_VALUE(LW(0), STRING("s_bero_4"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_pouch_remove_item, 18, LW(0))
    RETURN
EVT_END()

EVT_BEGIN(evt_souko_talk)
    IF_INT_EQUAL(GSWF(2445), 1)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("キノシコワ"), 30, 0, -500)
    CALL(evt_npc_set_position, STRING("ガードマン５"), -30, 0, -500)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("ガードマン５"))
    CALL(evt_set_dir_to_target, STRING("ガードマン５"), STRING("キノシコワ"))
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_status_onoff, 1, STRING("ガードマン５"), 2)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER(LW(0), -140)
            IF_INT_LESS(LW(0), 140)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_yoshi_ride, 0, 0)
    CALL(evt_cam3d_evt_set, 0, 133, -1033, 0, 47, -490, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_240_02"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_240_03"), 0, STRING("ガードマン５"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_240_04"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(0), 0)
        CALL(evt_npc_move_position, STRING("ガードマン５"), 200, -490, 0, FLOAT(80.0f), 1)
    ELSE
        CALL(evt_npc_move_position, STRING("ガードマン５"), -200, -490, 0, FLOAT(80.0f), 1)
    END_IF
    CALL(evt_npc_set_position, STRING("ガードマン５"), 0, -1000, 0)
    CALL(evt_npc_reverse_ry, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("mario"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("キノシコワ"), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(0), 0)
        SET_VALUE(LW(0), -40)
    ELSE
        SET_VALUE(LW(0), 40)
    END_IF
    CALL(evt_cam3d_evt_set, LW(0), 133, -1033, LW(0), 47, -490, 1000, 11)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(0), 0)
        ADD(LW(0), 30)
    ELSE
        SUBTRACT(LW(0), 30)
    END_IF
    CALL(evt_npc_move_position, STRING("キノシコワ"), LW(0), -500, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_240_05"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(0), 0)
        CALL(evt_npc_move_position, STRING("キノシコワ"), 200, -500, 0, FLOAT(60.0f), 1)
    ELSE
        CALL(evt_npc_move_position, STRING("キノシコワ"), -200, -500, 0, FLOAT(60.0f), 1)
    END_IF
    CALL(evt_npc_set_position, STRING("キノシコワ"), 0, -1000, 0)
    SET_VALUE(GSWF(2445), 1)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_bred_nigeru)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_status_onoff, 1, STRING("チャンピョン"), 2)
    CALL(evt_npc_set_position, STRING("チャンピョン"), -550, 0, -400)
    CALL(evt_set_dir_to_target, STRING("チャンピョン"), STRING("mario"))
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_eff_fukidashi, 1, 0, STRING("チャンピョン"), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_297_27_00"), 0, STRING("チャンピョン"))
    WAIT_MS(300)
    CALL(evt_npc_sound_data_set, STRING("チャンピョン"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, 20000, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(evt_npc_move_position, STRING("チャンピョン"), -550, -500, 0, FLOAT(200.0f), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("チャンピョン"), 0, -500, 0, FLOAT(200.0f), 1)
        CALL(evt_npc_set_position, STRING("チャンピョン"), 0, -1000, 0)
    END_INLINE_EVENT
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_297_27_01"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG3_TOU1"), 2000)
    SET_VALUE(GSW(0), 145)
    RETURN
EVT_END()

EVT_BEGIN(boss_msg)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_462_01"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(out_from_tou_10_re)
    CALL(evt_npc_set_position, STRING("ガードマン１"), -590, 0, 350)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_npc_move_position, STRING("ガードマン１"), -590, 300, 0, FLOAT(80.0f), 1)
    RETURN
EVT_END()

USER_FUNC(kemuri_stop) {
    return 2;
}

EVT_BEGIN(tou_04_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_cam_road_reset)
    SPAWN_EVENT(evt_drop)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_item_entry, STRING("item_01"), 272, 515, 15, 125, 16, GSWF(2520), 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save1"), -520, 60, -110, 0, 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save3"), 450, 60, -465, 0, 0)
    SET_VALUE(LW(1), 0)
    IF_INT_EQUAL(GSWF(2375), 1)
        CALL(kemuri_stop)
        CALL(evt_mario_set_pos, -350, 0, -580)
        CALL(evt_party_set_pos, 0, -350, 0, -600)
        SET_VALUE(LW(1), 1)
        SPAWN_EVENT(evt_gans)
    END_IF
    IF_INT_EQUAL(GSWF(2376), 1)
        CALL(kemuri_stop)
        CALL(evt_mario_set_pos, -390, 0, -510)
        CALL(evt_party_set_pos, 0, -390, 0, -510)
        SPAWN_EVENT(evt_kinoshikowa)
    END_IF
    IF_INT_EQUAL(GSWF(2377), 1)
        CALL(kemuri_stop)
        CALL(evt_mario_set_pos, -390, 0, -510)
        CALL(evt_party_set_pos, 0, -390, 0, -510)
        SPAWN_EVENT(evt_kinoshikowa2)
    END_IF
    IF_INT_EQUAL(GSWF(2378), 1)
        CALL(kemuri_stop)
        CALL(evt_mario_set_pos, -625, 0, -300)
        CALL(evt_party_set_pos, 0, -630, 0, -300)
        SPAWN_EVENT(evt_kinoshikowa3)
    END_IF
    IF_INT_EQUAL(GSW(0), 163)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
    ELSE
        IF_INT_EQUAL(LW(1), 0)
            SWITCH(GSW(0))
                CASE_BETWEEN(127, 137)
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU4"))
                CALL(evt_snd_bgmoff, 26624)
                CASE_GREATER_OR_EQUAL(165)
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU4"))
                CALL(evt_snd_bgmoff, 26624)
                CASE_DEFAULT
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
                CALL(evt_snd_bgmon, 160, 0)
                CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU4"))
            END_SWITCH
        ELSE
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG3_GANCE1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU4"))
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(2383), 1)
        SET_VALUE(GSWF(2379), 0)
        SET_VALUE(GSWF(2380), 0)
        SET_VALUE(GSWF(2381), 1)
        SET_VALUE(GSWF(2382), 0)
    END_IF
    IF_INT_EQUAL(GSWF(2384), 1)
        SET_VALUE(GSWF(2379), 0)
        SET_VALUE(GSWF(2380), 0)
        SET_VALUE(GSWF(2381), 0)
        SET_VALUE(GSWF(2382), 0)
    END_IF
    IF_INT_EQUAL(GSWF(2385), 1)
        SET_VALUE(GSWF(2379), 0)
        SET_VALUE(GSWF(2380), 0)
        SET_VALUE(GSWF(2381), 0)
        SET_VALUE(GSWF(2382), 0)
    END_IF
    IF_INT_EQUAL(GSWF(2379), 1)
        CALL(evt_mario_set_pos, -625, 0, 300)
        CALL(evt_party_set_pos, 0, -640, 0, 300)
        SPAWN_EVENT(evt_gardman)
    END_IF
    IF_INT_EQUAL(GSWF(2380), 1)
        CALL(evt_mario_set_pos, -625, 0, -300)
        CALL(evt_party_set_pos, 0, -640, 0, -300)
        CALL(evt_party_set_pos, 1, 0, -1000, 0)
        SPAWN_EVENT(evt_gardman2)
    END_IF
    IF_INT_EQUAL(GSWF(2381), 1)
        CALL(evt_mario_set_pos, -625, 0, -300)
        CALL(evt_party_set_pos, 0, -640, 0, -300)
        CALL(evt_party_set_pos, 1, 0, -1000, 0)
        CALL(evt_mario_set_dir, 270, 0, 0)
        CALL(evt_party_set_dir, 0, 270, 0)
        SPAWN_EVENT(evt_gardman3)
    END_IF
    IF_INT_EQUAL(GSWF(2382), 1)
        CALL(evt_mario_set_pos, 350, 0, -580)
        CALL(evt_party_set_pos, 0, 350, 0, -600)
        CALL(evt_party_set_pos, 1, 0, -1000, 0)
        SPAWN_EVENT(evt_gardman4)
    END_IF
    IF_INT_EQUAL(GSWF(2384), 1)
        CALL(evt_mario_set_pos, 530, 0, 530)
        CALL(evt_party_set_pos, 0, 530, 0, 530)
        CALL(evt_cam_road_reset)
        SPAWN_EVENT(evt_toile)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2385), 1)
        CALL(evt_mario_set_pos, 530, 0, 530)
        CALL(evt_party_set_pos, 0, 530, 0, 530)
        CALL(evt_cam_road_reset)
        SPAWN_EVENT(evt_toile2)
        RETURN
    END_IF
    SET_VALUE(LW(2), 0)
    LOOP(11)
        SET_VALUE(LW(0), LW(2))
        CALL_CHILD(bero_case_switch_off)
        ADD(LW(2), 1)
    END_LOOP
    SWITCH(GSW(0))
        CASE_LESS(130)
        CASE_LESS(131)
        SET_VALUE(LW(0), 5)
        CALL_CHILD(bero_case_switch_on)
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_LESS(1)
            SET_VALUE(LW(0), 3)
            CALL_CHILD(bero_case_switch_on)
            CASE_LESS(11)
            SET_VALUE(LW(0), 6)
            CALL_CHILD(bero_case_switch_on)
            CASE_DEFAULT
            SET_VALUE(LW(0), 7)
            CALL_CHILD(bero_case_switch_on)
        END_SWITCH
    END_SWITCH
    SET_VALUE(LW(0), 0)
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(LW(0), 8)
    CALL_CHILD(bero_case_switch_on)
    IF_INT_EQUAL(GSW(0), 141)
        SET_VALUE(LW(0), STRING("s_bero_7"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    IF_INT_EQUAL(GSW(0), 142)
        SET_VALUE(LW(0), STRING("s_bero_7"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    IF_INT_EQUAL(GSW(0), 157)
        IF_INT_EQUAL(GSWF(2399), 0)
            SET_VALUE(LW(0), 10)
            CALL_CHILD(bero_case_switch_on)
            SET_VALUE(LW(0), 0)
            CALL_CHILD(bero_case_switch_off)
            SET_VALUE(LW(0), 8)
            CALL_CHILD(bero_case_switch_off)
            CALL(evt_run_case_evt, 9, 1, STRING("w_bero"), 0, PTR(boss_msg), 0)
            CALL(evt_run_case_evt, 9, 1, STRING("e_bero"), 0, PTR(boss_msg), 0)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 163)
        SET_VALUE(LW(0), 10)
        CALL_CHILD(bero_case_switch_on)
        SET_VALUE(LW(0), 0)
        CALL_CHILD(bero_case_switch_off)
        SET_VALUE(LW(0), 8)
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_run_case_evt, 9, 1, STRING("w_bero"), 0, PTR(boss_msg), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("e_bero"), 0, PTR(boss_msg), 0)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 163)
        IF_INT_EQUAL(GSWF(2496), 0)
            SET_VALUE(LW(0), 5)
            CALL_CHILD(bero_case_switch_on)
        ELSE
            IF_INT_EQUAL(GSWF(2499), 1)
                SET_VALUE(LW(0), 5)
                CALL_CHILD(bero_case_switch_on)
            END_IF
            IF_INT_EQUAL(GSWF(2498), 1)
                IF_INT_EQUAL(GSWF(2500), 0)
                    SET_VALUE(LW(0), 5)
                    CALL_CHILD(bero_case_switch_on)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(evt_cam_road_reset)
    IF_INT_EQUAL(GSWF(2450), 1)
        IF_INT_EQUAL(GSWF(2451), 0)
            CALL(evt_mario_kill_party, 0)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(2388), 1)
        CALL(evt_party_set_pos, 1, 0, -1000, 0)
        CALL(evt_party_stop, 1)
        CALL(evt_mario_clear_party, 1)
    END_IF
    IF_INT_LESS(GSW(0), 147)
        CALL(evt_mobj_lock, STRING("lock"), 18, 0, 10, -550, 0, PTR(evt_chk), PTR(evt_lock), GSWF(2436))
        SET_VALUE(LW(0), STRING("s_bero_4"))
        CALL_CHILD(bero_case_switch_off)
    ELSE
        SET_VALUE(LW(0), STRING("s_bero_4"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    IF_INT_NOT_EQUAL(GSWF(2377), 1)
        IF_INT_NOT_EQUAL(GSWF(2378), 1)
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), 11)
                IF_INT_LESS(GSW(0), 147)
                    SPAWN_EVENT(evt_souko_talk)
                END_IF
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 144)
        SPAWN_EVENT(evt_bred_nigeru)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 172)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("s_bero_7"))
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(1))
            IF_INT_EQUAL(LW(1), 10)
                SPAWN_EVENT(out_from_tou_10_re)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

// clang-format on

#include "rsh.h"
#include "battle_database_rsh.h"
#include "data/mapdata.h"
#include "driver/camdrv.h"
#include "driver/imgdrv.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "driver/swdrv.h"
#include "dolphin/gx.h"
#include "dolphin/mtx.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "party/party_motion.h"
#include "rsh_00.h"
#include "rsh_02.h"
#include "rsh_03.h"
#include "rsh_04.h"
#include "rsh_05.h"
#include "rsh_06.h"
#include <stdio.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
extern s32 npc_urouro_init_event[];
extern s32 npc_urouro_move_event[];
extern s32 npc_urouro_return_event[];
s32 evt_rsh_door_open[548];
s32 evt_rsh_door_close[404];
s32 evt_rsh_rnd_npc_2_1_talk[8];
s32 evt_rsh_rnd_npc_2_2_talk[16];
s32 evt_rsh_rnd_npc_2_3_talk[16];
s32 evt_rsh_rnd_npc_2_4_talk[8];
s32 evt_rsh_rnd_npc_2_5_talk[8];
s32 evt_rsh_rnd_npc_2_6_talk[8];
s32 evt_rsh_rnd_npc_2_7_talk[8];
s32 evt_rsh_rnd_npc_2_8_talk[8];
s32 evt_rsh_rnd_npc_2_9_talk[8];
s32 evt_rsh_rnd_npc_2_10_talk[8];
s32 evt_rsh_rnd_npc_3_1_talk[8];
s32 evt_rsh_rnd_npc_3_2_talk[8];
s32 evt_rsh_rnd_npc_3_3_talk[8];
s32 evt_rsh_rnd_npc_3_4_talk[8];
s32 evt_rsh_rnd_npc_3_5_talk[25];
s32 evt_rsh_rnd_npc_3_6_talk[8];
s32 evt_rsh_rnd_npc_3_7_talk[8];
s32 evt_rsh_rnd_npc_3_8_talk[8];
s32 evt_rsh_rnd_npc_3_9_talk[8];
s32 evt_rsh_rnd_npc_3_10_talk[8];
s32 evt_rsh_rnd_npc_4_1_talk[8];
s32 evt_rsh_rnd_npc_4_2_talk[8];
s32 evt_rsh_rnd_npc_4_3_talk[16];
s32 evt_rsh_rnd_npc_4_4_talk[8];
s32 evt_rsh_rnd_npc_4_5_talk[8];
s32 evt_rsh_rnd_npc_4_6_talk[8];
s32 evt_rsh_rnd_npc_4_7_talk[8];
s32 evt_rsh_rnd_npc_4_8_talk[39];
s32 evt_rsh_rnd_npc_4_9_talk[8];
s32 evt_rsh_rnd_npc_4_10_talk[8];
s32 evt_rsh_rnd_npc_5_1_talk[8];
s32 evt_rsh_rnd_npc_5_2_talk[8];
s32 evt_rsh_rnd_npc_5_3_talk[47];
s32 evt_rsh_rnd_npc_5_4_talk[8];
s32 evt_rsh_rnd_npc_5_5_talk[8];
s32 evt_rsh_rnd_npc_5_6_talk[8];
s32 evt_rsh_rnd_npc_5_7_talk[8];
s32 evt_rsh_rnd_npc_5_8_talk[8];
s32 evt_rsh_rnd_npc_5_9_talk[8];
s32 evt_rsh_rnd_npc_5_10_talk[8];

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

EVT_BEGIN(simi_check)
    SET_VALUE(LF(0), 0)
    CALL(evt_sub_get_mapname, LW(10))
    IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_03_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        SWITCH(GSW(0))
            CASE_BETWEEN(283, 302)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_yuka"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_yuka_simi"), 1)
            CASE_DEFAULT
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_yuka"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_yuka_simi"), 1)
        END_SWITCH
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(simi_offscreen_check)
    SET_VALUE(LF(0), 0)
    CALL(evt_sub_get_mapname, LW(10))
    IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_03_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        SWITCH(GSW(0))
            CASE_BETWEEN(283, 302)
            CALL(evt_mapobj_clear_offscreen, 0, STRING("s_yuka"))
            CASE_DEFAULT
            CALL(evt_mapobj_clear_offscreen, 0, STRING("s_yuka_simi"))
        END_SWITCH
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kami_check)
    CALL(evt_sub_get_mapname, LW(10))
    IF_STR_EQUAL(LW(10), STRING("rsh_01_a"))
        IF_INT_EQUAL(GSW(0), 304)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yuka_3_kami"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_3"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_3_kami"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yuka_3"), 1)
        END_IF
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
        SWITCH(GSW(0))
            CASE_BETWEEN(306, 307)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yuka_5_kami"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_5_kami_2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_5"), 1)
            CASE_DEFAULT
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_5_kami"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_5_kami_2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yuka_5"), 1)
        END_SWITCH
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kami_offscreen_check)
    CALL(evt_sub_get_mapname, LW(10))
    IF_STR_EQUAL(LW(10), STRING("rsh_01_a"))
        IF_INT_EQUAL(GSW(0), 304)
            CALL(evt_mapobj_clear_offscreen, 0, STRING("yuka_3"))
        ELSE
            CALL(evt_mapobj_clear_offscreen, 0, STRING("yuka_3_kami"))
        END_IF
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
        SWITCH(GSW(0))
            CASE_BETWEEN(306, 307)
            CALL(evt_mapobj_clear_offscreen, 0, STRING("yuka_5"))
            CASE_DEFAULT
            CALL(evt_mapobj_clear_offscreen, 0, STRING("yuka_5_kami"))
        END_SWITCH
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_door_init)
    CALL(evt_paper_entry, STRING("OFF_d_meku"))
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_meku"))
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_door_entry)
    SET_INT_READ_ADDR(LW(0))
    READ_INT(LW(1))
    READ_INT(LW(2))
    READ_INT(LW(3))
    READ_INT(LW(4))
    READ_INT(LW(5))
    READ_INT(LW(6))
    READ_INT(LW(7))
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(2), 1)
    CALL(evt_run_case_evt, 9, 1, LW(4), 0, PTR(evt_rsh_door_open), 0)
    CALL(evt_run_case_evt, 9, 1, LW(6), 0, PTR(evt_rsh_door_close), 0)
    RETURN
EVT_END()


EVT_BEGIN(evt_rsh_door_open)
    SET_INT_READ_ADDR(LW(0))
    READ_INT(LW(1))
    READ_INT(LW(2))
    READ_INT(LW(3))
    READ_INT(LW(4))
    READ_INT(LW(5))
    READ_INT(LW(6))
    READ_INT(LW(7))
    CALL(evt_hitobj_attr_onoff, 0, 1, LW(4), 8388608)
    CALL(evt_hitobj_attr_onoff, 0, 1, LW(6), 8388608)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(img_wait)
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(2), 1)
    CALL_CHILD(kami_check)
    CALL(evt_hit_get_position, LW(4), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_map_playanim, LW(5), 0, 0)
    CALL(evt_map_checkanim, LW(5), LW(10), LW(11))
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(3), 1)
    CALL(evt_hit_get_position, LW(4), LW(10), LW(11), LW(12))
    SUBTRACT(LW(12), 50)
    CALL(evt_mario_party_door_move, LW(10), LW(12), FLOAT(80.0f))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_free_capture, STRING("capture_img"), 2)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 1, 0, 0, 0, 0)
    CALL(evt_mapobj_set_offscreen, 1, STRING("rouka"), STRING("offscreen"))
    CALL_CHILD(simi_offscreen_check)
    CALL(evt_mapobj_set_offscreen, 1, STRING("w_tobira"), STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("e_tobira"), STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("rouka"), 1)
    CALL(evt_npc_check, STRING("車掌"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_offscreen, STRING("車掌"), STRING("offscreen"))
        CALL(evt_npc_flag_onoff, 1, STRING("車掌"), 128)
    END_IF
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF6_ROOM_IN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_clear_offscreen, 1, STRING("rouka"))
        CALL(evt_mapobj_clear_offscreen, 1, STRING("w_tobira"))
        CALL(evt_mapobj_clear_offscreen, 1, STRING("e_tobira"))
        CALL(evt_npc_check, STRING("車掌"), LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_npc_set_offscreen, STRING("車掌"), 0)
        END_IF
        CALL(evt_img_wait_animend, STRING("capture_img"))
        CALL(evt_img_onoff, STRING("capture_img"), 0)
    END_INLINE_EVENT
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_hitobj_attr_onoff, 0, 0, LW(4), 8388608)
    CALL(evt_hitobj_attr_onoff, 0, 0, LW(6), 8388608)
    SET_VALUE(LF(0), 0)
    CALL(evt_sub_get_mapname, LW(10))
    IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_02_b"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_02_c"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        IF_STR_EQUAL(LW(1), STRING("heya_5"))
            SWITCH(GSW(0))
                CASE_EQUAL(301)
                CALL(evt_mario_key_onoff, 0)
                CALL(evt_img_wait_animend, STRING("capture_img"))
                CALL(evt_party_stop, 0)
                WAIT_MS(300)
                CALL(evt_mario_get_party, LW(0))
                SWITCH(LW(0))
                    CASE_EQUAL(1)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_146_0"), 0, STRING("party"))
                    CASE_EQUAL(2)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_146_1"), 0, STRING("party"))
                    CASE_EQUAL(4)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_146_2"), 0, STRING("party"))
                    CASE_EQUAL(5)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_146_3"), 0, STRING("party"))
                    CASE_EQUAL(6)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_146_4"), 0, STRING("party"))
                    CASE_EQUAL(3)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_146_5"), 0, STRING("party"))
                    CASE_EQUAL(7)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_146_6"), 0, STRING("party"))
                END_SWITCH
                CALL(evt_party_run, 0)
                CALL(evt_mario_key_onoff, 1)
                CASE_BETWEEN(306, 307)
                CALL(evt_snd_bgmoff, 18432)
                CASE_EQUAL(326)
                CALL(evt_mario_key_onoff, 0)
                CALL(evt_img_wait_animend, STRING("capture_img"))
                CALL(evt_party_stop, 0)
                WAIT_MS(300)
                CALL(evt_mario_get_party, LW(0))
                SWITCH(LW(0))
                    CASE_EQUAL(1)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_224_0"), 0, STRING("party"))
                    CASE_EQUAL(2)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_224_1"), 0, STRING("party"))
                    CASE_EQUAL(4)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_224_2"), 0, STRING("party"))
                    CASE_EQUAL(5)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_224_3"), 0, STRING("party"))
                    CASE_EQUAL(6)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_224_4"), 0, STRING("party"))
                    CASE_EQUAL(3)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_224_5"), 0, STRING("party"))
                    CASE_EQUAL(7)
                    CALL(evt_msg_print, 0, STRING("stg6_rsh_224_6"), 0, STRING("party"))
                END_SWITCH
                CALL(evt_party_run, 0)
                CALL(evt_mario_key_onoff, 1)
                CASE_DEFAULT
                CALL(evt_mario_key_onoff, 0)
                CALL(evt_img_wait_animend, STRING("capture_img"))
                CALL(evt_mario_key_onoff, 1)
            END_SWITCH
        END_IF
    END_IF
    SET_VALUE(LF(0), 0)
    CALL(evt_sub_get_mapname, LW(10))
    IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_02_b"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_02_c"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        IF_STR_EQUAL(LW(1), STRING("heya_4"))
            IF_INT_EQUAL(GSWF(3434), 1)
                SET_VALUE(GSWF(3435), 1)
            END_IF
            IF_INT_EQUAL(GSWF(3445), 1)
                CALL(evt_snd_bgmoff, 2048)
            END_IF
            SET_VALUE(GSWF(3459), 1)
        END_IF
    END_IF
    SET_VALUE(LF(0), 0)
    CALL(evt_sub_get_mapname, LW(10))
    IF_STR_EQUAL(LW(10), STRING("rsh_04_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_04_b"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(10), STRING("rsh_04_c"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        IF_STR_EQUAL(LW(1), STRING("heya_5"))
            SET_VALUE(GSWF(3446), 1)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("heya_3"))
            SET_VALUE(GSWF(3447), 1)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_door_close)
    SET_INT_READ_ADDR(LW(0))
    READ_INT(LW(1))
    READ_INT(LW(2))
    READ_INT(LW(3))
    READ_INT(LW(4))
    READ_INT(LW(5))
    READ_INT(LW(6))
    READ_INT(LW(7))
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_hitobj_attr_onoff, 0, 1, LW(4), 8388608)
    CALL(evt_hitobj_attr_onoff, 0, 1, LW(6), 8388608)
    CALL(img_wait)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(GSW(0), 288)
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), STRING("rsh_04_a"))
                WAIT_MS(1000)
                SET_VALUE(GSWF(3444), 1)
                CALL(evt_bero_mapchange, STRING("rsh_04_b"), STRING("e_bero"))
            END_IF
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(GSW(0), 294)
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), STRING("rsh_04_b"))
                WAIT_MS(1000)
                SET_VALUE(GSWF(3444), 1)
                CALL(evt_bero_mapchange, STRING("rsh_04_c"), STRING("e_bero"))
            END_IF
        END_IF
    END_INLINE_EVENT
    IF_INT_EQUAL(GSW(0), 297)
        CALL(evt_sub_get_mapname, LW(10))
        IF_STR_EQUAL(LW(10), STRING("rsh_02_c"))
            IF_INT_EQUAL(GSWF(3439), 0)
                SET_VALUE(GSWF(3439), 1)
                CALL(evt_msg_print, 0, STRING("stg6_rsh_123"), 0, STRING("ゆうれい"))
            END_IF
        END_IF
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_sub_get_mapname, LW(10))
        IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
            IF_STR_EQUAL(LW(1), STRING("heya_5"))
                SWITCH(GSW(0))
                    CASE_BETWEEN(306, 307)
                    CALL(evt_snd_bgmon, 160, 0)
                END_SWITCH
            END_IF
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_get_mapname, LW(10))
        SET_VALUE(LF(0), 0)
        IF_STR_EQUAL(LW(10), STRING("rsh_02_a"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(10), STRING("rsh_02_b"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(10), STRING("rsh_02_c"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_INT_EQUAL(LF(0), 1)
            IF_STR_EQUAL(LW(1), STRING("heya_4"))
                IF_INT_EQUAL(GSWF(3445), 1)
                    CALL(evt_snd_bgmon, 160, 0)
                END_IF
            END_IF
        END_IF
    END_INLINE_EVENT
    SET_VALUE(GSWF(3446), 0)
    SET_VALUE(GSWF(3447), 0)
    SET_VALUE(GSWF(3459), 0)
    CALL(evt_map_playanim, LW(5), 2, 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_free_capture, STRING("capture_img"), 2)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 1, 0, 0, 608, 480)
    CALL(evt_mapobj_set_offscreen, 1, STRING("rouka"), STRING("offscreen"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL_CHILD(kami_offscreen_check)
    CALL_CHILD(simi_offscreen_check)
    CALL(evt_npc_check, STRING("車掌"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_offscreen, STRING("車掌"), STRING("offscreen"))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF6_ROOM_OUT1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_2"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        CALL(evt_hit_get_position, LW(6), LW(13), LW(14), LW(15))
        CALL(evt_mario_mov_pos, LW(13), LW(12), 300)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_clear_offscreen, 1, STRING("rouka"))
        CALL(evt_mapobj_clear_offscreen, 1, LW(1))
        CALL(evt_mapobj_clear_offscreen, 1, LW(2))
        CALL(evt_npc_check, STRING("車掌"), LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_npc_flag_onoff, 0, STRING("車掌"), 128)
        END_IF
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_npc_set_offscreen, STRING("車掌"), 0)
        END_IF
    END_INLINE_EVENT
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("rouka"), 1)
    CALL_CHILD(simi_check)
    BEGIN_INLINE_EVENT
        CALL(evt_hit_get_position, LW(6), LW(10), LW(11), LW(12))
        ADD(LW(12), 50)
        CALL(evt_mario_party_door_move, LW(10), LW(12), FLOAT(80.0f))
    END_INLINE_EVENT
    CALL(evt_hit_get_position, LW(4), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1_2"), LW(10), LW(11), LW(12), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(3), 1)
    CALL(evt_map_playanim, LW(7), 0, 0)
    CALL(evt_map_checkanim, LW(7), LW(10), LW(11))
    WAIT_MS(LW(11))
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(2), 1)
    CALL(evt_hitobj_attr_onoff, 0, 0, LW(4), 8388608)
    CALL(evt_hitobj_attr_onoff, 0, 0, LW(6), 8388608)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()



static const GXTexMtx texMtx[10] = {
    GX_TEXMTX0, GX_TEXMTX1, GX_TEXMTX2, GX_TEXMTX3, GX_TEXMTX4,
    GX_TEXMTX5, GX_TEXMTX6, GX_TEXMTX7, GX_TEXMTX8, GX_TEXMTX9,
};

EVT_BEGIN(evt_rsh_window)
    CALL(evt_map_set_tevcallback, 1, PTR(rsh_window_tev))
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT(LW(1))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_mapobj_flag_onoff, 0, 1, LW(1), 16777216)
        CALL(evt_mapobj_flag_onoff, 0, 0, LW(1), 256)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u32 rsh_01_rnd_npc_1[15] = {
    (u32)"c_kinopiko_v", (u32)"キノピオ女紫", (u32)&evt_rsh_rnd_npc_2_1_talk, (u32)"c_kinopio_b",
    (u32)"キノピオ青", (u32)&evt_rsh_rnd_npc_2_2_talk, (u32)"c_kinopio_g", (u32)"キノピオ緑",
    (u32)&evt_rsh_rnd_npc_2_3_talk, (u32)"c_nokonoko", (u32)"ノコノコ", (u32)&evt_rsh_rnd_npc_2_4_talk,
    (u32)"c_kuribo_n", (u32)"NPCクリボー", (u32)&evt_rsh_rnd_npc_2_5_talk,
};

// TODO: type
u32 rsh_01_rnd_npc_2[9] = {
    (u32)"c_kinopiko_p", (u32)"キノピオ女桃", (u32)&evt_rsh_rnd_npc_2_6_talk, (u32)"c_kinopio_b",
    (u32)"キノピオ青", (u32)&evt_rsh_rnd_npc_2_8_talk, (u32)"c_kinopiko", (u32)"キノピオ女",
    (u32)&evt_rsh_rnd_npc_2_9_talk,
};

// TODO: type
u32 rsh_01_rnd_npc_3[6] = {
    (u32)"c_kokinopio_y", (u32)"キノピオ子黄緑", (u32)&evt_rsh_rnd_npc_2_7_talk, (u32)"c_kokinopio_b",
    (u32)"キノピオ子青", (u32)&evt_rsh_rnd_npc_2_10_talk,
};

// TODO: type
u32 rsh_03_rnd_npc_1[39] = {
    (u32)"c_kinopio", (u32)"キノピオ", (u32)&evt_rsh_rnd_npc_3_1_talk, (u32)"c_kinopiko_p",
    (u32)"キノピオ女桃", (u32)&evt_rsh_rnd_npc_3_2_talk, (u32)"c_kinopiko_v", (u32)"キノピオ女紫",
    (u32)&evt_rsh_rnd_npc_3_3_talk, (u32)"c_kinopio_b", (u32)"キノピオ青", (u32)&evt_rsh_rnd_npc_3_4_talk,
    (u32)"c_kinopiko_p", (u32)"キノピオ女桃", (u32)&evt_rsh_rnd_npc_3_5_talk, (u32)"c_kinopiko_v",
    (u32)"キノピオ女紫", (u32)&evt_rsh_rnd_npc_3_7_talk, (u32)"c_nokonoko", (u32)"ノコノコ",
    (u32)&evt_rsh_rnd_npc_3_8_talk, (u32)"c_yoshi", (u32)"ヨッシー大", (u32)&evt_rsh_rnd_npc_3_9_talk,
    (u32)"c_nokonoko", (u32)"ノコノコ", (u32)&evt_rsh_rnd_npc_4_1_talk, (u32)"c_yoshi",
    (u32)"ヨッシー大", (u32)&evt_rsh_rnd_npc_4_2_talk, (u32)"c_jugem_n", (u32)"ＮＰＣジュゲム",
    (u32)&evt_rsh_rnd_npc_4_3_talk, (u32)"c_kinopio_b", (u32)"キノピオ青", (u32)&evt_rsh_rnd_npc_4_4_talk,
    (u32)"c_nokonoko", (u32)"ノコノコ", (u32)&evt_rsh_rnd_npc_4_5_talk,
};

// TODO: type
u32 rsh_03_rnd_npc_2[21] = {
    (u32)"c_kokinopio_g", (u32)"キノピオ子緑", (u32)&evt_rsh_rnd_npc_3_6_talk, (u32)"c_kokinopio_y",
    (u32)"キノピオ子黄緑", (u32)&evt_rsh_rnd_npc_3_10_talk, (u32)"c_kinopio", (u32)"キノピオ",
    (u32)&evt_rsh_rnd_npc_4_6_talk, (u32)"c_kokinopio_g", (u32)"キノピオ子緑", (u32)&evt_rsh_rnd_npc_4_7_talk,
    (u32)"c_kuribo_n", (u32)"NPCクリボー", (u32)&evt_rsh_rnd_npc_4_8_talk, (u32)"c_kinopio_g",
    (u32)"キノピオ緑", (u32)&evt_rsh_rnd_npc_4_9_talk, (u32)"c_nokonoko", (u32)"ノコノコ",
    (u32)&evt_rsh_rnd_npc_4_10_talk,
};

// TODO: type
u32 rsh_04_rnd_npc_1[6] = {
    (u32)"c_kinopio_g", (u32)"キノピオ緑", (u32)&evt_rsh_rnd_npc_5_2_talk, (u32)"c_kinopio_b",
    (u32)"キノピオ青", (u32)&evt_rsh_rnd_npc_5_10_talk,
};

// TODO: type
u32 rsh_04_rnd_npc_2[6] = {
    (u32)"c_nokonoko", (u32)"ノコノコ", (u32)&evt_rsh_rnd_npc_5_8_talk, (u32)"c_kinopio",
    (u32)"キノピオ", (u32)&evt_rsh_rnd_npc_5_9_talk,
};

// TODO: type
u32 rsh_04_rnd_npc_3[6] = {
    (u32)"c_kokinopio_g", (u32)"キノピオ子緑", (u32)&evt_rsh_rnd_npc_5_5_talk, (u32)"c_kokinopio",
    (u32)"キノピオ子", (u32)&evt_rsh_rnd_npc_5_6_talk,
};

// TODO: type
u32 rsh_04_rnd_npc_4[12] = {
    (u32)"c_kokinopio_b", (u32)"キノピオ子青", (u32)&evt_rsh_rnd_npc_5_1_talk, (u32)"c_kokinopio_y",
    (u32)"キノピオ子黄緑", (u32)&evt_rsh_rnd_npc_5_3_talk, (u32)"c_kokinopio_b", (u32)"キノピオ子青",
    (u32)&evt_rsh_rnd_npc_5_4_talk, (u32)"c_kinopiko", (u32)"キノピオ女", (u32)&evt_rsh_rnd_npc_5_7_talk,
};

EVT_BEGIN(evt_rsh_rnd_npc_2_1_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_15"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_2_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_16"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("stg6_rsh_c_16_sel"))
    CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_16_01"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_3_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_17"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("stg6_rsh_c_17_sel"))
    CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_17_01"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_4_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_18"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_5_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_19"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_6_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_20"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_7_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_21"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_8_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_22"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_9_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_23"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_2_10_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_24"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_1_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_25"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_2_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_26"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_3_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_27"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_4_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_28"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_5_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_29"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("stg6_rsh_c_29_sel"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_29_01"))
    ELSE
        CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_29_02"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_6_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_30"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_7_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_31"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_8_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_32"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_9_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_33"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_3_10_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_34"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_1_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_35"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_2_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_36"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_3_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_37"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("stg6_rsh_c_37_sel"))
    CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_37_01"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_4_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_38"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_5_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_39"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_6_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_40"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_7_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_41"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_8_talk)
    IF_INT_EQUAL(GSWF(3457), 0)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_42"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("stg6_rsh_c_42_sel"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_42_01"))
            SET_VALUE(GSWF(3457), 1)
        ELSE
            CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_42_02"))
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_42_03"), 0, STRING("me"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_9_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_43"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_4_10_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_44"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_1_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_45"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_2_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_46"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_3_talk)
    IF_INT_EQUAL(GSWF(3458), 0)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_47"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("stg6_rsh_c_47_sel"))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_47_01"))
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_47_02"))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("stg6_rsh_c_47_03"))
        END_SWITCH
        SET_VALUE(GSWF(3458), 1)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_47_04"), 0, STRING("me"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_4_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_48"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_5_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_49"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_6_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_50"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_7_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_51"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_8_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_52"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_9_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_53"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(evt_rsh_rnd_npc_5_10_talk)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_c_54"), 0, STRING("me"))
    RETURN
EVT_END()

char* rsh_01_rnd_npc_1_list[5] = {
    "２−１", "２−２", "２−３", "２−４", "２−５",
};

char* rsh_01_rnd_npc_2_list[3] = {
    "２−６", "２−８", "２−９",
};

char* rsh_01_rnd_npc_3_list[2] = {
    "２−７", "２−１０",
};

char* rsh_03_rnd_npc_1_list[13] = {
    "３−１", "３−２", "３−３", "３−４", "３−５", "３−７", "３−８", "３−９", "４−１", "４−２", "４−３", "４−４", "４−５",
};

char* rsh_03_rnd_npc_2_list[7] = {
    "３−６", "３−１０", "４−６", "４−７", "４−８", "４−９", "４−１０",
};

char* rsh_04_rnd_npc_1_list[2] = {
    "５−２", "５−１０",
};

char* rsh_04_rnd_npc_2_list[2] = {
    "５−８", "５−９",
};

char* rsh_04_rnd_npc_3_list[2] = {
    "５−５", "５−６",
};

char* rsh_04_rnd_npc_4_list[4] = {
    "５−１", "５−３", "５−４", "５−７",
};

#ifdef MUST_MATCH
static const Zero256 zero_rsh_000103fc = {0};
#endif

EVT_BEGIN(debug_print_rnd_npc)
    SET_VALUE(LW(0), 0)
    LOOP(9)
        SWITCH(LW(0))
            CASE_LESS(3)
            SET_VALUE(LW(1), 0)
            SET_VALUE(LW(2), LW(0))
            CASE_LESS(5)
            SET_VALUE(LW(1), 1)
            SET_VALUE(LW(2), LW(0))
            SUBTRACT(LW(2), 3)
            CASE_DEFAULT
            SET_VALUE(LW(1), 2)
            SET_VALUE(LW(2), LW(0))
            SUBTRACT(LW(2), 5)
        END_SWITCH
        CALL(evt_rsh_get_rnd_npc, LW(1), LW(2), LW(3))
        CALL(evt_rsh_get_rnd_npc_data, LW(1), LW(2), LW(3), LW(4), LW(5))
        CALL(debug_print_func, LW(1), LW(2), LW(3), LW(4))
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(evt_rsh_rnd_npc_init)
    SET_VALUE(GSW(821), 0)
    SET_VALUE(GSW(822), 0)
    SET_VALUE(GSW(823), 0)
    CALL(evt_sub_random, 4, LW(0))
    CALL(evt_rsh_set_rnd_npc, 0, 0, LW(0))
    CALL(evt_sub_random, 2, LW(0))
    CALL(evt_rsh_set_rnd_npc, 0, 1, LW(0))
    CALL(evt_sub_random, 1, LW(0))
    CALL(evt_rsh_set_rnd_npc, 0, 2, LW(0))
    CALL(evt_sub_random, 12, LW(0))
    CALL(evt_rsh_set_rnd_npc, 1, 0, LW(0))
    CALL(evt_sub_random, 6, LW(0))
    CALL(evt_rsh_set_rnd_npc, 1, 1, LW(0))
    CALL(evt_sub_random, 1, LW(0))
    CALL(evt_rsh_set_rnd_npc, 2, 0, LW(0))
    CALL(evt_sub_random, 1, LW(0))
    CALL(evt_rsh_set_rnd_npc, 2, 1, LW(0))
    CALL(evt_sub_random, 1, LW(0))
    CALL(evt_rsh_set_rnd_npc, 2, 2, LW(0))
    CALL(evt_sub_random, 3, LW(0))
    CALL(evt_rsh_set_rnd_npc, 2, 3, LW(0))
    CALL_CHILD(debug_print_rnd_npc)
    RETURN
EVT_END()



EVT_BEGIN(evt_rsh_01_rnd_npc_entry)
    CALL(evt_rsh_get_rnd_npc, 0, 0, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 0, 0, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 0, 0, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客１"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客１"), LW(2))
    CALL(evt_npc_set_position, STRING("乗客１"), -275, 0, -225)
    CALL(evt_npc_set_ry, STRING("乗客１"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客１"), 512)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客１"), LW(3))
    CALL(evt_rsh_get_rnd_npc, 0, 1, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 0, 1, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 0, 1, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客２"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客２"), LW(2))
    CALL(evt_npc_set_position, STRING("乗客２"), -45, 0, -175)
    CALL(evt_npc_set_ry, STRING("乗客２"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客２"), 512)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客２"), LW(3))
    CALL(evt_rsh_get_rnd_npc, 0, 2, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 0, 2, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 0, 2, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客３"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客３"), LW(2))
    CALL(evt_npc_set_position, STRING("乗客３"), 85, 16, -255)
    CALL(evt_npc_set_ry, STRING("乗客３"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客３"), 512)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客３"), LW(3))
    RETURN
EVT_END()

// TODO: type
u32 rsh_03_rnd_npc_2_ent[46] = {
    (u32)"乗客２", 0x00000000, 0x00000000, (u32)npc_urouro_init_event,
    (u32)npc_urouro_move_event, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)npc_urouro_return_event, 0x00000000, 0x00000001,
    0xC3B40000, 0x00000000, 0xC1200000, 0x42480000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(evt_rsh_03_rnd_npc_entry)
    CALL(evt_rsh_get_rnd_npc, 1, 0, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 1, 0, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 1, 0, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客１"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客１"), LW(2))
    CALL(evt_npc_set_position, STRING("乗客１"), 460, 0, -60)
    CALL(evt_npc_set_ry, STRING("乗客１"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客１"), 512)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客１"), LW(3))
    CALL(evt_rsh_get_rnd_npc, 1, 1, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 1, 1, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 1, 1, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客２"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客２"), LW(2))
    CALL(evt_npc_setup, PTR(rsh_03_rnd_npc_2_ent))
    CALL(evt_npc_set_position, STRING("乗客２"), -360, 0, -10)
    CALL(evt_npc_set_ry, STRING("乗客２"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客２"), 1536)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客２"), LW(3))
    RETURN
EVT_END()

// TODO: type
u32 rsh_04_rnd_npc_3_ent[46] = {
    (u32)"乗客３", 0x00000000, 0x00000000, (u32)npc_urouro_init_event,
    (u32)npc_urouro_move_event, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)npc_urouro_return_event, 0x00000000, 0x00000001,
    0xC3340000, 0x00000000, 0xC3110000, 0x42480000,
    0x00000000, 0x42480000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(evt_rsh_04_rnd_npc_entry)
    CALL(evt_rsh_get_rnd_npc, 2, 0, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 2, 0, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 2, 0, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客１"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客１"), LW(2))
    CALL(evt_npc_set_position, STRING("乗客１"), 265, 0, -150)
    CALL(evt_npc_set_ry, STRING("乗客１"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客１"), 512)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客１"), LW(3))
    CALL(evt_rsh_get_rnd_npc, 2, 1, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 2, 1, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 2, 1, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客２"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客２"), LW(2))
    CALL(evt_npc_set_position, STRING("乗客２"), -230, 0, -215)
    CALL(evt_npc_set_ry, STRING("乗客２"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客２"), 512)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客２"), LW(3))
    CALL(evt_rsh_get_rnd_npc, 2, 2, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 2, 2, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 2, 2, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客３"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客３"), LW(2))
    CALL(evt_npc_setup, PTR(rsh_04_rnd_npc_3_ent))
    CALL(evt_npc_set_position, STRING("乗客３"), -180, 0, -145)
    CALL(evt_npc_set_ry, STRING("乗客３"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客３"), 1536)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客３"), LW(3))
    CALL(evt_rsh_get_rnd_npc, 2, 3, LW(0))
    CALL(evt_rsh_get_rnd_npc_data, 2, 3, LW(0), LW(1), LW(2))
    CALL(evt_rsh_get_rnd_npc_talk_evt, 2, 3, LW(0), LW(3))
    CALL(evt_npc_entry, STRING("乗客４"), LW(1))
    CALL(evt_npc_set_tribe, STRING("乗客４"), LW(2))
    CALL(evt_npc_set_position, STRING("乗客４"), -85, 16, -255)
    CALL(evt_npc_set_ry, STRING("乗客４"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("乗客４"), 1536)
    CALL(evt_rsh_set_rnd_npc_talk_evt, STRING("乗客４"), LW(3))
    RETURN
EVT_END()




// clang-format on
void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    if (swByteGet(0) < 0x153) {
        relSetEvtAddr("rsh_00_a", rsh_00_a_init_evt);
        relSetEvtAddr("rsh_00_b", rsh_00_b_init_evt);
        relSetEvtAddr("rsh_00_c", rsh_00_c_init_evt);
        relSetEvtAddr("rsh_01_a", rsh_01_a_init_evt);
        relSetEvtAddr("rsh_01_b", rsh_01_b_init_evt);
        relSetEvtAddr("rsh_01_c", rsh_01_c_init_evt);
        relSetEvtAddr("rsh_02_a", rsh_02_a_init_evt);
        relSetEvtAddr("rsh_02_b", rsh_02_b_init_evt);
        relSetEvtAddr("rsh_02_c", rsh_02_c_init_evt);
        relSetEvtAddr("rsh_03_a", rsh_03_a_init_evt);
        relSetEvtAddr("rsh_03_b", rsh_03_b_init_evt);
        relSetEvtAddr("rsh_03_c", rsh_03_c_init_evt);
        relSetEvtAddr("rsh_04_a", rsh_04_a_init_evt);
        relSetEvtAddr("rsh_04_b", rsh_04_b_init_evt);
        relSetEvtAddr("rsh_04_c", rsh_04_c_init_evt);
        relSetEvtAddr("rsh_05_a", rsh_05_a_init_evt);
        relSetEvtAddr("rsh_06_a", rsh_06_a_init_evt);
        relSetEvtAddr("rsh_07_a", rsh_07_a_init_evt);
        relSetEvtAddr("rsh_07_b", rsh_07_b_init_evt);
        relSetEvtAddr("rsh_07_c", rsh_07_c_init_evt);
        relSetEvtAddr("rsh_08_a", rsh_08_a_init_evt);
    }
    else {
        relSetEvtAddr("rsh_00_a", rsh_00_a_init_evt_nidome);
        relSetEvtAddr("rsh_00_b", rsh_00_b_init_evt_nidome);
        relSetEvtAddr("rsh_00_c", rsh_00_c_init_evt_nidome);
        relSetEvtAddr("rsh_01_a", rsh_01_a_init_evt_nidome);
        relSetEvtAddr("rsh_01_b", rsh_01_b_init_evt_nidome);
        relSetEvtAddr("rsh_01_c", rsh_01_c_init_evt_nidome);
        relSetEvtAddr("rsh_02_a", rsh_02_a_init_evt_nidome);
        relSetEvtAddr("rsh_02_b", rsh_02_b_init_evt_nidome);
        relSetEvtAddr("rsh_02_c", rsh_02_c_init_evt_nidome);
        relSetEvtAddr("rsh_03_a", rsh_03_a_init_evt_nidome);
        relSetEvtAddr("rsh_03_b", rsh_03_b_init_evt_nidome);
        relSetEvtAddr("rsh_03_c", rsh_03_c_init_evt_nidome);
        relSetEvtAddr("rsh_04_a", rsh_04_a_init_evt_nidome);
        relSetEvtAddr("rsh_04_b", rsh_04_b_init_evt_nidome);
        relSetEvtAddr("rsh_04_c", rsh_04_c_init_evt_nidome);
        relSetEvtAddr("rsh_05_a", rsh_05_a_init_evt_nidome);
        relSetEvtAddr("rsh_06_a", rsh_06_a_init_evt_nidome);
        relSetEvtAddr("rsh_07_a", rsh_07_a_init_evt);
        relSetEvtAddr("rsh_07_b", rsh_07_b_init_evt);
        relSetEvtAddr("rsh_07_c", rsh_07_c_init_evt);
        relSetEvtAddr("rsh_08_a", rsh_08_a_init_evt);
    }
    relSetBtlAddr("rsh", btlsetup_rsh_tbl, btl_rsh_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

USER_FUNC(img_wait) {
    ImageEntry* img = imgNameToPtr("capture_img", gp->inBattle != 0);

    if (!(img->flags & 2) && img->field_0x118 < 1.0f) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_YIELD;
}

// Match note: `s` groups the object pointer and its flags word in one scalarised struct, which makes MWCC
// allocate them to r5/r6 (two plain locals come out swapped). The tail increments go through a named
// pointer to a local (`++*p`) so the compiler keeps the update in the callee-saved register instead of
// folding it into a temp at the store. The `angle`/`half` temporaries below fix the pool and near block;
// `fovY / 2.0f` gives the variable-first `fmuls` (not `fovY * 0.5f`). All three are match-only shapes.
void rsh_window_tev(RshWindowTevParam* param, s32 arg1) {
    RshWindowTevObj s;
    s32 stage;
    s32 texMap;
    s32 texCoord;
    s32 field18;
    u16 x0, y0, x1, y1;
    s32 width, height;
    s32 halfHeight, halfWidth;
    void* buf;
    GXTexObj texObj;
    Mtx frustumMtx;
    f32 near;
    f32 half;
    f32 angle;
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;

    s.obj = param->obj;
    s.flags = s.obj->flags;
    stage = param->field_C;
    texMap = param->field_10;
    texCoord = param->field_14;
    field18 = param->field_18;

    if (s.flags & 0x20) {
        while ((s.obj = s.obj->parent) != NULL) {
            s.flags = s.obj->flags;
            if ((s.flags & 0x10) == 0) {
                continue;
            }
            if ((s.flags & 0x20000000) == 0) {
                s.obj->flags |= 0x20000000;
                return;
            }
            x0 = *(u16*)(s.obj->field_0xEC + 0);
            y0 = *(u16*)(s.obj->field_0xEC + 2);
            x1 = *(u16*)(s.obj->field_0xEC + 4);
            y1 = *(u16*)(s.obj->field_0xEC + 6);
            break;
        }
    }
    else {
        if ((s.flags & 0x20000000) == 0) {
            s.obj->flags |= 0x20000000;
            return;
        }
        x0 = *(u16*)(s.obj->field_0xEC + 0);
        y0 = *(u16*)(s.obj->field_0xEC + 2);
        x1 = *(u16*)(s.obj->field_0xEC + 4);
        y1 = *(u16*)(s.obj->field_0xEC + 6);
    }

    if (x0 == 0x3FF || y0 == 0x3FF || x1 == 0x3FF || y1 == 0x3FF) {
        return;
    }
    if (x0 == 0 && y0 == 0 && x1 == 0 && y1 == 0) {
        return;
    }

    x0 -= x0 & 1;
    y0 -= y0 & 1;
    x1 += x1 & 1;
    y1 += y1 & 1;
    if (x1 > gp->fbWidth) {
        x1 = gp->fbWidth;
    }
    if (y1 > gp->efbHeight) {
        y1 = gp->efbHeight;
    }

    halfWidth = (x1 - x0) / 2;
    halfHeight = (y1 - y0) / 2;
    width = x1 - x0;
    height = y1 - y0;

    buf = smartAlloc(GXGetTexBufferSize((u16)halfWidth, (u16)halfHeight, GX_TF_RGB5A3, GX_FALSE, GX_FALSE), HEAP_EFFECT)->address;
    GXSetTexCopySrc((u16)x0, (u16)y0, (u16)width, (u16)height);
    GXSetTexCopyDst((u16)halfWidth, (u16)halfHeight, GX_TF_RGB5A3, GX_TRUE);
    GXCopyTex(buf, GX_FALSE);
    GXInvalidateTexAll();
    GXPixModeSync();
    GXTexModeSync();
    GXInitTexObj(&texObj, buf, (u16)halfWidth, (u16)halfHeight, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    half = 0.5f;
    angle = (f32)tan(6.2831855f * (camGetPtr(CAMERA_3D)->fovY / 2.0f) / 360.0f);
    top = (f32)(gp->efbHeight / 2 - y1);
    bottom = (f32)(gp->efbHeight / 2 - y0);
    left = (f32)(x0 - gp->fbWidth / 2);
    right = (f32)(x1 - gp->fbWidth / 2);
    near = (f32)gp->efbHeight * half / angle;
    C_MTXLightFrustum(frustumMtx, top, bottom, left, right, near,
        half, half, 0.4f, 0.6f);
    PSMTXConcat(frustumMtx, *(Mtx*)((u8*)camGetCurPtr() + 0x11C), frustumMtx);
    PSMTXConcat(frustumMtx, *(Mtx*)param->field_4, frustumMtx);

    GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texMtx[texMap], GX_FALSE, GX_PTIDENTITY);
    GXLoadTexMtxImm(frustumMtx, texMtx[texMap], GX_MTX3x4);
    GXLoadTexObj(&texObj, texMap);
    GXSetTevOrder(stage, texCoord, texMap, GX_COLOR_NULL);
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevKColorSel(stage, GX_TEV_KCSEL_3_4);
    { s32* p = &stage; param->field_C = ++*p; }
    { s32* p = &texCoord; param->field_14 = ++*p; }
    param->field_18 = field18;
    { s32* p = &texMap; param->field_10 = ++*p; }
}

USER_FUNC(evt_rsh_get_rnd_npc) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);
    s32 value;

    switch (type) {
        case 0: {
            s32 gsw = evtGetValue(NULL, GSW(821));

            switch (index) {
                case 0:
                    value = gsw & 7;
                    break;
                case 1:
                    value = (gsw >> 3) & 3;
                    break;
                case 2:
                    value = (gsw >> 5) & 1;
                    break;
                default:
                    value = 0;
                    break;
            }
        } break;
        case 1: {
            s32 gsw = evtGetValue(NULL, GSW(822));

            switch (index) {
                case 0:
                    value = gsw & 0xF;
                    break;
                case 1:
                    value = (gsw >> 4) & 7;
                    break;
                default:
                    value = 0;
                    break;
            }
        } break;
        case 2: {
            s32 gsw = evtGetValue(NULL, GSW(823));

            switch (index) {
                case 0:
                    value = gsw & 1;
                    break;
                case 1:
                    value = (gsw >> 1) & 1;
                    break;
                case 2:
                    value = (gsw >> 2) & 1;
                    break;
                case 3:
                    value = (gsw >> 3) & 3;
                    break;
                default:
                    value = 0;
                    break;
            }
        } break;
    }
    evtSetValue(event, args[2], value);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_rsh_set_rnd_npc) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);
    s32 value = evtGetValue(event, args[2]);

    switch (type) {
        case 0: {
            s32 gsw = evtGetValue(NULL, GSW(821));

            switch (index) {
                case 0:
                    gsw |= value & 7;
                    break;
                case 1:
                    gsw |= (value & 3) << 3;
                    break;
                case 2:
                    gsw |= (value & 1) << 5;
                    break;
            }
            evtSetValue(NULL, GSW(821), gsw);
        } break;
        case 1: {
            s32 gsw = evtGetValue(NULL, GSW(822));

            switch (index) {
                case 0:
                    gsw |= value & 0xF;
                    break;
                case 1:
                    gsw |= (value & 7) << 4;
                    break;
            }
            evtSetValue(NULL, GSW(822), gsw);
        } break;
        case 2: {
            s32 gsw = evtGetValue(NULL, GSW(823));

            switch (index) {
                case 0:
                    gsw |= value & 1;
                    break;
                case 1:
                    gsw |= (value & 1) << 1;
                    break;
                case 2:
                    gsw |= (value & 1) << 2;
                    break;
                case 3:
                    gsw |= (value & 3) << 3;
                    break;
            }
            evtSetValue(NULL, GSW(823), gsw);
        } break;
    }
    return EVT_RETURN_DONE;
}

// dont_inline keeps _get_rnd_npc_data out of line: original callers emit a real `bl _get_rnd_npc_data`.
#pragma dont_inline on
void* _get_rnd_npc_data(s32 type, s32 group, s32 index) {
    u32* data = NULL;

    switch (type) {
        case 0:
            switch (group) {
                case 0:
                    data = rsh_01_rnd_npc_1;
                    break;
                case 1:
                    data = rsh_01_rnd_npc_2;
                    break;
                case 2:
                    data = rsh_01_rnd_npc_3;
                    break;
            }
            break;
        case 1:
            switch (group) {
                case 0:
                    data = rsh_03_rnd_npc_1;
                    break;
                case 1:
                    data = rsh_03_rnd_npc_2;
                    break;
            }
            break;
        case 2:
            switch (group) {
                case 0:
                    data = rsh_04_rnd_npc_1;
                    break;
                case 1:
                    data = rsh_04_rnd_npc_2;
                    break;
                case 2:
                    data = rsh_04_rnd_npc_3;
                    break;
                case 3:
                    data = rsh_04_rnd_npc_4;
                    break;
            }
            break;
    }
    if (data == NULL) {
        data = rsh_01_rnd_npc_1;
        index = 0;
    }
    return (void*)((u8*)data + index * 0xC);
}
#pragma dont_inline reset
USER_FUNC(evt_rsh_get_rnd_npc_data) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);
    s32 npc = evtGetValue(event, args[2]);
    s32* data = _get_rnd_npc_data(type, index, npc);

    evtSetValue(event, args[3], data[0]);
    evtSetValue(event, args[4], data[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_rsh_get_rnd_npc_talk_evt) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);
    s32 npc = evtGetValue(event, args[2]);
    s32* data = _get_rnd_npc_data(type, index, npc);

    evtSetValue(event, args[3], data[2]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_rsh_set_rnd_npc_talk_evt) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 evt = evtGetValue(event, args[1]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);

    npc->talkEvt = (void*)evt;
    return EVT_RETURN_DONE;
}

USER_FUNC(debug_print_func) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);
    s32 npc = evtGetValue(event, args[2]);
    s32 unk = evtGetValue(event, args[3]);
#ifdef MUST_MATCH
    char name[256];
#else
    char name[256] = {0};
#endif
    char buf[256];
    char** list;
#ifdef MUST_MATCH
    *(Zero256*)name = zero_rsh_000103fc;
#endif

    switch (type) {
        case 0:
            strcpy(name, "RSH_01:");
            switch (index) {
                case 0:
                    list = rsh_01_rnd_npc_1_list;
                    break;
                case 1:
                    list = rsh_01_rnd_npc_2_list;
                    break;
                case 2:
                    list = rsh_01_rnd_npc_3_list;
                    break;
            }
            break;
        case 1:
            strcpy(name, "RSH_03:");
            switch (index) {
                case 0:
                    list = rsh_03_rnd_npc_1_list;
                    break;
                case 1:
                    list = rsh_03_rnd_npc_2_list;
                    break;
            }
            break;
        case 2:
            strcpy(name, "RSH_04:");
            switch (index) {
                case 0:
                    list = rsh_04_rnd_npc_1_list;
                    break;
                case 1:
                    list = rsh_04_rnd_npc_2_list;
                    break;
                case 2:
                    list = rsh_04_rnd_npc_3_list;
                    break;
                case 3:
                    list = rsh_04_rnd_npc_4_list;
                    break;
            }
            break;
    }
    sprintf(buf, "%s%d(%s, %s)\n", name, index, list[npc], (char*)unk);
    return EVT_RETURN_DONE;
}

#include "tou_02.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
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
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "driver/camdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "mario/mario.h"
#include "party.h"
#include "party/party_cloud.h"
#include "memory.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>

#define PI 3.141592653589793f
#define TWO_PI 6.283185307179586f

#pragma pool_data off

#pragma force_active on
extern s32 evt_tou_get_ranking_27_text_1ED4[];

static f32 randf(f32 min, f32 max);
static void pose_change(FanPose* pose, const char* name, const char* motion);
static void pose_main(void);
static void pose_init(void);

// clang-format off
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(s_door_open)
    IF_FLOAT_EQUAL(LW(0), FLOAT(0.0f))
        IF_INT_EQUAL(GSWF(2526), 0)
            SET_VALUE(GSWF(2526), 1)
            BEGIN_INLINE_EVENT
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AUTO_DOOR1"), LW(0), LW(1), LW(2), GW(13))
            END_INLINE_EVENT
        END_IF
    END_IF
    IF_FLOAT_EQUAL(LW(0), FLOAT(180.0f))
        SET_VALUE(GSWF(2526), 0)
        CALL(evt_snd_sfxoff, GW(13))
    END_IF
    MULTIPLY_FLOAT(LW(0), FLOAT(0.33301f))
    CALL(evt_mapobj_trans, 1, STRING("door_1_b"), LW(0), 0, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("door_1_a"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_close)
    IF_FLOAT_EQUAL(LW(0), FLOAT(180.0f))
        IF_INT_EQUAL(GSWF(2526), 0)
            SET_VALUE(GSWF(2526), 1)
            BEGIN_INLINE_EVENT
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AUTO_DOOR1"), LW(0), LW(1), LW(2), GW(13))
            END_INLINE_EVENT
        END_IF
    END_IF
    IF_FLOAT_EQUAL(LW(0), FLOAT(0.0f))
        SET_VALUE(GSWF(2526), 0)
        CALL(evt_snd_sfxoff, GW(13))
    END_IF
    MULTIPLY_FLOAT(LW(0), FLOAT(0.33301f))
    CALL(evt_mapobj_trans, 1, STRING("door_1_b"), LW(0), 0, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("door_1_a"), LW(0), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(n_1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_a"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n_1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_a"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_a"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_a"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n_3_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_4"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n_3_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_4"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n_4_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_5"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n_4_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_5"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[6] = {
    {"s_bero", 1, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_01", "a_door_mon", 5, 5, s_door_close, s_door_open},
    {"n_bero_1", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_03", "s_bero_1", 5, 5, n_1_door_close, n_1_door_open},
    {"n_bero_2", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_03", "s_bero_2", 5, 5, n_2_door_close, n_2_door_open},
    {"n_bero_3", 1, 4, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_04", "e_bero", 5, 5, n_3_door_close, n_3_door_open},
    {"n_bero_4", 1, 4, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_04", "w_bero", 5, 5, n_4_door_close, n_4_door_open},
    {NULL},
};

EVT_BEGIN(evt_move_g)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    ADD(LW(2), 100)
    CALL(evt_mario_mov_pos, LW(0), LW(2), 500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ガードマン１"), -530, -185, 0, FLOAT(80.0f), 1)
        CALL(evt_set_dir_to_target, STRING("ガードマン１"), STRING("mario"))
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("ガードマン２"), 530, -185, 0, FLOAT(80.0f), 1)
    CALL(evt_set_dir_to_target, STRING("ガードマン２"), STRING("mario"))
    SET_VALUE(LW(0), 3)
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(LW(0), 4)
    CALL_CHILD(bero_case_switch_on)
    IF_INT_LESS(GSW(0), 129)
        SET_VALUE(GSW(0), 129)
    ELSE
        SET_VALUE(GSWF(2497), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(init_gard1)
    SWITCH(GSW(0))
        CASE_LESS(129)
        CALL(evt_npc_set_position, STRING("me"), -470, 0, -185)
        CASE_LESS(171)
        CALL(evt_npc_set_position, STRING("me"), -530, 0, -185)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2497), 0)
            CALL(evt_npc_set_position, STRING("me"), -470, 0, -185)
        ELSE
            CALL(evt_npc_set_position, STRING("me"), -530, 0, -185)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_gard1)
    SWITCH(GSW(0))
        CASE_LESS(128)
        CALL(evt_msg_print, 0, STRING("stg3_tou_24"), 0, 0)
        CASE_LESS(129)
        CALL(evt_msg_print, 0, STRING("stg3_tou_37"), 0, 0)
        CALL_CHILD(evt_move_g)
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_37_01"), 0, 0)
        CASE_LESS(171)
        CALL(evt_msg_print, 0, STRING("stg3_tou_37_02"), 0, 0)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_re_002_1"), 0, 0)
        ELSE
            IF_INT_EQUAL(GSWF(2497), 0)
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_001"), 0, 0)
                CALL_CHILD(evt_move_g)
            ELSE
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_002"), 0, 0)
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_gard2)
    SWITCH(GSW(0))
        CASE_LESS(129)
        CALL(evt_npc_set_position, STRING("me"), 470, 0, -185)
        CASE_LESS(171)
        CALL(evt_npc_set_position, STRING("me"), 530, 0, -185)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2497), 0)
            CALL(evt_npc_set_position, STRING("me"), 470, 0, -185)
        ELSE
            CALL(evt_npc_set_position, STRING("me"), 530, 0, -185)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_gard2)
    SWITCH(GSW(0))
        CASE_LESS(128)
        CALL(evt_msg_print, 0, STRING("stg3_tou_24"), 0, 0)
        CASE_LESS(129)
        CALL(evt_msg_print, 0, STRING("stg3_tou_37"), 0, 0)
        CALL_CHILD(evt_move_g)
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_37_01"), 0, 0)
        CASE_LESS(171)
        CALL(evt_msg_print, 0, STRING("stg3_tou_37_02"), 0, 0)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_re_002_1"), 0, 0)
        ELSE
            IF_INT_EQUAL(GSWF(2497), 0)
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_001"), 0, 0)
                CALL_CHILD(evt_move_g)
            ELSE
                CALL(evt_msg_print, 0, STRING("stg3_tou_re_002"), 0, 0)
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_madam)
    SWITCH(GSW(0))
        CASE_LESS(172)
        CALL(evt_npc_set_position, STRING("me"), 525, 0, 0)
        CASE_EQUAL(172)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("me"), 525, 0, 0)
        ELSE
            CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        END_IF
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_madam)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_01_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_01_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_01_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_01_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_01_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_01_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_01_07"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_repo)
    CALL(evt_npc_set_position, STRING("me"), -100, 0, -25)
    RETURN
EVT_END()

EVT_BEGIN(talk_repo)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_02_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_02_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_02_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_02_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_02_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_02_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_02_07"), 0, 0)
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_024"), 0, 0)
                CASE_LESS_OR_EQUAL(10)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_024_1"), 0, 0)
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2504), 0)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_023"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_024_1"), 0, 0)
                END_IF
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_cam1)
    CALL(evt_npc_set_position, STRING("me"), -150, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(talk_cam1)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_03_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_03_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_03_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_03_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_03_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_03_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_03_07"), 0, 0)
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_026"), 0, 0)
                CASE_LESS_OR_EQUAL(10)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_026_1"), 0, 0)
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2504), 0)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_025"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_026_1"), 0, 0)
                END_IF
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_cam2)
    IF_INT_LESS(GSW(0), 128)
        CALL(evt_npc_set_position, STRING("me"), 80, 200, -125)
    ELSE
        CALL(evt_npc_set_position, STRING("me"), 30, 200, -60)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(talk_cam2)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_04_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_04_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_04_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_04_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_04_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_04_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_04_07"), 0, 0)
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_028"), 0, 0)
                CASE_LESS_OR_EQUAL(10)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_028_1"), 0, 0)
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2504), 0)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_027"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_028_1"), 0, 0)
                END_IF
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_kino4)
    CALL(evt_npc_set_position, STRING("me"), -350, 0, 75)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(reg_kino4)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), -350, 75, 0, FLOAT(48.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_reverse_ry, STRING("me"))
        CALL(evt_npc_move_position, STRING("me"), 350, 75, 0, FLOAT(48.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_reverse_ry, STRING("me"))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(talk_kino4)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_05_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_05_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_05_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_05_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_05_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_05_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_05_07"), 0, 0)
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_030"), 0, 0)
                CASE_LESS_OR_EQUAL(10)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_030_1"), 0, 0)
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2504), 0)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_029"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_030_1"), 0, 0)
                END_IF
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_kino5)
    CALL(evt_npc_set_position, STRING("me"), -520, 0, 25)
    RETURN
EVT_END()

EVT_BEGIN(talk_kino5)
    CALL(evt_pouch_get_yoshiname, LW(10))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_06_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_06_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_06_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_mario_get_party, LW(0))
            IF_INT_EQUAL(LW(0), 4)
                CALL(evt_msg_print_insert, 0, STRING("stg3_npc_02_06_04_yos"), 0, 0, LW(10))
            ELSE
                CALL(evt_msg_print_insert, 0, STRING("stg3_npc_02_06_04"), 0, 0, LW(10))
            END_IF
            CASE_GREATER(0)
            CALL(evt_mario_get_party, LW(0))
            IF_INT_EQUAL(LW(0), 4)
                CALL(evt_msg_print_insert, 0, STRING("stg3_npc_02_06_05_yos"), 0, 0, LW(10), LW(10))
            ELSE
                CALL(evt_msg_print_insert, 0, STRING("stg3_npc_02_06_05"), 0, 0, LW(10))
            END_IF
            CASE_DEFAULT
            CALL(evt_mario_get_party, LW(0))
            IF_INT_EQUAL(LW(0), 4)
                CALL(evt_msg_print_insert, 0, STRING("stg3_npc_02_06_06_yos"), 0, 0, LW(10))
            ELSE
                CALL(evt_msg_print_insert, 0, STRING("stg3_npc_02_06_06"), 0, 0, LW(10))
            END_IF
        END_SWITCH
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_mario_get_party, LW(0))
            IF_INT_EQUAL(LW(0), 4)
                CALL(evt_msg_print_insert, 0, STRING("stg3_npc_02_06_07_yos"), 0, 0, LW(10))
            ELSE
                CALL(evt_msg_print, 0, STRING("stg3_npc_02_06_07"), 0, 0)
            END_IF
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(evt_mario_get_party, LW(0))
                IF_INT_EQUAL(LW(0), 4)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_032_yos"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_032"), 0, 0)
                END_IF
                CASE_LESS_OR_EQUAL(10)
                CALL(evt_mario_get_party, LW(0))
                IF_INT_EQUAL(LW(0), 4)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_032_1_yos"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_032_1"), 0, 0)
                END_IF
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2504), 0)
                    CALL(evt_mario_get_party, LW(0))
                    IF_INT_EQUAL(LW(0), 4)
                        CALL(evt_msg_print_insert, 0, STRING("stg3_npc_re_031_yos"), 0, 0, LW(10))
                    ELSE
                        CALL(evt_msg_print, 0, STRING("stg3_npc_re_031"), 0, 0)
                    END_IF
                ELSE
                    CALL(evt_mario_get_party, LW(0))
                    IF_INT_EQUAL(LW(0), 4)
                        CALL(evt_msg_print, 0, STRING("stg3_npc_re_032_1_yos"), 0, 0)
                    ELSE
                        CALL(evt_msg_print, 0, STRING("stg3_npc_re_032_1"), 0, 0)
                    END_IF
                END_IF
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_kinopiko)
    CALL(evt_npc_set_position, STRING("me"), -490, 0, 25)
    RETURN
EVT_END()

EVT_BEGIN(talk_kinopiko)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_07_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_07_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_07_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_07_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_07_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_07_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_07_07"), 0, 0)
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_034"), 0, 0)
                CASE_LESS_OR_EQUAL(10)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_034_1"), 0, 0)
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2504), 0)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_033"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_034_1"), 0, 0)
                END_IF
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_kinopio_f)
    CALL(evt_npc_set_position, STRING("キノピオ女"), 100, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(talk_kinopio_f)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_08_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_08_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_08_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_08_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_08_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_08_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2500), 0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_08_07"), 0, 0)
        ELSE
            CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_036"), 0, 0)
                CASE_LESS_OR_EQUAL(10)
                CALL(evt_msg_print, 0, STRING("stg3_npc_re_036_1"), 0, 0)
                CASE_DEFAULT
                IF_INT_EQUAL(GSWF(2504), 0)
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_035"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg3_npc_re_036_1"), 0, 0)
                END_IF
            END_SWITCH
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_chu)
    SWITCH(GSW(0))
        CASE_LESS(144)
        CALL(evt_npc_set_position, STRING("me"), 245, 0, 125)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_chu)
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_10_01"), 0, 0)
        CASE_LESS(164)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        SWITCH(LW(0))
            CASE_GREATER(15)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_10_02"), 0, 0)
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_10_03"), 0, 0)
            CASE_GREATER(5)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_10_04"), 0, 0)
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_10_05"), 0, 0)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_npc_02_10_06"), 0, 0)
        END_SWITCH
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_npc_02_10_07"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_champ)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("me"), -15, 0, 25)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32768)
            END_INLINE_EVENT
        END_IF
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
u32 move_pos[26] = {
    0x00000000, 0x00000000, 0xFFFFFF9C, 0xFFFFFFBA,
    0xFFFFFF1F, 0x00000032, 0xFFFFFF1A, 0x00000087,
    0xFFFFFF97, 0x0000006E, 0x0000000A, 0xFFFFFFF6,
    0x00000087, 0x00000087, 0x000000DC, 0x00000028,
    0x0000008A, 0xFFFFFFBF, 0x00000023, 0xFFFFFFCE,
    0x00000000, 0x0000006E, 0xFFFFFFB0, 0xFFFFFFD8,
    0x0000006E, 0xFFFFFFD8,
};

EVT_BEGIN(move_champ)
    SET_VALUE(LW(10), 0)
    LOOP(0)
        SET_INT_READ_ADDR(PTR(move_pos))
        LOOP(13)
            READ_INT2(LW(0), LW(1))
            CALL(evt_sub_random, 5, LW(0))
            ADD(LW(0), 2)
            MULTIPLY(LW(0), 1000)
            WAIT_MS(LW(0))
            CALL(evt_npc_get_position, STRING("me"), LW(5), LW(6), LW(7))
            CALL(evt_sub_random, 100, LW(0))
            CALL(evt_sub_random, 100, LW(1))
            SUBTRACT(LW(0), 50)
            ADD(LW(5), LW(0))
            ADD(LW(6), 40)
            SUBTRACT(LW(7), LW(1))
            CALL(evt_msg_toge, 2, LW(5), LW(6), LW(7))
            CALL(evt_end_count, LW(0))
            IF_INT_LESS(LW(0), 10)
                RETURN
            END_IF
            BEGIN_INLINE_EVENT
                SWITCH(LW(10))
                    CASE_EQUAL(0)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_182"), 0, 0)
                    CASE_EQUAL(1)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_183"), 0, 0)
                    CASE_EQUAL(2)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_184"), 0, 0)
                    CASE_EQUAL(3)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_185"), 0, 0)
                    CASE_EQUAL(4)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_186"), 0, 0)
                    CASE_EQUAL(5)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_187"), 0, 0)
                    CASE_EQUAL(6)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_188"), 0, 0)
                    CASE_EQUAL(7)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_189"), 0, 0)
                    CASE_EQUAL(8)
                    CALL(evt_msg_print, 0, STRING("stg3_tou_186_01"), 0, 0)
                END_SWITCH
            END_INLINE_EVENT
            ADD(LW(10), 1)
            IF_INT_GREATER(LW(10), 8)
                SET_VALUE(LW(10), 0)
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(talk_champ)
    SWITCH(GSW(0))
        CASE_LESS(171)
        CALL(evt_end_count, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_190_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_190"), 0, STRING("me"))
        END_IF
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg3_tou_479_02"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_shikowa)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("me"), -25, 0, 75)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32768)
            END_INLINE_EVENT
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_shikowa)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        CALL(evt_msg_print, 0, STRING("stg3_tou_479_01"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_uranoko)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("me"), -75, 0, 50)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32768)
            END_INLINE_EVENT
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_uranoko)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        CALL(evt_msg_print, 0, STRING("stg3_tou_479_03"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_ironA)
    CALL(evt_npc_set_scale, STRING("me"), FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.0f))
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("me"), -30, 0, -50)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32768)
            END_INLINE_EVENT
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_ironA)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        CALL(evt_msg_print, 0, STRING("stg3_tou_479_04"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_ironB)
    CALL(evt_npc_set_scale, STRING("me"), FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.0f))
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("me"), -60, 0, -25)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32768)
            END_INLINE_EVENT
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_ironB)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        CALL(evt_msg_print, 0, STRING("stg3_tou_479_05"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_borodo)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("me"), -100, 0, 100)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32768)
            END_INLINE_EVENT
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_borodo)
    SWITCH(GSW(0))
        CASE_EQUAL(171)
        CALL(evt_msg_print, 0, STRING("stg3_tou_479_06"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[391] = {
    (u32)"ガードマン１", 0x40080600, 0x00000000, (u32)&init_gard1,
    0x00000000, (u32)&talk_gard1, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ガードマン２",
    0x40080600, 0x00000000, (u32)&init_gard2, 0x00000000,
    (u32)&talk_gard2, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"マダム", 0x40080600,
    0x00000000, (u32)&init_madam, 0x00000000, (u32)&talk_madam,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"レポーター", 0x40080600, 0x00000000,
    (u32)&init_repo, 0x00000000, (u32)&talk_repo, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"カメラマン１", 0x40080600, 0x00000000, (u32)&init_cam1,
    0x00000000, (u32)&talk_cam1, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"カメラマン２",
    0x40080600, 0x00000000, (u32)&init_cam2, 0x00000000,
    (u32)&talk_cam2, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"キノピオ４", 0x40080600,
    0x00000000, (u32)&init_kino4, (u32)&reg_kino4, (u32)&talk_kino4,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"キノピオ５", 0x40080600, 0x00000000,
    (u32)&init_kino5, 0x00000000, (u32)&talk_kino5, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"キノピコ", 0x40080600, 0x00000000, (u32)&init_kinopiko,
    0x00000000, (u32)&talk_kinopiko, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"バッジ屋",
    0x40080600, 0x00000000, (u32)&init_chu, 0x00000000,
    (u32)&talk_chu, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"チャンピョン", 0x40080600,
    0x00000000, (u32)&init_champ, 0x00000000, (u32)&talk_champ,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"キノシコワ", 0x40080600, 0x00000000,
    (u32)&init_shikowa, 0x00000000, (u32)&talk_shikowa, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"ウラノコ", 0x40080600, 0x00000000, (u32)&init_uranoko,
    0x00000000, (u32)&talk_uranoko, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"アイアンＡ",
    0x40080600, 0x00000000, (u32)&init_ironA, 0x00000000,
    (u32)&talk_ironA, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"アイアンＢ", 0x40080600,
    0x00000000, (u32)&init_ironB, 0x00000000, (u32)&talk_ironB,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ボロドー", 0x40080600, 0x00000000,
    (u32)&init_borodo, 0x00000000, (u32)&talk_borodo, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(evt_tougi2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    SET_VALUE(LF(0), 0)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("n_bero_2"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mario_set_pos, -25, 200, -220)
        CALL(evt_party_set_pos, 0, 25, 200, -220)
    ELSE
        CALL(evt_mario_set_pos, -25, 0, -220)
        CALL(evt_party_set_pos, 0, 25, 0, -220)
    END_IF
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 270, -1)
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_cam3d_evt_set, 0, 290, 190, 0, 238, -158, 0, 11)
    ELSE
        CALL(evt_cam3d_evt_set, 0, 90, 190, 0, 38, -158, 0, 11)
    END_IF
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 2)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        IF_INT_EQUAL(LF(0), 1)
            CALL_CHILD(n_2_door_open)
        ELSE
            CALL_CHILD(n_1_door_open)
        END_IF
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(2), 100)
        CALL(evt_mario_mov_pos, LW(0), LW(2), 1000)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(2), 100)
        CALL(evt_party_move_pos, 0, LW(0), LW(2), 1000)
    END_INLINE_EVENT
    WAIT_MS(700)
    SET_VALUE(LW(1), 2)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        IF_INT_EQUAL(LF(0), 1)
            CALL_CHILD(n_2_door_close)
        ELSE
            CALL_CHILD(n_1_door_close)
        END_IF
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_party, STRING("stg3_tou_33"))
    CALL(evt_msg_select, 0, STRING("stg3_tou_33_yn"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_get_party, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_34_kur"))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_34_nok"))
            CASE_EQUAL(5)
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_34_win"))
        END_SWITCH
    ELSE
        CALL(evt_mario_get_party, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_35_kur"))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_35_nok"))
            CASE_EQUAL(5)
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_35_win"))
        END_SWITCH
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg3_tou_36"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSWF(2374), 0)
    SET_VALUE(GSW(0), 128)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: type
u32 ext_fan_data[84] = {
    (u32)"c_kinopio", (u32)"KNP_W_1", 0x40000000, (u32)"c_kinopio",
    (u32)"KNP_W_1", 0x40C00000, (u32)"c_kinopio", (u32)"KNP_W_1",
    0x41200000, (u32)"c_kinopio", (u32)"KNP_W_1", 0x41600000,
    (u32)"c_kinopio", (u32)"KNP_W_1", 0x41900000, (u32)"c_kinopio",
    (u32)"KNP_W_1", 0x41B00000, (u32)"c_kinopio", (u32)"KNP_W_1",
    0x41D00000, (u32)"c_kinopio", (u32)"KNP_W_1", 0x41F00000,
    (u32)"c_kinopio", (u32)"KNP_W_1", 0x42080000, (u32)"c_kinopiko",
    (u32)"KNF_W_1", 0x40000000, (u32)"c_kinopiko", (u32)"KNF_W_1",
    0x40C00000, (u32)"c_kinopiko", (u32)"KNF_W_1", 0x41200000,
    (u32)"c_kinopiko", (u32)"KNF_W_1", 0x41600000, (u32)"c_kinopiko",
    (u32)"KNF_W_1", 0x41900000, (u32)"c_kinopiko", (u32)"KNF_W_1",
    0x41B00000, (u32)"c_kinopiko", (u32)"KNF_W_1", 0x41D00000,
    (u32)"c_kinopiko", (u32)"KNF_W_1", 0x41F00000, (u32)"c_kinopiko",
    (u32)"KNF_W_1", 0x42080000, (u32)"c_kinopiko", (u32)"KNF_W_1",
    0x42180000, (u32)"c_jugemu", (u32)"JGM_W_1", 0x00000000,
    (u32)"c_jugemu", (u32)"JGM_W_1", 0x40800000, (u32)"c_jugemu",
    (u32)"JGM_W_1", 0x41000000, (u32)"c_jugemu", (u32)"JGM_W_1",
    0x41400000, (u32)"c_jugemu", (u32)"JGM_W_1", 0x41800000,
    (u32)"c_jugemu", (u32)"JGM_W_1", 0x41A00000, (u32)"c_jugemu",
    (u32)"JGM_W_1", 0x41C00000, (u32)"c_jugemu", (u32)"JGM_W_1",
    0x41E00000, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(mario_momikucha_msg)
    SET_VALUE(LW(10), 0)
    LOOP(0)
        CALL(evt_sub_random, 5, LW(0))
        ADD(LW(0), 2)
        MULTIPLY(LW(0), 1000)
        WAIT_MS(LW(0))
        CALL(evt_sub_random, 200, LW(0))
        CALL(evt_sub_random, 200, LW(1))
        SUBTRACT(LW(0), 50)
        SUBTRACT(LW(1), 50)
        CALL(evt_msg_toge, 2, LW(0), 30, LW(1))
        CALL(evt_end_count, LW(0))
        IF_INT_LESS(LW(0), 10)
            RETURN
        END_IF
        BEGIN_INLINE_EVENT
            SWITCH(LW(10))
                CASE_EQUAL(0)
                CALL(evt_msg_print, 0, STRING("stg3_tou_236"), 0, 0)
                CASE_EQUAL(1)
                CALL(evt_msg_print, 0, STRING("stg3_tou_237"), 0, 0)
                CASE_EQUAL(2)
                CALL(evt_msg_print, 0, STRING("stg3_tou_238"), 0, 0)
                CASE_EQUAL(3)
                CALL(evt_msg_print, 0, STRING("stg3_tou_239"), 0, 0)
                CASE_EQUAL(4)
                CALL(evt_msg_print, 0, STRING("stg3_tou_240"), 0, 0)
                CASE_EQUAL(5)
                CALL(evt_msg_print, 0, STRING("stg3_tou_240_01"), 0, 0)
            END_SWITCH
        END_INLINE_EVENT
        ADD(LW(10), 1)
        IF_INT_GREATER(LW(10), 5)
            SET_VALUE(LW(10), 0)
        END_IF
    END_LOOP
    RETURN
EVT_END()

// TODO: write ext_fan_dispent (global, 0x1a0 bytes) here:
// its asm is in build/G8MJ01/tou/asm/tou_02.s; implemented below in source order

// TODO: write ext_fan_main (global, 0xfcc bytes) here:

// TODO: write ext_fan_init (global, 0x670 bytes) here:
// its asm is in build/G8MJ01/tou/asm/tou_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_momikucha)
    SET_VALUE(LF(0), 0)
    SET_VALUE(GF(10), 0)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 133)
        IF_INT_EQUAL(GSWF(2402), 0)
            SET_VALUE(GSWF(2402), 1)
            SET_VALUE(LF(0), 1)
            SET_VALUE(GF(10), 0)
        END_IF
    END_IF
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        IF_INT_EQUAL(GSWF(2403), 0)
            SET_VALUE(GSWF(2403), 1)
            SET_VALUE(LF(0), 1)
            SET_VALUE(GF(10), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        RETURN
    END_IF
    CALL(evt_npc_entry, STRING("fan"), STRING("c_kinopio"))
    CALL(evt_npc_entry, STRING("fan2"), STRING("c_kinopiko"))
    CALL(evt_npc_entry, STRING("fan3"), STRING("c_jugemu"))
    CALL(evt_npc_set_position, STRING("fan"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("fan2"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("fan3"), 0, -1000, 0)
    CALL(evt_ext_entry, 200, PTR(ext_fan_data), PTR(ext_fan_init), PTR(ext_fan_main), PTR(ext_fan_dispent))
    IF_INT_EQUAL(GF(10), 0)
        CALL(evt_npc_set_position, STRING("チャンピョン"), 0, 0, 0)
        CALL(evt_npc_change_interrupt, STRING("チャンピョン"), 1, PTR(move_champ))
    ELSE
        SPAWN_EVENT(mario_momikucha_msg)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_key)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_yoshi_ride, 0, 0)
    SET_VALUE(GSW(0), 154)
    CALL(evt_item_get_item_end_wait, STRING("key"))
    CALL(evt_party_stop, 0)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_307_08"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_307_09"), 0, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_pouch_receive_mail, 15)
    CALL(evt_pouch_open_mail, 15)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_1)
    SET_VALUE(GSWF(2456), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_2)
    SET_VALUE(GSWF(2457), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_3)
    SET_VALUE(GSWF(2458), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_4)
    SET_VALUE(GSWF(2459), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_5)
    SET_VALUE(GSWF(2460), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_6)
    SET_VALUE(GSWF(2461), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_7)
    SET_VALUE(GSWF(2462), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_8)
    SET_VALUE(GSWF(2463), 1)
    IF_INT_EQUAL(GSW(0), 153)
        CALL(evt_mapobj_get_position, STRING("p_m_8"), LW(0), LW(1), LW(2))
        CALL(evt_item_entry, STRING("key"), 19, LW(0), LW(1), LW(2), 4, -1, PTR(evt_key))
        CALL(evt_item_set_move_dir_speed, STRING("key"), FLOAT(180.0f), FLOAT(90.0f))
        CALL(evt_item_set_gravity, STRING("key"), FLOAT(1.0f))
        CALL(evt_item_set_bound_rate, STRING("key"), FLOAT(0.6f))
        CALL(evt_item_set_jump_power, STRING("key"), FLOAT(500.0f))
        CALL(evt_item_status_on, STRING("key"), 16)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_poster_9)
    SET_VALUE(GSWF(2464), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_after_peach)
    CALL(evt_cam3d_evt_set, 0, 120, 494, 0, 40, 0, 0, 11)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_mario_set_pos, 25, 0, 75)
    CALL(evt_party_set_pos, 0, 60, 0, 100)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_1_a"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_1_b"), 1)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_fade_end_wait)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_479_00_01"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_set, 0, 100, 395, 0, 40, 0, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_479_00_02"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -40, 100, 395, -40, 40, 0, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_479_00_03"), 0, STRING("チャンピョン"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_479_00_04"), 0, STRING("ウラノコ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 100, 395, 0, 40, 0, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_479_00_05"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_set, 0, 120, 494, 0, 40, 0, 500, 11)
    CALL(evt_party_run, 0)
    CALL(evt_mario_mov_pos2, 0, 190, FLOAT(80.0f))
    CALL(evt_bero_mapchange, STRING("tou_01"), STRING("a_door_mon"))
    RETURN
EVT_END()

EVT_BEGIN(tou_02_init_evt)
    SWITCH(GSW(0))
        CASE_BETWEEN(127, 137)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU2"))
        CALL(evt_snd_bgmoff, 26624)
        CASE_GREATER_OR_EQUAL(165)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU2"))
        CALL(evt_snd_bgmoff, 26624)
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
        CALL(evt_snd_bgmon, 160, 0)
        CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU2"))
    END_SWITCH
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    IF_INT_EQUAL(GSWF(2374), 1)
        CALL_CHILD(bero_case_entry)
        SPAWN_EVENT(evt_tougi2)
    ELSE
        CALL_CHILD(evt_bero_info_run)
    END_IF
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL_CHILD(init_kinopio_f)
    CALL(evt_npc_change_interrupt, STRING("キノピオ女"), 6, PTR(talk_kinopio_f))
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -260, 0, -120, STRING("sita"), 125, GSWF(5623))
    IF_INT_LESS(GSW(0), 129)
        SET_VALUE(LW(0), 3)
        CALL_CHILD(bero_case_switch_off)
        SET_VALUE(LW(0), 4)
        CALL_CHILD(bero_case_switch_off)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 172)
        IF_INT_EQUAL(GSWF(2497), 0)
            SET_VALUE(LW(0), 3)
            CALL_CHILD(bero_case_switch_off)
            SET_VALUE(LW(0), 4)
            CALL_CHILD(bero_case_switch_off)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 171)
        IF_INT_EQUAL(GSWF(2496), 0)
            CALL(evt_npc_set_position, STRING("キノピオ女"), 0, -1000, 0)
            CALL(evt_npc_set_position, STRING("キノピオ４"), 0, -1000, 0)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 171)
        CALL(evt_mario_get_party, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_mario_set_party, 0, 1)
            CALL(evt_party_run, 0)
            CALL(evt_pouch_mario_recovery)
            CALL(evt_pouch_all_party_recovery)
        END_IF
        CALL(evt_pouch_remove_item, 65, LW(0))
        SPAWN_EVENT(evt_after_peach)
        SET_VALUE(GSWF(2402), 1)
        SET_VALUE(GSWF(2403), 1)
    END_IF
    CALL(evt_map_playanim, STRING("anm_paneru"), 1, 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            LOOP(0)
                CALL(evt_map_checkanim, STRING("anm_paneru"), LW(0), LW(1))
                IF_INT_LESS(LW(1), 5333)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AMB_OBJ_SCREEN2"), 178, 0, -98, 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            WAIT_FRAMES(10)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AMB_OBJ_SCREEN1"), 178, 0, -98, 0)
            LOOP(0)
                CALL(evt_map_checkanim, STRING("anm_paneru"), LW(0), LW(1))
                IF_INT_LESS(LW(1), 2000)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AMB_OBJ_SCREEN2"), 178, 0, -98, 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            WAIT_FRAMES(5)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AMB_OBJ_SCREEN1"), 178, 0, -98, 0)
            LOOP(0)
                CALL(evt_map_checkanim, STRING("anm_paneru"), LW(0), LW(1))
                IF_INT_GREATER(LW(1), 6333)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_LOOP
    END_INLINE_EVENT
    CALL_CHILD(evt_momikucha)
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    SWITCH(LW(0))
        CASE_LESS(2)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("p_p_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("p_p_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("p_p_3"), 1)
        CALL(evt_hitobj_onoff, STRING("a_p_p_2"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_p_p_3"), 1, 1)
        CALL(evt_cloud_init)
        IF_INT_EQUAL(GSWF(2457), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_2"), STRING("a_p_m_2"), PTR(evt_poster_2))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_2"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_2"), 1, 0)
        END_IF
        IF_INT_EQUAL(GSWF(2458), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_3"), STRING("a_p_m_3"), PTR(evt_poster_3))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_3"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_3"), 1, 0)
        END_IF
        IF_INT_EQUAL(GSWF(2459), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_4"), STRING("a_p_m_4"), PTR(evt_poster_4))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_4"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_4"), 1, 0)
        END_IF
        IF_INT_EQUAL(GSWF(2460), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_5"), STRING("a_p_m_5"), PTR(evt_poster_5))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_5"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_5"), 1, 0)
        END_IF
        IF_INT_EQUAL(GSWF(2461), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_6"), STRING("a_p_m_6"), PTR(evt_poster_6))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_6"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_6"), 1, 0)
        END_IF
        IF_INT_EQUAL(GSWF(2462), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_7"), STRING("a_p_m_7"), PTR(evt_poster_7))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_7"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_7"), 1, 0)
        END_IF
        IF_INT_EQUAL(GSWF(2463), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_8"), STRING("a_p_m_8"), PTR(evt_poster_8))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_8"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_8"), 1, 0)
        END_IF
        IF_INT_EQUAL(GSWF(2464), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_9"), STRING("a_p_m_9"), PTR(evt_poster_9))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_9"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_9"), 1, 0)
        END_IF
        CASE_LESS(11)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("p_p_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("p_p_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("p_p_3"), 1)
        CALL(evt_hitobj_onoff, STRING("a_p_p_2"), 1, 1)
        CALL(evt_hitobj_onoff, STRING("a_p_p_3"), 1, 0)
        CALL(evt_cloud_init)
        IF_INT_EQUAL(GSWF(2456), 0)
            CALL(evt_cloud_ent, 0, STRING("p_m_1"), STRING("a_p_m_1"), PTR(evt_poster_1))
        ELSE
            CALL(evt_mapobj_flag_onoff, 0, 1, STRING("p_m_1"), 1)
            CALL(evt_hitobj_onoff, STRING("a_p_m_1"), 1, 0)
        END_IF
        CASE_DEFAULT
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("p_p_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("p_p_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("p_p_3"), 1)
        CALL(evt_hitobj_onoff, STRING("a_p_p_2"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_p_p_3"), 1, 0)
    END_SWITCH
    IF_INT_EQUAL(GSWF(2463), 1)
        IF_INT_EQUAL(GSW(0), 153)
            CALL(evt_item_entry, STRING("key"), 19, -110, 200, -180, 16, -1, PTR(evt_key))
        END_IF
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_kaidan_7"), 2)
    RETURN
EVT_END()

// TODO: type
ExtFanWork* fan_wp;

// NONMATCHING (98.53%): the body is structurally identical but the stack frame is 0x30
// instead of 0x60 (the original reserves extra space for the name buffer).
static void pose_init(void) {
    ExtFanData* data = (ExtFanData*)ext_fan_data;
    char name[0x10];
    char motion[0x8];
    s32 start = 0;
    s32 i = 0;

    strcpy(name, "");
    strcpy(motion, "");
    fan_wp->groupCount = -1;
    do {
        if (strcmp(data->name, name) != 0) {
            strcpy(name, data->name);
            strcpy(motion, "");
        }
        if (strcmp(data->motion, motion) != 0) {
            strcpy(motion, data->motion);
            start = i;
            fan_wp->groupCount++;
        }
        strcpy(fan_wp->groups[fan_wp->groupCount].name, name);
        strcpy(fan_wp->groups[fan_wp->groupCount].motion, motion);
        fan_wp->groups[fan_wp->groupCount].start = start;
        fan_wp->groups[fan_wp->groupCount].count = i - start + 1;
        fan_wp->groups[fan_wp->groupCount].scale = data->scale;
        fan_wp->groups[fan_wp->groupCount].field_0x18 = 1;
        i++;
        data++;
    } while (data->name != NULL);
    fan_wp->groupCount++;
}

// NONMATCHING (98.63%): only general-purpose register allocation differs (the original
// keeps fan/pose/i in r24-r26).
static void pose_main(void) {
    ExtPose* pose = extGetPosePtr();
    FanPose* fan = fan_wp->poses;
    s32 i;

    for (i = 0; i < fan_wp->poseNum; i++, fan++, pose++) {
        PoseGroup* group = (PoseGroup*)((u8*)gp + 0xC + fan->groupIndex * 0x28);
        f32 ratio = (f32)OSTicksToMilliseconds((u64)gp->renderTime - (u64)fan->time);

        ratio = (60.0f * ratio) / 1000.0f / group->scale;
        if (ratio >= 1.0f) {
            fan->time = gp->renderTime;
            ratio = 0.0f;
        }
        pose->field_0x10 = group->start + (s32)((f32)(group->count - 1) * ratio);
    }
}

static void pose_change(FanPose* pose, const char* name, const char* motion) {
    PoseGroup* group = &fan_wp->groups[0];
    s32 i;

    for (i = 0; i < fan_wp->groupCount; i++, group++) {
        if (strcmp(group->name, name) == 0 && strcmp(group->motion, motion) == 0) {
            break;
        }
    }
    if (i < fan_wp->groupCount && pose->groupIndex != i) {
        pose->groupIndex = i;
        pose->time = gp->renderTime;
    }
}

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;
    return r * (max - min) + min;
}

// TODO: write ext_fan_init (global, 0x670 bytes) here

void fan_disp(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

// TODO: write ext_fan_main (global, 0xfcc bytes) here

// NONMATCHING (98.79%): only general-purpose register allocation differs from the original.
void ext_fan_dispent(void) {
    ExtPose* pose = extGetPosePtr();
    FanPose* fan = fan_wp->poses;
    s32 i;

    for (i = 0; i < fan_wp->poseNum; i++, fan++, pose++) {
        PoseGroup* group = (PoseGroup*)((u8*)gp + 0xC + fan->groupIndex * 0x28);
        f32 ratio = (f32)OSTicksToMilliseconds((u64)gp->renderTime - (u64)fan->time);

        ratio = (60.0f * ratio) / 1000.0f / group->scale;
        if (ratio >= 1.0f) {
            fan->time = gp->renderTime;
            ratio = 0.0f;
        }
        pose->field_0x10 = group->start + (s32)((f32)(group->count - 1) * ratio);
    }
    dispEntry(4, 0.0f, 1, (DispCallback)fan_disp, NULL);
}

// NONMATCHING (54.58%): structure matches but the sin/cos coefficient loads are not
// hoisted before the branch (the original loads 0.00761/0.16605/1.0 and 0.03705/0.4967 early)
// and the resulting fpr allocation differs.
s32 check_hammer(FanPose* pose) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* party;
    f32 ang;
    f32 rad;
    f32 s;
    f32 c;
    f32 x;
    f32 z;

    if (mario->position.y > 0.0f) {
        return 0;
    }
    if (pose->flags & 1) {
        return 0;
    }
    if (mario->currMotionId == 0x12 && fan_wp->field_0x50C > 10) {
        f32 t;

        ang = reviseAngle(mario->unk1A8 - 90.0f + camGetPtr(4)->viewYaw);
        rad = TWO_PI * ang / 360.0f;
        s = 0.00761f;
        if (rad <= PI) {
            t = MTXDegToRad(90.0f);
            if (rad <= t) {
                f32 x2 = rad * rad;
                s = (s * x2) - 0.16605f;
                s = (s * x2) + 1.0f;
                s = s * rad;
            } else {
                f32 d = t - (rad - t);
                f32 d2 = d * d;
                s = (s * d2) - 0.16605f;
                s = (s * d2) + 1.0f;
                s = s * d;
            }
        } else {
            t = MTXDegToRad(270.0f);
            if (rad < t) {
                f32 d = rad - PI;
                f32 d2 = d * d;
                s = (s * d2) - 0.16605f;
                s = (s * d2) + 1.0f;
                s = s * d;
                s = -s;
            } else {
                f32 d = MTXDegToRad(90.0f) - (rad - t);
                f32 d2 = d * d;
                s = (s * d2) - 0.16605f;
                s = (s * d2) + 1.0f;
                s = s * d;
                s = -s;
            }
        }
        x = 35.0f * s + mario->position.x;
        c = 0.03705f;
        if (rad <= PI) {
            if (rad <= MTXDegToRad(90.0f)) {
                f32 x2 = rad * rad;
                c = (c * x2) - 0.4967f;
                c = (c * x2) + 1.0f;
            } else {
                f32 d = MTXDegToRad(90.0f) - (rad - MTXDegToRad(90.0f));
                f32 d2 = d * d;
                c = (c * d2) - 0.4967f;
                c = (c * d2) + 1.0f;
                c = -c;
            }
        } else {
            if (rad < MTXDegToRad(270.0f)) {
                f32 d = rad - PI;
                f32 d2 = d * d;
                c = (c * d2) - 0.4967f;
                c = (c * d2) + 1.0f;
                c = -c;
            } else {
                f32 d = MTXDegToRad(90.0f) - (rad - MTXDegToRad(270.0f));
                f32 d2 = d * d;
                c = (c * d2) - 0.4967f;
                c = (c * d2) + 1.0f;
            }
        }
        z = mario->position.z - 35.0f * c;
        if (distABf(pose->position.x, pose->position.z, x, z) < 35.0f) {
            return 1;
        }
    }
    if (mario->currMotionId == 0x13 && mario->currSubMotionId == 0x14) {
        if (PSVECDistance(&mario->position, &pose->position) < 65.0f) {
            return 1;
        }
    }
    party = partyGetPtr(0);
    if (party != NULL && party->currentMemberId == 5 && (party->flags & 0x100)) {
        Vec local;
        f32 dist;
        f32 t;

        ang = TWO_PI * cloudGetBreathDir() / 360.0f;
        dist = cloudGetBreathDist();
        s = 0.00761f;
        if (ang <= PI) {
            t = MTXDegToRad(90.0f);
            if (ang <= t) {
                f32 x2 = ang * ang;
                s = (s * x2) - 0.16605f;
                s = (s * x2) + 1.0f;
                s = s * ang;
            } else {
                f32 d = t - (ang - t);
                f32 d2 = d * d;
                s = (s * d2) - 0.16605f;
                s = (s * d2) + 1.0f;
                s = s * d;
            }
        } else {
            t = MTXDegToRad(270.0f);
            if (ang < t) {
                f32 d = ang - PI;
                f32 d2 = d * d;
                s = (s * d2) - 0.16605f;
                s = (s * d2) + 1.0f;
                s = s * d;
                s = -s;
            } else {
                f32 d = MTXDegToRad(90.0f) - (ang - t);
                f32 d2 = d * d;
                s = (s * d2) - 0.16605f;
                s = (s * d2) + 1.0f;
                s = s * d;
                s = -s;
            }
        }
        local.x = party->position.x + dist * s;
        local.y = party->position.y;
        c = 0.03705f;
        if (ang <= PI) {
            if (ang <= MTXDegToRad(90.0f)) {
                f32 x2 = ang * ang;
                c = (c * x2) - 0.4967f;
                c = (c * x2) + 1.0f;
            } else {
                f32 d = MTXDegToRad(90.0f) - (ang - MTXDegToRad(90.0f));
                f32 d2 = d * d;
                c = (c * d2) - 0.4967f;
                c = (c * d2) + 1.0f;
                c = -c;
            }
        } else {
            if (ang < MTXDegToRad(270.0f)) {
                f32 d = ang - PI;
                f32 d2 = d * d;
                c = (c * d2) - 0.4967f;
                c = (c * d2) + 1.0f;
                c = -c;
            } else {
                f32 d = MTXDegToRad(90.0f) - (ang - MTXDegToRad(270.0f));
                f32 d2 = d * d;
                c = (c * d2) - 0.4967f;
                c = (c * d2) + 1.0f;
            }
        }
        local.z = party->position.z - dist * c;
        if (ang >= 0.0f) {
            if (PSVECDistance(&local, &pose->position) < 30.0f) {
                return 2;
            }
        }
    }
    return 0;
}

USER_FUNC(evt_end_count) {
    evtSetValue(event, event->args[0], fan_wp->field_0x510);
    return EVT_RETURN_DONE;
}
// clang-format on

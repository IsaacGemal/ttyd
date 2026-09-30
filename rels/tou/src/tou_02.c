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
#include "driver/hitdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "mario/mario.h"
#include "party.h"
#include "party/party_cloud.h"
#include "pmario_sound.h"
#include "memory.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>

#define PI 3.141592653589793f
#define TWO_PI 6.283185307179586f

#pragma pool_data off

#pragma force_active on
extern s32 evt_tou_get_ranking_27_text_1ED4[];
u8 psndSFX_get_vol(s32 id);
void psndSFX_vol(s32 id, u8 vol);

static f32 randf(f32 min, f32 max);
static void pose_change(FanPose* pose, const char* name, const char* motion);
static void pose_main(void);
static void pose_init(void);
static inline f32 fanSinf(f32 rad);
static inline f32 fanCosf(f32 rad);

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

// 13 (x,z) coordinate pairs read by READ_INT2; no matching struct in include/.
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

static NpcSetupInfo npcEnt[17] = {
    { "ガードマン１", 0x40080600, 0, &init_gard1, NULL, &talk_gard1, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "ガードマン２", 0x40080600, 0, &init_gard2, NULL, &talk_gard2, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "マダム", 0x40080600, 0, &init_madam, NULL, &talk_madam, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "レポーター", 0x40080600, 0, &init_repo, NULL, &talk_repo, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "カメラマン１", 0x40080600, 0, &init_cam1, NULL, &talk_cam1, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "カメラマン２", 0x40080600, 0, &init_cam2, NULL, &talk_cam2, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "キノピオ４", 0x40080600, 0, &init_kino4, &reg_kino4, &talk_kino4, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "キノピオ５", 0x40080600, 0, &init_kino5, NULL, &talk_kino5, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "キノピコ", 0x40080600, 0, &init_kinopiko, NULL, &talk_kinopiko, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "バッジ屋", 0x40080600, 0, &init_chu, NULL, &talk_chu, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "チャンピョン", 0x40080600, 0, &init_champ, NULL, &talk_champ, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "キノシコワ", 0x40080600, 0, &init_shikowa, NULL, &talk_shikowa, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "ウラノコ", 0x40080600, 0, &init_uranoko, NULL, &talk_uranoko, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "アイアンＡ", 0x40080600, 0, &init_ironA, NULL, &talk_ironA, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "アイアンＢ", 0x40080600, 0, &init_ironB, NULL, &talk_ironB, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { "ボロドー", 0x40080600, 0, &init_borodo, NULL, &talk_borodo, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
    { 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 },
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

ExtFanData ext_fan_data[28] = {
    { "c_kinopio", "KNP_W_1", 2.0f },
    { "c_kinopio", "KNP_W_1", 6.0f },
    { "c_kinopio", "KNP_W_1", 10.0f },
    { "c_kinopio", "KNP_W_1", 14.0f },
    { "c_kinopio", "KNP_W_1", 18.0f },
    { "c_kinopio", "KNP_W_1", 22.0f },
    { "c_kinopio", "KNP_W_1", 26.0f },
    { "c_kinopio", "KNP_W_1", 30.0f },
    { "c_kinopio", "KNP_W_1", 34.0f },
    { "c_kinopiko", "KNF_W_1", 2.0f },
    { "c_kinopiko", "KNF_W_1", 6.0f },
    { "c_kinopiko", "KNF_W_1", 10.0f },
    { "c_kinopiko", "KNF_W_1", 14.0f },
    { "c_kinopiko", "KNF_W_1", 18.0f },
    { "c_kinopiko", "KNF_W_1", 22.0f },
    { "c_kinopiko", "KNF_W_1", 26.0f },
    { "c_kinopiko", "KNF_W_1", 30.0f },
    { "c_kinopiko", "KNF_W_1", 34.0f },
    { "c_kinopiko", "KNF_W_1", 38.0f },
    { "c_jugemu", "JGM_W_1", 0.0f },
    { "c_jugemu", "JGM_W_1", 4.0f },
    { "c_jugemu", "JGM_W_1", 8.0f },
    { "c_jugemu", "JGM_W_1", 12.0f },
    { "c_jugemu", "JGM_W_1", 16.0f },
    { "c_jugemu", "JGM_W_1", 20.0f },
    { "c_jugemu", "JGM_W_1", 24.0f },
    { "c_jugemu", "JGM_W_1", 28.0f },
    { 0x00000000, 0x00000000, 0.0f },
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

ExtFanWork* fan_wp;

static void pose_init(void) {
    ExtFanData* data = (ExtFanData*)ext_fan_data;
    char name[0x20];
    char motion[0x20];
    s32 i = 0;
    s32 start;

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

// NONMATCHING (98.84%): only general-purpose register allocation differs (the original
// keeps fan/pose/i in r24-r26).
static void pose_main(void) {
    ExtPose* pose = extGetPosePtr();
    s32 i;
    FanPose* fan = fan_wp->poses;

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

// NONMATCHING (90.94%): written from the asm; the inlined pose_init/pose_change leave a
// 0x110-byte frame instead of 0x100 and the general-purpose registers are allocated differently.
void ext_fan_init(void) {
    s32 i;
    ExtPose* pose;
    FanPose* fan;

    fan_wp = _mapAlloc(0x51C);
    memset(fan_wp, 0, 0x51C);
    fan_wp->poseNum = extGetPoseNum();
    fan_wp->poses = _mapAlloc(fan_wp->poseNum * 0x48);
    memset(fan_wp->poses, 0, fan_wp->poseNum * 0x48);
    pose_init();
    pose = extGetPosePtr();
    fan = fan_wp->poses;
    for (i = 0; i < fan_wp->poseNum; i++, pose++, fan++) {
        Vec pos;
        Vec vel;
        s32 type;

        pose->flags |= 7;
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = 0.0f;
        (*pose->matrix)[1][3] = 0.0f;
        (*pose->matrix)[2][3] = 0.0f;
        pose->field_0x4 = randf(0.0f, 360.0f);
        type = irand(1000) % 3;
        switch (type) {
        case 0:
            pose_change(fan, "c_kinopio", "KNP_W_1");
            break;
        case 1:
            pose_change(fan, "c_kinopiko", "KNF_W_1");
            break;
        default:
            pose_change(fan, "c_jugemu", "JGM_W_1");
            break;
        }
        fan->field_0x2 = 0;
        pos.x = randf(-100.0f, 100.0f);
        pos.y = 0.0f;
        pos.z = randf(-100.0f, 100.0f);
        fan->position = pos;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        fan->velocity = vel;
    }
    {
        Vec pos;
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        fan_wp->sfxId = psndSFXOn_3D("SFX_STG3_AUDIENCE_FAN1", &pos);
        fan_wp->volume = psndSFX_get_vol(fan_wp->sfxId);
    }
}

void fan_disp(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

// NONMATCHING (85.65%): written from the asm; the check_hammer dispatch order and the
// register allocation of the case 1/2 trig blocks still differ.
#pragma opt_propagation off // hoist the sin/cos coefficient loads out of the inline branches
void ext_fan_main(void) {
    MarioWork* mario = marioGetPtr();
    FanPose* fan;
    ExtPose* pose;
    Vec d;
    Vec vec = { 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 j;

    if (mario->currMotionId == 0x12) {
        fan_wp->field_0x50C++;
    } else {
        fan_wp->field_0x50C = 0;
    }

    if (evtGetValue(NULL, -0x55C4A76)) {
        pose = extGetPosePtr();
        fan = fan_wp->poses;
        i = fan_wp->poseNum;
        while (i > 0) {
            if (fan->field_0x2 == 0) {
                fan->position = mario->position;
                vec = fan->position;
                pose->flags &= ~1;
                fan->flags |= 1;
                break;
            }
            fan++;
            pose++;
            i--;
        }
    } else {
        s32 found = 0;
        pose = extGetPosePtr();
        fan = fan_wp->poses;
        for (i = 0; i < fan_wp->poseNum; i++, fan++, pose++) {
            if (fan->field_0x2 != 0) {
                continue;
            }
            if (found == 0) {
                found = 1;
                fan->position = mario->position;
                pose->flags &= ~1;
                fan->flags |= 1;
            } else {
                NpcEntry* npc = npcNameToPtr("\x83\x60\x83\x83\x83\x93\x83\x73\x83\x87\x83\x93");
                fan->position = npc->position;
                vec = fan->position;
                pose->flags &= ~1;
                fan->flags |= 1;
                break;
            }
        }
    }

    fan = fan_wp->poses;
    for (i = 0; i < fan_wp->poseNum; i++, fan++) {
        if (fan->field_0x2 != 0) {
            continue;
        }
        PSVECSubtract(&vec, &fan->position, &d);
        if (PSVECMag(&d) != 0.0f) {
            PSVECNormalize(&d, &d);
            d.y = 0.0f;
            PSVECScale(&d, &d, 0.1f);
            PSVECAdd(&fan->velocity, &d, &fan->velocity);
        }
    }

    fan = fan_wp->poses;
    for (i = 0; i < fan_wp->poseNum; i++, fan++) {
        if (fan->field_0x2 != 0) {
            continue;
        }
        {
            FanPose* other = fan_wp->poses;
            for (j = 0; j < fan_wp->poseNum; j++, other++) {
                if (other->field_0x2 != 0) {
                    continue;
                }
                if (i == j) {
                    continue;
                }
                if (PSVECSquareDistance(&fan->position, &other->position) < 900.0f) {
                    PSVECSubtract(&fan->position, &other->position, &d);
                    d.y = 0.0f;
                    PSVECNormalize(&d, &d);
                    PSVECScale(&d, &d, 5.0f);
                    PSVECAdd(&fan->velocity, &d, &fan->velocity);
                    PSVECSubtract(&other->velocity, &d, &other->velocity);
                }
            }
        }
    }

    {
        const char* flySfx = "SFX_STG3_AUDIENCE_FAN_FLY1";
        fan = fan_wp->poses;
        for (i = 0; i < fan_wp->poseNum; i++, fan++) {
        if (fan->field_0x2 != 0) {
            continue;
        }
        switch (check_hammer(fan)) {
        case 1:
            fan->field_0x2 = 10;
            {
                f32 a = (TWO_PI * (135.0f + (f32)rand() / 32767.0f * 90.0f)) / 360.0f;
                f32 dist = 400.0f + (f32)rand() / 32767.0f * 100.0f;
                f32 s = fanSinf(a);
                f32 c;
                f32 vx, vz;

                vx = fan->position.x + dist * s;
                c = fanCosf(a);
                vz = fan->position.z - dist * c;
                {
                    Vec v = { 0.0f, 0.0f, 0.0f };
                    v.x = vx;
                    v.y = -100.0f;
                    v.z = vz;
                    fan->target = v;
                }
                fan->field_0x3C = 4.0f + (f32)rand() / 32767.0f * 2.0f;
                fan->field_0x40 = 0.1f + (f32)rand() / 32767.0f * 0.1f;
                {
                    f32 dy = fan->target.y - fan->position.y;
                    f32 dxy = distABf(fan->position.x, fan->position.z, fan->target.x, fan->target.z);
                    f32 ang = angleABf(fan->position.x, fan->position.z, fan->target.x, fan->target.z);

                    fan->field_0x44 = (s16)(dxy / fan->field_0x3C);
                    fan->field_0x38 = fan->field_0x40 * (f32)fan->field_0x44 * 0.5f + dy / (f32)fan->field_0x44;
                    (void)ang;
                }
                psndSFXOn_3D(flySfx, &fan->position);
            }
            break;
        case 2:
            {
                f32 ang = angleABf(mario->position.x, mario->position.z, fan->position.x, fan->position.z);
                f32 breath = cloudGetBreathDir();
                f32 a;

                fan->field_0x2 = 20;
                fan->target.x = 0.0f;
                fan->target.y = 0.0f;
                fan->target.z = 0.0f;
                a = (ang + breath) * 0.5f;
                a = TWO_PI * a / 360.0f;
                {
                    f32 scale = 0.1f + (f32)rand() / 32767.0f * 0.1f;
                    f32 s = fanSinf(a);
                    f32 c;

                    fan->field_0x38 = scale * s;
                    c = fanCosf(a);
                    fan->field_0x3C = 0.03f + (f32)rand() / 32767.0f * 0.03f;
                    fan->field_0x40 = -scale * c;
                    fan->field_0x44 = 120;
                }
                psndSFXOn_3D(flySfx, &fan->position);
            }
            break;
        default:
            break;
        }
        }
    }
    fan_wp->field_0x510 = fan_wp->poseNum;

    pose = extGetPosePtr();
    fan = fan_wp->poses;
    for (i = 0; i < fan_wp->poseNum; i++, fan++, pose++) {
        switch (fan->field_0x2) {
        case 0:
            PSVECNormalize(&fan->velocity, &fan->velocity);
            {
                f32 mag = PSVECMag(&fan->velocity);
                HitCheckQuery query;

                query.singleSided = 0;
                query.targetPos.x = fan->position.x;
                query.targetPos.y = fan->position.y + 4.0f;
                query.targetPos.z = fan->position.z;
                query.targetDir.x = fan->velocity.x;
                query.targetDir.y = 0.0f;
                query.targetDir.z = fan->velocity.z;
                query.targetDistance = 25.0f + mag;
                if (hitCheckVecFilter(&query, NULL) != NULL) {
                    C_VECReflect(&fan->velocity, &query.hitNormal, &fan->velocity);
                } else {
                    fan->position.x += fan->velocity.x;
                    fan->position.z += fan->velocity.z;
                    fan->velocity.x *= 0.6f;
                    fan->velocity.z *= 0.6f;
                }
            }
            if (fan->position.x < -350.0f) {
                fan->position.x = -350.0f;
            }
            if (fan->position.x > 350.0f) {
                fan->position.x = 350.0f;
            }
            if (fan->position.z > 195.0f + (f32)i / 100.0f) {
                fan->position.z = 195.0f + (f32)i / 100.0f;
            }
            break;
        case 10:
            fan->position.y += fan->field_0x38;
            fan->field_0x38 -= fan->field_0x40;
            {
                f32 ang = angleABf(fan->position.x, fan->position.z, fan->target.x, fan->target.z);
                f32 a = TWO_PI * ang / 360.0f;
                f32 s = fanSinf(a);
                f32 c;

                fan->position.x += fan->field_0x3C * s;
                c = fanCosf(a);
                fan->position.y += fan->field_0x3C * -c;
            }
            fan->field_0x44--;
            if (fan->field_0x44 < 0) {
                fan->field_0x2 = 100;
            }
            break;
        case 20:
            fan->field_0x44--;
            if (fan->field_0x44 > 0) {
                fan->target.x += fan->field_0x38;
                fan->target.y += fan->field_0x3C;
                fan->target.z += fan->field_0x40;
                fan->position.x += fan->target.x;
                fan->position.y += fan->target.y;
                fan->position.z += fan->target.z;
            } else {
                fan->field_0x2 = 100;
            }
            break;
        case 100:
            pose->flags &= ~3;
            fan->field_0x2++;
            break;
        default:
            break;
        }
        if (fan->flags & 1) {
            fan_wp->field_0x510--;
        } else if (fan->field_0x2 != 0) {
            fan_wp->field_0x510--;
        }
        (*pose->matrix)[0][3] = fan->position.x;
        (*pose->matrix)[1][3] = fan->position.y;
        (*pose->matrix)[2][3] = fan->position.z;
        pose->field_0x4 = angleABf(vec.x, vec.z, fan->position.x, fan->position.z);
    }
    psndSFX_pos(fan_wp->sfxId, &vec);
    psndSFX_vol(fan_wp->sfxId, (u8)(fan_wp->volume * fan_wp->field_0x510 / fan_wp->poseNum));
}
#pragma opt_propagation reset

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

static inline f32 fanSinf(f32 rad) {
    f32 f0, f1, sinv = 0.00761f;
    f32 c1 = 0.16605f;
    f32 c2 = 1.0f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        } else {
            f0 = rad - 1.5707964f;
            f1 = 1.5707964f - f0;
            f0 = f1 * f1;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * f1;
        }
    } else if (rad < 4.712389f) {
        f1 = rad - 3.1415927f;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    } else {
        f0 = rad - 4.712389f;
        f1 = 1.5707964f - f0;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 fanCosf(f32 rad) {
    f32 f0, cosv = 0.03705f;
    f32 c1 = 0.4967f;
    f32 c2 = 1.0f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
        } else {
            f0 = rad - 1.5707964f;
            f0 = 1.5707964f - f0;
            f0 = f0 * f0;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        f0 = rad - 3.1415927f;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = rad - 4.712389f;
        f0 = 1.5707964f - f0;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

// NONMATCHING (93.69%): the body is otherwise identical; only floating-point register
// allocation differs (the original keeps rad/s/coeffs in f6/f4/f3/f2/f1).
#pragma opt_propagation off // stop MWCC rematerialising the sin/cos coefficients per branch
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
        ang = reviseAngle(mario->unk1A8 - 90.0f + camGetPtr(4)->viewYaw);
        rad = TWO_PI * ang / 360.0f;
        s = fanSinf(rad);
        x = 35.0f * s + mario->position.x;
        c = fanCosf(rad);
        z = mario->position.z - 35.0f * c;
        if (distABf(pose->position.x, pose->position.z, x, z) < 35.0f) {
            return 1;
        }
    }
    if (mario->currMotionId == 0x13 && (s32)mario->currSubMotionId == 0x14) {
        if (PSVECDistance(&mario->position, &pose->position) < 65.0f) {
            return 1;
        }
    }
    party = partyGetPtr(0);
    if (party != NULL && party->currentMemberId == 5 && (party->flags & 0x100)) {
        Vec local;
        f32 dist;

        ang = TWO_PI * cloudGetBreathDir() / 360.0f;
        dist = cloudGetBreathDist();
        s = fanSinf(ang);
        local.x = party->position.x + dist * s;
        local.y = party->position.y;
        c = fanCosf(ang);
        local.z = party->position.z - dist * c;
        if (ang >= 0.0f) {
            if (PSVECDistance(&local, &pose->position) < 30.0f) {
                return 2;
            }
        }
    }
    return 0;
}
#pragma opt_propagation reset

USER_FUNC(evt_end_count) {
    evtSetValue(event, event->args[0], fan_wp->field_0x510);
    return EVT_RETURN_DONE;
}
// clang-format on

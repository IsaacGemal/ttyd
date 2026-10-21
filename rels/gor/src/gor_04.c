#include "gor_04.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "driver/envdrv.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(kpa_hikousen_yure);
static void resha_tev(void* param);
static USER_FUNC(ramp_r);
static s32 peach_mail[235];
NpcSetupInfo screamEnt[2];

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

// TODO: type
static u32 sharinDt[116] = {
    (u32)"sha_1_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_3", 0x00000002, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_4", 0x00000001, 0x00000001, 0xF24A7E80,
    (u32)"sha_1_5", 0x00000001, 0x00000001, 0xF24A7E80,
    (u32)"sha_2_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_2_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_2_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_3_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_3_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_3_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_4_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_4_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_4_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_5_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_5_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_5_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_1_c", 0x00000000, 0x00000000, 0xF24A7680,
    (u32)"sha_1_2_c", 0x00000000, 0x00000000, 0xF24A7680,
    (u32)"sha_1_3_c", 0x00000002, 0x00000000, 0xF24A7680,
    (u32)"sha_1_4_c", 0x00000001, 0x00000001, 0xF24A7680,
    (u32)"sha_1_5_c", 0x00000001, 0x00000001, 0xF24A7680,
    (u32)"sha_6_1_c", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_2_c", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_3_c", 0x00000000, 0x00000000, 0xF24A7E80,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(evt_sharin_main)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    SWITCH(LW(1))
        CASE_EQUAL(0)
        SET_FLOAT_VALUE(LW(11), FLOAT(23.89f))
        CASE_EQUAL(1)
        SET_FLOAT_VALUE(LW(11), FLOAT(31.14f))
        CASE_EQUAL(2)
        SET_FLOAT_VALUE(LW(11), FLOAT(55.02f))
    END_SWITCH
    LOOP(0)
        SWITCH(LW(2))
            CASE_EQUAL(0)
            SET_FLOAT_VALUE(LW(9), GW(0))
            CASE_EQUAL(1)
            SET_FLOAT_VALUE(LW(9), GW(1))
        END_SWITCH
        DIVIDE_FLOAT(LW(9), LW(11))
        MULTIPLY_FLOAT(LW(9), FLOAT(57.295f))
        ADD_FLOAT(LW(10), LW(9))
        IF_INT_GREATER(LW(10), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(0.0f))
            ADD_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        SET_FLOAT_VALUE(LW(12), LW(10))
        MULTIPLY_FLOAT(LW(12), LW(3))
        CALL(evt_mapobj_rotate, 1, LW(0), 0, 0, LW(12))
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_sharin)
    SET_INT_READ_ADDR(PTR(sharinDt))
    LOOP(0)
        READ_INT4(LW(0), LW(1), LW(2), LW(3))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        SPAWN_EVENT(evt_sharin_main)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 clankDt[45] = {
    (u32)"bou_1", 0x00000000, 0x00000000, 0xF24ACA80,
    0xF24A7E80, (u32)"bou_2", 0x00000001, 0x00000001,
    0xF24ABC80, 0xF24A7E80, (u32)"bou_3", 0x00000000,
    0x00000000, 0xF24ACA80, 0xF24A7E80, (u32)"bou_4",
    0x00000001, 0x00000001, 0xF24ABC80, 0xF24A7E80,
    (u32)"bou_1_c", 0x00000000, 0x00000000, 0xF24ACA80,
    0xF24A7680, (u32)"bou_2_c", 0x00000001, 0x00000001,
    0xF24ABC80, 0xF24A7680, (u32)"bou_3_c", 0x00000000,
    0x00000000, 0xF24ACA80, 0xF24A7680, (u32)"bou_4_c",
    0x00000001, 0x00000001, 0xF24ABC80, 0xF24A7680,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(evt_clank_main)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    SWITCH(LW(1))
        CASE_EQUAL(0)
        SET_FLOAT_VALUE(LW(11), FLOAT(23.89f))
        CASE_EQUAL(1)
        SET_FLOAT_VALUE(LW(11), FLOAT(31.14f))
        CASE_EQUAL(2)
        SET_FLOAT_VALUE(LW(11), FLOAT(55.02f))
    END_SWITCH
    LOOP(0)
        SWITCH(LW(2))
            CASE_EQUAL(0)
            SET_FLOAT_VALUE(LW(9), GW(0))
            CASE_EQUAL(1)
            SET_FLOAT_VALUE(LW(9), GW(1))
        END_SWITCH
        DIVIDE_FLOAT(LW(9), LW(11))
        MULTIPLY_FLOAT(LW(9), FLOAT(57.295f))
        ADD_FLOAT(LW(10), LW(9))
        IF_INT_GREATER(LW(10), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(0.0f))
            ADD_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        SET_FLOAT_VALUE(LW(12), LW(10))
        MULTIPLY_FLOAT(LW(12), LW(4))
        CALL(evt_sub_get_sincos, LW(12), LW(13), LW(14))
        MULTIPLY_FLOAT(LW(13), LW(3))
        MULTIPLY_FLOAT(LW(14), LW(3))
        SUBTRACT_FLOAT(LW(14), LW(3))
        MULTIPLY_FLOAT(LW(14), FLOAT(-1.0f))
        CALL(evt_mapobj_trans, 1, LW(0), LW(14), LW(13), FLOAT(0.0f))
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_clank)
    SET_INT_READ_ADDR(PTR(clankDt))
    LOOP(0)
        READ_INT4(LW(0), LW(1), LW(2), LW(3))
        READ_INT(LW(4))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        SPAWN_EVENT(evt_clank_main)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_bg)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(11), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), GW(0))
        ADD_FLOAT(LW(10), LW(0))
        IF_INT_GREATER(LW(10), FLOAT(800.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(1600.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(-800.0f))
            ADD_FLOAT(LW(10), FLOAT(1600.0f))
        END_IF
        ADD_FLOAT(LW(11), LW(0))
        IF_INT_GREATER(LW(11), FLOAT(2400.0f))
            SUBTRACT_FLOAT(LW(11), FLOAT(4800.0f))
        END_IF
        IF_INT_LESS(LW(11), FLOAT(-2400.0f))
            ADD_FLOAT(LW(11), FLOAT(4800.0f))
        END_IF
        CALL(evt_mapobj_trans, 1, STRING("jimen"), LW(10), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("yama"), LW(10), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("iwayama"), LW(11), 0, 0)
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_hoshi)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(9), GW(0))
        DIVIDE_FLOAT(LW(9), FLOAT(30.0f))
        MULTIPLY_FLOAT(LW(9), FLOAT(57.295f))
        ADD_FLOAT(LW(10), LW(9))
        IF_INT_GREATER(LW(10), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(0.0f))
            ADD_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        CALL(evt_mapobj_rotate, 0, STRING("hosi_right"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_left"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_center"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_right_c"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_left_c"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_center_c"), LW(10), 0, 0)
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start)
    SET_VALUE(GSWF(3583), 1)
    SET_VALUE(GSWF(3579), 0)
    SET_VALUE(GSWF(3580), 0)
    SET_VALUE(GSWF(3581), 0)
    CALL(evt_mapobj_get_position, STRING("ranpu"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        IF_INT_LESS(GSW(0), 339)
            CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_2"), 0)
        ELSE
            CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_2_2"), 0)
        END_IF
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_3"), 0)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
        CALL(evt_sub_intpl_msec_init, 11, 0, -1500, 20000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(0), FLOAT(-15.0f))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            CALL(evt_sub_get_stopwatch, LW(0), LW(1))
            IF_INT_GREATER(LW(1), 1500)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, 0, -1000, 1000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        CALL(evt_sub_intpl_msec_init, 11, -1000, 0, 1000)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start2)
    SET_VALUE(GSWF(3583), 1)
    SET_VALUE(GSWF(3579), 0)
    SET_VALUE(GSWF(3580), 0)
    SET_VALUE(GSWF(3581), 0)
    CALL(evt_mapobj_get_position, STRING("ranpu"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_2_3"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_3"), 0)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
        CALL(evt_sub_intpl_msec_init, 11, 0, 1500, 20000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(0), FLOAT(15.0f))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            CALL(evt_sub_get_stopwatch, LW(0), LW(1))
            IF_INT_GREATER(LW(1), 1500)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, 0, 1000, 1000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        CALL(evt_sub_intpl_msec_init, 11, 1000, 0, 1000)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start3)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(-15.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(-15.0f))
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start4)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(15.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(15.0f))
    RETURN
EVT_END()

static EVT_BEGIN(stop_1)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(3000.0f))
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_3"), 0)
    END_IF
    CALL(evt_sub_intpl_msec_init, 11, 3000, 0, 10000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    MULTIPLY_FLOAT(LW(0), FLOAT(1.0f))
    CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
    SUBTRACT_FLOAT(LW(2), LW(0))
    SET_FLOAT_VALUE(GW(0), LW(2))
    SET_FLOAT_VALUE(GW(1), LW(2))
    MULTIPLY_FLOAT(GW(0), FLOAT(-1.0f))
    MULTIPLY_FLOAT(GW(1), FLOAT(-1.0f))
    SET_FLOAT_VALUE(LW(2), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GSWF(3583), 0)
    RETURN
EVT_END()

static EVT_BEGIN(stop_2)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(-3000.0f))
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_2_2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_3"), 0)
    END_IF
    CALL(evt_sub_intpl_msec_init, 11, -3000, 0, 10000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    MULTIPLY_FLOAT(LW(0), FLOAT(1.0f))
    CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
    SUBTRACT_FLOAT(LW(2), LW(0))
    SET_FLOAT_VALUE(GW(0), LW(2))
    SET_FLOAT_VALUE(GW(1), LW(2))
    MULTIPLY_FLOAT(GW(0), FLOAT(-1.0f))
    MULTIPLY_FLOAT(GW(1), FLOAT(-1.0f))
    SET_FLOAT_VALUE(LW(2), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GSWF(3583), 0)
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_stop)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        SPAWN_EVENT(stop_1)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        SPAWN_EVENT(stop_2)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        IF_INT_EQUAL(GSWF(3578), 0)
            SPAWN_EVENT(stop_1)
        ELSE
            SPAWN_EVENT(stop_2)
        END_IF
    END_IF
    RETURN
EVT_END()

#include "rel/common/ramp_r.h"

static EVT_BEGIN(evt_ramp)
    LOOP(0)
        CALL(ramp_r)
        CALL(evt_mapobj_rotate, 1, STRING("ranpu"), 0, 0, LW(10))
        CALL(evt_mapobj_rotate, 1, STRING("ranpu_c"), 0, 0, LW(10))
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static char* pika_tbl[15] = {
    "pika_1_1", "pika_1_2", "pika_1_3", "pika_1_4",
    "pika_2", "pika_3", "pika_4", "pika_5",
    "pika_6", "pika_1_1_c", "pika_1_2_c", "pika_1_3_c",
    "pika_1_4_c", "pika_6_c", NULL,
};

static char* pika_tbl_rsh_06[10] = {
    "pika_1_1", "pika_1_2", "pika_1_3", "pika_1_4",
    "pika_2", "pika_3", "pika_4", "pika_5",
    "pika_6", NULL,
};

#include "rel/common/resha_tev.h"

static EVT_BEGIN(hom_10_evt_resha_sub)
    SET_VALUE(GSWF(3583), 0)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SPAWN_EVENT(evt_sharin)
    SPAWN_EVENT(evt_clank)
    SET_VALUE(LF(0), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_10"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_11"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_12"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("rsh_06_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        SPAWN_EVENT(evt_bg)
    END_IF
    SPAWN_EVENT(evt_hoshi)
    SPAWN_EVENT(evt_ramp)
    CALL(evt_map_set_tevcallback, 1, PTR(resha_tev))
    SET_INT_READ_ADDR(PTR(pika_tbl))
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("rsh_06_a"))
        SET_INT_READ_ADDR(PTR(pika_tbl_rsh_06))
    END_IF
    LOOP(0)
        READ_INT(LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 16777216)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 256)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 268435456)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_right)
    CALL_CHILD(hom_10_evt_resha_sub)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_NOT_EQUAL(LW(0), STRING("rsh_06_a"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_7"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_8"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_6"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_7"), 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_left)
    CALL_CHILD(hom_10_evt_resha_sub)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_6"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_5"), 1)
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_init_evt)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH2"))
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    IF_INT_EQUAL(GSWF(3578), 0)
        CALL_CHILD(hom_10_evt_resha_right)
        CALL_CHILD(hom_10_evt_resha_start3)
        SET_VALUE(LW(0), 500)
        SET_VALUE(LW(1), -500)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_3"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_4"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_5"), 1)
    ELSE
        CALL_CHILD(hom_10_evt_resha_left)
        CALL_CHILD(hom_10_evt_resha_start4)
        SET_VALUE(LW(0), 0)
        SET_VALUE(LW(1), 1000)
        CALL(evt_mapobj_trans, 1, STRING("train"), -1500, 0, 0)
    END_IF
    CALL(evt_cam_letter_box_onoff, 1, 1)
    CALL(evt_cam3d_evt_set, LW(0), 300, 1400, LW(0), 125, 0, 0, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_seq_wait, 2)
        CALL(evt_cam3d_evt_set, LW(1), 300, 1400, LW(1), 125, 0, 8000, 0)
        WAIT_MS(4000)
        CALL(evt_bero_mapchange, STRING("rsh_02_a"), STRING("nidome"))
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(evt_train)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(10))
    IF_INT_GREATER(LW(10), 100)
        CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -40, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("tobira"), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_hitobj_onoff, STRING("n_bero"), 1, 0)
    CALL(evt_mario_mov_pos2, -360, 70, FLOAT(60.0f))
    CALL(evt_mario_mov_pos2, -370, 18, FLOAT(60.0f))
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_move_pos2, 0, -350, 22, FLOAT(60.0f))
    CALL(evt_party_set_dir, 0, 270, 300)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_party_move_pos2, 1, -360, 26, FLOAT(60.0f))
    CALL(evt_party_set_dir, 1, 270, 300)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, -40, 0, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("tobira"), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    WAIT_MS(500)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("飛行船係員"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("ジュゲムＡ"), 32768)
    CALL(evt_cam3d_evt_set, 668, 95, 290, 361, 95, 121, 0, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("train_07"), 0, STRING("駅員"))
    WAIT_MS(500)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG4_TRAIN1"))
    SPAWN_EVENT(hom_10_evt_resha_start)
    WAIT_MS(4000)
    CALL(evt_cam3d_evt_set, 410, 90, 571, 410, 90, 121, 2000, 12)
    WAIT_MS(9500)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 345)
        SET_VALUE(GSWF(3456), 1)
    END_IF
    SET_VALUE(GSWF(3578), 0)
    SET_VALUE(GSWF(3582), 1)
    WAIT_FRAMES(1)
    SET_VALUE(GSW(870), 0)
    SET_VALUE(GSWF(3461), 0)
    SET_VALUE(GSWF(3460), 0)
    SET_VALUE(GSWF(3581), 0)
    SET_VALUE(GSWF(3580), 0)
    IF_INT_LESS(GSW(0), 282)
        CALL(evt_bero_mapchange, STRING("sys_01"), STRING("stg_6"))
    ELSE
        CALL(evt_bero_mapchange, STRING("rsh_02_a"), STRING("nidome_start"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_train2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, -360, 232, 603, -360, 137, 62, 0, 11)
    CALL(evt_npc_flag_onoff, 1, STRING("駅員"), 32768)
    CALL(evt_npc_set_position, STRING("駅員"), -400, 80, 100)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    LOOP(0)
        IF_INT_EQUAL(GW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_flag_onoff, 0, STRING("駅員"), 32768)
    CALL(evt_mario_set_pos, -350, 80, 30)
    CALL(evt_party_set_pos, 0, -370, 80, 30)
    CALL(evt_party_set_pos, 1, -360, 80, 20)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_cont_onoff, 1, 1)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_party_set_dir, 1, 270, 0)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -40, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("tobira"), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -345, 85, FLOAT(80.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -330, 100, FLOAT(80.0f))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, -40, 0, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("tobira"), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(GSW(0), 345)
        SET_VALUE(GSW(0), 345)
    END_IF
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_party_run, 1)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        IF_INT_NOT_EQUAL(LW(0), LW(3))
            BREAK_LOOP
        END_IF
        IF_INT_NOT_EQUAL(LW(1), LW(4))
            BREAK_LOOP
        END_IF
        IF_INT_NOT_EQUAL(LW(2), LW(5))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_cam3d_evt_off, 1000, 11)
    SET_VALUE(GSWF(3461), 1)
    SET_VALUE(GSWF(3460), 0)
    SET_VALUE(GSWF(3581), 1)
    IF_INT_LESS(GSW(0), 346)
        SPAWN_EVENT(peach_mail)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(peach_mail)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), -275)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION1"))
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), -35)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
        CALL(evt_party_set_dir, 0, 90, -1)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print_party, STRING("gor_04_mail6_00"))
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
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_mario_set_dir, 90, 300, 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), -20)
        ADD(LW(2), 10)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
        CALL(evt_party_set_dir, 0, 90, -1)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print, 0, STRING("gor_04_mail6_01"), 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("gor_04_mail6_02"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 346)
    CALL(evt_pouch_receive_mail, 5)
    CALL(evt_pouch_open_mail, 5)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[5] = {
    {"s_bero", 0, 0, 0x00004E20, 0x000186A0, 0, 0, 0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "gor_03", "n_bero", 0x0001, 0x0001, 0, 0},
    {"dokan_1", 0x0002, 0, 0x00000009, 0x000186A0, 0, 0, 0xFFFFFFFF, NULL, 0x00000006, NULL, NULL, "A_dokan_h00", 0x0001, 0x0001, 0, 0},
    {"A_dokan_h00", 0x1002, 0, 0x00000009, 0x000186A0, 0, 0, 0xFFFFFFFF, NULL, 0x00000006, NULL, NULL, "dokan_1", 0x0001, 0x0001, 0, 0},
    {"n_bero", 0x0001, 0x0008, 0x00004E20, 0x000186A0, 0, 0, 0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "rsh_01_a", "s_bero", 0x0001, 0x0001, "n_1_door_close", "n_1_door_open"},
    {NULL},
};

EVT_BEGIN(hikosenkakari_init)
    SET_VALUE(GF(10), 0)
    RETURN
EVT_END()

EVT_BEGIN(hikosenkakari_init_ship)
    CALL(evt_npc_set_position, STRING("飛行船係員"), 315, 0, 160)
    CALL(evt_hitobj_onoff, STRING("A_tousenbo"), 1, 0)
    SET_VALUE(GF(10), 1)
    RETURN
EVT_END()

EVT_BEGIN(hikosenkakari_talk)
    IF_INT_EQUAL(GF(10), 1)
        CALL(evt_msg_print, 0, STRING("hikou_06"), 0, STRING("me"))
        RETURN
    END_IF
    CALL(evt_msg_print, 0, STRING("hikou_00"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("hikou_01"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("hikou_04"))
        CALL(evt_pouch_check_item, 52, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("hikou_05"), 0, STRING("me"))
        ELSE
            CALL(evt_party_stop, 0)
            CALL(evt_party_stop, 1)
            CALL(evt_mario_dispflag_onoff, 1, 2)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_mario_get_dispdir, LW(10))
            IF_INT_GREATER_OR_EQUAL(LW(10), 180)
                ADD(LW(0), 15)
            ELSE
                SUBTRACT(LW(0), 15)
            END_IF
            ADD(LW(1), 20)
            ADD(LW(2), 10)
            SET_VALUE(LW(10), STRING("airship"))
            CALL(evt_item_entry, LW(10), 52, LW(0), LW(1), LW(2), 16, -1, 0)
            CALL(evt_item_flag_on, LW(10), 2)
            WAIT_MS(1000)
            CALL(evt_item_delete, LW(10))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_mario_dispflag_onoff, 0, 2)
            CALL(evt_msg_print, 0, STRING("hikou_02"), 0, STRING("me"))
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
            SET_VALUE(GF(0), 1)
            SET_VALUE(GF(1), 1)
            BEGIN_INLINE_EVENT
                CALL(evt_npc_move_position, STRING("me"), 315, 160, 0, FLOAT(120.0f), 0)
            END_INLINE_EVENT
            BEGIN_INLINE_EVENT
                CALL(evt_party_move_pos2, 0, 235, 140, FLOAT(120.0f))
                CALL(evt_party_set_dir, 0, 90, 200)
                SET_VALUE(GF(0), 0)
            END_INLINE_EVENT
            BEGIN_INLINE_EVENT
                CALL(evt_party_move_pos2, 1, 195, 140, FLOAT(120.0f))
                CALL(evt_party_set_dir, 1, 90, 200)
                SET_VALUE(GF(1), 0)
            END_INLINE_EVENT
            CALL(evt_mario_mov_pos2, 285, 140, FLOAT(120.0f))
            CALL(evt_mario_set_dir, 90, 200, 0)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
            CALL(evt_hitobj_onoff, STRING("A_tousenbo"), 1, 0)
            SET_VALUE(GF(10), 1)
            WAIT_WHILE(GF(0))
            WAIT_WHILE(GF(1))
            CALL(evt_party_run, 0)
            CALL(evt_party_run, 1)
        END_IF
    ELSE
        CALL(evt_msg_print_add, 0, STRING("hikou_03"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ekiin_init)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("resha_return"))
        CALL(evt_npc_set_position, STRING("me"), -400, 80, 100)
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("n_bero"))
        CALL(evt_npc_set_position, STRING("me"), -400, 80, 100)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ekiin_talk)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("resha_return"))
        CALL(evt_msg_print, 0, STRING("train_06"), 0, STRING("me"))
        RETURN
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("n_bero"))
        CALL(evt_msg_print, 0, STRING("train_06"), 0, STRING("me"))
        RETURN
    END_IF
    CALL(evt_msg_print, 0, STRING("train_00"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("train_01"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("train_03"))
        CALL(evt_pouch_check_item, 53, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("train_05"), 0, STRING("me"))
        ELSE
            CALL(evt_mario_dispflag_onoff, 1, 2)
            CALL(evt_mario_set_pose, STRING("M_C_3"))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_mario_get_dispdir, LW(10))
            IF_INT_GREATER_OR_EQUAL(LW(10), 180)
                ADD(LW(0), 10)
            ELSE
                SUBTRACT(LW(0), 10)
            END_IF
            ADD(LW(1), 20)
            ADD(LW(2), 10)
            SET_VALUE(LW(10), STRING("train"))
            CALL(evt_item_entry, LW(10), 53, LW(0), LW(1), LW(2), 16, -1, 0)
            CALL(evt_item_flag_on, LW(10), 2)
            WAIT_MS(1000)
            CALL(evt_item_delete, LW(10))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_mario_dispflag_onoff, 0, 2)
            CALL(evt_msg_print, 0, STRING("train_04"), 0, STRING("me"))
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
            CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
            CALL(evt_snd_bgmoff_f, 512, 2000)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(0), -350)
                CALL(evt_npc_move_position, STRING("me"), -390, 80, 0, FLOAT(120.0f), 0)
            ELSE
                CALL(evt_npc_move_position, STRING("me"), -320, 80, 0, FLOAT(120.0f), 0)
            END_IF
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
            CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
            CALL_CHILD(evt_train)
        END_IF
    ELSE
        CALL(evt_msg_print_add, 0, STRING("train_02"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(jugemu_a_init)
    IF_INT_EQUAL(GSW(0), 215)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("me"), -80, 80, 85)
    RETURN
EVT_END()

EVT_BEGIN(jugemu_a_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_04_000"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_04_001"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_04_002"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_04_003"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_04_004"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_04_005"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_04_006"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_04_007"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(kinopiko_b_init)
    CALL(evt_npc_set_position, STRING("me"), -575, 80, 120)
    RETURN
EVT_END()

static EVT_BEGIN(kinopiko_b_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_04_008"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_04_009"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_04_010"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_04_011"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_04_012"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_04_013"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_04_014"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_04_015"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(baba_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

static EVT_BEGIN(hkoopa_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

static EVT_BEGIN(luigi_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(339, 372)
        CALL(evt_npc_set_position, STRING("me"), 65, 0, 120)
        CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(luigi_talk)
    CALL_CHILD(luigi_gor_first_talk)
    RETURN
EVT_END()

static NpcSetupInfo luigiEnt[2] = {
    {"ルイージ", 0x00080600, 0, (void*)&luigi_init, NULL, (void*)&luigi_talk},
    {NULL},
};

static EVT_BEGIN(luigi_npcEnt)
    SWITCH(GSW(0))
        CASE_BETWEEN(339, 372)
        CALL(evt_npc_entry, STRING("ルイージ"), STRING("c_luigi"))
        CALL(evt_npc_set_tribe, STRING("ルイージ"), STRING("ルイージ"))
        CALL(evt_npc_setup, PTR(luigiEnt))
        CALL(evt_npc_entry, STRING("ス・クリーミ"), STRING("c_lp_moa"))
        CALL(evt_npc_set_tribe, STRING("ス・クリーミ"), STRING("ス・クリーミ"))
        CALL(evt_npc_setup, PTR(screamEnt))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(scream_init)
    CALL(evt_npc_set_position, STRING("me"), 105, 0, 120)
    CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(scream_talk)
    CALL(evt_msg_print, 0, STRING("luigi_npc_05"), 0, STRING("me"))
    RETURN
EVT_END()

NpcSetupInfo screamEnt[2] = {
    {"ス・クリーミ", 0x00080600, 0, (void*)&scream_init, NULL, (void*)&scream_talk},
    {NULL},
};

static NpcSetupInfo npcEnt[7] = {
    {"飛行船係員", 0x00080600, 0, (void*)&hikosenkakari_init, NULL, (void*)&hikosenkakari_talk},
    {"駅員", 0x00080600, 0, (void*)&ekiin_init, NULL, (void*)&ekiin_talk},
    {"ジュゲムＡ", 0x00080600, 0, (void*)&jugemu_a_init, NULL, (void*)&jugemu_a_talk},
    {"キノピオ女Ｂ", 0x00080600, 0, (void*)&kinopiko_b_init, NULL, (void*)&kinopiko_b_talk},
    {"ババ", 0x00000600, 0, (void*)&baba_init, NULL, NULL},
    {"飛クッパ", 0x00000600, 0, (void*)&hkoopa_init, NULL, NULL},
    {NULL},
};

EVT_BEGIN(evt_hikousen)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG3_AIRSHIP1"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_10"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_9"), 1)
        WAIT_MS(500)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_7"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_8"), 1)
        WAIT_MS(500)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("h_bou_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("h_bou_4"), 1)
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("gondora_1"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AIR_SHIP_GONDOLA_MOVE1"), LW(10), LW(11), LW(12), LW(6))
    CALL(evt_hit_bind_mapobj, STRING("A_noriba"), STRING("gondora_1"))
    CALL(evt_sub_intpl_msec_init, 11, 0, 90, 2000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("gondora_1"), 0, LW(0), -2)
    CALL(evt_hit_bind_update, STRING("A_noriba"))
    CALL(evt_mapobj_get_position, STRING("gondora_1"), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfx_pos, LW(6), LW(7), LW(8), LW(9))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(6))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gondora_1"), 1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    BEGIN_INLINE_EVENT
        IF_INT_LESS(GSW(0), 127)
            WAIT_MS(4000)
            CALL(evt_bero_mapchange, STRING("tou_20"), STRING("start"))
        ELSE
            WAIT_MS(10000)
            CALL(evt_bero_mapchange, STRING("tou_00"), 0)
        END_IF
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(2), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(3), FLOAT(0.0f))
    CALL(evt_snd_sfxon, STRING("SFX_STG3_AIR_SHIP_DRIVE2"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 12, 0, 30000, 15000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(2), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 12, 0, 10000, 15000)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(3), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_mapobj_trans, 1, STRING("hikousen"), GW(2), GW(3), 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_wing)
    LOOP(0)
        CALL(evt_sub_random, 100, LW(0))
        IF_INT_LESS(LW(0), 20)
            LOOP(3)
                CALL(evt_sub_intpl_init, 12, -10, 10, 10)
                LOOP(11)
                    CALL(evt_sub_intpl_get_value)
                    CALL(evt_mapobj_rotate, 1, STRING("hane"), 0, 0, LW(0))
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(evt_sub_intpl_init, 12, 10, -10, 10)
                LOOP(11)
                    CALL(evt_sub_intpl_get_value)
                    CALL(evt_mapobj_rotate, 1, STRING("hane"), 0, 0, LW(0))
                    WAIT_FRAMES(1)
                END_LOOP
            END_LOOP
        ELSE
            CALL(evt_sub_intpl_init, 12, -10, 10, 30)
            LOOP(31)
                CALL(evt_sub_intpl_get_value)
                CALL(evt_mapobj_rotate, 1, STRING("hane"), 0, 0, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init, 12, 10, -10, 30)
            LOOP(31)
                CALL(evt_sub_intpl_get_value)
                CALL(evt_mapobj_rotate, 1, STRING("hane"), 0, 0, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_hikousen_return)
    SPAWN_EVENT(hikosenkakari_init_ship)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_bgmode_on)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gondora_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hako_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gondora_2"), 1)
    CALL(evt_cam3d_evt_set, 446, 181, 658, 446, 86, -117, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_AIR_SHIP_DRIVE5"), 0)
    CALL(evt_cam_letter_box_onoff, 1, 1)
    CALL(evt_seq_wait, 2)
    SET_VALUE(GF(0), 0)
    SET_FLOAT_VALUE(GW(2), FLOAT(3000.0f))
    SET_FLOAT_VALUE(GW(3), FLOAT(1000.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("hikousen"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_AIR_SHIP_DRIVE1"), LW(0), LW(1), LW(2), LW(10))
        WAIT_MS(15000)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 4000, 0, 15000)
        LABEL(10)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(2), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(10)
        END_IF
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 1000, 0, 15000)
        LABEL(20)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_VALUE(GW(3), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(20)
        END_IF
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_mapobj_trans, 1, STRING("hikousen"), GW(2), GW(3), 0)
        CALL(evt_mapobj_rotate, 1, STRING("hikousen"), 0, 180, 0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_paper_entry, STRING("OFF_d_kaiten"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_kaiten"))
    CALL(evt_offscreen_entry, STRING("ofs"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("hikousen"), STRING("ofs"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("gondora_1"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("gondora_2"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("ofs"), 0, 0, 0, 0, 0)
    CALL(evt_img_alloc_capture, STRING("img"), 1, STRING("ofs"), 1, 0, 0, 0, 0)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hikousen"), 1)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_AIR_SHIP_REVERSE1"), 0)
    CALL(evt_img_onoff, STRING("img"), 1)
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_offscreen_delete, STRING("ofs"))
    CALL(evt_paper_delete, STRING("OFF_d_kaiten"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("hikousen"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("hikousen"), 1)
    CALL(evt_mapobj_rotate, 1, STRING("hikousen"), 0, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("gondora_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("hako_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_10"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_9"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_7"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bou_8"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("h_bou_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("h_bou_4"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gondora_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hako_1"), 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("h_bou_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("h_bou_4"), 1)
        WAIT_MS(700)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bou_7"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bou_8"), 1)
        WAIT_MS(700)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bou_9"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bou_10"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff_f, 512, 3000)
        WAIT_MS(2000)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    END_INLINE_EVENT
    CALL(evt_mario_set_pos, 350, 135, -352)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mapobj_get_position, STRING("gondora_1"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AIR_SHIP_GONDOLA_MOVE1"), LW(10), LW(11), LW(12), LW(6))
    CALL(evt_hit_bind_mapobj, STRING("A_noriba"), STRING("gondora_1"))
    CALL(evt_sub_intpl_msec_init, 11, 900, 0, 3000)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_trans, 1, STRING("gondora_1"), 0, LW(0), -2)
    CALL(evt_hit_bind_update, STRING("A_noriba"))
    CALL(evt_mapobj_get_position, STRING("gondora_1"), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfx_pos, LW(6), LW(7), LW(8), LW(9))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_snd_sfxoff, LW(6))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 30)
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(80.0f))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static USER_FUNC(kpa_hikousen_yure) {
    f32 value = evtGetFloat(event, event->args[0]);
    npcNameToPtr("飛クッパ")->moveOffset.y = value;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(gor_04_koopa_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 425, 250, 340, 425, 214, 112, 0, 11)
    CALL(evt_mario_set_pos, 75, 0, 154)
    CALL(evt_npc_set_position, STRING("ババ"), 170, 25, 154)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_npc_set_ry, STRING("ババ"), 270)
    CALL(evt_seq_wait, 2)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 128, 163, 666, 128, 71, 113, 2000, 11)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("kpa_stg4_000"), 0, STRING("ババ"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 280, 153, 657, 280, 57, 116, 1600, 11)
    CALL(evt_npc_kamek_move_position, STRING("ババ"), 251, 128, 5, 1600, 0, 0)
    SET_VALUE(GF(0), 1)
    CALL(evt_cam3d_evt_zoom_in, 20, 800, 0, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("kpa_stg4_001"), 0, STRING("ババ"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_002"), 0, STRING("飛行船係員"))
    CALL(evt_npc_get_position, STRING("飛行船係員"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(0))
    ADD(LW(3), 1)
    LOOP(10)
        CALL(evt_npc_set_position, STRING("飛行船係員"), LW(3), LW(1), LW(2))
        SUBTRACT(LW(3), 2)
        WAIT_MS(50)
        CALL(evt_npc_set_position, STRING("飛行船係員"), LW(3), LW(1), LW(2))
        ADD(LW(3), 2)
        WAIT_MS(50)
    END_LOOP
    CALL(evt_npc_set_position, STRING("飛行船係員"), LW(0), LW(1), LW(2))
    CALL(evt_cam3d_evt_set, 337, 134, 549, 337, 58, 116, 500, 11)
    CALL(evt_npc_sound_data_set, STRING("飛行船係員"), STRING("SFX_FLD_NPC_PUKUPUKUI_MOVE1"), STRING("SFX_FLD_NPC_PUKUPUKUI_MOVE1"), 12000, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(evt_npc_get_position, STRING("飛行船係員"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_PUKU_ESCAPE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_status_onoff, 1, STRING("飛行船係員"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("飛行船係員"), 1073741824)
    CALL(evt_npc_move_position, STRING("飛行船係員"), 395, 130, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_move_position, STRING("飛行船係員"), 405, 110, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_jump_position_nohit, STRING("飛行船係員"), 415, 15, 38, 400, 40)
    CALL(evt_npc_move_position, STRING("飛行船係員"), 550, 38, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_status_onoff, 0, STRING("飛行船係員"), 2)
    CALL(evt_npc_flag_onoff, 0, STRING("飛行船係員"), 1073741824)
    CALL(evt_cam3d_evt_set, 273, 134, 549, 273, 58, 116, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("kpa_stg4_003"), 0, STRING("ババ"))
    CALL(evt_npc_set_ry, STRING("ババ"), 270)
    CALL(evt_mario_set_pos, -222, 80, 137)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 102, 163, 666, 102, 71, 113, 2000, 11)
    CALL(evt_npc_kamek_move_position, STRING("ババ"), 155, 154, 5, 2000, 0, 0)
    SET_VALUE(GF(0), 1)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ババ"), 1, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("kpa_stg4_004"), 0, STRING("ババ"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_kamek_move_position, STRING("ババ"), 105, 154, 5, 0, FLOAT(30.0f), 0)
    SET_VALUE(GF(0), 1)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_ry, STRING("ババ"), 90)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ババ"))
    WAIT_MS(100)
    CALL(evt_npc_set_ry, STRING("ババ"), 270)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ババ"))
    WAIT_MS(100)
    CALL(evt_npc_set_ry, STRING("ババ"), 90)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ババ"))
    WAIT_MS(100)
    CALL(evt_npc_set_ry, STRING("ババ"), 270)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ババ"))
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_AIRSHIP_MOVE1"), 200, 50, 100, LW(15))
    WAIT_MS(1000)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ババ"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("飛クッパ"), 200, 115, 500)
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(1), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_sub_get_sincos, LW(0), LW(2), LW(3))
            MULTIPLY_FLOAT(LW(2), FLOAT(3.0f))
            SET_FLOAT_VALUE(LW(3), LW(2))
            SUBTRACT_FLOAT(LW(3), LW(1))
            CALL(kpa_hikousen_yure, LW(3))
            WAIT_FRAMES(1)
            ADD_FLOAT(LW(0), FLOAT(2.0f))
            SET_FLOAT_VALUE(LW(1), LW(2))
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("ババ"), 90)
        WAIT_MS(500)
        CALL(evt_npc_set_ry, STRING("ババ"), 270)
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("飛クッパ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(15), LW(0), 50, 100)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("飛クッパ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_LAUGH2_2"), LW(0), LW(1), LW(2), LW(15))
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_snd_sfxchk, LW(15), LW(0))
            IF_INT_EQUAL(LW(0), -1)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("飛クッパ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(15), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("飛クッパ"), -80, 550, 0, FLOAT(120.0f), 0)
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("飛クッパ"), -80, 50, 154)
    CALL(evt_npc_set_ry, STRING("飛クッパ"), 90)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("飛クッパ"), 45, 154, 0, FLOAT(120.0f), 0)
    SET_VALUE(GF(0), 1)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 65, 214, 670, 65, 125, 115, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("kpa_stg4_005"), 0, STRING("飛クッパ"))
    SET_VALUE(GF(1), 1)
    CALL(evt_npc_get_position, STRING("飛クッパ"), LW(2), LW(3), LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_AIRSHIP_MOVE1"), LW(2), LW(3), LW(4), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 0, 1000, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    SET_FLOAT_VALUE(LW(5), FLOAT(-50.0f))
    MULTIPLY_FLOAT(LW(5), LW(0))
    SET_FLOAT_VALUE(LW(6), FLOAT(100.0f))
    MULTIPLY_FLOAT(LW(6), LW(0))
    SET_FLOAT_VALUE(LW(7), FLOAT(-100.0f))
    MULTIPLY_FLOAT(LW(7), LW(0))
    ADD_FLOAT(LW(5), LW(2))
    ADD_FLOAT(LW(6), LW(3))
    ADD_FLOAT(LW(7), LW(4))
    CALL(evt_npc_set_position, STRING("飛クッパ"), LW(5), LW(6), LW(7))
    CALL(evt_snd_sfx_pos, LW(9), LW(5), LW(6), LW(7))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_pera_onoff, STRING("ババ"), 0)
        SET_FLOAT_VALUE(LW(0), FLOAT(-18.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(1.0f))
        CALL(evt_npc_get_dir, STRING("ババ"), LW(3))
        CALL(evt_npc_get_position, STRING("ババ"), LW(4), LW(5), LW(6))
        CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_ROUND1"), LW(4), LW(5), LW(6), LW(7))
        LOOP(3)
            LOOP(60)
                ADD_FLOAT(LW(3), LW(0))
                CALL(evt_npc_set_ry, STRING("ババ"), LW(3))
                WAIT_FRAMES(1)
            END_LOOP
            ADD_FLOAT(LW(0), LW(1))
        END_LOOP
        MODULO(LW(3), 360)
        ADD(LW(3), 360)
        SET_VALUE(LW(4), 0)
        LOOP(0)
            ADD_FLOAT(LW(3), LW(0))
            CALL(evt_npc_set_ry, STRING("ババ"), LW(3))
            IF_INT_LESS_OR_EQUAL(LW(3), LW(4))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_set_ry, STRING("ババ"), 270)
        CALL(evt_npc_pera_onoff, STRING("ババ"), 1)
        CALL(evt_snd_sfxoff, LW(7))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("飛クッパ"), LW(2), LW(3), LW(4))
    CALL(evt_sub_get_sincos, 90, LW(0), GW(0))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 90, 300, 900)
        LABEL(3)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_sub_get_sincos, LW(0), LW(2), GW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(3)
        END_IF
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("飛クッパ"), LW(2), LW(3), LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_AIRSHIP_MOVE1"), LW(2), LW(3), LW(4), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 0, 1000, 1000)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    SET_FLOAT_VALUE(LW(5), FLOAT(250.0f))
    MULTIPLY_FLOAT(LW(5), LW(0))
    SET_FLOAT_VALUE(LW(6), GW(0))
    MULTIPLY_FLOAT(LW(6), FLOAT(120.0f))
    SET_FLOAT_VALUE(LW(7), FLOAT(500.0f))
    MULTIPLY_FLOAT(LW(7), LW(0))
    ADD_FLOAT(LW(5), LW(2))
    ADD_FLOAT(LW(6), LW(3))
    ADD_FLOAT(LW(7), LW(4))
    CALL(evt_npc_set_position, STRING("飛クッパ"), LW(5), LW(6), LW(7))
    CALL(evt_snd_sfx_pos, LW(9), LW(5), LW(6), LW(7))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    CALL(evt_npc_flag_onoff, 1, STRING("飛クッパ"), 128)
    CALL(evt_cam3d_evt_set, 102, 163, 666, 102, 71, 113, 500, 11)
    LOOP(0)
        IF_INT_NOT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_dir, STRING("ババ"), LW(3))
    ADD(LW(2), 5)
    CALL(evt_npc_set_position, STRING("houki"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("houki"), LW(3))
    CALL(evt_npc_set_anim, STRING("ババ"), STRING("D_1"))
    SET_VALUE(LW(0), 100)
    LOOP(6)
        WAIT_MS(LW(0))
        CALL(evt_npc_flag_onoff, 1, STRING("houki"), 128)
        WAIT_MS(LW(0))
        CALL(evt_npc_flag_onoff, 0, STRING("houki"), 128)
        DIVIDE(LW(0), 2)
    END_LOOP
    CALL(evt_npc_set_position, STRING("houki"), 0, -1000, 0)
    CALL(evt_npc_set_anim, STRING("ババ"), STRING("D_1"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("ババ"), LW(2), LW(3), LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FALL1"), LW(2), LW(3), LW(4), 0)
    CALL(evt_sub_intpl_msec_init, 1, LW(3), 0, 200)
    LABEL(2)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_npc_set_position, STRING("ババ"), LW(2), LW(0), LW(4))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_DOWN1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(50)
    CALL(evt_npc_set_anim, STRING("ババ"), STRING("D_2"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ババ"), STRING("P_1"))
    WAIT_FRAMES(1)
    CALL(evt_npc_set_ry, STRING("ババ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_set_autotalkpose, STRING("ババ"), STRING("P_1"), STRING("T_6"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_006"), 0, STRING("ババ"))
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("tou_00"), STRING(""))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(gor_04_init_evt)
    IF_INT_EQUAL(GSW(0), 215)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_KUPPA1"))
        CALL(evt_snd_bgm_scope, 0, 1)
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR5"))
    ELSE
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_NOT_EQUAL(LW(0), STRING("hikousen_return"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
        END_IF
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR5"))
    END_IF
    CALL(evt_snd_bgmon, 160, 0)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(luigi_npcEnt)
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_NOT_EQUAL(GSW(0), 215)
        CALL(evt_mobj_save_blk, STRING("mobj_save"), 140, 60, 85, 0, 0)
    END_IF
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -510, 80, 120, STRING("S_ground"), 125, GSWF(5583))
    CALL(evt_item_entry, STRING("item_00"), 125, 475, 0, 100, 16, GSWF(5584), 0)
    SPAWN_EVENT(evt_wing)
    CALL(evt_run_case_evt, 5, 1, STRING("A_noriba"), 0, PTR(evt_hikousen), 0)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hikousen_return"))
        SPAWN_EVENT(evt_hikousen_return)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gondora_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hako_1"), 1)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bero"), 2)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("n_bero"))
        SET_VALUE(GSWF(3578), 1)
        SET_VALUE(GSWF(3461), 1)
        SET_VALUE(GSWF(3460), 0)
        SET_VALUE(GSWF(3581), 1)
    ELSE
        SET_VALUE(GSWF(3578), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("resha_return"))
        SET_VALUE(GSWF(3578), 1)
        CALL_CHILD(hom_10_evt_resha_left)
        CALL_CHILD(hom_10_evt_resha_stop)
        SPAWN_EVENT(evt_train2)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_on)
    ELSE
        IF_INT_EQUAL(GSWF(3578), 1)
            CALL_CHILD(hom_10_evt_resha_left)
            SET_VALUE(LW(0), STRING("n_bero"))
            CALL_CHILD(bero_case_switch_on)
            CALL(evt_npc_set_position, STRING("駅員"), -400, 80, 100)
        ELSE
            CALL_CHILD(hom_10_evt_resha_right)
            SET_VALUE(LW(0), STRING("n_bero"))
            CALL_CHILD(bero_case_switch_off)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 345)
        SPAWN_EVENT(peach_mail)
    END_IF
    IF_INT_EQUAL(GSW(0), 215)
        CALL(evt_mario_set_mode, 2)
        CALL(evt_mario_kill_party, 0)
        CALL(evt_npc_entry, STRING("houki"), STRING("c_kamek_be"))
        CALL(evt_npc_set_anim, STRING("houki"), STRING("H_1"))
        CALL(evt_npc_set_position, STRING("houki"), 0, -1000, 0)
        CALL(evt_npc_flag_onoff, 1, STRING("houki"), 32)
        SPAWN_EVENT(gor_04_koopa_evt)
    END_IF
    IF_INT_EQUAL(GSW(0), 344)
        CALL(evt_mario_get_party, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_mario_set_party, 0, 1)
            CALL(evt_party_run, 0)
            CALL(evt_pouch_mario_recovery)
            CALL(evt_pouch_all_party_recovery)
        END_IF
    END_IF
    SPAWN_EVENT(evt_sub_mail_event)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_2"), 2)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

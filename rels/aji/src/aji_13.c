#include "aji_13.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
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
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_emergency[];
static USER_FUNC(mario_chk);
static void hitIsGroup(void);  // TODO: real signature
static void _hitIsGroupSub(void);  // TODO: real signature

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

EVT_BEGIN(evt_snd_stop)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 370)
        CALL(evt_snd_sfxoff, GW(11))
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_haikei_in)
    BEGIN_INLINE_EVENT
        CALL(evt_bero_exec_wait, 65536)
        WAIT_FRAMES(10)
        CALL(evt_mario_cam_keep_off)
        CALL(evt_mario_cam_y_on)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[15] = {
    {"w_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, evt_snd_stop, "aji_18", "e_bero", 5, 5, w_door_close, w_door_open},
    {"e_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, evt_snd_stop, "aji_14", "w_bero", 5, 5, e_door_close, e_door_open},
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_6", 1, 1},
    {"dokan_2", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_7", 1, 1},
    {"dokan_3", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_8", 1, 1},
    {"dokan_4", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_9", 1, 1},
    {"dokan_5", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_10", 1, 1},
    {"dokan_6", 4098, 0, 0x09, 100000, 0, 0, -1, evt_haikei_in, 6, NULL, NULL, "dokan_1", 1, 1},
    {"dokan_7", 4098, 0, 0x09, 100000, 0, 0, -1, evt_haikei_in, 6, NULL, NULL, "dokan_2", 1, 1},
    {"dokan_8", 4098, 0, 0x09, 100000, 0, 0, -1, evt_haikei_in, 6, NULL, NULL, "dokan_3", 1, 1},
    {"dokan_9", 4098, 0, 0x09, 100000, 0, 0, -1, evt_haikei_in, 6, NULL, NULL, "dokan_4", 1, 1},
    {"dokan_10", 4098, 0, 0x09, 100000, 0, 0, -1, evt_haikei_in, 6, NULL, NULL, "dokan_5", 1, 1},
    {"dokan_c", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_d", 1, 1},
    {"dokan_d", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_c", 1, 1},
    {NULL},
};

static char* name_tbl[13] = {
    "A_ugoku_1", "A_ugoku_2", "A_ugoku_3", "A_ugoku_4", "A_ugoku_h_1", "A_ugoku_h_2", "A_ugoku_h_3",
    "A_ugoku_h_4", "A_ugoku_h_5", "A_ugoku_h_6", "A_ugoku_h_7", "A_ugoku_h_8", "A_ugoku_h_9",
};

char* name_tbl2[6] = {
    "A_zenmai1", "A_zenmai2", "A_zenmai3", "A__zenmai4", "A_zenmai5", "A_zenmai6",
};

EVT_BEGIN(move_left_right)
    SET_VALUE(LW(5), LW(0))
    SET_VALUE(LW(6), LW(1))
    SET_VALUE(LW(7), LW(2))
    SET_VALUE(LW(8), LW(3))
    SET_VALUE(LW(9), LW(4))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(evt_hit_bind_mapobj, LW(5), LW(6))
    END_IF
    MULTIPLY(LW(7), 100)
    MULTIPLY(LW(8), 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, LW(7), LW(8), LW(9))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        CALL(evt_mapobj_trans, 1, LW(6), LW(0), 0, 0)
        IF_INT_NOT_EQUAL(LW(5), 0)
            CALL(evt_hit_bind_update, LW(5))
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        WAIT_MS(300)
        LOOP(0)
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, LW(8), LW(7), LW(9))
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        CALL(evt_mapobj_trans, 1, LW(6), LW(0), 0, 0)
        IF_INT_NOT_EQUAL(LW(5), 0)
            CALL(evt_hit_bind_update, LW(5))
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        WAIT_MS(300)
        LOOP(0)
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

// TODO: write unk_aji_000035a0 (global, 0xc8 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_13.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(move_up_down)
    SET_VALUE(LW(5), LW(0))
    SET_VALUE(LW(6), LW(1))
    SET_VALUE(LW(7), LW(2))
    SET_VALUE(LW(8), LW(3))
    SET_VALUE(LW(9), LW(4))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(evt_hit_bind_mapobj, LW(5), LW(6))
    END_IF
    MULTIPLY(LW(7), 100)
    MULTIPLY(LW(8), 100)
    SET_VALUE(LW(4), LW(8))
    IF_STR_EQUAL(LW(5), STRING("A_ugoku_2"))
        SET_VALUE(LW(3), 1)
        SET_VALUE(GF(1), 0)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                CALL(evt_mario_get_name_hitobj_head, LW(10))
                CALL(unk_aji_000035a0, LW(10), LW(5), LW(10))
                IF_INT_NOT_EQUAL(LW(10), 0)
                    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                    IF_FLOAT_GREATER(LW(1), FLOAT(0.1f))
                        SET_VALUE(GF(1), 1)
                    END_IF
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
    ELSE
        SET_VALUE(LW(3), 0)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, LW(7), LW(8), LW(9))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        CALL(evt_mapobj_trans, 1, LW(6), 0, LW(0), 0)
        IF_INT_NOT_EQUAL(LW(5), 0)
            CALL(evt_hit_bind_update, LW(5))
        END_IF
        IF_INT_NOT_EQUAL(LW(3), 0)
            IF_INT_NOT_EQUAL(GF(1), 0)
                MULTIPLY_FLOAT(LW(0), FLOAT(100.0f))
                GOTO(5)
            END_IF
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        LABEL(5)
        IF_INT_NOT_EQUAL(LW(3), 0)
            IF_INT_EQUAL(GF(1), 0)
                SET_VALUE(LW(8), LW(4))
            ELSE
                SET_VALUE(LW(8), LW(0))
            END_IF
            WAIT_MS(300)
            LOOP(0)
                IF_INT_EQUAL(GF(1), 0)
                    BREAK_LOOP
                END_IF
                SET_VALUE(GF(1), 0)
                WAIT_MS(1000)
            END_LOOP
        ELSE
            WAIT_MS(300)
        END_IF
        LOOP(0)
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, LW(8), LW(7), LW(9))
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        CALL(evt_mapobj_trans, 1, LW(6), 0, LW(0), 0)
        IF_INT_NOT_EQUAL(LW(5), 0)
            CALL(evt_hit_bind_update, LW(5))
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        SET_VALUE(LW(8), LW(4))
        WAIT_MS(300)
        LOOP(0)
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(move_r)
    SET_VALUE(LW(5), LW(0))
    SET_VALUE(LW(6), LW(1))
    SET_VALUE(LW(7), LW(2))
    SET_VALUE(LW(8), LW(3))
    SET_VALUE(LW(9), LW(4))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(evt_hit_bind_mapobj, LW(5), LW(6))
    END_IF
    MULTIPLY(LW(7), 100)
    MULTIPLY(LW(8), 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 0, LW(7), LW(8), LW(9))
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, LW(6), 0, 0, LW(0))
            IF_INT_NOT_EQUAL(LW(5), 0)
                CALL(evt_hit_bind_update, LW(5))
            END_IF
            IF_INT_EQUAL(GF(0), 1)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(0)
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u32 left_right_tbl[37] = {
    (u32)"A_ugoku_1", (u32)"S_ugoku_1", 0x00000000, 0xFFFFFF6A,
    0x00000BB8, (u32)"A_ugoku_3", (u32)"S_ugoku_3", 0x00000000,
    0x00000064, 0x000005DC, (u32)"A_ugoku_4", (u32)"S_ugoku_4",
    0x00000000, 0xFFFFFF9C, 0x000005DC, (u32)"A_ugoku_h_1",
    (u32)"S_ugoku_h_1", 0xFFFFFFE2, 0x000000C8, 0x00000BB8,
    (u32)"A_ugoku_h_2", (u32)"S_ugoku_h_2", 0xFFFFFFCE, 0x00000032,
    0x00000BB8, (u32)"A_ugoku_h_3", (u32)"S_ugoku_h_3", 0x00000032,
    0xFFFFFFCE, 0x00000BB8, (u32)"A_ugoku_h_7", (u32)"S_ugoku_h_7",
    0xFFFFFFCE, 0x00000050, 0x00001194, 0x00000000,
    0x00000000,
};

// TODO: type
u32 up_down_tbl[32] = {
    (u32)"A_ugoku_2", (u32)"S_ugoku_2", 0x00000000, 0xFFFFFF6A,
    0x00000AF0, (u32)"A_ugoku_h_4", (u32)"S_ugoku_h_4", 0x00000000,
    0x00000064, 0x00000BB8, (u32)"A_ugoku_h_5", (u32)"S_ugoku_h_5",
    0x00000000, 0xFFFFFF38, 0x00000FA0, (u32)"A_ugoku_h_6",
    (u32)"S_ugoku_h_6", 0x00000028, 0xFFFFFF6A, 0x00000BB8,
    (u32)"A_ugoku_h_8", (u32)"S_ugoku_h_8", 0x00000014, 0xFFFFFF9C,
    0x000007D0, (u32)"A_ugoku_h_9", (u32)"S_ugoku_h_9", 0xFFFFFF9C,
    0x00000014, 0x000007D0, 0x00000000, 0x00000000,
};

// TODO: type
u32 r_tbl[77] = {
    (u32)"A_zenmai1", (u32)"S_zenmai1", 0x00000000, 0x00000168,
    0x00001770, (u32)"A_zenmai2", (u32)"S_zenmai2", 0x00000000,
    0xFFFFFE98, 0x00001770, (u32)"A_zenmai3", (u32)"S_zenmai3",
    0x00000000, 0x00000168, 0x00000BB8, (u32)"A__zenmai4",
    (u32)"S_zenmai4", 0x00000000, 0x00000168, 0x00002328,
    (u32)"A_zenmai5", (u32)"S_zenmai5", 0x00000000, 0xFFFFFE98,
    0x00001770, (u32)"A_zenmai6", (u32)"S_zenmai6", 0x00000000,
    0xFFFFFE98, 0x00000BB8, 0x00000000, (u32)"S_zen1",
    0x00000000, 0x00000168, 0x00001770, 0x00000000,
    (u32)"S_zen2", 0x00000000, 0xFFFFFE98, 0x00000BB8,
    0x00000000, (u32)"S_zen3", 0x00000000, 0xFFFFFE98,
    0x00001770, 0x00000000, (u32)"S_zen4", 0x00000000,
    0x00000168, 0x00001770, 0x00000000, (u32)"S_zen5",
    0x00000000, 0x00000168, 0x00001770, 0x00000000,
    (u32)"S_zen6", 0x00000000, 0xFFFFFE98, 0x00000BB8,
    0x00000000, (u32)"S_zen7", 0x00000000, 0x00000168,
    0x00001770, 0x00000000, (u32)"S_zen8", 0x00000000,
    0xFFFFFE98, 0x00001770, 0x00000000, (u32)"S_zen9",
    0x00000000, 0x00000168, 0x00000BB8, 0x00000000,
    0x00000000,
};

// TODO: write mario_chk (static, 0x52c bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_13.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(shikake_init)
    SET_VALUE(GF(0), 0)
    SET_INT_READ_ADDR(PTR(left_right_tbl))
    LOOP(0)
        READ_INT2(LW(0), LW(1))
        READ_INT3(LW(2), LW(3), LW(4))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        SPAWN_EVENT(move_left_right)
    END_LOOP
    SET_INT_READ_ADDR(PTR(up_down_tbl))
    LOOP(0)
        READ_INT2(LW(0), LW(1))
        READ_INT3(LW(2), LW(3), LW(4))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        SPAWN_EVENT(move_up_down)
    END_LOOP
    SET_INT_READ_ADDR(PTR(r_tbl))
    LOOP(0)
        READ_INT2(LW(0), LW(1))
        READ_INT3(LW(2), LW(3), LW(4))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        SPAWN_EVENT(move_r)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(mario_chk)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(evt_sw)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(GF(0), 1)
    CALL(evt_cam3d_evt_set, 475, 425, 743, 475, 325, 0, 1000, 11)
    WAIT_MS(2500)
    CALL(evt_paper_entry, STRING("OFF_hei_07"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_hei_07"))
    CALL(evt_offscreen_entry, STRING("ofs"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_kaidan"), STRING("ofs"))
    CALL(evt_offscreen_get_boundingbox, STRING("ofs"), LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, LW(10), LW(11), LW(12), LW(13))
    WAIT_FRAMES(1)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_onoff, STRING("img"), 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF7_KAIDAN_APPEAR1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kaidan"), 1)
    CALL(evt_hitobj_onoff, STRING("A_kaidan"), 1, 1)
    WAIT_MS(1800)
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_kaidan"))
    CALL(evt_offscreen_delete, STRING("ofs"))
    SET_VALUE(GSW(0), 371)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(500)
    SET_VALUE(GF(0), 0)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_hari)
    SET_VALUE(LW(8), LW(0))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(evt_sub_random, 900, LW(3))
        IF_INT_EQUAL(LF(0), 0)
            MULTIPLY(LW(3), -1)
            SET_VALUE(LF(0), 1)
        ELSE
            SET_VALUE(LF(0), 0)
        END_IF
        CALL(evt_sub_random, 1500, LW(9))
        ADD(LW(9), 1500)
        CALL(evt_sub_intpl_msec_init, 11, LW(2), LW(3), LW(9))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, LW(8), 0, 0, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        SET_VALUE(LW(2), LW(3))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_ramp)
    CALL(evt_mapobj_get_position, STRING("S_bel"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_CONVEYER_MOVE1"), LW(0), LW(1), LW(2), GW(11))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_pato1"), 1)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(evt_mapobj_rotate, 1, STRING("S_pato1"), 0, LW(0), 0)
            ADD_FLOAT(LW(0), FLOAT(10.0f))
            IF_FLOAT_GREATER(LW(0), FLOAT(360.0f))
                SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("S_hari01"))
    SPAWN_EVENT(evt_hari)
    SET_VALUE(LW(0), STRING("S_hari02"))
    SPAWN_EVENT(evt_hari)
    SET_VALUE(LW(0), STRING("S_hari03"))
    SPAWN_EVENT(evt_hari)
    SET_VALUE(LW(0), STRING("S_hari04"))
    SPAWN_EVENT(evt_hari)
    RETURN
EVT_END()

EVT_BEGIN(evt_bcv_sw)
    SET_VALUE(GSW(0), 370)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -575, 152, 578, -575, 57, 37, 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, STRING("S_bel"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_CONVEYER_ACTIVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_1_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_2_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_3_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_4_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_1_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_2_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_3_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_4_2"), 1)
    CALL(evt_map_playanim, STRING("kon"), 1, 0)
    SPAWN_EVENT(evt_ramp)
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_key_get)
    SET_VALUE(GSWF(4181), 1)
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x00000020, 0xFFFFFFFF,
};

static EVT_BEGIN(evt_door_msg)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_pouch_check_item, 32, LW(0))
    IF_INT_LESS(LW(0), 1)
        CALL(evt_msg_print, 0, STRING("stg7_aji_67"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(evt_pouch_remove_item, LW(0), LW(1))
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_on)
        SET_VALUE(GSW(0), 372)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi1_mid"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi1_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_lights1_m"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_lights1_r"), 1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DOOR_UNLOCK1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_exit_case_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write bg_cam_chk (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_13.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(aji_13_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI7"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(shikake_init)
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou1"), STRING("A_hikou1"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ami2"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_dai5"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_dai3"), 2)
    IF_INT_LESS(GSW(0), 370)
        CALL(evt_mobj_switch_blue, 0, STRING("bcv_sw"), -150, 0, 100, PTR(evt_bcv_sw), GSWF(4185))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_pato1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_1_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_2_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_3_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_4_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_1_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_2_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_3_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_4_2"), 1)
    ELSE
        CALL(evt_map_playanim, STRING("kon"), 1, 0)
        CALL_CHILD(evt_ramp)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_1_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_2_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_3_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("meta_4_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_1_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_2_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_3_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("meta_4_2"), 1)
    END_IF
    IF_INT_EQUAL(GSWF(4181), 0)
        CALL(evt_item_entry, STRING("key"), 32, -50, 100, 100, 16, -1, PTR(evt_key_get))
    END_IF
    IF_INT_LESS(GSW(0), 371)
        CALL(evt_mobj_switch_blue, 0, STRING("sw"), 650, 200, 100, PTR(evt_sw), GSWF(4184))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kaidan"), 1)
        CALL(evt_hitobj_onoff, STRING("A_kaidan"), 1, 0)
    END_IF
    IF_INT_LESS(GSW(0), 372)
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi1"), 0, PTR(evt_door_msg), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi1_mid"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_lights1_m"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi1_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_lights1_r"), 1)
    END_IF
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 200, 60, 100, 0, 0)
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 10, 300, 60, 100, 0, 0)
    CALL(evt_item_entry, STRING("item00"), 150, -670, 320, -450, 16, GSWF(4215), 0)
    CALL(evt_item_set_scale, STRING("item00"), FLOAT(0.6f))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        CALL(evt_npc_check_delete, STRING("軍団ザコ魔法使い"))
        CALL(evt_npc_check_delete, STRING("バリアーンＺ"))
    END_IF
    SPAWN_EVENT(evt_emergency)
    BEGIN_INLINE_EVENT
        CALL(bg_cam_chk)
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: write hitIsGroup (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_13.s; this is where the original has it, just before the first script that uses it

// TODO: write _hitIsGroupSub (static, 0x530 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_13.s; this is where the original has it, just before the first script that uses it
// clang-format on

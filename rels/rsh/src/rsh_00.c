#include "rsh_00.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_rsh_window[];
static void ext_disp(void);  // TODO: real signature
static void randf(void);  // TODO: real signature
static void moamoa_pose_change(void);  // TODO: real signature
static void moamoa_pose_main(void);  // TODO: real signature
static void moamoa_pose_init(void);  // TODO: real signature

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 moamoa_data[243] = {
    (u32)"c_n_moa_a", (u32)"S_1", 0x00000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x3F800000, (u32)"c_n_moa_a", (u32)"S_1",
    0x40000000, (u32)"c_n_moa_a", (u32)"S_1", 0x40400000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x40800000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x40A00000, (u32)"c_n_moa_a", (u32)"S_1",
    0x40C00000, (u32)"c_n_moa_a", (u32)"S_1", 0x40E00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41100000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41200000, (u32)"c_n_moa_a", (u32)"S_1", 0x41300000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41400000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41500000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41600000, (u32)"c_n_moa_a", (u32)"S_1", 0x41700000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41800000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41880000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41900000, (u32)"c_n_moa_a", (u32)"S_1", 0x41980000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41A00000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41A80000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41B00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41B80000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41C00000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41C80000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41D00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41D80000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41E00000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41E80000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41F00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41F80000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x42000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x42040000, (u32)"c_n_moa_a", (u32)"S_1",
    0x42080000, (u32)"c_n_moa_a", (u32)"S_1", 0x420C0000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x42100000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x42140000, (u32)"c_n_moa_a", (u32)"S_1",
    0x42180000, (u32)"c_n_moa_a", (u32)"S_1", 0x421C0000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x00000000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x3F800000, (u32)"c_n_moa_b", (u32)"S_1",
    0x40000000, (u32)"c_n_moa_b", (u32)"S_1", 0x40400000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x40800000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x40A00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x40C00000, (u32)"c_n_moa_b", (u32)"S_1", 0x40E00000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41000000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41100000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41200000, (u32)"c_n_moa_b", (u32)"S_1", 0x41300000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41400000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41500000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41600000, (u32)"c_n_moa_b", (u32)"S_1", 0x41700000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41800000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41880000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41900000, (u32)"c_n_moa_b", (u32)"S_1", 0x41980000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41A00000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41A80000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41B00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41B80000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41C00000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41C80000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41D00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41D80000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41E00000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41E80000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41F00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41F80000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x42000000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x42040000, (u32)"c_n_moa_b", (u32)"S_1",
    0x42080000, (u32)"c_n_moa_b", (u32)"S_1", 0x420C0000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x42100000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x42140000, (u32)"c_n_moa_b", (u32)"S_1",
    0x42180000, (u32)"c_n_moa_b", (u32)"S_1", 0x421C0000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: write evt_moamoa_set_seq_all (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_set_one (global, 0x14c bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_moamoa)
    CALL(evt_snd_bgmoff_f, 512, 120)
    CALL(evt_cam3d_evt_set, -70, 113, 373, 126, 38, -13, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 1, STRING("運転手"), 32)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 85, -110, FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 115, -110, FLOAT(80.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_set_dir_to_target, STRING("運転手"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg6_rsh_231"), 0, STRING("me"))
    CALL(evt_npc_flag_onoff, 0, STRING("運転手"), 32)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_VOICE1"), 265, 25, -145, 0)
    CALL(evt_moamoa_set_one, 0, 265, 25, -145)
    CALL(evt_moamoa_set_one, 1, 265, 35, -230)
    WAIT_MS(1000)
    CALL(evt_npc_reverse_ry, STRING("me"))
    CALL(evt_msg_print, 0, STRING("stg6_rsh_232"), 0, STRING("me"))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG6_MOAMOA_VOICE2"), LW(10))
        WAIT_MS(1000)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_moamoa_set_seq_all, 1)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 329)
    LOOP(3)
        CALL(evt_cam3d_evt_set, -2, 86, 230, 126, 38, -13, 350, 11)
        WAIT_MS(350)
        CALL(evt_cam3d_evt_set, -70, 113, 373, 126, 38, -13, 350, 11)
        WAIT_MS(350)
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("a"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("b"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1536)
    CALL(evt_npc_set_anim, STRING("me"), STRING("B_1"))
    CALL(evt_mario_set_pose, STRING("M_I_N"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_npc_set_anim, STRING("me"), STRING("T_2"))
    CALL(evt_msg_print, 0, STRING("stg6_rsh_233"), 0, STRING("me"))
    CALL(evt_npc_set_anim, STRING("me"), STRING("B_2"))
    CALL(evt_cam3d_evt_set, -2, 86, 230, 126, 38, -13, 500, 11)
    WAIT_MS(500)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_234_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_234_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_234_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_234_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_234_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_234_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_234_6"), 0, STRING("party"))
    END_SWITCH
    CALL(evt_party_run, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG6_MOAMOA_VOICE2"), LW(10))
    CALL(evt_snd_sfx_vol, LW(10), 88)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_235_00"), 0, STRING("me"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    RETURN
EVT_END()

// TODO: write moamoa_dispent (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_main (global, 0x3e4 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_init (global, 0x72c bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_rsh_moamoa_on_window)
    SWITCH(GSW(0))
        CASE_BETWEEN(329, 331)
        CALL(evt_snd_sfxon, STRING("SFX_STG6_MOAMOA_VOICE2"), LW(10))
        CALL(evt_snd_sfx_vol, LW(10), 88)
        CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
        CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
        CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
        CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
        CALL(evt_ext_entry, 1000, PTR(moamoa_data), PTR(moamoa_init), PTR(moamoa_main), PTR(moamoa_dispent))
        CALL(evt_moamoa_set_seq_all, 4)
    END_SWITCH
    RETURN
EVT_END()

static BeroEntry bero_data_a[2] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_01_a", "e_bero", 1, 1, "w_door_close", "w_door_open"},
    {NULL},
};

static BeroEntry bero_data_b[2] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_01_b", "e_bero", 1, 1, "w_door_close", "w_door_open"},
    {NULL},
};

static BeroEntry bero_data_c[2] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_01_c", "e_bero", 1, 1, "w_door_close", "w_door_open"},
    {NULL},
};

EVT_BEGIN(evt_handle)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 329)
            IF_INT_LESS(GSW(0), 334)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(0)
            ADD(LW(0), 10)
            MODULO(LW(0), 360)
            CALL(evt_mapobj_rotate, 1, STRING("handoru_1"), 0, LW(0), 0)
            SET_VALUE(LW(1), LW(0))
            MULTIPLY(LW(1), -1)
            CALL(evt_mapobj_rotate, 1, STRING("handoru_2"), 0, LW(1), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 0, -40, 300)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("handoru_3"), LW(0), 0, 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
            CALL(evt_sub_intpl_msec_init, 11, -40, 0, 300)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("handoru_3"), LW(0), 0, 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(regu_unten)
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 0)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 310)
            IF_INT_LESS(GSW(0), 326)
                RETURN
            END_IF
        END_IF
        IF_INT_GREATER_OR_EQUAL(GSW(0), 329)
            IF_INT_LESS(GSW(0), 339)
                RETURN
            END_IF
        END_IF
        IF_INT_GREATER_OR_EQUAL(GSW(0), 346)
            IF_INT_EQUAL(GSWF(3579), 1)
                RETURN
            END_IF
            IF_INT_EQUAL(GSWF(3581), 1)
                RETURN
            END_IF
            IF_INT_EQUAL(GSWF(3580), 1)
                RETURN
            END_IF
        END_IF
        CALL(evt_sub_random, 300, LW(10))
        SWITCH(LW(10))
            CASE_LESS(100)
            IF_INT_LESS(GSW(0), 328)
            END_IF
            SET_VALUE(LW(2), LW(3))
            CALL(evt_sub_random, 135, LW(10))
            SUBTRACT(LW(10), 45)
            SET_VALUE(LW(3), LW(10))
            CALL(evt_sub_intpl_msec_init, 11, LW(2), LW(3), 1000)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_rotate, 1, STRING("handoru_1"), 0, LW(0), 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
            CASE_LESS(200)
            IF_INT_LESS(GSW(0), 328)
            END_IF
            SET_VALUE(LW(4), LW(5))
            CALL(evt_sub_random, 40, LW(10))
            MULTIPLY(LW(10), -1)
            SET_VALUE(LW(5), LW(10))
            CALL(evt_sub_intpl_msec_init, 11, LW(4), LW(5), 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("handoru_3"), LW(0), 0, 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
            CASE_DEFAULT
            IF_INT_LESS(GSW(0), 328)
            END_IF
            SET_VALUE(LW(6), LW(7))
            CALL(evt_sub_random, 135, LW(10))
            SUBTRACT(LW(10), 90)
            SET_VALUE(LW(7), LW(10))
            CALL(evt_sub_intpl_msec_init, 11, LW(6), LW(7), 1000)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_rotate, 1, STRING("handoru_2"), 0, LW(0), 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(2)
            END_IF
        END_SWITCH
        CALL(evt_sub_random, 2, LW(10))
        ADD(LW(10), 1)
        MULTIPLY(LW(10), 1000)
        WAIT_MS(LW(10))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(init_unten)
    SWITCH(GSW(0))
        CASE_BETWEEN(329, 331)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 1536)
        CALL(evt_npc_set_anim, STRING("me"), STRING("B_2"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_unten)
    SWITCH(GSW(0))
        CASE_LESS(288)
        IF_INT_EQUAL(LSWF(1), 0)
            CALL(evt_msg_print, 0, STRING("stg6_rsh_00"), 0, 0)
            SET_VALUE(LSWF(1), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg6_rsh_01"), 0, 0)
            SET_VALUE(LSWF(1), 0)
        END_IF
        CASE_LESS(292)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_01_01"), 0, 0)
        CASE_LESS(293)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_108"), 0, STRING("me"))
        CALL(evt_npc_set_anim, STRING("me"), STRING("A_1"))
        WAIT_MS(500)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_DRIVER_SIGN1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_set_anim, STRING("me"), STRING("A_2"))
        WAIT_MS(2000)
        CALL(evt_npc_set_anim, STRING("me"), STRING("A_3"))
        WAIT_MS(500)
        CALL(evt_item_entry, STRING("sign"), 77, 0, 0, 0, 17, -1, 0)
        CALL(evt_item_get_item, STRING("sign"))
        CALL(evt_item_get_item_end_wait, STRING("sign"))
        CALL(evt_npc_set_anim, STRING("me"), STRING("T_1"))
        CALL(evt_msg_print, 0, STRING("stg6_rsh_110"), 0, STRING("me"))
        SET_VALUE(GSW(0), 293)
        CASE_LESS(294)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_01_01"), 0, 0)
        CASE_LESS(301)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_01_02"), 0, 0)
        CASE_LESS(303)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_147_01"), 0, 0)
        CASE_LESS(310)
        IF_INT_EQUAL(LSWF(2), 0)
            CALL(evt_msg_print, 0, STRING("stg6_rsh_152"), 0, 0)
            SET_VALUE(LSWF(2), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg6_rsh_153"), 0, 0)
        END_IF
        CASE_LESS(326)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_209"), 0, 0)
        CASE_LESS(327)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_217"), 0, 0)
        CASE_LESS(329)
        CALL_CHILD(evt_moamoa)
        CASE_LESS(332)
        CALL(evt_npc_set_anim, STRING("me"), STRING("T_2"))
        CALL(evt_msg_print, 0, STRING("stg6_rsh_235"), 0, 0)
        CALL(evt_npc_set_anim, STRING("me"), STRING("B_2"))
        CASE_LESS(346)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_253"), 0, 0)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(3580), 1)
            CALL(evt_msg_print, 0, STRING("stg6_rsh_c_08_1"), 0, 0)
            RETURN
        END_IF
        IF_INT_EQUAL(GSWF(3578), 0)
            CALL(evt_msg_print, 0, STRING("stg6_rsh_c_07"), 0, 0)
        ELSE
            IF_INT_EQUAL(GSWF(3581), 0)
                CALL(evt_msg_print, 0, STRING("stg6_rsh_c_08"), 0, 0)
            ELSE
                CALL(evt_msg_print, 0, STRING("stg6_rsh_c_08_1"), 0, 0)
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npc_data[46] = {
    (u32)"運転手", 0x40020610, 0x00000000, (u32)&init_unten,
    (u32)&regu_unten, (u32)&talk_unten, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static char* access_tbl[5] = {
    "nittei", "stg6_rsh_259", "soujyuu", "stg6_rsh_260", NULL,
};

static EVT_BEGIN(evt_access)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, LW(1), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_access_entry)
    SET_INT_READ_ADDR(PTR(access_tbl))
    LOOP(0)
        READ_INT2(LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 9, 1, LW(0), 0, PTR(evt_access), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(rsh_00_init_evt)
    SWITCH(GSW(0))
        CASE_BETWEEN(310, 325)
        CASE_GREATER_OR_EQUAL(333)
        CASE_DEFAULT
        CALL(evt_map_playanim, STRING("yure"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_sora"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_yama"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_iwayama"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_saboten"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_hosi"), 1, 0)
    END_SWITCH
    CALL(evt_npc_setup, PTR(npc_data))
    CALL_CHILD(evt_access_entry)
    SPAWN_EVENT(evt_handle)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -210, 60, -145, 0, 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_NOT_EQUAL(LW(0), STRING("rsh_00_c"))
        CALL(evt_mobj_kururing_floor, STRING("kururin1"), -132, 0, -95, STRING("s_zentai"), 125, GSWF(5648))
    ELSE
        CALL(evt_mobj_kururing_floor, STRING("kururin1"), -132, 0, -95, STRING("s_yuka1"), 125, GSWF(5648))
    END_IF
    SWITCH(GSW(0))
        CASE_BETWEEN(284, 286)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
        CASE_BETWEEN(304, 309)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
        CASE_BETWEEN(310, 325)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_HOM1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_BETWEEN(327, 328)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
        CASE_BETWEEN(329, 331)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
        CASE_BETWEEN(334, 344)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_DEFAULT
        CALL(evt_sub_get_mapname, LW(0))
        IF_STR_EQUAL(LW(0), STRING("rsh_00_a"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
        END_IF
        IF_STR_EQUAL(LW(0), STRING("rsh_00_b"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH3"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
        END_IF
        IF_STR_EQUAL(LW(0), STRING("rsh_00_c"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(rsh_00_a_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_a))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_00_init_evt)
    SWITCH(GSW(0))
        CASE_BETWEEN(328, 331)
        CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
        CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
        CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
        CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
        CALL(evt_ext_entry, 1000, PTR(moamoa_data), PTR(moamoa_init), PTR(moamoa_main), PTR(moamoa_dispent))
        IF_INT_GREATER_OR_EQUAL(GSW(0), 329)
            CALL(evt_moamoa_set_seq_all, 4)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(rsh_00_b_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_b))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_00_init_evt)
    RETURN
EVT_END()

static char* window_tbl[5] = {
    "mado_uturi_1", "mado_uturi_2", "mado_uturi_3", "mado_uturi_4", NULL,
};

EVT_BEGIN(rsh_00_c_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_c))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_00_init_evt)
    SET_VALUE(LW(0), PTR(window_tbl))
    CALL_CHILD(evt_rsh_window)
    RETURN
EVT_END()

EVT_BEGIN(rsh_00_init_evt_nidome)
    IF_INT_EQUAL(GSWF(3579), 0)
        IF_INT_EQUAL(GSWF(3580), 0)
            IF_INT_EQUAL(GSWF(3581), 0)
                CALL(evt_map_playanim, STRING("yure"), 1, 0)
                IF_INT_EQUAL(GSWF(3578), 0)
                    CALL(evt_map_playanim, STRING("hai_sora"), 1, 0)
                    CALL(evt_map_playanim, STRING("hai_yama"), 1, 0)
                    CALL(evt_map_playanim, STRING("hai_iwayama"), 1, 0)
                    CALL(evt_map_playanim, STRING("hai_saboten"), 1, 0)
                ELSE
                    CALL(evt_map_playanim, STRING("rt_hai_sora"), 1, 0)
                    CALL(evt_map_playanim, STRING("rt_hai_yama"), 1, 0)
                    CALL(evt_map_playanim, STRING("rt_hai_iwayama"), 1, 0)
                    CALL(evt_map_playanim, STRING("rt_hai_saboten"), 1, 0)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(evt_npc_setup, PTR(npc_data))
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -210, 60, -145, 0, 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_NOT_EQUAL(LW(0), STRING("rsh_00_c"))
        CALL(evt_mobj_kururing_floor, STRING("kururin1"), -132, 0, -95, STRING("s_zentai"), 125, GSWF(5648))
    ELSE
        CALL(evt_mobj_kururing_floor, STRING("kururin1"), -132, 0, -95, STRING("s_yuka1"), 125, GSWF(5648))
    END_IF
    CALL_CHILD(evt_access_entry)
    IF_INT_EQUAL(GSWF(3460), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
    ELSE
        IF_INT_EQUAL(GSWF(3461), 1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
            CALL(evt_snd_envoff, 512)
            CALL(evt_snd_bgmoff, 26624)
        ELSE
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), STRING("rsh_00_a"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH1"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
            END_IF
            IF_STR_EQUAL(LW(0), STRING("rsh_00_b"))
            END_IF
            IF_STR_EQUAL(LW(0), STRING("rsh_00_c"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_4"))
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_00_a_init_evt_nidome)
    SET_VALUE(LW(0), PTR(bero_data_a))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_00_init_evt_nidome)
    RETURN
EVT_END()

EVT_BEGIN(rsh_00_b_init_evt_nidome)
    SET_VALUE(LW(0), PTR(bero_data_b))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_00_init_evt_nidome)
    RETURN
EVT_END()

EVT_BEGIN(rsh_00_c_init_evt_nidome)
    SET_VALUE(LW(0), PTR(bero_data_c))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_00_init_evt_nidome)
    SET_VALUE(LW(0), PTR(window_tbl))
    CALL_CHILD(evt_rsh_window)
    RETURN
EVT_END()

// TODO: type
static u32 wp[1] = {
    0x00000000,
};

// TODO: write ext_disp (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

// TODO: write randf (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_pose_change (static, 0xec bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_pose_main (static, 0x194 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_pose_init (static, 0x1c8 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_00.s; this is where the original has it, just before the first script that uses it
// clang-format on

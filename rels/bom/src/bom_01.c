#include "bom_01.h"
#include "driver/envdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_bom_snowfall[];
static USER_FUNC(starstone_set_ry);
static USER_FUNC(starstone_set_pos);
static USER_FUNC(starstone_entry);
static USER_FUNC(anim_tevcallback);
static USER_FUNC(yami_view);
static USER_FUNC(make_name);
static USER_FUNC(depth);
static void depthDisp(void);  // TODO: real signature
static void ext_dispent(void);  // TODO: real signature
static void ext_main(void);  // TODO: real signature
static void ext_disp(void);  // TODO: real signature
static void ext_init(void);  // TODO: real signature
static void pose_change(void);  // TODO: real signature
static void pose_main(void);  // TODO: real signature
static void pose_init(void);  // TODO: real signature

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(evt_door_1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.555f))
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), LW(0), FLOAT(0.0f), FLOAT(0.0f))
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    RETURN
EVT_END()

char* door_npc[4] = {
    "ホワイト", "ペントリット", "スラリー", NULL,
};

static EVT_BEGIN(door_open)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(door_close)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"s_ie_door",
    (u32)"A_ie_door_u", (u32)"A_ie_door", (u32)"s_ie_kai", 0x00000000,
    (u32)"S_ie_naka", 0x00000000, (u32)&evt_door_1, (u32)&evt_door_2,
    (u32)&evt_wall, (u32)&door_npc, 0x00000000, 0x00000000,
    (u32)&door_open, (u32)&door_close,
};

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "bom_04", "e_bero4", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "bom_02", "w_bero", 1, 1},
    {NULL},
};

static EVT_BEGIN(first_evt)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_cam_letter_box_onoff, 1, 1)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 340, 100, -53, 340, 80, -302, 0, 11)
    CALL(evt_mario_set_pos, -580, 0, 0)
    CALL(evt_party_set_pos, 0, -580, 0, 0)
    CALL(evt_seq_wait, 2)
    BEGIN_INLINE_EVENT
        WAIT_MS(6000)
        CALL(evt_telop_entry, 13)
    END_INLINE_EVENT
    WAIT_MS(4000)
    CALL(evt_cam3d_evt_set, 200, 150, 1001, -40, 130, 348, 6000, 11)
    WAIT_MS(6000)
    CALL(evt_mario_normalize)
    CALL(evt_mario_mov_pos2, -351, 0, FLOAT(120.0f))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -380, 97, 267, -380, 38, -9, 0, 11)
    WAIT_MS(300)
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    IF_INT_EQUAL(GSWF(3885), 1)
        CALL(evt_msg_print_party, STRING("stg7_bom_01__2_00"))
    ELSE
        CALL(evt_msg_print_party, STRING("stg7_bom_01__2"))
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 353)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_bom_01_karakara)
    SET_VALUE(LW(9), LW(0))
    SET_VALUE(LW(8), 0)
    LOOP(0)
        CALL(evt_mapobj_get_position, LW(9), LW(0), LW(1), LW(2))
        CALL(evt_sub_intpl_msec_init, 12, 1, 10, 1000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        SUBTRACT(LW(8), LW(0))
        IF_INT_LESS(LW(8), 0)
            ADD(LW(8), 360)
        END_IF
        CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(8), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        CALL(evt_sub_random, 10, LW(5))
        ADD(LW(5), 1)
        MULTIPLY(LW(5), 30)
        LOOP(LW(5))
            SUBTRACT(LW(8), LW(0))
            IF_INT_LESS(LW(8), 0)
                ADD(LW(8), 360)
            END_IF
            CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(8), 0)
            WAIT_FRAMES(1)
        END_LOOP
        BEGIN_INLINE_EVENT
            WAIT_MS(900)
        END_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 12, 10, 1, 1000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        SUBTRACT(LW(8), LW(0))
        IF_INT_LESS(LW(8), 0)
            ADD(LW(8), 360)
        END_IF
        CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(8), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 ext_data[93] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: write ext_dispent (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_main (static, 0x1494 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_init (static, 0x50c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write make_ext_table (global, 0x124 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_bom1000)
    CALL(make_ext_table)
    CALL(evt_npc_entry, STRING("dummy"), STRING("c_bom_sen"))
    CALL(evt_ext_entry, 1000, PTR(ext_data), PTR(ext_init), PTR(ext_main), PTR(ext_dispent))
    RETURN
EVT_END()

EVT_BEGIN(gamen_shake)
    CALL(evt_cam_shake, 4, FLOAT(0.002f), FLOAT(0.0f), 0)
    RETURN
EVT_END()

EVT_BEGIN(gamen_shake2)
    CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 100)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 100)
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 100)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 100)
    RETURN
EVT_END()

EVT_BEGIN(mario_tobideru)
    CALL(evt_mario_set_pos, 400, 780, 0)
    CALL(evt_sub_intpl_msec_init, 2, 0, 400, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    SET_VALUE(LW(2), 400)
    SET_VALUE(LW(3), 780)
    ADD(LW(2), LW(0))
    ADD(LW(3), LW(0))
    CALL(evt_mario_set_pos, LW(2), LW(3), 0)
    SUBTRACT(LW(2), 30)
    SUBTRACT(LW(3), 30)
    CALL(evt_party_set_pos, 0, LW(2), LW(3), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    RETURN
EVT_END()

// TODO: write taihou_r (global, 0xec bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(mario_shot)
    SPAWN_EVENT(mario_tobideru)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_DISCHARGE1"), 0)
    CALL(evt_sub_intpl_msec_init, 2, 0, -50, 150)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, -40)
    CALL(evt_mapobj_trans, 1, STRING("dekatai"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SPAWN_EVENT(gamen_shake2)
    CALL(evt_eff, STRING("kemuri"), STRING("kemuri_test"), 11, 570, 950, 0, FLOAT(3.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("ripple"), STRING("ripple_n64"), 2, 570, 950, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("particle"), STRING("particle"), 0, 520, 900, 0, 1, 1, 0, 100, 0, 0, 0, 0)
    CALL(taihou_r)
    BEGIN_INLINE_EVENT
        CALL(evt_eff, 0, STRING("hit"), 1, 570, 950, 0, FLOAT(3.0f), 4, 0, 0, 0, 0, 0, 0)
        WAIT_MS(200)
        CALL(evt_eff, 0, STRING("hit"), 1, 570, 950, 0, FLOAT(3.0f), 4, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 5, -50, 0, 150)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, -40)
    CALL(evt_mapobj_trans, 1, STRING("dekatai"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(effect_delete)
    CALL(evt_eff_delete, STRING("kemuri"))
    CALL(evt_eff_delete, STRING("ripple"))
    CALL(evt_eff_delete, STRING("particle"))
    RETURN
EVT_END()

static EVT_BEGIN(mario_kemuri)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_eff, 0, STRING("kemuri_test"), 12, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(2)
    END_LOOP
    RETURN
EVT_END()

// TODO: write depth (static, 0x44 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_depth)
    CALL(depth)
    RETURN
EVT_END()

EVT_BEGIN(cam_outside)
    CALL(evt_map_set_blend, 0, 255, 255, 255, 255)
    CALL(evt_map_set_blend, 1, 255, 255, 255, 255)
    CALL(evt_map_blend_off, 0)
    CALL(evt_map_blend_off, 1)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), 0, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_ie_kai"), 1)
    RETURN
EVT_END()

EVT_BEGIN(cam_inside)
    CALL(evt_cam3d_evt_set, -160, 125, -100, 10, 55, -427, 0, 11)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 0)
    CALL(evt_map_set_blend, 1, 0, 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), 90, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), 0, -90, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_ie_kai"), 1)
    RETURN
EVT_END()

EVT_BEGIN(cam_inside2)
    CALL(evt_cam3d_evt_set, -93, 104, -250, 9, 62, -447, 0, 11)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 0)
    CALL(evt_map_set_blend, 1, 0, 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), 90, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), 0, -90, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_ie_kai"), 1)
    RETURN
EVT_END()

static EVT_BEGIN(party_damage_pose)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_D_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_D_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("D_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_D_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_D_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_D_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_D_1"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(effect_1)
    SET_VALUE(LW(0), -220)
    LOOP(22)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, LW(0), 5, -220, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, LW(0), 5, 220, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        ADD(LW(0), 20)
    END_LOOP
    SET_VALUE(LW(0), -220)
    LOOP(22)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, -220, 5, LW(0), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, 220, 5, LW(0), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        ADD(LW(0), 20)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(open_hatch)
    CALL(evt_mapobj_get_position, LW(1), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_SHUTTER_OPEN1"), LW(10), LW(11), LW(12), 0)
    SET_VALUE(LW(8), LW(0))
    SET_VALUE(LW(9), LW(1))
    SET_VALUE(LW(7), LW(2))
    CALL(evt_sub_intpl_msec_init, 1, 0, 180, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    IF_INT_EQUAL(LW(7), 0)
        CALL(evt_mapobj_trans, 1, LW(9), 0, 0, LW(0))
        MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
        CALL(evt_mapobj_trans, 1, LW(8), 0, 0, LW(0))
    ELSE
        CALL(evt_mapobj_trans, 1, LW(9), LW(0), 0, 0)
        MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
        CALL(evt_mapobj_trans, 1, LW(8), LW(0), 0, 0)
    END_IF
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SPAWN_EVENT(gamen_shake2)
    RETURN
EVT_END()

// TODO: type
static u32 spline_dt[9] = {
    0xC3480000, 0x00000000, 0x44228000, 0x44138000,
    0x43480000, 0xC5066000, 0x44AC8000, 0x42C80000,
    0xC59AB000,
};

char* oh_data[8] = {
    "ニトロ", "stg7_bom_65_a", "ペントリット", "stg7_bom_65_b", "スラリー", "stg7_bom_65_c", "ボムヘイD",
    "stg7_bom_65_d",
};

// TODO: write make_name (static, 0x68 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write bom_fall (global, 0x254 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write bom_flag (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write sight_dir (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_taihou)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_msg_print, 0, STRING("stg7_bom_63"), 0, STRING("me"))
    WAIT_MS(300)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos, 0, 10, 0, 500)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos, 0, 10, 500)
    CALL(evt_cam3d_evt_set, 0, 152, 555, 0, 57, 13, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("chika_off"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("jime_ugo"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yuka2"), 1)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    WAIT_FRAMES(1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    CALL_CHILD(party_damage_pose)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_FLOOR_OPEN1"), LW(10), LW(11), LW(12), LW(9))
    CALL(evt_sub_intpl_msec_init, 12, 0, 20, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_b"), LW(0), -5, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_a"), LW(0), -5, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    WAIT_MS(800)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_MARIO_FALL1"), LW(3), LW(4), LW(5), LW(6))
    CALL(evt_sub_intpl_msec_init, 1, 0, -100, 400)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_mario_set_pos, LW(3), LW(0), LW(5))
    CALL(evt_snd_sfx_pos, LW(6), LW(3), LW(0), LW(5))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_party_set_pos, 0, LW(3), LW(0), LW(5))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_mario_set_pos, 0, -500, 0)
    CALL(evt_party_set_pos, 0, 0, -500, 0)
    CALL(evt_snd_sfxoff, LW(6))
    CALL(evt_mario_cont_onoff, 1)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 0)
    DEBUG_PUT_MSG(STRING("ビビアンしっぽ無効"))
    CALL(L_evt_party_vivian_tail, 0, 0)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_move_position, STRING("me"), -50, -40, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("me"), -55, -30, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("me"), -45, 80, 0, FLOAT(80.0f), 1)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("me"), 0, 275, 0, FLOAT(80.0f), 1)
        CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 0, 230, 994, 0, 80, 259, 1000, 11)
    WAIT_MS(1000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(300)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    CALL(evt_msg_print, 0, STRING("stg7_bom_64"), 0, STRING("me"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_64_01"), 0, STRING("ホワイト"))
    SET_INT_READ_ADDR(PTR(oh_data))
    LOOP(4)
        READ_INT2(LW(0), LW(1))
        BEGIN_INLINE_EVENT
            CALL(evt_set_dir_to_target, LW(0), STRING("mario"))
            CALL(evt_msg_print, 0, LW(1), 0, LW(0))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_npc_get_position, LW(0), LW(10), LW(11), LW(12))
            CALL(evt_npc_jump_position_nohit, LW(0), LW(10), LW(11), LW(12), 300, FLOAT(20.0f))
        END_INLINE_EVENT
        WAIT_MS(300)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg7_bom_66"), 0, STRING("me"))
    SET_VALUE(GF(0), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ホワイト"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("ペントリット"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("スラリー"), 512)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg7_bom_67"), 0, STRING("ホワイト"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ホワイト"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg7_bom_67"), 0, STRING("ペントリット"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ペントリット"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("ペントリット"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("スラリー"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("スラリー"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg7_bom_67"), 0, STRING("スラリー"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(1000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ホワイト"), 2)
        CALL(evt_npc_move_position, STRING("ホワイト"), -50, -80, 0, FLOAT(100.0f), 1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ペントリット"), 2)
        CALL(evt_npc_move_position, STRING("ペントリット"), 0, -80, 0, FLOAT(100.0f), 1)
        SET_VALUE(GF(1), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("スラリー"), 2)
        CALL(evt_npc_move_position, STRING("スラリー"), 50, -80, 0, FLOAT(100.0f), 1)
        SET_VALUE(GF(2), 1)
    END_INLINE_EVENT
    LABEL(2)
    IF_INT_EQUAL(GF(0), 0)
        WAIT_FRAMES(1)
        GOTO(2)
    END_IF
    IF_INT_EQUAL(GF(1), 0)
        WAIT_FRAMES(1)
        GOTO(2)
    END_IF
    IF_INT_EQUAL(GF(2), 0)
        WAIT_FRAMES(1)
        GOTO(2)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("ペントリット"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("スラリー"), 512)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ホワイト"), -50, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_move_position, STRING("ホワイト"), -35, -260, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    WAIT_MS(400)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ペントリット"), -50, -80, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), -55, -225, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    WAIT_MS(400)
    CALL(evt_npc_move_position, STRING("スラリー"), -50, -80, 0, FLOAT(80.0f), 1)
    CALL(evt_cam3d_evt_set, 0, 152, 108, 0, 57, -432, 0, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("スラリー"), -70, -180, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    SET_VALUE(LW(0), PTR(door))
    SPAWN_EVENT(evt_door_open_bom_01)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ホワイト"), 0, -320, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ホワイト"), 35, -330, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ホワイト"), 80, -380, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), 70, 30, -435, 300, FLOAT(20.0f))
        CALL(evt_npc_move_position, STRING("ホワイト"), 40, -445, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), 0, 40, -430, 300, FLOAT(20.0f))
        CALL(evt_npc_set_ry, STRING("ホワイト"), 90)
    END_INLINE_EVENT
    WAIT_MS(400)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ペントリット"), 0, -320, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 35, -330, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 80, -380, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 80, -415, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 45, -415, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("ペントリット"), -20, 15, -390, 300, FLOAT(20.0f))
        CALL(evt_npc_set_ry, STRING("ペントリット"), 315)
    END_INLINE_EVENT
    WAIT_MS(400)
    CALL(evt_npc_move_position, STRING("スラリー"), 0, -320, 0, FLOAT(70.0f), 1)
    SET_VALUE(GF(0), 0)
    CALL(evt_npc_move_position, STRING("スラリー"), 35, -330, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_move_position, STRING("スラリー"), 80, -380, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_move_position, STRING("スラリー"), 80, -415, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_move_position, STRING("スラリー"), 45, -415, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_jump_position_nohit, STRING("スラリー"), 20, 15, -390, 300, FLOAT(20.0f))
    CALL(evt_npc_set_ry, STRING("スラリー"), 90)
    CALL(evt_cam3d_evt_set, -160, 125, -100, 10, 55, -427, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_set_dir_to_target, STRING("ホワイト"), STRING("ペントリット"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_68"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("ペントリット"), STRING("ホワイト"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_69"), 0, STRING("ペントリット"))
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("ペントリット"))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE1"), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL(evt_map_playanim, STRING("kikai"), 1, 0)
    WAIT_MS(1000)
    CALL(evt_set_dir_to_target, STRING("ペントリット"), STRING("ホワイト"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_70"), 0, STRING("ペントリット"))
    CALL(evt_npc_set_ry, STRING("ペントリット"), 315)
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("ホワイト"), STRING("スラリー"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_71"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("スラリー"), STRING("ホワイト"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_72"), 0, STRING("スラリー"))
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("スラリー"))
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, 200, 350, 1224, 200, 125, 250, 0, 11)
    CALL_CHILD(cam_outside)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SIREN1"), LW(10))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg7_bom_73"), 0, STRING("ホワイト"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 0, 350, 1224, 0, 125, 250, 6000, 11)
    WAIT_MS(6000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_74"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG7_HUGE_GUN1"))
    CALL(evt_cam3d_evt_set, 0, 350, 1224, 0, 125, 250, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 283, 932, 0, 125, 250, 3000, 11)
    WAIT_MS(1500)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_QUAKE1"), LW(7))
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    WAIT_MS(1500)
    SPAWN_EVENT(effect_1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_QUAKE2"), 0)
    SPAWN_EVENT(gamen_shake2)
    WAIT_MS(300)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yuka3"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("chika_off"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hikari"), 1)
    CALL(evt_mapobj_trans, 1, STRING("chika_off"), 0, FLOAT(0.0f), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_DOWN1"), LW(8))
    CALL(evt_sub_intpl_msec_init, 0, 0, -20, 1000)
    LABEL(3)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_a"), 0, LW(0), 0)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_b"), 0, LW(0), 0)
    ADD(LW(0), -5)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_a"), -20, LW(0), 0)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_b"), 20, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(3)
    END_IF
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_OPEN2"), LW(8))
    CALL(evt_sub_intpl_msec_init, 0, 0, 170, 3000)
    LABEL(4)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_b"), LW(0), -20, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_a"), LW(0), -20, 0)
    ADD(LW(0), -20)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_a"), LW(0), -25, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_b"), LW(0), -25, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(4)
    END_IF
    CALL(evt_snd_sfxoff, LW(8))
    SPAWN_EVENT(gamen_shake2)
    WAIT_MS(600)
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxoff, LW(7))
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 100)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(8))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_75"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, 0, 283, 932, 0, 125, 250, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -200, 734, 684, -10, 50, 63, 2800, 11)
    WAIT_MS(1800)
    SET_VALUE(LW(0), STRING("hiraku_s_1_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_1_b"))
    SET_VALUE(LW(2), 0)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    SET_VALUE(LW(0), STRING("hiraku_s_2_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_2_b"))
    SET_VALUE(LW(2), 1)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    SET_VALUE(LW(0), STRING("hiraku_s_3_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_3_b"))
    SET_VALUE(LW(2), 0)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    SET_VALUE(LW(0), STRING("hiraku_s_4_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_4_b"))
    SET_VALUE(LW(2), 1)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    WAIT_MS(1000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(8))
    CALL_CHILD(cam_inside2)
    WAIT_MS(300)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg7_bom_76"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, -230, 734, 684, -10, 50, 63, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    SET_VALUE(LW(0), 0)
    LOOP(36)
        ADD(LW(0), 1)
        CALL(make_name, LW(0), LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        WAIT_MS(100)
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_cam3d_evt_set, -543, 200, 1194, -16, 175, 60, 3000, 11)
        WAIT_MS(5500)
        CALL(evt_cam3d_evt_set, 0, 70, 650, 0, 60, 250, 0, 11)
    END_INLINE_EVENT
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_UP1"), LW(8))
    CALL(evt_sub_intpl_msec_init, 1, 0, 1600, 11250)
    LABEL(5)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_dekatai"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(5)
    END_IF
    SPAWN_EVENT(effect_1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_STOP1"), 0)
    DELETE_EVENT(LW(9))
    SPAWN_EVENT(gamen_shake2)
    WAIT_MS(2000)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hikari"), 1)
    CALL(evt_cam3d_evt_set, 1500, 100, 2539, 0, 650, 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, -1500, 100, 2539, 0, 650, 0, 5000, 11)
    WAIT_MS(5000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_77"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    SET_VALUE(GW(5), 70)
    SPAWN_EVENT_GET_ID(PTR(evt_depth), LW(9))
    CALL(sight_dir, 3)
    CALL_CHILD(cam_outside)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 62, -676, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, 420, 151, -171, 420, 62, -676, 2000, 11)
    WAIT_MS(2000)
    CALL(sight_dir, 2)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 62, -676, 2000, 11)
    WAIT_MS(2000)
    CALL(sight_dir, 5)
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, -1500, 100, 2539, 0, 650, 0, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_MOVE1"), LW(8))
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 0, -500, 3000)
    LABEL(6)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(6)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_STOP2"), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 500)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(8))
    SET_VALUE(GW(10), 70)
    SPAWN_EVENT_GET_ID(PTR(evt_depth), LW(9))
    CALL(sight_dir, 4)
    CALL_CHILD(cam_outside)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 62, -676, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(300)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg7_bom_78"), 0, 0)
    WAIT_MS(300)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_moon"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_moon"), 400, -738, -676)
    CALL(evt_mapobj_scale, 1, STRING("S_moon"), FLOAT(0.8f), FLOAT(0.8f), FLOAT(0.8f))
    CALL(sight_dir, 0)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 262, -676, 2000, 11)
    WAIT_MS(2000)
    CALL(sight_dir, 4)
    CALL(evt_sub_intpl_msec_init, 11, 70, 0, 1000)
    LABEL(100)
    CALL(evt_sub_intpl_msec_get_value)
    SET_VALUE(GW(5), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(100)
    END_IF
    DELETE_EVENT(LW(9))
    WAIT_MS(2000)
    CALL(sight_dir, 6)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_moon"), 1)
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_79"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, -1500, 100, 2539, 0, 650, 0, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_MOVE1"), LW(8))
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, -500, -400, 1000)
    LABEL(7)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(7)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_STOP2"), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 500)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg7_bom_80"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_HATCH_OPEN1"), LW(8))
    CALL(evt_sub_intpl_msec_init, 11, 0, -1200, 1000)
    LABEL(8)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_rotate, 1, STRING("futa"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(8)
    END_IF
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, 21, 175, 781, 9, 42, 265, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg7_bom_81"), 0, STRING("村長"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_BOMB_TOGETHER1"), LW(8))
    CALL_CHILD(evt_bom1000)
    WAIT_MS(5000)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, -120, 697, 788, 39, 556, 265, 0, 11)
    WAIT_MS(5000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL(bom_flag, 1, 1)
    CALL_CHILD(cam_inside2)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(bom_flag, 0, 1)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, -120, 697, 788, 39, 556, 265, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_BOMB_TOGETHER1_2"), LW(7))
    CALL(evt_msg_toge, 2, 180, 520, 100)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_1"), 0, 0)
    CALL(evt_msg_toge, 2, 100, 400, 95)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_2"), 0, 0)
    CALL(evt_msg_toge, 2, 10, 475, 75)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_3"), 0, 0)
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG7_HATCH_SHUT1"), LW(8))
        CALL(evt_sub_intpl_msec_init, 11, -1200, 0, 3000)
        LABEL(9)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, STRING("futa"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(9)
        END_IF
        CALL(evt_snd_sfxoff, LW(8))
        CALL(evt_snd_sfxoff, LW(7))
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_msg_toge, 2, 180, 520, 100)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_4"), 0, 0)
    CALL(evt_msg_toge, 2, 100, 400, 95)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_5"), 0, 0)
    CALL(evt_msg_toge, 2, 10, 475, 75)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_6"), 0, 0)
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("村長"))
    CALL(bom_flag, 1, 1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_86"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, 21, 175, 781, 9, 42, 265, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_87"), 0, STRING("村長"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 200, 0, 2483, 200, 550, 0, 0, 11)
    SPAWN_EVENT(mario_shot)
    WAIT_MS(800)
    CALL_CHILD(effect_delete)
    CALL(evt_cam3d_evt_set, 806, 1189, 451, 600, 982, 69, 0, 11)
    SPAWN_EVENT(mario_shot)
    WAIT_MS(800)
    CALL_CHILD(effect_delete)
    CALL(evt_cam3d_evt_set, -605, 170, 785, -25, 334, 227, 0, 11)
    SPAWN_EVENT(mario_shot)
    WAIT_MS(800)
    CALL_CHILD(effect_delete)
    SPAWN_EVENT_GET_ID(PTR(mario_kemuri), LW(9))
    CALL(evt_mario_set_pos, 200, 0, -650)
    CALL(evt_party_set_pos, 0, 200, 0, -650)
    CALL(evt_cam3d_evt_set, 370, 245, -173, 370, 62, -1205, 0, 11)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HAIKEI"), 1)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_DAMAGED2"), 200, 0, -650, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_AC3_4"), 200, 0, -650, LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_MARIO_FLY1"), 200, 0, -650, LW(5))
    SET_FLOAT_VALUE(LW(6), FLOAT(0.0f))
    CALL(evt_sub_spline_init, 1, 11, PTR(spline_dt), 3, 0, 2000)
    LABEL(20)
    CALL(evt_sub_spline_get_value, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfx_pos, LW(4), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfx_pos, LW(5), LW(0), LW(1), LW(2))
    ADD(LW(0), -20)
    ADD(LW(1), -20)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(3), 1)
        GOTO(20)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxoff, LW(5))
    WAIT_FRAMES(1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    WAIT_MS(1000)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_MARIO_LIGHT1"), 0)
    CALL(evt_eff64, 0, STRING("wangflush_n64"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_toge, 2, 340, 200, -590)
        CALL(evt_msg_print, 0, STRING("stg7_bom_88"), 0, 0)
        CALL(evt_msg_toge, 2, 400, 160, -590)
        CALL(evt_msg_print, 0, STRING("stg7_bom_89"), 0, 0)
        CALL(evt_msg_toge, 2, 373, 135, -590)
        CALL(evt_msg_print, 0, STRING("stg7_bom_90"), 0, 0)
    END_INLINE_EVENT
    CALL(bom_flag, 0, 1)
    CALL(bom_fall)
    WAIT_MS(6000)
    CALL(evt_ext_reset)
    CALL(evt_npc_set_ry, STRING("村長"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("村長"), 32768)
    CALL(evt_cam3d_evt_set, -60, 26, 493, 22, 33, 257, 0, 11)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("stg7_bom_91"), 0, STRING("村長"))
    CALL(evt_snd_bgmoff_f, 512, 2000)
    WAIT_MS(300)
    WAIT_MS(2000)
    CALL(evt_pouch_remove_item, 82, LW(0))
    CALL(evt_bero_mapchange, STRING("moo_03"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(init_soncho)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(356)
        CALL(evt_npc_set_position, STRING("me"), -15, 0, -35)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_soncho)
    SWITCH(GSW(0))
        CASE_LESS(357)
        CALL_CHILD(evt_taihou)
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_04_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_04_2"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_nitoro)
    CALL(evt_npc_set_position, STRING("me"), -120, 0, -220)
    RETURN
EVT_END()

EVT_BEGIN(talk_nitoro)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_02_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_02"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_02_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_03"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_03_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_pentrit)
    CALL(evt_npc_set_position, STRING("me"), -150, 0, 130)
    RETURN
EVT_END()

EVT_BEGIN(talk_pentrit)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_04_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_04"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_04_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_05"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_05_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_surary)
    CALL(evt_npc_set_position, STRING("me"), 152, 0, -188)
    RETURN
EVT_END()

EVT_BEGIN(talk_surary)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_06_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_06"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_06_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_07_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_murabito_d)
    CALL(evt_npc_set_position, STRING("me"), 175, 0, 210)
    RETURN
EVT_END()

EVT_BEGIN(talk_murabito_d)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_08_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_08"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_08_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_09"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_e)
    CALL(evt_npc_set_position, STRING("me"), -200, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_e)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_10_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_10"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_11"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_f)
    CALL(evt_npc_set_position, STRING("me"), -150, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_f)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_12_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_12"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_13"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_g)
    CALL(evt_npc_set_position, STRING("me"), -100, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_g)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_14_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_14"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_15"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_h)
    CALL(evt_npc_set_position, STRING("me"), -50, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_h)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_16_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_16"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_17"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_i)
    CALL(evt_npc_set_position, STRING("me"), 0, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_i)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_18_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_18"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_19"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_white)
    IF_INT_EQUAL(GSW(1420), 25)
        IF_INT_EQUAL(GSWF(5411), 0)
            RETURN
        END_IF
    END_IF
END_IF
IF_INT_EQUAL(GSWF(3876), 1)
    CALL(evt_npc_set_position, STRING("me"), 50, 0, -100)
END_IF
RETURN
EVT_END()

static EVT_BEGIN(talk_white)
    SWITCH(GSW(0))
        CASE_LESS(357)
        IF_INT_EQUAL(GSWF(3874), 0)
            CALL(evt_msg_print, 0, STRING("stg7_bom_59_02"), 0, 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg7_bom_59_03"), 0, 0)
        END_IF
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_04"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_05"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(musume_talk)
    SWITCH(GSW(0))
        CASE_LESS(354)
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_01"), 0, STRING("３人娘Ａ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_02"), 0, STRING("３人娘Ｂ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_03"), 0, STRING("３人娘Ｃ"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_04"), 0, STRING("３人娘Ａ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_05"), 0, STRING("３人娘Ｂ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_06"), 0, STRING("３人娘Ｃ"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_07"), 0, STRING("３人娘Ａ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_08"), 0, STRING("３人娘Ｂ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_09"), 0, STRING("３人娘Ｃ"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(musume_a_entry)
    CALL(evt_npc_entry, STRING("３人娘Ａ"), STRING("c_dance_r"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ａ"), STRING("踊り子"))
    CALL(evt_npc_set_position, STRING("３人娘Ａ"), 370, 5, -120)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ａ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ａ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

static EVT_BEGIN(musume_b_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｂ"), STRING("c_dance_p"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｂ"), STRING("踊り子ピンク"))
    CALL(evt_npc_set_position, STRING("３人娘Ｂ"), 400, 5, -150)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ｂ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ｂ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

static EVT_BEGIN(musume_c_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｃ"), STRING("c_dance_k"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｃ"), STRING("踊り子黄緑"))
    CALL(evt_npc_set_position, STRING("３人娘Ｃ"), 430, 5, -100)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ｃ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ｃ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[161] = {
    (u32)"村長", 0x40000600, 0x00000000, (u32)&init_soncho,
    0x00000000, (u32)&talk_soncho, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ニトロ",
    0x40000600, 0x00000000, (u32)&init_nitoro, 0x00000000,
    (u32)&talk_nitoro, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ペントリット", 0x40000600,
    0x00000000, (u32)&init_pentrit, 0x00000000, (u32)&talk_pentrit,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"スラリー", 0x40000600, 0x00000000,
    (u32)&init_surary, 0x00000000, (u32)&talk_surary, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"ボムヘイD", 0x40000600, 0x00000000, (u32)&init_murabito_d,
    0x00000000, (u32)&talk_murabito_d, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ホワイト",
    0x40000600, 0x00000000, (u32)&init_white, 0x00000000,
    (u32)&talk_white, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: write ext_dispent_minnnanokoe (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_init_minnnanokoe (global, 0x318 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write make_ext_table_minnnanokoe (global, 0x124 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(bom1000_minnnanokoe)
    CALL(make_ext_table_minnnanokoe)
    CALL(evt_npc_entry, STRING("dummy"), STRING("c_bom_sen"))
    CALL(evt_npc_set_position, STRING("dummy"), 0, -1000, 0)
    CALL(evt_ext_entry, 250, PTR(ext_data), PTR(ext_init_minnnanokoe), PTR(ext_main), PTR(ext_dispent_minnnanokoe))
    RETURN
EVT_END()

// TODO: write starstone_set_ry (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write starstone_set_pos (static, 0x9c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write starstone_entry (static, 0xb0 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write anim_tevcallback (static, 0x70 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write yami_view (static, 0xc4 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write bom1000_jump (global, 0x200 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write bom1000_msg (global, 0xfc bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(minnnanokoe_evt)
    SPAWN_EVENT(bom1000_minnnanokoe)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yuka1"), 1)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("村長"), 520, 0, 0)
    CALL(evt_npc_set_ry, STRING("村長"), 270)
    CALL(evt_npc_status_onoff, 1, STRING("村長"), 2)
    CALL(evt_npc_set_position, STRING("ニトロ"), -30, 0, -180)
    CALL(evt_npc_set_ry, STRING("ニトロ"), 270)
    CALL(evt_npc_set_position, STRING("ペントリット"), 0, 0, -180)
    CALL(evt_npc_set_ry, STRING("ペントリット"), 270)
    CALL(evt_npc_set_position, STRING("スラリー"), 30, 0, -180)
    CALL(evt_npc_set_ry, STRING("スラリー"), 90)
    CALL(evt_npc_set_position, STRING("ボムヘイD"), 570, 0, 25)
    CALL(evt_npc_set_ry, STRING("ボムヘイD"), 270)
    CALL(evt_npc_status_onoff, 1, STRING("ボムヘイD"), 2)
    CALL(evt_npc_set_position, STRING("ホワイト"), 0, 0, -230)
    CALL(evt_npc_set_ry, STRING("ホワイト"), 270)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bom01"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bom01"), 256)
    CALL(anim_tevcallback, STRING("村長"), 1)
    CALL(anim_tevcallback, STRING("ニトロ"), 1)
    CALL(anim_tevcallback, STRING("ペントリット"), 1)
    CALL(anim_tevcallback, STRING("スラリー"), 1)
    CALL(anim_tevcallback, STRING("ボムヘイD"), 1)
    CALL(anim_tevcallback, STRING("ホワイト"), 1)
    CALL(yami_view, -1500)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(starstone_entry, STRING("starstone"), 0, 50, 10)
    SET_VALUE(GW(0), 50)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(starstone_set_pos, STRING("starstone"), 0, GW(0), 10)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(starstone_set_ry, STRING("starstone"), LW(0))
            SUBTRACT_FLOAT(LW(0), FLOAT(3.0f))
            IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 5000, 5500, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
            CALL(evt_sub_intpl_msec_init, 11, 5500, 5000, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(2)
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 400, 106, 331, 400, 45, -13, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(500)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ボムヘイD"), 450, 25, 0, 90, 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("村長"), 400, 0, 0, 90, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_65"), 0, STRING("村長"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_66"), 0, STRING("村長"))
    CALL(evt_cam3d_evt_set, 0, 283, 1182, 0, 75, 0, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_67"), 0, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(bom1000_jump)
            WAIT_MS(100)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(14), 0)
        SET_VALUE(LW(15), 3)
        LOOP(0)
            CALL(bom1000_msg, LW(14), LW(15), LW(0), LW(2))
            CALL(evt_sub_random, 2, LW(1))
            ADD(LW(1), 1)
            MULTIPLY(LW(1), 30)
            CALL(evt_sub_random, 20, LW(3))
            SUBTRACT(LW(3), 10)
            ADD(LW(1), LW(3))
            CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
            CALL(evt_sub_random, 2, LW(3))
            SWITCH(LW(3))
                CASE_EQUAL(0)
                SET_VALUE(LW(3), STRING("stg8_las_148_68"))
                CASE_EQUAL(1)
                SET_VALUE(LW(3), STRING("stg8_las_148_69"))
                CASE_EQUAL(2)
                SET_VALUE(LW(3), STRING("stg8_las_148_70"))
            END_SWITCH
            BEGIN_INLINE_EVENT
                CALL(evt_msg_print, 0, LW(3), 0, 0)
            END_INLINE_EVENT
            WAIT_MS(300)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, 400, 106, 331, 400, 45, -13, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("村長"), 90)
    WAIT_MS(300)
    CALL(evt_npc_set_ry, STRING("ボムヘイD"), 90)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ボムヘイD"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_71"), 0, STRING("村長"))
    WAIT_MS(2000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("las_29"), STRING("minnnanokoe"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_door)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_bom_09_01"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write sight (global, 0x190 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(bom_01_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL_CHILD(evt_bero_info_run)
        CASE_LESS(353)
        CALL_CHILD(bero_case_entry)
        CASE_DEFAULT
        CALL_CHILD(evt_bero_info_run)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 400)
        SET_VALUE(LF(0), 0)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_VOICE1"))
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_BOM1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_BOM2"))
    END_IF
    SET_VALUE(LW(0), STRING("group10"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group11"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group12"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group13"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group14"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group15"))
    SPAWN_EVENT(evt_bom_01_karakara)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("chika_off"), 1)
    CALL(evt_hitobj_onoff, STRING("A_chika"), 1, 0)
    CALL_CHILD(evt_bom_snowfall)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_moon"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka3"), 1)
    CALL(evt_mapobj_trans, 1, STRING("chika_off"), 0, FLOAT(-1.0f), 0)
    CALL(evt_npc_setup, PTR(npcEnt))
    SWITCH(GSW(0))
        CASE_BETWEEN(352, 357)
        CALL_CHILD(musume_a_entry)
        CALL_CHILD(musume_b_entry)
        CALL_CHILD(musume_c_entry)
    END_SWITCH
    CALL(evt_run_case_evt, 9, 1, STRING("A_ie_door"), 0, PTR(evt_door), 0)
    CALL(evt_item_entry, STRING("item_00"), 125, -180, 5, 450, 16, GSWF(5659), 0)
    SWITCH(GSW(0))
        CASE_LESS(349)
        SET_VALUE(GSWF(3885), 1)
        CASE_LESS(353)
        SPAWN_EVENT(first_evt)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SPAWN_EVENT(minnnanokoe_evt)
        END_IF
    END_IF
    SPAWN_EVENT(evt_sub_mail_event)
    BEGIN_INLINE_EVENT
        CALL(sight)
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: type
u32 lp[1] = {
    0x00000000,
};

// TODO: name$915 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 name$915[16] = {
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
// };

// TODO: type
u32 sight_wp[1] = {
    0x00000000,
};

// TODO: write ext_disp_minnnanokoe (global, 0xb4 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write jump_minnnanokoe (global, 0x1cc bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write depthDisp (static, 0x62c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write jump (global, 0x250 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_disp (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write pose_change (static, 0xc8 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write pose_main (static, 0x180 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write pose_init (static, 0xfc bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write disp_sight (global, 0x16fc bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write drawNum (global, 0x224 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it

// TODO: write drawBG (global, 0x30c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom_01.s; this is where the original has it, just before the first script that uses it
// clang-format on

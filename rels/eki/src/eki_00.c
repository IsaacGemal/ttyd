#include "eki_00.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_ext.h"
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
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static void ext_moamoa_dispent(void);  // TODO: real signature
static void ext_moamoa_main(void);  // TODO: real signature
static void moamoa_hit_check(void);  // TODO: real signature
static void moamoa_jump(void);  // TODO: real signature
static void ext_shadow_disp(void);  // TODO: real signature
static void ext_disp(void);  // TODO: real signature
static void ext_moamoa_init(void);  // TODO: real signature
static void randf(void);  // TODO: real signature
static void evt_moamoa_target_off(void);  // TODO: real signature
static void evt_moamoa_fusion_debug(void);  // TODO: real signature
static void evt_moamoa_blow(void);  // TODO: real signature
static void evt_moamoa_kind_onoff(void);  // TODO: real signature
static void evt_moamoa_fusion(void);  // TODO: real signature
static USER_FUNC(evt_moamoa_end_wait);
static void evt_moamoa_ry_reset(void);  // TODO: real signature
static USER_FUNC(evt_moamoa_exit);
static USER_FUNC(evt_moamoa_flag_onoff);
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
static u32 ext_moamoa_data[141] = {
    (u32)"c_n_moa_a", (u32)"S_1", 0x00000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x40000000, (u32)"c_n_moa_a", (u32)"S_1",
    0x40800000, (u32)"c_n_moa_a", (u32)"S_1", 0x40C00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41200000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41400000, (u32)"c_n_moa_a", (u32)"S_1", 0x41600000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41800000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41900000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41A00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41B00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41C00000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41D00000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41E00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41F00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x42000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x42080000, (u32)"c_n_moa_a", (u32)"S_1",
    0x42100000, (u32)"c_n_moa_a", (u32)"S_1", 0x42180000,
    (u32)"c_n_moa_a", (u32)"D_1", 0x00000000, (u32)"c_n_moa_a",
    (u32)"D_1", 0x40800000, (u32)"c_n_moa_a", (u32)"D_1",
    0x41000000, (u32)"c_n_moa_b", (u32)"S_1", 0x00000000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x40000000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x40800000, (u32)"c_n_moa_b", (u32)"S_1",
    0x40C00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41000000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41200000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41400000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41600000, (u32)"c_n_moa_b", (u32)"S_1", 0x41800000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41900000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41A00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41B00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41C00000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41D00000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41E00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41F00000, (u32)"c_n_moa_b", (u32)"S_1", 0x42000000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x42080000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x42100000, (u32)"c_n_moa_b", (u32)"S_1",
    0x42180000, (u32)"c_n_moa_b", (u32)"D_1", 0x00000000,
    (u32)"c_n_moa_b", (u32)"D_1", 0x40800000, (u32)"c_n_moa_b",
    (u32)"D_1", 0x41000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: group_tbl$588 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 group_tbl$588[22] = {
//     (u32)"c_n_moa_a", (u32)"S_1", (u32)"c_n_moa_b", (u32)"S_1",
//     (u32)"c_kinodebu", (u32)"G_1", (u32)"c_puku_c", (u32)"U_1",
//     (u32)"c_powan", (u32)"O_1", (u32)"c_smoko", (u32)"T_2",
//     (u32)"c_rose", (u32)"G_1", (u32)"c_papab", (u32)"G_1",
//     (u32)"c_mamab", (u32)"G_1", (u32)"c_konari", (u32)"G_1",
//     (u32)"c_n_moa_a", (u32)"S_1",
// };

// TODO: mt$758 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 mt$758[12] = {
//     0x41000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x41000000, 0x00000000, 0x3F000000,
//     0x00000000, 0x00000000, 0x41000000, 0x00000000,
// };

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("e_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("e_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[3] = {
    {"e_bero", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_01", "w_dero", 5, 5, e_door_close, e_door_open},
    {"s_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hom_00", "n_bero_2", 5, 5, s_door_close, s_door_open},
    {NULL},
};

EVT_BEGIN(evt_bridge)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    SET_VALUE(GSWF(3576), 1)
    CALL(evt_bero_mapchange, STRING("hom_00"), STRING("n_bero_2"))
    RETURN
EVT_END()

EVT_BEGIN(evt_bridge2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_00"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_00"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_ori"), 2, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidann"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ori"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kaidan"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_ori"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kabe"), 1)
    CALL(evt_map_playanim, STRING("anm_ere_1"), 2, 0)
    CALL(evt_mario_set_pos, -210, 60, 0)
    CALL(evt_party_set_pos, 0, -230, 60, 0)
    CALL(evt_item_delete_check, STRING("item"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(1), 2000)
        CALL(evt_item_set_position, STRING("item"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_mapobj_rotate, 1, STRING("s_lever"), 0, 0, -80)
    SET_VALUE(GF(0), 1)
    SET_VALUE(GSW(0), 325)
    SET_VALUE(GSWF(3576), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_origami)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mapobj_get_position, STRING("s_lever"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_SWITCH_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -800, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, STRING("s_lever"), 0, 0, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam3d_evt_set, 340, 80, 280, -130, 90, -15, 1000, 11)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 1)
    SUBTRACT(LW(2), 1)
    CALL(evt_mario_mov_pos, LW(0), LW(2), 30)
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ORIGAMI_APPEAR1"), -220, 60, 0, 0)
    CALL(evt_map_playanim, STRING("anm_ori"), 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidann"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ori"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kaidan"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_ori"), 1)
    CALL(evt_hitobj_onoff, STRING("kaidann_A"), 1, 1)
    CALL(evt_map_checkanim, STRING("anm_ori"), LW(0), LW(1))
    WAIT_MS(1600)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kabe"), 1)
    WAIT_MS(1000)
    CALL(evt_mobj_switch_blue, 0, STRING("bridge_sw"), -220, 60, 0, PTR(evt_bridge), GSWF(3725))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_APPEAR1"), -220, 60, 0, 0)
    CALL(evt_mobj_set_scale, STRING("bridge_sw"), FLOAT(0.1f), FLOAT(0.1f), FLOAT(0.1f))
    CALL(evt_sub_intpl_msec_init, 8, 100, 1000, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    CALL(evt_mobj_set_scale, STRING("bridge_sw"), LW(0), LW(0), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GSW(0), 324)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

// TODO: write ext_moamoa_dispent (static, 0xdc bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_moamoa_main (static, 0x31c0 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_moamoa_init (static, 0x624 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_moamoa_init)
    CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
    CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
    CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
    CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
    CALL(evt_ext_entry, 150, PTR(ext_moamoa_data), PTR(ext_moamoa_init), PTR(ext_moamoa_main), PTR(ext_moamoa_dispent))
    RETURN
EVT_END()

static EVT_BEGIN(evt_meet_moamoa)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_6"), 0, STRING("party"))
    END_SWITCH
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 32, 151, 528, 32, 56, -12, 500, 11)
    WAIT_MS(2000)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_dispflag_onoff, 1, 2)
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
    END_INLINE_EVENT
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_6"), 0, STRING("party"))
    END_SWITCH
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 322)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

// TODO: write evt_moamoa_end_wait (static, 0x60 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_exit (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_moamoa_escape)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 0, 151, 528, 0, 56, -12, 500, 11)
    CALL(evt_snd_bgmoff_f, 512, 180)
    BEGIN_INLINE_EVENT
        CALL(evt_moamoa_end_wait)
        CALL(evt_moamoa_exit)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_run_case_evt, 9, 1, STRING("a_lever"), 0, PTR(evt_origami), GW(10))
    SET_VALUE(GSW(0), 323)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_elv_rumble)
    IF_INT_EQUAL(GF(0), 0)
        CALL(evt_map_checkanim, STRING("anm_ere_1"), LW(0), LW(1))
    ELSE
        CALL(evt_map_checkanim, STRING("anm_ere_2"), LW(0), LW(1))
    END_IF
    SUBTRACT(LW(1), 500)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_SHAKE1"), LW(3), LW(4), LW(5), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 250)
    WAIT_MS(LW(1))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_SHAKE1"), LW(3), LW(4), LW(5), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 250)
    RETURN
EVT_END()

static EVT_BEGIN(evt_elv_init)
    CALL(evt_paper_entry, STRING("OFF_hei_07"))
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_hei_07"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 2, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_clear_virtual_point, STRING("capture_img"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    SET_VALUE(GF(0), 0)
    RETURN
EVT_END()

// TODO: write img_set_color (global, 0xdc bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_flag_onoff (static, 0x84 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_elv_down)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    WAIT_FRAMES(20)
    CALL(evt_mario_mov_pos2, 190, -65, FLOAT(60.0f))
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(1))
    ADD(LW(1), 1)
    WAIT_MS(LW(1))
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 190, -65, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
        CALL(evt_party_jump_pos, 0, 160, 0, -160, 500, 0, -1000)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 190, -160, FLOAT(60.0f))
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_00"), STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_07"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_00"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_MOVE1"), LW(0), LW(1), LW(2), LW(10))
        CALL(evt_map_checkanim, STRING("anm_ere_1"), LW(0), LW(1))
        WAIT_MS(LW(1))
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_1"), 0, 0)
    SPAWN_EVENT(evt_elv_rumble)
    CALL(evt_map_checkanim, STRING("anm_ere_1"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(400)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_00"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_07"), STRING("offscreen"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 0, 1)
        CALL(evt_moamoa_flag_onoff, 1, 2)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 1, 1)
        CALL(evt_moamoa_flag_onoff, 0, 2)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_00"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_00"), 1, 0)
    SWITCH(GSW(0))
        CASE_LESS(323)
        CASE_EQUAL(323)
        CALL(evt_run_case_evt, 9, 1, STRING("a_lever"), 0, PTR(evt_origami), GW(10))
        CASE_DEFAULT
        CALL(evt_mapobj_rotate, 1, STRING("s_lever"), 0, 0, -80)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 324)
        CALL(evt_mobj_switch_blue, 0, STRING("bridge_sw"), -220, 60, 0, PTR(evt_bridge), GSWF(3725))
    END_IF
    CALL(evt_item_delete_check, STRING("item"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(1), 2000)
        CALL(evt_item_set_position, STRING("item"), LW(0), LW(1), LW(2))
    END_IF
    IF_INT_LESS(GSW(0), 324)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kaidann"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ori"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_kaidan"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_ori"), 1)
        CALL(evt_hitobj_onoff, STRING("kaidann_A"), 1, 0)
    ELSE
        CALL(evt_map_playanim, STRING("anm_ori"), 2, 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidann"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ori"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kaidan"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_ori"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kabe"), 1)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        WAIT_MS(100)
        CALL(evt_party_move_pos2, 0, 190, -65, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
        WAIT_MS(100)
        CALL(evt_party_move_pos2, 0, 150, -65, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_mario_mov_pos2, 190, -65, FLOAT(60.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    WAIT_MS(500)
    IF_INT_LESS(GSW(0), 322)
        CALL_CHILD(evt_meet_moamoa)
    END_IF
    IF_INT_EQUAL(GSW(0), 322)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
    END_IF
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(1))
    WAIT_MS(LW(1))
    RETURN
EVT_END()

EVT_BEGIN(evt_elv_up)
    CALL(evt_snd_bgmoff, 512)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    WAIT_FRAMES(20)
    CALL(evt_mario_mov_pos2, 190, -65, FLOAT(60.0f))
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(1))
    ADD(LW(1), 1)
    WAIT_MS(LW(1))
    BEGIN_INLINE_EVENT
        CALL(evt_hitobj_onoff, STRING("ere__A"), 0, 0)
        CALL(evt_party_move_pos, 0, 190, -65, 500)
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos, 0, 160, -160, 500)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir, 0, 90, 0)
        CALL(evt_hitobj_onoff, STRING("ere__A"), 0, 1)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos, 190, -160, 500)
    CALL(evt_mario_set_dir, 270, 0, 0)
    WAIT_MS(500)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_00"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_07"), STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_07"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 0, 1)
        CALL(evt_moamoa_flag_onoff, 1, 2)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 0, 2)
    END_IF
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_MOVE1"), LW(0), LW(1), LW(2), LW(10))
        CALL(evt_map_checkanim, STRING("anm_ere_2"), LW(0), LW(1))
        WAIT_MS(LW(1))
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_2"), 0, 0)
    SPAWN_EVENT(evt_elv_rumble)
    CALL(evt_map_checkanim, STRING("anm_ere_2"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(400)
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_00"), STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_07"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    IF_INT_LESS(GSW(0), 324)
    END_IF
    IF_INT_EQUAL(GSW(0), 324)
        CALL(evt_mobj_delete, STRING("bridge_sw"))
    END_IF
    CALL(evt_item_delete_check, STRING("item"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item"), LW(0), LW(1), LW(2))
        ADD(LW(1), 2000)
        CALL(evt_item_set_position, STRING("item"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_eki_00"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_00"), 1, 1)
    IF_INT_EQUAL(GSW(0), 323)
        CALL(evt_del_case_evt, 0, GW(10))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_party_jump_pos, 0, 190, 0, -65, 500, 0, -1000)
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos, 0, 150, -65, 1000)
        CALL(evt_party_stop, 0)
    END_INLINE_EVENT
    WAIT_FRAMES(30)
    CALL(evt_mario_mov_pos, 190, -65, 500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(1))
    WAIT_MS(LW(1))
    RETURN
EVT_END()

// TODO: type
static u32 item_tbl[2] = {
    0x0000001A, 0xFFFFFFFF,
};

EVT_BEGIN(evt_elv)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_LESS(GSW(0), 321)
        CALL(evt_win_item_select, 0, PTR(item_tbl), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        CALL(evt_pouch_remove_item, 26, LW(0))
        SET_VALUE(GSW(0), 321)
        CALL(evt_snd_sfxon, STRING("SFX_STG6_ELEVATOR_KEY1"), 0)
        CALL(evt_msg_print, 0, STRING("stg6_eki_07"), 0, 0)
    END_IF
    IF_INT_EQUAL(GF(0), 0)
        CALL_CHILD(evt_elv_down)
        SET_VALUE(GF(0), 1)
    ELSE
        CALL_CHILD(evt_elv_up)
        SET_VALUE(GF(0), 0)
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_elv_nomsg)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg6_eki_06"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_head)
    CALL(evt_item_delete, STRING("item"))
    CALL(evt_item_entry, STRING("item"), 300, -260, 160, -60, 4, GSWF(3745), 0)
    CALL(evt_item_set_move_dir_speed, STRING("item"), FLOAT(135.0f), FLOAT(90.0f))
    CALL(evt_item_set_gravity, STRING("item"), FLOAT(1.0f))
    CALL(evt_item_set_bound_rate, STRING("item"), FLOAT(0.6f))
    CALL(evt_item_set_jump_power, STRING("item"), FLOAT(500.0f))
    CALL(evt_item_status_on, STRING("item"), 16)
    CALL(evt_item_flag_on, STRING("item"), 2048)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(evt_kanban)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg6_eki_00_01"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(eki_00_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    IF_INT_EQUAL(GSWF(3576), 0)
        CALL_CHILD(evt_bero_info_run)
    ELSE
        CALL_CHILD(bero_case_entry)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 0)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 320)
        CALL_CHILD(evt_elv_init)
        CALL(evt_run_case_evt, 9, 1, STRING("a_suitti"), 0, PTR(evt_elv), 0)
    ELSE
        CALL(evt_run_case_evt, 9, 1, STRING("a_suitti"), 0, PTR(evt_elv_nomsg), 0)
    END_IF
    IF_INT_LESS(GSW(0), 323)
        CALL_CHILD(evt_moamoa_init)
    END_IF
    CALL(evt_item_entry, STRING("item"), 300, -260, 130, -60, 17, GSWF(3745), 0)
    IF_INT_EQUAL(GSWF(3745), 0)
        CALL(evt_run_case_evt, 16, 1, STRING("a_dai_sita"), 0, PTR(evt_head), 0)
    END_IF
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI2"))
    IF_INT_EQUAL(GSWF(3576), 1)
        SPAWN_EVENT(evt_bridge2)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("a_kan"), 0, PTR(evt_kanban), 0)
    RETURN
EVT_END()

// TODO: type
static u32 jumptable_00002400[66] = {
    (u32)((u8*)&ext_moamoa_main + 0xcf0), (u32)((u8*)&ext_moamoa_main + 0x1770), (u32)((u8*)&ext_moamoa_main + 0x1a68), (u32)((u8*)&ext_moamoa_main + 0x1ccc),
    (u32)((u8*)&ext_moamoa_main + 0x1f28), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2744), (u32)((u8*)&ext_moamoa_main + 0x2810), (u32)((u8*)&ext_moamoa_main + 0x2834),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x1f4c), (u32)((u8*)&ext_moamoa_main + 0x1f4c), (u32)((u8*)&ext_moamoa_main + 0x1f4c), (u32)((u8*)&ext_moamoa_main + 0x2250),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x22b0),
    (u32)((u8*)&ext_moamoa_main + 0x2600), (u32)((u8*)&ext_moamoa_main + 0x2690), (u32)((u8*)&ext_moamoa_main + 0x14d8), (u32)((u8*)&ext_moamoa_main + 0x1504),
    (u32)((u8*)&ext_moamoa_main + 0x15cc), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2b54), (u32)((u8*)&ext_moamoa_main + 0x2b7c),
    (u32)((u8*)&ext_moamoa_main + 0x2e08), (u32)((u8*)&ext_moamoa_main + 0x2e2c),
};

// TODO: type
static u32 wp[1] = {
    0x00000000,
};

// TODO: write moamoa_hit_check (static, 0xb28 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_jump (static, 0x270 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_shadow_disp (static, 0x90 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_disp (static, 0x9c bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write randf (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_target_off (static, 0xa8 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_fusion_debug (static, 0x1dc bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_blow (static, 0x74 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_kind_onoff (static, 0x220 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_fusion (static, 0x2a4 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_ry_reset (static, 0x60 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_pose_change (static, 0x168 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_pose_main (static, 0x17c bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_pose_init (static, 0x1c8 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it
// clang-format on

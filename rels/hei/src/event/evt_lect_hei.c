#include "evt_lect_hei.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_img.h"
#include "evt/evt_lecture.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static void bgm_start_wait(void);  // TODO: real signature
static USER_FUNC(set_dir);
static USER_FUNC(lect_party_ido_sum);
static void lect_party_ido_sub(void);  // TODO: real signature
static void efffukidashichange_cam(void);  // TODO: real signature
static void lect_npc_balloon_on_off(void);  // TODO: real signature
static USER_FUNC(lect_itemget_kesan);
static void lect_mobj_hit_nasi(void);  // TODO: real signature
static USER_FUNC(effitemget_busaiku);
static void effitemget_open_ptr(void);  // TODO: real signature
static USER_FUNC(effitemget_open);
static void effitemgetchange_cam_ptr(void);  // TODO: real signature
static USER_FUNC(effitemgetchange_cam);
static void lect_map_cam_chg(void);  // TODO: real signature
static void lect_map_cam_chg_sub(void);  // TODO: real signature
static void lecture_button_set_pos(void);  // TODO: real signature
static void lecture_button_seq_wait(void);  // TODO: real signature
static void lecture_button_get_icon_id(void);  // TODO: real signature
static void lecture_button_set_seq(void);  // TODO: real signature
static void lecture_button_flag_on_off(void);  // TODO: real signature
static void lecture_button_set_alpha(void);  // TODO: real signature
static void lecture_button_set_icon_id(void);  // TODO: real signature
static void lecture_button_rtn(void);  // TODO: real signature
static void lecture_button_disp(void);  // TODO: real signature
static USER_FUNC(x_flag_wait);
static void x_flag_get(void);  // TODO: real signature
static void x_flag_add(void);  // TODO: real signature
static USER_FUNC(x_flag_set);
static void lect_status_on(void);  // TODO: real signature
static void lect_status_off(void);  // TODO: real signature
static void lect_mario_ido_sum(void);  // TODO: real signature
static void lect_mario_ido_sub(void);  // TODO: real signature

// clang-format off
extend_data(116,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(117,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write lect_party_ido_sum (static, 0x29c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(nokotaro_party_lecture_init)
    CALL(evt_mario_set_pos, 350, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 40)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(90.0f), FLOAT(520.0f), FLOAT(-20.0f), FLOAT(26.0f), FLOAT(-140.0f), 1000, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(lect_party_ido_sum, 0, 1000)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_set_dir, 0, 90, 300)
    RETURN
EVT_END()

// TODO: write x_flag_wait (static, 0x68 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_set (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(nokotaro_party_lecture)
    CALL(evt_msg_print, 0, STRING("lecture_friends_04_0_1"), 0, 0)
    CALL(evt_msg_print, 0, STRING("lecture_friends_04_1"), 0, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(100.0f), FLOAT(630.0f), FLOAT(45.0f), FLOAT(39.0f), FLOAT(-100.0f), 1000, 0)
    WAIT_MS(1500)
    CALL(evt_cam_letter_box_disable, 1)
    LOOP(2)
        CALL(evt_party_nokotaro_kick, 0)
        WAIT_MS(500)
    END_LOOP
    CALL(evt_npc_change_fbat_mode, 8)
    SET_VALUE(LW(8), 2)
    LOOP(LW(8))
        SWITCH(LW(8))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("lecture_friends_05"))
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("lecture_friends_05_2"))
        END_SWITCH
        SET_VALUE(LW(1), STRING("TESTMOBJ"))
        CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
        ADD(LW(2), -70)
        ADD(LW(3), 100)
        SET_VALUE(GSWF(1803), 0)
        CALL(evt_mobj_switch_blue, 0, LW(1), LW(2), LW(3), LW(4), 0, GSWF(1803))
        CALL(evt_mobj_set_camid, LW(1), 7)
        CALL(evt_snd_sfxon, STRING("SFX_EVT_LECTURE_BLOCK_FALL1"), LW(7))
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(evt_mobj_get_position, LW(1), LW(2), LW(3), LW(4))
                IF_INT_LESS_OR_EQUAL(LW(3), 10)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_snd_sfxon, STRING("SFX_EVT_LECTURE_BLOCK_DOWN1"), 0)
            CALL(evt_snd_sfxoff, LW(7))
        END_INLINE_EVENT
        SET_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(evt_mobj_get_position, LW(1), LW(2), LW(3), LW(4))
            ADD_FLOAT(LW(0), FLOAT(5.0f))
            SUBTRACT_FLOAT(LW(3), LW(0))
            IF_INT_LESS_OR_EQUAL(LW(3), FLOAT(0.0f))
                SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
                CALL(evt_mobj_set_position, LW(1), LW(2), LW(3), LW(4))
                BREAK_LOOP
            ELSE
                CALL(evt_mobj_set_position, LW(1), LW(2), LW(3), LW(4))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(300)
        WAIT_MS(1000)
        CALL(evt_party_nokotaro_kick_hold, 0)
        SWITCH(LW(8))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("lecture_friends_05_1"))
            CASE_EQUAL(1)
            CALL(evt_mario_mov_pos2, 367, -38, FLOAT(135.0f))
            CALL(evt_mario_mov_pos2, 416, -38, FLOAT(135.0f))
            CALL(evt_mario_set_dir, 270, 300, 0)
        END_SWITCH
        CALL(evt_party_nokotaro_hold_cancel, 0)
    END_LOOP
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(x_flag_set, 0, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        WAIT_FRAMES(16)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 40)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 90, 300)
        CALL(x_flag_set, 0, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_05_3"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(90.0f), FLOAT(520.0f), FLOAT(-20.0f), FLOAT(26.0f), FLOAT(-140.0f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_06"))
    CALL(x_flag_wait, 0, 0)
    WAIT_MS(500)
    CALL(evt_party_set_pose, 0, STRING("PNK_O_1"))
    WAIT_MS(1000)
    CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(all_party_lecture_otosub)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GET_PARTY1"))
    WAIT_MS(2000)
    RETURN
EVT_END()

// TODO: write set_dir (static, 0x84 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_itemget_kesan (static, 0x174 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_busaiku (static, 0x12c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_open (static, 0x3c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemgetchange_cam (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(all_party_lecture_12_data_1CF80)
    CALL(evt_mario_normalize)
    IF_INT_NOT_EQUAL(LW(9), 1)
        IF_INT_NOT_EQUAL(LW(9), 5)
            CALL(evt_mario_goodbye_party, 0)
        ELSE
            CALL(evt_mario_kill_party, 0)
        END_IF
        WAIT_MS(500)
        CALL(evt_npc_get_position, LW(8), LW(0), LW(1), LW(2))
        CALL(evt_npc_get_dir, LW(8), LW(3))
        CALL(evt_npc_set_position, LW(8), 0, -1000, 0)
        CALL(evt_pouch_party_join, LW(9))
        CALL(evt_mario_set_party_pos, 0, LW(9), LW(0), LW(1), LW(2))
        CALL(set_dir, LW(3))
    ELSE
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_party_set_camid, 0, 7)
    SPAWN_EVENT_GET_ID(PTR(all_party_lecture_otosub), LW(11))
    CALL(lect_itemget_kesan, 0, FLOAT(600.0f), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("sub_bg"), STRING("itemget"), 1, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(effitemgetchange_cam, STRING("sub_bg"), 5)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    SWITCH(LW(9))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("lecture_friends_00"), 0, 0)
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("lecture_friends_04"), 0, 0)
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("lecture_friends_07"), 0, 0)
        CASE_EQUAL(4)
        CALL(evt_pouch_get_yoshiname, LW(0))
        CALL(evt_msg_print_insert, 0, STRING("lecture_friends_09"), 0, 0, LW(0))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("lecture_friends_12"), 0, 0)
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("lecture_friends_15"), 0, 0)
        CALL(evt_eff_softdelete, STRING("sunlight"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("lecture_friends_17"), 0, 0)
    END_SWITCH
    IS_EVENT_RUNNING(LW(11), LW(12))
    IF_INT_EQUAL(LW(12), 1)
        DELETE_EVENT(LW(11))
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_IF
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mario_set_camid, 7)
    CALL(effitemget_open, STRING("sub_bg"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_stop_for_event)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_LECTURE1"))
    CALL(evt_snd_envoff, 1024)
    CALL(lect_cam_save)
    CALL(effitemget_busaiku, 1)
    WAIT_FRAMES(1)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(1))
    ADD(LW(3), 2016)
    CALL(evt_party_set_pos, 0, LW(0), LW(3), LW(2))
    CALL(effitemgetchange_cam, STRING("sub_bg"), 4)
    CALL(evt_paper_entry, STRING("OFF_d_seal_upper_right"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_seal_upper_right"))
    CALL(evt_img_free_capture, STRING("img"), 0)
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("Z_1"))
    CALL(evt_img_onoff, STRING("img"), 1)
    WAIT_FRAMES(1)
    CALL(effitemget_busaiku, 0)
    CALL(evt_eff_softdelete, STRING("sub_bg"))
    WAIT_FRAMES(1)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(lect_test_systemlevel)
    SWITCH(LW(9))
        CASE_EQUAL(2)
        CALL_CHILD(nokotaro_party_lecture_init)
    END_SWITCH
    LOOP(6)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        WAIT_MS(100)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    SWITCH(LW(9))
        CASE_EQUAL(2)
        CALL_CHILD(nokotaro_party_lecture)
    END_SWITCH
    LOOP(6)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        CALL(evt_party_dispflg_onoff, 0, 1, 1)
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        CALL(evt_party_dispflg_onoff, 0, 1, 0)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    CALL(evt_mario_init_camid)
    CALL(evt_party_init_camid, 0)
    CALL(lect_cam_load, 0)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 0)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_seal_upper_right"))
    CALL(evt_npc_start_for_event)
    SWITCH(LW(9))
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 eff[1] = {
    0x00000000,
};

// TODO: type
static u32 lbtn[34] = {
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

// TODO: type
static u32 x_flag_data[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 wwstart[2] = {
    0x00000000, 0x00000000,
};

// TODO: write bgm_start_wait (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_party_ido_sub (static, 0x14 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write efffukidashichange_cam (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_npc_balloon_on_off (static, 0x94 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mobj_hit_nasi (static, 0xb0 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_open_ptr (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemgetchange_cam_ptr (static, 0x50 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_map_cam_chg (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_map_cam_chg_sub (static, 0x298 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_pos (static, 0xcc bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_seq_wait (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_get_icon_id (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_seq (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_flag_on_off (static, 0x88 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_alpha (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_icon_id (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_rtn (static, 0x184 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_disp (static, 0xd0 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_get (static, 0x54 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_add (static, 0x60 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_status_on (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_status_off (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mario_ido_sum (static, 0x278 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mario_ido_sub (static, 0x14 bytes) here:
// its asm is in build/G8MJ01/hei/asm/event/evt_lect_hei.s; this is where the original has it, just before the first script that uses it
// clang-format on

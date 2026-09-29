#include "evt_lect_eki.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_img.h"
#include "evt/evt_lecture.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(bgm_start_wait);
static void lect_push_b_check(void);  // TODO: real signature
static USER_FUNC(lect_push_a_check);
static void lect_mobj_set_nosysblend(void);  // TODO: real signature
static USER_FUNC(juyoitem_goato);
static USER_FUNC(juyoitem_gogo);
static void juyoitem_end(void);  // TODO: real signature
static USER_FUNC(juyoitem_get_no);
static void lect_mobj_hit_nasi(void);  // TODO: real signature
static USER_FUNC(effitemget_busaiku);
static USER_FUNC(effitemget_open_ptr);
static void effitemget_open(void);  // TODO: real signature
static USER_FUNC(effitemgetchange_cam_ptr);
static void effitemgetchange_cam(void);  // TODO: real signature
static USER_FUNC(lect_map_cam_chg);
static void lect_map_cam_chg_sub(void);  // TODO: real signature
static USER_FUNC(lecture_button_set_pos);
static USER_FUNC(lecture_button_seq_wait);
static void lecture_button_get_icon_id(void);  // TODO: real signature
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_flag_on_off);
static USER_FUNC(lecture_button_set_alpha);
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_rtn);
static void lecture_button_disp(void);  // TODO: real signature
static void x_flag_wait(void);  // TODO: real signature
static USER_FUNC(x_flag_get);
static void x_flag_add(void);  // TODO: real signature
static USER_FUNC(x_flag_set);
static USER_FUNC(lect_status_on);
static USER_FUNC(lect_status_off);
static USER_FUNC(lect_mario_ido_sum);
static void lect_mario_ido_sub(void);  // TODO: real signature

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write lect_mario_ido_sum (static, 0x278 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(boots_lv3_juyoitem_lecture_init)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_mario_set_pos, 0, -600, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 150)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1073741856)
    CALL(evt_npc_set_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
    CALL(evt_mario_set_pos, LW(3), LW(4), LW(5))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(lect_mario_ido_sum, 1000)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, 0, -600, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 0, 11)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pos, 0, -600, 0)
    CALL(evt_mario_set_dir, 90, 300, 0)
    RETURN
EVT_END()

// TODO: write lecture_button_set_alpha (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_icon_id (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(boots_lv3_juyoitem_lecture_sub01)
    CALL(lecture_button_set_alpha, 0, 255)
    CALL(lecture_button_set_alpha, 1, 128)
    CALL(lecture_button_set_icon_id, 1, 121)
    LOOP(0)
        CALL(lecture_button_set_icon_id, 0, 101)
        WAIT_MS(300)
        CALL(lecture_button_set_icon_id, 0, 100)
        WAIT_MS(300)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(boots_lv3_juyoitem_lecture_sub02)
    CALL(lecture_button_set_icon_id, 0, 101)
    LOOP(0)
        CALL(evt_mario_get_motion, LW(3))
        IF_INT_EQUAL(LW(3), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(lecture_button_set_alpha, 0, 128)
    CALL(lecture_button_set_alpha, 1, 255)
    LOOP(0)
        CALL(lecture_button_set_icon_id, 1, 120)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 123)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 124)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 125)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 122)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 119)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 118)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 117)
        WAIT_MS(15)
    END_LOOP
    RETURN
EVT_END()

// TODO: write lect_push_a_check (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_map_cam_chg (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_pos (static, 0xcc bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_seq_wait (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_seq (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_flag_on_off (static, 0x88 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_rtn (static, 0x184 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_get (static, 0x54 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_set (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_status_on (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_status_off (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(boots_lv3_juyoitem_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    CALL(lect_map_cam_chg, STRING("S_rec"), 7)
    CALL(evt_npc_set_camid, STRING("キノピコ"), 7)
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 85)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_msg_print, 0, STRING("lecture_u_boots_04"), 0, STRING("キノピコ"))
    CALL(evt_msg_print_add, 0, STRING("lecture_u_boots_05"))
    LOOP(0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("lecture_u_boots_06"), 0, 0)
        CALL(x_flag_set, 1, 1)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_3"), STRING("T_3"))
            CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_3"))
            LOOP(0)
                CALL(evt_sub_random, 2, LW(1))
                ADD(LW(1), 4)
                MULTIPLY(LW(1), 2)
                LOOP(LW(1))
                    WAIT_MS(500)
                    CALL(x_flag_get, 1, LW(0))
                    IF_INT_EQUAL(LW(0), 2)
                        BREAK_LOOP
                    END_IF
                END_LOOP
                CALL(x_flag_get, 1, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("T_3"))
                CALL(evt_msg_print, 0, STRING("lecture_u_boots_07"), 0, STRING("キノピコ"))
                CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_3"))
            END_LOOP
            CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
            CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_1"))
        END_INLINE_EVENT
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(85.0f), FLOAT(520.0f), FLOAT(0.0f), FLOAT(40.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
        CALL(lecture_button_set_pos, 1, FLOAT(-140.0f), FLOAT(-25.0f), FLOAT(0.0f))
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_icon_id, 0, 100)
        CALL(lecture_button_set_seq, 0, 2)
        CALL(lecture_button_flag_on_off, 1, 2, 1)
        CALL(lecture_button_set_icon_id, 1, 121)
        CALL(lecture_button_set_seq, 1, 2)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        SPAWN_EVENT_GET_ID(PTR(boots_lv3_juyoitem_lecture_sub01), LW(11))
        SET_VALUE(LW(7), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SET_VALUE(LW(7), 0)
        CALL(mainwindow_opendisable)
        CALL(evt_mario_trigflag_onoff, 1, 67108864)
        CALL(evt_mario_set_enable_key, 256)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        LOOP(0)
            CALL(lect_status_off)
            CALL(evt_mario_get_pos, 0, LW(5), LW(1), LW(6))
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(1), FLOAT(-470.0f))
                CALL(x_flag_set, 1, 2)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(7), 0)
                CALL(lect_push_a_check, LW(7))
                IF_INT_NOT_EQUAL(LW(7), 0)
                    DELETE_EVENT(LW(11))
                    SPAWN_EVENT_GET_ID(PTR(boots_lv3_juyoitem_lecture_sub02), LW(11))
                END_IF
            ELSE
                IF_INT_EQUAL(LW(7), 1)
                    CALL(lect_push_a_check, LW(7))
                    IF_INT_EQUAL(LW(7), 0)
                        DELETE_EVENT(LW(11))
                        SPAWN_EVENT_GET_ID(PTR(boots_lv3_juyoitem_lecture_sub01), LW(11))
                    END_IF
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(lect_status_on)
        CALL(evt_mario_trigflag_onoff, 0, 67108864)
        CALL(evt_mario_set_enable_key, 0)
        CALL(mainwindow_openenable)
        DELETE_EVENT(LW(11))
        CALL(lecture_button_set_seq, 0, 3)
        CALL(lecture_button_set_seq, 1, 3)
        CALL(evt_msg_continue)
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(85.0f), FLOAT(520.0f), FLOAT(0.0f), FLOAT(40.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        CALL(evt_msg_print, 0, STRING("lecture_u_boots_08"), 0, STRING("キノピコ"))
        CALL(evt_msg_continue)
        WAIT_MS(500)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(1), 1)
        ADD(LW(2), 1)
        CALL(evt_mario_normalize)
        CALL(evt_mario_wait_landon)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mario_set_pos, LW(0), LW(1), 0)
        CALL(evt_mario_set_dir, 90, 300, 0)
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 65)
        CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
        CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_2"), STRING("T_2"))
        CALL(evt_msg_print, 0, STRING("lecture_u_boots_10"), 0, STRING("キノピコ"))
        CALL(evt_msg_select, 0, STRING("lecture_u_boots_11"))
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("lecture_u_boots_12"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("lecture_u_boots_13"))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_position, STRING("キノピコ"), 0, -1000, 0)
    RETURN
EVT_END()

// TODO: write bgm_start_wait (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write juyoitem_goato (static, 0x30 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write juyoitem_gogo (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write juyoitem_get_no (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_busaiku (static, 0x12c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_open_ptr (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemgetchange_cam_ptr (static, 0x50 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(all_juyoitem_lecture_rel_7_data_150F8)
    CALL(evt_mario_normalize)
    CALL(juyoitem_get_no, LW(10))
    CALL(evt_mario_set_camid, 7)
    CALL(effitemget_open_ptr, LW(9))
    CALL(juyoitem_gogo)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_stop_for_event)
    CALL(evt_npc_start_for_one_event, STRING("キノピコ"))
    CALL(evt_party_stop, 0)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_LECTURE1"))
    CALL(evt_snd_envoff, 1024)
    CALL(bgm_start_wait)
    CALL(lect_cam_save)
    CALL(effitemget_busaiku, 1)
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(1))
    ADD_FLOAT(LW(3), FLOAT(2016.0f))
    CALL(evt_mario_set_pos, LW(0), LW(3), LW(2))
    CALL(effitemgetchange_cam_ptr, LW(9), 4)
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
    CALL(evt_eff_softdelete_ptr, LW(9))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_mario_cont_onoff, 1)
    CALL(lect_test_systemlevel)
    CALL(evt_npc_sound_data_set, STRING("キノピコ"), STRING("SFX_FLD_NPC_KINOPIO_MOVE1L"), STRING("SFX_FLD_NPC_KINOPIO_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SWITCH(LW(10))
        CASE_EQUAL(8)
        CALL_CHILD(boots_lv3_juyoitem_lecture_init)
    END_SWITCH
    SWITCH(LW(10))
        CASE_EQUAL(8)
        CALL_CHILD(boots_lv3_juyoitem_lecture)
    END_SWITCH
    LOOP(6)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mario_init_camid)
    CALL(lect_cam_load, 0)
    CALL(evt_cam3d_evt_off, 0, 11)
    CALL(evt_cam_road_reset)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(juyoitem_goato)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_seal_upper_right"))
    CALL(evt_npc_start_for_event)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

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

// TODO: write lect_push_b_check (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mobj_set_nosysblend (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write juyoitem_end (static, 0x48 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mobj_hit_nasi (static, 0xb0 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_open (static, 0x3c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemgetchange_cam (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_map_cam_chg_sub (static, 0x298 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_get_icon_id (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_disp (static, 0xd0 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_wait (static, 0x68 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_add (static, 0x60 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mario_ido_sub (static, 0x14 bytes) here:
// its asm is in build/G8MJ01/eki/asm/event/evt_lect_eki.s; this is where the original has it, just before the first script that uses it
// clang-format on

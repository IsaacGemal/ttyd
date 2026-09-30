#include "evt_lect_tou.h"
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
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/itemdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "eff/eff_mario_balloon.h"
#include "manager/evtmgr_cmd.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "party.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"

// One entry per lecture button (two are on screen at a time). Only the fields
// read/written from this file are named; the rest is the original's padding.
typedef struct LectureButton {
    u8 flags;        // 0x0
    u8 seq;          // 0x1
    u8 pad_2[0xA];   // 0x2
    Vec position;    // 0xC
    u8 pad_18[0x28]; // 0x18
    s16 iconId;      // 0x40
    s16 alpha;       // 0x42
} LectureButton;     // 0x44

const Vec vec3_tou_00010580 = {200.0f, -25.0f, 0.0f};
const Vec vec3_tou_0001058c = {0.0f, 0.0f, 0.0f};
const Vec vec3_tou_00010598 = {0.0f, 2016.0f, 0.0f};
const Vec vec3_tou_000105a4 = {0.0f, -2016.0f, 0.0f};

#pragma pool_data off

#pragma force_active on
static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static EffectEntry* eff;
static s32 bgm_start_wait(void);
static USER_FUNC(set_dir);
static USER_FUNC(lect_push_b_check);
static USER_FUNC(lect_push_a_check);
static USER_FUNC(lect_mobj_set_nosysblend);
static USER_FUNC(juyoitem_goato);
static USER_FUNC(juyoitem_gogo);
static USER_FUNC(juyoitem_end);
static USER_FUNC(juyoitem_get_no);
static USER_FUNC(lect_party_ido_sum);
static f32 lect_party_ido_sub(f32, f32, f32, f32);
static USER_FUNC(efffukidashichange_cam);
static USER_FUNC(lect_npc_balloon_on_off);
static USER_FUNC(lect_itemget_kesan);
static USER_FUNC(lect_mobj_hit_nasi);
static USER_FUNC(effitemget_busaiku);
static USER_FUNC(effitemget_open_ptr);
static USER_FUNC(effitemget_open);
static USER_FUNC(effitemgetchange_cam_ptr);
static USER_FUNC(effitemgetchange_cam);
static USER_FUNC(lect_map_cam_chg);
static void lect_map_cam_chg_sub(MapObject* mapObj, s32 camId, s32 recurse);
static USER_FUNC(lecture_button_set_pos);
static USER_FUNC(lecture_button_seq_wait);
static USER_FUNC(lecture_button_get_icon_id);
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_flag_on_off);
static USER_FUNC(lecture_button_set_alpha);
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_rtn);
static void lecture_button_disp(CameraId cameraId, void* param);
static USER_FUNC(x_flag_wait);
static USER_FUNC(x_flag_get);
static USER_FUNC(x_flag_add);
static USER_FUNC(x_flag_set);
static USER_FUNC(lect_status_on);
static USER_FUNC(lect_status_off);
static USER_FUNC(lect_mario_ido_sum);
static f32 lect_mario_ido_sub(f32, f32, f32, f32);

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(yoshi_party_lecture_init)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 40)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(-2.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(lect_party_ido_sum, 0, 1000)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_set_dir, 0, 90, 300)
    RETURN
EVT_END()

EVT_BEGIN(yoshi_party_lecture)
    CALL(evt_msg_print, 0, STRING("lecture_friends_09_0_1"), 0, 0)
    CALL(evt_msg_print, 0, STRING("lecture_friends_09_1"), 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    WAIT_MS(300)
    CALL(evt_party_yoshi_ride, 0, 1)
    WAIT_MS(1000)
    CALL(evt_party_yoshi_ride, 0, 0)
    WAIT_MS(1000)
    CALL(evt_party_yoshi_ride, 0, 1)
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_09_1_1"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(470.0f), FLOAT(71.0f), FLOAT(30.0f), FLOAT(-101.8f), 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(300)
    CALL(x_flag_set, 0, 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(3), 0)
        LOOP(0)
            IF_INT_LESS_OR_EQUAL(LW(3), 0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                SET_VALUE(LW(3), 10)
            ELSE
                SUBTRACT(LW(3), 1)
            END_IF
            CALL(x_flag_get, 0, LW(0))
            IF_INT_EQUAL(LW(0), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 140)
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(160.0f))
    CALL(x_flag_set, 0, 2)
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_party_set_dir, 0, 270, 300)
    WAIT_MS(300)
    CALL(x_flag_set, 0, 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(3), 0)
        LOOP(0)
            IF_INT_LESS_OR_EQUAL(LW(3), 0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                SET_VALUE(LW(3), 10)
            ELSE
                SUBTRACT(LW(3), 1)
            END_IF
            CALL(x_flag_get, 0, LW(0))
            IF_INT_EQUAL(LW(0), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    SUBTRACT(LW(0), 140)
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(160.0f))
    CALL(x_flag_set, 0, 2)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_party_set_dir, 0, 90, 300)
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_10"))
    LOOP(2)
        CALL(evt_party_yoshi_fly, 0, 1, 1000)
        WAIT_MS(300)
    END_LOOP
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(-2.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_party_set_dir, 0, 270, 300)
    WAIT_MS(300)
    WAIT_MS(300)
    CALL(evt_party_yoshi_ride, 0, 0)
    CALL(evt_party_stop, 0)
    WAIT_MS(1000)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 55)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(160.0f))
    CALL(evt_party_set_dir, 0, 90, 300)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_11"))
    CALL(evt_party_set_pose, 0, STRING("PYS_O_1"))
    WAIT_MS(1000)
    CALL(evt_party_set_pose, 0, STRING("PYS_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(all_party_lecture_otosub)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GET_PARTY1"))
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(all_party_lecture_27_data_2DF64)
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
        CASE_EQUAL(4)
        CALL_CHILD(yoshi_party_lecture_init)
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
        CASE_EQUAL(4)
        CALL_CHILD(yoshi_party_lecture)
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

EVT_BEGIN(hammer_lv2_juyoitem_lecture_init)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_mario_set_pos, 0, 0, 120)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 150)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1073741856)
    CALL(evt_npc_set_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
    CALL(evt_mario_set_pos, 610, 10, -14)
    CALL(evt_cam3d_evt_set_rel_dir, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(lect_mario_ido_sum, 1000)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, 0, 0, 120)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 0, 11)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pos, 0, 0, 120)
    CALL(evt_mario_set_dir, 90, 300, 0)
    RETURN
EVT_END()

EVT_BEGIN(hammer_lv2_juyoitem_lecture_sub01)
    CALL(lecture_button_set_alpha, 0, 255)
    CALL(lecture_button_set_alpha, 1, 128)
    CALL(lecture_button_set_icon_id, 1, 121)
    LOOP(0)
        CALL(lecture_button_set_icon_id, 0, 103)
        WAIT_MS(300)
        CALL(lecture_button_set_icon_id, 0, 102)
        WAIT_MS(300)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(hammer_lv2_juyoitem_lecture_sub02)
    CALL(lecture_button_set_alpha, 0, 128)
    CALL(lecture_button_set_alpha, 1, 255)
    CALL(lecture_button_set_icon_id, 0, 103)
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

EVT_BEGIN(hammer_lv2_juyoitem_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    CALL(evt_npc_set_camid, STRING("キノピコ"), 7)
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(lecture_button_set_pos, 1, FLOAT(-140.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 85)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_msg_print, 0, STRING("lecture_s_hammer_04"), 0, STRING("キノピコ"))
    CALL(evt_msg_print_add, 0, STRING("lecture_s_hammer_05"))
    LOOP(0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("lecture_s_hammer_06"), 0, 0)
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
                CALL(evt_msg_print, 0, STRING("lecture_s_hammer_07"), 0, STRING("キノピコ"))
                CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_3"))
            END_LOOP
            CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
            CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_1"))
        END_INLINE_EVENT
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(85.0f), FLOAT(520.0f), FLOAT(0.0f), FLOAT(40.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_icon_id, 0, 102)
        CALL(lecture_button_set_seq, 0, 2)
        CALL(lecture_button_flag_on_off, 1, 2, 1)
        CALL(lecture_button_set_icon_id, 1, 121)
        CALL(lecture_button_set_seq, 1, 2)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        SPAWN_EVENT_GET_ID(PTR(hammer_lv2_juyoitem_lecture_sub01), LW(11))
        SET_VALUE(LW(7), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SET_VALUE(LW(7), 0)
        SET_VALUE(LW(12), 0)
        CALL(mainwindow_opendisable)
        CALL(evt_mario_trigflag_onoff, 1, 67108864)
        CALL(evt_mario_set_enable_key, 512)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        LOOP(0)
            CALL(lect_status_off)
            CALL(evt_mario_get_pos, 0, LW(5), LW(1), LW(6))
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            CALL(evt_mario_get_motion, LW(3))
            IF_INT_EQUAL(LW(12), 0)
                IF_INT_EQUAL(LW(3), 19)
                    ADD(LW(12), 1)
                END_IF
            ELSE
                IF_INT_EQUAL(LW(12), 1)
                    IF_INT_NOT_EQUAL(LW(3), 19)
                        CALL(x_flag_set, 1, 2)
                        BREAK_LOOP
                    END_IF
                END_IF
            END_IF
            IF_INT_EQUAL(LW(12), 0)
                SWITCH(LW(7))
                    CASE_EQUAL(0)
                    CALL(lect_push_b_check, LW(7))
                    IF_INT_NOT_EQUAL(LW(7), 0)
                        DELETE_EVENT(LW(11))
                        SPAWN_EVENT_GET_ID(PTR(hammer_lv2_juyoitem_lecture_sub02), LW(11))
                    END_IF
                    CASE_EQUAL(1)
                    CALL(lect_push_b_check, LW(7))
                    IF_INT_EQUAL(LW(7), 0)
                        DELETE_EVENT(LW(11))
                        SPAWN_EVENT_GET_ID(PTR(hammer_lv2_juyoitem_lecture_sub01), LW(11))
                    END_IF
                END_SWITCH
            ELSE
                IF_INT_LESS_OR_EQUAL(LW(7), 1)
                    SET_VALUE(LW(7), 2)
                    CALL(lecture_button_set_seq, 0, 3)
                    CALL(lecture_button_set_seq, 1, 3)
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
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
        CALL(evt_mario_set_pose, STRING("M_V_2"))
        WAIT_MS(500)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        WAIT_MS(300)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_adjust_dir)
        CALL(evt_mario_set_dir, 90, 150, 0)
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_2"), STRING("T_2"))
        CALL(evt_msg_print, 0, STRING("lecture_s_hammer_08"), 0, STRING("キノピコ"))
        CALL(evt_msg_select, 0, STRING("lecture_s_hammer_09"))
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("lecture_s_hammer_10"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("lecture_s_hammer_11"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(52.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), 35)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("A_1"))
    WAIT_MS(200)
    SET_VALUE(LW(1), STRING("TESTMOBJ"))
    CALL(evt_mobj_entry, LW(1), STRING("MOBJ_Lv1BigBigBlock"))
    CALL(lect_mobj_hit_nasi, LW(1))
    CALL(lect_mobj_set_nosysblend, LW(1))
    CALL(evt_mobj_set_camid, LW(1), 7)
    CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
    ADD(LW(2), 50)
    ADD(LW(3), 150)
    SUBTRACT(LW(4), 70)
    CALL(evt_mobj_flag_onoff, 1, LW(1), 64)
    CALL(evt_mobj_set_position, LW(1), LW(2), LW(3), LW(4))
    CALL(evt_mobj_set_gravity_bound, LW(1), FLOAT(0.16602f), FLOAT(1.0f))
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
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 300)
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
    CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("A_2"))
    WAIT_MS(170)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(52.0f), FLOAT(55.0f), FLOAT(-100.0f), 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("lecture_s_hammer_12"), 0, STRING("キノピコ"))
    WAIT_MS(300)
    LOOP(6)
        CALL(evt_mobj_flag_onoff, 1, LW(1), 16384)
        WAIT_MS(100)
        CALL(evt_mobj_flag_onoff, 0, LW(1), 16384)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mobj_delete, LW(1))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(52.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("lecture_s_hammer_13"), 0, STRING("キノピコ"))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_position, STRING("キノピコ"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(all_juyoitem_lecture_rel_27_data_2F48C)
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
        CASE_EQUAL(10)
        CALL_CHILD(hammer_lv2_juyoitem_lecture_init)
    END_SWITCH
    SWITCH(LW(10))
        CASE_EQUAL(10)
        CALL_CHILD(hammer_lv2_juyoitem_lecture)
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

static f32 lect_mario_ido_sub(f32 a, f32 b, f32 c, f32 d) {
    f32 e = b - a;

    return (e * c + a * d) / b;
}
static USER_FUNC(lect_mario_ido_sum) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 time = evtGetValue(event, args[0]);
    s32 elapsed;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(0), mario->position.x);
        evtSetFloat(event, LW(1), mario->position.y);
        evtSetFloat(event, LW(2), mario->position.z);
    }
    elapsed = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (elapsed > time) {
        elapsed = time;
    }
    mario->position.x = lect_mario_ido_sub(elapsed, time, evtGetFloat(event, LW(3)), evtGetFloat(event, LW(0)));
    mario->position.y = lect_mario_ido_sub(elapsed, time, evtGetFloat(event, LW(4)), evtGetFloat(event, LW(1)));
    mario->position.z = lect_mario_ido_sub(elapsed, time, evtGetFloat(event, LW(5)), evtGetFloat(event, LW(2)));
    if (elapsed >= time) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
static USER_FUNC(lect_status_off) {
    statusWinForceClose();
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_status_on) {
    statusWinForceCloseClear();
    return EVT_RETURN_DONE;
}
static USER_FUNC(x_flag_set) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    x_flag_data[idx] = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(x_flag_add) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    x_flag_data[idx] += evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(x_flag_get) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    evtSetValue(event, args[1], x_flag_data[idx]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(x_flag_wait) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);
    return (x_flag_data[idx] == value) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
// NONMATCHING (88.84%): button ptr r6 not r9; position copy bases &btn->position
// not btn+8; iconId test branches to else, not neg15. Everything else matches.
static void lecture_button_disp(CameraId cameraId, void* param) {
    s32 i;
    f32 dy;
    Vec pos;

    for (i = 0; i < 2; i++) {
        LectureButton* btn = &lbtn[i];
        Vec* pp = &btn->position;

        if (!(btn->flags & 1)) {
            if (btn->iconId < 0x7E && btn->iconId >= 0x75) {
                dy = -15.0f;
            } else {
                dy = 0.0f;
            }
            pos = *pp;
            pos.y += dy;
            iconDispGxAlpha(&pos, 0x50, btn->iconId, btn->alpha, 1.0f);
        }
    }
}
static USER_FUNC(lecture_button_rtn) {
    s32 i;
    LectureButton* btn;
    s32 done;

    if (isFirstCall) {
        memset(lbtn, 0, sizeof(lbtn));
        for (i = 0; i < 2; i++) {
            lbtn[i].position = vec3_tou_00010580;
        }
    }
    done = 0;
    for (i = 0; i < 2; i++) {
        btn = &lbtn[i];
        switch (btn->seq) {
            case 0:
                btn->iconId = 0x68;
                btn->seq = 1;
                break;
            case 2:
                btn->alpha += 7;
                if (btn->alpha >= 0xFF) {
                    btn->alpha = 0xFF;
                    btn->seq = 1;
                }
                break;
            case 3:
                btn->alpha -= 7;
                if (btn->alpha <= 0) {
                    btn->alpha = 0;
                    btn->seq = 1;
                }
                break;
            case 4:
                btn->flags |= 1;
                done++;
                break;
        }
    }
    if (done >= 2) {
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, 0.0f, 1, lecture_button_disp, NULL);
    return EVT_RETURN_BLOCK;
}
static USER_FUNC(lecture_button_set_icon_id) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];
    btn->iconId = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lecture_button_set_alpha) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];
    btn->alpha = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    u8 mask = evtGetValue(event, args[1]);
    s32 on = evtGetValue(event, args[2]);
    u8* p = &lbtn[idx].flags;
    if (on != 0) {
        *p |= mask;
    } else {
        *p &= ~mask;
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];
    btn->seq = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lecture_button_get_icon_id) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];
    evtSetValue(event, args[1], btn->seq);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lecture_button_seq_wait) {
    s32* args = event->args;

    s32 idx = evtGetValue(event, args[0]);
    s32 seq = evtGetValue(event, args[1]);
    LectureButton* btn = &lbtn[idx];
    return (btn->seq != seq) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}
static USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    Vec pos = vec3_tou_0001058c;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    *(Vec*)&lbtn[idx].position = pos;
    return EVT_RETURN_DONE;
}
static void lect_map_cam_chg_sub(MapObject* mapObj, s32 camId, s32 recurse) {
    mapObj->field_0x0[5] = camId;
    if (mapObj->child != NULL) {
        lect_map_cam_chg_sub(mapObj->child, camId, 1);
    }
    if (recurse) {
        if (mapObj->next != NULL) {
            lect_map_cam_chg_sub(mapObj->next, camId, 1);
        }
    }
}
static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    MapObject* obj = mapGetMapObj((const char*)name);

    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, camId, 0);
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    *(s32*)((u8*)eff->userdata + 0x38) = value;
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);

    *(s32*)((u8*)eff->userdata + 0x38) = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemget_open) {
    EffectEntry* eff = effNameToPtr((const char*)evtGetValue(event, event->args[0]));

    *(s32*)((u8*)eff->userdata + 0x20) = 1;
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemget_open_ptr) {
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, event->args[0]);

    *(s32*)((u8*)eff->userdata + 0x20) = 1;
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemget_busaiku) {
    s32 flag = evtGetValue(event, event->args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec pos;

    if (flag != 0) {
        pos = vec3_tou_00010598;
    } else {
        pos = vec3_tou_000105a4;
    }
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    PSVECAdd(&cam->cameraPos, &pos, &cam->storedPos);
    PSVECAdd(&cam->cameraAt, &pos, &cam->storedAt);
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_mobj_hit_nasi) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
    MapObjectEntry* mobj;
    s32 i;

    if (strcmp(name, "me") != 0) {
        mobj = mobjNameToPtr(name);
    } else {
        mobj = event->thisMapObj;
    }
    for (i = 0; i < 2; i++) {
        if (mobj->hitObj[i].name != NULL && strcmp(mobj->hitObj[i].name, "") != 0) {
            hitGrpFlagOn(mobj->hitObj[i].name, 1);
        }
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_itemget_kesan) {
    s32* args = event->args;
    s32 id = 1;
    PartyEntry* party;
    f32 dist;
    f32 speed;
    CameraEntry* cam;
    Vec pos;
    Vec dir;

    if (args[0] == 0) {
        id = partyCtrlNo;
    }
    party = partyGetPtr(id);
    dist = evtGetFloat(event, args[1]);
    cam = camGetPtr(CAMERA_3D);
    if (party == NULL) {
        evtSetFloat(event, args[2], 0.0f);
        evtSetFloat(event, args[3], 0.0f);
        evtSetFloat(event, args[4], 0.0f);
        return EVT_RETURN_DONE;
    }
    pos = party->position;
    speed = *(f32*)((u8*)party + 0xF0);
    pos.y += speed / 2.0f;
    PSVECSubtract(&pos, &cam->cameraPos, &dir);
    PSVECNormalize(&dir, &dir);
    PSVECScale(&dir, &dir, dist);
    PSVECAdd(&cam->cameraPos, &dir, &dir);
    evtSetFloat(event, args[2], dir.x);
    evtSetFloat(event, args[3], dir.y);
    evtSetFloat(event, args[4], dir.z);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_npc_balloon_on_off) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);

    if (evtGetValue(event, args[1]) != 0) {
        EffMarioBalloonData* data;

        eff = effMarioBalloonEntry(1, npc);
        data = (EffMarioBalloonData*)eff->userdata;
        data->cameraId = 7;
    } else {
        effDelete(eff);
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(efffukidashichange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    *(s32*)((u8*)eff->userdata + 0x4C) = value;
    return EVT_RETURN_DONE;
}
static f32 lect_party_ido_sub(f32 a, f32 b, f32 c, f32 d) {
    f32 e = b - a;

    return (e * c + a * d) / b;
}
static USER_FUNC(lect_party_ido_sum) {
    s32* args = event->args;
    s32 id = 1;
    PartyEntry* party;
    s32 time;
    s32 elapsed;

    if (args[0] == 0) {
        id = partyCtrlNo;
    }
    party = partyGetPtr(id);
    time = evtGetValue(event, args[1]);
    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(3), party->position.x);
        evtSetFloat(event, LW(4), party->position.y);
        evtSetFloat(event, LW(5), party->position.z);
        return EVT_RETURN_BLOCK;
    }
    elapsed = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (elapsed > time) {
        elapsed = time;
    }
    party->position.x = lect_party_ido_sub(elapsed, time, evtGetFloat(event, LW(3)), evtGetFloat(event, LW(0)));
    party->position.y = lect_party_ido_sub(elapsed, time, evtGetFloat(event, LW(4)), evtGetFloat(event, LW(1)));
    party->position.z = lect_party_ido_sub(elapsed, time, evtGetFloat(event, LW(5)), evtGetFloat(event, LW(2)));
    if (elapsed >= time) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
static USER_FUNC(juyoitem_get_no) {
    s32* args = event->args;
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    evtSetValue(event, args[0], item->itemId);
    return EVT_RETURN_DONE;
}
static USER_FUNC(juyoitem_end) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];
    EffectEntry* eff = (EffectEntry*)event->lwData[9];

    effSoftDelete(eff);
    marioSetCamId(CAMERA_3D);
    iconFlagOff(item->name, 0x200);
    return EVT_RETURN_DONE;
}
static USER_FUNC(juyoitem_gogo) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    iconFlagOff(item->name, 0x200);
    itemSetPosition(item, 0.0f, -1000.0f, 0.0f);
    marioChgStayMotion();
    marioStSystemLevel(0);
    return EVT_RETURN_DONE;
}
static USER_FUNC(juyoitem_goato) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    iconFlagOn(item->name, 2);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_mobj_set_nosysblend) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
    MapObjectEntry* mobj;

    if (strcmp(name, "me") != 0) {
        mobj = mobjNameToPtr(name);
    } else {
        mobj = event->thisMapObj;
    }
    animPoseSetMaterialFlagOn(mobj->poseId, 0x1800);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_push_a_check) {
    s32* args = event->args;

    if (keyGetButton(0) & 0x100) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_push_b_check) {
    s32* args = event->args;

    if (keyGetButton(0) & 0x200) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(set_dir) {
    s32* args = event->args;
    s32 dir = evtGetValue(event, args[0]);
    PartyEntry* party = partyGetPtr(0);

    if (party != NULL) {
        f32 d = (f32)(270 - (dir > 180 ? 270 : 90));

        party->field_10C = d;
        party->field_110 = d;
    }
    return EVT_RETURN_DONE;
}
static s32 bgm_start_wait(void) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// clang-format on

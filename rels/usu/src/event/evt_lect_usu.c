#include "evt_lect_usu.h"
#include "rel/common/evt_lect.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "eff/eff_fukidashi.h"
#include "eff/eff_itemget.h"
#include "eff/eff_mario_balloon.h"
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
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_roll.h"
#include "motion/mot_slit.h"
#include "party.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static f32 lect_mario_ido_sub(f32 f1, f32 f2, f32 f3, f32 f4);
static USER_FUNC(lect_mario_ido_sum);
static USER_FUNC(lect_status_off);
static USER_FUNC(lect_status_on);
static USER_FUNC(x_flag_set);
static USER_FUNC(x_flag_add);
static USER_FUNC(x_flag_get);
static USER_FUNC(x_flag_wait);
static void lecture_button_disp(CameraId cameraId, void* param);
static USER_FUNC(lecture_button_rtn);
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_set_alpha);
static USER_FUNC(lecture_button_flag_on_off);
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_get_icon_id);
static USER_FUNC(lecture_button_seq_wait);
static USER_FUNC(lecture_button_set_pos);
static void lect_map_cam_chg_sub(MapObject* obj, s32 camId, BOOL recurse);
static USER_FUNC(lect_map_cam_chg);
static USER_FUNC(effitemgetchange_cam);
static USER_FUNC(effitemgetchange_cam_ptr);
static USER_FUNC(effitemget_open);
static USER_FUNC(effitemget_open_ptr);
static USER_FUNC(effitemget_busaiku);
static USER_FUNC(lect_mobj_hit_nasi);
static USER_FUNC(lect_itemget_kesan);
static USER_FUNC(lect_npc_balloon_on_off);
static USER_FUNC(efffukidashichange_cam);
static f32 lect_party_ido_sub(f32 f1, f32 f2, f32 f3, f32 f4);
static USER_FUNC(lect_party_ido_sum);
static USER_FUNC(dai_moveti_set);
static USER_FUNC(dai_moveti_get);
static USER_FUNC(lect_slit_check);
static USER_FUNC(evtflash);
static USER_FUNC(lect_push_r_check);
static USER_FUNC(evt_nomal_cange);
static USER_FUNC(set_dir);
static USER_FUNC(bgm_start_wait);

static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static EffectEntry* eff;
static Vec daiv;

extend_data(16,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(17,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

#include "rel/common/evt_lect/lect_mario_ido.h"
#include "rel/common/evt_lect/lect_status.h"
#include "rel/common/x_flag.h"
#include "rel/common/evt_lect/lecture_button_disp.h"
#include "rel/common/evt_lect/lecture_button_rtn.h"
#include "rel/common/evt_lect/lecture_button_set_icon_id.h"
#include "rel/common/evt_lect/lecture_button_set_pos.h"
#include "rel/common/evt_lect/lect_map_cam_chg.h"
#include "rel/common/evt_lect/effitemget.h"
#include "rel/common/evt_lect/effitemget_busaiku.h"
#include "rel/common/evt_lect/lect_mobj_hit_nasi.h"
#include "rel/common/evt_lect/party_lecture.h"
#include "rel/common/evt_lect/majin_lecture.h"
#include "rel/common/set_dir.h"
#include "rel/common/bgm_start_wait.h"

EVT_BEGIN(vivian_party_lecture_init)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 40)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.8f), FLOAT(0.0f), FLOAT(30.0f), FLOAT(-101.8f), 1000, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(lect_party_ido_sum, 0, 1000)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_set_dir, 0, 90, 300)
    RETURN
EVT_END()

EVT_BEGIN(vivian_party_lecture)
    CALL(evt_msg_print, 0, STRING("lecture_friends_12_0_1"), 0, 0)
    CALL(evt_msg_print, 0, STRING("lecture_friends_12_1"), 0, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(105.0f), FLOAT(597.0f), FLOAT(0.0f), FLOAT(63.0f), FLOAT(-101.8f), 1000, 11)
    WAIT_MS(1000)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_party_vivian_hold, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 200)
    ADD(LW(1), 50)
    CALL(evt_npc_start_for_one_event, STRING("テレサ"))
    CALL(evt_npc_flag_onoff, 1, STRING("テレサ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("テレサ"), 1073741856)
    CALL(evt_npc_set_position, STRING("テレサ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_camid, STRING("テレサ"), 7)
    CALL(evt_npc_set_ry, STRING("テレサ"), 270)
    CALL(evt_npc_get_position, STRING("テレサ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 200)
    CALL(evt_npc_move_position, STRING("テレサ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_eff_fukidashi, 1, STRING("fuki"), STRING("テレサ"), 1, 0, 0, 0, 0, 0, 0, 60)
    CALL(efffukidashichange_cam, STRING("fuki"), 7)
    WAIT_MS(1000)
    LOOP(2)
        CALL(evt_npc_reverse_ry, STRING("テレサ"))
        WAIT_MS(800)
    END_LOOP
    SUBTRACT(LW(0), 200)
    CALL(evt_npc_move_position, STRING("テレサ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_position, STRING("テレサ"), 0, -1000, 0)
    CALL(evt_npc_stop_for_one_event, STRING("テレサ"))
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_13"))
    CALL(evt_party_vivian_unhold, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.8f), FLOAT(0.0f), FLOAT(30.0f), FLOAT(-101.8f), 1000, 0)
    CALL(evt_party_stop, 0)
    WAIT_FRAMES(16)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 40)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(80.0f))
    CALL(evt_party_set_dir, 0, 90, 300)
    WAIT_MS(1000)
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_14"))
    CALL(evt_party_set_pose, 0, STRING("PTR_O_1"))
    WAIT_MS(1000)
    CALL(evt_party_set_pose, 0, STRING("PTR_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(all_party_lecture_otosub)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GET_PARTY1"))
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(all_party_lecture_29_data_10988)
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
        CASE_EQUAL(6)
        CALL_CHILD(vivian_party_lecture_init)
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
        CASE_EQUAL(6)
        CALL_CHILD(vivian_party_lecture)
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

EVT_BEGIN(roll_majin_lecture_init)
    CALL(lect_set_systemlevel, 0)
    CALL(lect_test_systemlevel)
    CALL(evt_cam3d_evt_set_rel_dir, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

EVT_BEGIN(roll_majin_lecture_sub01)
    CALL(lecture_button_set_alpha, 0, 255)
    CALL(lecture_button_set_alpha, 1, 128)
    CALL(lecture_button_set_icon_id, 1, 121)
    LOOP(0)
        CALL(lecture_button_set_icon_id, 0, 129)
        WAIT_MS(300)
        CALL(lecture_button_set_icon_id, 0, 128)
        WAIT_MS(300)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(roll_majin_lecture_sub02)
    CALL(lecture_button_set_alpha, 0, 128)
    CALL(lecture_button_set_alpha, 1, 255)
    CALL(lecture_button_set_icon_id, 0, 129)
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

EVT_BEGIN(roll_majin_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(lecture_button_set_pos, 1, FLOAT(-140.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(evt_npc_flag_onoff, 0, STRING("ペラ魔人"), 2048)
    CALL(evt_cam_get_at, 4, LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 25)
    CALL(evt_npc_set_position, STRING("ペラ魔人"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1073741856)
    WAIT_FRAMES(1)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_cam_get_at, 4, LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 25)
            CALL(evt_npc_set_position, STRING("ペラ魔人"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 7)
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_1"))
    WAIT_MS(200)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_12"), 0, STRING("ペラ魔人"))
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    LOOP(4)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_LIGHT1"), LW(0), LW(1), LW(2), 0)
        CALL(evtflash)
    END_LOOP
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_13"), 0, STRING("ペラ魔人"))
    CALL(evt_msg_print_add, 0, STRING("stg4_usu_majin3_14"))
    LOOP(0)
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_icon_id, 0, 128)
        CALL(lecture_button_set_seq, 0, 2)
        CALL(lecture_button_flag_on_off, 1, 2, 1)
        CALL(lecture_button_set_icon_id, 1, 121)
        CALL(lecture_button_set_seq, 1, 2)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        SPAWN_EVENT_GET_ID(PTR(roll_majin_lecture_sub01), LW(11))
        SET_VALUE(LW(7), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SET_VALUE(LW(7), 0)
        CALL(evt_cam_letter_box_disable, 1)
        CALL(mainwindow_opendisable)
        CALL(evt_mario_trigflag_onoff, 1, 67108864)
        CALL(evt_mario_set_enable_key, 32)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        LOOP(0)
            CALL(lect_status_off)
            CALL(evt_mario_get_pos, 0, LW(5), LW(1), LW(6))
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            CALL(evt_mario_get_motion, LW(3))
            IF_INT_EQUAL(LW(3), 22)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(7), 0)
                CALL(lect_push_r_check, LW(7))
                IF_INT_NOT_EQUAL(LW(7), 0)
                    DELETE_EVENT(LW(11))
                    SPAWN_EVENT_GET_ID(PTR(roll_majin_lecture_sub02), LW(11))
                END_IF
            ELSE
                IF_INT_EQUAL(LW(7), 1)
                    CALL(lect_push_r_check, LW(7))
                    IF_INT_EQUAL(LW(7), 0)
                        DELETE_EVENT(LW(11))
                        SPAWN_EVENT_GET_ID(PTR(roll_majin_lecture_sub01), LW(11))
                    END_IF
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_continue)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(lect_status_on)
        CALL(evt_mario_trigflag_onoff, 0, 67108864)
        CALL(evt_mario_set_enable_key, 0)
        CALL(mainwindow_openenable)
        CALL(evt_cam_letter_box_disable, 0)
        DELETE_EVENT(LW(11))
        CALL(lecture_button_set_seq, 0, 3)
        CALL(lecture_button_set_seq, 1, 3)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        WAIT_MS(300)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_18"), 0, STRING("ペラ魔人"))
        CALL(evt_msg_select, 0, STRING("stg4_usu_majin3_19"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_nomal_cange)
            CALL(evt_msg_toge, 1, 0, 0, 0)
            CALL(evt_msg_print_add, 0, STRING("stg4_usu_majin3_20"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("stg4_usu_majin3_20_1"))
    CALL(evt_nomal_cange)
    CALL(evt_msg_print_add, 0, STRING("stg4_usu_majin3_21"))
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 4)
    CALL(evt_npc_set_position, STRING("ペラ魔人"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 2048)
    RETURN
EVT_END()
// clang-format on

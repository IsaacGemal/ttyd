#include "evt_lect_gon.h"
#include "rel/common/evt_lect.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_lecture.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "eff/eff_itemget.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_roll.h"
#include "motion/mot_slit.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include <dolphin/mtx.h>
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
static USER_FUNC(dai_moveti_set);
static USER_FUNC(dai_moveti_get);
static USER_FUNC(lect_slit_check);
static USER_FUNC(evtflash);
static USER_FUNC(lect_push_r_check);
static USER_FUNC(evt_nomal_cange);
static USER_FUNC(bgm_start_wait);


// .bss is emitted in reverse declaration order; this order reproduces the original's layout.
static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static Vec daiv;

// clang-format off
extend_data(100,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(101,
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
#include "rel/common/evt_lect/majin_lecture.h"
#include "rel/common/bgm_start_wait.h"

EVT_BEGIN(slit_majin_lecture_init)
    CALL(lect_set_systemlevel, 0)
    CALL(lect_test_systemlevel)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

EVT_BEGIN(slit_majin_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1073741856)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 7)
    WAIT_FRAMES(1)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_cam_get_at, 4, LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 25)
            CALL(evt_npc_set_position, STRING("ペラ魔人"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_1"))
    WAIT_MS(200)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_gon_12_12"), 0, STRING("ペラ魔人"))
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    LOOP(4)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_LIGHT1"), LW(0), LW(1), LW(2), 0)
        CALL(evtflash)
    END_LOOP
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_gon_12_13"), 0, STRING("ペラ魔人"))
    CALL(evt_msg_print_add, 0, STRING("stg1_gon_12_14"))
    LOOP(0)
        LOOP(0)
            CALL(lecture_button_flag_on_off, 0, 2, 1)
            CALL(lecture_button_set_icon_id, 0, 128)
            CALL(lecture_button_set_seq, 0, 2)
            CALL(lecture_button_seq_wait, 0, 1)
            CALL(x_flag_set, 0, 1)
            BEGIN_INLINE_EVENT
                LOOP(0)
                    CALL(lecture_button_set_icon_id, 0, 129)
                    WAIT_MS(300)
                    CALL(x_flag_get, 0, LW(0))
                    IF_INT_EQUAL(LW(0), 2)
                        BREAK_LOOP
                    END_IF
                    CALL(lecture_button_set_icon_id, 0, 128)
                    WAIT_MS(300)
                    CALL(x_flag_get, 0, LW(0))
                    IF_INT_EQUAL(LW(0), 2)
                        BREAK_LOOP
                    END_IF
                END_LOOP
            END_INLINE_EVENT
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SET_VALUE(LW(7), 0)
            CALL(evt_cam_letter_box_disable, 1)
            CALL(mainwindow_opendisable)
            CALL(evt_mario_trigflag_onoff, 1, 134217728)
            CALL(evt_mario_trigflag_onoff, 1, 67108864)
            CALL(evt_mario_set_enable_key, 32)
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_mario_key_onoff, 1)
            LOOP(0)
                CALL(lect_status_off)
                CALL(evt_mario_get_pos, 0, LW(5), LW(1), LW(6))
                CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
                WAIT_FRAMES(1)
                CALL(lect_slit_check, LW(3))
                IF_INT_EQUAL(LW(7), 0)
                    IF_INT_EQUAL(LW(3), 1)
                        ADD(LW(7), 1)
                        SET_VALUE(LW(11), 0)
                    END_IF
                ELSE
                    IF_INT_EQUAL(LW(7), 1)
                        IF_INT_EQUAL(LW(3), 0)
                            SET_VALUE(LW(11), 0)
                            BREAK_LOOP
                        END_IF
                        ADD(LW(11), 1)
                        IF_INT_GREATER_OR_EQUAL(LW(11), 180)
                            SET_VALUE(LW(11), 1)
                            BREAK_LOOP
                        END_IF
                    END_IF
                END_IF
            END_LOOP
            CALL(evt_msg_continue)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_mario_key_onoff, 0)
            CALL(lect_status_on)
            CALL(evt_mario_trigflag_onoff, 0, 134217728)
            CALL(evt_mario_trigflag_onoff, 0, 67108864)
            CALL(evt_mario_set_enable_key, 0)
            CALL(mainwindow_openenable)
            CALL(x_flag_set, 0, 2)
            CALL(evt_cam_letter_box_disable, 0)
            CALL(lecture_button_set_seq, 0, 3)
            CALL(evt_mario_normalize)
            IF_INT_EQUAL(LW(11), 1)
                LOOP(0)
                    WAIT_FRAMES(1)
                    CALL(lect_slit_check, LW(3))
                    IF_INT_EQUAL(LW(3), 0)
                        BREAK_LOOP
                    END_IF
                END_LOOP
            END_IF
            CALL(evt_mario_adjust_dir)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(lecture_button_seq_wait, 0, 1)
            WAIT_MS(300)
            IF_INT_EQUAL(LW(11), 1)
                BREAK_LOOP
            ELSE
                CALL(evt_msg_toge, 1, 0, 0, 0)
                CALL(evt_msg_print, 0, STRING("stg1_gon_12_15_1"), 0, STRING("ペラ魔人"))
            END_IF
        END_LOOP
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg1_gon_12_15"), 0, STRING("ペラ魔人"))
        CALL(evt_msg_select, 0, STRING("stg1_gon_12_15_yn"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("stg1_gon_12_16"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("stg1_gon_12_17"))
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 4)
    CALL(evt_npc_set_position, STRING("ペラ魔人"), 0, -1000, 0)
    RETURN
EVT_END()
// clang-format on

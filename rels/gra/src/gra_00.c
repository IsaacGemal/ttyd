#include "gra_00.h"
#include "driver/camdrv.h"
#include "driver/msgdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
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
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "party/party_motion.h"
#include "rel/gra/event/evt_kagemario.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_gra_vivian_pose_set[];
static USER_FUNC(cam_r);
static USER_FUNC(zoom);

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

// TODO: write camera_yure (global, 0x19c bytes) here:
// its asm is in build/G8MJ01/gra/asm/gra_00.s; this is where the original has it, just before the first script that uses it
// NONMATCHING (98.78%): 18 of 103 instructions differ in operands only (registers or offsets)
USER_FUNC(camera_yure) {
    s32 count;
    CameraEntry* cam;
    CameraEntry* cam2;
    Vec vec;
    f32 s;
    f32 scale;

    count = evtGetValue(event, event->args[0]);
    cam = camGetPtr(CAMERA_3D);
    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = count;
    }
    s = (f32)sin(18.849556f * (f32)event->userdata[0] / (f32)event->userdata[1]) * 0.1f;
    scale = (f32)(event->userdata[1] - event->userdata[0]) * s / (f32)event->userdata[1];
    PSVECSubtract(&cam->cameraAt, &cam->cameraPos, &vec);
    PSVECScale(&vec, &vec, scale);
    cam->viewOffset.x = vec.x;
    cam->viewOffset.y = vec.y;
    cam->viewOffset.z = vec.z;
    cam2 = camGetPtr(CAMERA_3D_IMAGE);
    cam2->viewOffset.x = vec.x;
    cam2->viewOffset.y = vec.y;
    cam2->viewOffset.z = vec.z;
    event->userdata[0]++;
    if (event->userdata[0] > event->userdata[1]) {
        return 2;
    }
    return 0;
}

EVT_BEGIN(evt_yabure_on)
    CALL(evt_paper_entry, STRING("OFF_d_yabu_small"))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF4_LANPEL_APPEAR1"), -145, 270, -25, 0)
    BEGIN_INLINE_EVENT
        CALL(camera_yure, 20)
    END_INLINE_EVENT
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_yabu_small"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 100, 100, 202, 160)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_position, STRING("img"), -460, -261)
    CALL(evt_img_set_virtual_point, STRING("img"), 0, 0, -78)
    CALL(evt_img_onoff, STRING("img"), 1)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    RETURN
EVT_END()

EVT_BEGIN(evt_yabure_off)
    CALL(evt_img_release, STRING("img"))
    RETURN
EVT_END()

EVT_BEGIN(evt_nameent)
    CALL(evt_paper_entry, STRING("OFF_d_roll"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_roll"))
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("Z_1"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 1)
    WAIT_FRAMES(1)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_win_nameent_on, LW(0))
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF4_NAME_ENTRY1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_2"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_win_nameent_wait)
    CALL(evt_win_nameent_name, LW(0))
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF4_NAME_ENTRY2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_win_nameent_off)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_roll"))
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

// TODO: write cam_r at the end of the file (deferred-inline order puts it first in .text)

static USER_FUNC(zoom) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec vec;

    PSVECSubtract(&cam->cameraPos, &cam->cameraAt, &vec);
    PSVECScale(&vec, &vec, 0.6f);
    PSVECAdd(&cam->cameraAt, &vec, &vec);
    evtSetFloat(event, args[0], vec.x);
    evtSetFloat(event, args[1], vec.y);
    evtSetFloat(event, args[2], vec.z);
    evtSetFloat(event, args[3], cam->cameraAt.x);
    evtSetFloat(event, args[4], cam->cameraAt.y);
    evtSetFloat(event, args[5], cam->cameraAt.z);
    return 2;
}

EVT_BEGIN(name_seikai)
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_NM_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("にせマリオ"), STRING("EM_O_2"))
    CALL(evt_npc_set_autotalkpose, STRING("にせマリオ"), STRING("EM_Z_3"), STRING("EM_T_4"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(10), LW(11), LW(12))
    ADD(LW(2), LW(12))
    DIVIDE(LW(2), 2)
    ADD(LW(2), 542)
    SET_VALUE(LW(15), LW(2))
    SET_VALUE(LW(14), LW(12))
    SUBTRACT(LW(14), LW(15))
    MULTIPLY_FLOAT(LW(14), FLOAT(0.6f))
    SET_VALUE(LW(13), LW(2))
    ADD(LW(13), LW(14))
    CALL(evt_cam3d_evt_set, LW(10), 73, LW(13), LW(10), 30, LW(12), 500, 11)
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg4_gra_12"), 0, STRING("にせマリオ"))
    SET_VALUE(LW(15), LW(2))
    SET_VALUE(LW(14), LW(12))
    SUBTRACT(LW(14), LW(15))
    MULTIPLY_FLOAT(LW(14), FLOAT(0.79f))
    SET_VALUE(LW(13), LW(2))
    ADD(LW(13), LW(14))
    CALL(evt_cam3d_evt_set, LW(10), 53, LW(13), LW(10), 30, LW(12), 500, 11)
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg4_gra_13"), 0, STRING("にせマリオ"))
    SET_VALUE(LW(15), LW(2))
    SET_VALUE(LW(14), LW(12))
    SUBTRACT(LW(14), LW(15))
    MULTIPLY_FLOAT(LW(14), FLOAT(0.88f))
    SET_VALUE(LW(13), LW(2))
    ADD(LW(13), LW(14))
    CALL(evt_cam3d_evt_set, LW(10), 43, LW(13), LW(10), 30, LW(12), 500, 11)
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg4_gra_14"), 0, STRING("にせマリオ"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg4_gra_15"), 0, STRING("にせマリオ"))
        CALL(evt_msg_continue)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_LANPEL_SHOCK1"), LW(0), LW(1), LW(2), LW(10))
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 8)
    IF_INT_LESS(LW(0), LW(10))
        MULTIPLY(LW(6), -1)
    END_IF
    LOOP(0)
        ADD(LW(5), LW(6))
        IF_INT_LESS(LW(5), 0)
            ADD(LW(5), 360)
        END_IF
        IF_INT_GREATER(LW(5), 360)
            SUBTRACT(LW(5), 360)
        END_IF
        CALL(cam_r, LW(5))
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(cam_r, 0)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(3), LW(4), LW(5))
    ADD(LW(0), LW(3))
    DIVIDE(LW(0), 2)
    ADD(LW(2), LW(5))
    DIVIDE(LW(2), 2)
    SET_VALUE(LW(3), LW(2))
    ADD(LW(3), 542)
    CALL(evt_cam3d_evt_set, LW(0), 82, LW(3), LW(0), 30, LW(2), 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("にせマリオ"), STRING("EM_J_2"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("にせマリオ"), STRING("EM_J_2"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        LOOP(0)
            LOOP(0)
                CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
                CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
                CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(10))
                IF_INT_LESS(LW(10), 20)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_MARIO_ROUND1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_set_pose, STRING("M_D_1"))
            CALL(evt_sub_intpl_msec_init, 5, 0, 3600, 2000)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_mario_set_dispdir, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            IF_INT_EQUAL(GF(0), 1)
                SET_VALUE(GF(1), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        LOOP(0)
            LOOP(0)
                CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
                CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
                CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(10))
                IF_INT_LESS(LW(10), 20)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_gra_vivian_pose_set, 1, STRING("PTR_S_4"))
            CALL(evt_gra_vivian_pose_set, 7, STRING("PTR_T_5"))
            CALL(evt_party_set_pose, 0, STRING("PTR_D_1"))
            CALL(evt_sub_intpl_msec_init, 5, 0, 3600, 2000)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_party_set_dispdir, 0, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
            CALL(evt_party_set_pose, 0, STRING("PTR_S_1"))
            IF_INT_EQUAL(GF(0), 1)
                SET_VALUE(GF(2), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_party_run, 0)
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("にせマリオ"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("にせマリオ"), 1024)
    CALL(evt_npc_status_onoff, 1, STRING("にせマリオ"), 2)
    CALL(evt_npc_set_anim, STRING("にせマリオ"), STRING("EM_R_3"))
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_NM_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    ELSE
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_NM_SURPRISED1_2"), LW(0), LW(1), LW(2), 0)
    END_IF
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("にせマリオ"), -300, LW(2), 0, FLOAT(300.0f), 0)
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_NM_SURPRISED1_2"), LW(0), LW(1), LW(2), 0)
    ELSE
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_NM_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    END_IF
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("にせマリオ"), 200, LW(2), 0, FLOAT(300.0f), 0)
    CALL(evt_npc_set_position, STRING("にせマリオ"), 0, -1000, 0)
    SET_VALUE(GF(0), 1)
    CALL(zoom, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_party_get_pos, 0, LW(13), LW(14), LW(15))
    ADD(LW(10), LW(13))
    DIVIDE(LW(10), 2)
    CALL(evt_cam3d_evt_set, LW(10), LW(1), LW(2), LW(10), LW(4), LW(5), 800, 11)
    LOOP(0)
        IF_INT_EQUAL(GF(1), 1)
            IF_INT_EQUAL(GF(2), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 300, 0)
    END_INLINE_EVENT
    CALL(evt_party_set_dir, 0, 90, 300)
    WAIT_MS(500)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_gra_vivian_pose_set, 1, STRING("PTR_S_1"))
    CALL(evt_gra_vivian_pose_set, 7, STRING("PTR_T_1"))
    CALL(evt_msg_print_party, STRING("stg4_gra_16"))
    CALL(evt_party_run, 0)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 500, 11)
    RETURN
EVT_END()

EVT_BEGIN(evt_machibuse)
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(0), 30)
            IF_INT_GREATER_OR_EQUAL(LW(0), -126)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_bgmoff_f, 512, 120)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_normalize)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(0), -48)
            CALL(evt_mario_set_dir, 90, 200, 0)
            CALL(evt_party_set_dir, 0, 90, 200)
        ELSE
            CALL(evt_mario_set_dir, 270, 200, 0)
            CALL(evt_party_set_dir, 0, 270, 200)
        END_IF
        CALL(evt_mario_set_pose, STRING("M_N_3"))
    END_INLINE_EVENT
    CALL(evt_npc_set_position, STRING("にせマリオ"), -125, 240, -25)
    IF_INT_EQUAL(GSW(0), 206)
        CALL(evt_msg_print, 0, STRING("stg4_gra_05"), 0, STRING("にせマリオ"))
    ELSE
        CALL(evt_msg_print, 0, STRING("stg4_gra_00"), 0, STRING("にせマリオ"))
    END_IF
    CALL(evt_npc_set_position, STRING("にせマリオ"), 0, -1000, 0)
    CALL(evt_cam3d_evt_set, -88, 352, 517, -88, 257, -25, 800, 11)
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_RPL_APPEAR1"))
    CALL(evt_snd_envoff, 1024)
    CALL_CHILD(evt_yabure_on)
    IF_INT_LESS(LW(0), -48)
        SET_VALUE(LF(0), 1)
    ELSE
        SET_VALUE(LF(0), 0)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mario_set_pos, -100, 0, 0)
    ELSE
        CALL(evt_mario_set_pos, 0, 0, 0)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_EQUAL(LF(0), 1)
        ADD(LW(0), -40)
    ELSE
        ADD(LW(0), 40)
    END_IF
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_EQUAL(LF(0), 1)
        ADD(LW(0), 75)
    ELSE
        ADD(LW(0), -75)
    END_IF
    WAIT_MS(200)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_position, STRING("にせマリオ"), -145, 270, -25)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_LANPEL_M_FALL1"), -145, 270, -25, LW(10))
        CALL(evt_npc_jump_position_nohit, STRING("にせマリオ"), LW(0), LW(1), LW(2), 700, FLOAT(130.0f))
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_set_dir_to_target, STRING("にせマリオ"), STRING("mario"))
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_NM_HAPPY1"), -145, 270, -25, 0)
    CALL_CHILD(evt_yabure_off)
    CALL(evt_gra_vivian_pose_set, 1, STRING("PTR_S_4"))
    CALL(evt_gra_vivian_pose_set, 2, STRING("PTR_S_4"))
    CALL(evt_gra_vivian_pose_set, 3, STRING("PTR_S_4"))
    CALL(evt_gra_vivian_pose_set, 7, STRING("PTR_T_5"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(3), LW(4), LW(5))
    ADD(LW(0), LW(3))
    DIVIDE(LW(0), 2)
    ADD(LW(2), LW(5))
    DIVIDE(LW(2), 2)
    SET_VALUE(LW(3), LW(2))
    ADD(LW(3), 542)
    CALL(evt_cam3d_evt_set, LW(0), 137, LW(3), LW(0), 30, LW(2), 0, 11)
    WAIT_FRAMES(1)
    IF_INT_LESS(GSW(0), 203)
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("stg4_gra_01"), 0, STRING("にせマリオ"))
        CALL(zoom, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
        CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 900, 11)
        WAIT_MS(900)
        CALL(evt_msg_print, 0, STRING("stg4_gra_02"), 0, STRING("にせマリオ"))
    ELSE
        CALL(zoom, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
        CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 1200, 11)
        WAIT_MS(200)
    END_IF
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    WAIT_MS(1000)
    SWITCH(GSW(0))
        CASE_LESS(203)
        CALL(evt_msg_print, 0, STRING("stg4_gra_03"), 0, STRING("にせマリオ"))
        CASE_LESS(206)
        CALL(evt_msg_print, 0, STRING("stg4_gra_09"), 0, STRING("にせマリオ"))
        CASE_LESS(207)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 15)
        ADD(LW(2), 30)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(70.0f))
        WAIT_MS(500)
        CALL(evt_party_stop, 0)
        WAIT_MS(300)
        CALL(evt_msg_print_party, STRING("stg4_gra_06"))
        CALL(evt_msg_print, 0, STRING("stg4_gra_07"), 0, STRING("にせマリオ"))
        CALL(evt_party_run, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_gra_09"), 0, STRING("にせマリオ"))
    END_SWITCH
    SWITCH(GSW(0))
        CASE_LESS(209)
        SET_VALUE(LW(0), 2)
        CALL_CHILD(evt_nameent)
        WAIT_MS(300)
        CASE_DEFAULT
        SET_VALUE(LW(0), 3)
        CALL_CHILD(evt_nameent)
        WAIT_MS(300)
        IF_STR_EQUAL(LW(0), STRING("ランペル"))
            CALL_CHILD(name_seikai)
            SET_VALUE(GSW(0), 210)
            CALL(evt_pouch_remove_item, 68, LW(0))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_GRA1"))
            CALL(evt_snd_envoff, 513)
            CALL(evt_snd_envon, 288, 0)
            CALL(evt_gra_vivian_pose_set, 1, STRING("PTR_S_1"))
            CALL(evt_gra_vivian_pose_set, 2, STRING("PTR_W_1"))
            CALL(evt_gra_vivian_pose_set, 3, STRING("PTR_R_1"))
            CALL(evt_gra_vivian_pose_set, 7, STRING("PTR_T_1"))
            CALL(evt_mario_dispflag_onoff, 0, 2)
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_party_run, 0)
            RETURN
        END_IF
    END_SWITCH
    CALL(evt_msg_print, 0, STRING("stg4_gra_04"), 0, STRING("にせマリオ"))
    CALL(evt_npc_set_battle_info, STRING("にせマリオ"), 2)
    CALL(evt_npc_battle_start, STRING("にせマリオ"))
    CALL(evt_npc_wait_battle_end)
    SWITCH(GSW(0))
        CASE_LESS(203)
        SET_VALUE(GSW(0), 203)
        CASE_LESS(206)
        CASE_LESS(207)
        SET_VALUE(GSW(0), 207)
    END_SWITCH
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_gra_vivian_pose_set, 1, STRING("PTR_S_4"))
    CALL(evt_gra_vivian_pose_set, 7, STRING("PTR_T_5"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("にせマリオ"), LW(10), LW(11), LW(12))
    CALL(evt_mario_set_pos, LW(10), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("にせマリオ"), LW(0), LW(11), LW(12))
    CALL(evt_set_dir_to_target, STRING("にせマリオ"), STRING("mario"))
    IF_INT_LESS(LW(10), LW(0))
        CALL(evt_cam3d_evt_set, -225, 134, 534, -225, 39, -8, 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("にせマリオ"), 32)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_mov_pos2, -385, 0, FLOAT(200.0f))
        END_INLINE_EVENT
        WAIT_MS(1100)
        CALL(evt_cam3d_evt_set, -88, 137, 517, -88, 42, -25, 800, 11)
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("stg4_gra_11"), 0, STRING("にせマリオ"))
        CALL(evt_bero_mapchange, STRING("usu_01"), STRING("e_bero"))
    ELSE
        CALL(evt_cam3d_evt_set, 186, 149, 534, 186, 54, -8, 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("にせマリオ"), 32)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_mov_pos2, 385, 0, FLOAT(200.0f))
        END_INLINE_EVENT
        WAIT_MS(1250)
        CALL(evt_cam3d_evt_set, -20, 137, 517, -20, 42, -25, 800, 11)
        WAIT_MS(850)
        CALL(evt_msg_print, 0, STRING("stg4_gra_11"), 0, STRING("にせマリオ"))
        CALL(evt_bero_mapchange, STRING("gra_01"), STRING("w_bero1"))
    END_IF
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gra_01", "w_bero1", 1, 1},
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "usu_01", "e_bero", 1, 1},
    {NULL},
};

static NpcSetupInfo npcEnt[2] = {
    { "にせマリオ", 0x00000600 },
    { NULL },
};

static TreeAccessEntry tree_tbl[2] = {
    { 2, "A_ki_1", "S_gra00_ki", NULL, NULL, NULL },
    { 7, NULL, NULL, NULL, NULL, NULL },
};

EVT_BEGIN(evt_majin_key)
    SET_VALUE(GSWF(2091), 1)
    RETURN
EVT_END()

EVT_BEGIN(gra_00_init_evt)
    SET_VALUE(GSWF(256), 1)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_GRA1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_USU1"))
    CALL(evt_kagemario_init_11_text_804)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_11_data_4F98)
    CALL(evt_npc_setup, PTR(npcEnt))
    SET_VALUE(LW(0), PTR(tree_tbl))
    CALL_CHILD(evt_sub_tree_access_entry)
    CALL(evt_mobj_brick, STRING("blk01"), 250, 15, 100, 0, 0, 0, LSWF(1))
    CALL(evt_mobj_brick, STRING("blk02"), -265, 60, -85, 0, 3, 0, GSWF(2089))
    IF_INT_EQUAL(GSWF(2091), 0)
        CALL(evt_item_entry, STRING("majin_key"), 35, -258, 0, -85, 16, -1, PTR(evt_majin_key))
    END_IF
    SWITCH(GSW(0))
        CASE_BETWEEN(202, 209)
        SPAWN_EVENT(evt_machibuse)
    END_SWITCH
    RETURN
EVT_END()

// TODO: write msg_callback (global, 0xdc bytes) here:
// its asm is in build/G8MJ01/gra/asm/gra_00.s; this is where the original has it, just before the first script that uses it
void msg_callback(s32 state, smartEntry* smart) {
    MessageData* data = smart->address;
    EventEntry* event = data->event;

    switch (state) {
    case 0:
        evtSetValue(event, GW(0), 0);
        evtSetValue(event, GW(1), 0);
        event->lwData[15] = 0;
        break;
    case 5:
        if (event->lwData[15] == 3) {
            marioKeyOff();
        }
        event->lwData[15]++;
        break;
    case 1:
        evtSetValue(event, GW(1), 1);
        break;
    case 3:
        evtSetValue(event, GW(0), 1);
        break;
    }
}

static USER_FUNC(cam_r) {
    f32 angle = evtGetFloat(event, event->args[0]);
    f32 zero = 0.0f;
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    if (angle == zero) {
        cam->rollAngle = zero;
        cam->viewOffset.z = zero;
    } else {
        f32 yure;
        cam->rollAngle = angle;
        yure = (f32)sin((f32)gp->retraceCount / 10.0f);
        cam->viewOffset.z = -(__fabsf(15.0f * yure));
        cam->cameraPos.y = 97.0f;
        cam->cameraAt.y = 45.0f;
    }
    return 2;
}
// clang-format on

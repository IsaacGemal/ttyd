#include "dou_07.h"
#include "driver/animdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_lecture.h"
#include "evt/evt_map.h"
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
#include "npc_event.h"
#include "party/party_motion.h"
#include "pmario_sound.h"

#pragma pool_data off

#pragma force_active on
extern s32 bubble_find_event[];
extern s32 bubble_init_event[];
extern s32 bubble_lost_event[];
extern s32 bubble_move_event[];
extern s32 bubble_return_event[];
extern s32 ship_majin_lecture[];
extern s32 ship_majin_lecture_init[];
static USER_FUNC(bgm_start_wait);
static void majin_callback(s32 type);

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(erumosu_regl)
    RETURN
EVT_END()

EVT_BEGIN(erumosu_dead)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    SET_VALUE(GSWF(2989), 1)
    SPAWN_EVENT(enemy_event_dead_event)
    WAIT_MS(500)
    CALL(evt_item_entry, STRING("item_key"), 36, LW(0), LW(1), LW(2), 4, GSWF(2987), 0)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[2] = {
    {
        "エルモス", 0x00000608, 0x00000000, bubble_init_event,
        bubble_move_event, 0x00000000, erumosu_dead, bubble_find_event,
        bubble_lost_event, bubble_return_event, 0x00000000, 0x00000000,
        100.0f, -6.0f, -37.0f, 50.0f,
        50.0f, 50.0f, 200.0f, 300.0f,
        1000.0f, 360.0f, 0x0000000F,
    },
};

static BeroEntry bero_entry_data[2] = {
    {
        "s_bero", 0x0000, 0x0000, 0x00004E20,
        0x000186A0, 0x00000000, 0x00000000, -1,
        0x00000000, -1, 0x00000000, "dou_08",
        "n_bero", 0x0001, 0x0001, 0x00000000,
        0x00000000,
    },
};

static u32 key_tbl[2] = {
    0x00000024, 0xFFFFFFFF,
};

EVT_BEGIN(apper_elmos)
    SET_VALUE(GSWF(2988), 1)
    CALL(evt_npc_set_position, STRING("エルモス"), 100, -6, -37)
    CALL(evt_npc_set_color, STRING("エルモス"), 255, 255, 255, 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_color, STRING("エルモス"), 255, 255, 255, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static void majin_callback(s32 type) {
    MapObjectEntry* obj = mobjNameToPtr("mobj_box00");

    if (type == 0) {
        animPoseSetAnim(obj->poseId, "A_2", 1);
    } else if (type == 5) {
        if (animPoseGetLoopTimes(obj->poseId) >= 1.0f) {
            animPoseSetAnim(obj->poseId, "A_2", 1);
        }
    } else if (type == 2) {
        animPoseSetAnim(obj->poseId, "S_1", 1);
    }
}

EVT_BEGIN(peramajin_before)
    IF_INT_EQUAL(GSW(0), 246)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_normalize)
        CALL(evt_party_stop, 0)
        CALL(evt_mario_adjust_dir)
        CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
        ADD(LW(1), 30)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            IF_INT_LESS_OR_EQUAL(LW(3), LW(0))
                CALL(evt_mario_set_dir, 90, 200, 0)
            ELSE
                CALL(evt_mario_set_dir, 270, 200, 0)
            END_IF
        END_INLINE_EVENT
        CALL(evt_pouch_check_item, 36, LW(10))
        IF_INT_LESS_OR_EQUAL(LW(10), 0)
            CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
            CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_08"), PTR(majin_callback), 0)
            CALL(evt_mobj_exec_cancel, STRING("mobj_box00"))
            CALL(evt_party_run, 0)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_09"), PTR(majin_callback), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(10), 0)
        IF_INT_EQUAL(LW(10), -1)
            CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
            CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_10"), PTR(majin_callback), 0)
            CALL(evt_mobj_exec_cancel, STRING("mobj_box00"))
            CALL(evt_party_run, 0)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            ADD(LW(3), 35)
            ADD(LW(5), 15)
            CALL(evt_party_move_pos2, 0, LW(3), LW(5), FLOAT(80.0f))
            IF_INT_LESS_OR_EQUAL(LW(3), LW(0))
                CALL(evt_party_set_dir, 0, 90, -1)
            ELSE
                CALL(evt_party_set_dir, 0, 270, -1)
            END_IF
        END_INLINE_EVENT
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_11"), PTR(majin_callback), 0)
        CALL(evt_mario_set_pose, STRING("M_I_N"))
        CALL(evt_cam3d_evt_set, -33, 92, 412, -33, 36, 92, 500, 11)
        CALL(evt_pouch_remove_item, 36, LW(10))
        CALL(evt_party_run, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_adjust_dir)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        IF_INT_LESS_OR_EQUAL(LW(3), LW(0))
            CALL(evt_mario_set_dir, 90, 200, 0)
        ELSE
            CALL(evt_mario_set_dir, 270, 200, 0)
        END_IF
    END_INLINE_EVENT
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_01"), 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLACKBOX_SWING1"), LW(0), LW(1), LW(2), LW(15))
    CALL(evt_mobj_set_anim, STRING("mobj_box00"), STRING("A_3"))
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(300)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_02"), PTR(majin_callback), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_03"), PTR(majin_callback), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_04"), PTR(majin_callback), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_05"), PTR(majin_callback), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_06"), PTR(majin_callback), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_07"), PTR(majin_callback), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_APPEAR1"), 0)
    CALL_CHILD(apper_elmos)
    CALL(evt_mobj_exec_cancel, STRING("mobj_box00"))
    SET_VALUE(GSW(0), 246)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static s32 bgm_start_wait(EventEntry* event, BOOL isFirstCall) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(peramajin_after)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_pouch_add_item, 5, LW(0))
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_PERAMAJIN1"))
    CALL(evt_snd_envoff, 1024)
    CALL(bgm_start_wait)
    SET_VALUE(LW(1), 60)
    LOOP(LW(1))
        SET_VALUE(LW(0), 255)
        MULTIPLY(LW(0), LW(1))
        DIVIDE(LW(0), 60)
        CALL(evt_mapobj_color, 1, STRING("s02_moku"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_color, 1, STRING("s02_moku"), 255, 255, 255, 0)
    CALL(evt_paper_entry, STRING("OFF_d_kusya"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_kusya"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_set_virtual_point, STRING("img"), -33, 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_img_clear_virtual_point, STRING("img"))
    END_INLINE_EVENT
    CALL(evt_img_onoff, STRING("img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_APPEAR1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_COME1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_mario_set_camid, 7)
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_mapobj_color, 1, STRING("s02_moku"), 255, 255, 255, 255)
    CALL(lect_cam_save)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    WAIT_FRAMES(1)
    CALL_CHILD(ship_majin_lecture_init)
    CALL(evt_party_run, 0)
    CALL_CHILD(ship_majin_lecture)
    CALL(evt_party_stop, 0)
    LOOP(6)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mario_set_pos, LW(10), LW(11), LW(12))
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_mario_init_camid)
    CALL(lect_cam_load, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_GOOUT1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff_f, 513, 2000)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_2"))
    CALL(evt_img_wait_animend, STRING("img"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_GOOUT2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_kusya"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print_party, STRING("stg1_gon_majin4_19"))
    SET_VALUE(GSW(0), 247)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    RETURN
EVT_END()

EVT_BEGIN(dou_07_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU7"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_EQUAL(GSWF(2989), 0)
        CALL(evt_npc_setup, PTR(npcEnt))
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change_01"), STRING("Ship_Change_01"))
    CALL(evt_map_playanim, STRING("moku"), 1, 0)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    SET_VALUE(GF(0), 0)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), -33, -18, 56, 3, PTR(peramajin_before), PTR(peramajin_after), GSWF(2980))
    CALL(evt_item_entry, STRING("item_00"), 270, -75, -30, -15, 16, GSWF(2985), 0)
    IF_INT_EQUAL(GSWF(2988), 1)
        IF_INT_EQUAL(GSWF(2989), 0)
            SPAWN_EVENT(apper_elmos)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(2989), 1)
        CALL(evt_item_entry, STRING("item_key"), 36, 100, -6, -37, 14, GSWF(2987), 0)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

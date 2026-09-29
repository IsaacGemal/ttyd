#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_movefloor.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

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

EVT_BEGIN(bero_00_open_door)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(bero_00_close_door)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"n_bero", 1, 1, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_08", "s_bero", 5, 5, (s32)&bero_00_close_door, (s32)&bero_00_open_door},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_00", "w_bero", 1, 1},
    {"dokan__1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "bom_00", "dokan", 1, 1},
    {"dokan_2", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "tik_18", "dokan_2", 1, 1},
    {"dokan_3", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_5", 1, 1},
    {"dokan_4", 2, 0, 2, 100000, 0, 0, -1, NULL, 10, NULL, "gor_03", "dokan_1", 1, 1},
    {"dokan_5", 0x1002, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_3", 1, 1},
    {NULL},
};

static BeroEntry bero_entry_data2[] = {
    {"dokan__1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "sys_01", "stg_7", 1, 1},
    {NULL},
};

EVT_BEGIN(tik_07_down_evt)
    IF_INT_EQUAL(GSWF(1223), 0)
        RETURN
    END_IF
    SET_VALUE(GSWF(1223), 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 100, 300, 0)
    CALL(evt_party_set_pos, 0, 70, 300, 0)
    CALL(evt_party_set_pos, 1, 85, 300, -30)
    CALL(evt_cam3d_evt_set, FLOAT(100.0f), FLOAT(252.6f), FLOAT(528.7f), FLOAT(100.0f), FLOAT(157.1f), FLOAT(-13.0f), 0, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_seq_wait, 2)
        CALL(evt_mario_cont_onoff, 1)
        CALL(evt_party_cont_onoff, 0, 1)
        CALL(evt_party_cont_onoff, 1, 1)
        CALL(evt_mario_wait_landon)
        WAIT_MS(100)
        CALL(evt_cam_road_reset)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_MS(400)
        CALL(evt_mario_key_onoff, 1)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(peach_mail)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION1"))
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 40)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_msg_print_party, STRING("tik_07_mail4_00"))
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_cam3d_evt_set, -218, 209, 233, -218, 157, -63, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_msg_print, 0, STRING("tik_07_mail4_01"), 0, 0)
    CALL(evt_msg_print_party, STRING("tik_07_mail4_02"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 220)
    CALL(evt_pouch_receive_mail, 3)
    CALL(evt_pouch_open_mail, 3)
    RETURN
EVT_END()

static EVT_BEGIN(evt_block)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_hikari"), 1)
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 282, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tik_07_init_evt)
    CALL_CHILD(tik_07_down_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 475, 60, 125, 0, 0)
    CALL(evt_mobj_lv_blk, STRING("mobj_blk01"), -300, 100, -50, PTR(evt_block), GSWF(1340), STRING("MOBJ_Lv2BigBlock"))
    IF_INT_EQUAL(GSWF(1340), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_hikari"), 1)
    END_IF
    CALL(evt_mobj_itembox, STRING("mobj_box00"), -550, 0, 100, 0, 0, PTR(mobj_box00), GSWF(1359))
    CALL(evt_mobj_powerupblk, STRING("pblk2"), -180, 160, 0, 0, GSWF(5533), 0)
    CALL(evt_item_entry, STRING("item_00"), 125, 460, 160, -450, 16, GSWF(5594), 0)
    CALL(evt_item_set_scale, STRING("item_00"), FLOAT(0.6f))
    CALL(evt_item_entry, STRING("item_01"), 125, 125, 0, -60, 16, GSWF(5595), 0)
    SET_VALUE(LW(0), STRING("ugo_1"))
    SET_VALUE(LW(1), STRING("a_ugo_1"))
    SET_VALUE(LW(2), -20)
    SET_VALUE(LW(3), 20)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 1)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT(moving_floor)
    SET_VALUE(LW(0), STRING("ugo_2"))
    SET_VALUE(LW(1), STRING("a_ugo_2"))
    SET_VALUE(LW(2), 20)
    SET_VALUE(LW(3), -20)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 1)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT(moving_floor)
    SET_VALUE(LW(0), STRING("ugo_3"))
    SET_VALUE(LW(1), STRING("a_ugi_3"))
    SET_VALUE(LW(2), -70)
    SET_VALUE(LW(3), 30)
    SET_VALUE(LW(4), 3000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT(moving_floor)
    SET_VALUE(LW(0), STRING("ugo_4"))
    SET_VALUE(LW(1), STRING("a_ugo_4"))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), -60)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 1)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT(moving_floor)
    SET_VALUE(GW(0), 5000)
    SET_VALUE(GW(1), 0)
    SET_VALUE(LW(0), STRING("kur_1"))
    SET_VALUE(LW(1), STRING("a_kur_1"))
    SET_VALUE(LW(2), 60)
    SET_VALUE(LW(3), -20)
    SET_VALUE(LW(4), 50)
    SET_VALUE(LW(5), 1)
    SET_RAW_VALUE(LW(6), GW(0))
    SET_RAW_VALUE(LW(7), GW(1))
    SPAWN_EVENT(ridedown_floor)
    SET_VALUE(LW(0), STRING("kur_2"))
    SET_VALUE(LW(1), STRING("a_kur_2"))
    SET_VALUE(LW(2), 60)
    SET_VALUE(LW(3), -20)
    SET_VALUE(LW(4), 50)
    SET_VALUE(LW(5), 0)
    SET_RAW_VALUE(LW(6), GW(0))
    SET_RAW_VALUE(LW(7), GW(1))
    SPAWN_EVENT(ridedown_floor)
    IF_INT_LESS(GSW(0), 261)
        CALL(evt_npc_check_delete, STRING("トゲノコ"))
        CALL(evt_npc_check_delete, STRING("ハンマーブロス"))
    ELSE
        CALL(evt_npc_check_delete, STRING("ハンニャー１"))
        CALL(evt_npc_check_delete, STRING("ハンニャー２"))
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 349)
        IF_INT_LESS(GSW(0), 352)
            SET_VALUE(LW(0), STRING("dokan__1"))
            CALL_CHILD(bero_case_switch_off)
            CALL(evt_bero_overwrite, STRING("dokan__1"), PTR(bero_entry_data2))
            SET_VALUE(LW(0), STRING("dokan__1"))
            CALL_CHILD(bero_case_switch_on)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 219)
        SPAWN_EVENT(peach_mail)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

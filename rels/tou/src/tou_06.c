#include "tou_06.h"
#include <stdio.h>
#include <stdlib.h>
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(get_pos);
s32* tbl;

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(n_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[] = {
    {"n_bero", 0x0001, 0x0004, 0x00004E20, 0x000186A0, 0, 0, -1, NULL, -1, NULL, "tou_04", "s_bero_4", 0x0005, 0x0005,
     n_door_close, n_door_open},
    {"w_bero", 0x0001, 0x0004, 0x00000002, 0x000186A0, 0, 0, -1, NULL, -1, NULL, "tou_12", "w_bero", 0x0005, 0x0005,
     w_door_close, w_door_open},
    {NULL},
};

EVT_BEGIN(evt_ana)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_mapchange, STRING("tou_05"), STRING("fall"))
    RETURN
EVT_END()

EVT_BEGIN(evt_kontena)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    SET_VALUE(GSW(0), 149)
    RETURN
EVT_END()

EVT_BEGIN(evt_kaidan)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -90, 208, 680, -90, 80, -8, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 1)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        LOOP(8)
            CALL(evt_snd_sfxon_3d, STRING("SFX_OFF3_PERA_KAIDAN_APPEAR1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(350)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidan"), 1)
    CALL(evt_hitobj_onoff, STRING("a_kaidan"), 1, 1)
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_release, STRING("img"))
    SET_VALUE(GSW(0), 150)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

char* cloud_kontena2_list[5] = {
    "kontena_2",
    "fuda_1",
    "fuda_2",
    "fuda_3",
    NULL,
};

EVT_BEGIN(evt_kontena2)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuda_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuda_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuda_3"), 1)
    SET_VALUE(GSWF(2435), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_block_broken)
    SET_VALUE(GSW(0), 151)
    RETURN
EVT_END()

static s32 key_tbl[2] = {
    19, -1,
};

static EVT_BEGIN(evt_chk)
    CALL(evt_pouch_check_item, 19, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_lock)
    SET_VALUE(GSW(0), 155)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_pouch_remove_item, 19, LW(0))
    RETURN
EVT_END()

EVT_BEGIN(evt_chuchu)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_CYUCYURINA1"))
    CALL(evt_snd_envoff, 512)
    CALL(evt_npc_set_position, STRING("チュチュリーナ"), -100, 50, -60)
    CALL(evt_npc_set_ry, STRING("チュチュリーナ"), 270)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_MS(200)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir_npc, STRING("チュチュリーナ"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, STRING("m"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, 300)
        CALL(evt_eff_fukidashi, 3, STRING("p"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_set_dir_to_target, STRING("チュチュリーナ"), STRING("mario"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -75, 117, 224, -75, 88, -8, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg3_tou_308"), 0, STRING("チュチュリーナ"))
    WAIT_MS(300)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, 16, 0, -60)
    CALL(evt_party_set_pos, 0, 40, 0, -65)
    CALL(evt_cam3d_evt_set, 4, 67, 224, 4, 38, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg3_tou_309"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -75, 117, 224, -75, 88, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_310"), 0, STRING("チュチュリーナ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 4, 67, 224, 4, 38, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg3_tou_311"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -75, 117, 224, -75, 88, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_312"), 0, STRING("チュチュリーナ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 4, 67, 224, 4, 38, -8, 1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 32)
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), -65, -60, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), -10, 0, -60, 300, 30)
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), 0, -60, 0, FLOAT(80.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_313"), 0, STRING("チュチュリーナ"))
    WAIT_MS(300)
    CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_CHURINA_KISS1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_CHURINA_KISS2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_K_1"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
        ADD(LW(1), 30)
        CALL(evt_eff, 0, STRING("kiss"), 0, LW(0), LW(1), LW(2), -45, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_S_1"))
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), -20, -60, 0, FLOAT(80.0f), 1)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_set, 25, 67, 224, 25, 38, -8, 300, 11)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_314"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 4, 67, 224, 4, 38, -8, 300, 0)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_315"), 0, STRING("チュチュリーナ"))
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), 0, -140, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("チュチュリーナ"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("チュチュリーナ"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
    CALL(evt_snd_bgmoff, 26624)
    CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
    CALL(evt_msg_print_party, STRING("stg3_tou_316"))
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
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
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_316_01"), 0, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_316_02"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_I_Z"))
    WAIT_MS(1500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_pouch_receive_mail, 12)
    CALL(evt_pouch_open_mail, 12)
    SET_VALUE(GSW(0), 148)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static s32 list[25][3] = {
    {-305, 250, 130},
    {-221, 200, 5},
    {-235, 200, 85},
    {-159, 250, -14},
    {-129, 200, 45},
    {80, 200, 39},
    {67, 260, -25},
    {26, 200, 89},
    {-85, 130, 126},
    {-180, 60, 79},
    {-72, 0, -72},
    {-18, 0, -21},
    {61, 0, -74},
    {119, 0, -35},
    {162, 0, 31},
    {111, 0, 43},
    {47, 0, 37},
    {12, 0, -4},
    {-56, 0, 33},
    {-20, 0, 97},
    {58, 0, 90},
    {116, 0, 105},
    {158, 0, 112},
    {-78, 0, 131},
    {-116, 0, -111},
};

EVT_BEGIN(iri_13_pants_00_evt)
    SET_VALUE(GSWF(5408), 1)
    RETURN
EVT_END()

USER_FUNC(make_tbl) {
    s32 i;
    s32 j;
    s32 k;
    s32 temp;

    tbl = _mapAlloc(0x64);
    for (i = 0; i < 25; i++) {
        tbl[i] = i;
    }
    for (i = 0; i < 50; i++) {
        j = rand() % 25;
        k = rand() % 25;
        temp = tbl[j];
        tbl[j] = tbl[k];
        tbl[k] = temp;
    }
    if (evtGetValue(NULL, GSWF(5408)) == 0) {
        for (i = 0; i < 25; i++) {
            if (tbl[i] == 0) {
                temp = tbl[0];
                tbl[0] = tbl[i];
                tbl[i] = temp;
                break;
            }
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(get_pos) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 entry = tbl[idx];

    evtSetValue(event, args[1], list[entry][0]);
    evtSetValue(event, args[2], list[entry][1]);
    evtSetValue(event, args[3], list[entry][2]);
    return EVT_RETURN_DONE;
}

USER_FUNC(free_tbl) {
    _mapFree(tbl);
    tbl = NULL;
    return EVT_RETURN_DONE;
}

USER_FUNC(iri_13_make_name) {
    static char name[8];
    s32* args = event->args;

    sprintf(name, "pants%02d", evtGetValue(event, args[0]));
    evtSetValue(event, args[1], (s32)name);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(iri_13_init)
    IF_INT_EQUAL(GSWF(5406), 1)
        RETURN
    END_IF
    CALL(evt_pouch_check_item, 107, LW(0))
    SET_VALUE(LW(1), 20)
    SUBTRACT(LW(1), LW(0))
    CALL(make_tbl)
    SET_VALUE(LW(2), 0)
    LOOP(0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        CALL(get_pos, LW(2), LW(3), LW(4), LW(5))
        CALL(iri_13_make_name, LW(2), LW(6))
        SET_VALUE(LF(0), 0)
        IF_INT_EQUAL(LW(2), 0)
            IF_INT_EQUAL(GSWF(5408), 0)
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_item_entry, LW(6), 107, LW(3), LW(4), LW(5), 16, -1, PTR(iri_13_pants_00_evt))
        ELSE
            CALL(evt_item_entry, LW(6), 107, LW(3), LW(4), LW(5), 16, -1, 0)
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("gor_hae"), 1)
        ADD(LW(2), 1)
        SUBTRACT(LW(1), 1)
    END_LOOP
    CALL(free_tbl)
    IF_INT_NOT_EQUAL(LW(2), 0)
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(evt_sub_random, 1000, LW(0))
                ADD(LW(0), 3000)
                WAIT_MS(LW(0))
                CALL(evt_mapobj_get_position, STRING("gor_hae"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FLY1"), LW(0), LW(1), LW(2), 0)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"チュチュリーナ", 0x40000600},
    {NULL},
};

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 257, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tou_06_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_run_case_evt, 0, 1, STRING("ana"), 0, PTR(evt_ana), 0)
    IF_INT_LESS(GSW(0), 149)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 0, STRING("kontena"), STRING("a_kon_H"), PTR(evt_kontena))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kontena"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kon_H"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(2435), 0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 149)
            CALL(evt_cloud_init)
        END_IF
        CALL(evt_cloud_ent, 1, PTR(cloud_kontena2_list), STRING("a_kon_N"), PTR(evt_kontena2))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kontena_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuda_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuda_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuda_3"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kon_N"), 1, 0)
    END_IF
    CALL(evt_mobj_lv_blk, STRING("i"), 126, -1, -126, 0, GSWF(2433), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("j"), 176, -1, -76, 0, GSWF(2434), STRING("MOBJ_Lv1BigBlock"))
    IF_INT_EQUAL(GSWF(2433), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("i"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2434), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("j"), 64)
    END_IF
    CALL(evt_mobj_lv_blk, STRING("a"), -126, -1, -126, 0, GSWF(2425), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("b"), -76, -1, -126, 0, GSWF(2426), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("c"), -176, -1, -76, 0, GSWF(2427), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("d"), -126, -1, -76, 0, GSWF(2428), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("e"), -76, -1, -76, 0, GSWF(2429), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("f"), -176, -1, -26, 0, GSWF(2430), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("g"), -126, -1, -26, 0, GSWF(2431), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("h"), -76, -1, -26, 0, GSWF(2432), STRING("MOBJ_Lv1BigBlock"))
    IF_INT_EQUAL(GSWF(2425), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("a"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2426), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("b"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2427), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("c"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2428), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("d"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2429), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("e"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2430), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("f"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2431), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("g"), 64)
    END_IF
    IF_INT_EQUAL(GSWF(2432), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("h"), 64)
    END_IF
    IF_INT_LESS(GSW(0), 150)
        CALL(evt_mobj_switch_blue, 0, STRING("sw"), -165, 0, -115, PTR(evt_kaidan), GSWF(2438))
        CALL(evt_mobj_flag_onoff, 0, STRING("sw"), 64)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kaidan"), 1)
        CALL(evt_hitobj_onoff, STRING("a_kaidan"), 1, 0)
        CALL(evt_paper_entry, STRING("OFF_d_ori"))
        CALL(evt_img_entry, STRING("img"))
        CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_ori"))
        CALL(evt_img_onoff, STRING("img"), 0)
    END_IF
    IF_INT_LESS(GSW(0), 151)
        CALL(evt_mobj_lv_blk, STRING("aa"), -450, 200, 55, PTR(evt_block_broken), GSWF(2439), STRING("MOBJ_Lv1BigBigBlock"))
        CALL(evt_mobj_flag_onoff, 0, STRING("aa"), 64)
    END_IF
    IF_INT_LESS(GSW(0), 155)
        CALL(evt_mobj_lock, STRING("lock"), 19, 95, 210, 50, -80, PTR(evt_chk), PTR(evt_lock), GSWF(2437))
        SET_VALUE(LW(0), 1)
        CALL_CHILD(bero_case_switch_off)
    END_IF
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -30, 260, -30, 0, GSWF(5547), 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 50, 0, 0, STRING("tou_6_sita"), 125, GSWF(5626))
    CALL(evt_hitobj_attr_onoff, 0, 1, STRING("MOBJ_kururin1"), 4194304)
    CALL(evt_item_entry, STRING("item_01"), 279, -450, 250, -25, 16, GSWF(2521), 0)
    CALL(evt_item_entry, STRING("item_02"), 257, 175, 0, -125, 16, GSWF(2523), 0)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("fall"))
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_set_pos, 125, 0, -30)
        CALL(evt_cam_road_reset)
        CALL(evt_mario_cam_off)
        CALL(evt_mario_set_pos, 125, 100, -30)
        CALL(evt_party_set_pos, 0, 125, 100, -30)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        BEGIN_INLINE_EVENT
            CALL(evt_seq_wait, 2)
            CALL(evt_mario_cont_onoff, 1)
            CALL(evt_mario_wait_landon)
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_mario_cam_on)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_seq_wait, 2)
            CALL(evt_party_cont_onoff, 0, 1)
        END_INLINE_EVENT
    END_IF
    CALL(evt_map_playanim, STRING("anm_hae"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gor_hae"), 1)
    IF_INT_EQUAL(GSW(1420), 13)
        CALL_CHILD(iri_13_init)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidan"), 2)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        RETURN
    END_IF
    IF_INT_LESS(GSW(0), 148)
        SPAWN_EVENT(evt_chuchu)
    ELSE
        IF_INT_EQUAL(GSW(0), 163)
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envoff, 512)
        ELSE
            SWITCH(GSW(0))
                CASE_BETWEEN(127, 137)
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
                CALL(evt_snd_bgmoff, 26624)
                CASE_GREATER_OR_EQUAL(165)
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
                CALL(evt_snd_bgmoff, 26624)
                CASE_DEFAULT
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
            END_SWITCH
        END_IF
    END_IF
    RETURN
EVT_END()

// clang-format on

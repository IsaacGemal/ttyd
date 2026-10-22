#include "pik_01.h"
#include <dolphin/mtx.h>
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/envdrv.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "driver/npcdrv.h"
#include "eff/eff_starstone.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "kpa_act.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(starstone_set_ry);
static USER_FUNC(starstone_set_pos);
static USER_FUNC(starstone_entry);
static USER_FUNC(anim_tevcallback);
static USER_FUNC(yami_view);

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static HitDamageReturn dmgReturnPt[] = {
    {"mod_00", {0.0f, -1000.0f, 0.0f}},
    {"mod_01", {0.0f, -1000.0f, 0.0f}},
    {"mod_02", {0.0f, -1000.0f, 0.0f}},
    {"mod_03", {0.0f, -1000.0f, 0.0f}},
    {"mod_04", {0.0f, -1000.0f, 0.0f}},
    {"mod_05", {0.0f, -1000.0f, 0.0f}},
    {"mod_06", {0.0f, -1000.0f, 0.0f}},
    {"mod_07", {0.0f, -1000.0f, 0.0f}},
    {NULL, {0.0f, 0.0f, 0.0f}},
};

static EVT_BEGIN(n_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_a"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_a"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[] = {
    {"nw_bero", 0, 0, 7, 0x000186A0, 0, 0, -1, NULL, -1, NULL, "pik_04", "se_bero", 1, 1},
    {"n_bero", 1, 6, 1, 0x000186A0, 0, 0, -1, NULL, -1, NULL, "pik_02", "s_bero", 5, 5, n_door_close, n_door_open},
    {"dokan_1", 2, 0, 9, 0x000186A0, 0, 0, -1, NULL, 6, NULL, "tik_17", "dokan_1", 1, 1},
    {NULL},
};

static char* name_tbl[] = {
    "yajirusi_1", "yajirusi_2",
};

EVT_BEGIN(evt_powan)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_msg_print, 0, STRING("stg6_pik_06"), 0, 0)
    WAIT_MS(300)
    CALL(evt_npc_flag_onoff, 1, STRING("ポワン探偵"), 131088)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), -50, 10, -235)
    WAIT_FRAMES(1)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_pik_07_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_pik_07_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_pik_07_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_pik_07_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_pik_07_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_pik_07_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_pik_07_6"), 0, STRING("party"))
    END_SWITCH
    WAIT_MS(500)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_set_dir_to_target, STRING("ポワン探偵"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg6_pik_08"), 0, STRING("ポワン探偵"))
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_party_set_dir, 0, 270, 200)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_POWAN1"))
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -350, 120, 40, -55, 50, -236, 500, 11)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 4000, 11)
    CALL(evt_npc_status_onoff, 1, STRING("ポワン探偵"), 2)
    CALL(evt_npc_move_position, STRING("ポワン探偵"), 85, -100, 0, FLOAT(50.0f), 1)
    CALL(evt_cam3d_evt_set, -108, 131, 121, 188, 58, -167, 1000, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 228, -140, FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 190, -136, FLOAT(80.0f))
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("ポワン探偵"), 159, -171, 0, FLOAT(50.0f), 1)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_pik_09_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_pik_09_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_pik_09_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_pik_09_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_pik_09_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_pik_09_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_pik_09_6"), 0, STRING("party"))
    END_SWITCH
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg6_pik_10"), 0, STRING("ポワン探偵"))
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("stg6_pik_11"), 0, STRING("ポワン探偵"))
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("ポワン探偵"), 210, -193, 0, FLOAT(50.0f), 1)
    CALL(evt_mobj_lock_unlock, STRING("lock"))
    WAIT_MS(1000)
    CALL(evt_npc_move_position, STRING("ポワン探偵"), 180, -220, 0, FLOAT(50.0f), 1)
    CALL(evt_set_dir_to_target, STRING("ポワン探偵"), STRING("mario"))
    SET_VALUE(GSW(0), 335)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(init_powan)
    SWITCH(GSW(0))
        CASE_EQUAL(335)
        CALL(evt_npc_set_position, STRING("me"), 165, 20, -190)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 131088)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_powan)
    CALL(evt_msg_print, 0, STRING("stg6_pik_11"), 0, STRING("me"))
    RETURN
EVT_END()

USER_FUNC(chk_luigi_color) {
    s32* args = event->args;
    s32 color = marioGetColor();

    if (color == 1) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(miyageya_talk)
    IF_INT_EQUAL(GSW(1420), 27)
        CALL(chk_luigi_color, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            IF_INT_EQUAL(GSWF(5405), 0)
                CALL(evt_msg_print, 0, STRING("irai_27_001"), 0, 0)
                CALL(evt_mario_set_pose, STRING("M_N_5B"))
                WAIT_MS(300)
                CALL(evt_mario_set_pose, STRING("M_I_G"))
                WAIT_MS(800)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_msg_print, 0, STRING("irai_27_001_1"), 0, 0)
                SET_VALUE(GSWF(5405), 1)
            ELSE
                CALL(evt_msg_print, 0, STRING("irai_27_002"), 0, 0)
            END_IF
        ELSE
            CALL(evt_npc_flag_onoff, 1, STRING("土産屋"), 32)
            CALL(evt_cam3d_evt_set, -545, 90, 530, -290, 55, 285, 800, 11)
            BEGIN_INLINE_EVENT
                LOOP(0)
                    CALL(evt_mario_party_use_check, LW(0))
                    IF_INT_EQUAL(LW(0), 0)
                        BREAK_LOOP
                    END_IF
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(evt_party_stop, 0)
                CALL(evt_party_move_pos, 0, -225, 321, 800)
                CALL(evt_party_set_dir, 0, 270, 300)
            END_INLINE_EVENT
            CALL(evt_mario_mov_pos, -245, 300, 800)
            CALL(evt_mario_set_dir, 270, 300, 0)
            WAIT_MS(800)
            CALL(evt_set_dir_to_target, STRING("土産屋"), STRING("mario"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("irai_27_003"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(1000)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("irai_27_004"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_I_Z"))
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("irai_27_005"), 0, 0)
            CALL(evt_npc_get_position, STRING("土産屋"), LW(0), LW(1), LW(2))
            CALL(evt_get_target_dir, STRING("土産屋"), STRING("mario"), LW(3))
            SET_VALUE(LW(4), LW(0))
            SET_VALUE(LW(5), LW(2))
            CALL(evt_npc_add_dirdist, LW(4), LW(5), LW(3), 30)
            CALL(evt_npc_flag_onoff, 1, STRING("土産屋"), 32)
            CALL(evt_npc_move_position, STRING("土産屋"), LW(4), LW(5), 0, 50, 0)
            CALL(evt_pouch_get_haveitemcnt, LW(3))
            CALL(evt_pouch_check_item, 1, LW(4))
            IF_INT_EQUAL(LW(4), 0)
                SET_VALUE(LW(4), 10)
            ELSE
                SET_VALUE(LW(4), 20)
            END_IF
            IF_INT_LESS(LW(3), LW(4))
                CALL(evt_msg_print, 0, STRING("irai_27_006"), 0, 0)
                CALL(evt_item_entry, STRING("sc"), 214, 0, -1000, 0, 17, -1, 0)
                CALL(evt_item_get_item, STRING("sc"))
                CALL(evt_item_get_item_end_wait, STRING("sc"))
                SET_VALUE(GSWF(3278), 1)
                CALL(evt_npc_move_position, STRING("土産屋"), LW(0), LW(2), 0, 50, 0)
                CALL(evt_npc_flag_onoff, 0, STRING("土産屋"), 32)
            ELSE
                WAIT_MS(500)
                CALL(evt_npc_move_position, STRING("土産屋"), LW(0), LW(2), 0, 80, 0)
                CALL(evt_npc_flag_onoff, 0, STRING("土産屋"), 32)
                CALL(evt_msg_print, 0, STRING("irai_27_007_1"), 0, STRING("土産屋"))
                WAIT_MS(300)
                CALL(evt_item_entry, STRING("iri_27"), 214, -246, 15, 246, 16, GSWF(3278), 0)
                WAIT_MS(300)
                CALL(evt_msg_print, 0, STRING("irai_27_007_2"), 0, STRING("土産屋"))
            END_IF
            CALL(evt_npc_set_position, STRING("ルイージ"), -386, 0, 214)
            CALL(evt_npc_flag_onoff, 1, STRING("ルイージ"), 131120)
            CALL(evt_npc_move_position, STRING("ルイージ"), -330, 272, 0, 60, 0)
            CALL(evt_msg_print, 0, STRING("irai_27_008"), 0, STRING("ルイージ"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_mario_set_dir_npc, STRING("ルイージ"))
            CALL(evt_set_dir_to_target, STRING("土産屋"), STRING("ルイージ"))
            WAIT_MS(300)
            CALL(evt_eff_fukidashi, 1, STRING(""), STRING("土産屋"), 0, 0, 0, 0, 0, 0, 0, 60)
            WAIT_MS(1200)
            CALL(evt_msg_print, 0, STRING("irai_27_009"), 0, 0)
            CALL(evt_npc_get_position, STRING("ルイージ"), LW(0), LW(1), LW(2))
            CALL(evt_npc_jump_position_nohit, STRING("ルイージ"), LW(0), LW(1), LW(2), 300, 10)
            CALL(evt_msg_print, 0, STRING("irai_27_010"), 0, STRING("ルイージ"))
            CALL(evt_mario_set_pose, STRING("M_N_1"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("irai_27_011"), 0, 0)
            CALL(evt_msg_print, 0, STRING("irai_27_012"), 0, STRING("ルイージ"))
            CALL(evt_npc_move_position, STRING("ルイージ"), -386, 214, 0, 150, 0)
            CALL(evt_npc_flag_onoff, 0, STRING("ルイージ"), 131088)
            CALL(evt_npc_set_position, STRING("ルイージ"), 0, -1000, 0)
            CALL(evt_set_dir_to_target, STRING("土産屋"), STRING("mario"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("irai_27_013"), 0, 0)
            CALL(evt_mario_set_dir_npc, STRING("土産屋"))
            WAIT_MS(600)
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("irai_27_014"), 0, 0)
            CALL(evt_cam3d_evt_off, 800, 11)
            WAIT_MS(800)
            SET_VALUE(GSW(1420), 0)
            SET_VALUE(GSWF(5401), 1)
            CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
            CALL(evt_party_run, 0)
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_02"), 0, STRING("me"))
        CASE_BETWEEN(373, 376)
        IF_INT_EQUAL(LSWF(1), 0)
            CALL(evt_npc_set_force_regl_anim, STRING("me"), STRING("KNF_S_2"))
            CALL(evt_npc_set_anim, STRING("me"), STRING("KNF_S_1"))
            CALL(evt_npc_set_autotalkpose, STRING("me"), STRING("KNF_S_2"), STRING("KNF_T_2"))
            CALL(evt_msg_print, 0, STRING("kpa_stg7_004"), 0, STRING("me"))
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 512)
            SET_VALUE(LSWF(1), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("kpa_stg7_004_1"), 0, STRING("me"))
        END_IF
        CASE_DEFAULT
        CALL(chk_luigi_color, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            IF_INT_LESS_OR_EQUAL(GSW(0), 402)
                CALL(evt_msg_print, 0, STRING("stg7_pik_clear_03"), 0, STRING("me"))
            ELSE
                CALL(evt_msg_print, 0, STRING("stg7_pik_clear_03_0"), 0, STRING("me"))
            END_IF
        ELSE
            IF_INT_EQUAL(GSWF(5401), 0)
                CALL(evt_msg_print, 0, STRING("stg7_pik_clear_03_1"), 0, STRING("me"))
            ELSE
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 200, 10)
                CALL(evt_msg_print, 0, STRING("stg7_pik_clear_03_2"), 0, STRING("me"))
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(miyageya_init)
    CALL(evt_npc_set_position, STRING("me"), -270, -5, 260)
    SWITCH(GSW(0))
        CASE_BETWEEN(373, 376)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    END_SWITCH
    CALL(evt_npc_change_interrupt, STRING("me"), 6, PTR(miyageya_talk))
    RETURN
EVT_END()

s32 iri_29_item_tbl1[] = {
    0xCC, -1,
};

s32 iri_29_item_tbl2[] = {
    0xAA, -1,
};

s32 iri_29_item_tbl3[] = {
    0xA2, -1,
};

EVT_BEGIN(konarikin_talk)
    IF_INT_EQUAL(GSW(1420), 29)
        IF_INT_EQUAL(GSWF(5406), 0)
            IF_INT_EQUAL(GSWF(5405), 0)
                CALL(evt_msg_print, 0, STRING("irai_29_00"), 0, 0)
                CALL(evt_msg_select, 0, STRING("irai_29_00_s"))
                IF_INT_GREATER_OR_EQUAL(LW(0), 3)
                    CALL(evt_msg_continue)
                    RETURN
                END_IF
                SET_VALUE(GSW(1469), LW(0))
                CALL(evt_msg_print_add, 0, STRING("irai_29_01"))
                SET_VALUE(GSWF(5405), 1)
            ELSE
                CALL(evt_pouch_check_item, 100, LW(0))
                IF_INT_GREATER(LW(0), 0)
                    CALL(evt_msg_print, 0, STRING("irai_29_11"), 0, 0)
                    RETURN
                END_IF
                SWITCH(GSW(1469))
                    CASE_EQUAL(0)
                    CALL(evt_pouch_check_item, 204, LW(0))
                    IF_INT_GREATER(LW(0), 0)
                        CALL(evt_msg_print, 0, STRING("irai_29_05"), 0, 0)
                        CALL(evt_win_item_select, 0, PTR(iri_29_item_tbl1), LW(0), 0)
                        IF_INT_NOT_EQUAL(LW(0), -1)
                            CALL(evt_pouch_remove_item, 204, LW(0))
                            CALL(evt_msg_print, 0, STRING("irai_29_08"), 0, 0)
                            CALL(evt_item_entry, STRING("pre"), 100, 0, -1000, 0, 17, -1, 0)
                            CALL(evt_item_get_item, STRING("pre"))
                            CALL(evt_item_get_item_end_wait, STRING("pre"))
                        END_IF
                    ELSE
                        CALL(evt_msg_print, 0, STRING("irai_29_02"), 0, 0)
                    END_IF
                    CASE_EQUAL(1)
                    CALL(evt_pouch_check_item, 170, LW(0))
                    IF_INT_GREATER(LW(0), 0)
                        CALL(evt_msg_print, 0, STRING("irai_29_06"), 0, 0)
                        CALL(evt_win_item_select, 0, PTR(iri_29_item_tbl2), LW(0), 0)
                        IF_INT_NOT_EQUAL(LW(0), -1)
                            CALL(evt_pouch_remove_item, 170, LW(0))
                            CALL(evt_msg_print, 0, STRING("irai_29_09"), 0, 0)
                            CALL(evt_item_entry, STRING("pre"), 100, 0, -1000, 0, 17, -1, 0)
                            CALL(evt_item_get_item, STRING("pre"))
                            CALL(evt_item_get_item_end_wait, STRING("pre"))
                        END_IF
                    ELSE
                        CALL(evt_msg_print, 0, STRING("irai_29_03"), 0, 0)
                    END_IF
                    CASE_EQUAL(2)
                    CALL(evt_pouch_check_item, 162, LW(0))
                    IF_INT_GREATER(LW(0), 0)
                        CALL(evt_msg_print, 0, STRING("irai_29_07"), 0, 0)
                        CALL(evt_win_item_select, 0, PTR(iri_29_item_tbl3), LW(0), 0)
                        IF_INT_NOT_EQUAL(LW(0), -1)
                            CALL(evt_pouch_remove_item, 162, LW(0))
                            CALL(evt_msg_print, 0, STRING("irai_29_10"), 0, 0)
                            CALL(evt_item_entry, STRING("pre"), 100, 0, -1000, 0, 17, -1, 0)
                            CALL(evt_item_get_item, STRING("pre"))
                            CALL(evt_item_get_item_end_wait, STRING("pre"))
                        END_IF
                    ELSE
                        CALL(evt_msg_print, 0, STRING("irai_29_04"), 0, 0)
                    END_IF
                END_SWITCH
            END_IF
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_29_12"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("irai_29_13"), 0, 0)
            CALL(evt_sub_get_coin, 3)
            BEGIN_INLINE_EVENT
                WAIT_MS(300)
                CALL(evt_mario_set_dir_npc, STRING("コナリキン"))
            END_INLINE_EVENT
            BEGIN_INLINE_EVENT
                WAIT_MS(300)
                CALL(evt_party_set_dir_npc, 0, STRING("コナリキン"))
            END_INLINE_EVENT
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
            CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
            CALL(evt_npc_move_position, STRING("me"), -222, -225, 0, 120, 0)
            CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
            SET_VALUE(GSW(1420), 0)
            SET_VALUE(GSWF(5403), 1)
            CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
        END_IF
        RETURN
    END_IF
    CALL(evt_msg_print, 0, STRING("kpa_stg7_005"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("kpa_stg7_005_1"))
    CALL(evt_msg_continue)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(evt_msg_print, 0, STRING("kpa_stg7_006"), 0, STRING("mario"))
        CALL(evt_npc_set_autotalkpose, STRING("me"), STRING("C_1"), STRING("T_4"))
        CALL(evt_msg_print, 0, STRING("kpa_stg7_007"), 0, STRING("me"))
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
        CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
        CALL(evt_npc_set_anim, STRING("me"), STRING("R_2"))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KONARIKIN_ESCAPE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_move_position, STRING("me"), -200, 380, 0, FLOAT(150.0f), 0)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("kpa_stg7_008"), 0, STRING("mario"))
        CALL(evt_msg_print, 0, STRING("kpa_stg7_009"), 0, STRING("me"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("kpa_stg7_010"), 0, STRING("mario"))
        CALL(evt_msg_print, 0, STRING("kpa_stg7_011"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(konarikin_init)
    CALL_CHILD(npc_init_evt)
    IF_INT_EQUAL(GSW(1420), 29)
        CALL(evt_npc_set_position, STRING("me"), -62, -5, -367)
        RETURN
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        IF_INT_LESS(GSW(0), 376)
            CALL(evt_npc_set_position, STRING("me"), 145, 0, 380)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(narikin_papa_mama_talk)
    CALL(evt_cam3d_evt_set, -109, 163, 752, -109, 67, 211, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_011_3"), 0, STRING("ナリキンパパ"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_011_4"), 0, STRING("ナリキンママ"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_011_5"), 0, STRING("ナリキンパパ"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_011_6"), 0, STRING("ナリキンママ"))
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_011_7"), 0, STRING("mario"))
    RETURN
EVT_END()

EVT_BEGIN(narikin_papa_init)
    CALL_CHILD(npc_init_evt)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 16777216)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        IF_INT_LESS(GSW(0), 376)
            CALL(evt_npc_set_position, STRING("me"), -6, -5, 161)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(narikin_mama_init)
    CALL_CHILD(npc_init_evt)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 16777216)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        IF_INT_LESS(GSW(0), 376)
            CALL(evt_npc_set_position, STRING("me"), 18, -5, 185)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(madam_talk)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_011_1"), 0, STRING("me"))
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_011_2"), 0, STRING("mario"))
    RETURN
EVT_END()

EVT_BEGIN(madam_init)
    CALL_CHILD(npc_init_evt)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        IF_INT_LESS(GSW(0), 376)
            CALL(evt_npc_set_position, STRING("me"), -395, 0, -72)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(baba_talk)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_003_1"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(baba_init)
    CALL_CHILD(npc_init_evt)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(takun4_init)
    CALL(evt_npc_flag_onoff, 1, STRING("ターくん４"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(takun4_talk)
    SWITCH(GSW(0))
        CASE_LESS(347)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_04"), 0, STRING("me"))
        CASE_BETWEEN(373, 376)
        CALL(evt_msg_print, 0, STRING("kpa_stg7_011_7_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_05"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_05_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(takun5_init)
    CALL(evt_npc_flag_onoff, 1, STRING("ターくん５"), 1536)
    SWITCH(GSW(0))
        CASE_BETWEEN(373, 376)
        CALL(evt_npc_set_position, STRING("ターくん５"), 0, -1000, 0)
        CASE_DEFAULT
        CALL(evt_npc_flag_onoff, 1, STRING("ターくん５"), 131088)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(takun5_talk)
    SWITCH(GSW(0))
        CASE_LESS(347)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_06"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_07_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(takun6_init)
    CALL(evt_npc_flag_onoff, 1, STRING("ターくん６"), 1536)
    SWITCH(GSW(0))
        CASE_BETWEEN(373, 376)
        CALL(evt_npc_set_position, STRING("ターくん６"), 0, -1000, 0)
        CASE_DEFAULT
        CALL(evt_npc_flag_onoff, 1, STRING("ターくん６"), 131088)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(takun6_talk)
    SWITCH(GSW(0))
        CASE_LESS(347)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_08"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_09"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_09_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static NpcSetupInfo npc_data[] = {
    {"ポワン探偵", 0x40000600, 0, init_powan, NULL, talk_powan},
    {"土産屋", 0x40000600, 0, miyageya_init},
    {"ナリキンママ", 0, 0, narikin_mama_init, NULL, narikin_papa_mama_talk},
    {"ナリキンパパ", 0, 0, narikin_papa_init, NULL, narikin_papa_mama_talk},
    {"コナリキン", 0, 0, konarikin_init, NULL, konarikin_talk},
    {"マダム", 0, 0, madam_init, NULL, madam_talk},
    {"ババ", 0, 0, baba_init, NULL, baba_talk},
    {NULL},
};

EVT_BEGIN(teresa_fuwafuwa)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_sub_random, 14, LW(4))
    ADD(LW(4), 1)
    MULTIPLY(LW(4), 30)
    WAIT_MS(LW(4))
    LOOP(0)
        CALL(evt_sub_get_sincos, LW(3), LW(4), LW(5))
        MULTIPLY_FLOAT(LW(4), FLOAT(1.5f))
        ADD_FLOAT(LW(4), LW(1))
        CALL(evt_npc_set_position, STRING("me"), LW(0), LW(4), LW(2))
        ADD(LW(3), 3)
        MODULO(LW(3), 360)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(resaresa_talk)
    CALL(evt_msg_print, 0, STRING("stg7_pik_clear_03_2_01"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(resaresa_entry)
    CALL(evt_npc_entry, STRING("レサレサ"), STRING("c_resaresa"))
    CALL(evt_npc_set_tribe, STRING("レサレサ"), STRING("レサレサ"))
    CALL(evt_npc_set_position, STRING("レサレサ"), 30, 15, 120)
    CALL(evt_npc_set_ry, STRING("レサレサ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("レサレサ"), 4194816)
    CALL(evt_npc_change_interrupt, STRING("レサレサ"), 1, PTR(teresa_fuwafuwa))
    CALL(evt_npc_change_interrupt, STRING("レサレサ"), 6, PTR(resaresa_talk))
    RETURN
EVT_END()

EVT_BEGIN(sebaschan_talk)
    CALL(evt_msg_print, 0, STRING("stg7_pik_clear_03_2_02"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(sebaschan_entry)
    CALL(evt_npc_entry, STRING("セバスチャン"), STRING("c_sebaschan"))
    CALL(evt_npc_set_tribe, STRING("セバスチャン"), STRING("セバスチャン"))
    CALL(evt_npc_set_position, STRING("セバスチャン"), 75, 4, 90)
    CALL(evt_npc_set_ry, STRING("セバスチャン"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("セバスチャン"), 20972032)
    CALL(evt_npc_change_interrupt, STRING("セバスチャン"), 1, PTR(teresa_fuwafuwa))
    CALL(evt_npc_change_interrupt, STRING("セバスチャン"), 6, PTR(sebaschan_talk))
    RETURN
EVT_END()

EVT_BEGIN(koopa_door_evt)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mobj_get_position, STRING("lock"), LW(3), LW(4), LW(5))
    CALL(evt_sub_get_dir, LW(3), LW(5), LW(0), LW(2), LW(6))
    SUBTRACT(LW(6), 45)
    IF_INT_LESS(LW(6), 0)
        ADD(LW(6), 360)
    END_IF
    IF_INT_LESS(LW(6), 180)
        CALL(evt_mario_set_dir, 270, 300, 0)
    ELSE
        CALL(evt_mario_set_dir, 90, 300, 0)
    END_IF
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -132, 120, 148, 185, 90, -170, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_012"), 0, 0)
    CALL(kpa_chg_pose, STRING("KPA_I_2"))
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(1), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 150)
            WAIT_MS(150)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_ANGRY1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_ANGRY2"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(GF(1), 1)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_012_01"), 0, STRING("mario"))
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mobj_lock_unlock, STRING("lock"))
    WAIT_MS(600)
    SET_VALUE(GF(0), 1)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_012_02"), 0, 0)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_6"), STRING("KPA_T_6"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_013"), 0, STRING("mario"))
    SET_VALUE(LW(1), 1)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(10))
        IF_INT_EQUAL(LW(10), 0)
            SET_FLOAT_VALUE(LW(0), FLOAT(180.0f))
            CALL_CHILD(n_door_open)
            BREAK_LOOP
        ELSE
            CALL_CHILD(n_door_open)
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos, 230, -215, 600)
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_bero_mapchange, STRING("pik_02"), STRING(""))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(pik_01_koopa_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pos, -325, -5, -325)
    CALL(evt_npc_set_position, STRING("ババ"), -425, 25, -425)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ババ"), 32768)
    CALL(evt_npc_set_ry, STRING("ババ"), 90)
    CALL(evt_cam3d_evt_set, -273, 145, 196, -273, 69, -237, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_npc_flag_onoff, 0, STRING("ババ"), 32768)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_kamek_move_position, STRING("ババ"), -327, -327, 10, 0, FLOAT(60.0f), 0)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -227, -227, FLOAT(60.0f))
    WAIT_MS(500)
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(300)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_000"), 0, STRING("mario"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_001"), 0, STRING("ババ"))
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_npc_set_anim, STRING("ババ"), STRING("A_3A"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -150, 95, 514, 114, 130, -241, 2000, 11)
    WAIT_MS(2000)
    CALL(evt_npc_set_anim, STRING("ババ"), STRING("S_2"))
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, -273, 145, 196, -273, 69, -237, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_002"), 0, STRING("mario"))
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mario_set_dir, 90, 400, 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_off, 800, 11)
    SET_VALUE(GSW(0), 374)
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(1), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_kamek_move_position, STRING("ババ"), -255, -300, 10, 0, 80, 0)
    SET_VALUE(GF(1), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(delete_npc_regl_evt)
    CALL(evt_npc_get_reglid, STRING("ターくん４"), LW(0))
    CALL(evt_npc_change_interrupt, STRING("ターくん４"), 1, 0)
    DELETE_EVENT(LW(0))
    CALL(evt_npc_get_reglid, STRING("ターくん５"), LW(0))
    CALL(evt_npc_change_interrupt, STRING("ターくん５"), 1, 0)
    DELETE_EVENT(LW(0))
    CALL(evt_npc_get_reglid, STRING("ターくん６"), LW(0))
    CALL(evt_npc_change_interrupt, STRING("ターくん６"), 1, 0)
    DELETE_EVENT(LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(sekai_yami_shake)
    LOOP(0)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 500)
    END_LOOP
    RETURN
EVT_END()

const Vec vec3_pik_00002824 = {0.0f, 100.0f, 0.0f};
const Vec vec3_pik_00002830 = {0.0f, 1.0f, 0.0f};

static USER_FUNC(yami_view) {
    Vec camPos;
    Vec up;
    Vec target;
    Mtx mtx;
    f32 value = evtGetFloat(event, event->args[0]);

    target = (Vec){0.0f, 0.0f, 0.0f};
    target.z = value;
    up = vec3_pik_00002830;
    camPos = vec3_pik_00002824;
    camPos.z = 100.0f + value;
    C_MTXLookAt(mtx, &camPos, &up, &target);
    envSetYamiView(mtx);
    return EVT_RETURN_DONE;
}

static USER_FUNC(anim_tevcallback) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    NpcEntry* npc = npcNameToPtr_NoAssert((const char*)name);

    if (npc == NULL) {
        return EVT_RETURN_DONE;
    }
    animPoseSetMaterialFlagOn(npc->poseId, (flag & 0xF) << 24);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(sekai_yami_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), 40, -5, 100)
    CALL(evt_npc_set_ry, STRING("ポワン探偵"), 270)
    CALL(evt_map_set_tevcallback, 3, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 50331648)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback, STRING("ポワン探偵"), 3)
    CALL(anim_tevcallback, STRING("ターくん５"), 3)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(yami_view, -800)
    CALL(evt_cam3d_evt_set, 15, 134, 667, 15, 38, 126, 0, 11)
    SPAWN_EVENT(sekai_yami_shake)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_QUAKE1"), LW(0))
    WAIT_MS(800)
    CALL(evt_npc_reverse_ry, STRING("ポワン探偵"))
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ポワン探偵"))
    CALL(evt_npc_reverse_ry, STRING("ポワン探偵"))
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ポワン探偵"))
    CALL(evt_msg_print, 0, STRING("stg8_dark_07"), 0, STRING("ポワン探偵"))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_reverse_ry, STRING("ポワン探偵"))
            WAIT_FRAMES(1)
            CALL(evt_npc_wait_pera, STRING("ポワン探偵"))
            WAIT_MS(200)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(0))
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("bom_02"), STRING("sekai_yami2"))
    RETURN
EVT_END()

static USER_FUNC(starstone_entry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* effect = effStarStoneEntry(5, x, y, z, 1.0f);

    effSetName(effect, (const char*)name);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_pos) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* effect = effNameToPtr((const char*)name);

    ((effStarStoneData*)effect->userdata)->x = x;
    ((effStarStoneData*)effect->userdata)->y = y;
    ((effStarStoneData*)effect->userdata)->z = z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_ry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 ry = evtGetFloat(event, args[1]);
    EffectEntry* effect = effNameToPtr((const char*)name);
    effStarStoneData* data = effect->userdata;

    data->rotY = ry;
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(minnnanokoe_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), 90, -5, 10)
    CALL(evt_npc_set_ry, STRING("ポワン探偵"), 270)
    CALL(evt_npc_set_position, STRING("マダム"), 10, -5, 0)
    CALL(evt_npc_set_ry, STRING("マダム"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("マダム"), 32768)
    CALL(evt_npc_set_position, STRING("コナリキン"), 70, 0, -70)
    CALL(evt_npc_set_ry, STRING("コナリキン"), 270)
    CALL(evt_npc_set_position, STRING("ナリキンママ"), 30, 0, -105)
    CALL(evt_npc_set_ry, STRING("ナリキンママ"), 90)
    CALL(evt_npc_set_position, STRING("ナリキンパパ"), 100, 5, -75)
    CALL(evt_npc_set_ry, STRING("ナリキンパパ"), 270)
    CALL(evt_map_set_tevcallback, 3, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 50331648)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback, STRING("ポワン探偵"), 3)
    CALL(anim_tevcallback, STRING("マダム"), 3)
    CALL(anim_tevcallback, STRING("コナリキン"), 3)
    CALL(anim_tevcallback, STRING("ナリキンママ"), 3)
    CALL(anim_tevcallback, STRING("ナリキンパパ"), 3)
    CALL(anim_tevcallback, STRING("ターくん４"), 3)
    CALL(anim_tevcallback, STRING("ターくん５"), 3)
    CALL(anim_tevcallback, STRING("ターくん６"), 3)
    CALL(yami_view, -800)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(evt_cam3d_evt_set, 50, 120, 750, 50, 55, 0, 0, 11)
    CALL(starstone_entry, STRING("starstone"), 50, 50, 0)
    SET_VALUE(GW(0), 50)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(starstone_set_pos, STRING("starstone"), 50, GW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(starstone_set_ry, STRING("starstone"), LW(0))
            SUBTRACT_FLOAT(LW(0), FLOAT(3.0f))
            IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 5000, 5500, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
            CALL(evt_sub_intpl_msec_init, 11, 5500, 5000, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(2)
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 0, STRING("マダム"), 32768)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_53"), 0, STRING("マダム"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_54"), 0, STRING("ポワン探偵"))
    CALL(evt_cam3d_evt_set, 50, 78, 349, 50, 47, 0, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_55"), 0, STRING("ナリキンママ"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_56"), 0, STRING("ナリキンパパ"))
    CALL(evt_npc_set_ry, STRING("コナリキン"), 90)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_57"), 0, STRING("コナリキン"))
    CALL(evt_npc_set_ry, STRING("コナリキン"), 270)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_58"), 0, STRING("コナリキン"))
    CALL(evt_cam3d_evt_set, 50, 120, 750, 50, 55, 0, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_59"), 0, STRING("マダム"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_60"), 0, STRING("ナリキンパパ"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_61"), 0, STRING("ナリキンママ"))
    CALL(evt_npc_reverse_ry, STRING("ポワン探偵"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("ポワン探偵"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_62"), 0, STRING("ポワン探偵"))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_reverse_ry, STRING("ポワン探偵"))
            WAIT_MS(500)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("bom_02"), STRING("minnnanokoe"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_switch_init)
    IF_INT_EQUAL(GSWF(1348), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_dakan"), 1)
        CALL(evt_hitobj_onoff, STRING("A_dokan"), 1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_27_npc_entry)
    CALL(evt_npc_entry, STRING("ルイージ"), STRING("c_luigi"))
    CALL(evt_npc_set_tribe, STRING("ルイージ"), STRING("ルイージ"))
    CALL(evt_npc_flag_onoff, 1, STRING("ルイージ"), 1536)
    CALL(evt_npc_status_onoff, 1, STRING("ルイージ"), 2)
    CALL(evt_npc_set_position, STRING("ルイージ"), 0, -1000, 0)
    RETURN
EVT_END()

USER_FUNC(set_r) {
    MapObjectEntry* obj = mobjNameToPtr("lock");

    obj->rotation.y = -45.0f;
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_lock_hit_scale) {
    MapObjectEntry* obj = mobjNameToPtr("lock");
    f32 scale = 2.0f;

    obj->scale2.x *= scale;
    obj->scale2.y *= scale;
    mobjCalcMtx2(obj);
    obj->flags &= ~4;
    return EVT_RETURN_DONE;
}

void test_tev(void* param) {
    envAddTev(4, param);
}

void test_tev2(void* param) {
    envAddTev(7, param);
}

EVT_BEGIN(pik_01_init_evt)
    SET_VALUE(LF(0), 0)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        IF_INT_LESS(GSW(0), 375)
            CALL(evt_mario_set_mode, 2)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    CALL_CHILD(evt_switch_init)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_powerupblk, STRING("pblk01"), 400, 95, 125, 0, GSWF(5563), 0)
    IF_INT_EQUAL(LF(0), 1)
        SET_VALUE(LW(0), 0)
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("nw_bero"), 8388612)
    END_IF
    SET_VALUE(LF(1), 0)
    IF_INT_EQUAL(LF(0), 0)
        IF_INT_EQUAL(GSW(0), 400)
            CALL(evt_bero_get_entername, LW(0))
            IF_STR_EQUAL(LW(0), STRING("sekai_yami2"))
                CALL(evt_snd_bgmoff, 512)
                CALL(evt_snd_envoff, 512)
                SET_VALUE(LF(1), 1)
            END_IF
            IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_VOICE1"))
                CALL(evt_snd_envoff, 512)
                SET_VALUE(LF(1), 1)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(1), 0)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_PIK1_1"))
        END_IF
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_KUPPA1"))
        CALL(evt_snd_bgm_scope, 0, 1)
        CALL(evt_snd_bgmon, 512, STRING("ENV_STG6_PIK1_1"))
    END_IF
    CALL(evt_map_playanim, STRING("anm_sibuki"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_hamon"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_opuku"), 1, 0)
    SET_INT_READ_ADDR(PTR(name_tbl))
    LOOP(2)
        READ_INT(LW(9))
        SPAWN_EVENT(evt_pik_yajirushi)
    END_LOOP
    CALL(evt_hit_damage_return_set, PTR(dmgReturnPt))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_map_set_tevcallback, 1, PTR(test_tev))
    CALL(evt_map_set_tevcallback, 2, PTR(test_tev2))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("opuku"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("opuku"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_1"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_1"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_2"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_2"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_3"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_3"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_4"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_4"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_5"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_5"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_6"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_6"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HEIPO_7"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("HEIPO_7"), 256)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(0)
            CALL(evt_mapobj_rotate, 1, STRING("opuku"), 0, LW(0), 0)
            ADD(LW(0), 1)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_setup, PTR(npc_data))
    IF_INT_EQUAL(LF(1), 0)
        IF_INT_EQUAL(GSW(1420), 27)
            CALL_CHILD(iri_27_npc_entry)
        END_IF
        IF_INT_EQUAL(GSWF(5401), 1)
            CALL(evt_item_entry, STRING("iri_27"), 214, -246, 15, 246, 16, GSWF(3278), 0)
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 403)
        CALL_CHILD(resaresa_entry)
        CALL_CHILD(sebaschan_entry)
    END_IF
    CALL_CHILD(takun4_init)
    CALL(evt_npc_change_interrupt, STRING("ターくん４"), 6, PTR(takun4_talk))
    CALL_CHILD(takun5_init)
    CALL(evt_npc_change_interrupt, STRING("ターくん５"), 6, PTR(takun5_talk))
    CALL_CHILD(takun6_init)
    CALL(evt_npc_change_interrupt, STRING("ターくん６"), 6, PTR(takun6_talk))
    IF_INT_LESS(GSW(0), 335)
        CALL(evt_mobj_entry, STRING("lock"), STRING("MOBJ_Lock"))
        CALL(evt_mobj_set_position, STRING("lock"), 226, 30, -216)
        CALL(evt_mobj_set_anim, STRING("lock"), STRING("Z_1"))
        CALL(set_r)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_run_case_evt, 9, 1, STRING("a_harigami"), 0, PTR(evt_powan), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 0, 1, STRING("kami"), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mobj_entry, STRING("lock"), STRING("MOBJ_Lock"))
        CALL(evt_mobj_flag_onoff, 1, STRING("lock"), 32)
        CALL(evt_mobj_set_position, STRING("lock"), 226, 30, -216)
        CALL(evt_mobj_set_anim, STRING("lock"), STRING("Z_1"))
        CALL(set_r)
        CALL(kpa_lock_hit_scale)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("polySurface743_q00096"), 1)
        CALL(evt_run_case_evt, 9, 1, STRING("MOBJ_lock"), 0, PTR(koopa_door_evt), 0)
    END_IF
    IF_INT_EQUAL(GSW(0), 373)
        CALL(evt_mario_kill_party, 0)
        SPAWN_EVENT(pik_01_koopa_evt)
    END_IF
    IF_INT_EQUAL(GSW(0), 374)
        CALL(evt_mario_set_pos, -227, 0, -227)
        CALL(evt_npc_set_position, STRING("ババ"), -255, 25, -300)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_npc_set_ry, STRING("ババ"), 90)
    END_IF
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami2"))
            CALL_CHILD(delete_npc_regl_evt)
            SPAWN_EVENT(sekai_yami_evt)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            CALL_CHILD(delete_npc_regl_evt)
            SPAWN_EVENT(minnnanokoe_evt)
        END_IF
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

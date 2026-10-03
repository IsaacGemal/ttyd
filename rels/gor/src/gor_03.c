#include "gor_03.h"
#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/casedrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "driver/msgdrv.h"
#include "driver/shadowdrv.h"
#include "eff/eff_bomb.h"
#include "eff/eff_mario_balloon.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <stdio.h>
#include <string.h>
#include "evt/evt_badgeshop.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_lottery.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_shop.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_urouro.h"
#include "evt/evt_win.h"
#include "evt/evt_window.h"
#include "evt/evt_yuugijou.h"
#include "evt_goryuugijo.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "pmario_sound.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_yuuranking_alloc_10_text_A47C[];
extern s32 evt_yuuranking_free_10_text_A3F8[];
extern s32 luigi_gor_first_talk[];
extern s32 yuuranking_wait_button_10_text_A37C[];
static USER_FUNC(enzan_lw_load);
static USER_FUNC(enzan_lw_save);
static USER_FUNC(viewport);
static USER_FUNC(mario_keyoff_wait);
static USER_FUNC(fall_rtn);
static void monte_disp(CameraId cameraId, void* param);
static s32 get_monte_check(Vec* a, Vec* b);
static USER_FUNC(setmertn);
static u32 xlw[16];
static u32 dataadr[8];
static EffectEntry* eff[1];
static u32 monte_p[1];
NpcEntry* npc[1];
u32 exchange_msgp[1];
u32 cherryEnt[46];
s32 majorin_nusumigiki[331];
s32 kuzi_bomb_talk[16];

// clang-format off
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

EVT_BEGIN(debug_event)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 64)
        CALL(evt_sub_system_flag_onoff, 0, 64)
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(gor_03_down_evt)
    SET_VALUE(GSWF(1223), 1)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[6] = {
    {"n_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, debug_event, -1, NULL, "gor_04", "s_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, debug_event, -1, NULL, "gor_01", "w_bero", 1, 1},
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, debug_event, 6, NULL, "tik_07", "dokan_4", 1, 1},
    {"s_bero", 3840, 0, 0x4E20, 100000, 0, 0, -1, debug_event, -1, gor_03_down_evt, "tik_07", NULL, 1, 1},
    {"A_dokan_02", 8194, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, "yuu_00", "", 1, 1},
    {NULL},
};

// TODO: type
u32 yuu_00_data[15] = {
    (u32)"A_dokan_02", 0x20020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000006, 0x00000000, (u32)"yuu_00", (u32)"",
    0x00010001, 0x00000000, 0x00000000,
};

// TODO: type
u32 yuu_01_data[15] = {
    (u32)"A_dokan_02", 0x20020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000006, 0x00000000, (u32)"yuu_01", (u32)"",
    0x00010001, 0x00000000, 0x00000000,
};

// TODO: type
u32 yuu_02_data[15] = {
    (u32)"A_dokan_02", 0x20020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000006, 0x00000000, (u32)"yuu_02", (u32)"",
    0x00010001, 0x00000000, 0x00000000,
};

// TODO: type
u32 yuu_03_data[15] = {
    (u32)"A_dokan_02", 0x20020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000006, 0x00000000, (u32)"yuu_03", (u32)"",
    0x00010001, 0x00000000, 0x00000000,
};

EVT_BEGIN(maffiaboss_ticket_put)
    CALL(evt_item_entry, STRING("ticket"), 52, 0, -1000, 0, 16, -1, 0)
    CALL(evt_item_flag_on, STRING("ticket"), 32768)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmon, 513, STRING("BGM_FF_GET_KEYITEM1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmon, 513, STRING("BGM_STG0_CSN1"))
    END_INLINE_EVENT
    CALL(evt_item_get_item, STRING("ticket"))
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_ticket_check)
    LOOP(0)
        SET_VALUE(LF(0), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), -296)
            IF_INT_LESS_OR_EQUAL(LW(0), -104)
                IF_INT_GREATER_OR_EQUAL(LW(1), 155)
                    IF_INT_LESS_OR_EQUAL(LW(1), 180)
                        IF_INT_GREATER_OR_EQUAL(LW(2), -110)
                            ADD(LF(0), 1)
                        END_IF
                    END_IF
                END_IF
            END_IF
        END_IF
        CALL(evt_mario_get_name_hitobj_ride, LW(0))
        IF_STR_EQUAL(LW(0), STRING("A_i02_tke_ue"))
            SET_VALUE(LW(5), 2)
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(LF(0), 1)
            SET_VALUE(LW(5), 0)
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(GF(15), 1)
            SET_VALUE(LW(5), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_snd_bgmon, 513, STRING("BGM_STG0_CSN1"))
    CALL(evt_msg_print, 0, STRING("mac_2_116"), 0, STRING("マフィアボス"))
    CALL(evt_npc_set_ry_lr, STRING("マフィアボス"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("マフィアボス"), LW(10), LW(11), LW(12))
        IF_INT_LESS_OR_EQUAL(LW(2), LW(12))
            CALL(evt_mario_set_dir, 90, 200, 0)
        ELSE
            CALL(evt_mario_set_dir, 270, 200, 0)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("マフィアボス"), LW(10), LW(11), LW(12))
        IF_INT_LESS_OR_EQUAL(LW(2), LW(12))
            CALL(evt_party_set_dir, 0, 90, 200)
        ELSE
            CALL(evt_party_set_dir, 0, 270, 200)
        END_IF
    END_INLINE_EVENT
    CALL(evt_set_dir_to_target, STRING("マフィアボス"), STRING("mario"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_2_117"), 0, STRING("マフィアボス"))
    IF_INT_EQUAL(LW(5), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("マフィアボス"), 1073741824)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("マフィアボス"), LW(10), LW(11), LW(12))
        CALL(evt_npc_move_position, STRING("マフィアボス"), LW(0), -68, 0, FLOAT(60.0f), 0)
        WAIT_MS(500)
        CALL_CHILD(maffiaboss_ticket_put)
        CALL(evt_msg_print, 0, STRING("mac_2_118"), 0, STRING("マフィアボス"))
        CALL(evt_npc_move_position, STRING("マフィアボス"), LW(10), LW(12), 0, FLOAT(60.0f), 0)
        CALL(evt_npc_flag_onoff, 0, STRING("マフィアボス"), 1073741824)
    ELSE
        CALL_CHILD(maffiaboss_ticket_put)
        CALL(evt_msg_print, 0, STRING("mac_2_118"), 0, STRING("マフィアボス"))
    END_IF
    CALL(evt_npc_move_position, STRING("マフィアボス"), -198, -80, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_119"), 0, STRING("マフィアボス"))
    SET_VALUE(GSW(0), 124)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GF(14), 0)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_init)
    SWITCH(GSW(0))
        CASE_EQUAL(123)
        CALL(evt_npc_set_position, STRING("me"), -198, 155, -80)
        IF_INT_EQUAL(GSWF(1236), 1)
            SET_VALUE(GF(15), 0)
            SPAWN_EVENT(maffiaboss_ticket_check)
        END_IF
        CASE_BETWEEN(268, 278)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 1090781184)
        CALL(evt_npc_set_position, STRING("me"), -200, 170, -152)
        CALL(evt_npc_set_rotate_offset, STRING("me"), 0, 20, 0)
        CALL(evt_npc_set_rotate, STRING("me"), 90, 0, 0)
        CALL(evt_npc_set_autotalkpose, STRING("me"), STRING("S_2"), STRING("T_2"))
        CALL(evt_npc_change_interrupt, STRING("me"), 6, 0)
        CASE_GREATER(278)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), -198, 155, -80)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_maffiaboss)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), -296)
            IF_INT_LESS_OR_EQUAL(LW(0), -104)
                IF_INT_GREATER_OR_EQUAL(LW(1), 155)
                    IF_INT_GREATER_OR_EQUAL(LW(2), -180)
                        IF_INT_LESS_OR_EQUAL(LW(2), -53)
                            BREAK_LOOP
                        END_IF
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    SET_VALUE(LSWF(3), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア１"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア２"), 1073741856)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -210, -165, FLOAT(120.0f))
        CALL(evt_mario_set_dir, 90, 200, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos2, 0, -180, -180, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_2_045"), 0, STRING("マフィアボス"))
    WAIT_WHILE(GF(0))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -485, 250, -95, -188, 205, -95, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_046"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -683, 289, -160, -189, 215, -160, 0, 11)
    CALL(evt_npc_get_position, STRING("マフィア１"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), LW(0))
    ADD(LW(10), -20)
    CALL(evt_npc_move_position, STRING("マフィア１"), LW(10), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_047"), 0, STRING("マフィア１"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("マフィア１"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("マフィア２"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), LW(0))
    ADD(LW(10), -20)
    CALL(evt_npc_move_position, STRING("マフィア２"), LW(10), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_048"), 0, STRING("マフィア２"))
    CALL(evt_npc_move_position, STRING("マフィア２"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_049"), 0, STRING("マフィアボス"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_050"), 0, STRING("マフィアボス"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_2_051"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_2_052"), 0, STRING("マフィア２"))
    CALL(evt_party_move_pos2, 0, -170, -175, FLOAT(120.0f))
    CALL(evt_msg_print_party, STRING("mac_2_053"))
    CALL(evt_cam3d_evt_set, -509, 252, -125, -188, 202, -125, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_2_056"), 0, STRING("マフィアボス"))
    WAIT_MS(250)
    CALL(evt_msg_print_party, STRING("mac_2_057"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_2_060"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -509, 252, -230, -188, 202, -230, 500, 11)
    WAIT_MS(800)
    CALL(evt_npc_get_position, STRING("マフィア１"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), LW(0))
    ADD(LW(10), -20)
    CALL(evt_npc_move_position, STRING("マフィア１"), LW(10), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_061"), 0, STRING("マフィア１"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("マフィア１"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("マフィア２"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), LW(0))
    ADD(LW(10), -20)
    CALL(evt_npc_move_position, STRING("マフィア２"), LW(10), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_062"), 0, STRING("マフィア２"))
    CALL(evt_npc_move_position, STRING("マフィア２"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_063"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -683, 289, -160, -189, 215, -160, 0, 11)
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_2_064"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_2_065"), 0, STRING("マフィア２"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_066"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -509, 252, -125, -188, 202, -125, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_2_067"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -385, 230, -83, -187, 200, -83, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 1, 500)
    END_INLINE_EVENT
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_2_068"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア１"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア２"), 1073741856)
    SET_VALUE(GSW(0), 122)
    SET_VALUE(LSWF(3), 0)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_airship_ticket)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), -296)
            IF_INT_LESS_OR_EQUAL(LW(0), -104)
                IF_INT_GREATER_OR_EQUAL(LW(1), 155)
                    IF_INT_GREATER_OR_EQUAL(LW(2), -180)
                        IF_INT_LESS_OR_EQUAL(LW(2), -53)
                            BREAK_LOOP
                        END_IF
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    SET_VALUE(LSWF(3), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィアボス"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア１"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア２"), 1073741856)
    CALL(evt_msg_print, 0, STRING("mac_2_088"), 0, STRING("マフィアボス"))
    CALL(evt_msg_select, 0, STRING("mac_2_088_yesno"))
    CALL(evt_msg_continue)
    CALL(evt_cam3d_evt_set, -509, 252, -100, -188, 202, -100, 500, 11)
    WAIT_MS(500)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("mac_2_089"), 0, STRING("マフィアボス"))
        CALL(evt_cam3d_evt_set, -683, 289, -160, -189, 215, -160, 0, 11)
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("mac_2_090"), 0, STRING("マフィアボス"))
    ELSE
        CALL(evt_msg_print, 0, STRING("mac_2_091"), 0, STRING("マフィアボス"))
    END_IF
    WAIT_MS(500)
    CALL(evt_npc_set_position, STRING("モニー"), -260, 155, -410)
    CALL(evt_npc_set_ry, STRING("モニー"), 180)
    CALL(evt_npc_set_position, STRING("ピートン"), -275, 155, -440)
    CALL(evt_npc_set_ry, STRING("ピートン"), 180)
    CALL(evt_npc_set_ry, STRING("マフィアボス"), 0)
    CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
    CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("mac_2_092"), 0, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -683, 289, -385, -189, 215, -385, 500, 11)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, -485, 250, -90, -188, 205, -90, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_093"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -683, 289, -385, -189, 215, -385, 0, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_pos, -118, 155, -122)
        CALL(evt_mario_set_dir, 270, 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_pos, 0, -118, 155, -157)
        CALL(evt_party_set_dir, 0, 270, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_2_094"), 0, STRING("ピートン"))
    CALL(evt_cam3d_evt_set, -683, 289, -160, -189, 215, -160, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_2_095"), 0, STRING("マフィアボス"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("モニー"), -220, -245, 0, FLOAT(100.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("ピートン"), -190, -250, 0, FLOAT(100.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_096"), 0, STRING("モニー"))
    CALL(evt_cam3d_evt_set, -663, 284, -157, -189, 213, -157, 200, 11)
    CALL(evt_npc_move_position, STRING("ピートン"), -190, -230, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_097"), 0, STRING("ピートン"))
    CALL(evt_cam3d_evt_set, -643, 278, -154, -189, 210, -154, 200, 11)
    CALL(evt_npc_move_position, STRING("モニー"), -220, -225, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_098"), 0, STRING("モニー"))
    CALL(evt_cam3d_evt_set, -623, 273, -151, -189, 207, -151, 200, 11)
    CALL(evt_npc_move_position, STRING("ピートン"), -190, -210, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_099"), 0, STRING("ピートン"))
    CALL(evt_cam3d_evt_set, -603, 268, -148, -189, 204, -148, 200, 11)
    CALL(evt_npc_move_position, STRING("モニー"), -220, -205, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_100"), 0, STRING("モニー"))
    CALL(evt_cam3d_evt_set, -583, 262, -145, -188, 202, -145, 200, 11)
    CALL(evt_npc_move_position, STRING("ピートン"), -190, -190, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_101"), 0, STRING("ピートン"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_102"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -534, 247, -150, -188, 194, -150, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("マフィアボス"), LW(0), LW(1), LW(2))
    ADD(LW(0), -20)
    CALL(evt_npc_move_position, STRING("マフィアボス"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_103"), 0, STRING("マフィアボス"))
    CALL(evt_npc_get_position, STRING("マフィアボス"), LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    CALL(evt_npc_move_position, STRING("マフィアボス"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_104"), 0, STRING("マフィアボス"))
    CALL(evt_npc_set_ry, STRING("マフィアボス"), 270)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_105"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_2_106"), 0, STRING("ピートン"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_2_107"), 0, STRING("マフィアボス"))
    CALL(evt_npc_move_position, STRING("モニー"), -220, -190, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_108"), 0, STRING("モニー"))
    CALL(evt_npc_move_position, STRING("ピートン"), -190, -175, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_109"), 0, STRING("ピートン"))
    CALL(evt_cam3d_evt_set, -450, 237, -180, -188, 200, -180, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_110"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_2_111"), 0, STRING("ピートン"))
    CALL(evt_cam3d_evt_set, -450, 242, -85, -188, 205, -85, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("mac_2_112"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -683, 289, -160, -189, 215, -160, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_npc_get_position, STRING("マフィア１"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), LW(0))
    ADD(LW(10), -20)
    CALL(evt_npc_move_position, STRING("マフィア１"), LW(10), LW(2), 0, FLOAT(60.0f), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("モニー"), 90)
        CALL(evt_npc_set_ry, STRING("ピートン"), 90)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_2_113"), 0, STRING("マフィア１"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("マフィア１"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("マフィア２"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), LW(0))
    ADD(LW(10), -20)
    CALL(evt_npc_move_position, STRING("マフィア２"), LW(10), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_2_114"), 0, STRING("マフィア２"))
    CALL(evt_npc_move_position, STRING("マフィア２"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    WAIT_MS(833)
    CALL(evt_msg_print, 0, STRING("mac_2_115"), 0, STRING("モニー"))
    CALL(evt_npc_set_ry, STRING("モニー"), 90)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ピートン"), -190, -400, 0, FLOAT(180.0f), 0)
        CALL(evt_npc_set_position, STRING("ピートン"), 0, -1000, 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("モニー"), -220, -400, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("モニー"), 0, -1000, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 0, STRING("マフィアボス"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("モニー"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア１"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア２"), 1073741856)
    SET_VALUE(GSWF(1236), 1)
    SET_VALUE(GF(15), 0)
    SPAWN_EVENT(maffiaboss_ticket_check)
    SET_VALUE(LSWF(3), 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_teshita_irai)
    CALL(evt_msg_print, 0, STRING("mac_5_012"), 0, STRING("マフィアボス"))
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -435, 248, -235, -188, 212, -235, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_013"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_014"), 0, STRING("マフィア２"))
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_cam3d_evt_off, 0, 11)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_015"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_016"), 0, STRING("マフィア２"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_6"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_5_017"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_018"), 0, STRING("マフィア２"))
    WAIT_MS(250)
    CALL(evt_msg_print, 0, STRING("mac_5_019"), 0, STRING("マフィアボス"))
    CALL(evt_npc_set_ry, STRING("マフィア１"), 0)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_020"), 0, STRING("マフィア１"))
    CALL(evt_npc_set_ry, STRING("マフィア２"), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_021"), 0, STRING("マフィア２"))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_5_022"))
    CALL(evt_npc_set_ry, STRING("マフィア１"), 180)
    CALL(evt_npc_set_ry, STRING("マフィア２"), 180)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_023"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_024"), 0, STRING("マフィア２"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_025"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_026"), 0, STRING("マフィア２"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_027"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_028"), 0, STRING("マフィア２"))
    CALL(evt_msg_select, 0, STRING("mac_5_028_yesno"))
    CALL(evt_msg_continue)
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print, 0, STRING("mac_5_029"), 0, STRING("マフィア１"))
        CALL(evt_msg_print, 0, STRING("mac_5_030"), 0, STRING("マフィア２"))
        CALL(evt_msg_select, 0, STRING("mac_5_030_yesno"))
        CALL(evt_msg_continue)
    END_IF
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_031"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_032"), 0, STRING("マフィア２"))
    SET_VALUE(GSW(0), 273)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_train_ticket_init)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(2), 30)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_040"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_5_041"), 0, STRING("ピートン"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_train_ticket)
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("マフィアボス"))
    CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("マフィアボス"))
    CALL(evt_npc_flag_onoff, 1, STRING("マフィアボス"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア１"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア２"), 1073741856)
    CALL(evt_msg_print, 0, STRING("mac_5_046"), 0, STRING("マフィアボス"))
    CALL(evt_npc_set_anim, STRING("マフィアボス"), STRING("A_1"))
    WAIT_MS(500)
    CALL(evt_npc_set_autotalkpose, STRING("マフィアボス"), STRING("S_1"), STRING("T_1"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_047"), 0, STRING("モニー"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_5_048"), 0, STRING("ピートン"))
    IF_INT_EQUAL(LW(15), 0)
        DEBUG_PUT_MSG(STRING("ベッドの前から"))
        BEGIN_INLINE_EVENT
            CALL(evt_mario_mov_pos2, -245, -185, FLOAT(60.0f))
            CALL(evt_mario_set_dir, 90, 200, 0)
        END_INLINE_EVENT
    ELSE
        DEBUG_PUT_MSG(STRING("ベッドの横から"))
        BEGIN_INLINE_EVENT
            CALL(evt_mario_mov_pos2, -245, -197, FLOAT(60.0f))
            WAIT_FRAMES(1)
            CALL(evt_mario_mov_pos2, -245, -185, FLOAT(60.0f))
            CALL(evt_mario_set_dir, 90, 200, 0)
        END_INLINE_EVENT
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -230, -200, FLOAT(60.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -584, 265, -145, -189, 200, -145, 500, 11)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_CSN1"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("マフィアボス"), LW(0), LW(1), LW(2))
    ADD(LW(0), 25)
    CALL(evt_msg_print, 0, STRING("mac_5_049"), 0, STRING("マフィアボス"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_050"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_5_051"), 0, STRING("マフィアボス"))
    CALL(evt_npc_set_rotate, STRING("マフィアボス"), 0, 0, 0)
    CALL(evt_npc_jump_position_nohit, STRING("マフィアボス"), -250, 155, -120, 400, 20)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィアボス"), 262144)
    CALL(evt_npc_set_ry, STRING("マフィアボス"), 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -485, 244, -125, -189, 195, -125, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_052"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -683, 282, -165, -189, 208, -165, 0, 11)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
        ADD(LW(1), 60)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_5_053"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ピートン"), LW(0), LW(1), LW(2))
        ADD(LW(1), 80)
        ADD(LW(2), 10)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_5_054"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("マフィア２"), LW(0), LW(1), LW(2))
        ADD(LW(1), 60)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_5_056"), 0, 0)
    END_INLINE_EVENT
    WAIT_FRAMES(1)
    CALL(evt_npc_get_position, STRING("マフィア１"), LW(0), LW(1), LW(2))
    ADD(LW(1), 80)
    ADD(LW(2), 10)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("mac_5_055"), 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_057"), 0, STRING("マフィアボス"))
    CALL(evt_npc_get_position, STRING("ピートン"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), LW(0), LW(1), LW(2), 200, 30)
    CALL(evt_msg_print, 0, STRING("mac_5_058"), 0, STRING("ピートン"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_059"), 0, STRING("マフィアボス"))
    WAIT_MS(250)
    CALL(evt_msg_print, 0, STRING("mac_5_060"), 0, STRING("ピートン"))
    CALL(evt_cam3d_evt_set, -510, 243, -155, -189, 195, -155, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_061"), 0, STRING("マフィアボス"))
    CALL(evt_mario_set_pose, STRING("M_N_B"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_5_062"), 0, STRING("マフィア１"))
    CALL(evt_cam3d_evt_set, -685, 280, -170, -191, 206, -170, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_063"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_064"), 0, STRING("マフィアボス"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_065"), 0, STRING("マフィア２"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -510, 243, -155, -189, 195, -155, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_066"), 0, STRING("マフィアボス"))
    CALL(evt_cam3d_evt_set, -685, 280, -170, -191, 206, -170, 0, 11)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_mov_pos2, LW(0), -165, FLOAT(120.0f))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_npc_get_position, STRING("マフィア１"), LW(0), LW(1), LW(2))
        ADD(LW(1), 60)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_5_067"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_npc_get_position, STRING("マフィア２"), LW(0), LW(1), LW(2))
        ADD(LW(1), 80)
        ADD(LW(2), 10)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_5_068"), 0, 0)
    END_INLINE_EVENT
    CALL(evt_item_entry, STRING("ticket"), 53, 0, -1000, 0, 16, -1, 0)
    CALL(evt_item_flag_on, STRING("ticket"), 32768)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 513, STRING("BGM_FF_GET_KEYITEM1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_item_get_item, STRING("ticket"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_069"), 0, STRING("マフィアボス"))
    CALL(evt_npc_set_ry, STRING("マフィアボス"), 180)
    WAIT_MS(800)
    CALL(evt_npc_set_ry, STRING("マフィアボス"), 0)
    WAIT_MS(1300)
    CALL(evt_msg_print, 0, STRING("mac_5_070"), 0, STRING("マフィアボス"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("モニー"), LW(0), LW(1), LW(2), 300, 30)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("ピートン"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), LW(0), LW(1), LW(2), 300, 30)
    CALL(evt_msg_print, 0, STRING("mac_5_071"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_5_072"), 0, STRING("ピートン"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_5_073"), 0, STRING("マフィアボス"))
    CALL(evt_npc_move_position, STRING("マフィアボス"), -290, -140, 0, FLOAT(80.0f), 0)
    CALL(evt_npc_move_position, STRING("マフィアボス"), -290, -400, 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_position, STRING("マフィアボス"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("マフィアボスカジノ"), -270, 0, -450)
    CALL(evt_npc_set_ry, STRING("マフィアボスカジノ"), 180)
    WAIT_MS(1200)
    CALL(evt_cam3d_evt_set, -534, 252, -115, -188, 200, -115, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("ピートン"), 180)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_074"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_5_075"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("mac_5_076"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_5_077"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("mac_5_078"), 0, STRING("モニー"))
    CALL(evt_cam3d_evt_set, -683, 285, -150, -188, 211, -150, 0, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_set_ry, STRING("マフィア１"), 180)
        CALL(evt_npc_move_position, STRING("マフィア１"), -135, -250, 0, FLOAT(80.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_ry, STRING("マフィア２"), 180)
    CALL(evt_npc_move_position, STRING("マフィア２"), -135, -200, 0, FLOAT(80.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_5_079"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_080"), 0, STRING("マフィア２"))
    CALL(evt_msg_print, 0, STRING("mac_5_081"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_5_082"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("mac_5_083"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_084"), 0, STRING("マフィア２"))
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア１"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア２"), 1073741856)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 279)
    BEGIN_INLINE_EVENT
        CALL(evt_del_case_evt, 0, GW(12))
        CALL(evt_del_case_evt, 0, GW(13))
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
    CALL(evt_snd_bgmoff, 18432)
    SET_VALUE(GF(5), 1)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_talk_common)
    SET_VALUE(LSWF(3), 1)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        CALL(evt_msg_print, 0, STRING("mac_2_044_01"), 0, STRING("マフィアボス"))
        CASE_EQUAL(121)
        CASE_EQUAL(122)
        CALL(evt_msg_print, 0, STRING("mac_2_069"), 0, STRING("マフィアボス"))
        CASE_EQUAL(123)
        SET_VALUE(GF(15), 1)
        SET_VALUE(GF(14), 1)
        WAIT_WHILE(GF(14))
        CASE_BETWEEN(124, 126)
        CALL(evt_msg_print, 0, STRING("mac_2_120"), 0, STRING("マフィアボス"))
        CASE_EQUAL(125)
        CALL(evt_msg_print, 0, STRING("mac_2_120"), 0, STRING("マフィアボス"))
        CASE_BETWEEN(268, 271)
        CALL(evt_msg_print, 0, STRING("mac_5_011"), 0, STRING("マフィアボス"))
        CASE_EQUAL(272)
        CALL_CHILD(maffiaboss_teshita_irai)
        CASE_EQUAL(273)
        CALL(evt_msg_print, 0, STRING("mac_5_037"), 0, STRING("マフィアボス"))
        CASE_BETWEEN(274, 275)
        CALL(evt_msg_print, 0, STRING("mac_5_037"), 0, STRING("マフィアボス"))
        CASE_EQUAL(278)
        CALL_CHILD(maffiaboss_train_ticket)
        CASE_LESS(121)
        CALL(evt_msg_print, 0, STRING("gor_03_01_01"), 0, STRING("マフィアボス"))
        CASE_LESS(230)
        CALL(evt_msg_print, 0, STRING("gor_03_01_03"), 0, STRING("マフィアボス"))
    END_SWITCH
    SET_VALUE(LSWF(3), 0)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_talk)
    CALL(evt_party_stop, 0)
    CALL_CHILD(maffiaboss_talk_common)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_desk_talk)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir_npc, STRING("マフィアボス"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_INLINE_EVENT
    CALL(evt_npc_flag_check, STRING("マフィアボス"), 16777216, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_set_dir_to_target, STRING("マフィアボス"), STRING("mario"))
        END_INLINE_EVENT
    END_IF
    CALL_CHILD(maffiaboss_talk_common)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_casino_init)
    SWITCH(GSW(0))
        CASE_GREATER(278)
        CALL(evt_npc_set_position, STRING("me"), -270, 0, -450)
        CALL(evt_npc_set_ry, STRING("me"), 180)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_casino_talk)
    SWITCH(GSW(0))
        CASE_LESS(272)
        CALL(evt_msg_print, 0, STRING("gor_03_01_04"), 0, STRING("me"))
        CASE_LESS(352)
        CALL(evt_msg_print, 0, STRING("gor_03_01_05"), 0, STRING("me"))
        CASE_LESS(382)
        CALL(evt_msg_print, 0, STRING("gor_03_01_06"), 0, STRING("me"))
        CASE_LESS(403)
        CALL(evt_msg_print, 0, STRING("gor_03_01_07"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(403)
        CALL(evt_msg_print, 0, STRING("gor_03_01_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_nekomu00)
    CALL(evt_msg_print, 0, STRING("mac_5_009"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_010"), 0, STRING("マフィア２"))
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_nekomu01)
    CALL(evt_msg_print, 0, STRING("mac_5_033"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_034"), 0, STRING("マフィア２"))
    RETURN
EVT_END()

EVT_BEGIN(maffiaboss_nekomu02)
    CALL(evt_msg_print, 0, STRING("mac_5_035"), 0, STRING("マフィア１"))
    CALL(evt_msg_print, 0, STRING("mac_5_036"), 0, STRING("マフィア２"))
    RETURN
EVT_END()

EVT_BEGIN(maffia1_init)
    CALL(evt_npc_set_position, STRING("me"), -135, 155, -260)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_EQUAL(LSWF(3), 0)
                CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(maffia1_talk)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        CALL(evt_msg_print, 0, STRING("mac_2_044_02"), 0, STRING("me"))
        CASE_BETWEEN(122, 123)
        IF_INT_EQUAL(GSWF(1236), 0)
            CALL(evt_msg_print, 0, STRING("mac_2_070"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("mac_2_116_buka1"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(124, 126)
        CALL(evt_msg_print, 0, STRING("mac_2_121"), 0, STRING("me"))
        CASE_BETWEEN(268, 272)
        CALL_CHILD(maffiaboss_nekomu00)
        CASE_EQUAL(273)
        CALL_CHILD(maffiaboss_nekomu01)
        CASE_BETWEEN(274, 275)
        CALL_CHILD(maffiaboss_nekomu02)
        CASE_EQUAL(278)
        CALL(evt_msg_print, 0, STRING("mac_5_042"), 0, STRING("me"))
        CASE_LESS(121)
        CALL(evt_msg_print, 0, STRING("gor_03_02_01"), 0, STRING("me"))
        CASE_LESS(122)
        CALL(evt_msg_print, 0, STRING("gor_03_02_02"), 0, STRING("me"))
        CASE_LESS(230)
        CALL(evt_msg_print, 0, STRING("gor_03_02_03"), 0, STRING("me"))
        CASE_LESS(272)
        CALL(evt_msg_print, 0, STRING("gor_03_02_04"), 0, STRING("me"))
        CASE_LESS(352)
        CALL(evt_msg_print, 0, STRING("gor_03_02_05"), 0, STRING("me"))
        CASE_LESS(382)
        CALL(evt_msg_print, 0, STRING("gor_03_02_06"), 0, STRING("me"))
        CASE_LESS(403)
        CALL(evt_msg_print, 0, STRING("gor_03_02_07"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(403)
        CALL(evt_msg_print, 0, STRING("gor_03_02_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(maffia2_init)
    CALL(evt_npc_set_position, STRING("me"), -135, 155, -210)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_EQUAL(LSWF(3), 0)
                CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(maffia2_talk)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        CALL(evt_msg_print, 0, STRING("mac_2_044_03"), 0, STRING("me"))
        CASE_BETWEEN(122, 123)
        IF_INT_EQUAL(GSWF(1236), 0)
            CALL(evt_msg_print, 0, STRING("mac_2_071"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("mac_2_116_buka2"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(124, 126)
        CALL(evt_msg_print, 0, STRING("mac_2_122"), 0, STRING("me"))
        CASE_BETWEEN(268, 272)
        CALL_CHILD(maffiaboss_nekomu00)
        CASE_EQUAL(273)
        CALL_CHILD(maffiaboss_nekomu01)
        CASE_BETWEEN(274, 275)
        CALL_CHILD(maffiaboss_nekomu02)
        CASE_EQUAL(278)
        CALL(evt_msg_print, 0, STRING("mac_5_043"), 0, STRING("me"))
        CASE_LESS(121)
        CALL(evt_msg_print, 0, STRING("gor_03_03_01"), 0, STRING("me"))
        CASE_LESS(122)
        CALL(evt_msg_print, 0, STRING("gor_03_03_02"), 0, STRING("me"))
        CASE_LESS(230)
        CALL(evt_msg_print, 0, STRING("gor_03_03_03"), 0, STRING("me"))
        CASE_LESS(272)
        CALL(evt_msg_print, 0, STRING("gor_03_03_04"), 0, STRING("me"))
        CASE_LESS(352)
        CALL(evt_msg_print, 0, STRING("gor_03_03_05"), 0, STRING("me"))
        CASE_LESS(382)
        CALL(evt_msg_print, 0, STRING("gor_03_03_06"), 0, STRING("me"))
        CASE_LESS(403)
        CALL(evt_msg_print, 0, STRING("gor_03_03_07"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(403)
        CALL(evt_msg_print, 0, STRING("gor_03_03_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(maffia3_init)
    CALL(evt_npc_set_position, STRING("me"), -140, 0, 10)
    RETURN
EVT_END()

EVT_BEGIN(maffia3_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_008"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_009"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_010"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_011"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_012"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_013"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_014"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_015"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(mony_init)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        CALL(evt_npc_set_position, STRING("me"), -265, 155, -225)
        CALL(evt_npc_set_ry, STRING("me"), 180)
        CASE_EQUAL(278)
        CALL(evt_npc_set_position, STRING("me"), -145, 155, -95)
        CALL(evt_npc_set_ry, STRING("me"), 0)
        CASE_BETWEEN(279, 281)
        CALL(evt_npc_set_position, STRING("me"), -240, 155, -95)
        CALL(evt_npc_set_ry, STRING("me"), 0)
        CASE_GREATER(281)
        CALL(evt_npc_set_position, STRING("me"), -130, 155, -115)
        CALL(evt_npc_set_ry, STRING("me"), 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(mony_talk)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        CALL(evt_msg_print, 0, STRING("mac_2_044_05"), 0, STRING("me"))
        CASE_EQUAL(278)
        CALL(evt_msg_print, 0, STRING("mac_5_044"), 0, STRING("me"))
        CASE_LESS(352)
        CALL(evt_msg_print, 0, STRING("gor_03_04_01"), 0, STRING("me"))
        CASE_LESS(382)
        CALL(evt_msg_print, 0, STRING("gor_03_04_02"), 0, STRING("me"))
        CASE_LESS(403)
        CALL(evt_msg_print, 0, STRING("gor_03_04_03"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(403)
        CALL(evt_msg_print, 0, STRING("gor_03_04_04"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_init)
    IF_INT_EQUAL(GSW(1420), 19)
        CALL(evt_npc_set_position, STRING("me"), 275, 0, 413)
        CALL(evt_npc_set_ry, STRING("me"), 0)
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        CALL(evt_npc_set_position, STRING("me"), -280, 155, -185)
        CALL(evt_npc_set_ry, STRING("me"), 0)
        CASE_EQUAL(278)
        CALL(evt_npc_set_position, STRING("me"), -145, 155, -155)
        CALL(evt_npc_set_ry, STRING("me"), 180)
        CASE_BETWEEN(279, 281)
        CALL(evt_npc_set_position, STRING("me"), -280, 155, -80)
        CALL(evt_npc_set_ry, STRING("me"), 0)
        CASE_GREATER(281)
        CALL(evt_npc_set_position, STRING("me"), -198, 155, -80)
        CALL(evt_npc_set_ry, STRING("me"), 0)
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
u32 iri_19_item_tbl[2] = {
    0x00000049, 0xFFFFFFFF,
};

EVT_BEGIN(peton_talk_common)
    IF_INT_GREATER(GSW(0), 279)
        SET_VALUE(LSWF(3), 1)
    END_IF
    IF_INT_EQUAL(GSW(1420), 19)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_19_00"), 0, STRING("ピートン"))
            SET_VALUE(GSWF(5405), 1)
        ELSE
            IF_INT_EQUAL(GSWF(5406), 0)
                CALL(evt_msg_print, 0, STRING("irai_19_01"), 0, STRING("ピートン"))
            ELSE
                CALL(evt_msg_print, 0, STRING("irai_19_02"), 0, STRING("ピートン"))
                CALL(evt_win_item_select, 0, PTR(iri_19_item_tbl), LW(0), 0)
                IF_INT_NOT_EQUAL(LW(0), -1)
                    CALL(evt_pouch_remove_item, 73, LW(0))
                    CALL(evt_msg_print, 0, STRING("irai_19_03"), 0, STRING("ピートン"))
                    CALL(evt_item_entry, STRING("mg3"), 93, 0, -1000, 0, 17, -1, 0)
                    CALL(evt_item_get_item, STRING("mg3"))
                    CALL(evt_item_get_item_end_wait, STRING("mg3"))
                    CALL(evt_msg_print, 0, STRING("irai_19_04"), 0, STRING("ピートン"))
                    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 32)
                    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
                    WAIT_FRAMES(1)
                    CALL(evt_npc_wait_pera, STRING("ピートン"))
                    CALL(evt_msg_print, 0, STRING("irai_19_05"), 0, STRING("ピートン"))
                    CALL(evt_npc_move_position, STRING("ピートン"), 224, 478, 0, 150, 0)
                    CALL(evt_npc_move_position, STRING("ピートン"), -20, 478, 0, 150, 0)
                    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 32)
                    SWITCH(GSW(0))
                        CASE_BETWEEN(279, 281)
                        CALL(evt_npc_set_position, STRING("ピートン"), -290, 155, -80)
                        CALL(evt_npc_set_ry, STRING("ピートン"), 90)
                        CASE_GREATER(281)
                        CALL(evt_npc_set_position, STRING("ピートン"), -198, 155, -90)
                        CALL(evt_npc_set_ry, STRING("ピートン"), 90)
                    END_SWITCH
                    SET_VALUE(GSW(1420), 0)
                    SET_VALUE(GSWF(5393), 1)
                    CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
                    CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
                END_IF
            END_IF
        END_IF
        RETURN
    END_IF
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        CALL(evt_msg_print, 0, STRING("mac_2_044_04"), 0, STRING("me"))
        CASE_EQUAL(278)
        CALL(evt_msg_print, 0, STRING("mac_5_045"), 0, STRING("ピートン"))
        CASE_LESS(352)
        CALL(evt_msg_print, 0, STRING("gor_03_05_01"), 0, STRING("ピートン"))
        CASE_LESS(382)
        CALL(evt_msg_print, 0, STRING("gor_03_05_02"), 0, STRING("ピートン"))
        CASE_LESS(403)
        CALL(evt_msg_print, 0, STRING("gor_03_05_03"), 0, STRING("ピートン"))
        CASE_GREATER_OR_EQUAL(403)
        CALL(evt_msg_print, 0, STRING("gor_03_05_04"), 0, STRING("ピートン"))
    END_SWITCH
    IF_INT_GREATER(GSW(0), 279)
        SET_VALUE(LSWF(3), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(peton_talk)
    CALL_CHILD(peton_talk_common)
    RETURN
EVT_END()

EVT_BEGIN(peton_desk_talk)
    IF_INT_EQUAL(GSW(1420), 19)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_INLINE_EVENT
    CALL(evt_npc_flag_check, STRING("ピートン"), 16777216, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("mario"))
        END_INLINE_EVENT
    END_IF
    CALL_CHILD(peton_talk_common)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_gurume)
    CALL(evt_npc_set_position, STRING("me"), -160, 0, 480)
    CALL(evt_lottery_error_check, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_eff_fukidashi, 1, STRING("husei_huki"), STRING("me"), 5, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(talk_gurume)
    CALL_CHILD(kuzi_bomb_talk)
    RETURN
EVT_END()

static EVT_BEGIN(luigi_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(164, 214)
        CALL(evt_npc_set_position, STRING("me"), 440, 0, -20)
        CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(luigi_talk)
    CALL_CHILD(luigi_gor_first_talk)
    RETURN
EVT_END()

static EVT_BEGIN(luigi_npcEnt)
    SWITCH(GSW(0))
        CASE_BETWEEN(164, 214)
        CALL(evt_npc_entry, STRING("チェリー"), STRING("c_lp_bom"))
        CALL(evt_npc_set_tribe, STRING("チェリー"), STRING("チェリー"))
        CALL(evt_npc_setup, PTR(cherryEnt))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(cherry_init)
    CALL(evt_npc_set_position, STRING("me"), 480, 0, -20)
    CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(cherry_talk)
    CALL(evt_msg_print, 0, STRING("luigi_npc_02"), 0, STRING("me"))
    RETURN
EVT_END()

// TODO: type
u32 cherryEnt[46] = {
    (u32)"チェリー", 0x00080600, 0x00000000, (u32)&cherry_init,
    0x00000000, (u32)&cherry_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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

EVT_BEGIN(bteresa_n_fadein)
    CALL(evt_npc_get_position, STRING("バニーテレサ"), LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_GAME_TELESA_MOVE2"), LW(1), LW(2), LW(3), 0)
    SET_VALUE(LW(1), LW(0))
    LOOP(LW(0))
        SET_VALUE(LW(2), LW(1))
        SUBTRACT(LW(2), LW(0))
        ADD(LW(2), 1)
        MULTIPLY(LW(2), 255)
        DIVIDE(LW(2), LW(1))
        CALL(evt_npc_set_color, STRING("バニーテレサ"), 255, 255, 255, LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(bteresa_n_fadeout)
    CALL(evt_npc_get_position, STRING("バニーテレサ"), LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_GAME_TELESA_MOVE1"), LW(1), LW(2), LW(3), 0)
    SET_VALUE(LW(1), LW(0))
    LOOP(LW(0))
        SET_VALUE(LW(2), LW(0))
        SUBTRACT(LW(2), 1)
        MULTIPLY(LW(2), 255)
        DIVIDE(LW(2), LW(1))
        CALL(evt_npc_set_color, STRING("バニーテレサ"), 255, 255, 255, LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(exchange_return_sub)
    IF_INT_NOT_EQUAL(LW(11), 0)
        CALL(evt_cam3d_evt_set, FLOAT(-541.0f), FLOAT(125.0f), FLOAT(-407.0f), FLOAT(-197.0f), FLOAT(64.0f), FLOAT(-407.0f), 600, 11)
        WAIT_MS(600)
    END_IF
    CALL(evt_npc_get_position, STRING("バニーテレサ"), LW(6), LW(7), LW(10))
    CALL(evt_npc_get_dir, STRING("バニーテレサ"), LW(9))
    SET_VALUE(LW(0), 48)
    CALL_CHILD(bteresa_n_fadeout)
    IF_INT_NOT_EQUAL(LW(11), 0)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 30)
    SUBTRACT(LW(2), 20)
    CALL(evt_npc_set_position, STRING("バニーテレサ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("バニーテレサ"), 180)
    SET_VALUE(LW(0), 48)
    CALL_CHILD(bteresa_n_fadein)
    CALL(yuugijou_get_montemedal_keep, LW(1))
    CALL(evt_msg_print_insert, 0, STRING("bunny_teresa_90"), 0, STRING("バニーテレサ"), LW(1))
    CALL(yuugijou_montemedal_return)
    SET_VALUE(LW(0), 48)
    CALL_CHILD(bteresa_n_fadeout)
    CALL(evt_npc_set_position, STRING("バニーテレサ"), LW(6), LW(7), LW(10))
    CALL(evt_npc_set_ry, STRING("バニーテレサ"), LW(9))
    SET_VALUE(LW(0), 48)
    IF_INT_EQUAL(LW(11), 0)
        CALL_CHILD(bteresa_n_fadein)
    ELSE
        BEGIN_INLINE_EVENT
            CALL_CHILD(bteresa_n_fadein)
        END_INLINE_EVENT
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(exchange_keep_sub)
    CALL(yuugijou_get_takeoutflag, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(yuugijou_get_montemedal, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_cam3d_evt_set, FLOAT(-541.0f), FLOAT(125.0f), FLOAT(-407.0f), FLOAT(-197.0f), FLOAT(64.0f), FLOAT(-407.0f), 600, 11)
            WAIT_MS(600)
            CALL(evt_npc_get_position, STRING("バニーテレサ"), LW(6), LW(7), LW(10))
            CALL(evt_npc_get_dir, STRING("バニーテレサ"), LW(9))
            SET_VALUE(LW(0), 48)
            CALL_CHILD(bteresa_n_fadeout)
            CALL(evt_cam3d_evt_off, 0, 11)
            WAIT_FRAMES(1)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 20)
            SUBTRACT(LW(2), 30)
            CALL(evt_npc_set_position, STRING("バニーテレサ"), LW(0), LW(1), LW(2))
            CALL(evt_npc_set_ry, STRING("バニーテレサ"), 180)
            SET_VALUE(LW(0), 48)
            CALL_CHILD(bteresa_n_fadein)
            CALL(yuugijou_get_montemedal, LW(1))
            CALL(evt_msg_print_insert, 0, STRING("bunny_teresa_91"), 0, STRING("バニーテレサ"), LW(1))
            CALL(yuugijou_montemedal_keep)
            SET_VALUE(LW(0), 48)
            CALL_CHILD(bteresa_n_fadeout)
            CALL(evt_npc_set_position, STRING("バニーテレサ"), LW(6), LW(7), LW(10))
            CALL(evt_npc_set_ry, STRING("バニーテレサ"), LW(9))
            SET_VALUE(LW(0), 1)
            CALL_CHILD(bteresa_n_fadein)
        ELSE
            CALL(yuugijou_montemedal_keep)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(gor_03_dokan_init_evt)
    CALL(evt_hit_bind_mapobj, STRING("A_dokan_l_02"), STRING("S_dokan_1"))
    IF_INT_NOT_EQUAL(GSWF(1229), 0)
        CALL(evt_mapobj_trans, 1, STRING("S_dokan_1"), 0, 0, 0)
    ELSE
        CALL(evt_mapobj_trans, 1, STRING("S_dokan_1"), 0, -31, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_dokan_1_kage"), 0, -1000, 0)
    END_IF
    CALL(evt_hit_bind_update, STRING("A_dokan_l_02"))
    RETURN
EVT_END()

EVT_BEGIN(gor_03_dokan_up_evt)
    CALL(evt_mapobj_get_position, STRING("S_dokan_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOKAN_APPEAR1"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_snd_sfx_vol, LW(3), 76)
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_mapobj_trans, 1, STRING("S_dokan_1_kage"), 0, 0, 0)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 60)
    LOOP(LW(0))
        SET_FLOAT_VALUE(LW(1), LW(0))
        SUBTRACT_FLOAT(LW(1), 1)
        MULTIPLY_FLOAT(LW(1), -31)
        DIVIDE_FLOAT(LW(1), 60)
        CALL(evt_mapobj_trans, 1, STRING("S_dokan_1"), 0, LW(1), 0)
        CALL(evt_hit_bind_update, STRING("A_dokan_l_02"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gor_03_dokan_down_evt)
    CALL(evt_mapobj_get_position, STRING("S_dokan_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOKAN_GOOUT1"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_snd_sfx_vol, LW(3), 76)
    BEGIN_INLINE_EVENT
        WAIT_MS(900)
        CALL(evt_mapobj_trans, 1, STRING("S_dokan_1_kage"), 0, -1000, 0)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 60)
    LOOP(LW(0))
        SET_FLOAT_VALUE(LW(1), 60)
        ADD_FLOAT(LW(1), 1)
        SUBTRACT_FLOAT(LW(1), LW(0))
        MULTIPLY_FLOAT(LW(1), -31)
        DIVIDE_FLOAT(LW(1), 60)
        CALL(evt_mapobj_trans, 1, STRING("S_dokan_1"), 0, LW(1), 0)
        CALL(evt_hit_bind_update, STRING("A_dokan_l_02"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
s32 exchange_ryokin_medal_tbl[9][3][2] = {
    0x00000003, 0x00000001, 0x0000000F, 0x00000005,
    0x0000001E, 0x0000000A, 0x00000003, 0x00000001,
    0x0000000F, 0x00000005, 0x0000001E, 0x0000000A,
    0x00000003, 0x00000001, 0x0000000F, 0x00000005,
    0x0000001E, 0x0000000A, 0x00000003, 0x00000001,
    0x0000000F, 0x00000005, 0x0000001E, 0x0000000A,
    0x00000003, 0x00000001, 0x0000000F, 0x00000005,
    0x0000001E, 0x0000000A, 0x00000003, 0x00000001,
    0x0000000F, 0x00000005, 0x0000001E, 0x0000000A,
    0x00000003, 0x00000001, 0x0000000F, 0x00000005,
    0x0000001E, 0x0000000A, 0x00000003, 0x00000001,
    0x0000000F, 0x00000005, 0x0000001E, 0x0000000A,
    0x00000003, 0x00000001, 0x0000000F, 0x00000005,
    0x0000001E, 0x0000000A,
};
USER_FUNC(mario_walkpoint_enzan) {
    s32* args = event->args;
    Vec a;
    Vec b;
    Vec c;
    f32 dist;

    a.x = evtGetFloat(event, args[0]);
    a.y = 0.0f;
    a.z = evtGetFloat(event, args[1]);
    b.x = evtGetFloat(event, args[2]);
    b.y = 0.0f;
    b.z = evtGetFloat(event, args[3]);
    dist = evtGetFloat(event, args[4]);
    PSVECSubtract(&a, &b, &c);
    PSVECNormalize(&c, &c);
    PSVECScale(&c, &c, dist);
    PSVECAdd(&c, &b, &c);
    evtSetFloat(event, args[5], c.x);
    evtSetFloat(event, args[6], c.z);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(bteresa_talk_sub_minigame)
    SET_VALUE(LW(15), LW(0))
    LOOP(0)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_34"))
        CALL(evt_msg_select, 0, STRING("bunny_teresa_yesno2"))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_msg_print_add, 0, STRING("bunny_teresa_48"))
            RETURN
        ELSE
            IF_INT_EQUAL(LW(0), 2)
                CALL(evt_msg_continue)
                SWITCH(LW(15))
                    CASE_EQUAL(0)
                    CALL(evt_msg_print, 0, STRING("bunny_teresa_38"), 0, STRING("me"))
                    CASE_EQUAL(1)
                    CALL(evt_msg_print, 0, STRING("bunny_teresa_39"), 0, STRING("me"))
                    CASE_EQUAL(2)
                    CALL(evt_msg_print, 0, STRING("bunny_teresa_40"), 0, STRING("me"))
                    CASE_EQUAL(3)
                    CALL(evt_msg_print, 0, STRING("bunny_teresa_41"), 0, STRING("me"))
                END_SWITCH
            ELSE
                BREAK_LOOP
            END_IF
        END_IF
    END_LOOP
    SET_VALUE(LW(1), 10)
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(1))
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_35"))
        RETURN
    END_IF
    MULTIPLY(LW(1), -1)
    CALL(yuugijou_add_montemedal, 0, LW(1))
    CALL(yuugijou_montemedal_wait)
    WAIT_MS(200)
    CALL(evt_msg_print_add, 0, STRING("bunny_teresa_36"))
    WAIT_MS(500)
    CALL_CHILD(gor_03_dokan_up_evt)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_dokan_1"), LW(3), LW(4), LW(5))
    CALL(mario_walkpoint_enzan, LW(0), LW(2), LW(3), LW(5), FLOAT(40.0f), LW(0), LW(2))
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(120.0f))
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, LW(3), 30, LW(5), 500, 0, 10)
    CALL(evt_msg_print, 0, STRING("bunny_teresa_37"), 0, STRING("me"))
    SWITCH(LW(15))
        CASE_EQUAL(0)
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("A_dokan_02"), PTR(yuu_00_data))
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_on)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("A_dokan_02"), PTR(yuu_03_data))
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_on)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("A_dokan_02"), PTR(yuu_02_data))
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_on)
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("A_dokan_02"), PTR(yuu_01_data))
        SET_VALUE(LW(0), STRING("A_dokan_02"))
        CALL_CHILD(bero_case_switch_on)
    END_SWITCH
    CALL(yuugijou_montemedal_disp_onoff, 0)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_bgmoff_f, 513, 2000)
    CALL(evt_snd_envoff_f, 512, 2000)
    SET_VALUE(LW(1), STRING("A_dokan_02"))
    CALL(evt_bero_id_filter, LW(1))
    CALL(evt_bero_get_info_num, LW(1))
    CALL_CHILD(bero_case_exec_pipe)
    RETURN
EVT_END()

EVT_BEGIN(bteresa_talk_sub_keihin)
    CALL(evt_badgeShop_bteresa_get_kind_cnt, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_64"))
        GOTO(0)
    END_IF
    IF_INT_EQUAL(GSWF(1228), 0)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_60"))
        SET_VALUE(GSWF(1228), 1)
    ELSE
        CALL(evt_msg_continue)
    END_IF
    LABEL(1)
    CALL(yuugijou_set_yupflag, 1)
    CALL(evt_win_other_select, 16)
    CALL(yuugijou_set_yupflag, 0)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_74"), 0, STRING("me"))
        GOTO(0)
    END_IF
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_msg_print, 0, STRING("bunny_teresa_65"), 0, STRING("me"))
        GOTO(1)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(1), 240)
        CALL(evt_sub_get_language, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_insert, 0, STRING("bunny_teresa_67"), 0, STRING("me"), LW(3), LW(2), LW(4))
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("bunny_teresa_67"), 0, STRING("me"), LW(2), LW(3), LW(4))
        END_IF
    ELSE
        CALL(evt_sub_get_language, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_insert, 0, STRING("bunny_teresa_66"), 0, STRING("me"), LW(3), LW(2))
        ELSE
        END_IF
        CALL(evt_msg_print_insert, 0, STRING("bunny_teresa_66"), 0, STRING("me"), LW(2), LW(3))
    END_IF
    CALL(evt_msg_select, 0, STRING("bunny_teresa_yesno"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_68"))
        GOTO(0)
    END_IF
    CALL(evt_pouch_add_item, LW(1), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        IF_INT_GREATER_OR_EQUAL(LW(1), 240)
            CALL(evt_msg_print_add, 0, STRING("bunny_teresa_72"))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("bunny_teresa_73"))
        END_IF
        GOTO(0)
    END_IF
    MULTIPLY(LW(3), -1)
    CALL(yuugijou_add_montemedal, 0, LW(3))
    CALL(yuugijou_montemedal_wait)
    WAIT_MS(200)
    CALL(evt_badgeShop_bteresa_get_kind_cnt, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_69"))
        GOTO(0)
    END_IF
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_69"))
        GOTO(0)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("bunny_teresa_70"))
    CALL(evt_msg_select, 0, STRING("bunny_teresa_yesno"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_71"))
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    GOTO(1)
    LABEL(0)
    RETURN
EVT_END()

USER_FUNC(minigameselect_msg_set) {
    s32* args = event->args;
    s8* msg = (s8*)msgSearch((const char*)evtGetValue(event, args[0]));
    s32 flags[4];
    s32 i;
    s32 src;
    s32 pos;
    s32 count;
    s32 num;

    flags[0] = evtGetValue(event, GSWF(1232));
    flags[1] = evtGetValue(event, GSWF(1235));
    flags[2] = evtGetValue(event, GSWF(1234));
    flags[3] = evtGetValue(event, GSWF(1233));

    pos = 0;
    src = 0;

    while (msg[src] != '\n') {
        ((char*)exchange_msgp[0])[pos++] = msg[src++];
    }
    {
        char* base = (char*)exchange_msgp[0];
        base[pos] = msg[src];
    }
    src++;
    pos++;

    count = 0;
    for (i = 0; i < 4; i++) {
        if (flags[i] != 0) {
            while (msg[src] != '\n') {
                ((char*)exchange_msgp[0])[pos++] = msg[src++];
            }
            count++;
            {
                char* base = (char*)exchange_msgp[0];
                base[pos++] = msg[src++];
            }
        } else {
            while (msg[src] != '\n') {
                src++;
            }
            src++;
        }
    }

    while (msg[src] != 0) {
        ((char*)exchange_msgp[0])[pos++] = msg[src++];
    }
    {
        char* base = (char*)exchange_msgp[0];
        base[pos] = msg[src];
    }

    {
        u32* base = exchange_msgp;
        i = 0;
        while (((s8*)base[0])[i] != '>') {
            i++;
        }
        while (((s8*)base[0])[i] != ' ') {
            i--;
        }
    }
    num = 0x50 - (4 - count) * 12;
    {
        u32* base = exchange_msgp;
        ((char*)base[0])[i + 1] = num / 10 + '0';
        ((char*)base[0])[i + 2] = num % 10 + '0';
    }

    evtSetValue(event, args[1], exchange_msgp[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_exchange_msg_alloc) {
    exchange_msgp[0] = (u32)_mapAlloc(0x100);
    memset((void*)exchange_msgp[0], 0, 0x100);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_exchange_msg_free) {
    if (exchange_msgp[0] == 0) {
        return EVT_RETURN_DONE;
    }
    _mapFree((void*)exchange_msgp[0]);
    exchange_msgp[0] = 0;
    return EVT_RETURN_DONE;
}

static USER_FUNC(setmertn) {
    event->ownerNPC = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

EVT_BEGIN(bteresa_table_talk)
    CALL(setmertn, STRING("バニーテレサ"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    IF_INT_EQUAL(GSW(1420), 11)
        CALL(evt_msg_print, 0, STRING("irai_11_001"), 0, STRING("me"))
        CALL(yuugijou_get_takeoutflag, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(yuugijou_montemedal_return)
        END_IF
        CALL(yuugijou_montemedal_disp_onoff, 1)
        CALL(yuugijou_add_montemedal, 0, 10)
        CALL(yuugijou_montemedal_wait)
        CALL(evt_msg_print, 0, STRING("irai_11_003"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_N_B"))
        WAIT_MS(600)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_print, 0, STRING("irai_11_004"), 0, STRING("me"))
        SET_VALUE(GSW(1420), 0)
        SET_VALUE(GSWF(5385), 1)
        CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_party_run, 0)
        RETURN
    END_IF
    CALL(yuugijou_get_takeoutflag, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(yuugijou_get_montemedal_keep, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(11), 0)
            CALL_CHILD(exchange_return_sub)
        ELSE
            CALL(yuugijou_montemedal_return)
        END_IF
    END_IF
    SET_VALUE(LW(1), 0)
    IF_INT_EQUAL(GSWF(1232), 0)
        CALL(evt_pouch_check_item, 91, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            ADD(LW(1), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1233), 0)
        CALL(evt_pouch_check_item, 92, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            ADD(LW(1), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1234), 0)
        CALL(evt_pouch_check_item, 93, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            ADD(LW(1), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1235), 0)
        CALL(evt_pouch_check_item, 94, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            ADD(LW(1), 1)
        END_IF
    END_IF
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_42"), 0, STRING("me"))
        IF_INT_EQUAL(GSWF(1232), 0)
            CALL(evt_pouch_check_item, 91, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                SET_VALUE(GSWF(1232), 1)
                CALL(evt_msg_print, 0, STRING("bunny_teresa_43"), 0, 0)
            END_IF
        END_IF
        IF_INT_EQUAL(GSWF(1235), 0)
            CALL(evt_pouch_check_item, 94, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                SET_VALUE(GSWF(1235), 1)
                CALL(evt_msg_print, 0, STRING("bunny_teresa_44"), 0, 0)
            END_IF
        END_IF
        IF_INT_EQUAL(GSWF(1234), 0)
            CALL(evt_pouch_check_item, 93, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                SET_VALUE(GSWF(1234), 1)
                CALL(evt_msg_print, 0, STRING("bunny_teresa_45"), 0, 0)
            END_IF
        END_IF
        IF_INT_EQUAL(GSWF(1233), 0)
            CALL(evt_pouch_check_item, 92, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                SET_VALUE(GSWF(1233), 1)
                CALL(evt_msg_print, 0, STRING("bunny_teresa_46"), 0, 0)
            END_IF
        END_IF
        SET_VALUE(GSWF(1227), 1)
        SET_VALUE(LW(14), 1)
    ELSE
        SET_VALUE(LW(14), 0)
        IF_INT_NOT_EQUAL(GSWF(1232), 0)
            SET_VALUE(LW(14), 1)
        END_IF
        IF_INT_NOT_EQUAL(GSWF(1235), 0)
            SET_VALUE(LW(14), 1)
        END_IF
        IF_INT_NOT_EQUAL(GSWF(1234), 0)
            SET_VALUE(LW(14), 1)
        END_IF
        IF_INT_NOT_EQUAL(GSWF(1233), 0)
            SET_VALUE(LW(14), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1227), 0)
        IF_INT_LESS(GSW(0), 403)
            IF_INT_EQUAL(GSWF(1226), 0)
                CALL(evt_msg_print, 0, STRING("bunny_teresa_00"), 0, STRING("me"))
                SET_VALUE(GSWF(1226), 1)
            ELSE
                CALL(evt_msg_print, 0, STRING("bunny_teresa_01"), 0, STRING("me"))
            END_IF
        ELSE
            CALL(evt_msg_print, 0, STRING("bunny_teresa_01_1"), 0, STRING("me"))
        END_IF
        SET_VALUE(GSWF(1227), 1)
    ELSE
        CALL(evt_msg_print, 0, STRING("bunny_teresa_02"), 0, STRING("me"))
    END_IF
    CALL(evt_exchange_msg_alloc)
    CALL(minigameselect_msg_set, STRING("bunny_teresa_03"), LW(0))
    CALL(evt_msg_select, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 5)
        LOOP(0)
            IF_INT_NOT_EQUAL(GSWF(1232), 0)
                IF_INT_EQUAL(LW(0), 0)
                    BREAK_LOOP
                END_IF
            ELSE
                ADD(LW(0), 1)
            END_IF
            IF_INT_NOT_EQUAL(GSWF(1235), 0)
                IF_INT_EQUAL(LW(0), 1)
                    BREAK_LOOP
                END_IF
            ELSE
                ADD(LW(0), 1)
            END_IF
            IF_INT_NOT_EQUAL(GSWF(1234), 0)
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
            ELSE
                ADD(LW(0), 1)
            END_IF
            IF_INT_NOT_EQUAL(GSWF(1233), 0)
                IF_INT_EQUAL(LW(0), 3)
                    BREAK_LOOP
                END_IF
            ELSE
                ADD(LW(0), 1)
            END_IF
            BREAK_LOOP
        END_LOOP
    END_IF
    IF_INT_EQUAL(LW(0), 5)
        CALL(evt_msg_print_add, 0, STRING("bunny_teresa_13"))
        GOTO(0)
    END_IF
    IF_INT_EQUAL(LW(0), 4)
        CALL_CHILD(bteresa_talk_sub_keihin)
    ELSE
        CALL_CHILD(bteresa_talk_sub_minigame)
    END_IF
    LABEL(0)
    CALL(evt_exchange_msg_free)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(bteresa_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_hako_tele_01"), 0, PTR(bteresa_table_talk), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_hako_tele_02"), 0, PTR(bteresa_table_talk), 0)
    RETURN
EVT_END()

EVT_BEGIN(bteresa_talk)
    SET_VALUE(LW(1), GSW(117))
    SET_VALUE(LW(0), 0)
    ADD(LW(0), GSWF(1240))
    ADD(LW(0), GSWF(1241))
    ADD(LW(0), GSWF(1242))
    ADD(LW(0), GSWF(1243))
    IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
        ADD(LW(1), 1)
    END_IF
    SWITCH(LW(1))
        CASE_EQUAL(0)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_11"), 0, STRING("me"))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_11"), 0, STRING("me"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_11_1"), 0, STRING("me"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_11_2"), 0, STRING("me"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_11_3"), 0, STRING("me"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_11_4"), 0, STRING("me"))
    END_SWITCH
    SET_VALUE(GSW(117), LW(1))
    RETURN
EVT_END()

EVT_BEGIN(kyaku1_init)
    RETURN
EVT_END()

EVT_BEGIN(kyaku1_talk)
    SET_VALUE(LF(0), 0)
    CALL(evt_pouch_check_item, 91, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(evt_pouch_check_item, 92, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(evt_pouch_check_item, 93, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(evt_pouch_check_item, 94, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_msg_print, 0, STRING("kyaku1_00_00"), 0, STRING("me"))
    ELSE
        CALL(evt_msg_print, 0, STRING("kyaku1_00"), 0, STRING("me"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kyaku4_init)
    RETURN
EVT_END()

EVT_BEGIN(kyaku4_talk)
    SET_VALUE(LF(0), 0)
    CALL(evt_pouch_check_item, 91, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(evt_pouch_check_item, 92, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(evt_pouch_check_item, 93, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(evt_pouch_check_item, 94, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_msg_print, 0, STRING("kyaku4_00_00"), 0, STRING("me"))
    ELSE
        CALL(evt_msg_print, 0, STRING("kyaku4_00"), 0, STRING("me"))
    END_IF
    RETURN
EVT_END()

// TODO: type
f32 montemedalzpos[2] = {
    -235.0f, -160.0f,
};

const Vec vec3_gor_0001a234 = {-113.0f, 70.0f, 0.0f};
const Vec vec3_gor_0001a240 = {0.0f, 10.0f, 0.0f};

// fall_rtn is 100%. The sfx string initialisers use a comma-fold (MUST_MATCH) so MWCC
// materialises each address directly instead of via an extra `mr`; see the note in
// rels/yuu/src/yuu_00.c.
static USER_FUNC(fall_rtn) {
    s32 j;
    s32 i;
    s32 off;
    s32 alive = 0;
    f32 f23;
    f32 moveX;
    f32 moveZ;
    Vec tmp;

    if (isFirstCall) {
        u8* data = (u8*)((GoryuugijoWork*)event->lwData[0])->data;
        monte_p[0] = (u32)_mapAlloc(*(s32*)(data + 0x5c) * 0x50);
        memset((void*)monte_p[0], 0, *(s32*)(data + 0x5c) * 0x50);
        *(u8**)((u8*)monte_p[0] + 0x4c) = data;

        for (i = 0; i < *(s32*)(*(u8**)((u8*)monte_p[0] + 0x4c) + 0x5c); i++) {
            u8* e = (u8*)monte_p[0] + i * 0x50 + 8;
            f32 spd;

            Vec pos = {-113.0f, 70.0f, 0.0f};
            pos.z = montemedalzpos[*(s32*)(*(u8**)((u8*)monte_p[0] + 0x4c) + 0x60)];
            *(Vec*)(e + 0x04) = pos;

            f23 = (2.0f * (f32)(i & 1) - 1.0f) * ((f32)(rand() % 101) / 10.0f);
            *(f32*)(e + 0x0c) += f23;
            *(f32*)(e + 0x10) = 1.0f;

            spd = 2.0f + (f32)(rand() % 300) / 100.0f;
            moveZ = 0.0f;
            moveX = 0.0f;
            movePos(&moveX, &moveZ, spd, 270.0f - 3.0f * f23);
            {
                Vec vel = {0.0f, 10.0f, 0.0f};
                vel.x = moveX;
                vel.z = moveZ;
                *(Vec*)((unsigned char*)monte_p[0] + i * 0x50 + 0x40) = vel;
            }

            *(u16*)((unsigned char*)monte_p[0] + i * 0x50 + 0x02) = (u16)(((rand() % 700) + 100) * 60 / 1000);
        }
    }

    for (j = 0, off = 0; j < *(s32*)(*(u8**)((u8*)monte_p[0] + 0x4c) + 0x5c); j++, off += 0x50) {
        u8* e = (u8*)monte_p[0] + off;
#ifdef MUST_MATCH
        // match-only: folding each literal through a comma makes MWCC materialise its address
        // directly (no extra `mr`), matching the target.
        const char* sfx_leap = (0, "SFX_EVT_GAME_MONTE_LEAPED1");
        const char* sfx_count = (0, "SFX_EVT_GAME_MONTE_COUNT1");
        const char* sfx_count2 = (0, "SFX_EVT_GAME_MONTE_COUNT1");
#else
        const char* sfx_leap = "SFX_EVT_GAME_MONTE_LEAPED1";
        const char* sfx_count = "SFX_EVT_GAME_MONTE_COUNT1";
        const char* sfx_count2 = "SFX_EVT_GAME_MONTE_COUNT1";
#endif

        switch (*(u8*)(e + 0x01)) {
        case 0:
            if ((s32)*(s16*)(e + 0x02) != 0) {
                *(s16*)(e + 0x02) = *(s16*)(e + 0x02) - 1;
            } else {
                *(u8*)(e + 0x01) = *(u8*)(e + 0x01) + 1;
            }
            break;
        case 1:
            PSVECAdd((Vec*)(e + 0x0c), (Vec*)(e + 0x40), (Vec*)(e + 0x0c));
            *(f32*)(e + 0x44) = *(f32*)(e + 0x44) - 0.81666666f;
            if (*(f32*)(e + 0x10) <= 0.0f) {
                *(f32*)(e + 0x10) = 0.0f;
                psndSFXOn_3D(sfx_leap, (Vec*)(e + 0x0c));
                if (fabs(*(f32*)(e + 0x44)) <= 4.0) {
                    *(u8*)(e + 0x01) = *(u8*)(e + 0x01) + 1;
                    *(s16*)(e + 0x04) = 300;
                } else {
                    f32 scale;
                    scale = 0.5f + (f32)(rand() % 21) / 100.0f;
                    *(f32*)(e + 0x44) = *(f32*)(e + 0x44) * -scale;
                    *(f32*)(e + 0x40) = 0.0f;
                    *(f32*)(e + 0x48) = 0.0f;
                }
            }

            if (get_monte_check((Vec*)(e + 0x0c), &marioGetPtr()->position) == 1) {
                psndSFXOn_3D(sfx_count, (Vec*)(e + 0x0c));
                if (yuwp->tokens < 0x1869f) {
                    yuwp->tokens++;
                }
                *(u8*)(e + 0x00) = 1;
                *(u8*)(e + 0x01) = 3;
            }
            break;
        case 2:
            *(s16*)(e + 0x04) = *(s16*)(e + 0x04) - 1;
            if (*(s16*)(e + 0x04) == 0) {
                *(u8*)(e + 0x00) = 1;
                *(u8*)(e + 0x01) = *(u8*)(e + 0x01) + 1;
            }
            if (*(s16*)(e + 0x04) < 0x3c) {
                if (*(s16*)(e + 0x04) & 2) {
                    *(f32*)(e + 0x10) = 0.0f;
                } else {
                    *(f32*)(e + 0x10) = -1000.0f;
                }
            }

            tmp = *(Vec*)(e + 0x0c);
            tmp.y = 0.0f;
            if (get_monte_check(&tmp, &marioGetPtr()->position) == 1) {
                psndSFXOn_3D(sfx_count2, (Vec*)(e + 0x0c));
                if (yuwp->tokens < 0x1869f) {
                    yuwp->tokens++;
                }
                *(u8*)(e + 0x00) = 1;
                *(u8*)(e + 0x01) = 3;
            }
            break;
        case 3:
            alive++;
            break;
        }
    }

    if (alive < *(s32*)(*(u8**)((u8*)monte_p[0] + 0x4c) + 0x5c)) {
        dispEntry(4, 0.0f, 1, monte_disp, (void*)monte_p[0]);
        return 0;
    }

    *(s32*)(*(u8**)((u8*)monte_p[0] + 0x4c) + 0x5c) = 0;
    *(s32*)(*(u8**)((u8*)monte_p[0] + 0x4c) + 0x6c) = 0;
    _mapFree((void*)monte_p[0]);
    return 2;
}

EVT_BEGIN(slot_fall_monte)
    CALL(fall_rtn)
    RETURN
EVT_END()
USER_FUNC(goryuugijo_get_slot_phase) {
    s32* args = event->args;
    GoryuugijoWork* work = (GoryuugijoWork*)evtGetValue(event, args[0]);

    evtSetValue(event, args[1], *(s32*)((u8*)work->data + 0x04));
    return EVT_RETURN_DONE;
}

USER_FUNC(goryuugijo_set_slot_phase) {
    s32* args = event->args;
    GoryuugijoWork* work = (GoryuugijoWork*)evtGetValue(event, args[0]);
    u8* data = (u8*)work->data;

    *(s32*)(data + 0x04) = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_goryuugijo_set_nowslot) {
    s32* args = event->args;
    GoryuugijoWork* work = (GoryuugijoWork*)evtGetValue(event, args[0]);
    u8* data = (u8*)work->data;

    *(s32*)(data + 0x60) = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(goryuugijo_get_dataadr) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    if (index >= 8) {
        evtSetValue(event, args[1], 0);
    } else {
        evtSetValue(event, args[1], dataadr[index]);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(slot_set)
    CALL(goryuugijo_get_dataadr, 1, LW(0))
    CALL(evt_goryuugijo_set_nowslot, LW(0), LW(8))
    CALL(goryuugijo_get_slot_phase, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(yuugijou_get_takeoutflag, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(yuugijou_get_montemedal_keep, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(11), 2)
            CALL_CHILD(exchange_return_sub)
        ELSE
            CALL(yuugijou_montemedal_return)
        END_IF
    END_IF
    SWITCH(LW(8))
        CASE_EQUAL(0)
        CALL(evt_cam3d_evt_set, FLOAT(-393.9f), FLOAT(113.1f), FLOAT(-235.0f), FLOAT(-197.0f), FLOAT(78.7f), FLOAT(-235.0f), 600, 11)
        WAIT_MS(600)
        CASE_EQUAL(1)
        CALL(evt_cam3d_evt_set, FLOAT(-393.9f), FLOAT(113.1f), FLOAT(-160.0f), FLOAT(-197.0f), FLOAT(78.7f), FLOAT(-160.0f), 600, 11)
        WAIT_MS(600)
    END_SWITCH
    LOOP(0)
        CALL(yuugijou_get_montemedal, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("slot_mess_01"), 0, 0)
            GOTO(0)
        END_IF
        CALL(evt_msg_select, 0, STRING("slot_mess_00"))
        IF_INT_EQUAL(LW(0), 1)
            GOTO(0)
        END_IF
        CALL(yuugijou_add_montemedal, 0, -1)
        CALL(goryuugijo_get_dataadr, 1, LW(0))
        CALL(goryuugijo_set_slot_phase, LW(0), 1)
        LOOP(0)
            CALL(goryuugijo_get_slot_phase, LW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                WAIT_MS(300)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(1), 8)
                GOTO(0)
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    LABEL(0)
    CALL(evt_cam3d_evt_off, 600, 11)
    WAIT_MS(600)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(slot1_set)
    SET_VALUE(LW(8), 0)
    CALL_CHILD(slot_set)
    RETURN
EVT_END()

EVT_BEGIN(slot2_set)
    SET_VALUE(LW(8), 1)
    CALL_CHILD(slot_set)
    RETURN
EVT_END()

char* drumname[2][17] = {
    "S_srot01_dram_01", "S_srot01_dram_02", "S_srot01_dram_03", "S_srot01_reber", "A_srot01",
    "S_srot01_m_01", "S_srot01_s_01", "S_srot01_7_01", "S_srot01_m_02", "S_srot01_s_02",
    "S_srot01_7_02", "S_srot01_m_03", "S_srot01_s_03", "S_srot01_7_03", "neon_01", "neon_03",
    "neon_02", "S_srot02_dram_01", "S_srot02_dram_02", "S_srot02_dram_03", "S_srot02_reber",
    "A_srot02", "S_srot02_m_01", "S_srot02_s_01", "S_srot02_7_01", "S_srot02_m_02", "S_srot02_s_02",
    "S_srot02_7_02", "S_srot02_m_03", "S_srot02_s_03", "S_srot02_7_03", "neon_04", "neon_06",
    "neon_05",
};

// TODO: type
u32 slotevttbl[2] = {
    (u32)&slot1_set, (u32)&slot2_set,
};

// TODO: type
u32 slotgara[48] = {
    0x00000000, 0x00000000, 0x00000002, 0x00000000,
    0x00000001, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000002,
    0x00000001, 0x00000000, 0x00000000, 0x00000002,
    0x00000000, 0x00000000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000000,
    0x00000001, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000002,
    0x00000001, 0x00000000, 0x00000000, 0x00000002,
    0x00000000, 0x00000000, 0x00000001, 0x00000000,
};

// TODO: type
u32 upsidegara[48] = {
    0x00000000, 0x00000000, 0x00000002, 0x00000000,
    0x00000001, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000002,
    0x00000001, 0x00000000, 0x00000000, 0x00000002,
    0x00000000, 0x00000000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000000,
    0x00000001, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000001,
    0x00000000, 0x00000001, 0x00000000, 0x00000002,
    0x00000001, 0x00000000, 0x00000000, 0x00000002,
    0x00000000, 0x00000000, 0x00000001, 0x00000000,
};

EVT_BEGIN(ranking_set)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, FLOAT(-541.9f), FLOAT(115.5f), FLOAT(-115.0f), FLOAT(-197.0f), FLOAT(56.8f), FLOAT(-115.0f), 600, 11)
    WAIT_MS(600)
    LOOP(0)
        CALL(evt_msg_print, 0, STRING("ranking_mess_00"), 0, 0)
        CALL(evt_msg_select, 0, STRING("ranking_mess_01"))
        CALL(evt_msg_continue)
        IF_INT_EQUAL(LW(0), 5)
            GOTO(0)
        END_IF
        CALL(evt_yuuranking_alloc_10_text_A47C, 0, 0, LW(0), 2, 0, 0, 0, 0)
        WAIT_FRAMES(1)
        CALL(yuuranking_wait_button_10_text_A37C)
        CALL(evt_yuuranking_free_10_text_A3F8)
    END_LOOP
    LABEL(0)
    CALL(evt_cam3d_evt_off, 600, 11)
    WAIT_MS(600)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()
USER_FUNC(evt_exchange_msg_set) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    const char* msg = msgSearch((const char*)evtGetValue(event, args[1]));
    s32 value = evtGetValue(NULL, GSW(0));
    s32 rank = 0;

    if (value >= 0x192) {
        rank = 8;
    } else if (value >= 0x177) {
        rank = 7;
    } else if (value >= 0x158) {
        rank = 6;
    } else if (value >= 0x10b) {
        rank = 5;
    } else if (value >= 0xda) {
        rank = 4;
    } else if (value >= 0xab) {
        rank = 3;
    } else if (value >= 0x75) {
        rank = 2;
    } else if (value >= 0x3d) {
        rank = 1;
    }
    sprintf((char*)exchange_msgp[0], msg, a, exchange_ryokin_medal_tbl[rank][0][1], exchange_ryokin_medal_tbl[rank][0][0],
            exchange_ryokin_medal_tbl[rank][1][1], exchange_ryokin_medal_tbl[rank][1][0], exchange_ryokin_medal_tbl[rank][2][1],
            exchange_ryokin_medal_tbl[rank][2][0]);
    evtSetValue(event, args[2], exchange_msgp[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(exchange_ryokin_medal) {
    s32* args = event->args;
    s32 sel = evtGetValue(event, args[0]);
    s32 value = evtGetValue(NULL, GSW(0));
    s32 rank = 0;

    if (value >= 0x192) {
        rank = 8;
    } else if (value >= 0x177) {
        rank = 7;
    } else if (value >= 0x158) {
        rank = 6;
    } else if (value >= 0x10b) {
        rank = 5;
    } else if (value >= 0xda) {
        rank = 4;
    } else if (value >= 0xab) {
        rank = 3;
    } else if (value >= 0x75) {
        rank = 2;
    } else if (value >= 0x3d) {
        rank = 1;
    }
    evtSetValue(event, args[1], exchange_ryokin_medal_tbl[rank][sel][0]);
    evtSetValue(event, args[2], exchange_ryokin_medal_tbl[rank][sel][1]);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(exchange_set)
    CALL(evt_mario_key_onoff, 0)
    CALL(yuugijou_get_takeoutflag, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(yuugijou_get_montemedal_keep, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(11), 1)
            CALL_CHILD(exchange_return_sub)
            SET_VALUE(LW(0), 0)
        ELSE
            CALL(yuugijou_montemedal_return)
            SET_VALUE(LW(0), 0)
        END_IF
    ELSE
        SET_VALUE(LW(0), 0)
    END_IF
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_cam3d_evt_set, FLOAT(-541.6f), FLOAT(147.4f), FLOAT(-314.6f), FLOAT(-197.0f), FLOAT(86.9f), FLOAT(-314.6f), 600, 11)
        WAIT_MS(600)
    END_IF
    CALL(evt_msg_toge, 2, FLOAT(-147.0f), FLOAT(82.2f), FLOAT(-307.9f))
    IF_INT_EQUAL(GSWF(1225), 0)
        CALL(exchange_ryokin_medal, 0, LW(1), LW(2))
        CALL(evt_msg_print_insert, 0, STRING("exchange_mess_00"), 0, 0, LW(1), LW(2))
        SET_VALUE(GSWF(1225), 1)
    ELSE
        CALL(evt_msg_print, 0, STRING("exchange_mess_01"), 0, 0)
    END_IF
    CALL(evt_exchange_msg_alloc)
    CALL(evt_unitwin_alloc, 0)
    CALL(unitwincoin_disp_onoff, 1)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_exchange_msg_set, LW(0), STRING("exchange_mess_02"), LW(1))
        CALL(evt_msg_select, 1, LW(1))
        IF_INT_EQUAL(LW(0), 3)
            CALL(unitwincoin_disp_onoff, 0)
            CALL(evt_unitwin_free)
            CALL(evt_exchange_msg_free)
            CALL(evt_msg_print_add, 0, STRING("exchange_mess_05"))
            GOTO(0)
        END_IF
        CALL(exchange_ryokin_medal, LW(0), LW(1), LW(2))
        CALL(evt_pouch_get_coin, LW(3))
        IF_INT_LESS(LW(3), LW(1))
            CALL(evt_msg_print_add, 0, STRING("exchange_mess_03"))
            CALL(unitwincoin_disp_onoff, 0)
            CALL(evt_unitwin_free)
            CALL(evt_exchange_msg_free)
            GOTO(0)
        END_IF
        CALL(evt_msg_continue)
        MULTIPLY(LW(1), -1)
        CALL(evt_pouch_add_coin, LW(1))
        CALL(unitwincoin_wait)
        CALL(yuugijou_add_montemedal, 0, LW(2))
        CALL(yuugijou_add_montemedal_moto, LW(2))
        CALL(yuugijou_montemedal_wait)
        WAIT_MS(200)
        CALL(evt_msg_toge, 2, FLOAT(-147.0f), FLOAT(82.2f), FLOAT(-307.9f))
        CALL(evt_msg_print, 0, STRING("exchange_mess_04"), 0, 0)
    END_LOOP
    LABEL(0)
    CALL(evt_cam3d_evt_off, 600, 11)
    WAIT_MS(600)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(roten_4_init)
    CALL(evt_npc_set_position, STRING("me"), 0, -5, 335)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131088)
    RETURN
EVT_END()

EVT_BEGIN(roten_4_regl)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(roten_4_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_016"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_017"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_018"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_019"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_020"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_021"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_022"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_023"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kinopiko_a_init)
    CALL(evt_npc_set_position, STRING("me"), -145, 0, -110)
    RETURN
EVT_END()

EVT_BEGIN(kinopiko_a_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_048"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_049"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_050"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_051"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_052"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_053"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_054"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_055"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(kinopiko_b_init)
    CALL(evt_npc_set_position, STRING("me"), 155, 0, -50)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131088)
    RETURN
EVT_END()

EVT_BEGIN(kinopiko_b_regl)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(kinopiko_b_talk)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(GSW(0))
        CASE_LESS(112)
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_072_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_072"), 0, STRING("me"))
        END_IF
        CASE_LESS(164)
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_073_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_073"), 0, STRING("me"))
        END_IF
        CASE_LESS(215)
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_074_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_074"), 0, STRING("me"))
        END_IF
        CASE_LESS(261)
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_075_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_075"), 0, STRING("me"))
        END_IF
        CASE_LESS(339)
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_076_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_076"), 0, STRING("me"))
        END_IF
        CASE_LESS(373)
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_077_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_077"), 0, STRING("me"))
        END_IF
        CASE_LESS(401)
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_078_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_078"), 0, STRING("me"))
        END_IF
        CASE_DEFAULT
        IF_INT_EQUAL(LW(0), 5)
            CALL(evt_msg_print, 0, STRING("gor_03_079_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_03_079"), 0, STRING("me"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kinopio_a_init)
    CALL(evt_npc_set_position, STRING("me"), 330, 128, -170)
    RETURN
EVT_END()

EVT_BEGIN(kinopio_a_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_024"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_025"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_026"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_027"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_028"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_029"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_030"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_031"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kokinopio_a_init)
    CALL(evt_npc_set_position, STRING("me"), 460, 37, -152)
    RETURN
EVT_END()

EVT_BEGIN(kokinopio_a_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_032"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_033"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_034"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_035"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_036"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_037"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_038"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_039"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kokinopio_b_init)
    CALL(evt_npc_set_position, STRING("me"), 35, -5, 300)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131088)
    RETURN
EVT_END()

EVT_BEGIN(kokinopio_b_regl)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kokinopio_b_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_040"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_041"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_042"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_043"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_044"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_045"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_046"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_047"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_a_init)
    CALL(evt_npc_set_position, STRING("me"), 110, 146, -310)
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_a_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_056"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_057"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_058"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_059"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_060"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_061"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_062"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_063"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kuribo_1_init)
    CALL(evt_npc_set_position, STRING("me"), 470, 10, 295)
    RETURN
EVT_END()

EVT_BEGIN(kuribo_1_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_03_064"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_03_065"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_03_066"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_03_067"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_03_068"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_03_069"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_03_070"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_03_071"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[506] = {
    (u32)"マフィアボス", 0x00080600, 0x00000000, (u32)&maffiaboss_init,
    0x00000000, (u32)&maffiaboss_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"マフィアボスカジノ",
    0x00080600, 0x00000000, (u32)&maffiaboss_casino_init, 0x00000000,
    (u32)&maffiaboss_casino_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"マフィア１", 0x00080600,
    0x00000000, (u32)&maffia1_init, 0x00000000, (u32)&maffia1_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"マフィア２", 0x00080600, 0x00000000,
    (u32)&maffia2_init, 0x00000000, (u32)&maffia2_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"マフィア３", 0x00080600, 0x00000000, (u32)&maffia3_init,
    0x00000000, (u32)&maffia3_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"モニー",
    0x00080600, 0x00000000, (u32)&mony_init, 0x00000000,
    (u32)&mony_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ピートン", 0x00080600,
    0x00000000, (u32)&peton_init, 0x00000000, (u32)&peton_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"グルメボム兵", 0x40000600, 0x00000000,
    (u32)&init_gurume, 0x00000000, (u32)&talk_gurume, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"ルイージ", 0x00080600, 0x00000000, (u32)&luigi_init,
    0x00000000, (u32)&luigi_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"バニーテレサ",
    0x00000600, 0x00000000, (u32)&bteresa_init, 0x00000000,
    (u32)&bteresa_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"客１", 0x00080600,
    0x00000000, (u32)&kyaku1_init, 0x00000000, (u32)&kyaku1_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"客４", 0x00080600, 0x00000000,
    (u32)&kyaku4_init, 0x00000000, (u32)&kyaku4_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"店員", 0x00000200, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ロテン４",
    0x00080600, 0x00000000, (u32)&roten_4_init, (u32)&roten_4_regl,
    (u32)&roten_4_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"キノピオ女Ａ", 0x00080600,
    0x00000000, (u32)&kinopiko_a_init, 0x00000000, (u32)&kinopiko_a_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"キノピオ女Ｂ", 0x00080600, 0x00000000,
    (u32)&kinopiko_b_init, (u32)&kinopiko_b_regl, (u32)&kinopiko_b_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"キノピオＡ", 0x00080600, 0x00000000, (u32)&kinopio_a_init,
    0x00000000, (u32)&kinopio_a_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"キノピオ子供Ａ",
    0x00080600, 0x00000000, (u32)&kokinopio_a_init, 0x00000000,
    (u32)&kokinopio_a_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"キノピオ子供Ｂ", 0x00080600,
    0x00000000, (u32)&kokinopio_b_init, (u32)&kokinopio_b_regl, (u32)&kokinopio_b_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ノコノコＡ", 0x00080600, 0x00000000,
    (u32)&nokonoko_a_init, 0x00000000, (u32)&nokonoko_a_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"クリボー１", 0x00080600, 0x00000000, (u32)&kuribo_1_init,
    0x00000000, (u32)&kuribo_1_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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

static EVT_BEGIN(evt_door_01_00)
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_doa_m"), 6144)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_doa_o"), 6144)
    END_IF
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_i01_doa_m"), 0, LW(0), 0)
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mapobj_rotate, 1, STRING("S_i01_doa_o"), 0, LW(0), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_01)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01uB"), 0, 0, LW(2))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01sB"), 0, 0, LW(3))
    RETURN
EVT_END()

EVT_BEGIN(evt_door_01_10)
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_doa_m"), 6144)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_doa_o"), 6144)
    END_IF
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_i01_doa_o"), 0, LW(0), 0)
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mapobj_rotate, 1, STRING("S_i01_doa_m"), 0, LW(0), 0)
    END_IF
    RETURN
EVT_END()

static char* door_data_01_npc[2] = {
    "店員", NULL,
};

EVT_BEGIN(evt_door_01_0_open)
    SET_VALUE(GW(10), 0)
    SET_VALUE(GW(11), 0)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_mobj_check, STRING("pblk2"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("pblk2"), 16386)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_door_01_0_close)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i01_doa_m"), 6144)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i01_doa_o"), 6144)
    IF_INT_EQUAL(GSW(0), 124)
        CALL(evt_door_end_wait)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_party_stop, 0)
        CALL(evt_msg_print_party, STRING("mac_2_123"))
        SET_VALUE(GSW(0), 125)
        CALL(evt_party_run, 0)
        CALL(evt_mario_key_onoff, 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 279)
        SPAWN_EVENT(majorin_nusumigiki)
    END_IF
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    CALL(evt_mobj_check, STRING("pblk2"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("pblk2"), 16386)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_door_01_1_open)
    CALL(evt_hitobj_onoff, STRING("A_ie_03A"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_gaitou"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_kabe_01"), 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_soto"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_04_soto"), 1)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア３"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィアボスカジノ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("マジョリン"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("グルメボム兵"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("ルイージ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("バニーテレサ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("客１"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("客４"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("ロテン４"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("キノピオ女Ａ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("キノピオ女Ｂ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("キノピオＡ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("キノピオ子供Ａ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("キノピオ子供Ｂ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("ノコノコＡ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("クリボー１"), 128)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_mobj_check, STRING("pblk2"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("pblk2"), 16386)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_door_01_1_close)
    CALL(evt_hitobj_onoff, STRING("A_ie_03A"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_gaitou"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_kabe_01"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_soto"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_04_soto"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア３"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィアボスカジノ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("マジョリン"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("グルメボム兵"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("ルイージ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("バニーテレサ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("客１"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("客４"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("ロテン４"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピオ女Ａ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピオ女Ｂ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピオＡ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピオ子供Ａ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピオ子供Ｂ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("ノコノコＡ"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("クリボー１"), 128)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i01_doa_m"), 6144)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i01_doa_o"), 6144)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    CALL(evt_mobj_check, STRING("pblk2"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("pblk2"), 16386)
    END_IF
    RETURN
EVT_END()

// TODO: type
u32 door_data_01_0[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_i01_doa_m",
    (u32)"A_ie01_doa_m02", (u32)"A_ie01_doa_m01", (u32)"S_ie_01_soto", 0x00000000,
    (u32)"S_ie_01_naka", (u32)"A_ie_01C", (u32)&evt_door_01_00, 0x00000000,
    (u32)&evt_wall_01, (u32)&door_data_01_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_01_0_open, (u32)&evt_door_01_0_close,
};

// TODO: type
u32 door_data_01_1[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_i01_doa_o",
    (u32)"A_ie01_doa_o02", (u32)"A_ie01_doa_o01", (u32)"S_ie_01_soto", 0x00000000,
    (u32)"S_ie_01_naka", (u32)"A_ie_01C", (u32)&evt_door_01_10, 0x00000000,
    (u32)&evt_wall_01, (u32)&door_data_01_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_01_1_open, (u32)&evt_door_01_1_close,
};

EVT_BEGIN(evt_door_01_entry)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_door_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(3), 0, PTR(evt_door_open), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_02_00)
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_02_f1_doa01"), 6144)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_02_f1_doa02"), 6144)
    END_IF
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.6f))
    MULTIPLY_FLOAT(LW(3), FLOAT(-0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f1_doa01"), 0, LW(2), 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f1_doa02"), 0, LW(3), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_0)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f1B"), 0, 0, LW(2))
    RETURN
EVT_END()

char* door_data_02_0_npc[6] = {
    "バニーテレサ", "客１", "客４", "キノピオ女Ａ", "マフィアボスカジノ", NULL,
};

// Matches with a switch on the negated key check (an if/else if-converts; the switch keeps cntlzw/srwi.).
static USER_FUNC(mario_keyoff_wait) {
    switch (!marioChkKey()) {
    case 0:
        break;
    default:
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_YIELD;
}

// NONMATCHING (96.89%): byte-offset arithmetic aligned the strength reduction; only the chosen
// callee-saved register numbers (data r31 vs r17, temporaries) and pool symbol names differ.
USER_FUNC(goryuugijo_door_init_slot) {
    s32* args = event->args;
    GoryuugijoWork* work = (GoryuugijoWork*)evtGetValue(event, args[0]);
    u8* data = (u8*)work->data;
    s32 i;
    s32 j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            f32* angle = (f32*)(data + i * 0xc + j * 4 + 0xc);

            *angle = 0.0f;
            if (*angle >= 360.0f) {
                *angle -= 360.0f;
            }
            *angle = 45.0f * irand(8);
            mapObjRotate(drumname[i][j], 0.0f, 0.0f, *angle);
            *(u8*)(data + i * 3 + j + 0x33) = *(u32*)((unsigned char*)upsidegara + i * 0x60 + j * 0x20 + irand(8) * 4);
            mapGrpFlagOn(drumname[i][j * 3 + 5], 1);
            mapGrpFlagOn(drumname[i][j * 3 + 6], 1);
            mapGrpFlagOn(drumname[i][j * 3 + 7], 1);
        }
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(door_data_02_0_open_sub)
    WAIT_FRAMES(1)
    CALL(goryuugijo_get_dataadr, 1, LW(0))
    CALL(goryuugijo_door_init_slot, LW(0))
    CALL(mario_keyoff_wait)
    RETURN
EVT_END()
USER_FUNC(teresa_badge_item_list_init) {
    badgeShop_bteresaGeneration();
    return EVT_RETURN_DONE;
}

EVT_BEGIN(door_data_02_0_open)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i02_soto_kai"), 1)
    SPAWN_EVENT(door_data_02_0_open_sub)
    SET_VALUE(GSWF(1227), 0)
    CALL(yuugijou_set_yupflag, 0)
    CALL(teresa_badge_item_list_init)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_STG0_CSN1"))
    CALL(evt_snd_envoff, 1024)
    RETURN
EVT_END()

EVT_BEGIN(door_data_02_0_close)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_02_f1_doa01"), 6144)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_02_f1_doa02"), 6144)
    CALL(yuugijou_get_takeoutflag, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
    END_IF
    CALL(goryuugijo_get_dataadr, 1, LW(0))
    CALL(goryuugijo_set_slot_phase, LW(0), 11)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(mario_keyoff_wait)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i02_soto_kai"), 1)
    RETURN
EVT_END()

char* door_data_02_mobj[2] = {
    "kururin1", NULL,
};

// TODO: type
u32 door_data_02_0l[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_02_f1_doa01",
    (u32)"A_ie_02_f1_doa_l02______", (u32)"A_ie_02_f1_doa_l01", (u32)"S_ie_02_soto", 0x00000000,
    (u32)"S_i02_f1", (u32)"A_ie_02C", (u32)&evt_door_02_00, 0x00000000,
    (u32)&evt_wall_02_0, (u32)&door_data_02_0_npc, 0x00000000, 0x00000000,
    (u32)&door_data_02_0_open, (u32)&door_data_02_0_close,
};

// TODO: type
u32 door_data_02_0r[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_02_f1_doa02",
    (u32)"A_ie_02_f1_doa_r02", (u32)"A_ie_02_f1_doa_r01", (u32)"S_ie_02_soto", 0x00000000,
    (u32)"S_i02_f1", (u32)"A_ie_02C", (u32)&evt_door_02_00, 0x00000000,
    (u32)&evt_wall_02_0, (u32)&door_data_02_0_npc, (u32)&door_data_02_mobj, 0x00000000,
    (u32)&door_data_02_0_open, (u32)&door_data_02_0_close,
};

EVT_BEGIN(evt_door_02_10)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2_doa"), 0, LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_02_11)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2_doa"), 0, 0, LW(2))
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_1)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2sB"), 0, 0, LW(2))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2uB"), 0, 0, LW(3))
    RETURN
EVT_END()

char* door_data_02_1_npc[6] = {
    "マフィアボス", "マフィア１", "マフィア２", "モニー", "ピートン", NULL,
};
USER_FUNC(get_mario_dispReqDir) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    evtSetValue(event, args[0], (s32) * (f32*)((u8*)mario + 0x1AC));
    return EVT_RETURN_DONE;
}

USER_FUNC(desk_talk_func) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 b = evtGetValue(event, args[1]);
    s32 x = evtGetValue(event, args[2]);
    s32 y = evtGetValue(event, args[3]);
    s32 z = evtGetValue(event, args[4]);

    if (a == 1 && b == 0) {
        eff[0] = effMarioBalloonEntry(1, npc[0]);
    }
    if (a == 0 && b == 1) {
        effDelete(eff[0]);
    }
    npc[0]->position.x = (f32)x;
    npc[0]->position.y = (f32)y;
    npc[0]->position.z = (f32)z;
    return EVT_RETURN_DONE;
}

USER_FUNC(desk_talk_init_func) {
    npc[0] = (NpcEntry*)_mapAlloc(0x340);
    eff[0] = (EffectEntry*)_mapAlloc(0x28);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(desk_talk_check)
    CALL(desk_talk_init_func)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(15), 0)
    LOOP(0)
        CALL(evt_mario_get_name_hitobj_front, LW(10))
        SET_VALUE(LF(10), 0)
        SWITCH(GSW(0))
            CASE_LESS(268)
            CALL(evt_npc_get_height, STRING("マフィアボス"), LW(5))
            CALL(evt_npc_get_position, STRING("マフィアボス"), LW(2), LW(3), LW(4))
            SET_VALUE(LW(11), STRING("A_i02_f2_set00C"))
            IF_STR_EQUAL(LW(10), LW(11))
                SET_VALUE(LF(10), 1)
            END_IF
            CASE_LESS(282)
            CALL(evt_npc_get_height, STRING("マフィアボス"), LW(5))
            CALL(evt_npc_get_position, STRING("マフィアボス"), LW(2), LW(3), LW(4))
            SET_VALUE(LW(11), STRING("A_i02_bed_l"))
            IF_STR_EQUAL(LW(10), LW(11))
                SET_VALUE(LF(10), 1)
            END_IF
            SET_VALUE(LW(11), STRING("A_i02_bed_m"))
            IF_STR_EQUAL(LW(10), LW(11))
                SET_VALUE(LF(10), 1)
            END_IF
            CASE_DEFAULT
            CALL(evt_npc_get_height, STRING("ピートン"), LW(5))
            CALL(evt_npc_get_position, STRING("ピートン"), LW(2), LW(3), LW(4))
            SET_VALUE(LW(11), STRING("A_i02_f2_set00C"))
            IF_STR_EQUAL(LW(10), LW(11))
                SET_VALUE(LF(10), 1)
            END_IF
        END_SWITCH
        SET_VALUE(LW(0), 0)
        IF_INT_EQUAL(LF(10), 1)
            IF_INT_EQUAL(LSWF(3), 0)
                CALL(get_mario_dispReqDir, LW(9))
                CALL(evt_mario_party_use_check, LW(13))
                CALL(evt_mario_get_name_hitobj_front, LW(14))
                IF_INT_NOT_EQUAL(LW(13), 4)
                    IF_STR_EQUAL(LW(14), STRING("A_i02_bed_m"))
                        IF_INT_EQUAL(LW(9), 0)
                            SET_VALUE(LW(0), 1)
                            SET_VALUE(LW(15), 5)
                        END_IF
                    END_IF
                    IF_INT_EQUAL(LW(9), 180)
                        SET_VALUE(LW(0), 1)
                        SET_VALUE(LW(15), 5)
                    END_IF
                END_IF
            END_IF
        END_IF
        IF_INT_NOT_EQUAL(LW(15), 0)
            SET_VALUE(LW(0), 1)
            ADD(LW(15), -1)
        END_IF
        ADD(LW(3), LW(5))
        CALL(desk_talk_func, LW(0), LW(1), LW(2), LW(3), LW(4))
        SET_VALUE(LW(1), LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_door_02_1_open)
    SET_VALUE(GF(5), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(1600)
        IF_INT_NOT_EQUAL(GSW(0), 278)
            CALL(evt_snd_bgmoff, 1024)
            CALL(evt_snd_bgmon, 1, STRING("BGM_STG0_CSN1"))
            CALL(evt_snd_envoff, 1024)
        ELSE
            CALL(evt_snd_bgmoff_f, 512, 2000)
            CALL(evt_snd_envoff, 1024)
        END_IF
    END_INLINE_EVENT
    SWITCH(GSW(0))
        CASE_LESS(268)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_02_bed"), 1)
        CALL(evt_hitobj_onoff, STRING("A_i02_bed"), 1, 0)
        CASE_LESS(279)
        CALL(evt_npc_set_anim, STRING("マフィアボス"), STRING("S_2"))
        IF_INT_EQUAL(GSW(0), 278)
            CALL(evt_mario_dispflag_onoff, 1, 4)
            CALL(evt_mario_dispflag_onoff, 1, 2048)
            CALL(evt_door_end_wait)
            CALL(evt_mario_dispflag_onoff, 0, 2048)
            CALL(evt_mario_dispflag_onoff, 0, 4)
            CALL(evt_mario_set_dir, 90, 0, 0)
            CALL_CHILD(maffiaboss_train_ticket_init)
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i02_f2_set00C"), 1)
        CALL(evt_hitobj_onoff, STRING("Ai02_tke"), 1, 0)
        CASE_LESS(282)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i02_f2_set00C"), 1)
        CALL(evt_hitobj_onoff, STRING("Ai02_tke"), 1, 0)
        CASE_DEFAULT
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_02_bed"), 1)
        CALL(evt_hitobj_onoff, STRING("A_i02_bed"), 1, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(evt_door_02_1_close)
    IF_INT_NOT_EQUAL(GSW(0), 278)
        IF_INT_EQUAL(GF(5), 0)
            CALL(evt_snd_bgmoff, 513)
            CALL(evt_snd_bgmon, 288, 0)
            CALL(evt_snd_envoff, 513)
            CALL(evt_snd_envon, 288, 0)
        ELSE
            CALL(evt_snd_bgmon, 160, 0)
            CALL(evt_snd_envoff, 513)
            CALL(evt_snd_envon, 288, 0)
        END_IF
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
        CALL(evt_snd_envoff, 513)
        CALL(evt_snd_envon, 288, 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
u32 door_data_02_1[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_02_f2_doa",
    (u32)"A_ie_02_f2_doa02", (u32)"A_ie_02_f2_doa01", (u32)"S_ie_02_soto", 0x00000000,
    (u32)"S_i02_f2", (u32)"A_ie_02C", (u32)&evt_door_02_10, (u32)&evt_door_02_11,
    (u32)&evt_wall_02_1, (u32)&door_data_02_1_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_02_1_open, (u32)&evt_door_02_1_close,
};

static EVT_BEGIN(evt_door_03_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_03)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(-0.5f))
    MULTIPLY_FLOAT(LW(3), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_i03_ueB"), 0, 0, LW(2))
    CALL(evt_mapobj_rotate, 1, STRING("S_i03_sitaB"), 0, 0, LW(3))
    RETURN
EVT_END()

static char* door_data_03_npc[2] = {
    "ノコノコＡ", NULL,
};

EVT_BEGIN(door_data_03_open)
    CALL(evt_hitobj_onoff, STRING("A_ie_04A"), 1, 0)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(door_data_03_close)
    CALL(evt_hitobj_onoff, STRING("A_ie_04A"), 1, 1)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
static u32 door_data_03[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_03_doa",
    (u32)"A_ie_03_doa02", (u32)"A_ie_03_doa01", (u32)"S_ie_03_soto", 0x00000000,
    (u32)"S_ie_03_naka", (u32)"S_ie_03_C", (u32)&evt_door_03_00, 0x00000000,
    (u32)&evt_wall_03, (u32)&door_data_03_npc, 0x00000000, 0x00000000,
    (u32)&door_data_03_open, (u32)&door_data_03_close,
};

static EVT_BEGIN(evt_door_03_caseopen)
    CALL_CHILD(evt_door_open_roll)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_03_open)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set_xyz, 193, 30, -396, 0, 150, 11)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_mario_mov_pos2, 275, LW(12), FLOAT(60.0f))
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d_ex, STRING("SFX_STG3_DRAWER_OPEN1"), LW(10), LW(11), LW(12), 88, 255, 0, 16, 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -30, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(10), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_i03_tan_hiki"), LW(10), 0, 0)
        CALL(evt_hit_bind_update, STRING("A_i03_tan_hiki"))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_mario_mov_pos2, 245, LW(12), FLOAT(60.0f))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_snd_sfxon_3d_ex, STRING("SFX_STG3_DRAWER_OPEN1"), LW(10), LW(11), LW(12), 88, 255, 0, 16, 0)
        CALL(evt_sub_intpl_msec_init, 11, -30, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(10), LW(1))
            CALL(evt_mapobj_trans, 1, STRING("S_i03_tan_hiki"), LW(10), 0, 0)
            CALL(evt_hit_bind_update, STRING("A_i03_tan_hiki"))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_jump_pos, 183, 20, -394, 500, 0, -1000)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    CALL(evt_run_case_evt, 8, 1, LW(3), 0, PTR(evt_door_03_caseopen), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bero_n"), 1)
    CALL(evt_hitobj_onoff, STRING("A_ie_04A"), 1, 0)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_03_close)
    CALL(evt_hitobj_onoff, STRING("A_ie_04A"), 1, 1)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
static u32 door_data_03_roll[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_03_doa",
    0x00000000, (u32)"A_ie_03_rol", (u32)"S_ie_03_soto", 0x00000000,
    (u32)"S_ie_03_naka", (u32)"S_ie_03_C", (u32)&evt_door_03_00, 0x00000000,
    (u32)&evt_wall_03, (u32)&door_data_03_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_03_open, (u32)&evt_door_03_close,
};

static EVT_BEGIN(evt_door_03_entry)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    SET_VALUE(LW(15), 0)
    CALL(evt_run_case_evt, 8, 1, LW(3), 0, PTR(evt_door_03_caseopen), 0)
    CALL(evt_hit_bind_mapobj, STRING("A_i03_tan_hiki"), STRING("S_i03_tan_hiki"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_04)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    MULTIPLY_FLOAT(LW(3), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_i04_ueB"), 0, 0, LW(2))
    CALL(evt_mapobj_rotate, 1, STRING("S_i04_stB"), 0, 0, LW(3))
    RETURN
EVT_END()

static char* door_data_04_npc[3] = {
    "キノピオＡ", "キノピオ子供Ａ", NULL,
};

static EVT_BEGIN(evt_door_04_open)
    CALL(evt_hitobj_onoff, STRING("A_ie_03A"), 1, 0)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_04_close_snd)
    CALL(evt_hitobj_onoff, STRING("A_ie_03A"), 1, 1)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
static u32 door_data_04[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_04_doa",
    (u32)"A_ie_04_doa02", (u32)"A_ie_04_doa01", (u32)"S_ie_04_soto", 0x00000000,
    (u32)"S_ie_04_naka", (u32)"A_ie_04_C", (u32)&evt_door_04_00, 0x00000000,
    (u32)&evt_wall_04, (u32)&door_data_04_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_04_open, (u32)&evt_door_04_close_snd,
};

EVT_BEGIN(evt_door_04_caseclose)
    CALL_CHILD(evt_door_close_roll)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_close)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bero_n"), 1)
    CALL(evt_cam3d_evt_set_xyz, 328, 37, -395, 0, 150, 11)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_mario_mov_pos2, LW(10), -400, FLOAT(60.0f))
    CALL(evt_mario_wait_landon)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    SET_VALUE(LW(15), 0)
    CALL(evt_run_case_evt, 8, 1, LW(2), 0, PTR(evt_door_04_caseclose), 0)
    CALL(evt_hitobj_onoff, STRING("A_ie_03A"), 1, 1)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door_data_04_roll[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_04_doa",
    (u32)"A_ie_04_rol", 0x00000000, (u32)"S_ie_04_soto", 0x00000000,
    (u32)"S_ie_04_naka", (u32)"A_ie_04_C", (u32)&evt_door_04_00, 0x00000000,
    (u32)&evt_wall_04, (u32)&door_data_04_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_04_open, (u32)&evt_door_04_close,
};

static EVT_BEGIN(evt_door_04_entry)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    SET_VALUE(LW(15), 0)
    CALL(evt_run_case_evt, 8, 1, LW(2), 0, PTR(evt_door_04_caseclose), 0)
    RETURN
EVT_END()

EVT_BEGIN(wall_bomb_init)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_after_01"), 1)
    IF_INT_EQUAL(GSWF(1224), 0)
        CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_before1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_before"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_after_01"), 1)
        CALL(evt_hitobj_onoff, STRING("A_ana_ana"), 1, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wall_bomb)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, STRING("S_ana_before"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_hitobj_onoff, STRING("A_ana_ana"), 1, 0)
    SET_VALUE(GSWF(1224), 1)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_ana_before"), STRING("offscreen"))
    CALL(evt_offscreen_get_boundingbox2, STRING("offscreen"), LW(10), LW(11), LW(12), LW(13))
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(12), 0)
        IF_INT_EQUAL(LW(13), 0)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 1, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_img_clear_virtual_point, STRING("capture_img"))
        CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_after_01"), 1)
        CALL(evt_img_wait_animend, STRING("capture_img"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_after_01"), 1)
    END_IF
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_ana_before"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    RETURN
EVT_END()

EVT_BEGIN(uraguchi_tenin)
    CALL(evt_npc_move_position, STRING("店員"), -490, 24, 0, FLOAT(60.0f), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("店員"), 1024)
    CALL(evt_npc_set_anim, STRING("店員"), STRING("S_1"))
    CALL(evt_npc_set_ry, STRING("店員"), 0)
    RETURN
EVT_END()

EVT_BEGIN(uraguchi_event)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_envoff, 1024)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("店員"), 1073741856)
    CALL(evt_msg_print, 0, STRING("mac_2_041"), 0, STRING("店員"))
    CALL(evt_msg_select, 0, STRING("mac_2_041_select"))
    CALL(evt_msg_continue)
    CALL(evt_msg_print, 0, STRING("mac_2_042"), 0, STRING("店員"))
    CALL(evt_msg_select, 0, STRING("mac_2_041_select"))
    CALL(evt_msg_continue)
    IF_INT_NOT_EQUAL(LW(0), 2)
        CALL(evt_msg_print, 0, STRING("mac_2_044"), 0, STRING("店員"))
        CALL(evt_npc_flag_onoff, 0, STRING("店員"), 1073741856)
        SET_VALUE(GW(10), 0)
        SET_VALUE(GW(11), 0)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        CALL(evt_snd_envoff, 513)
        CALL(evt_snd_envon, 288, 0)
        RETURN
    END_IF
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    CALL(evt_cam3d_evt_set, -873, 124, 6, -398, 53, 6, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_043"), 0, STRING("店員"))
    CALL(evt_npc_set_ry, STRING("店員"), 0)
    WAIT_MS(200)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_cam3d_evt_set, -873, 124, -106, -398, 53, -106, 2500, 11)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("店員"), LW(3), LW(4), LW(5))
            IF_INT_GREATER_OR_EQUAL(LW(2), LW(5))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("店員"), LW(3), LW(4), LW(5))
            IF_INT_GREATER_OR_EQUAL(LW(2), LW(5))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("店員"), 1024)
    CALL(evt_npc_status_onoff, 1, STRING("店員"), 2)
    CALL(evt_npc_move_position, STRING("店員"), -486, -195, 0, FLOAT(60.0f), 0)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_anim, STRING("店員"), STRING("T_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("店員"), STRING("S_1"))
    CALL(evt_npc_get_position, STRING("店員"), LW(0), LW(1), LW(2))
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_LOCK_SWING1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_UNLOCK1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("店員"), -486, -165, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("店員"), 180)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_2_043_01"), 0, STRING("店員"))
    SET_VALUE(LW(0), PTR(door_data_01_1))
    CALL_CHILD(evt_door_01_entry)
    SET_VALUE(GSWF(1230), 1)
    CALL(evt_npc_flag_onoff, 0, STRING("店員"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("店員"), 4194304)
    CALL(evt_npc_get_reglid, STRING("店員"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_change_interrupt, STRING("店員"), 1, PTR(uraguchi_tenin))
    CALL(evt_npc_restart_regular_event, STRING("店員"))
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(uraguchi_check)
    IF_INT_EQUAL(GSWF(1230), 1)
        RETURN
    END_IF
    IF_INT_EQUAL(GW(10), 0)
        SET_VALUE(GW(10), LW(0))
    ELSE
        IF_INT_EQUAL(GW(11), 0)
            SET_VALUE(GW(11), LW(0))
        END_IF
    END_IF
    IF_INT_NOT_EQUAL(GW(10), 152)
        RETURN
    END_IF
    IF_INT_NOT_EQUAL(GW(11), 141)
        RETURN
    END_IF
    CALL_CHILD(uraguchi_event)
    RETURN
EVT_END()

static char* obj_list[12] = {
    "S_item_01", "A_item_01", "S_item_02", "A_item_02", "S_item_03", "A_item_03", "S_item_04",
    "A_item_04", "S_item_05", "A_item_05", "S_item_06", "A_item_06",
};

// TODO: type
static u32 goods_list[12] = {
    0x00000095, 0x00000014, 0x00000087, 0x0000000A,
    0x00000098, 0x00000002, 0x00000097, 0x00000032,
    0x0000008D, 0x0000000C, 0x00000080, 0x0000000A,
};

// TODO: type
static u32 trade_list[24] = {
    0x00000095, 0x000A0000, 0x00000087, 0x00050000,
    0x00000098, 0x00010000, 0x00000097, 0x00190000,
    0x0000008D, 0x00060000, 0x00000080, 0x00050000,
    0x000000A5, 0x00400000, 0x000000A6, 0x00020000,
    0x000000AA, 0x00030000, 0x000000AE, 0x00020000,
    0x000000AF, 0x00020000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shopper_data[38] = {
    (u32)"店員", 0x00000000, (u32)&uraguchi_check, (u32)"gor_shop2_00",
    (u32)"gor_shop2_01", (u32)"gor_shop2_02", (u32)"gor_shop2_03", (u32)"gor_shop2_04",
    (u32)"gor_shop2_05", (u32)"gor_shop2_06", (u32)"gor_shop2_07", (u32)"gor_shop2_08",
    (u32)"gor_shop2_09", (u32)"gor_shop2_10", (u32)"gor_shop2_11", (u32)"gor_shop2_12",
    (u32)"gor_shop2_13", (u32)"gor_shop2_14", (u32)"gor_shop2_15", (u32)"gor_shop2_16",
    (u32)"gor_shop2_17", (u32)"gor_shop2_18", (u32)"gor_shop2_19", (u32)"gor_shop2_20",
    (u32)"gor_shop2_21", (u32)"gor_shop2_22", (u32)"gor_shop2_23", (u32)"gor_shop2_24",
    (u32)"gor_shop2_25", (u32)"gor_shop2_26", (u32)"gor_shop2_27", (u32)"gor_shop2_28",
    (u32)"gor_shop2_29", (u32)"gor_shop2_30", (u32)"gor_shop2_31", (u32)"gor_shop2_32",
    (u32)"gor_shop2_33", (u32)"gor_shop2_34",
};
USER_FUNC(dummy_shadow) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    f32 size = evtGetFloat(event, args[3]);

    shadowSetCollision(shadowEntry(x, y, z, 10.0f * size), 0);
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(majorin_nusumigiki)
    CALL(evt_npc_set_ry_lr, STRING("マジョリン"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("マジョリン"), 512)
    SET_FLOAT_VALUE(GW(9), FLOAT(1.0f))
    BEGIN_INLINE_EVENT
        CALL(dummy_shadow, -250, 0, 125, GW(9))
    END_INLINE_EVENT
    CALL(evt_door_end_wait)
    CALL(evt_cam3d_evt_set, -425, 134, 733, -425, 38, 192, 500, 11)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            DEBUG_PUT_REG(LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_party_stop, 0)
        CALL(evt_party_flg_onoff, 0, 16777216, 1)
        CALL(evt_party_move_pos2, 0, -450, 200, FLOAT(120.0f))
        CALL(evt_party_flg_onoff, 0, 16777216, 0)
        CALL(evt_party_set_dir, 0, 90, 200)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -400, 200, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 270, 200, 0)
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print_party, STRING("mac_5_900"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 3, -250, 0, 120)
    CALL(evt_msg_print, 0, STRING("mac_5_901"), 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        LOOP(3)
            CALL(evt_party_set_dir, 0, 270, 200)
            WAIT_MS(200)
            CALL(evt_party_set_dir, 0, 90, 200)
            WAIT_MS(200)
        END_LOOP
    END_INLINE_EVENT
    LOOP(3)
        CALL(evt_mario_set_dir, 270, 200, 0)
        WAIT_MS(200)
        CALL(evt_mario_set_dir, 90, 200, 0)
        WAIT_MS(200)
    END_LOOP
    CALL(evt_cam3d_evt_set, -250, 69, 350, -250, 30, 128, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_set_pos, -350, 0, 200)
    CALL(evt_party_set_pos, 0, -390, 0, 200)
    CALL(evt_msg_toge, 2, -250, 0, 120)
    CALL(evt_msg_print, 0, STRING("mac_5_902"), 0, 0)
    SET_FLOAT_VALUE(GW(9), FLOAT(0.0f))
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_WITCH1"))
    CALL(evt_npc_majo_disp_on, STRING("マジョリン"), -250, 0, 125, 0)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -315, 114, 586, -315, 45, 192, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_5_903"))
    CALL(evt_cam3d_evt_set, -250, 69, 350, -250, 30, 128, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_904"), 0, STRING("マジョリン"))
    CALL(evt_npc_majo_disp_off, STRING("マジョリン"))
    CALL(evt_npc_set_position, STRING("マジョリン"), 0, -1000, 0)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -370, 88, 463, -370, 40, 192, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_5_905"))
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 280)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(yuureturn_rtn)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
    CALL(evt_snd_envon_f, 272, STRING("ENV_STG0_GOR4"), 0)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_snd_bgmoff_f, 1024, 0)
    CALL(evt_snd_envoff_f, 1024, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(yuugijou_montemedal_disp_onoff, 1)
    SET_VALUE(LW(0), PTR(door_data_02_0r))
    SPAWN_EVENT(evt_door_open_quick)
    SET_VALUE(LW(0), PTR(door_data_02_0l))
    SPAWN_EVENT(evt_door_open_quick)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    SUBTRACT(LW(5), 40)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP2"), LW(3), LW(4), LW(5), 0)
    CALL(evt_mario_jump_pos, LW(3), 0, LW(5), 500, 0, 20)
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_LANDING1"), LW(3), LW(4), LW(5), 0)
    WAIT_MS(500)
    CALL_CHILD(gor_03_dokan_down_evt)
    WAIT_MS(500)
    IF_INT_LESS(GSW(0), 403)
        CALL(evt_msg_print, 0, STRING("bunny_teresa_17"), 0, STRING("バニーテレサ"))
    ELSE
        CALL(evt_msg_print, 0, STRING("bunny_teresa_17_1"), 0, STRING("バニーテレサ"))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(peach_mail)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(2), -300)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(0), -50)
        CALL(evt_mario_mov_pos, -50, LW(2), 500)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(0), 50)
        CALL(evt_mario_mov_pos, 50, LW(2), 500)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION1"))
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 40)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
        CALL(evt_party_set_dir, 0, 270, -1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print_party, STRING("gor_03_mail3_00"))
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
    CALL(evt_cam3d_evt_set, LW(0), 91, -11, LW(0), 39, -307, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_msg_print, 0, STRING("gor_03_mail3_01"), 0, 0)
    CALL(evt_msg_print_party, STRING("gor_03_mail3_02"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 173)
    CALL(evt_pouch_receive_mail, 2)
    CALL(evt_pouch_open_mail, 2)
    RETURN
EVT_END()

EVT_BEGIN(kuzi_bomb_talk_normal)
    CALL(evt_unitwin_alloc, 0)
    CALL(evt_cam3d_evt_set, -150, 120, 890, -150, 50, 470, 800, 11)
    WAIT_MS(800)
    IF_INT_EQUAL(GSWF(1237), 0)
        SET_VALUE(GSWF(1237), 1)
        CALL(evt_msg_print, 0, STRING("kuji_bomb_000"), 0, STRING("グルメボム兵"))
        CALL(evt_msg_select, 0, STRING("kuji_bomb_001"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_002"))
            CALL(unitwincoin_disp_onoff, 1)
            CALL(evt_msg_select, 0, STRING("kuji_bomb_003"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_pouch_get_coin, LW(0))
                IF_INT_LESS(LW(0), 10)
                    CALL(unitwincoin_disp_onoff, 0)
                    CALL(evt_msg_print_add, 0, STRING("kuji_bomb_022_1"))
                ELSE
                    CALL(evt_pouch_add_coin, -10)
                    CALL(unitwincoin_wait)
                    CALL(unitwincoin_disp_onoff, 0)
                    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_2"), STRING("T_2"))
                    CALL(evt_msg_print_add, 0, STRING("kuji_bomb_004"))
                    CALL(evt_lottery_buy, LW(0))
                    CALL(evt_item_entry, STRING("kuzi"), 106, 0, -1000, 0, 17, -1, 0)
                    CALL(evt_item_get_item, STRING("kuzi"))
                    CALL(evt_item_get_item_end_wait, STRING("kuzi"))
                    CALL(evt_msg_print, 0, STRING("kuji_bomb_006"), 0, STRING("グルメボム兵"))
                END_IF
            ELSE
                CALL(unitwincoin_disp_onoff, 0)
                CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_4"), STRING("T_4"))
                CALL(evt_msg_print_add, 0, STRING("kuji_bomb_007"))
            END_IF
        ELSE
            CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_4"), STRING("T_4"))
            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_007"))
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("kuji_bomb_008"), 0, STRING("グルメボム兵"))
        LABEL(0)
        CALL(evt_msg_select, 0, STRING("kuji_bomb_009"))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_pouch_check_item, 106, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print_add, 0, STRING("kuji_bomb_010"))
                CALL(unitwincoin_disp_onoff, 1)
                CALL(evt_msg_select, 0, STRING("kuji_bomb_011"))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(evt_pouch_get_coin, LW(0))
                    IF_INT_LESS(LW(0), 10)
                        CALL(unitwincoin_disp_onoff, 0)
                        CALL(evt_msg_print_add, 0, STRING("kuji_bomb_022_1"))
                    ELSE
                        CALL(evt_pouch_add_coin, -10)
                        CALL(unitwincoin_wait)
                        CALL(unitwincoin_disp_onoff, 0)
                        CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_2"), STRING("T_2"))
                        CALL(evt_msg_print_add, 0, STRING("kuji_bomb_012"))
                        CALL(evt_lottery_buy, LW(0))
                        CALL(evt_item_entry, STRING("kuzi"), 106, 0, -1000, 0, 17, -1, 0)
                        CALL(evt_item_get_item, STRING("kuzi"))
                        CALL(evt_item_get_item_end_wait, STRING("kuzi"))
                        CALL(evt_msg_print, 0, STRING("kuji_bomb_014"), 0, STRING("グルメボム兵"))
                    END_IF
                ELSE
                    CALL(unitwincoin_disp_onoff, 0)
                    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_4"), STRING("T_4"))
                    CALL(evt_msg_print_add, 0, STRING("kuji_bomb_022"))
                END_IF
            ELSE
                CALL(evt_msg_print_add, 0, STRING("kuji_bomb_015"))
                CALL(unitwincoin_disp_onoff, 1)
                CALL(evt_msg_select, 0, STRING("kuji_bomb_016"))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(evt_pouch_get_coin, LW(0))
                    IF_INT_LESS(LW(0), 100)
                        CALL(unitwincoin_disp_onoff, 0)
                        CALL(evt_msg_print_add, 0, STRING("kuji_bomb_022_1"))
                    ELSE
                        CALL(evt_pouch_add_coin, -100)
                        CALL(unitwincoin_wait)
                        CALL(unitwincoin_disp_onoff, 0)
                        CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_2"), STRING("T_2"))
                        CALL(evt_msg_print_add, 0, STRING("kuji_bomb_017"))
                        CALL(evt_lottery_buy, LW(0))
                        CALL(evt_pouch_remove_item, 106, LW(0))
                        CALL(evt_item_entry, STRING("kuzi"), 106, 0, -1000, 0, 17, -1, 0)
                        CALL(evt_item_get_item, STRING("kuzi"))
                        CALL(evt_item_get_item_end_wait, STRING("kuzi"))
                        CALL(evt_msg_print, 0, STRING("kuji_bomb_019"), 0, STRING("グルメボム兵"))
                    END_IF
                ELSE
                    CALL(unitwincoin_disp_onoff, 0)
                    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_4"), STRING("T_4"))
                    CALL(evt_msg_print_add, 0, STRING("kuji_bomb_022"))
                END_IF
            END_IF
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_020"))
            CALL(evt_msg_print, 0, STRING("kuji_bomb_021"), 0, STRING("グルメボム兵"))
            GOTO(0)
            CASE_EQUAL(2)
            CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_4"), STRING("T_4"))
            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_022"))
        END_SWITCH
    END_IF
    CALL(evt_unitwin_free)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_1"), STRING("T_1"))
    RETURN
EVT_END()
void kuji_bomb_055_callback(s32 type, s32* param) {
    NpcEntry* npc;
    u8* p;

    p = (u8*)param[0];
    npc = npcNameToPtr("グルメボム兵");

    switch (type) {
    case 3:
        if (*(u16*)(p + 8) == 1) {
            evtSetValue(NULL, GF(0), 1);
        }
        break;
    case 5:
        switch (*(s32*)(p + 0x10)) {
        case 0x1d:
            animPoseSetAnim(npc->poseId, "T_3", 1);
            break;
        case 0xc8:
            animPoseSetAnim(npc->poseId, "T_4", 1);
            break;
        case 0x158:
            animPoseSetAnim(npc->poseId, "T_3", 1);
            break;
        case 0x1f1:
            animPoseSetAnim(npc->poseId, "T_4", 1);
            break;
        case 0x2b7:
        case 0x2b8:
            npc->flags &= ~0x200;
            animPoseSetAnim(npc->poseId, "B_1", 1);
            break;
        }
        break;
    }
}

static USER_FUNC(viewport) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]);
    f32 b = evtGetFloat(event, args[1]);
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);
    CameraEntry* cam;

    cam = camGetPtr(3);
    cam->viewport[0] = a;
    cam->viewport[1] = b;
    cam->viewport[2] = c;
    cam->viewport[3] = d;
    cam = camGetPtr(4);
    cam->viewport[0] = a;
    cam->scissor[0] = a;
    cam->viewport[1] = b;
    cam->scissor[1] = b;
    cam->viewport[2] = c;
    cam->scissor[2] = c;
    cam->viewport[3] = d;
    cam->scissor[3] = d;
    cam = camGetPtr(8);
    cam->viewport[0] = a;
    cam->viewport[1] = b;
    cam->viewport[2] = c;
    cam->viewport[3] = d;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(kuzi_bomb_talk_husei)
    CALL(evt_unitwin_alloc, 0)
    CALL(evt_lottery_flag, LW(0))
    IF_BITS_SET(LW(0), 16)
        SET_VALUE(LW(0), 0)
    ELSE
        SET_VALUE(LW(0), 1)
    END_IF
    IF_INT_EQUAL(GSWF(257), 1)
        SET_VALUE(LW(0), 1)
    ELSE
        SET_VALUE(GSWF(257), 1)
    END_IF
    CALL(evt_cam3d_evt_set, -150, 120, 890, -150, 50, 470, 800, 11)
    WAIT_MS(800)
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_lottery_flag, LW(0))
        IF_BITS_SET(LW(0), 8)
            SET_VALUE(LW(0), 1)
        ELSE
            SET_VALUE(LW(0), 0)
        END_IF
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("kuji_bomb_061"), 0, STRING("グルメボム兵"))
            GOTO(0)
        ELSE
            CALL(evt_msg_print, 0, STRING("kuji_bomb_062"), 0, STRING("グルメボム兵"))
            GOTO(99)
        END_IF
    END_IF
    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_4"), STRING("T_4"))
    CALL(evt_msg_print, 0, STRING("kuji_bomb_037"), 0, STRING("グルメボム兵"))
    CALL(evt_mario_set_pose, STRING("M_N_1"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("kuji_bomb_038"), 0, STRING("グルメボム兵"))
    CALL(evt_msg_select, 0, STRING("kuji_bomb_039"))
    CALL(evt_msg_print_add, 0, STRING("kuji_bomb_040"))
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("kuji_bomb_041"), 0, STRING("グルメボム兵"))
    CALL(evt_msg_select, 0, STRING("kuji_bomb_042"))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(10)
    ELSE
        CALL(evt_msg_print_add, 0, STRING("kuji_bomb_043"))
        CALL(evt_msg_select, 0, STRING("kuji_bomb_044"))
        IF_INT_EQUAL(LW(0), 0)
            GOTO(10)
        ELSE
            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_045"))
            CALL(evt_msg_select, 0, STRING("kuji_bomb_046"))
            IF_INT_EQUAL(LW(0), 0)
                GOTO(10)
            ELSE
                CALL(evt_msg_print_add, 0, STRING("kuji_bomb_047"))
                CALL(evt_msg_select, 0, STRING("kuji_bomb_048"))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(evt_msg_print_add, 0, STRING("kuji_bomb_049"))
                    LABEL(0)
                    CALL(unitwincoin_disp_onoff, 1)
                    CALL(evt_msg_select, 0, STRING("kuji_bomb_050"))
                    IF_INT_EQUAL(LW(0), 0)
                        CALL(evt_pouch_get_coin, LW(0))
                        IF_INT_LESS(LW(0), 500)
                            CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_4"), STRING("T_4"))
                            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_062_1"))
                        ELSE
                            CALL(evt_pouch_add_coin, -500)
                            CALL(unitwincoin_wait)
                            CALL(unitwincoin_disp_onoff, 0)
                            CALL(evt_eff_delete, STRING("husei_huki"))
                            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_051"))
                            CALL(evt_pouch_remove_item, 106, LW(0))
                            CALL(evt_lottery_buy, LW(0))
                            SET_VALUE(GSWF(257), 0)
                            CALL(evt_item_entry, STRING("kuzi"), 106, 0, -1000, 0, 17, -1, 0)
                            CALL(evt_item_get_item, STRING("kuzi"))
                            CALL(evt_item_get_item_end_wait, STRING("kuzi"))
                            CALL(evt_msg_print, 0, STRING("kuji_bomb_053"), 0, STRING("グルメボム兵"))
                        END_IF
                    ELSE
                        CALL(unitwincoin_disp_onoff, 0)
                        CALL(evt_msg_print_add, 0, STRING("kuji_bomb_054"))
                    END_IF
                ELSE
                    GOTO(10)
                END_IF
            END_IF
        END_IF
    END_IF
    GOTO(99)
    LABEL(10)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
        CALL(evt_mario_set_pose, STRING("M_N_7"))
        CALL(evt_snd_sfxon, STRING("SFX_STG8_QUAKE1"), GW(5))
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            CALL(evt_sub_get_stopwatch, LW(0), LW(1))
            IF_INT_GREATER_OR_EQUAL(LW(1), 30000)
                BREAK_LOOP
            END_IF
            IF_INT_NOT_EQUAL(GF(1), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 300)
        END_LOOP
        CALL(evt_snd_sfxoff, GW(5))
    END_INLINE_EVENT
    CALL(evt_msg_continue)
    CALL(evt_msg_print, 0, STRING("kuji_bomb_055"), PTR(kuji_bomb_055_callback), STRING("グルメボム兵"))
    SET_VALUE(GF(1), 1)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_TEC_SHUTDOWN1"), 0)
    CALL(evt_sub_intpl_msec_init, 2, 0, 239, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 480)
        SUBTRACT(LW(2), LW(0))
        SUBTRACT(LW(2), LW(0))
        CALL(viewport, 0, LW(0), 608, LW(2))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 2, 0, 303, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 608)
        SUBTRACT(LW(2), LW(0))
        SUBTRACT(LW(2), LW(0))
        CALL(viewport, LW(0), 240, LW(2), 1)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(viewport, 0, 0, 1, 1)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("グルメボム兵"), 512)
    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_3"), STRING("T_3"))
    CALL(evt_npc_set_anim, STRING("グルメボム兵"), STRING("S_3"))
    WAIT_MS(3000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_TEC_SHUTDOWN1"), 0)
    CALL(evt_sub_intpl_msec_init, 2, 303, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 608)
        SUBTRACT(LW(2), LW(0))
        SUBTRACT(LW(2), LW(0))
        CALL(viewport, LW(0), 240, LW(2), 1)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 2, 239, 0, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 480)
        SUBTRACT(LW(2), LW(0))
        SUBTRACT(LW(2), LW(0))
        CALL(viewport, 0, LW(0), 608, LW(2))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(viewport, 0, 0, 608, 480)
    WAIT_MS(1000)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR1"))
    CALL(evt_msg_print, 0, STRING("kuji_bomb_056"), 0, STRING("グルメボム兵"))
    CALL(evt_msg_select, 0, STRING("kuji_bomb_057"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(10)
    ELSE
        CALL(evt_msg_print_add, 0, STRING("kuji_bomb_058"))
        CALL(evt_msg_select, 0, STRING("kuji_bomb_059"))
        IF_INT_NOT_EQUAL(LW(0), 0)
            GOTO(10)
        ELSE
            CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_1"), STRING("T_1"))
            CALL(evt_msg_print_add, 0, STRING("kuji_bomb_060"))
            GOTO(0)
        END_IF
    END_IF
    LABEL(99)
    CALL(evt_unitwin_free)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_1"), STRING("T_1"))
    RETURN
EVT_END()

// NONMATCHING (99.04%): one extra mr r3,r0 before npcNameToPtr (string address materialised into r0, original into r3).
USER_FUNC(kuzi_bomb_bomb_eff) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 state = evtGetValue(event, args[1]);
    NpcEntry* npc;
    npc = npcNameToPtr("グルメボム兵");

    if (type == 2 || type == 3) {
        return EVT_RETURN_DONE;
    }
    switch (state) {
    case 0:
        if (npc->position.y > 25.0f) {
            effBombEntry(0, npc->position.x - 30.0f, npc->position.y, npc->position.z, 1.0f);
            if (type == 0) {
                state = 1;
            } else {
                state = 2;
            }
        }
        break;
    case 1:
        if (npc->position.y > 50.0f) {
            effBombEntry(0, npc->position.x + 30.0f, npc->position.y, npc->position.z, 1.0f);
            state = 2;
        }
        break;
    case 2:
        if (npc->position.y > 75.0f) {
            effBombEntry(0, npc->position.x - 30.0f, npc->position.y, npc->position.z, 1.0f);
            state = 3;
        }
        break;
    case 3:
        if (npc->position.y > 100.0f) {
            effBombEntry(0, npc->position.x + 30.0f, npc->position.y, npc->position.z, 1.0f);
            state = 4;
        }
        break;
    }
    evtSetValue(event, args[2], state);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(kuzi_bomb_bomb)
    SET_VALUE(LW(15), LW(0))
    WAIT_FRAMES(40)
    CALL(evt_npc_get_position, STRING("グルメボム兵"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LOTTERY_BOMB_BURST1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("グルメボム兵"), STRING("J_1A"))
    SET_FLOAT_VALUE(LW(0), FLOAT(20.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(-0.8f))
    SET_VALUE(LW(2), 60)
    CALL(evt_npc_get_position, STRING("グルメボム兵"), LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LOTTERY_BOMB_BURST2"), LW(3), LW(4), LW(5), 0)
    SET_VALUE(LW(6), 0)
    LOOP(0)
        ADD_FLOAT(LW(4), LW(0))
        ADD_FLOAT(LW(0), LW(1))
        CALL(evt_npc_set_position, STRING("グルメボム兵"), LW(3), LW(4), LW(5))
        IF_INT_LESS(LW(0), FLOAT(1.0f))
            SET_FLOAT_VALUE(LW(0), FLOAT(1.0f))
            SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
        END_IF
        CALL(kuzi_bomb_bomb_eff, LW(15), LW(6), LW(6))
        SUBTRACT(LW(2), 1)
        IF_INT_LESS_OR_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(20)
    CALL(evt_npc_set_anim, STRING("グルメボム兵"), STRING("J_1B"))
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(-1.0f))
    CALL(evt_npc_get_position, STRING("グルメボム兵"), LW(2), LW(3), LW(4))
    LOOP(0)
        ADD_FLOAT(LW(3), LW(0))
        ADD_FLOAT(LW(0), LW(1))
        CALL(evt_npc_set_position, STRING("グルメボム兵"), LW(2), LW(3), LW(4))
        IF_INT_LESS(LW(0), FLOAT(-6.0f))
            SET_FLOAT_VALUE(LW(0), FLOAT(-6.0f))
            SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(3), FLOAT(0.0f))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_position, STRING("グルメボム兵"), LW(2), FLOAT(0.0f), LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PARTY_PBM_DOWN1"), LW(2), 0, LW(4), 0)
    RETURN
EVT_END()
USER_FUNC(kuzi_get_keihin) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 ret;

    switch (type) {
    case 0:
        if (evtGetValue(NULL, GSWF(1238)) == 0) {
            ret = 0x104;
            evtSetValue(NULL, GSWF(1238), 1);
        } else {
            ret = 0xc4;
        }
        break;
    case 1:
        if (evtGetValue(NULL, GSWF(1239)) == 0) {
            ret = 0x12f;
            evtSetValue(NULL, GSWF(1239), 1);
        } else {
            ret = 0xc2;
        }
        break;
    case 2:
        ret = 0x96;
        break;
    case 3:
        ret = 0x97;
        break;
    default:
        ret = 0x94;
        break;
    }
    evtSetValue(event, args[1], ret);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(kuzi_keiziban_normal)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_lottery_todaynum, LW(0))
    CALL(evt_msg_print_insert, 0, STRING("kuji_bomb_023"), 0, 0, LW(0))
    CALL(evt_pouch_check_item, 106, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_lottery_flag, LW(0))
    IF_BITS_SET(LW(0), 16)
        SET_VALUE(LW(0), 0)
    ELSE
        SET_VALUE(LW(0), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_set_dir_to_target, STRING("グルメボム兵"), STRING("mario"))
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_lottery_flag, LW(1))
        SET_VALUE(LW(0), 4)
        IF_BITS_SET(LW(1), 32768)
            SET_VALUE(LW(0), 3)
        END_IF
        IF_BITS_SET(LW(1), 16384)
            SET_VALUE(LW(0), 2)
        END_IF
        IF_BITS_SET(LW(1), 8192)
            SET_VALUE(LW(0), 1)
        END_IF
        IF_BITS_SET(LW(1), 4096)
            SET_VALUE(LW(0), 0)
        END_IF
        IF_INT_NOT_EQUAL(LW(0), 4)
            CALL(evt_cam3d_evt_set, -150, 120, 890, -150, 50, 470, 800, 11)
            WAIT_MS(800)
            IF_INT_LESS_OR_EQUAL(LW(0), 1)
                CALL(evt_npc_flag_onoff, 0, STRING("グルメボム兵"), 1536)
                CALL(evt_npc_set_anim, STRING("グルメボム兵"), STRING("B_1"))
            END_IF
            CALL(evt_msg_print, 0, STRING("kuji_bomb_023_1"), 0, STRING("グルメボム兵"))
            IF_INT_LESS_OR_EQUAL(LW(0), 1)
                SET_VALUE(GW(0), LW(0))
                CALL_CHILD(kuzi_bomb_bomb)
                SET_VALUE(LW(0), GW(0))
            END_IF
            CALL(evt_npc_flag_onoff, 1, STRING("グルメボム兵"), 1536)
            SWITCH(LW(0))
                CASE_EQUAL(0)
                SET_VALUE(LW(1), STRING("kuji_bomb_024"))
                SET_VALUE(LW(2), STRING("M_N_7"))
                CASE_EQUAL(1)
                SET_VALUE(LW(1), STRING("kuji_bomb_025"))
                SET_VALUE(LW(2), STRING("M_N_7"))
                CASE_EQUAL(2)
                SET_VALUE(LW(1), STRING("kuji_bomb_026"))
                SET_VALUE(LW(2), STRING("M_N_5B"))
                CASE_EQUAL(3)
                SET_VALUE(LW(1), STRING("kuji_bomb_027"))
                SET_VALUE(LW(2), STRING("M_N_5B"))
            END_SWITCH
            CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_2"), STRING("T_2"))
            CALL(evt_msg_print, 0, LW(1), 0, STRING("グルメボム兵"))
            BEGIN_INLINE_EVENT
                CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2"), LW(1), LW(2), LW(3), 0)
                    CASE_EQUAL(1)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2"), LW(1), LW(2), LW(3), 0)
                    CASE_EQUAL(2)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_AWAKE1"), LW(1), LW(2), LW(3), 0)
                    CASE_EQUAL(3)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_AWAKE1"), LW(1), LW(2), LW(3), 0)
                END_SWITCH
            END_INLINE_EVENT
            CALL(evt_mario_set_pose, LW(2))
            WAIT_MS(800)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_set_dir_to_target, STRING("グルメボム兵"), STRING("mario"))
            CALL(kuzi_get_keihin, LW(0), LW(1))
            SET_VALUE(LW(2), LW(0))
            ADD(LW(2), 1)
            CALL(evt_msg_print_insert, 0, STRING("kuji_bomb_028"), 0, STRING("グルメボム兵"), LW(2))
            CALL(evt_mario_mov_pos2, -120, 480, 60)
            CALL(evt_item_entry, STRING("keihin"), LW(1), 0, -1000, 0, 17, -1, 0)
            CALL(evt_item_get_item, STRING("keihin"))
            CALL(evt_item_get_item_end_wait, STRING("keihin"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print, 0, STRING("kuji_bomb_030"), 0, STRING("グルメボム兵"))
                CALL(evt_pouch_remove_item, 106, LW(0))
                CALL(evt_lottery_buy, LW(0))
                CALL(evt_item_entry, STRING("kuzi"), 106, 0, -1000, 0, 17, -1, 0)
                CALL(evt_item_get_item, STRING("kuzi"))
                CALL(evt_item_get_item_end_wait, STRING("kuzi"))
                CALL(evt_msg_print, 0, STRING("kuji_bomb_032"), 0, STRING("グルメボム兵"))
            ELSE
                CALL(evt_msg_print, 0, STRING("kuji_bomb_029"), 0, STRING("グルメボム兵"))
            END_IF
            CALL(evt_cam3d_evt_off, 500, 11)
            WAIT_MS(500)
        ELSE
            CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_1"), STRING("T_1"))
            CALL(evt_msg_print, 0, STRING("kuji_bomb_033"), 0, STRING("グルメボム兵"))
            CALL(evt_mario_mov_pos2, -120, 480, 60)
            CALL(evt_item_entry, STRING("kinoko"), 148, 0, -1000, 0, 17, -1, 0)
            CALL(evt_item_get_item, STRING("kinoko"))
            CALL(evt_item_get_item_end_wait, STRING("kinoko"))
            CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_2"), STRING("T_2"))
            CALL(evt_msg_print, 0, STRING("kuji_bomb_035"), 0, STRING("グルメボム兵"))
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_npc_set_autotalkpose, STRING("グルメボム兵"), STRING("S_1"), STRING("T_1"))
    CALL(evt_npc_set_anim, STRING("グルメボム兵"), STRING("S_1"))
    RETURN
EVT_END()

EVT_BEGIN(kuzi_keiziban_husei)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("kuji_bomb_036"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kuzi_bomb_talk)
    CALL(evt_lottery, 0, LW(0), LW(1))
    IF_BITS_SET(LW(0), 4)
        CALL_CHILD(kuzi_bomb_talk_husei)
    ELSE
        CALL_CHILD(kuzi_bomb_talk_normal)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kuzi_keiziban_entry)
    CALL(evt_unitwin_alloc, 0)
    CALL(unk_8017524c, 1)
    CALL(evt_lottery, 1, LW(0), LW(1))
    IF_BITS_SET(LW(0), 4)
        CALL_CHILD(kuzi_keiziban_husei)
    ELSE
        CALL_CHILD(kuzi_keiziban_normal)
    END_IF
    CALL(unk_8017524c, 0)
    CALL(evt_unitwin_free)
    RETURN
EVT_END()
USER_FUNC(gor_npc_fuwafuwa) {
    s32* args = event->args;
    NpcEntry* npc;
    f32 scale;
    s32 duration;
    s32 time;
    f32 elapsed;
    u32 idx;
    f32 v;
    u32* now;
    u32* saved;
    s32 name;
    f32 base;

    name = evtGetValue(event, args[0]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    scale = evtGetFloat(event, args[1]);
    duration = evtGetValue(event, args[2]);
    {
        f32 table[40] = {
            0.0f, 0.5f, 1.0f, 1.4f, 1.8f, 2.0f, 2.3f, 2.5f,
            2.6f, 2.7f, 2.7f, 2.7f, 2.6f, 2.5f, 2.3f, 2.0f,
            1.8f, 1.4f, 1.0f, 0.5f, -0.0f, -0.5f, -1.0f, -1.4f,
            -1.8f, -2.0f, -2.3f, -2.5f, -2.6f, -2.7f, -2.7f, -2.7f,
            -2.6f, -2.5f, -2.3f, -2.0f, -1.8f, -1.4f, -1.0f, -0.5f,
        };

        if (isFirstCall) {
            event->savedTime = gp->renderTime;
            event->userdataf[1] = npc->position.x;
            event->userdataf[2] = npc->position.y;
            *(f32*)&event->field_84 = npc->position.z;
        }
        now = (u32*)&gp->renderTime;
        saved = (u32*)&event->savedTime;
        time = (now[1] - saved[1]) / ((*(u32*)0x800000F8 >> 2) / 1000);
        elapsed = sysMsec2FrameFloat((f32)time);
        idx = (u32)(s32)((0.5f + elapsed) * scale) % 40;
        v = event->userdataf[2];
        v += table[idx];
        npc->prevPosition.y = v;
        npc->position.y = v;
    }
    if (duration == 0) {
        return EVT_RETURN_BLOCK;
    }
    if (time < duration) {
        return EVT_RETURN_BLOCK;
    }
    v = event->userdataf[2];
    npc->prevPosition.y = v;
    npc->position.y = v;
    return EVT_RETURN_YIELD;
}

EVT_BEGIN(tenin_fuwafuwa)
    CALL(gor_npc_fuwafuwa, STRING("店員"), FLOAT(0.5f), 0)
    RETURN
EVT_END()

EVT_BEGIN(casino_fuwafuwa)
    CALL(gor_npc_fuwafuwa, STRING("バニーテレサ"), FLOAT(0.5f), 0)
    RETURN
EVT_END()

EVT_BEGIN(debug_event_mode)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSW(0), 121)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 123)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 124)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 272)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 278)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 279)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        RETURN
    END_IF
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 64)
        CALL(evt_sub_system_flag_onoff, 0, 64)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_set_pos, -327, 155, -432)
        CALL(evt_party_set_pos, 0, -327, 155, -467)
        CALL(evt_mario_cont_onoff, 1)
        CALL(evt_party_cont_onoff, 0, 1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_cam_road_reset)
        CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
        CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_cam3d_evt_off, 0, 11)
        CALL(evt_mario_key_onoff, 1)
    END_IF
    RETURN
EVT_END()
static USER_FUNC(enzan_lw_load) {
    s32 i;
    for (i = 0; i < 16; i++) {
        event->lwData[i] = xlw[i];
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(enzan_lw_save) {
    s32 i;
    for (i = 0; i < 16; i++) {
        xlw[i] = event->lwData[i];
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(monte_evt_door_close)
    CALL(enzan_lw_save)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(exchange_keep_sub)
    CALL(evt_mario_key_onoff, 1)
    CALL(enzan_lw_load)
    CALL_CHILD(evt_door_close)
    RETURN
EVT_END()

EVT_BEGIN(monte_evt_door_entry)
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 0)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(monte_evt_door_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(3), 0, PTR(evt_door_open), 0)
    END_IF
    CALL(evt_door_param, LW(0), 9, LW(1))
    IF_BITS_SET(LW(1), 1)
    END_IF
    RETURN
EVT_END()
USER_FUNC(evt_goryuugijo_set_caseid) {
    s32* args = event->args;
    GoryuugijoWork* work = (GoryuugijoWork*)evtGetValue(event, args[0]);
    s32* data = (s32*)work->data;
    s32 index = evtGetValue(event, args[1]);
    s32 value = evtGetValue(event, args[2]);

    data[index + 0x19] = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(goryuugijo_set_dataadr) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    if (index < 8) {
        dataadr[index] = evtGetValue(event, args[1]);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(gor_03_init_evt)
    IF_INT_EQUAL(GSWF(1229), 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR4"))
    ELSE
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
    END_IF
    IF_INT_NOT_EQUAL(GSWF(1229), 0)
        CALL(evt_mario_normalize)
    END_IF
    CALL_CHILD(gor_03_dokan_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hitobj_attr_onoff, 1, 0, STRING("s_bero"), 4)
    CALL_CHILD(luigi_npcEnt)
    CALL(evt_npc_setup, PTR(npcEnt))
    SWITCH(GSW(0))
        CASE_BETWEEN(278, 280)
        CALL(evt_npc_set_tribe, STRING("モニー"), STRING("モニー（指輪）"))
        CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
        CASE_GREATER_OR_EQUAL(281)
        CALL(evt_npc_set_tribe, STRING("モニー"), STRING("モニー（指輪）"))
        IF_INT_EQUAL(GSWF(5393), 1)
            CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
        END_IF
    END_SWITCH
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    SET_VALUE(LW(0), PTR(door_data_01_0))
    CALL_CHILD(evt_door_entry)
    IF_INT_EQUAL(GSWF(1230), 1)
        SET_VALUE(LW(0), PTR(door_data_01_1))
        CALL_CHILD(evt_door_entry)
    END_IF
    SET_VALUE(LW(0), PTR(door_data_02_0l))
    CALL_CHILD(monte_evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_02_0r))
    CALL_CHILD(monte_evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_02_1))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_03))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_03_roll))
    CALL_CHILD(evt_door_03_entry)
    SET_VALUE(LW(0), PTR(door_data_04))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_04_roll))
    CALL_CHILD(evt_door_04_entry)
    SPAWN_EVENT(wall_bomb_init)
    CALL(evt_run_case_evt, 15, 1, STRING("A_ana_ana"), 0, PTR(wall_bomb), 0)
    CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list), PTR(shopper_data), PTR(trade_list))
    CALL(evt_item_entry, STRING("item_00"), 125, 550, 0, 180, 16, GSWF(5580), 0)
    CALL(evt_item_entry, STRING("item_01"), 125, -585, 0, 220, 16, GSWF(5581), 0)
    CALL(evt_item_entry, STRING("item_02"), 125, 220, 10, 225, 16, GSWF(5582), 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -200, 0, 125, STRING("S_ground_A"), 125, GSWF(5579))
    CALL(evt_mobj_powerupblk, STRING("pblk1"), 250, 60, -510, 0, GSWF(5527), 90)
    CALL(evt_mobj_powerupblk, STRING("pblk2"), -563, 0, -150, 0, GSWF(5528), 90)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kou_kusas01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kou_kusas02"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kou_kusas03"), 1)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas01"), 1)
        CASE_LESS(112)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas01"), 1)
        CASE_LESS(164)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas01"), 1)
        CASE_LESS(215)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas02"), 1)
        CASE_LESS(261)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas02"), 1)
        CASE_LESS(339)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas02"), 1)
        CASE_LESS(373)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas03"), 1)
        CASE_LESS(401)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas03"), 1)
        CASE_DEFAULT
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kusas03"), 1)
    END_SWITCH
    SPAWN_EVENT_GET_ID(PTR(desk_talk_check), GW(15))
    CALL_CHILD(debug_event_mode)
    CALL(evt_goryuugijo_alloc, LW(0), 0)
    CALL(goryuugijo_set_dataadr, 0, LW(0))
    CALL(evt_goryuugijo_alloc, LW(0), 1)
    CALL(goryuugijo_set_dataadr, 1, LW(0))
    CALL(evt_run_case_evt, 9, 1, STRING("A_srot01"), 0, PTR(slot1_set), LW(1))
    CALL(evt_goryuugijo_set_caseid, LW(0), 0, LW(1))
    CALL(evt_run_case_evt, 9, 1, STRING("A_srot02"), 0, PTR(slot2_set), LW(1))
    CALL(evt_goryuugijo_set_caseid, LW(0), 1, LW(1))
    CALL(evt_run_case_evt, 9, 1, STRING("A_gb_a"), 0, PTR(ranking_set), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_mot_a"), 0, PTR(exchange_set), 0)
    IF_INT_NOT_EQUAL(GSWF(1229), 0)
        SET_VALUE(GSWF(1229), 0)
        SPAWN_EVENT(yuureturn_rtn)
    END_IF
    IF_INT_EQUAL(GSW(0), 172)
        SPAWN_EVENT(peach_mail)
    END_IF
    SPAWN_EVENT(tenin_fuwafuwa)
    SPAWN_EVENT(casino_fuwafuwa)
    SPAWN_EVENT(evt_sub_mail_event)
    CALL(evt_run_case_evt, 9, 1, STRING("A_kuzi"), 0, PTR(kuzi_keiziban_entry), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_02"), 2)
    IF_INT_EQUAL(GSW(0), 121)
        SPAWN_EVENT(maffiaboss_maffiaboss)
    END_IF
    IF_INT_EQUAL(GSW(0), 123)
        IF_INT_EQUAL(GSWF(1236), 0)
            SPAWN_EVENT(maffiaboss_airship_ticket)
        END_IF
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(268)
        CALL(evt_run_case_evt, 9, 0, STRING("A_i02_f2_set00C"), 0, PTR(maffiaboss_desk_talk), 0)
        CASE_LESS(279)
        SET_VALUE(LW(15), 1)
        CALL(evt_run_case_evt, 9, 0, STRING("A_i02_bed_l"), 0, PTR(maffiaboss_desk_talk), GW(12))
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 0, STRING("A_i02_bed_m"), 0, PTR(maffiaboss_desk_talk), GW(13))
        CASE_LESS(282)
        CASE_DEFAULT
        CALL(evt_run_case_evt, 9, 0, STRING("A_i02_f2_set00C"), 0, PTR(peton_desk_talk), 0)
    END_SWITCH
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kou_kabe"), 2)
    RETURN
EVT_END()

// The phase-0x2d block shares `u8* pw = data+i*4` before `u8* pb = data+i`, `s8 s` and indexes `tmp1[j]`
// (the operand order matters for the register pick); case 11 keeps `CaseSetup* sp = &setup` so the struct
// address is materialised once (r22) exactly like the original, and case 3's `dir` is a single nested ternary
// (an if/else compiles an extra `mr`). The sel loop is written as a
// `k = 0; goto sel_test; sel_body: ... k = 1; sel_inc: k++; sel_test: if (k < 2) goto sel_body;` with the
// success path setting `k = 1` in each case and the invalid path `goto sel_inc`: a plain `for` compiles to
// `mtctr`/`bdnz` here, this form reproduces the original's explicit `li r9,0; b test` counter and its
// `li r9,1; stb; addi r9,r9,1; cmpwi r9,2; blt` break.
// Case 3 is the original's `if (i==sel) { switch(i){...} } else { stop = 0; } if (stop && ...) {...}` (its
// `bne` after `cmpw i, sel` targets the `li r25,0` the `else` emits). Writing it that way is required for the
// case-2 `else` to exist, and it changes which registers MWCC hands out; the declaration order
// `j, k, stop, work, data, i, changed` is what makes that structure score 98.06%.
//
// The trailing `return 0;` is not the original source. The original never writes r3 before `blr`, so it is
// really a `void` function: with the `USER_FUNC` (s32) prototype MWCC treats r3 as the live return register
// and uses r4 for the switch jump-table base, shifting the whole scratch register set (that version scores
// 97.25%). Any non-`void` return makes r3 free again and the dispatch matches; compiling the same body as
// `void` scores 98.18% and removes the extra `li r3,0` that `return 0` emits. The prototype is in the
// existing header line `USER_FUNC(goryuugijo_main_slot);`, which must not be changed here; if it is changed to
// `void`, drop this `return 0;`.
//
// Still differs (all register allocation / scheduling): event stays in r25 vs the original's r26 (the entry
// `mr r25, r3` vs `mr r26, r3`) even when the two later `event` uses are replaced by `work`/`work->entry`;
// `stop` lands in r30 vs the original's r25, pb in r31 vs r30; the case-5 string address is materialised into
// r0 and then `mr`'d instead of loaded directly into its register; and the case-5 pointer / case-11 index /
// `cnt` loop init / sel conditions keep a different register set.
// NONMATCHING (98.06%): see above.
USER_FUNC(goryuugijo_main_slot) {
    s32 j;
    s32 k;
    s32 stop;
    GoryuugijoWork* work = (GoryuugijoWork*)event->lwData[0];
    u8* data = (u8*)work->data;
    s32 i;

    switch (*(s32*)(data + 0x04)) {
    case 1:
        *(s16*)(data + 0x74) = 0;
        *(s32*)(data + 0x54) = 0;
        *(s32*)(data + 0x58) = 0;
        *(s32*)(data + 0x44) = 0;
        *(s32*)(data + 0x40) = 0;
        *(s32*)(data + 0x3c) = 0;
        *(s32*)(data + 0x50) = 0;
        *(s32*)(data + 0x4c) = 0;
        *(s32*)(data + 0x48) = 0;
        *(s8*)(data + 0x32) = 1;
        *(s8*)(data + 0x31) = 1;
        *(s8*)(data + 0x30) = 1;
        *(f32*)(data + 0x2c) = 0.0f;
        *(f32*)(data + 0x28) = 0.0f;
        *(f32*)(data + 0x24) = 0.0f;
        *(u32*)data &= ~2;
        *(u32*)data &= ~4;
        *(s32*)(data + 0x04) += 1;
        break;
    case 2:
        slot_neon_sub(work, data);
        if (*(s32*)(data + 0x54) >= 0xf) {
            *(s32*)(data + 0x54) = 0;
            *(s32*)(data + 0x04) += 1;
            *(s32*)(data + 0x70) = psndSFXOn("SFX_EVT_GAME_SLOT_ROUND1");
        } else {
            *(s32*)(data + 0x54) += 1;
            *(f32*)(data + 0x08) += 4.0f;
            mapObjRotate(drumname[*(s32*)(data + 0x60)][3], 0.0f, 0.0f, *(f32*)(data + 0x08));
        }
        break;
    case 3:
    {
        s32 changed;
        slot_neon_sub(work, data);
        if (*(s32*)(data + 0x54) <= 6) {
            if (*(s32*)(data + 0x54) >= 6) {
                *(f32*)(data + 0x08) = 0.0f;
            } else {
                *(f32*)(data + 0x08) -= 10.0f;
            }
            mapObjRotate(drumname[*(s32*)(data + 0x60)][3], 0.0f, 0.0f, *(f32*)(data + 0x08));
            *(s32*)(data + 0x54) += 1;
        }
        i = *(s16*)(data + 0x74);
        if ((s32)((u8*)data + i)[0x30] != 2 || (keyGetDirRep(0) & 0xC000) != 0) {
            s32 dir;
            i = *(s16*)(data + 0x74);
            dir = ((s32)((u8*)data + i)[0x30] != 2) ? 1 : ((keyGetDirRep(0) & 0x4000) ? -1 : 1);
            for (j = 0; j < 3; j++) {
                i = *(s16*)(data + 0x74) + dir;
                *(s16*)(data + 0x74) = i;
                if (*(s16*)(data + 0x74) < 0) {
                    *(s16*)(data + 0x74) = 2;
                } else if (*(s16*)(data + 0x74) > 2) {
                    *(s16*)(data + 0x74) = 0;
                }
                if (*(s8*)(data + *(s16*)(data + 0x74) + 0x30) == 2) {
                    break;
                }
            }
        }
        changed = 0;
        {
        char* stopSfx = "SFX_EVT_GAME_SLOT_STOP1";
        for (i = 0; i < 3; i++) {
            if (*(s8*)(data + i + 0x30) == 0) {
                changed += 1;
                continue;
            }
            if (i == *(s16*)(data + 0x74)) {
                switch (i) {
                case 0:
                    if (keyGetButtonTrg(0) & 0x100) {
                        stop = 1;
                    } else {
                        stop = 0;
                    }
                    break;
                case 1:
                    if (keyGetButtonTrg(0) & 0x100) {
                        stop = 1;
                    } else {
                        stop = 0;
                    }
                    break;
                case 2:
                    if (keyGetButtonTrg(0) & 0x100) {
                        stop = 1;
                    } else {
                        stop = 0;
                    }
                    break;
                }
            } else {
                stop = 0;
            }
                if (stop && *(s8*)(data + i + 0x30) == 2) {
                    *(s8*)(data + i + 0x30) = 3;
                    if (*(u32*)data & 2) {
                        *(s8*)(data + i + 0x39) = 0;
                    } else {
                        *(s8*)(data + i + 0x39) = irand(2);
                    }
                    psndSFXOn(stopSfx);
                }
            changed += slot_rotate_sub(work, data, i);
        }
        }
        if (changed == 3) {
            for (i = 0; i < 3; i++) {
                char** p = drumname[*(s32*)(data + 0x60)] + i * 3;
                mapGrpFlagOn(p[5], 1);
                mapGrpFlagOn(p[6], 1);
                mapGrpFlagOn(p[7], 1);
            }
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][15], 0, 0);
            *(s32*)(data + 0x54) = 0;
            *(s32*)(data + 0x04) += 1;
        }
        break;
    }
    case 4:
    {
        s32 phase = *(s32*)(data + 0x54);
        if (phase == 0) {
            s32 tmp1[3];
            s32 cnt;
            for (i = 0; i < 3; i++) {
                tmp1[i] = *(u32*)((u8*)slotgara + *(s32*)(data + 0x60) * 0x60 + i * 0x20 + (s32)(*(f32*)(data + 0xc + *(s32*)(data + 0x60) * 0xc + i * 4) / 45.0f) * 4);
            }
            if (tmp1[0] == tmp1[1] && tmp1[1] == tmp1[2]) {
                *(s32*)(data + 0x04) = 6;
                break;
            }
            if ((*(u32*)data & 4) == 0) {
                *(s32*)(data + 0x04) = 6;
                break;
            }
            cnt = 0;
            {
            s32 j2;
            for (j2 = 0; j2 < 3; j2++) {
                if (tmp1[j2] == *(s8*)(data + *(s32*)(data + 0x60) * 3 + j2 + 0x33)) {
                    cnt += 1;
                }
            }
            }
            if (cnt < 2) {
                *(s32*)(data + 0x04) = 6;
                break;
            }
        }
        if (phase == 0 || phase == 0x12 || phase == 0x24) {
            for (i = 0; i < 3; i++) {
                char** p = drumname[*(s32*)(data + 0x60)] + i * 3;
                mapGrpFlagOn(p[5], 1);
                mapGrpFlagOn(p[6], 1);
                mapGrpFlagOn(p[7], 1);
            }
        }
        if (*(s32*)(data + 0x54) == 9 || *(s32*)(data + 0x54) == 0x1b || *(s32*)(data + 0x54) == 0x2d) {
            for (i = 0; i < 3; i++) {
                char** p = drumname[*(s32*)(data + 0x60)] + i * 3;
                mapGrpFlagOn(p[5], 1);
                mapGrpFlagOn(p[6], 1);
                mapGrpFlagOn(p[7], 1);
            }
        }
        if (*(s32*)(data + 0x54) == 0x2d) {
            s32 tmp[3][3];
            for (i = 0; i < 3; i++) {
                u8* base = (u8*)slotgara + i * 0x20;
                s32 a = (s32)(*(f32*)(data + 0xc + *(s32*)(data + 0x60) * 0xc + i * 4) / 45.0f);
                for (j = 0; j < 3; j++) {
                    tmp[j][i] = *(u32*)(base + *(s32*)(data + 0x60) * 0x60 + ((a + j - 1) & 7) * 4);
                }
            }
            for (i = 0; i < 3; i++) {
                s8 s;
                s32 sel;
                if (tmp[1][i] == *(s8*)(data + *(s32*)(data + 0x60) * 3 + i + 0x33)) {
                    continue;
                }
                sel = irand(2);
                {
                u8* pw = data + i * 4;
                u8* pb = data + i;
                k = 0;
                goto sel_test;
                sel_body:
                    sel = (sel + 1) & 1;
                    switch (sel) {
                    case 0:
                        s = *(s8*)(pb + *(s32*)(data + 0x60) * 3 + 0x33);
                        if (s != tmp[0][i] && tmp[2][i] == s) {
                            goto sel_inc;
                        }
                        if (s == 0) {
                            pb[0x39] = 1;
                            *(s32*)(pw + 0x48) = 1;
                            *(f32*)(pw + 0x24) = -1.40625f;
                        } else {
                            pb[0x39] = 6;
                            if (*(s8*)(pb + *(s32*)(data + 0x60) * 3 + 0x33) == 2) {
                                pb[0x39] += 8;
                            }
                            *(s32*)(pw + 0x48) = 0;
                            *(f32*)(pw + 0x24) = 1.40625f;
                        }
                        pb[0x30] = 4;
                        k = 1;
                        break;
                    case 1:
                        s = *(s8*)(pb + *(s32*)(data + 0x60) * 3 + 0x33);
                        if (s != tmp[2][i] && s == tmp[0][i]) {
                            goto sel_inc;
                        }
                        if (s == 0) {
                            pb[0x39] = 0;
                            *(s32*)(pw + 0x48) = 0;
                            *(f32*)(pw + 0x24) = 1.40625f;
                        } else {
                            pb[0x39] = 7;
                            if (*(s8*)(pb + *(s32*)(data + 0x60) * 3 + 0x33) == 2) {
                                pb[0x39] += 8;
                            }
                            *(s32*)(pw + 0x48) = 1;
                            *(f32*)(pw + 0x24) = -1.40625f;
                        }
                        pb[0x30] = 4;
                        k = 1;
                        break;
                    }
                sel_inc:
                    k++;
                sel_test:
                    if (k < 2) goto sel_body;
                }
            }
            *(s32*)(data + 0x04) += 1;
        }
        *(s32*)(data + 0x54) += 1;
        break;
    }
    case 5:
    {
        s32 n = 0;
        s32 m5;
        for (m5 = 0; m5 < 3; m5++) {
            if (*(s8*)(data + m5 + 0x30) == 0) {
                n += 1;
            } else {
                n += slot_rotate_sub(work, data, m5);
            }
        }
        if (n == 3) {
            *(s32*)(data + 0x54) = 0;
            *(s32*)(data + 0x04) += 1;
        }
        break;
    }
    case 6:
    {
        s32 tmp[3];
        for (i = 0; i < 3; i++) {
            tmp[i] = *(u32*)((u8*)slotgara + *(s32*)(data + 0x60) * 0x60 + i * 0x20 + (s32)(*(f32*)(data + 0xc + *(s32*)(data + 0x60) * 0xc + i * 4) / 45.0f) * 4);
        }
        if (tmp[0] == tmp[1] && tmp[1] == tmp[2]) {
            *(s32*)(data + 0x54) = 0;
            *(s32*)(data + 0x04) += 1;
            psndSFXOn("SFX_EVT_GAME_SLOT_SUCCESS1");
            switch (tmp[0]) {
            case 0:
                *(s32*)(data + 0x5c) = 3;
                break;
            case 1:
                *(s32*)(data + 0x5c) = 0xf;
                break;
            case 2:
                *(s32*)(data + 0x5c) = 0x64;
                break;
            }
            caseDelete(*(s32*)(data + 0x64));
            caseDelete(*(s32*)(data + 0x68));
            *(u32*)data |= 1;
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][16], 1, 0);
            *(u32*)data |= 8;
        } else {
            *(s32*)(data + 0x04) = 0xb;
        }
        psndSFXOff(*(s32*)(data + 0x70));
        break;
    }
    case 7:
        if (*(s32*)(data + 0x54) >= 0x30) {
            *(s32*)(data + 0x54) = 0;
            *(s32*)(data + 0x04) += 1;
        }
        *(s32*)(data + 0x54) += 1;
        break;
    case 8:
        if (*(s32*)(data + 0x54) >= 0x24) {
            *(s32*)(data + 0x54) = 0x13;
            *(s32*)(data + 0x04) += 1;
        }
        *(s32*)(data + 0x54) += 1;
        break;
    case 9:
        if (*(s32*)(data + 0x54) >= 0x13) {
            *(s32*)(data + 0x54) = 0;
            if (*(EventEntry**)(data + 0x6c) == NULL) {
                *(EventEntry**)(data + 0x6c) = evtEntry(slot_fall_monte, 0, 0);
                (*(EventEntry**)(data + 0x6c))->lwData[0] = event->lwData[0];
            }
            *(s32*)(data + 0x04) += 1;
        } else {
            *(s32*)(data + 0x54) += 1;
        }
        break;
    case 10:
    {
        EventEntry* e = *(EventEntry**)(data + 0x6c);
        if (e == NULL || evtCheckID(e->eventId) == 0) {
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][16], 2, 0);
            *(u32*)data &= ~8;
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][15], 0, 0);
            *(s32*)(data + 0x54) = 0;
            *(EventEntry**)(data + 0x6c) = NULL;
            *(s32*)(data + 0x04) += 1;
        } else {
            *(s32*)(data + 0x54) += 1;
        }
        break;
    }
    case 11:
    {
        if (*(EventEntry**)(data + 0x6c) != NULL) {
            u8* p = (u8*)monte_p[0];
            *(s32*)(*(u8**)(p + 0x4c) + 0x5c) = 0;
            _mapFree((void*)monte_p[0]);
            evtDeleteID((*(EventEntry**)(data + 0x6c))->eventId);
            *(EventEntry**)(data + 0x6c) = NULL;
        }
        *(s32*)(data + 0x54) = 0;
        *(s32*)(data + 0x58) = 0;
        *(s32*)(data + 0x04) = 0;
        *(s32*)(data + 0x44) = 0;
        *(s32*)(data + 0x40) = 0;
        *(s32*)(data + 0x3c) = 0;
        *(s32*)(data + 0x50) = 0;
        *(s32*)(data + 0x4c) = 0;
        *(s32*)(data + 0x48) = 0;
        if (*(u32*)data & 1) {
            s32 m;
            CaseSetup setup;
            CaseSetup* sp = &setup;
            *(u32*)data &= ~1;
            for (m = 0; m < 2; m++) {
                sp->activeConditionId = 0x8009;
                sp->hitObjName = drumname[m][4];
                sp->swFlag = 0;
                sp->activeFunc = 0;
                sp->evtCode = (void*)slotevttbl[m];
                sp->priority = 0;
                memcpy(sp->lwData, event->lwData, 0x40);
                *(s32*)(data + m * 4 + 0x64) = caseEntry(sp);
            }
        }
        if (*(u32*)data & 8) {
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][16], 2, 0);
            *(u32*)data &= ~8;
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][15], 0, 0);
        }
        break;
    }
    }
    return 0;
}

void goryuugijo_disp_slot(CameraId cameraId, void* param) {
    u8* p = (u8*)((void**)param)[7];
    u32 tbl[3] = {0x1f3, 0x1f3, 0x1f3};
    Vec pos;
    f32 projv[7];
    f32 viewportv[6];
    f32 sx;
    f32 sy;
    f32 sz;
    Mtx trans;
    Mtx scale;
    Mtx mtx;
    CameraEntry* cam;
    u16 iconId;
    s32 i;

    for (i = 0; i < 3; i++) {
        if (*(s8*)(p + 0x30 + i) == 2 && i == *(s16*)(p + 0x74)) {
            mapObjGetPos(drumname[*(s32*)(p + 0x60)][i], &pos);
            pos.x -= 25.0f;
            pos.y += 10.0f;
            cam = camGetPtr(4);
            GXSetProjection(cam->projection, cam->projType);
            GXGetProjectionv(projv);
            GXGetViewportv(viewportv);
            GXProject(pos.x, pos.y, pos.z, cam->view, projv, viewportv, &sx, &sy, &sz);
            cam = camGetPtr(cameraId);
            GXSetProjection(cam->projection, cam->projType);
            PSMTXScale(scale, 1.1f, 1.1f, 1.1f);
            PSMTXTrans(trans, -304.0f + sx, 240.0f - sy, 0.0f);
            PSMTXConcat(trans, scale, mtx);
            iconDispGx2(mtx, 0x10, 0x1f2);
            PSMTXTrans(trans, -304.0f + sx, 31.35f + (240.0f - sy), 0.0f);
            PSMTXConcat(trans, scale, mtx);
#ifdef MUST_MATCH
            // iconDispGx2's header says s16 but the original reads a zero-extended u16 (no extsh).
            ((void (*)(Mtx, s16, u16))iconDispGx2)(mtx, 0x10, (u16)tbl[i]);
#else
            iconDispGx2(mtx, 0x10, (u16)tbl[i]);
#endif
        }
    }
}

// NONMATCHING (98.92%): `data + (sel*0xc + 0xc) + index*4` (not the flat form) and
// `q = (s32*)((s8*)data) + index` reproduce the original address shape; the last
// `if (*(s32*)(data + index*4 + 0x3c) == 0)` reuses r3 instead of rematerialising `data + index*4` into r5.
s32 slot_rotate_sub(void* arg0, u8* data, s32 index) {
    s32 dir = 0;
    s32 changed = 0;
    s32 steps = (s32)((f32)(s32)(*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)) / 45.0f);
    f32 diff;
    s32 next;

    if ((*(f32*)(data + index * 4 + 0x24)) > 0.0f) {
        next = (steps + 1) & 7;
        diff = 45.0f * (f32)next - (*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4));
        if (diff < 0.0f) {
            diff += 360.0f;
        }
        dir = (diff <= (*(f32*)(data + index * 4 + 0x24))) ? 1 : 0;
    } else {
        diff = 45.0f * (f32)steps - (*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4));
        if (diff > 0.0f) {
            diff -= 360.0f;
        }
        dir = (diff > (*(f32*)(data + index * 4 + 0x24))) ? -1 : 0;
    }
    if ((((s8*)data)[index + 0x30] == 3 || ((s8*)data)[index + 0x30] == 4) && dir != 0) {
        if (((s8*)data)[index + 0x39] != 0) {
            if (((s8*)data)[index + 0x30] == 4) {
                s32* q = ((s32*)((s8*)data) + index);
                if (q[0xf] == 0) {
                    if (q[0x12] != 0) {
                        q[0x12]--;
                        q[0xf] = 0;
                    } else {
                        q[0xf] = 0x12;
                    }
                } else {
                    q[0xf]--;
                }
            }
            if (*(s32*)(data + index * 4 + 0x3c) == 0) {
                ((s8*)data)[index + 0x39]--;
            }
        } else {
            ((s8*)data)[index + 0x30] = 0;
            changed = 1;
            (*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)) = 45.0f * (f32)steps;
            goto done;
        }
    }
    (*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)) += (*(f32*)(data + index * 4 + 0x24));
    if ((*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)) >= 360.0f) {
        (*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)) -= 360.0f;
    }
    if ((*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)) < 0.0f) {
        (*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)) += 360.0f;
    }
done:
    mapObjRotate(drumname[*(s32*)(data + 0x60)][index], 0.0f, 0.0f, (*(f32*)(data + (*(s32*)(data + 0x60) * 0xc + 0xc) + index * 4)));
    return changed;
}

// Matching: indexing the u32 table as `&upsidegara[cur * 0x18]` (0x18 elements = 0x60 bytes) gives the
// original's `add r0,r31,r5` / `add r5,r0,r27` two-step base, and `s8* d` gives the `lbzx r4` byte.
void slot_neon_sub(void* arg0, u8* data) {
    s32 i;
    s32 r;
    s32 cur;

    if (*(s32*)(data + 0x58) > 0x4e) {
        return;
    }
    if (*(s32*)(data + 0x58) == 0) {
        if (irand(0xa) == 0) {
            *(u32*)data |= 2;
        }
        *(u32*)data &= ~2;
    }
    if (*(s32*)(data + 0x58) < 0x1b) {
        if (*(s32*)(data + 0x58) & 1) {
            for (i = 0; i < 3; i++) {
                u8* up;
                s8* d;
                r = irand(8);
                cur = *(s32*)(data + 0x60);
                up = (u8*)&upsidegara[cur * 0x18] + i * 0x20;
                d = (s8*)(data + cur * 3 + i + 0x33);
                while (*(s32*)(up + r * 4) == *d) {
                    r = (r + 1) & 7;
                }
                *d = *(s32*)(up + r * 4);
                if (*(u32*)data & 2) {
                    int c = *(s32*)(data + 0x60);
                    mapGrpFlagOn(drumname[c][i * 3 + 5], 1);
                    mapGrpFlagOn(drumname[c][i * 3 + 6], 1);
                    mapGrpFlagOn(drumname[c][i * 3 + 7], 1);
                } else {
                    int c = *(s32*)(data + 0x60);
                    mapGrpFlagOn(drumname[c][i * 3 + 5], 1);
                    mapGrpFlagOn(drumname[c][i * 3 + 6], 1);
                    mapGrpFlagOn(drumname[c][i * 3 + 7], 1);
                }
            }
        }
    } else if (*(s32*)(data + 0x58) == 0x1b) {
        for (i = 0; i < 3; i++) {
            s32 c;
            cur = *(s32*)(data + 0x60);
            *(u8*)(data + cur * 3 + i + 0x33) = i;
            c = *(s32*)(data + 0x60);
            mapGrpFlagOn(drumname[c][i * 3 + 5], 1);
            mapGrpFlagOn(drumname[c][i * 3 + 6], 1);
            mapGrpFlagOn(drumname[c][i * 3 + 7], 1);
        }
        cur = *(s32*)(data + 0x60);
        if (*(s8*)(data + cur * 3 + 0x33) == *(s8*)(data + cur * 3 + 0x34) &&
            *(s8*)(data + cur * 3 + 0x34) == *(s8*)(data + cur * 3 + 0x35)) {
            *(u32*)data |= 4;
        }
        if ((*(u32*)data & 4) && (*(u32*)data & 2)) {
            *(f32*)(data + 0x2c) = 2.8125f;
            *(f32*)(data + 0x28) = 2.8125f;
            *(f32*)(data + 0x24) = 2.8125f;
        } else {
            *(f32*)(data + 0x2c) = 11.25f;
            *(f32*)(data + 0x28) = 11.25f;
            *(f32*)(data + 0x24) = 11.25f;
        }
        *(s8*)(data + 0x32) = 2;
        *(s8*)(data + 0x31) = 2;
        *(s8*)(data + 0x30) = 2;
    } else {
        if (*(u32*)data & 4) {
            if (*(s32*)(data + 0x58) == 0x21 || *(s32*)(data + 0x58) == 0x33 || *(s32*)(data + 0x58) == 0x45) {
                for (i = 0; i < 3; i++) {
                    cur = *(s32*)(data + 0x60);
                    mapGrpFlagOn(drumname[cur][i * 3 + 5], 1);
                    mapGrpFlagOn(drumname[cur][i * 3 + 6], 1);
                    mapGrpFlagOn(drumname[cur][i * 3 + 7], 1);
                }
            }
            if (*(s32*)(data + 0x58) == 0x2a || *(s32*)(data + 0x58) == 0x3c || *(s32*)(data + 0x58) == 0x4e) {
                for (i = 0; i < 3; i++) {
                    cur = *(s32*)(data + 0x60);
                    mapGrpFlagOn(drumname[cur][i * 3 + 5], 1);
                    mapGrpFlagOn(drumname[cur][i * 3 + 6], 1);
                    mapGrpFlagOn(drumname[cur][i * 3 + 7], 1);
                }
            }
        }
    }
    if (*(u32*)data & 2) {
        if (*(s32*)(data + 0x58) == 0 || *(s32*)(data + 0x58) == 0xc || *(s32*)(data + 0x58) == 0x18) {
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][14], 0, 0);
        }
        if (*(s32*)(data + 0x58) == 6 || *(s32*)(data + 0x58) == 0x12) {
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][15], 0, 0);
        }
    } else {
        if (*(s32*)(data + 0x58) == 0x18) {
            mapPlayAnimationLv(drumname[*(s32*)(data + 0x60)][14], 0, 0);
        }
    }
    if (*(s32*)(data + 0x58) <= 0x4e) {
        *(s32*)(data + 0x58) = *(s32*)(data + 0x58) + 1;
    }
}

// NONMATCHING (95.83%): data pointer lands in r5 instead of r4 (constant addr materialised into r4).
USER_FUNC(goryuugijo_init_slot) {
    GoryuugijoWork* work = (GoryuugijoWork*)event->lwData[0];
    u8* data = (u8*)work->data;

    *(s32*)(data + 0x54) = 0;
    *(f32*)(data + 0x8) = 0.0f;
    *(s32*)(data + 0x6c) = 0;
    *(u32*)data &= ~1;
}

void goryuugijo_upsidewindow_disp_sub(s32 a, s32 b) {
    mapGrpFlagOn(drumname[a][b * 3 + 5], 1);
    mapGrpFlagOn(drumname[a][b * 3 + 6], 1);
    mapGrpFlagOn(drumname[a][b * 3 + 7], 1);
}

void goryuugijo_upsidewindow_dispoff_sub(s32 a, s32 b) {
    mapGrpFlagOn(drumname[a][b * 3 + 5], 1);
    mapGrpFlagOn(drumname[a][b * 3 + 6], 1);
    mapGrpFlagOn(drumname[a][b * 3 + 7], 1);
}

static void monte_disp(CameraId cameraId, void* param) {
    u8* p = (u8*)param;
    s32 i;

    for (i = 0; i < *(s32*)(*(u32*)(p + 0x4c) + 0x5c); i++) {
        if (p[i * 0x50] == 0) {
            iconDispGx((Vec*)(p + i * 0x50 + 0xc), 0x40, 0x13f, 1.0f);
        }
    }
}

static s32 get_monte_check(Vec* a, Vec* b) {
    if (PSVECDistance(a, b) < 20.0f) {
        if (fabs(a->y - b->y) < 20.0) {
            return 1;
        }
    }
    return 0;
}

USER_FUNC(evt_goryuugijo_get_caseid) {
    s32* args = event->args;
    GoryuugijoWork* work = (GoryuugijoWork*)evtGetValue(event, args[0]);
    s32* data = (s32*)work->data;
    s32 index = evtGetValue(event, args[1]);

    evtSetValue(event, args[2], data[index + 0x19]);
    return EVT_RETURN_DONE;
}

void goryuugijo_disp_montemedal(CameraId cameraId, void* param) {
    monteCountDisp();
}

USER_FUNC(goryuugijo_main_montemedal) {
}

USER_FUNC(goryuugijo_init_montemedal) {
    if (evtGetValue(event, GSWF(1229)) == 0) {
        yuugijou_montemedal_keep(event, isFirstCall);
    }
}

s32 exchange_ret_tbl_no(void) {
    s32 value = evtGetValue(NULL, GSW(0));
    s32 ret = 0;

    if (value >= 0x192) {
        ret = 8;
    } else if (value >= 0x177) {
        ret = 7;
    } else if (value >= 0x158) {
        ret = 6;
    } else if (value >= 0x10b) {
        ret = 5;
    } else if (value >= 0xda) {
        ret = 4;
    } else if (value >= 0xab) {
        ret = 3;
    } else if (value >= 0x75) {
        ret = 2;
    } else if (value >= 0x3d) {
        ret = 1;
    }
    return ret;
}
// clang-format on

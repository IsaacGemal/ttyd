#include "bom_02.h"
#include <dolphin/mtx.h>
#include "driver/animdrv.h"
#include "driver/envdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_kinopio.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_shop.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_bom_01_karakara[];
extern s32 evt_bom_snowfall[];
static USER_FUNC(anim_tevcallback_mobj);
static USER_FUNC(anim_tevcallback);
static USER_FUNC(yami_view);

// clang-format off
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(evt_ie1_door_1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie1_door"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    RETURN
EVT_END()

EVT_BEGIN(evt_ie1_wall)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.66602f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie1_kai"), FLOAT(0.0f), FLOAT(0.0f), LW(0))
    RETURN
EVT_END()

const char* ie1_npc[] = {
    "宿店員", NULL,
};

EVT_BEGIN(evt_ie1_door_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_ie1_door_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

DoorEntry ie1_door[] = {
    { 0, 3, 1, "S_ie1_door", "A_ie1_door1", "A_ie1_door1_u", "S_ie1_soto", NULL,
      "S_ie1_naka", NULL, evt_ie1_door_1, 0, evt_ie1_wall, ie1_npc, NULL, NULL,
      evt_ie1_door_open, evt_ie1_door_close },
};

EVT_BEGIN(evt_ie2_door_1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie2_door"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    RETURN
EVT_END()

EVT_BEGIN(evt_ie2_wall)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie2_kai"), FLOAT(0.0f), FLOAT(0.0f), LW(0))
    RETURN
EVT_END()

const char* ie2_npc[] = {
    "店員", NULL,
};

EVT_BEGIN(evt_ie2_door_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_ie2_door_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

DoorEntry ie2_door[] = {
    { 0, 3, 1, "S_ie2_door", "A__ie2_door2", "A__ie2_door2_u", "S_ie2_soto", NULL,
      "S_ie2_naka", NULL, evt_ie2_door_1, 0, evt_ie2_wall, ie2_npc, NULL, NULL,
      evt_ie2_door_open, evt_ie2_door_close },
};

EVT_BEGIN(evt_ie3_door_1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie3_door"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    RETURN
EVT_END()

EVT_BEGIN(evt_ie3_door_2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie3_door"), FLOAT(0.0f), FLOAT(0.0f), LW(0))
    RETURN
EVT_END()

EVT_BEGIN(evt_ie3_wall)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie3_kai"), FLOAT(0.0f), FLOAT(0.0f), LW(0))
    RETURN
EVT_END()

const char* ie3_npc[] = {
    "ホワイト", "ボムヘイG", NULL,
};

EVT_BEGIN(evt_ie3_door_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_ie3_door_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

const char* ie3_item[] = {
    "item_00", NULL,
};

DoorEntry ie3_door[] = {
    { 0, 3, 1, "S_ie3_door", "A_ie3_door3_u", "A_ie3_door3", "S_ie3_soto", NULL,
      "S_ie3_naka", NULL, evt_ie3_door_1, (s32)evt_ie3_door_2, evt_ie3_wall, ie3_npc, NULL, ie3_item,
      evt_ie3_door_open, evt_ie3_door_close },
};

static BeroEntry bero_entry_data[2] = {
    { "w_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "bom_01", "e_bero", 1, 1, 0, 0 },
    { 0 },
};

static EVT_BEGIN(evt_kime_pose)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_Y_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_Y_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("Y_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_Y_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_Y_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_Y_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_Y_1"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(evt_taihou)
    CALL(evt_cam3d_evt_set, 140, 100, 593, 140, 40, 248, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 180, 225, FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 160, 250, FLOAT(80.0f))
    CALL(evt_mario_set_dir, 270, 200, 0)
    WAIT_MS(500)
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_60"), 0, STRING("me"))
    WAIT_MS(300)
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(200)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL_CHILD(evt_kime_pose)
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_party_set_breed_pose, 0, 1)
    CALL(evt_msg_print_party, STRING("stg7_bom_60_2"))
    WAIT_MS(300)
    CALL(evt_party_run, 0)
    CALL(evt_mario_set_dir, 270, 200, 0)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg7_bom_61"), 0, STRING("me"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 50, 152, 800, 50, 57, 260, 3000, 11)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131088)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_move_position, STRING("me"), -45, 270, 0, FLOAT(60.0f), 1)
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir_npc, STRING("me"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir_npc, 0, STRING("me"))
    END_INLINE_EVENT
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_62"), 0, STRING("me"))
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("me"), -140, 200, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131088)
    SET_VALUE(GSW(0), 356)
    CALL(evt_cam3d_evt_off, 500, 11)
    RETURN
EVT_END()

EVT_BEGIN(urotuku_white)
    IF_INT_EQUAL(GSWF(3888), 1)
        CALL(evt_msg_print, 0, STRING("stg7_bom_41_01"), 0, STRING("me"))
        RETURN
    END_IF
    CALL(evt_cam3d_evt_set, 120, 97, 700, 120, 65, 251, 800, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg7_bom_39"), 0, STRING("me"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    CALL(evt_npc_set_position, STRING("ホワイト"), 230, 30, -125)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL_CHILD(evt_ie3_door_1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(evt_ie3_door_1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 132624)
    CALL(evt_npc_status_onoff, 1, STRING("ホワイト"), 2)
    CALL(evt_npc_move_position, STRING("ホワイト"), 200, -85, 0, FLOAT(60.0f), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg7_bom_41"), 0, STRING("ホワイト"))
        CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
        LABEL(3)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(evt_ie3_door_1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(3)
        END_IF
        CALL(evt_npc_move_position, STRING("ホワイト"), 230, -125, 0, FLOAT(80.0f), 1)
        CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
        LABEL(4)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(evt_ie3_door_1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(4)
        END_IF
        CALL(evt_npc_set_position, STRING("ホワイト"), 380, 45, -150)
        CALL(evt_npc_set_ry, STRING("ホワイト"), 270)
        CALL(evt_npc_flag_onoff, 0, STRING("ホワイト"), 1536)
        CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("N_1"))
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    CALL(evt_msg_print_add, 0, STRING("stg7_bom_40"))
    CALL(evt_msg_print_add, 0, STRING("stg7_bom_40_02"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSWF(3888), 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_soncho)
    SWITCH(GSW(0))
        CASE_LESS(356)
        CALL(evt_npc_set_position, STRING("me"), 120, 0, 265)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_soncho)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_20_00"), 0, STRING("me"))
        CASE_LESS(354)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            IF_INT_EQUAL(GSWF(3886), 0)
                SET_VALUE(GSWF(3886), 1)
                CALL(evt_msg_print, 0, STRING("stg7_bom_20"), 0, STRING("me"))
                WAIT_MS(300)
                CALL(evt_mario_set_pose, STRING("M_I_2"))
                WAIT_MS(800)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_msg_print, 0, STRING("stg7_bom_21"), 0, STRING("me"))
            ELSE
                CALL(evt_msg_print, 0, STRING("stg7_bom_21_01"), 0, STRING("me"))
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
                RETURN
            END_IF
            WAIT_MS(300)
            CALL(evt_party_stop, 0)
            WAIT_MS(300)
            CALL(evt_party_move_pos2, 0, 80, 300, FLOAT(60.0f))
            CALL(evt_party_set_dir_npc, 0, STRING("me"))
            CALL(evt_msg_print_party, STRING("stg7_bom_22"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_23"), 0, STRING("me"))
            CALL(evt_party_run, 0)
        ELSE
            BEGIN_INLINE_EVENT
                CALL(evt_party_move_pos, 0, 50, 280, 500)
                CALL(evt_party_set_dir_npc, 0, STRING("me"))
            END_INLINE_EVENT
            CALL(evt_mario_mov_pos, 70, 260, 500)
            CALL(evt_mario_set_dir, 90, 200, 0)
            CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
            IF_INT_EQUAL(GSWF(3886), 0)
                SET_VALUE(GSWF(3886), 1)
                CALL(evt_msg_print, 0, STRING("stg7_bom_20"), 0, STRING("me"))
                WAIT_MS(300)
                CALL(evt_mario_set_pose, STRING("M_I_2"))
                WAIT_MS(800)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_msg_print, 0, STRING("stg7_bom_21"), 0, STRING("me"))
            ELSE
                CALL(evt_msg_print, 0, STRING("stg7_bom_21_01"), 0, STRING("me"))
            END_IF
            CALL(evt_party_stop, 0)
            WAIT_MS(300)
            CALL(evt_party_move_pos2, 0, 80, 300, FLOAT(60.0f))
            CALL(evt_party_set_dir_npc, 0, STRING("me"))
            CALL(evt_msg_print, 0, STRING("stg7_bom_24"), 0, STRING("party"))
            WAIT_MS(300)
            CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 60)
            WAIT_MS(1000)
            CALL(evt_cam3d_evt_set, 90, 105, 611, 90, 45, 266, 500, 11)
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("stg7_bom_25"), 0, STRING("party"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_26"), 0, STRING("me"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_27"), 0, STRING("party"))
            WAIT_MS(300)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
            CALL(evt_msg_print, 0, STRING("stg7_bom_28"), 0, STRING("me"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_29"), 0, STRING("party"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_30"), 0, STRING("me"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_31"), 0, STRING("party"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_32"), 0, STRING("me"))
            WAIT_MS(300)
            CALL(evt_msg_print, 0, STRING("stg7_bom_32_02"), 0, STRING("party"))
            WAIT_MS(300)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("stg7_bom_33"), 0, STRING("me"))
            SET_VALUE(GSW(0), 354)
            CALL(evt_cam3d_evt_off, 500, 11)
            CALL(evt_party_run, 0)
        END_IF
        CASE_LESS(355)
        SET_VALUE(LW(0), 0)
        IF_INT_EQUAL(GSWF(3874), 1)
            ADD(LW(0), 1)
        END_IF
        IF_INT_EQUAL(GSWF(3876), 1)
            ADD(LW(0), 1)
        END_IF
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 0, STRING("stg7_bom_34"), 0, STRING("me"))
            CALL(evt_msg_select, 0, STRING("stg7_bom_34_yn"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print_add, 0, STRING("stg7_bom_38"))
            ELSE
                CALL(evt_msg_print_add, 0, STRING("stg7_bom_37"))
            END_IF
            CASE_EQUAL(1)
            IF_INT_EQUAL(GSWF(3874), 0)
                CALL(evt_msg_print, 0, STRING("stg7_bom_38_01"), 0, STRING("me"))
            END_IF
            IF_INT_EQUAL(GSWF(3876), 0)
                IF_INT_EQUAL(GSWF(3880), 0)
                    CALL(evt_msg_print, 0, STRING("stg7_bom_38_02"), 0, STRING("me"))
                ELSE
                    CALL_CHILD(urotuku_white)
                END_IF
            END_IF
        END_SWITCH
        CASE_LESS(356)
        CALL_CHILD(evt_taihou)
    END_SWITCH
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_e)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_e)
    IF_INT_EQUAL(GSW(1420), 30)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_30_001"), 0, 0)
            SET_VALUE(GSWF(5405), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_30_002"), 0, 0)
            IF_INT_EQUAL(GSWF(5406), 0)
                CALL(evt_msg_print_add, 0, STRING("irai_30_003"))
            ELSE
                CALL(evt_msg_continue)
                CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
                SET_VALUE(GF(0), 1)
                BEGIN_INLINE_EVENT
                    CALL(evt_party_move_pos2, 0, 175, 45, 60)
                    CALL(evt_party_set_dir_npc, 0, STRING("me"))
                    WAIT_MS(300)
                    SET_VALUE(GF(0), 0)
                END_INLINE_EVENT
                CALL(evt_mario_mov_pos2, 145, 45, 60)
                CALL(evt_mario_set_dir_npc, STRING("me"))
                WAIT_WHILE(GF(0))
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
                CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
                WAIT_FRAMES(1)
                CALL(evt_npc_wait_pera, STRING("me"))
                CALL(evt_mario_set_pose, STRING("M_I_2"))
                WAIT_MS(1000)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_msg_print, 0, STRING("irai_30_004"), 0, 0)
                CALL(evt_pouch_get_haveitemcnt, LW(0))
                CALL(evt_pouch_check_item, 1, LW(1))
                IF_INT_EQUAL(LW(1), 0)
                    SET_VALUE(LW(1), 10)
                ELSE
                    SET_VALUE(LW(1), 20)
                END_IF
                IF_INT_LESS(LW(0), LW(1))
                    CALL(evt_msg_continue)
                    CALL(evt_item_entry, STRING("sr"), 201, 0, -1000, 0, 17, -1, 0)
                    CALL(evt_item_get_item, STRING("sr"))
                    CALL(evt_item_get_item_end_wait, STRING("sr"))
                    SET_VALUE(GSWF(3890), 1)
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("irai_30_005_1"))
                    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
                    CALL(evt_npc_move_position, STRING("me"), 50, 35, 0, 60, 0)
                    CALL(evt_item_entry, STRING("iri_30"), 201, 50, 0, 35, 16, GSWF(3890), 0)
                    WAIT_MS(600)
                    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, 60, 0)
                    CALL(evt_npc_status_onoff, 0, STRING("me"), 2)
                END_IF
                CALL(evt_msg_print, 0, STRING("irai_30_006"), 0, 0)
                SET_VALUE(GSW(1420), 0)
                SET_VALUE(GSWF(5404), 1)
                CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
            END_IF
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_10_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_10"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_10_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_11"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_11_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_f)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_f)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_12_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_12"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_12_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_13"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_13_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_g)
    IF_INT_EQUAL(GSW(1420), 25)
        CALL(evt_npc_set_position, STRING("me"), 270, 30, -180)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        RETURN
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_g)
    IF_INT_EQUAL(GSW(1420), 25)
        IF_INT_EQUAL(GSWF(5405), 1)
            IF_INT_EQUAL(GSWF(5407), 0)
                CALL(evt_msg_print, 0, STRING("irai_25_00"), 0, 0)
                SET_VALUE(GSWF(5406), 1)
                RETURN
            END_IF
        END_IF
        IF_INT_EQUAL(GSWF(5409), 1)
            IF_INT_EQUAL(GSWF(5410), 0)
                CALL(evt_msg_print, 0, STRING("irai_25_01"), 0, 0)
                RETURN
            END_IF
        END_IF
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_14_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_14"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_14_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_15"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_15_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_h)
    RETURN
EVT_END()

s32 iri_26_item_tbl[] = {
    0xE5, -1,
};

static EVT_BEGIN(talk_murabito_h)
    IF_INT_EQUAL(GSW(1420), 26)
        CALL(evt_pouch_check_item, 229, LW(0))
        IF_INT_GREATER(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("irai_26_01"), 0, 0)
            CALL(evt_msg_select, 0, STRING("irai_26_01_s"))
            CALL(evt_msg_print_add, 0, STRING("irai_26_02"))
            CALL(evt_msg_select, 0, STRING("irai_26_02_s"))
            CALL(evt_msg_print_add, 0, STRING("irai_26_03"))
            CALL(evt_msg_select, 0, STRING("irai_26_03_s"))
            CALL(evt_msg_print_add, 0, STRING("irai_26_04"))
            CALL(evt_win_item_select, 0, PTR(iri_26_item_tbl), LW(0), 0)
            IF_INT_NOT_EQUAL(LW(0), -1)
                CALL(evt_pouch_remove_item, 229, LW(0))
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                LOOP(3)
                    CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 150, 5)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(evt_msg_print, 0, STRING("irai_26_05"), 0, 0)
                CALL(evt_sub_get_coin, 20)
                CALL(evt_sub_save_playtime, GSW(1461))
                SET_VALUE(GSW(1420), 0)
                SET_VALUE(GSWF(5400), 1)
                CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
            END_IF
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_26_00"), 0, 0)
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_16_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_16"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_16_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_17"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_17_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_i)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_i)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_18_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_18"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_18_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_19"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_19_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_tenin)
    CALL(evt_npc_set_position, STRING("me"), 125, 30, -450)
    RETURN
EVT_END()

EVT_BEGIN(talk_tenin)
    RETURN
EVT_END()

static EVT_BEGIN(init_white)
    IF_INT_EQUAL(GSW(1420), 25)
        IF_INT_EQUAL(GSWF(5409), 1)
            IF_INT_EQUAL(GSWF(5411), 0)
                CALL(evt_npc_set_position, STRING("ホワイト"), 380, 45, -150)
                CALL(evt_npc_set_ry, STRING("ホワイト"), 270)
                IF_INT_EQUAL(GSWF(5410), 1)
                    CALL(evt_npc_flag_onoff, 0, STRING("me"), 16777216)
                    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1536)
                ELSE
                    CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("N_1"))
                    CALL(evt_npc_set_width, STRING("ホワイト"), 10)
                    CALL(evt_npc_change_interrupt, STRING("ホワイト"), 6, 0)
                END_IF
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(3880), 1)
        IF_INT_EQUAL(GSWF(3876), 0)
            CALL(evt_npc_set_width, STRING("ホワイト"), 10)
            CALL(evt_npc_set_position, STRING("ホワイト"), 380, 45, -150)
            CALL(evt_npc_set_ry, STRING("ホワイト"), 270)
            CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("N_1"))
            CALL(evt_npc_change_interrupt, STRING("ホワイト"), 6, 0)
        END_IF
    END_IF
    RETURN
EVT_END()

// The symbol map starts lbl_3_data_76CC inside mario_jump's last command, CALL(evt_npc_flag_onoff, 0, "ホワイト",
// 32), so the script is written across the two arrays.
EVT_BEGIN(mario_jump)
    CALL(evt_mario_cam_keep_on)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 32)
    CALL(evt_npc_get_position, STRING("ホワイト"), LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 500, 2, -1000)
    BEGIN_INLINE_EVENT
        SUBTRACT(LW(0), 20)
        SUBTRACT(LW(2), 20)
        CALL(evt_eff, 0, STRING("nice"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_eff, 0, STRING("small_star"), 3, LW(0), LW(1), LW(2), 0, -1, 0, 3, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WHITE_DAMAGED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WHITE_NICE1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 100)
    END_INLINE_EVENT
    ADD(LW(2), -10)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP4"), LW(10), LW(11), LW(12), 0)
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 500, 2, -1000)
    BEGIN_INLINE_EVENT
        SUBTRACT(LW(0), 20)
        SUBTRACT(LW(2), 20)
        CALL(evt_eff, 0, STRING("nice"), 1, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_eff, 0, STRING("small_star"), 3, LW(0), LW(1), LW(2), 0, -1, 0, 3, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WHITE_DAMAGED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WHITE_GOOD1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 100)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP2"), LW(10), LW(11), LW(12), 0)
    CALL(evt_mario_jump_pos, 335, 30, -130, 500, 2, -1000)
    EVT_CMD(4, OPCODE_USER_FUNC), PTR(evt_npc_flag_onoff), 0,
};

s32 lbl_3_data_76CC[] = {
    STRING("ホワイト"), 32,
    CALL(evt_mario_cam_keep_off)
    RETURN
EVT_END()

s32 iri_25_item_tbl[] = {
    0x6D, -1,
};

static EVT_BEGIN(talk_white)
    IF_INT_EQUAL(GSW(1420), 25)
        IF_INT_EQUAL(GSWF(5409), 1)
            IF_INT_EQUAL(GSWF(5411), 0)
                SET_VALUE(LF(0), 0)
                IF_INT_EQUAL(GSWF(5410), 1)
                    SET_VALUE(LF(0), 1)
                END_IF
                IF_INT_EQUAL(LF(0), 1)
                    CALL(evt_msg_print, 0, STRING("irai_25_19"), 0, STRING("ホワイト"))
                END_IF
                CALL(evt_win_item_select, 0, PTR(iri_25_item_tbl), LW(0), 0)
                IF_INT_NOT_EQUAL(LW(0), 109)
                    RETURN
                END_IF
                CALL(evt_msg_print, 0, STRING("irai_25_18"), 0, STRING("ホワイト"))
                CALL(evt_pouch_remove_item, 109, LW(0))
                SET_VALUE(GSWF(5411), 1)
                BEGIN_INLINE_EVENT
                    WAIT_MS(1000)
                    CALL(evt_mario_set_dir, 270, 300, 0)
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    WAIT_MS(1000)
                    CALL(evt_party_set_dir_npc, 0, STRING("ホワイト"))
                END_INLINE_EVENT
                CALL(evt_npc_set_width, STRING("ホワイト"), 40)
                CALL(evt_npc_status_onoff, 1, STRING("ホワイト"), 2)
                CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 32)
                CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 131088)
                CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), 245, 30, -110, 500, 80)
                CALL(evt_npc_move_position, STRING("ホワイト"), 170, -55, 0, FLOAT(80.0f), 1)
                CALL(evt_npc_set_position, STRING("ホワイト"), 0, -1000, 0)
                RETURN
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_25_white_bed)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print, 0, STRING("irai_25_02"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_mario_mov_pos2, 330, -120, 60)
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(200)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_03"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_04"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_05"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_06"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_07"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_08"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_09"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_10"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_11"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("irai_25_12"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_25_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    GOTO(101)
    LABEL(100)
    CALL(evt_msg_print_add, 0, STRING("irai_25_13"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    RETURN
    LABEL(101)
    CALL(evt_party_run, 0)
    CALL(evt_msg_print, 0, STRING("irai_25_14"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("N_2"))
    WAIT_MS(600)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WHITE_ROUND1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("O_1"))
    WAIT_MS(1900)
    CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("O_2"))
    WAIT_MS(1200)
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 1536)
    CALL(evt_msg_print, 0, STRING("irai_25_15"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("irai_25_16"), 0, STRING("ホワイト"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("irai_25_17"), 0, STRING("ホワイト"))
    CALL_CHILD(talk_white)
    CALL(evt_npc_change_interrupt, STRING("ホワイト"), 6, PTR(talk_white))
    CALL(evt_npc_set_width, STRING("ホワイト"), 30)
    SET_VALUE(GSWF(5410), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_white_bed)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print, 0, STRING("stg7_bom_43"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_43_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 32)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("ホワイト"), LW(3), LW(4), LW(5))
    CALL(evt_sub_get_dir, LW(3), LW(5), LW(0), LW(2), LW(6))
    IF_INT_GREATER_OR_EQUAL(LW(6), 80)
        IF_INT_LESS_OR_EQUAL(LW(6), 100)
            CALL(evt_mario_mov_pos2, 376, -111, 60)
        END_IF
    END_IF
    CALL(evt_mario_mov_pos2, 330, -120, 60)
    WAIT_FRAMES(1)
    CALL(evt_npc_flag_onoff, 0, STRING("ホワイト"), 32)
    CALL(evt_mario_get_dispdir, LW(0))
    IF_INT_LESS(LW(0), 180)
        CALL(evt_mario_set_dir, 90, 200, 0)
        WAIT_MS(200)
    END_IF
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_44"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_44_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_45"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_45_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_46"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_46_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_47"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_47_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_48"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_48_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_49"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_49_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_50"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_50_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_51"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_51_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_52"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_52_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    CALL(evt_msg_print, 0, STRING("stg7_bom_53"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg7_bom_53_yn"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(evt_msg_continue)
    CALL_CHILD(mario_jump)
    GOTO(1)
    LABEL(0)
    CALL(evt_msg_print_add, 0, STRING("stg7_bom_54"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
    LABEL(1)
    CALL(evt_msg_print, 0, STRING("stg7_bom_55"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("N_2"))
    WAIT_MS(600)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WHITE_ROUND1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("O_1"))
    WAIT_MS(1900)
    CALL(evt_npc_set_anim, STRING("ホワイト"), STRING("O_2"))
    WAIT_MS(1200)
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 1536)
    CALL(evt_msg_print, 0, STRING("stg7_bom_56"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg7_bom_57"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg7_bom_58"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg7_bom_59"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_set_dir, 270, 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_party_set_dir_npc, 0, STRING("ホワイト"))
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("ホワイト"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 131088)
    CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), 245, 30, -110, 500, 80)
    CALL(evt_npc_move_position, STRING("ホワイト"), 170, -55, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_set_position, STRING("ホワイト"), 0, -1000, 0)
    SET_VALUE(GSWF(3876), 1)
    SET_VALUE(LW(0), 0)
    IF_INT_EQUAL(GSWF(3874), 1)
        ADD(LW(0), 1)
    END_IF
    IF_INT_EQUAL(GSWF(3876), 1)
        ADD(LW(0), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        SET_VALUE(GSW(0), 355)
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(kiza_talk)
    SWITCH(GSW(0))
        CASE_LESS(354)
        CALL(evt_msg_print, 0, STRING("stg7_bom_19_01"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_19_02"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_19_03"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(kiza_entry)
    CALL(evt_npc_entry, STRING("キザ野郎"), STRING("c_kiza"))
    CALL(evt_npc_set_tribe, STRING("キザ野郎"), STRING("キザ野郎"))
    CALL(evt_npc_set_position, STRING("キザ野郎"), -190, 5, -130)
    CALL(evt_npc_set_ry, STRING("キザ野郎"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("キザ野郎"), 1536)
    CALL(evt_npc_change_interrupt, STRING("キザ野郎"), 6, PTR(kiza_talk))
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    { "村長", 0x40000600, 0, init_soncho, NULL, talk_soncho, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { "ボムヘイE", 0x40000600, 0, init_murabito_e, NULL, talk_murabito_e, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { "ボムヘイF", 0x40000600, 0, init_murabito_f, NULL, talk_murabito_f, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { "ボムヘイG", 0x40000600, 0, init_murabito_g, NULL, talk_murabito_g, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { "ボムヘイH", 0x40000600, 0, init_murabito_h, NULL, talk_murabito_h, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { "ボムヘイI", 0x40000600, 0, init_murabito_i, NULL, talk_murabito_i, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { "店員", 0x40000600, 0, init_tenin, NULL, talk_tenin, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { "ホワイト", 0x41000000, 0, init_white, NULL, talk_white, NULL, NULL, NULL, NULL, NULL, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0, 0, 0, 0, 0 },
    { NULL },
};

static char* obj_list[] = {
    "S_item_01", "A_item_01", "S_item_02", "A_item_02",
    "S_item_03", "A_item_03", "S_item_04", "A_item_04",
    "S_item_05", "A_item_05", "S_item_06", "A_item_06",
};

static s32 goods_list[] = {
    0x82, 0x1E, 0x83, 0x06,
    0x89, 0x0F, 0x8C, 0x0C,
    0x9B, 0x14, 0x95, 0x0F,
};

static s32 trade_list[] = {
    0x82, 0x000F0000, 0x83, 0x00030000,
    0x89, 0x00070000, 0x8C, 0x00060000,
    0x9B, 0x000A0000, 0x95, 0x00070000,
    0x84, 0x00080000, 0xAD, 0x000E0000,
    0xE0, 0x003C0000, 0xC0, 0x00140000,
    0xDC, 0x000F0000, 0xE4, 0x00180000,
    0, 0,
};

static char* shopper_data[] = {
    "店員", NULL, NULL, "shop_00",
    "shop_01", "shop_02", "shop_03", "shop_04",
    "shop_05", "shop_06", "shop_07", "shop_08",
    "shop_09", "shop_10", "shop_11", "shop_12",
    "shop_13", "shop_14", "shop_15", "shop_16",
    "shop_17", "shop_18", "shop_19", "shop_20",
    "shop_21", "shop_22", "shop_23", "shop_24",
    "shop_25", "shop_26", "shop_27", "shop_28",
    "shop_29", "shop_30", "shop_31", "shop_32",
    "shop_33", "shop_34",
};

const Vec vec3_bom_00007cc0 = {0.0f, 100.0f, 0.0f};
const Vec vec3_bom_00007ccc = {0.0f, 1.0f, 0.0f};
extern const f32 float_100_bom_00007e7c;

static USER_FUNC(yami_view) {
    Vec camPos;
    Vec up;
    Vec target;
    Mtx mtx;
    f32 value = evtGetFloat(event, event->args[0]);

    target = (Vec){0.0f, 0.0f, 0.0f};
    target.z = value;
    up = vec3_bom_00007ccc;
    camPos = vec3_bom_00007cc0;
    camPos.z = float_100_bom_00007e7c + value;
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
    CALL(evt_npc_set_position, STRING("村長"), -30, 0, 175)
    CALL(evt_npc_set_ry, STRING("村長"), 90)
    CALL(evt_npc_set_position, STRING("ボムヘイE"), -160, 0, -40)
    CALL(evt_npc_set_position, STRING("ボムヘイF"), -165, 0, 295)
    CALL(evt_npc_set_position, STRING("ボムヘイG"), 117, 0, 251)
    CALL(evt_npc_status_onoff, 1, STRING("ボムヘイE"), 2)
    CALL(evt_npc_status_onoff, 1, STRING("ボムヘイF"), 2)
    CALL(evt_npc_status_onoff, 1, STRING("ボムヘイG"), 2)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bom02"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bom02"), 256)
    CALL(anim_tevcallback, STRING("村長"), 1)
    CALL(anim_tevcallback, STRING("ボムヘイE"), 1)
    CALL(anim_tevcallback, STRING("ボムヘイF"), 1)
    CALL(anim_tevcallback, STRING("ボムヘイG"), 1)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(yami_view, -800)
    CALL(evt_cam3d_evt_set, -30, 88, 723, -30, 55, 24, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_QUAKE1"), LW(15))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイE"), 10, -40, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイE"), 190, -40, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイE"), 10, -40, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイE"), -160, -40, 0, 120, 0)
        END_LOOP
        WAIT_MS(2000)
        CALL(evt_npc_reverse_ry, STRING("ボムヘイE"))
        WAIT_FRAMES(1)
        CALL(evt_npc_wait_pera, STRING("ボムヘイE"))
        CALL(evt_npc_reverse_ry, STRING("ボムヘイE"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイF"), -62, 295, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイF"), 44, 295, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイF"), 131, 295, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイF"), 44, 295, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイF"), -62, 295, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイF"), -165, 295, 0, 120, 0)
        END_LOOP
        WAIT_MS(2000)
        CALL(evt_npc_reverse_ry, STRING("ボムヘイF"))
        WAIT_FRAMES(1)
        CALL(evt_npc_wait_pera, STRING("ボムヘイF"))
        CALL(evt_npc_reverse_ry, STRING("ボムヘイF"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイG"), 26, 251, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイG"), -83, 251, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイG"), -180, 251, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイG"), -83, 251, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイG"), 26, 251, 0, 120, 0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_move_position, STRING("ボムヘイG"), 117, 251, 0, 120, 0)
        END_LOOP
        WAIT_MS(2000)
        CALL(evt_npc_reverse_ry, STRING("ボムヘイG"))
        WAIT_FRAMES(1)
        CALL(evt_npc_wait_pera, STRING("ボムヘイG"))
        CALL(evt_npc_reverse_ry, STRING("ボムヘイG"))
    END_INLINE_EVENT
    SET_FLOAT_VALUE(LW(0), FLOAT(0.007f))
    LOOP(5)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), LW(0), 1000)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.75f))
    END_LOOP
    SET_VALUE(GF(0), 1)
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(2000)
    CALL(evt_npc_reverse_ry, STRING("村長"))
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("村長"))
    CALL(evt_npc_reverse_ry, STRING("村長"))
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("村長"))
    CALL(evt_cam3d_evt_set, -30, 55, 473, -30, 35, 24, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg8_dark_08"), 0, STRING("村長"))
    WAIT_MS(2000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("las_26"), STRING("sekai_yami2"))
    RETURN
EVT_END()

static USER_FUNC(anim_tevcallback_mobj) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);

    animPoseSetMaterialFlagOn(mobjNameToPtr((const char*)name)->poseId, (flag & 0xF) << 24);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(minnnanokoe_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("村長"), 50, 0, 200)
    CALL(evt_npc_set_ry, STRING("村長"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("村長"), 32768)
    CALL(evt_npc_set_position, STRING("ボムヘイE"), -250, 0, 150)
    CALL(evt_npc_set_ry, STRING("ボムヘイE"), 90)
    CALL(evt_npc_status_onoff, 1, STRING("ボムヘイE"), 2)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bom02"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bom02"), 256)
    CALL(anim_tevcallback, STRING("村長"), 1)
    CALL(anim_tevcallback, STRING("ボムヘイE"), 1)
    CALL(anim_tevcallback_mobj, STRING("mobj_save"), 1)
    CALL(yami_view, -800)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(evt_cam3d_evt_set, 50, 218, 1025, 50, 70, 188, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 0, STRING("村長"), 32768)
    CALL(evt_cam3d_evt_set, 30, 92, 484, 30, 40, 188, 2000, 11)
    CALL(evt_npc_move_position, STRING("ボムヘイE"), 0, 150, 2000, 0, 0)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_63"), 0, STRING("ボムヘイE"))
    CALL(evt_npc_set_ry, STRING("村長"), 270)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_64"), 0, STRING("村長"))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("ボムヘイE"), -450, -50, 0, 90, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("村長"), -400, 0, 0, 90, 0)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("bom_01"), STRING("minnnanokoe"))
    RETURN
EVT_END()

// TODO: type
static u32 kino_dt[38] = {
    0x000000C6, 0xC3D48000, 0x42480000, 0xC3480000,
    0x0000000F, 0xC38E8000, 0x42380000, 0xC35F0000,
    0xC3888000, 0x41F00000, 0xC3310000, 0xC3DE8000,
    0x43810000, 0x42B00000, 0xC3AA8000, 0x42B80000,
    0xC33F0000, 0xC1C80000, 0xC3A00000, 0x41F00000,
    0xC2E60000, (u32)"宿店員", (u32)"yado_00", (u32)"yado_01",
    (u32)"yado_02", (u32)"yado_03", (u32)"yado_04", (u32)"yado_05",
    (u32)"yado_06", (u32)"yado_07", (u32)"yado_08", (u32)"yado_09",
    (u32)"yado_10", (u32)"yado_11", (u32)"yado_12", (u32)"yado_13",
    0x00000000, 0x00000000,
};

EVT_BEGIN(bom_02_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami2"))
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_VOICE1"))
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_BOM1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_BOM3"))
    END_IF
    CALL_CHILD(evt_bom_snowfall)
    CALL(evt_npc_setup, PTR(npcEnt))
    SWITCH(GSW(0))
        CASE_BETWEEN(352, 357)
        CALL_CHILD(kiza_entry)
    END_SWITCH
    CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list), PTR(shopper_data), PTR(trade_list))
    CALL(evt_kinopio_setup, PTR(kino_dt))
    SET_VALUE(LW(0), PTR(ie1_door))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(ie2_door))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(ie3_door))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), STRING("group3713047r"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group3713048r"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group3713049r"))
    SPAWN_EVENT(evt_bom_01_karakara)
    CALL(evt_mobj_powerupblk, STRING("pblk01"), 425, 0, -45, 0, GSWF(5565), 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -30, 0, 115, STRING("S_wakuaaa"), 125, GSWF(5661))
    CALL(evt_item_entry, STRING("item_00"), 125, 210, 30, -168, 16, GSWF(5660), 0)
    CALL(evt_item_entry, STRING("item_01"), 164, -220, 5, -115, 16, GSWF(3892), 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -300, 65, 10, 0, 0)
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami2"))
            SPAWN_EVENT(sekai_yami_evt)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SPAWN_EVENT(minnnanokoe_evt)
        END_IF
    END_IF
    CALL(evt_eff, 0, STRING("nice"), 0, 0, -1000, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("nice"), 1, 0, -1000, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_map_playanim, STRING("yuge"), 1, 0)
    IF_INT_EQUAL(GSWF(3880), 1)
        IF_INT_EQUAL(GSWF(3876), 0)
            CALL(evt_run_case_evt, 9, 1, STRING("A_w_bed"), 0, PTR(evt_white_bed), 0)
        END_IF
    END_IF
    CALL(evt_map_replace_mapobj, STRING("hi1"), 1)
    CALL(evt_map_replace_mapobj, STRING("hi2"), 1)
    CALL(evt_map_replace_mapobj, STRING("hi3"), 1)
    CALL(evt_map_replace_mapobj, STRING("hi4"), 1)
    CALL(evt_map_replace_mapobj, STRING("hi5"), 1)
    CALL(evt_map_replace_mapobj, STRING("hi6"), 1)
    IF_INT_EQUAL(GSW(1420), 25)
        IF_INT_EQUAL(GSWF(5409), 1)
            IF_INT_EQUAL(GSWF(5411), 0)
                IF_INT_EQUAL(GSWF(5410), 0)
                    CALL(evt_run_case_evt, 9, 1, STRING("A_w_bed"), 0, PTR(iri_25_white_bed), 0)
                END_IF
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        IF_INT_EQUAL(GSWF(5404), 1)
            CALL(evt_item_entry, STRING("iri_30"), 201, 50, 0, 35, 16, GSWF(3890), 0)
        END_IF
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie1_naka"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie2_naka"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie3_naka"), 2)
    RETURN
EVT_END()
// clang-format on
const f32 float_100_bom_00007e7c = 100.0f;
#pragma force_active off

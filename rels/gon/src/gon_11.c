#include "gon_11.h"

#include <stdio.h>
#include <string.h>

#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "driver/imgdrv.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_light.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party.h"
#include "party/party_cloud.h"
#include "party/party_motion.h"
#include "pmario_sound.h"

#pragma pool_data off

#pragma force_active on

static CloudEntry* cloud;
static s8 cloud_once_flag;

static USER_FUNC(gonbaba_wait_landon);
static s32 _img_alloc_capture(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);
static s32 _get_boundingbox(const char* name, s32* left, s32* top, s32* width, s32* height);
static void _mapsetofs_all(CloudEntry* cp);
static void _mapclsofs_all(CloudEntry* cp);
static USER_FUNC(evt_cloud_main);

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(evt_sound)
    CALL(evt_snd_bgmoff, 513)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door01_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door03"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door04"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door01_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door03"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door04"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"w_bero", 1, 13, 0x4E20, 100000, 0, 0, -1, NULL, -1, evt_sound, "gon_10", "w_bero", 5, 5, door01_close, door01_open},
    {NULL},
};

static EVT_BEGIN(gonbaba_standup_shake)
    WAIT_MS(3700)
    CALL(evt_npc_get_position, STRING("ゴンババ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_GNB_STEP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 800)
    RETURN
EVT_END()

static EVT_BEGIN(gonbaba_fog_change)
    CALL(evt_map_get_fog, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    SET_VALUE(GW(0), LW(5))
    SET_VALUE(GW(1), LW(6))
    CONVERT_MS_TO_FRAME(LW(10), 8000)
    ADD(LW(10), 1)
    BEGIN_INLINE_EVENT
        LOOP(LW(10))
            CALL(evt_map_set_fog, LW(4), GW(0), GW(1), LW(7), LW(8), LW(9))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_init, 11, 0, 400, LW(10))
        LOOP(LW(10))
            CALL(evt_sub_intpl_get_value)
            SET_VALUE(GW(0), LW(0))
            ADD(GW(0), LW(5))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_init, 11, 0, 1600, LW(10))
    LOOP(LW(10))
        CALL(evt_sub_intpl_get_value)
        SET_VALUE(GW(1), LW(0))
        ADD(GW(1), LW(6))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(gonbaba_down_shake)
    CALL(evt_npc_get_position, STRING("ゴンババ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_STG1_GNB_DOWN1"), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_GNB_DOWN2"), LW(0), LW(1), LW(2), LW(8))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 90, 0, 10000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_snd_sfx_vol, LW(8), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        CALL(evt_snd_sfxoff, LW(8))
    END_INLINE_EVENT
    CALL(evt_cam_shake, 4, FLOAT(0.05f), FLOAT(0.0f), 500)
    CALL(evt_sub_intpl_init, 11, 50, 5, 50)
    LOOP(0)
        CALL(evt_sub_intpl_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
        CALL(evt_cam_shake, 4, LW(0), FLOAT(0.0f), 50)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 2000)
    RETURN
EVT_END()

static USER_FUNC(gonbaba_wait_landon) {
    NpcEntry* npc = npcNameToPtr("ゴンババ");
    return (*(u32*)npc->field_300 != 0) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(gonbaba_event)
    CALL(evt_hitobj_onoff, STRING("baba"), 1, 0)
    CALL(evt_npc_pera_onoff, STRING("ゴンババ"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ゴンババ"), 512)
    CALL(evt_map_set_fog, 2, 400, 1000, 0, 10, 40)
    CALL(evt_map_fog_onoff, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    IF_INT_EQUAL(GSWF(1498), 1)
        GOTO(100)
    END_IF
    CALL(evt_cam3d_evt_set, -480, 150, 459, -480, 67, -13, 0, 11)
    CALL(evt_mario_set_pos, -560, 10, 0)
    CALL(evt_party_set_pos, 0, -600, 10, 0)
    CALL(evt_seq_wait, 2)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(1), 0)
        SPAWN_EVENT(bero_close_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL_CHILD(door01_close)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_cam_shake, 4, FLOAT(0.015f), FLOAT(0.0f), 200)
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 100)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -490, 0, FLOAT(120.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -450, 0, FLOAT(120.0f))
    SPAWN_EVENT(bero_case_entry)
    WAIT_MS(1300)
    CALL(evt_msg_print, 0, STRING("stg1_gon_19"), 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 96)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 96)
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    WAIT_MS(2000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_GONBABA_FLY2"))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d_ex, STRING("SFX_STG1_GNB_ROAR2"), -900, 30, 490, 255, 255, 4300, 16, 0)
        CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_U_1"))
        CALL(evt_npc_wait_anim, STRING("ゴンババ"), FLOAT(1.0f))
        CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_S_1"))
    END_INLINE_EVENT
    SPAWN_EVENT(gonbaba_standup_shake)
    SPAWN_EVENT(gonbaba_fog_change)
    CALL(evt_cam3d_evt_set, -900, 31, 490, -205, 175, 20, 4500, 11)
    BEGIN_INLINE_EVENT
        WAIT_MS(1800)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door10"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kesu"), 1)
    END_INLINE_EVENT
    WAIT_MS(4000)
    LOOP(30)
        CALL(evt_cam3d_evt_set, -890, 33, 483, -205, 175, 20, 15, 11)
        WAIT_MS(15)
        CALL(evt_cam3d_evt_set, -900, 31, 490, -205, 175, 20, 15, 11)
        WAIT_MS(15)
    END_LOOP
    WAIT_MS(4500)
    SET_VALUE(LW(0), -246)
    SET_VALUE(LW(1), 146)
    SET_VALUE(LW(2), 43)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_19_2"), 0, STRING("ゴンババ"))
    CALL(evt_cam3d_evt_set, -545, -14, 417, -225, 227, 12, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("door10"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kesu"), 1)
    WAIT_MS(1000)
    SET_VALUE(LW(0), -246)
    SET_VALUE(LW(1), 146)
    SET_VALUE(LW(2), 43)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_20"), 0, STRING("ゴンババ"))
    CALL(evt_cam3d_evt_set, 77, 455, 380, -147, 293, 183, 0, 11)
    WAIT_MS(500)
    SET_VALUE(LW(0), -246)
    SET_VALUE(LW(1), 146)
    SET_VALUE(LW(2), 43)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg1_gon_20_2"), 0, STRING("ゴンババ"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -325, 30, 650, -350, 125, -33, 3000, 11)
    WAIT_MS(2500)
    CALL(evt_npc_set_battle_info, STRING("ゴンババ"), 20)
    CALL(evt_npc_battle_start, STRING("ゴンババ"))
    CALL(evt_npc_wait_battle_end)
    LABEL(100)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_map_fog_onoff, 0)
    CALL(evt_hitobj_onoff, STRING("baba"), 1, 1)
    CALL(evt_cam3d_evt_set, 0, 30, 842, 0, 135, 150, 0, 11)
    CALL(evt_light_mario_lighting_onoff, 0)
    CALL(evt_light_party_lighting_onoff, 0)
    CALL(evt_light_npc_lighting_onoff, 0, STRING("ノコタロウ父"))
    CALL(evt_light_npc_lighting_onoff, 0, STRING("クリスチーヌ"))
    CALL(evt_party_stop, 0)
    CALL(evt_npc_set_position, STRING("ゴンババ"), 0, 70, -100)
    CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_H_1"))
    CALL(evt_mario_set_pos, 25, 10, 150)
    CALL(evt_party_set_pos, 0, -25, 10, 150)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 90, -1)
    SPAWN_EVENT(gonbaba_down_shake)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_flag_onoff, 1, STRING("ゴンババ"), 131088)
        CALL(gonbaba_wait_landon)
        CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_H_2"))
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 25, 423, 0, 55, 150, 5000, 11)
    WAIT_MS(5000)
    CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_H_3"))
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        LOOP(0)
            CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_H_3"))
            CALL(evt_npc_get_position, STRING("ゴンババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_GNB_DOWN3"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(500)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_GNB_DOWN3"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(2000)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_GNB_DOWN3"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(166)
            WAIT_MS(2333)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_msg_print_party, STRING("stg1_gon_33_01"))
    CALL(evt_party_set_dir, 0, 270, -1)
    WAIT_MS(800)
    CALL(evt_party_set_dir, 0, 90, -1)
    WAIT_MS(800)
    CALL(evt_msg_print_party, STRING("stg1_gon_33_02"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_msg_toge, 2, 220, 160, -160)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_03"), 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("eff0"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("eff1"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_cam3d_evt_set, 255, 220, 648, 255, 170, 150, 1000, 11)
    WAIT_MS(1000)
    DELETE_EVENT(LW(10))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ゴンババ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_OUT1_1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_OUT1_2"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_OUT1_2"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_OUT1_2"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1100)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_OUT1_3"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_H_4"))
    WAIT_MS(2750)
    SET_VALUE(GW(10), 30)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(evt_npc_set_anim, STRING("ノコタロウ父"), STRING("NKP_Z_2"))
        CALL(evt_npc_flag_onoff, 0, STRING("ノコタロウ父"), 1024)
        CALL(evt_npc_pera_onoff, STRING("ノコタロウ父"), 0)
        SET_VALUE(LW(0), 0)
        LOOP(0)
            ADD(LW(0), GW(10))
            CALL(evt_npc_set_ry, STRING("ノコタロウ父"), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_set_ry, STRING("ノコタロウ父"), 90)
    CALL(evt_npc_set_position, STRING("ノコタロウ父"), 280, 60, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_OUT2"), 280, 60, 0, LW(10))
        CALL(evt_npc_jump_position_nohit, STRING("ノコタロウ父"), 275, 10, 200, 2000, 500)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_LANDING1"), 275, 10, 200, 0)
        CALL(evt_npc_get_position, STRING("ノコタロウ父"), LW(0), LW(1), LW(2))
        ADD(LW(1), 7)
        ADD(LW(2), 10)
        SET_VALUE(LW(3), LW(0))
        SUBTRACT(LW(3), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(3), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        SET_VALUE(LW(3), LW(0))
        ADD(LW(3), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(3), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(1)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_npc_set_anim, STRING("ゴンババ"), STRING("GNB_H_3"))
    CALL(evt_cam3d_evt_set, 255, 105, 648, 255, 55, 150, 500, 11)
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("ノコタロウ父"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKOPAPA_OUT3"), LW(0), LW(1), LW(2), 0)
    LOOP(0)
        SUBTRACT(GW(10), 1)
        IF_INT_EQUAL(GW(10), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(2)
    END_LOOP
    DELETE_EVENT(LW(10))
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_npc_set_anim, STRING("ノコタロウ父"), STRING("NKP_S_1"))
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("ノコタロウ父"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("ノコタロウ父"), LW(0), LW(1), LW(2), 300, 50)
    CALL(evt_npc_pera_onoff, STRING("ノコタロウ父"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("ノコタロウ父"), 1024)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_04"), 0, STRING("ノコタロウ父"))
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        CALL(evt_mario_goodbye_party, 0)
        CALL(evt_mario_hello_party, 0, 2)
        CALL(evt_party_stop, 0)
        SET_VALUE(GSWF(1500), 1)
    END_IF
    CALL(evt_mario_set_pos, 25, 10, 200)
    CALL(evt_party_set_pos, 0, -25, 10, 200)
    IF_INT_EQUAL(GSWF(1500), 1)
        CALL(evt_npc_set_position, STRING("クリスチーヌ"), -10, 10, 160)
        CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 32768)
        CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
        CALL(evt_npc_status_onoff, 1, STRING("クリスチーヌ"), 2)
    END_IF
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_05"), 0, STRING("party"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("ノコタロウ父"), 270)
    END_INLINE_EVENT
    WAIT_MS(500)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_06"), 0, STRING("ノコタロウ父"))
    CALL(evt_cam3d_evt_set, 0, 75, 450, 0, 50, 200, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_NOKO_SAIKAI1"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_07"), 0, STRING("party"))
    CALL(evt_cam3d_evt_set, 240, 75, 450, 240, 50, 200, 1000, 11)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 215, 200, FLOAT(80.0f))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_08"), 0, STRING("ノコタロウ父"))
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    IF_INT_EQUAL(GSWF(1500), 1)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_move_position, STRING("クリスチーヌ"), 150, 200, 0, FLOAT(80.0f), 1)
        END_INLINE_EVENT
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 180, 220, FLOAT(80.0f))
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_09"), 0, STRING("party"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_10"), 0, STRING("ノコタロウ父"))
    CALL(evt_cam3d_evt_set, 500, 380, 856, 250, 130, 190, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_11"), 0, STRING("party"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_12"), 0, STRING("ノコタロウ父"))
    CALL(evt_cam3d_evt_set, 220, 75, 450, 220, 50, 200, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_13"), 0, STRING("party"))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ノコタロウ父"), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 270, 75, 450, 270, 50, 200, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_14"), 0, STRING("ノコタロウ父"))
    CALL(evt_cam3d_evt_set, 245, 75, 450, 245, 50, 200, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ノコタロウ父"), STRING("NKP_A_1"))
    CALL(evt_item_entry, STRING("stone"), 114, 245, 45, 220, 17, -1, 0)
    WAIT_MS(500)
    CALL(evt_item_get_position, STRING("stone"), LW(0), LW(1), LW(2))
    SET_FLOAT_VALUE(LW(3), FLOAT(-1.0f))
    SET_FLOAT_VALUE(LW(4), FLOAT(-0.4f))
    SET_VALUE(LF(0), 0)
    CALL(evt_npc_set_anim, STRING("ノコタロウ父"), STRING("NKP_A_2"))
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_npc_set_anim, STRING("ノコタロウ父"), STRING("NKP_S_1"))
    END_INLINE_EVENT
    LOOP(0)
        ADD_FLOAT(LW(1), LW(3))
        ADD_FLOAT(LW(3), LW(4))
        CALL(evt_item_set_position, STRING("stone"), LW(0), LW(1), LW(2))
        IF_FLOAT_LESS(LW(1), FLOAT(10.0f))
            IF_INT_EQUAL(LF(0), 0)
                CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_STARSTONE_JUMP1"), LW(0), LW(1), LW(2), 0)
                MULTIPLY_FLOAT(LW(3), FLOAT(-0.7f))
                SET_FLOAT_VALUE(LW(1), FLOAT(10.0f))
                SET_VALUE(LF(0), 1)
            ELSE
                CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_STARSTONE_JUMP1"), LW(0), LW(1), LW(2), 0)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_item_set_position, STRING("stone"), LW(0), FLOAT(10.0f), LW(2))
    CALL(evt_npc_set_anim, STRING("ノコタロウ父"), STRING("NKP_S_1"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_15"), 0, STRING("party"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_16"), 0, STRING("ノコタロウ父"))
    CALL(evt_cam3d_evt_set, 190, 75, 450, 190, 50, 200, 500, 11)
    WAIT_MS(600)
    CALL(evt_mario_set_pose, STRING("M_I_G"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_33_17"), 0, STRING("ノコタロウ父"))
    CALL(evt_party_set_dir, 0, 270, -1)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, -1)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 245, 75, 450, 245, 50, 245, 2000, 11)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 245, 215, FLOAT(30.0f))
    END_INLINE_EVENT
    WAIT_MS(2000)
    SET_VALUE(GSW(0), 55)
    SET_VALUE(GSW(0), 56)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_envoff_f, 512, 2000)
    SET_VALUE(LW(0), 1)
    SET_VALUE(LW(1), STRING("stone"))
    CALL_CHILD(evt_sub_starstone)
    RETURN
EVT_END()

static char* capturemap[2] = {
    "gon_11_S", "\x00",
};

static Vec v_list[8] = {
    {-50.0f, 100.0f, 50.0f},
    {50.0f, 100.0f, 50.0f},
    {50.0f, 0.0f, 50.0f},
    {-50.0f, 0.0f, 50.0f},
    {-50.0f, 100.0f, -50.0f},
    {50.0f, 100.0f, -50.0f},
    {50.0f, 0.0f, -50.0f},
    {-50.0f, 0.0f, -50.0f},
};

// Builds a scratch EventEntry whose args are the eight values, the way
// evt_cloud.c calls evt_img_alloc_capture.
#pragma dont_inline on  // original emits calls to these helpers, not inlined copies
static s32 _img_alloc_capture(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7) {
    EventEntry event;
    s32 captureArgs[10];

    event.args = captureArgs;
    captureArgs[0] = a0;
    captureArgs[1] = a1;
    captureArgs[2] = a2;
    captureArgs[3] = a3;
    captureArgs[4] = a4;
    captureArgs[5] = a5;
    captureArgs[6] = a6;
    captureArgs[7] = a7;
    return evt_img_alloc_capture(&event, 1);
}

// Projects the hit object's position through the eight corners of v_list and
// returns the on-screen bounding box clamped to the 608x480 frame.
static s32 _get_boundingbox(const char* name, s32* left, s32* top, s32* width, s32* height) {
    Vec pos;
    Vec screen;
    s32 minX;
    s32 maxX;
    s32 minY;
    s32 maxY;
    s32 i;

    hitObjGetPos(name, &pos);
    PSVECAdd(&pos, &v_list[0], &screen);
    getScreenPoint(&screen, &screen);
    minX = (s32)screen.x;
    maxX = (s32)screen.x;
    minY = (s32)screen.y;
    maxY = (s32)screen.y;
    for (i = 1; i < 8; i++) {
        PSVECAdd(&pos, &v_list[i], &screen);
        getScreenPoint(&screen, &screen);
        if (screen.x < (f32)minX) {
            minX = (s32)screen.x;
        }
        if ((f32)maxX < screen.x) {
            maxX = (s32)screen.x;
        }
        if (screen.y < (f32)minY) {
            minY = (s32)screen.y;
        }
        if ((f32)maxY < screen.y) {
            maxY = (s32)screen.y;
        }
    }
    *left = minX + 0x130;
    *top = minY + 0xF0;
    *width = maxX - minX;
    *height = maxY - minY;
    if (*left + *width > 0x260) {
        *width = 0x260 - *left;
    }
    if (*top + *height > 0x1E0) {
        *height = 0x1E0 - *top;
    }
    if (*left < 0) {
        *left = 0;
    }
    if (*top < 0) {
        *top = 0;
    }
    return 1;
}

static void _mapsetofs_all(CloudEntry* cp) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((s8)cp->mapGrpName[i][0] == 0) {
            break;
        }
        mapGrpSetOffScreen(cp->mapGrpName[i], cp->offName);
    }
}

static void _mapclsofs_all(CloudEntry* cp) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((s8)cp->mapGrpName[i][0] == 0) {
            break;
        }
        mapGrpClearOffScreen(cp->mapGrpName[i]);
    }
}
#pragma dont_inline reset

static USER_FUNC(evt_cloud_main) {
    CloudEntry* cp = (CloudEntry*)event->lwData[0];
    s32 inBattle = (gp->inBattle != 0);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(0);
    ImageEntry* img;
    HitEntry* hit;
    HitEntry* h0;
    HitEntry* h1;
    HitEntry* h2;
    HitEntry* h3;

    if (isFirstCall) {
        return EVT_RETURN_BLOCK;
    }
    if (party == NULL) {
        return EVT_RETURN_BLOCK;
    }

    h0 = (HitEntry*)cloudGetHitBreathout(0);
    h1 = (HitEntry*)cloudGetHitBreathout(1);
    h2 = (HitEntry*)cloudGetHitBreathout(2);
    h3 = (HitEntry*)cloudGetHitBreathout(3);

    switch (cp->mode) {
    case 0:
        if (cloud_once_flag == 1) {
            break;
        }
        hit = hitNameToPtr(cp->hitName);
        if (h0 == hit || h1 == hit || h2 == hit || h3 == hit) {
            cp->mode++;
        }
        cp->field_0x18C = 0;
        break;

    case 1: {
        f32 angle = *(f32*)mario->unk1AC;

        if (angle > 90.0f && angle <= 270.0f) {
            strcpy(cp->animName, "P_oof_cloud_2");
        } else {
            strcpy(cp->animName, "P_oof_cloud_1");
        }
        if (animGroupBaseAsync(cp->animName, inBattle, NULL)) {
            OffscreenEntry* off;

            cloud_once_flag = 1;
            animPaperPoseEntry(cp->animName, inBattle);
            offscreenEntry(cp->offName);
            off = offscreenNameToPtr(cp->offName);
            off->unk3C = 4;
            off->unk40 = 4;
            _mapsetofs_all(cp);
            cp->mode++;
        }
        break;
    }

    case 2:
        imgEntry(cp->imgName, inBattle);
        img = imgNameToPtr(cp->imgName, inBattle);
        img->poseId = animPaperPoseGetId(cp->animName, inBattle);
        *(OSTime*)(img->field_0x10C + 4) = animTimeGetTime(inBattle);
        imgClearVirtualPoint(img);
        imgSetShadow(img, 2);
        img->flags |= 2;
        cp->mode = 0xDE;
        break;

    case 0xDE:
        if (_get_boundingbox(cp->hitName, &cp->field_0x19C, &cp->field_0x1A0, &cp->field_0x1A4,
                             &cp->field_0x1A8)) {
            _img_alloc_capture((s32)cp->imgName, 0, (s32)cp->offName, 1, cp->field_0x19C,
                               cp->field_0x1A0, cp->field_0x1A4, cp->field_0x1A8);
            cp->mode = 0xDF;
        }
        break;

    case 0xDF:
        img = imgNameToPtr(cp->imgName, inBattle);
        img->flags &= ~2;
        cp->mode = 3;
        // fall through to the mode 3 tail below

    case 3:
        img = imgNameToPtr(cp->imgName, inBattle);
        img->animName = "A_1";
        *(OSTime*)(img->field_0x10C + 4) = animTimeGetTime(inBattle);
        img->flags &= ~2;
        cp->field_0x198 = 0x1E;
        cp->field_0x1AC = 0.0f;
        cp->mode++;
        break;

    case 4:
        img = imgNameToPtr(cp->imgName, inBattle);
        if (cp->field_0x1AC == 0.0f || 1.0f + cp->field_0x1AC < img->field_0x118) {
            psndSFXOn_3D("SFX_OFF_CLAUD_WIND2", &mario->position);
            cp->field_0x1AC = img->field_0x118;
        }
        if (mario->currMotionId != 0x1B) {
            cp->mode = 0xA;
            break;
        }
        hit = hitNameToPtr(cp->hitName);
        if (!(h0 == hit || h1 == hit || h2 == hit || h3 == hit)) {
            if (cp->field_0x198 == 0) {
                cp->mode = 0xA;
            } else {
                cp->field_0x198--;
                cp->field_0x18C += 2;
            }
        } else {
            cp->field_0x198 = 0x1E;
            cp->field_0x18C += 2;
            if (cp->field_0x18C > cp->field_0x188) {
                cp->mode = 0x14;
            }
        }
        break;

    case 0xA:
        cp->mode = 0x63;
        break;

    case 0x14:
        psndSFXOn_3D("SFX_OFF_CLAUD_WIND3", &mario->position);
        if (cp->field_0x194 != NULL) {
            evtEntry(cp->field_0x194, 0, 0);
        }
        img = imgNameToPtr(cp->imgName, inBattle);
        img->animName = "A_2";
        *(OSTime*)(img->field_0x10C + 4) = animTimeGetTime(inBattle);
        cp->field_0x198 = 0x3C;
        cp->mode++;
        break;

    case 0x15:
        if (cp->field_0x198 != 0) {
            cp->field_0x198--;
        } else {
            cp->field_0x198 = 0x1E;
            cp->field_0x18C += 2;
            cp->mode = 0x64;
        }
        break;

    case 0x63:
    case 0x64:
        img = imgNameToPtr(cp->imgName, inBattle);
        _mapclsofs_all(cp);
        animPaperPoseRelease(img->poseId);
        imgFreeCapture(img, 0);
        imgSetShadow(img, 0);
        imgRelease(img, inBattle);
        offscreenDelete(cp->offName);
        cloud_once_flag = 0;
        if (cp->mode == 0x63) {
            cp->mode = 0;
            break;
        }
        return EVT_RETURN_DONE;

    default:
        break;
    }

    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(evt_cloud_main_evt)
    CALL(evt_cloud_main)
    RETURN
EVT_END()

USER_FUNC(evt_iri_16_cloud_init) {
    s32 i;

    cloud = (CloudEntry*)_mapAlloc(0x1B00);
    for (i = 0; i < 16; i++) {
        cloud[i].flags = 0;
        sprintf(cloud[i].imgName, "i_name_%1d", i);
        sprintf(cloud[i].offName, "o_name_%1d", i);
    }
    cloud_once_flag = 0;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_iri_16_cloud_ent) {
    s32* args;
    s32 a;
    s32 b;
    s32 c;
    CloudEntry* cp;
    s32 i;
    s32 j;

    args = event->args;
    a = evtGetValue(event, args[0]);
    b = evtGetValue(event, args[1]);
    c = evtGetValue(event, args[2]);

    for (i = 0; i < 16; i++) {
        if (cloud[i].flags == 0) {
            break;
        }
    }
    if (i >= 16) {
        return EVT_RETURN_DONE;
    }
    cp = &cloud[i];
    cp->flags = 1;
    strcpy(cp->animName, "P_oof_cloud_1");
    for (j = 0; j < 8; j++) {
        if ((s8)((const char**)a)[j][0] == 0) {
            break;
        }
        strcpy(cp->mapGrpName[j], ((const char**)a)[j]);
    }
    strcpy(cp->hitName, (const char*)b);
    cp->field_0x194 = (void*)c;
    cp->field_0x188 = 0xB4;
    cp->field_0x18C = 0;
    cp->mode = 0;
    cp->field_0x198 = 0;
    cp->event = evtEntry(evt_cloud_main_evt, 0, 0);
    cp->event->lwData[0] = (s32)cp;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(iri_16_box_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("mb"), 110, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("mb"))
    CALL(evt_item_get_item_end_wait, STRING("mb"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(iri_16_cloud_evt)
    CALL(evt_mobj_itembox, STRING("box"), 0, 0, 0, 0, 0, PTR(iri_16_box_evt), GSWF(5407))
    CALL(evt_hitobj_onoff, STRING("A_kura"), 1, 0)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    SET_VALUE(GSWF(5406), 1)
    RETURN
EVT_END()

EVT_BEGIN(iri_16_touch_evt)
    RETURN
EVT_END()

static EVT_BEGIN(iri_16_init)
    IF_INT_EQUAL(GSW(1420), 16)
        CALL(evt_hitobj_onoff, STRING("A_taka"), 1, 0)
        IF_INT_EQUAL(GSWF(5406), 0)
            CALL(evt_iri_16_cloud_init)
            CALL(evt_iri_16_cloud_ent, PTR(capturemap), STRING("A_kura"), PTR(iri_16_cloud_evt))
            CALL(evt_run_case_evt, 9, 1, STRING("A_kura"), 0, PTR(iri_16_touch_evt), 0)
        ELSE
            CALL(evt_hitobj_onoff, STRING("A_kura"), 1, 0)
            CALL(evt_mobj_itembox, STRING("box"), 0, 0, 0, 0, 0, PTR(iri_16_box_evt), GSWF(5407))
        END_IF
    ELSE
        CALL(evt_hitobj_onoff, STRING("A_taka"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("A_kura"), 1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(gon_11_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    SWITCH(GSW(0))
        CASE_LESS(55)
        CASE_DEFAULT
        CALL_CHILD(evt_bero_info_run)
    END_SWITCH
    SWITCH(GSW(0))
        CASE_LESS(55)
        CALL(evt_snd_bgmoff, 512)
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    END_SWITCH
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON4"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_npc_flag_onoff, 1, STRING("ノコタロウ父"), 1536)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_flag_onoff, 1, STRING("ゴンババ"), 32)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 55)
        CALL(evt_npc_set_position, STRING("ゴンババ"), 0, -1000, 0)
        CALL(evt_hitobj_onoff, STRING("baba"), 1, 0)
    ELSE
        CALL(evt_npc_flag_onoff, 1, STRING("ゴンババ"), 33554432)
        CALL(evt_npc_set_scale, STRING("ゴンババ"), FLOAT(1.3f), FLOAT(1.3f), FLOAT(1.3f))
        SPAWN_EVENT(gonbaba_event)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("ゴンババ"), 64)
    CALL(evt_light_charalight_all_onoff, 1)
    CALL(evt_light_npc_lighting_onoff, 1, STRING("ゴンババ"))
    CALL_CHILD(iri_16_init)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 55)
        CALL(evt_npc_delete, STRING("ノコタロウ父"))
        CALL(evt_npc_delete, STRING("ゴンババ"))
    END_IF
    RETURN
EVT_END()

// clang-format on


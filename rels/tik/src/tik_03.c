#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
s32 puni_switch[133];

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

EVT_BEGIN(bero_05_custom_in)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 112)
        IF_INT_EQUAL(GSWF(2884), 0)
            SET_VALUE(GSWF(2884), 1)
        END_IF
        IF_INT_EQUAL(GSWF(2840), 1)
            SET_VALUE(GSWF(2840), 0)
        END_IF
        IF_INT_EQUAL(GSWF(2827), 1)
            SET_VALUE(GSWF(2827), 0)
        END_IF
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(bero_05_custom_out)
    IF_INT_EQUAL(GSW(0), 69)
        SET_VALUE(GSW(0), 70)
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[45] = {
    (u32)"s_bero_1", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"tik_04", (u32)"n_bero_1",
    0x00010001, 0x00000000, 0x00000000, (u32)"dokan",
    0x00020000, 0x00000002, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, (u32)&bero_05_custom_in, 0x0000000A,
    0x00000000, (u32)"win_06", (u32)"dokan1", 0x00010001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
static u32 bero_entry_data2[30] = {
    (u32)"dokan", 0x00020000, 0x00000002, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x0000000A, (u32)&bero_05_custom_out, (u32)"sys_01", (u32)"stg_2",
    0x00010001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static EVT_BEGIN(guide_init)
    SWITCH(GSW(0))
        CASE_EQUAL(66)
        CALL(evt_npc_set_position, STRING("me"), -80, -149, 50)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CASE_EQUAL(67)
        CALL(evt_npc_set_position, STRING("me"), 100, -150, 50)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CASE_EQUAL(68)
        CALL(evt_npc_set_position, STRING("me"), 230, -50, 80)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(guide_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(67)
        WAIT_MS(1000)
        CALL_CHILD(puni_switch)
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[46] = {
    (u32)"ガイド", 0x00000600, 0x00000000, (u32)&guide_init,
    0x00000000, (u32)&guide_talk, 0x00000000, 0x00000000,
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

static EVT_BEGIN(puni_walk_se)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_MOVE2_1"), LW(0), LW(1), LW(2), 0)
        WAIT_FRAMES(5)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(puni_soguu_3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_MS(200)
    CALL(evt_mario_normalize)
    WAIT_MS(200)
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ガイド"), 0, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ガイド"), STRING("R_1"))
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mario_key_onoff, 1)
    END_INLINE_EVENT
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    SPAWN_EVENT_GET_ID(PTR(puni_walk_se), LW(10))
    CALL(evt_npc_move_position, STRING("ガイド"), -455, 59, 0, FLOAT(180.0f), 0)
    DELETE_EVENT(LW(10))
    SET_VALUE(GW(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(2), 0)
                IF_INT_LESS(LW(0), -370)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_normalize)
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir, 0, 270, -1)
        SET_VALUE(GW(0), 1)
    END_INLINE_EVENT
    LOOP(GW(0))
        CALL(evt_npc_reverse_ry, STRING("ガイド"))
        WAIT_MS(800)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 30)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(120.0f))
        CALL(evt_party_set_dir_npc, 0, STRING("ガイド"))
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(2), 10)
        CALL(evt_mario_mov_pos2, LW(0), 10, FLOAT(120.0f))
    END_IF
    CALL(evt_mario_set_dir_npc, STRING("ガイド"))
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073741856)
    CALL(evt_npc_set_ry, STRING("ガイド"), 90)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ガイド"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_anim, STRING("ガイド"), STRING("S_1"))
    WAIT_MS(500)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG2_PUNI1"))
    CALL(evt_snd_envoff, 512)
    CALL(evt_msg_print, 0, STRING("mac_1_012"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, -400, -35, 432, -418, -105, 24, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_1_013"), 0, STRING("ガイド"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_1_014"), 0, STRING("ガイド"))
    CALL(evt_msg_select, 0, STRING("mac_1_014_yesno"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("mac_1_019"))
    ELSE
        CALL(evt_msg_print_add, 0, STRING("mac_1_015"))
        CALL(evt_mario_set_dir, 90, 300, 0)
        CALL(evt_msg_print_party, STRING("mac_1_016"))
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_msg_print_party, STRING("mac_1_017"))
        CALL(evt_msg_print, 0, STRING("mac_1_018"), 0, STRING("ガイド"))
    END_IF
    WAIT_MS(333)
    CALL(evt_msg_print_party, STRING("mac_1_020"))
    CALL(evt_cam3d_evt_set, -435, -78, 289, -435, -130, -7, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_1_021"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, -400, -33, 432, -400, -105, 24, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_1_022"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_1_023"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, -400, -33, 432, -400, -105, 24, 500, 11)
    WAIT_MS(333)
    CALL(evt_msg_print_party, STRING("mac_1_024"))
    CALL(evt_cam3d_evt_set, -445, -90, 257, -445, -141, -42, 0, 11)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_1_025"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, -400, -23, 432, -400, -95, 24, 0, 11)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_1_026"), 0, STRING("ガイド"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 25)
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(60.0f))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_1_027"), 0, STRING("ガイド"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("mac_1_028"))
    CALL(evt_msg_print, 0, STRING("mac_1_029"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, 100, -23, 432, 100, -95, 24, 3000, 11)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("ガイド"), LW(10), LW(11), LW(12))
            IF_INT_LESS_OR_EQUAL(LW(0), LW(10))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("ガイド"), LW(10), LW(11), LW(12))
            IF_INT_LESS_OR_EQUAL(LW(0), LW(10))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    SPAWN_EVENT_GET_ID(PTR(puni_walk_se), LW(10))
    CALL(evt_npc_move_position, STRING("ガイド"), 100, 50, 0, FLOAT(180.0f), 0)
    DELETE_EVENT(LW(10))
    WAIT_MS(200)
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_1_029_00"), 0, STRING("ガイド"))
    CALL(evt_npc_flag_onoff, 0, STRING("ガイド"), 1073741856)
    WAIT_MS(800)
    CALL(evt_cam3d_evt_off, 0, 11)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    SET_VALUE(GSW(0), 67)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(anm_para_snd)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 55, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 115, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 175, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 245, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 305, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 365, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 425, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), -454, -50, -90, 0)
    RETURN
EVT_END()

EVT_BEGIN(anm_rapa_snd)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), -454, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 425, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 365, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 305, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 245, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 175, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 115, -50, -90, 0)
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_LOAD_APPEAR1"), 55, -50, -90, 0)
    RETURN
EVT_END()

EVT_BEGIN(puni_switch)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073741856)
    CALL(evt_msg_print, 0, STRING("mac_1_029_01"), 0, STRING("me"))
    CALL(evt_npc_set_ry, STRING("me"), 90)
    SPAWN_EVENT_GET_ID(PTR(puni_walk_se), LW(10))
    CALL(evt_npc_move_position, STRING("me"), 220, 50, 0, FLOAT(120.0f), 0)
    DELETE_EVENT(LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_1_030"), 0, STRING("me"))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BIKKURI_SWITCH_ON1"), 220, 50, 0, 0)
    CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 100)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    SET_VALUE(GSWF(1334), 1)
    CALL(evt_map_playanim, STRING("anm_para"), 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kabe_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kabe_2"), 1)
    CALL(evt_hitobj_onoff, STRING("a_para_d"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("a_para_d1"), 1, 0)
    CALL_CHILD(anm_para_snd)
    CALL(evt_npc_set_position, STRING("me"), 230, -50, 80)
    CALL(evt_npc_flag_onoff, 0, STRING("ガイド"), 1073741856)
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 68)
    CALL(evt_mario_key_onoff, 1)
    WAIT_MS(1500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("para"), 1)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    RETURN
EVT_END()

EVT_BEGIN(puni_nakama)
    LOOP(0)
        IF_INT_EQUAL(GSW(0), 68)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(2), -90)
                IF_INT_GREATER_OR_EQUAL(LW(0), 300)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 1)
    CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガイド"), 0, -1000, 0)
    CALL(evt_mario_set_party_pos, 1, 10, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 35)
    ADD(LW(2), 30)
    CALL(evt_party_move_pos2, 1, LW(0), LW(2), FLOAT(120.0f))
    CALL(evt_msg_print, 0, STRING("mac_1_031"), 0, STRING("extparty"))
    WAIT_MS(200)
    SET_VALUE(GSW(0), 69)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 1)
    RETURN
EVT_END()

EVT_BEGIN(tik03_switch_evt)
    IF_INT_EQUAL(GSWF(1334), 0)
        CALL(evt_map_playanim, STRING("anm_para"), 0, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kabe_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kabe_2"), 1)
        CALL(evt_hitobj_onoff, STRING("a_para_d"), 1, 0)
        SET_VALUE(GSWF(1334), 1)
        CALL_CHILD(anm_para_snd)
        WAIT_MS(800)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("para"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("para"), 1)
        CALL(evt_map_playanim, STRING("anm_rapa"), 0, 0)
        SET_VALUE(GSWF(1334), 0)
        CALL_CHILD(anm_rapa_snd)
        WAIT_MS(800)
        CALL(evt_hitobj_onoff, STRING("a_para_d"), 0, 1)
        CALL(evt_hitobj_onoff, STRING("a_para_d1"), 0, 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(tik_03_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_EQUAL(GSW(0), 66)
        SPAWN_EVENT(puni_soguu_3)
    END_IF
    IF_INT_LESS_OR_EQUAL(GSW(0), 67)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kabe_2"), 1)
    ELSE
        IF_INT_EQUAL(GSWF(1334), 0)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kabe_1"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kabe_1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("para"), 1)
            CALL(evt_hitobj_onoff, STRING("a_para_d"), 1, 0)
            CALL(evt_hitobj_onoff, STRING("a_para_d1"), 1, 0)
        END_IF
    END_IF
    IF_INT_LESS_OR_EQUAL(GSW(0), 68)
        SPAWN_EVENT(puni_nakama)
    END_IF
    CALL(evt_item_entry, STRING("item_00"), 276, -450, -50, -50, 16, GSWF(1356), 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 325, -50, 0, STRING("kakoi"), 125, GSWF(5590))
    IF_INT_LESS(GSW(0), 71)
        SET_VALUE(LW(0), STRING("dokan"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan"), PTR(bero_entry_data2))
        SET_VALUE(LW(0), STRING("dokan"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

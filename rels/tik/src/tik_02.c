#include "tik_02.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static void hitIsGroup(void);  // TODO: real signature
static void _hitIsGroupSub(void);  // TODO: real signature

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

static BeroEntry bero_entry_data[7] = {
    {"w_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_01", "e_bero_1", 1, 1},
    {"w_bwro_2", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_01", "e_bero_2", 1, 1},
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_2", 1, 1},
    {"dokan_2", 4098, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_1", 1, 1},
    {"w_bero", 4096, 0, 0x4E20, 100000, 0, 0, -1, NULL, 0, NULL, "tik_13", "e_bero", 1, 1},
    {"dokan", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, "hei_00", "dokan_2", 1, 1},
    {NULL},
};

static BeroEntry bero_entry_data2[2] = {
    {"dokan", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, "sys_01", "stg_1", 1, 1},
    {NULL},
};

EVT_BEGIN(geso_init)
    RETURN
EVT_END()

EVT_BEGIN(geso_talk)
    CALL(evt_msg_print, 0, STRING("mac_0_124"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(geso_dead)
    SET_VALUE(GF(0), 0)
    WAIT_MS(1000)
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[46] = {
    (u32)"ゲッソーのゲソ", 0x00000000, 0x00000003, (u32)&geso_init,
    0x00000000, (u32)&geso_talk, (u32)&geso_dead, 0x00000000,
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

// TODO: write mario_hosei_all (global, 0xd8 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(ukishima_move)
    CALL(evt_hit_bind_mapobj, LW(5), LW(4))
    SET_FLOAT_VALUE(LW(2), LW(6))
    LOOP(0)
        CALL(evt_mapobj_get_position, LW(4), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_TIK_FLOOR_MOVE1"), LW(11), LW(12), LW(13), LW(10))
        CALL(evt_sub_intpl_msec_init, 11, LW(6), LW(7), LW(8))
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_trans, 0, LW(4), LW(0), 0, 0)
            SET_FLOAT_VALUE(LW(3), LW(0))
            SUBTRACT_FLOAT(LW(3), LW(2))
            CALL(mario_hosei_all, LW(5), LW(3))
            SET_FLOAT_VALUE(LW(2), LW(0))
            CALL(evt_hit_bind_update, LW(5))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
        WAIT_MS(LW(9))
        CALL(evt_mapobj_get_position, LW(4), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_TIK_FLOOR_MOVE1"), LW(11), LW(12), LW(13), LW(10))
        CALL(evt_sub_intpl_msec_init, 11, LW(7), LW(6), LW(8))
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_trans, 0, LW(4), LW(0), 0, 0)
            SET_FLOAT_VALUE(LW(3), LW(0))
            SUBTRACT_FLOAT(LW(3), LW(2))
            CALL(mario_hosei_all, LW(5), LW(3))
            SET_FLOAT_VALUE(LW(2), LW(0))
            CALL(evt_hit_bind_update, LW(5))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
        WAIT_MS(LW(9))
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u8 geso_checkpose[8] = {
    0x4D, 0x5F, 0x53, 0x5F, 0x31, 0x00, 0x00, 0x00,
};

static EVT_BEGIN(ukishima_apper)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ugo_yuka_1"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ugo_yuka_2"), 2)
    CALL(evt_sub_intpl_msec_init, 1, -30, 0, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 60, LW(0), 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), -66, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -10, 400)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 60, LW(0), 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), -66, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -10, 0, 400)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 60, LW(0), 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), -66, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -5, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 60, LW(0), 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), -66, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -5, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 60, LW(0), 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), -66, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -1, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 60, LW(0), 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), -66, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -1, 0, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 60, LW(0), 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), -66, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

// TODO: write geso_set_checkpose (global, 0x54 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_02.s; this is where the original has it, just before the first script that uses it

// TODO: write hokori_color_func (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_02.s; this is where the original has it, just before the first script that uses it

// TODO: write ripple_scale_func (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_02.s; this is where the original has it, just before the first script that uses it

// TODO: write tik_npc_buruburu (global, 0x31c bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(geso_battle)
    CALL(evt_npc_set_position, STRING("ゲッソー"), -385, -60, 0)
    CALL(evt_npc_set_anim, STRING("ゲッソー"), STRING("GSO_S_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("ゲッソー"), 1073741856)
    CALL(evt_npc_set_position, STRING("ゲッソーのゲソ"), -444, 0, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ゲッソーのゲソ"), 128)
    CALL(evt_npc_set_width, STRING("ゲッソーのゲソ"), 60)
    CALL(evt_npc_set_height, STRING("ゲッソーのゲソ"), 5)
    CALL(evt_npc_set_balloontype, STRING("ゲッソーのゲソ"), 0)
    CALL(geso_set_checkpose, STRING("ゲッソーのゲソ"))
    SET_VALUE(GF(0), 1)
    WAIT_WHILE(GF(0))
    BEGIN_INLINE_EVENT_GET_ID(LW(12))
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(0), -410)
                SUBTRACT(LW(0), -410)
                MULTIPLY(LW(0), -1)
                CALL(evt_mario_set_hosei_xyz, LW(0), 0, 0)
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff_f, 512, 500)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_set_position, STRING("ゲッソーのゲソ"), 0, -1000, 0)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG0_GESO_LEG_MOVE1"), LW(8))
    CALL(evt_npc_set_anim, STRING("ゲッソー"), STRING("GSO_D_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_125"), 0, STRING("ゲッソー"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ゲッソー"), STRING("GSO_H_1"))
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_set_dir, 90, 200, 0)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(10)
            ADD(LW(0), -341)
            CALL(evt_snd_sfx_pit, LW(8), LW(0))
            WAIT_MS(60)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_snd_sfxon, STRING("SFX_STG0_GESO_LEG_MOVE2"), 0)
    WAIT_MS(400)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 1, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 1, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -580, 110, 241, -385, 40, 24, 500, 11)
    WAIT_MS(500)
    DELETE_EVENT(LW(12))
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -435, 10, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -420, -20, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG0_GESO_APPEAR2"), 0)
        CALL(evt_eff64, STRING("ripple00"), STRING("ripple_n64"), 0, -250, 0, -60, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(ripple_scale_func, STRING("ripple00"), FLOAT(2.0f))
        WAIT_MS(400)
        CALL(evt_eff64, STRING("ripple01"), STRING("ripple_n64"), 0, -250, 0, -60, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(ripple_scale_func, STRING("ripple01"), FLOAT(2.0f))
        WAIT_MS(400)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("ゲッソー"), STRING("GSO_O_1"))
    CALL(evt_npc_set_position, STRING("ゲッソー"), -250, -200, -60)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_eff, 0, STRING("splash"), 3, -250, -20, -60, FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("ゲッソー"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_GESO_LANDING1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_cam3d_evt_set, -719, 45, 395, -385, 75, 24, 300, 11)
    CALL(evt_npc_jump_position_nohit, STRING("ゲッソー"), -250, 0, -60, 300, 30)
    CALL(evt_npc_set_anim, STRING("ゲッソー"), STRING("GSO_O_2"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1_2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    LOOP(8)
        CALL(evt_cam3d_evt_set, -635, 52, 302, -385, 75, 24, 50, 11)
        WAIT_MS(50)
        CALL(evt_cam3d_evt_set, -719, 45, 395, -385, 75, 24, 50, 11)
        WAIT_MS(50)
    END_LOOP
    CALL(evt_cam3d_evt_set, -635, 52, 302, -385, 75, 24, 50, 11)
    WAIT_MS(50)
    CALL(evt_npc_flag_onoff, 1, STRING("ゲッソー"), 32768)
    CALL(evt_npc_set_battle_info, STRING("ゲッソー"), 3)
    CALL(evt_npc_battle_start, STRING("ゲッソー"))
    CALL(evt_npc_wait_battle_end)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(tik_npc_buruburu, STRING("ゲッソー"), 1, FLOAT(1.0f), 10, 0)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("ゲッソー"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_GESO_SHAKE1"), LW(0), LW(1), LW(2), LW(11))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_set, -300, 213, 647, -300, 100, 7, 0, 11)
    CALL(evt_npc_set_position, STRING("ゲッソー"), -250, -86, -60)
    WAIT_MS(1000)
    CALL(evt_npc_flag_onoff, 0, STRING("ゲッソー"), 32768)
    CALL(evt_msg_print, 0, STRING("mac_0_126"), 0, STRING("ゲッソー"))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.03f), 500)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_eff, 0, STRING("splash"), 1, -250, -20, -60, FLOAT(3.0f), 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    DELETE_EVENT(LW(10))
    CALL(evt_snd_sfxoff, LW(11))
    CALL(evt_npc_get_position, STRING("ゲッソー"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_GESO_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_glide_position, STRING("ゲッソー"), -250, 500, -60, 500, 0, 0, 1, 0)
    CALL(evt_npc_set_position, STRING("ゲッソー"), 0, -1000, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_GESO_CRASH1"), -250, 230, -60, 0)
    CALL(evt_eff64, STRING("hokori00"), STRING("hokori_n64"), 0, -200, 230, -60, 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("hokori01"), STRING("hokori_n64"), 0, -300, 230, -60, 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(hokori_color_func, STRING("hokori00"))
    CALL(hokori_color_func, STRING("hokori01"))
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 13, -30, 0, 1600)
        LOOP(100)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 0, STRING("s_dokan_1"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("a_dokan_1"))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mapobj_trans, 0, STRING("s_dokan_1"), 0, 0, 0)
        CALL(evt_hit_bind_update, STRING("a_dokan_1"))
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_STG0_ROAD_APPEAR1"), 0)
    CALL_CHILD(ukishima_apper)
    WAIT_MS(500)
    SET_VALUE(LW(4), STRING("ugo_yuka_1"))
    SET_VALUE(LW(5), STRING("a_ugo_yuka_1"))
    SET_FLOAT_VALUE(LW(6), 60)
    SET_FLOAT_VALUE(LW(7), 36)
    SET_VALUE(LW(8), 1000)
    SET_VALUE(LW(9), 2000)
    SPAWN_EVENT(ukishima_move)
    SET_VALUE(LW(4), STRING("ugo_yuka_2"))
    SET_VALUE(LW(5), STRING("a_ugo_yuka_2"))
    SET_FLOAT_VALUE(LW(6), -66)
    SET_FLOAT_VALUE(LW(7), -40)
    SET_VALUE(LW(8), 1000)
    SET_VALUE(LW(9), 2000)
    SPAWN_EVENT(ukishima_move)
    SET_VALUE(GSW(0), 21)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_TIK1"), 2000)
    RETURN
EVT_END()

static EVT_BEGIN(wall_bomb)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, STRING("bom_kabe_3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("w_bwro_2"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_hitobj_attr_onoff, 1, 1, STRING("w_bwro_2"), 4)
    SET_VALUE(GSWF(1332), 1)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("bom_kabe_3"), STRING("offscreen"))
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
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_kabe_3"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bom_kabe_4"), 1)
        CALL(evt_img_wait_animend, STRING("capture_img"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_kabe_3"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bom_kabe_4"), 1)
    END_IF
    CALL(evt_mapobj_clear_offscreen, 1, STRING("bom_kabe_3"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

// TODO: type
static u32 hitDmgRetPoints[28] = {
    (u32)"mod_1", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_2", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_3", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_4", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_5", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"dokan_1", 0x00000000, 0xC47A0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(tik_02_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK4"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_map_playanim, STRING("anm_nami_1"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_nami_2"), 1, 0)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
    IF_INT_EQUAL(GSWF(1332), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_kabe_4"), 1)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("w_bwro_2"), 4)
        SET_VALUE(LW(0), STRING("w_bwro_2"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_run_case_evt, 15, 1, STRING("w_bwro_2"), 0, PTR(wall_bomb), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_kabe_3"), 1)
    END_IF
    CALL(evt_mobj_powerupblk, STRING("pblk1"), 350, 60, 100, 0, GSWF(5531), 0)
    IF_INT_LESS(GSW(0), 21)
        CALL(evt_hit_bind_mapobj, STRING("a_ugo_yuka_1"), STRING("ugo_yuka_1"))
        CALL(evt_hit_bind_mapobj, STRING("a_ugo_yuka_2"), STRING("ugo_yuka_2"))
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_1"), 0, -100, 0)
        CALL(evt_mapobj_trans, 0, STRING("ugo_yuka_2"), 0, -100, 0)
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_1"))
        CALL(evt_hit_bind_update, STRING("a_ugo_yuka_2"))
        CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("s_dokan_1"))
        CALL(evt_mapobj_trans, 0, STRING("s_dokan_1"), 0, -30, 0)
        CALL(evt_hit_bind_update, STRING("a_dokan_1"))
    ELSE
        SET_VALUE(LW(4), STRING("ugo_yuka_1"))
        SET_VALUE(LW(5), STRING("a_ugo_yuka_1"))
        SET_FLOAT_VALUE(LW(6), 60)
        SET_FLOAT_VALUE(LW(7), 36)
        SET_VALUE(LW(8), 1000)
        SET_VALUE(LW(9), 2000)
        SPAWN_EVENT(ukishima_move)
        SET_VALUE(LW(4), STRING("ugo_yuka_2"))
        SET_VALUE(LW(5), STRING("a_ugo_yuka_2"))
        SET_FLOAT_VALUE(LW(6), -66)
        SET_FLOAT_VALUE(LW(7), -40)
        SET_VALUE(LW(8), 1000)
        SET_VALUE(LW(9), 2000)
        SPAWN_EVENT(ukishima_move)
    END_IF
    IF_INT_EQUAL(GSW(0), 20)
        SPAWN_EVENT(geso_battle)
    END_IF
    IF_INT_LESS(GSW(0), 23)
        SET_VALUE(LW(0), STRING("dokan"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan"), PTR(bero_entry_data2))
        SET_VALUE(LW(0), STRING("dokan"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change_01"), STRING("ue_1"))
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change_02"), STRING("ue_2"))
    RETURN
EVT_END()

// TODO: write hitIsGroup (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_02.s; this is where the original has it, just before the first script that uses it

// TODO: write _hitIsGroupSub (static, 0x530 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_02.s; this is where the original has it, just before the first script that uses it
// clang-format on

#include "gon_03.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "math_ext.h"
#include "party.h"
#include "pmario_sound.h"
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

static f32 randf(f32 min, f32 max);
static void senkaron_ext_init(void);
static void senkaron_ext_main(void);
static void senkaron_ext_dispent(void);
static void senkaron_ext_disp(void);
static void senkaron_ext_shadow_disp(void);
static USER_FUNC(senkaronToEscape);
static USER_FUNC(senkaronFlagOnMesPushing);
static USER_FUNC(senkaronFlagOffMesPushing);
static USER_FUNC(senkaronAttackStart);
static USER_FUNC(senkaronGetMarioPushedFlag);
s32 richard_init_event[29];
s32 richard_regl_event[2];
s32 richard_talk_event[53];
s32 leaderkaron_init_event[44];
s32 leaderkaron_talk_event[40];
s32 senkarondummy_init_event[8];
s32 senkarondummy_regl_event[2];
s32 senkarondummy_talk_event[2];
s32 senkaron_entry_event[9];
s32 senkaron_escape_event[4];

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

static NpcSetupInfo npcEnt[4] = {
    {"カロン1", 0, 0, senkarondummy_init_event, senkarondummy_regl_event, senkarondummy_talk_event},
    {"リチャード", 0, 0, richard_init_event, richard_regl_event, richard_talk_event},
    {"リーダー", 0, 0, leaderkaron_init_event, NULL, leaderkaron_talk_event},
};

static EVT_BEGIN(door01_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door01_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door02_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door02_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 12, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gon_02", "e_bero", 5, 5, door01_close, door01_open},
    {"e_bero", 1, 12, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gon_04", "w_bero_1", 5, 5, door02_close, door02_open},
    {NULL},
};

char* nokotaro_pose[2] = {
    "PNK_L_1", "PNK_L_2",
};


EVT_BEGIN(tegami_event)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), -130)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GSW(0), 38)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        CALL(evt_mario_goodbye_party, 0)
        CALL(evt_mario_hello_party, 0, 2)
    END_IF
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_party_stop, 0)
    CALL(evt_eff_fukidashi, 3, STRING("fukidashi"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1300)
    CALL(evt_msg_print, 0, STRING("stg1_gon_02"), 0, STRING("party"))
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam3d_evt_set, -30, 75, 150, -30, 25, -250, 3000, 11)
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_party_move_pos, 0, -40, -110, 1500)
    BEGIN_SIBLING_EVENT
        CALL(evt_mario_mov_pos2, -80, -112, FLOAT(120.0f))
        CALL(evt_mario_wait_move_end)
        CALL(evt_mario_mov_pos2, -75, -100, FLOAT(300.0f))
        CALL(evt_mario_wait_move_end)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_SIBLING_EVENT
    WAIT_MS(1500)
    CALL(evt_msg_print, 0, STRING("stg1_gon_03"), 0, STRING("party"))
    CALL(evt_party_set_pose, 0, STRING("PNK_T_2"))
    CALL(evt_msg_npc, STRING("(^x^)party"))
    CALL(evt_msg_continue)
    WAIT_MS(1000)
    CALL(evt_eff_fukidashi, 3, STRING("fukidashi"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
    WAIT_MS(1300)
    CALL(evt_msg_print, 0, STRING("stg1_gon_04"), 0, STRING("party"))
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_gon_04_1"), 0, STRING("party"))
    CALL(evt_msg_select, 0, STRING("stg1_gon_05_yesno"))
    CALL(evt_msg_continue)
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(GSWF(1481), 1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_5"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_print, 0, STRING("stg1_gon_07"), 0, STRING("party"))
        CALL(evt_party_set_dir, 0, 90, 200)
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("stg1_gon_08"), PTR(nokotaro_callback), STRING("(^x^)party"))
        WAIT_MS(1000)
        CALL(evt_party_set_dir, 0, 270, 200)
        CALL(evt_party_set_pose, 0, STRING("PNK_T_3"))
        CALL(evt_msg_print, 0, STRING("stg1_gon_09"), 0, STRING("(^x^)party"))
        CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
        CALL(evt_party_set_dir, 0, 90, 200)
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("stg1_gon_09_00_01"), 0, STRING("party"))
    ELSE
        CALL(evt_mario_set_pose, STRING("M_N_B"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_print, 0, STRING("stg1_gon_06"), 0, STRING("party"))
    END_IF
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()






static EVT_BEGIN(senkaron_event)
    IF_INT_LESS_OR_EQUAL(GSW(0), 39)
        CALL(senkaronFlagOffMesPushing)
        SET_VALUE(GW(14), 0)
        SET_VALUE(GW(15), 1)
        CALL(evt_npc_change_interrupt, STRING("リチャード"), 6, 0)
        CALL_CHILD(senkaron_entry_event)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_eff_fukidashi, 0, STRING("fukidashi"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_npc_set_anim, STRING("リーダー"), STRING("KRN_D_3"))
        WAIT_MS(1000)
        CALL(evt_cam3d_evt_set, 350, 80, 310, 350, 30, -90, 300, 0)
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg1_gon_10"), 0, 0)
        WAIT_MS(300)
        BEGIN_INLINE_EVENT
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        END_INLINE_EVENT
        BEGIN_SIBLING_EVENT
            CALL(evt_mario_set_pose, STRING("M_N_5B"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_I_Y"))
        END_SIBLING_EVENT
        SET_VALUE(LW(0), 0)
        CALL(evt_npc_set_anim, STRING("リーダー"), STRING("KRN_D_4"))
        LOOP(0)
            SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
            SET_FLOAT_VALUE(LW(11), FLOAT(10.0f))
            SET_FLOAT_VALUE(LW(12), FLOAT(-1.0f))
            LOOP(0)
                WAIT_FRAMES(1)
                CALL(evt_npc_get_position, STRING("リーダー"), LW(13), LW(10), LW(14))
                ADD_FLOAT(LW(10), LW(11))
                ADD_FLOAT(LW(11), LW(12))
                IF_INT_LESS_OR_EQUAL(LW(10), FLOAT(0.0f))
                    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
                    CALL(evt_npc_set_position, STRING("リーダー"), LW(13), LW(10), LW(14))
                    BREAK_LOOP
                END_IF
                CALL(evt_npc_set_position, STRING("リーダー"), LW(13), LW(10), LW(14))
            END_LOOP
            ADD(LW(0), 1)
            IF_INT_GREATER_OR_EQUAL(LW(0), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_npc_set_anim, STRING("リーダー"), STRING("KRN_R_1"))
        CALL(evt_cam3d_evt_off, 300, 0)
        WAIT_MS(700)
        CALL(evt_snd_sfxon, STRING("SFX_STG1_HONE_DOWN1_2"), 0)
        CALL(senkaronAttackStart)
        BEGIN_INLINE_EVENT
            WAIT_MS(400)
            CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.002f), 1500)
        END_INLINE_EVENT
        SET_FLOAT_VALUE(LW(3), FLOAT(-1.1f))
        SET_FLOAT_VALUE(LW(4), FLOAT(0.01f))
        SET_VALUE(LW(5), 90)
        WAIT_MS(1300)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_HONE__MOVE1"), 0, 0, 0, GW(13))
        CALL(evt_mario_set_pose, STRING("M_I_O"))
        WAIT_FRAMES(1)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_mario_dispflag_onoff, 1, 256)
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(0), 100)
                BREAK_LOOP
            END_IF
            ADD_FLOAT(LW(0), LW(3))
            ADD_FLOAT(LW(3), LW(4))
            IF_INT_GREATER_OR_EQUAL(LW(3), FLOAT(-0.01f))
                SET_FLOAT_VALUE(LW(3), FLOAT(-0.01f))
            END_IF
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            ADD(LW(5), 10)
            IF_INT_GREATER_OR_EQUAL(LW(5), 360)
                SUBTRACT(LW(5), 360)
            END_IF
            CALL(evt_mario_set_dispdir, LW(5))
        END_LOOP
        CALL(evt_mario_dispflag_onoff, 0, 256)
        CALL(evt_mario_cont_onoff, 1)
        CALL(evt_cam_letter_box_disable, 1)
        CALL(evt_mario_dispflag_onoff, 0, 2)
        CALL(evt_mario_key_onoff, 1)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        IF_INT_EQUAL(GSWF(1479), 0)
            CALL(evt_mario_cont_onoff, 0)
            CALL(senkaronFlagOnMesPushing)
            CALL(evt_mario_get_party, LW(3))
            SWITCH(LW(3))
                CASE_EQUAL(1)
                CALL(evt_msg_print, 0, STRING("stg1_gon_11"), 0, STRING("party"))
                CASE_EQUAL(2)
                CALL(evt_msg_print, 0, STRING("stg1_gon_12"), 0, STRING("party"))
            END_SWITCH
            SET_VALUE(GSWF(1479), 1)
            CALL(senkaronFlagOffMesPushing)
            CALL(evt_mario_cont_onoff, 1)
        END_IF
        CALL(evt_cam_letter_box_disable, 0)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                CALL(senkaronGetMarioPushedFlag, LW(6))
                IF_INT_NOT_EQUAL(LW(6), 0)
                    SET_VALUE(LW(7), 18)
                END_IF
                IF_INT_GREATER(LW(7), 0)
                    CALL(evt_mario_get_motion, LW(8))
                    IF_INT_EQUAL(LW(8), 0)
                        CALL(evt_mario_get_pose, LW(9))
                        IF_STR_NOT_EQUAL(LW(9), STRING("M_I_O"))
                            CALL(evt_mario_set_pose, STRING("M_I_O"))
                        END_IF
                        CALL(evt_mario_get_dispdir, LW(5))
                        ADD(LW(5), 10)
                        IF_INT_GREATER_OR_EQUAL(LW(5), 360)
                            SUBTRACT(LW(5), 360)
                        END_IF
                        CALL(evt_mario_set_dispdir, LW(5))
                        SUBTRACT(LW(7), 1)
                        IF_INT_EQUAL(LW(7), 0)
                            CALL(evt_mario_set_pose, STRING("M_S_1"))
                        END_IF
                    ELSE
                        SET_VALUE(LW(7), 0)
                    END_IF
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(unk_gon_00000374)
            IF_INT_GREATER_OR_EQUAL(LW(0), 1)
                BREAK_LOOP
            END_IF
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_sub_area_check, 0, -390, 0, 50, LW(0), LW(2), LW(10))
            IF_INT_EQUAL(LW(10), 1)
                IF_INT_EQUAL(GW(14), 0)
                    SET_VALUE(GW(14), 1)
                    CALL(evt_map_playanim, STRING("m_door01_b"), 0, 0)
                END_IF
            END_IF
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_sub_area_check, 0, -390, 0, 20, LW(0), LW(2), LW(10))
            IF_INT_EQUAL(LW(10), 1)
                SET_VALUE(GW(15), 0)
                CALL(evt_npc_change_interrupt, STRING("リチャード"), 6, PTR(richard_talk_event))
                CALL(evt_mario_key_onoff, 0)
                BEGIN_INLINE_EVENT
                    LOOP(0)
                        CALL(evt_mario_get_dispdir, LW(5))
                        ADD(LW(5), 10)
                        CALL(evt_mario_set_dispdir, LW(5))
                        WAIT_FRAMES(1)
                    END_LOOP
                END_INLINE_EVENT
                SET_VALUE(GSWF(1476), 1)
                IF_INT_EQUAL(GSWF(1477), 0)
                    SET_VALUE(GSWF(1477), 1)
                ELSE
                    SET_VALUE(GSWF(1478), 1)
                END_IF
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                ADD(LW(1), 50)
                CALL(evt_fade_set_spot_pos, LW(0), LW(1), LW(2))
                CALL(evt_fade_set_mapchange_type, 0, 4, 500, 9, 300)
                CALL(evt_bero_mapchange, STRING("gon_01"), STRING("e_bero"))
            END_IF
        END_LOOP
        CALL(senkaronFlagOnMesPushing)
        CALL(evt_npc_set_battle_info, STRING("リーダー"), 6)
        CALL(evt_npc_battle_start, STRING("リーダー"))
        CALL(evt_npc_wait_battle_end)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
        CALL(evt_snd_sfxoff, GW(13))
        SET_VALUE(GW(15), 0)
        CALL(evt_npc_change_interrupt, STRING("リチャード"), 6, PTR(richard_talk_event))
        CALL(evt_npc_set_position, STRING("リーダー"), 0, -1000, 0)
        CALL(evt_snd_sfxon, STRING("SFX_STG1_HONE_ESCAPE1"), 0)
        SPAWN_EVENT(senkaron_escape_event)
        SET_VALUE(GSW(0), 39)
        CALL(evt_map_playanim, STRING("m_door01_a"), 0, 0)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_on)
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(key_get)
    SET_VALUE(GSW(0), 46)
    RETURN
EVT_END()

EVT_BEGIN(gon_03_init_evt)
    CALL(evt_bg_disp_onoff, 0)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(LW(0), STRING("e_bero"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(GW(15), 0)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 39)
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_change_fbat_mode, 1)
    IF_INT_LESS(GSW(0), 38)
        SPAWN_EVENT(tegami_event)
    END_IF
    IF_INT_LESS(GSW(0), 46)
        CALL(evt_item_entry, STRING("key00"), 12, 200, 0, -200, 16, -1, PTR(key_get))
    END_IF
    CALL(evt_npc_entry, STRING("\"カロン2\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン2\""), -150, 0, 130)
    CALL(evt_npc_set_rotate, STRING("\"カロン2\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン2\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン2\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン2\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン2\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン2\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン2\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン3\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン3\""), 270, 0, -100)
    CALL(evt_npc_set_rotate, STRING("\"カロン3\""), 0, 180, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン3\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン3\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン3\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン3\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン3\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン3\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン4\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン4\""), 290, 0, 80)
    CALL(evt_npc_set_rotate, STRING("\"カロン4\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン4\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン4\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン4\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン4\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン4\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン4\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン5\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン5\""), -300, 0, -40)
    CALL(evt_npc_set_rotate, STRING("\"カロン5\""), 0, 180, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン5\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン5\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン5\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン5\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン5\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン5\""), 7)
    CALL(evt_map_playanim, STRING("hokori"), 1, 0)
    CALL(evt_npc_set_balloontype, STRING("リーダー"), 0)
    CALL(evt_npc_set_balloontype, STRING("リチャード"), 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -298, 0, -197, STRING("s_heya"), 125, GSWF(5603))
    RETURN
EVT_END()

EVT_BEGIN(richard_init_event)
    CALL(evt_npc_set_position, STRING("リチャード"), 0, 0, -100)
    CALL(evt_npc_set_width, STRING("リチャード"), FLOAT(30.0f))
    CALL(evt_npc_set_anim, STRING("リチャード"), STRING("KRN_Z_2"))
    CALL(evt_npc_flag_onoff, 1, STRING("リチャード"), 50331648)
    CALL(evt_npc_set_ry, STRING("リチャード"), 0)
    CALL(evt_npc_pera_onoff, STRING("リチャード"), 0)
    RETURN
EVT_END()

EVT_BEGIN(richard_regl_event)
    RETURN
EVT_END()

EVT_BEGIN(richard_talk_event)
    IF_INT_EQUAL(GW(15), 1)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_gon_09_01"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg1_gon_09_01_yn"))
    CALL(evt_msg_continue)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_set_pose, STRING("M_L_2"))
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg1_gon_09_02"), 0, 0)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(leaderkaron_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 512)
    IF_INT_LESS(GSW(0), 39)
        CALL(evt_npc_set_position, STRING("リーダー"), 380, 0, 10)
        CALL(evt_npc_set_width, STRING("リーダー"), FLOAT(48.0f))
        CALL(evt_npc_set_anim, STRING("リーダー"), STRING("KRN_Z_2"))
        CALL(evt_npc_set_ry, STRING("リーダー"), 270)
        CALL(evt_npc_flag_onoff, 1, STRING("リーダー"), 50331648)
    ELSE
        CALL(evt_npc_set_position, STRING("リーダー"), 0, -1000, 0)
    END_IF
    SET_VALUE(GW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(leaderkaron_talk_event)
    BEGIN_INLINE_EVENT
        LOOP(10)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_HONE_ACCESS1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(200)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 0x80000000)
    IF_INT_EQUAL(GW(0), 0)
        CALL(evt_npc_set_width, STRING("リーダー"), FLOAT(20.0f))
        SPAWN_EVENT(senkaron_event)
        SET_VALUE(GW(0), 1)
    END_IF
    RETURN
EVT_END()

static ExtMakeTextureEntry senkaron_ext_entry_data[26] = {
    { "c_karon_g_h", "KRN_R_1", 0.0f },
    { "c_karon_g_h", "KRN_R_1", 1.0f },
    { "c_karon_g_h", "KRN_R_1", 2.0f },
    { "c_karon_g_h", "KRN_R_1", 3.0f },
    { "c_karon_g_h", "KRN_R_1", 4.0f },
    { "c_karon_g_h", "KRN_R_1", 5.0f },
    { "c_karon_g_h", "KRN_R_1", 6.0f },
    { "c_karon_g_h", "KRN_R_1", 7.0f },
    { "c_karon_g_h", "KRN_R_1", 8.0f },
    { "c_karon_g_h", "KRN_R_1", 9.0f },
    { "c_karon_g_h", "KRN_R_1", 10.0f },
    { "c_karon_g_h", "KRN_R_1", 11.0f },
    { "c_karon_g_h", "KRN_R_1", 12.0f },
    { "c_karon_g_h", "KRN_R_1", 13.0f },
    { "c_karon_g_h", "KRN_R_1", 14.0f },
    { "c_karon_g_h", "KRN_R_1", 15.0f },
    { "c_karon_g_h", "KRN_R_1", 16.0f },
    { "c_karon_g_h", "KRN_R_1", 17.0f },
    { "c_karon_g_h", "KRN_R_1", 18.0f },
    { "c_karon_g_h", "KRN_R_1", 19.0f },
    { "c_karon_g_h", "KRN_R_1", 20.0f },
    { "c_karon_g_h", "KRN_R_1", 21.0f },
    { "c_karon_g_h", "KRN_R_1", 22.0f },
    { "c_karon_g_h", "KRN_R_1", 23.0f },
    { "c_karon_g_h", "KRN_D_1", 0.0f },
    { NULL, NULL, 0.0f },
};

EVT_BEGIN(senkarondummy_init_event)
    CALL(evt_npc_set_position, STRING("カロン1"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(senkarondummy_regl_event)
    RETURN
EVT_END()

EVT_BEGIN(senkarondummy_talk_event)
    RETURN
EVT_END()





EVT_BEGIN(senkaron_entry_event)
    CALL(evt_ext_entry, 1000, PTR(senkaron_ext_entry_data), PTR(senkaron_ext_init), PTR(senkaron_ext_main), PTR(senkaron_ext_dispent))
    RETURN
EVT_END()


EVT_BEGIN(senkaron_escape_event)
    CALL(senkaronToEscape)
    RETURN
EVT_END()

SenkaronEntry* senkaron;
s32 senkaronAttackStartFlag;
s32 senkaronEscapeFlag;
s32 senkaronMesPushingFlag;
s32 marioPushedFlag;
s32 marioHammerFrame;



void nokotaro_callback(s32 index) {
    PartyEntry* party = partyGetPtr(partyCtrlNo);

    switch (index) {
        case 0:
            partyChgPose(party, nokotaro_pose[1]);
            break;
        case 2:
            partyChgPose(party, nokotaro_pose[0]);
            break;
        case 3:
            partyChgPose(party, nokotaro_pose[1]);
            break;
        case 1:
            partyChgPose(party, nokotaro_pose[0]);
            break;
    }
}

USER_FUNC(unk_gon_00000374) {
    NpcEntry* npc = marioGetTouchNpcPtr();
    s32 flag = FALSE;

    if (marioKeyOffChk() || marioCtrlOffChk()) {
        flag = TRUE;
    }
    if (marioStGetSystemLevel() != 0) {
        flag = TRUE;
    }

    if (npc != NULL && strcmp(npc->description, "リーダー") == 0 && flag == FALSE) {
        evtSetValue(event, LW(0), 1);
    } else {
        evtSetValue(event, LW(0), 0);
    }
    return EVT_RETURN_DONE;
}

static f32 randf(f32 min, f32 max) {
    f32 ratio = (f32)rand() / 32767.0f;

    return ratio * (max - min) + min;
}

static void senkaron_ext_init(void) {
    s32 i;
    ExtPose* pose = extGetPosePtr();

    senkaron = _mapAlloc(0x5DC0);
    i = extGetPoseNum() - 1;
    while (i >= 0) {
        pose->flags = 3;
        pose->heading = randf(0.0f, 360.0f);
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = randf(-385.0f, 375.0f);
        (*pose->matrix)[1][3] = 200.0f;
        (*pose->matrix)[2][3] = randf(-125.0f, 140.0f);
        pose->heading = 90.0f;
        senkaron[i].state = 0;
        senkaron[i].timer = (s32)randf(1.0f, 50.0f);
        senkaron[i].velY = -5.0f;
        i--;
        pose++;
    }
    senkaronAttackStartFlag = 0;
    senkaronEscapeFlag = 0;
    marioHammerFrame = 0;
}

static void senkaron_ext_main(void) {
    f32 f31;
#ifdef MUST_MATCH
    // MWCC coalesces two equal scalar string pointers into one register; modelling the fly SFX name
    // as an array element forces the original's second live copy of it (r28/r27).
    const char* sfx[3];
#define sfxLanding sfx[0]
#define sfxFly0 sfx[1]
#define sfxFly1 sfx[2]
#else
    const char* sfxLanding;
    const char* sfxFly;
#define sfxFly0 sfxFly
#define sfxFly1 sfxFly
#endif
    f32 f30;
    s32 i;
    f32 f29;
    MarioWork* mario;
    ExtPose* pose;

    pose = extGetPosePtr();
    mario = marioGetPtr();
#ifdef MUST_MATCH
    {
        // Naming the constant makes MWCC load the field into f1 and 0.5f into f0, as in the original.
        f32 half = 0.5f;
        f29 = mario->wPlayerCollisionBox.x * half;
    }
#else
    f29 = mario->wPlayerCollisionBox.x * 0.5f;
#endif
    marioPushedFlag = 0;
    if (mario->currMotionId == MARIO_MOTION_HAMMER) {
        marioHammerFrame++;
        if (marioHammerFrame == 0xF) {
            marioHammerFrame = 0;
        }
    } else {
        marioHammerFrame = 0;
    }

    i = extGetPoseNum() - 1;
    sfxLanding = "SFX_STG1_HONE_LANDING1";
    f31 = 1.5f * (6.0f * f29);
    sfxFly0 = "SFX_STG1_HONE_FLY1";
    sfxFly1 = "SFX_STG1_HONE_FLY1";
    f30 = 3.0f * f29;

    for (; i >= 0; i--, pose++) {
        switch (senkaron[i].state) {
            case 0:
                pose->texId = (gp->retraceCount + i * i) % 0x18;
                if (senkaron[i].timer > 0) {
                    if (senkaronEscapeFlag == 1) {
                        senkaron[i].state = 0x62;
                    } else if (senkaronAttackStartFlag == 1) {
                        senkaron[i].timer--;
                    }
                } else {
                    (*pose->matrix)[1][3] += senkaron[i].velY;
                    senkaron[i].velY -= 0.3f;
                    if ((*pose->matrix)[1][3] < 0.0f) {
                        (*pose->matrix)[1][3] = 0.0f;
                        senkaron[i].state = 1;
                        senkaron[i].timer = 0;
                        senkaron[i].flags &= 0x0000FFFE;
                        if (i % 8 == 0) {
                            Vec v = {0.0f, 0.0f, 0.0f};

                            v.x = (*pose->matrix)[0][3];
                            v.y = (*pose->matrix)[1][3];
                            v.z = (*pose->matrix)[2][3];
                            psndSFXOn_3D(sfxLanding, &v);
                        }
                    }
                }
                break;
            case 1:
                pose->texId = (gp->retraceCount + i * i) % 0x18;
                if (senkaronEscapeFlag == 1) {
                    f32 len;

                    if ((*pose->matrix)[0][3] == 0.0f && (*pose->matrix)[2][3] == 0.0f) {
                        senkaron[i].velX = 1.0f;
                        senkaron[i].velZ = 1.0f;
                    } else {
                        len = (f32)sqrt((*pose->matrix)[0][3] * (*pose->matrix)[0][3]
                                        + (*pose->matrix)[2][3] * (*pose->matrix)[2][3]);
                        senkaron[i].velX = 4.0f * (*pose->matrix)[0][3] / len;
                        senkaron[i].velZ = 4.0f * (*pose->matrix)[2][3] / len;
                    }
                    if (senkaron[i].velX < 0.0f) {
                        pose->heading = 90.0f;
                    } else {
                        pose->heading = 270.0f;
                    }
                    senkaron[i].state = 0x62;
                    senkaron[i].timer = 300;
                } else {
                    if (senkaronMesPushingFlag == 1) {
                        break;
                    }
                    if (mario->currMotionId == MARIO_MOTION_HAMMER && marioHammerFrame == 0xA) {
                        if ((*pose->matrix)[2][3] > mario->position.z - 80.0f && (*pose->matrix)[2][3] < mario->position.z + 80.0f) {
                            if (mario->directionView > 0.0f && mario->directionView < 180.0f) {
                                if ((*pose->matrix)[0][3] > mario->position.x && (*pose->matrix)[0][3] < mario->position.x + f31) {
                                    f32 dx = (*pose->matrix)[0][3] - mario->position.x;
                                    f32 dz = (*pose->matrix)[2][3] - mario->position.z;
                                    f32 len = (f32)sqrt(dx * dx + dz * dz);

                                    senkaron[i].velX = 3.0f * dx / len - 1.0f;
                                    senkaron[i].velY = randf(5.0f, 10.0f);
                                    senkaron[i].velZ = randf(-5.0f, 10.0f);
                                    senkaron[i].state = 0x61;
                                    {
                                        Vec v = {0.0f, 0.0f, 0.0f};

                                        v.x = (*pose->matrix)[0][3];
                                        v.y = (*pose->matrix)[1][3];
                                        v.z = (*pose->matrix)[2][3];
                                        psndSFXOn_3D(sfxFly0, &v);
                                    }
                                }
                            } else if ((*pose->matrix)[0][3] < mario->position.x && (*pose->matrix)[0][3] > mario->position.x - f31) {
                                f32 dx = (*pose->matrix)[0][3] - mario->position.x;
                                f32 dz = (*pose->matrix)[2][3] - mario->position.z;
                                f32 len = (f32)sqrt(dx * dx + dz * dz);

                                senkaron[i].velX = 1.0f + 3.0f * dx / len;
                                senkaron[i].velY = randf(5.0f, 10.0f);
                                senkaron[i].velZ = randf(-5.0f, 10.0f);
                                senkaron[i].state = 0x61;
                                {
                                    Vec v = {0.0f, 0.0f, 0.0f};

                                    v.x = (*pose->matrix)[0][3];
                                    v.y = (*pose->matrix)[1][3];
                                    v.z = (*pose->matrix)[2][3];
                                    psndSFXOn_3D(sfxFly1, &v);
                                }
                            }
                        }
                    }
                    {
                        f32 px = mario->position.x;
                        f32 mx = (*pose->matrix)[0][3];

                        if (mx > px + f29 - 10.0f && mx < px + f30) {
                            f32 pz = mario->position.z;
                            f32 mz = (*pose->matrix)[2][3];

                            if (mz > pz - 20.0f && mz < pz + 20.0f) {
                                marioPushedFlag = 1;
                            }
                        }
                    }
                    if ((*pose->matrix)[0][3] >= mario->position.x) {
                        if (senkaron[i].flags & 1) {
                            pose->heading = 270.0f;
                        } else {
                            pose->heading = 90.0f;
                        }
                    } else {
                        if (senkaron[i].flags & 1) {
                            pose->heading = 90.0f;
                        } else {
                            pose->heading = 270.0f;
                        }
                    }
                    if (senkaron[i].timer == 0) {
                        f32 dx = (*pose->matrix)[0][3] - mario->position.x;
                        f32 dz = (*pose->matrix)[2][3] - mario->position.z;
                        f32 len = (f32)sqrt(dx * dx + dz * dz);
                        f32 d = 10.0f * len;
                        s32 r = (s32)randf(0.0f, 50.0f);

                        senkaron[i].velX = randf(-0.5f, 0.5f);
                        senkaron[i].velZ = randf(-0.5f, 0.5f);
                        senkaron[i].velX -= dx / d;
                        senkaron[i].velZ -= dz / d;
                        if (r == 1) {
                            senkaron[i].flags |= 1;
                        } else {
                            senkaron[i].flags &= 0x0000FFFE;
                        }
                        senkaron[i].timer = 60;
                    }
                    senkaron[i].timer--;
                    (*pose->matrix)[0][3] += senkaron[i].velX;
                    (*pose->matrix)[2][3] += senkaron[i].velZ;
                    if ((*pose->matrix)[0][3] > 375.0f) {
                        (*pose->matrix)[0][3] = 375.0f;
                    }
                    if ((*pose->matrix)[0][3] < -385.0f) {
                        (*pose->matrix)[0][3] = -385.0f;
                    }
                    if ((*pose->matrix)[2][3] > 140.0f) {
                        (*pose->matrix)[2][3] = 140.0f;
                    }
                    if ((*pose->matrix)[2][3] < -125.0f) {
                        (*pose->matrix)[2][3] = -125.0f;
                    }
                }
                break;
            case 0x61:
                pose->flags |= EXT_POSE_NO_SHADOW;
                if ((*pose->matrix)[1][3] > -100.0f) {
                    (*pose->matrix)[0][3] += senkaron[i].velX;
                    (*pose->matrix)[1][3] += senkaron[i].velY;
                    (*pose->matrix)[2][3] += senkaron[i].velZ;
                    if ((*pose->matrix)[2][3] <= -125.0f) {
                        (*pose->matrix)[2][3] = -125.0f;
                    }
                    senkaron[i].velY -= 0.3f;
                    pose->texId = 0x18;
                } else {
                    marioGetPtr();
                    (*pose->matrix)[0][3] = randf(-385.0f, 375.0f);
                    (*pose->matrix)[1][3] = 200.0f;
                    (*pose->matrix)[2][3] = randf(-125.0f, 140.0f);
                    pose->heading = randf(0.0f, 360.0f);
                    pose->flags &= ~EXT_POSE_NO_SHADOW;
                    senkaron[i].state = 0;
                    senkaron[i].timer = (s32)randf(1.0f, 1000.0f);
                    senkaron[i].velY = 0.0f;
                }
                break;
            case 0x62:
                if (senkaron[i].timer > 0) {
                    (*pose->matrix)[0][3] += senkaron[i].velX;
                    (*pose->matrix)[2][3] += senkaron[i].velZ;
                    senkaron[i].timer--;
                } else {
                    senkaron[i].state = 0x63;
                }
                pose->texId = (gp->retraceCount + i * i) % 0x18;
                break;
            case 0x63:
                break;
        }
    }

    if (marioPushedFlag == 1) {
        f32 nx = -390.0f - mario->position.x;
        f32 nz = -mario->position.z;
        f32 len = (f32)sqrt(nx * nx + nz * nz);
        f32 speed;

        switch (mario->currMotionId) {
            case MARIO_MOTION_HAMMER:
                speed = 0.0f;
                break;
            case MARIO_MOTION_WALK:
                speed = randf(2.0f, 2.5f);
                break;
            case MARIO_MOTION_DASH:
                speed = randf(2.8f, 3.3f);
                break;
            case MARIO_MOTION_JUMP:
                speed = randf(2.3f, 2.8f);
                break;
            default:
                speed = randf(0.7f, 1.2f);
                break;
        }
        mario->position.x += speed * (nx / len);
        mario->position.z += speed * (nz / len);
    }
}

#undef sfxLanding
#undef sfxFly0
#undef sfxFly1

static void senkaron_ext_dispent(void) {
    dispEntry(4, 0.0f, 1, (DispCallback)senkaron_ext_disp, NULL);
    dispEntry(4, 0.0f, 2, (DispCallback)senkaron_ext_shadow_disp, NULL);
}

static void senkaron_ext_disp(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

static void senkaron_ext_shadow_disp(void) {
    static Mtx mt = {
        {10.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 10.0f, 0.0f, 0.1f},
        {0.0f, 0.0f, 10.0f, 0.0f},
    };

    extLoadShadowMtx(mt);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
}

static USER_FUNC(senkaronToEscape) {
    senkaronEscapeFlag = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(senkaronFlagOnMesPushing) {
    senkaronMesPushingFlag = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(senkaronFlagOffMesPushing) {
    senkaronMesPushingFlag = 0;
    return EVT_RETURN_DONE;
}

USER_FUNC(senkaronAttackStart) {
    senkaronAttackStartFlag = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(senkaronGetMarioPushedFlag) {
    evtSetValue(event, event->args[0], marioPushedFlag);
    return EVT_RETURN_DONE;
}

// clang-format on

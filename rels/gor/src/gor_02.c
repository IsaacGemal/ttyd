#include "gor_02.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_johoya.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_urouro.h"
#include "evt/evt_win.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_monosiri.h"
#include "cam_road.h"
#include "driver/bgdrv.h"
#include "driver/camdrv.h"
#include "driver/imgdrv.h"
#include "driver/mapdrv.h"
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "driver/swdrv.h"
#include "dolphin/gx/GXTev.h"
#include "eff/eff_stardust.h"
#include "eff/eff_mahojin.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include "window/win_main.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_goki_sub_intpl_get_value[];
extern s32 evt_goki_sub_intpl_init[];
extern s32 evt_gor_villager_urouro_check_func[];
extern s32 gor_irai_init[];
extern s32 luigi_gor_first_talk[];
extern s32 unk_80239b2c[];
static USER_FUNC(set_depthPlaneOffset);
static USER_FUNC(evt_StatusClose);
static USER_FUNC(evt_StatusOpen);
static USER_FUNC(setmertn);
static USER_FUNC(bgm_start_wait);
static USER_FUNC(bgScrollControll);
static USER_FUNC(enzan_lw_load);
static USER_FUNC(enzan_lw_save);
static USER_FUNC(enzan_set_f_i);
static USER_FUNC(enzan_set_i_f);
static USER_FUNC(enzan_cam_turn_keisan_f);
static USER_FUNC(enzan_cam_turn_keisan);
static USER_FUNC(wariai_enzan_0f);
static USER_FUNC(wariai_enzan_0);
static USER_FUNC(wariai_enzan_1f_intpl);
static USER_FUNC(wariai_enzan_1f);
static USER_FUNC(wariai_enzan_1);
static USER_FUNC(effect_stardust);
static USER_FUNC(alpha_set_func);
static USER_FUNC(alpha_init_func);
static void alpha_tev(TevCallbackParam* param);
static u32 door_data_01[18];
u32 door_data_06[18];
s32 evt_gor_door_open_sanders[80];
s32 evt_gor_door_close_sanders[80];
s32 steel_coin[386];
s32 kurihakase_first_talk[787];
s32 kurihakase_wall_remove[257];
s32 kurihakase_lecture[286];
s32 kurihakase_lecture_badge[272];
s32 kurihakase_after3minutes[177];
u32 stg0_list[1];
u32 stg1_list[4];
u32 stg2_list[4];
u32 stg3_list[1];
u32 stg5_list[2];
u32 stg6_list[1];
u32 stg7_list[6];
u32 stg8_list[10];
u32 follow[24];
u32 _door[1];
static u32 _ap[1];
static u32 xlw[16];
u32 iri_13_item_tbl[1];
u32 camstring[1];

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

EVT_BEGIN(unk_evt_gor_0003d138)
    CALL(evt_hitobj_onoff, STRING("A_kabe"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_saku"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_kmn"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_gomi"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_l_ura_gomi"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_gor_0003d1a4)
    CALL(evt_hitobj_onoff, STRING("A_kabe"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_saku"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_kmn"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_gomi"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_l_ura_gomi"), 1, 1)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 10)
        CALL(evt_hitobj_onoff, STRING("A_saku_04"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(1213), 1)
        CALL(evt_hitobj_onoff, STRING("A_center_kabe_atr"), 1, 0)
    END_IF
    IF_INT_EQUAL(GW(10), 1)
        CALL(evt_hitobj_onoff, STRING("A_center_kabe_atr"), 1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(gor_02_event_00)
    IF_INT_NOT_EQUAL(GSW(0), 7)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    SPAWN_EVENT(steel_coin)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(gor_02_event_01)
    IF_INT_EQUAL(GSWF(1207), 1)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_party_stop, 1)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_party_pos, 1, 11, 0, -1000, 0)
    SET_VALUE(GSWF(1207), 1)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, gor_02_event_00, -1, NULL, "gor_01", "e_bero", 1, 1},
    {"dokan", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, gor_02_event_01, "tik_01", "dokan_2", 1, 1},
    {NULL},
};








EVT_BEGIN(powerupya_party_lank_rtn)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_party_set_dir_pos, 0, -362, -200)
            CALL(unk_800e23e0, -362, -200)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(enzan_lw_save)
    CALL(evt_snd_bgmoff_f, 1024, 2000)
    CALL(evt_msg_print, 0, STRING("powerup_12"), 0, STRING("me"))
    CALL(evt_sub_intpl_msec_init, 11, 0, 20, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_i01_sui_ao"), 0, LW(0), 0)
        CALL(evt_mapobj_trans, 1, STRING("S_i01_sui_aka"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(500)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_POWERUP1"))
    CALL(bgm_start_wait)
    CALL(evt_eff64, STRING("aaax"), STRING("lensflare_n64"), 0, -362, 44, -200, FLOAT(1.0f), -1, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_sui_ao"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_sui_aka"), 1)
    CALL(evt_msg_print, 0, STRING("powerup_14"), 0, STRING("me"))
    CALL(bgScrollControll, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("mario"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("party"), 2048)
    CALL(evt_mario_get_exparty, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_map_blend_set_flag, 1, STRING("extparty"), 2048)
    END_IF
    CALL(evt_map_blend_set_flag, 1, STRING("パワーアップ屋"), 2048)
    CALL(evt_map_set_blend, 0, 255, 255, 255, 255)
    CALL(evt_set_bgtpl, STRING("powup"))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S_ie_01"), 255, 255, 255, LW(0))
            CALL(evt_mapobj_color, 1, STRING("S_i01_kmnC"), 255, 255, 255, 255)
            CALL(evt_mapobj_color, 1, STRING("S_i01_sui"), 255, 255, 255, 255)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 90, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("S_i01_honC_l"), 0, 0, LW(0))
            MULTIPLY(LW(0), -1)
            CALL(evt_mapobj_rotate, 1, STRING("S_i01_honC_s"), LW(0), 0, 0)
            CALL(evt_mapobj_rotate, 1, STRING("S_i01_honC_r"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1600)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -362, 193, 686, -362, 37, -200, 1600, 11)
    WAIT_MS(1600)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 100, 7000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(enzan_cam_turn_keisan_f, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(6), 7000)
        DIVIDE(LW(6), 1000)
        ADD(LW(6), 1)
        LOOP(LW(6))
            CALL(evt_eff64, 0, STRING("crystal_n64"), 3, -362, 44, -200, FLOAT(1.0f), 60, 0, 0, 0, 0, 0, 0)
            WAIT_MS(1000)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
        SET_FLOAT_VALUE(LW(7), LW(4))
        ADD_FLOAT(LW(7), FLOAT(18.0f))
        CALL(evt_eff64, STRING("cccx"), STRING("pokopi_pcharge_n64"), 2, LW(3), LW(7), LW(5), FLOAT(1.0f), -1, 0, 0, 0, 0, 0, 0)
        WAIT_MS(500)
        CALL(evt_mario_get_party, LW(1))
        SWITCH(LW(1))
            CASE_EQUAL(1)
            CALL(evt_party_set_pose, 0, STRING("PKR_C_1"))
            CASE_EQUAL(2)
            CALL(evt_party_set_pose, 0, STRING("PNK_C_1"))
            CASE_EQUAL(3)
            CALL(evt_party_set_pose, 0, STRING("C_1"))
            CASE_EQUAL(4)
            CALL(evt_party_set_pose, 0, STRING("PYS_C_1"))
            CASE_EQUAL(5)
            CALL(evt_party_set_pose, 0, STRING("PWD_C_1"))
            CASE_EQUAL(6)
            CALL(evt_party_set_pose, 0, STRING("PTR_C_1"))
            CASE_EQUAL(7)
            CALL(evt_party_set_pose, 0, STRING("PCH_C_1"))
        END_SWITCH
    END_INLINE_EVENT
    WAIT_MS(7000)
    CALL(evt_eff_softdelete, STRING("aaax"))
    IF_INT_EQUAL(GSWF(1186), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i01_sui_ao"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i01_sui_aka"), 1)
    END_IF
    WAIT_MS(500)
    CALL(evt_eff64, STRING("dddx"), STRING("release_n64"), 65283, -362, 44, -200, FLOAT(1.0f), 200, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_eff_softdelete, STRING("cccx"))
    CALL(evt_mario_get_party, LW(1))
    SWITCH(LW(1))
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_I_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_I_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("I_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_I_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_I_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_I_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_I_1"))
    END_SWITCH
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    SET_FLOAT_VALUE(LW(7), LW(4))
    ADD_FLOAT(LW(7), FLOAT(18.0f))
    CALL(effect_stardust, 0, LW(3), LW(7), LW(5), FLOAT(50.0f), FLOAT(50.0f), 20, 120)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 1200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S_ie_01"), 255, 255, 255, LW(0))
            CALL(evt_mapobj_color, 1, STRING("S_i01_kmnC"), 255, 255, 255, 255)
            CALL(evt_mapobj_color, 1, STRING("S_i01_sui"), 255, 255, 255, 255)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 90, 0, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("S_i01_honC_l"), 0, 0, LW(0))
            MULTIPLY(LW(0), -1)
            CALL(evt_mapobj_rotate, 1, STRING("S_i01_honC_s"), LW(0), 0, 0)
            CALL(evt_mapobj_rotate, 1, STRING("S_i01_honC_r"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(bgScrollControll, 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_01_naka"), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_01_soto"), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_01_doa"), 2048)
        CALL(evt_map_blend_set_flag, 1, STRING("mario"), 2048)
        CALL(evt_map_blend_set_flag, 1, STRING("party"), 2048)
        CALL(evt_mario_get_exparty, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_map_blend_set_flag, 1, STRING("extparty"), 2048)
        END_IF
        CALL(evt_map_blend_set_flag, 1, STRING("パワーアップ屋"), 2048)
        CALL(evt_map_set_blend, 0, 0, 0, 0, 0)
        CALL(evt_set_bgtpl, 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_off, 1600, 11)
    WAIT_MS(1400)
    CALL(evt_party_set_breed_pose, 0, 1)
    CALL(evt_sub_intpl_msec_init, 11, 20, 0, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_i01_sui_ao"), 0, LW(0), 0)
        CALL(evt_mapobj_trans, 1, STRING("S_i01_sui_aka"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_get_party, LW(1))
    SWITCH(LW(1))
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
    WAIT_MS(1500)
    CALL(evt_party_set_breed_pose, 0, 1)
    CALL(evt_mario_dispflag_onoff, 0, 4)
    CALL(enzan_lw_load)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    RETURN
EVT_END()

EVT_BEGIN(powerupya_party_rtn)
    CALL(evt_win_coin_on, 1, LW(12))
    LOOP(0)
        CALL(evt_party_set_dir_pos, 0, -362, -200)
        CALL(evt_win_other_select, 6)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_msg_print, 0, STRING("powerup_13_01"), 0, STRING("me"))
            BREAK_LOOP
        END_IF
        CALL(evt_mario_get_party, LW(8))
        IF_INT_NOT_EQUAL(LW(1), LW(8))
            CALL(evt_mario_goodbye_party, 0)
            CALL(evt_mario_hello_party, 0, LW(1))
        END_IF
        CALL(evt_msg_print, 0, STRING("powerup_02"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("powerup_03"))
        CALL(evt_msg_continue)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_pouch_get_supercoin, LW(0))
            SUBTRACT(LW(0), 1)
            CALL(evt_pouch_set_supercoin, LW(0))
            WAIT_FRAMES(4)
            SUBTRACT(LW(0), 1)
            CALL(evt_pouch_set_supercoin, LW(0))
            WAIT_FRAMES(4)
            SUBTRACT(LW(0), 1)
            CALL(evt_pouch_set_supercoin, LW(0))
            WAIT_FRAMES(4)
            CALL(evt_win_coin_wait, LW(12))
            WAIT_MS(500)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_mario_get_party, LW(8))
            CALL(unitwin_getpartylank, LW(8), LW(0))
            ADD(LW(0), 1)
            CALL(unitwin_setpartylank, LW(8), LW(0))
            CALL_CHILD(powerupya_party_lank_rtn)
            CALL(evt_pouch_get_supercoin, LW(0))
            IF_INT_LESS(LW(0), 3)
                CALL(evt_msg_print, 0, STRING("powerup_13"), 0, STRING("me"))
                BREAK_LOOP
            END_IF
            CALL(unk_80239b2c, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print, 0, STRING("powerup_13"), 0, STRING("me"))
                BREAK_LOOP
            ELSE
                CALL(evt_msg_print, 0, STRING("powerup_15"), 0, STRING("me"))
                CALL(evt_msg_select, 0, STRING("powerup_03"))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(evt_msg_continue)
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("powerup_13_02"))
                    BREAK_LOOP
                END_IF
            END_IF
            CALL(evt_win_coin_on, 1, LW(12))
        END_IF
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(powerupya_table_talk)
    CALL(setmertn, STRING("パワーアップ屋"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    CALL(unk_80239b2c, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("powerup_16"), 0, STRING("me"))
        GOTO(0)
    END_IF
    IF_INT_EQUAL(GSWF(1185), 0)
        CALL(evt_msg_print, 0, STRING("powerup_00"), 0, STRING("me"))
        SET_VALUE(GSWF(1185), 1)
        SET_VALUE(GSWF(1250), 1)
    ELSE
        IF_INT_EQUAL(GSWF(1250), 0)
            CALL(evt_msg_print, 0, STRING("powerup_00_01"), 0, STRING("me"))
            SET_VALUE(GSWF(1250), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("powerup_00_02"), 0, STRING("me"))
        END_IF
    END_IF
    CALL(evt_pouch_get_supercoin, LW(2))
    IF_INT_LESS(LW(2), 3)
        CALL(evt_msg_print_add, 0, STRING("powerup_01"))
    ELSE
        CALL(evt_msg_continue)
        CALL_CHILD(powerupya_party_rtn)
    END_IF
    LABEL(0)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(powerupya_direct_talk)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_01_naka"), 1)
    CALL(evt_mapobj_get_position, STRING("S_ie_01_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 104, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 11, 104, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("S_ie_01_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_01_naka"), 1)
    RETURN
EVT_END()

// TODO: type
u32 unk_gor_0003e5c0[2] = {
    0x0000006C, 0xFFFFFFFF,
};

EVT_BEGIN(unk_evt_gor_0003e5c8)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        ADD(LW(1), 30)
        CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(200)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_gor_0003e648)
    WAIT_MS(200)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(2), -210)
        CALL(evt_mario_mov_pos2, -360, -270, FLOAT(120.0f))
        CALL(evt_mario_mov_pos2, -404, -220, FLOAT(120.0f))
    ELSE
        CALL(evt_mario_mov_pos2, -360, -160, FLOAT(120.0f))
        CALL(evt_mario_mov_pos2, -395, -160, FLOAT(120.0f))
    END_IF
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_gor_0003e708)
    WAIT_MS(200)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(2), -210)
        CALL(evt_party_move_pos2, 0, -360, -270, FLOAT(120.0f))
    ELSE
        CALL(evt_party_move_pos2, 0, -360, -160, FLOAT(120.0f))
    END_IF
    CALL(evt_party_move_pos2, 0, -425, -186, FLOAT(120.0f))
    CALL(evt_party_set_dir, 0, 90, 200)
    RETURN
EVT_END()



EVT_BEGIN(unk_evt_gor_0003e7a0)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_C_3"))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    ADD(LW(4), 20)
    ADD(LW(5), 10)
    CALL(unk_gor_0000574c, LW(6))
    IF_FLOAT_EQUAL(LW(6), FLOAT(0.0f))
        ADD(LW(3), -20)
    ELSE
        ADD(LW(3), 20)
    END_IF
    CALL(evt_item_entry, STRING("watasu"), 108, LW(3), LW(4), LW(5), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("watasu"), 2)
    WAIT_MS(1000)
    CALL(evt_item_delete, STRING("watasu"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_msg_print, 0, STRING("powerup_ev_04"), 0, STRING("me"))
    CALL(evt_mario_set_pose, STRING("M_N_1"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("me"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -335, 100, 158, -335, 33, -236, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("powerup_ev_05"), 0, STRING("me"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_2"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 2160, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_npc_set_rotate, STRING("me"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    SPAWN_EVENT_GET_ID(PTR(unk_evt_gor_0003e5c8), LW(5))
    CALL(evt_npc_get_position, STRING("me"), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_STARSTONE_SHINE1"), LW(7), LW(8), LW(9), LW(6))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LOTTERY_BOMB_BURST2"), LW(7), LW(8), LW(9), 0)
    SET_VALUE(LW(10), LW(8))
    ADD(LW(10), 200)
    CALL(evt_sub_intpl_msec_init, 0, LW(8), LW(10), 1600)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_position, STRING("me"), LW(7), LW(0), LW(9))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    DELETE_EVENT(LW(5))
    CALL(evt_npc_set_ry_lr, STRING("me"), 1)
    WAIT_MS(400)
    CALL(evt_snd_sfxoff, LW(6))
    WAIT_MS(1600)
    CALL(evt_mario_get_pos, 0, LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_QUESTION1"), LW(7), LW(8), LW(9), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 1, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(2000)
    CALL(evt_npc_get_position, STRING("me"), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BREAK_FLOOR1"), LW(7), LW(8), LW(9), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1000)
    WAIT_MS(500)
    CALL(evt_item_entry, STRING("orb"), 88, -365, 150, -110, 13, -1, 0)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(0), -365)
            CALL(evt_mario_set_dir, 90, 200, 0)
        ELSE
            CALL(evt_mario_set_dir, 270, 200, 0)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(0), -365)
            CALL(evt_party_set_dir, 0, 90, 200)
        ELSE
            CALL(evt_party_set_dir, 0, 270, 200)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 2160, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_npc_set_rotate, STRING("me"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_set_position, STRING("me"), -400, 200, -100)
    CALL(evt_npc_get_position, STRING("me"), LW(7), LW(8), LW(9))
    CALL(evt_sub_intpl_msec_init, 0, LW(8), 0, 1600)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_position, STRING("me"), LW(7), LW(0), LW(9))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_get_position, STRING("me"), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_NPC_LANDING1"), LW(7), LW(8), LW(9), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    WAIT_MS(750)
    CALL(evt_msg_print, 0, STRING("powerup_ev_06"), 0, STRING("me"))
    SPAWN_EVENT(unk_evt_gor_0003e648)
    CALL(evt_party_stop, 0)
    SPAWN_EVENT(unk_evt_gor_0003e708)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_move_position, STRING("me"), -365, -115, 0, FLOAT(120.0f), 0)
    WAIT_MS(500)
    CALL(evt_item_delete, STRING("orb"))
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("me"), -338, -160, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    WAIT_MS(1500)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_POWERUP1"))
    CALL(bgm_start_wait)
    CALL(evt_eff64, STRING("aaax"), STRING("lensflare_n64"), 0, -362, 27, -180, FLOAT(1.0f), -1, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_sui_ao"), 1)
    WAIT_MS(1500)
    CALL(evt_eff_softdelete, STRING("aaax"))
    SET_VALUE(GSWF(1186), 1)
    CALL(stonechangertn, STRING("S_i01_sui_aka"))
    CALL(evt_snd_bgmoff_f, 513, 2000)
    WAIT_MS(2000)
    CALL(evt_npc_jump_position_nohit, STRING("me"), -320, 0, -205, 500, 50)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("powerup_ev_07"), 0, STRING("me"))
    CALL(evt_pouch_remove_item, 108, LW(0))
    SET_VALUE(GSWF(1203), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_party_run, 0)
    CALL(evt_snd_bgmoff_f, 513, 2000)
    CALL(evt_snd_bgmon_f, 288, 0, 2000)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_gor_0003f020)
    IF_INT_EQUAL(GSWF(1203), 1)
        IF_INT_EQUAL(GSWF(1204), 0)
            CALL(evt_msg_print, 0, STRING("powerup_ev_01"), 0, STRING("me"))
            CALL(evt_cam3d_evt_set, -355, 114, 371, -355, 45, -23, 500, 11)
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("powerup_ev_02"), 0, STRING("me"))
            SPAWN_EVENT_GET_ID(PTR(powerupya_direct_talk), LW(10))
            WAIT_MS(500)
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
            CALL(evt_npc_move_position, STRING("me"), -320, -205, 0, FLOAT(120.0f), 0)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
            LOOP(0)
                IS_EVENT_RUNNING(LW(10), LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_cam3d_evt_off, 500, 11)
            WAIT_MS(500)
            SET_VALUE(LW(0), PTR(door_data_01))
            CALL_CHILD(evt_door_entry)
            SET_VALUE(GSWF(1204), 1)
        ELSE
            CALL(evt_pouch_check_item, 108, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(evt_win_item_select, 0, PTR(unk_gor_0003e5c0), LW(0), 0)
                IF_INT_NOT_EQUAL(LW(0), -1)
                    CALL_CHILD(unk_evt_gor_0003e7a0)
                END_IF
            ELSE
                CALL(evt_msg_print, 0, STRING("powerup_ev_03"), 0, STRING("me"))
            END_IF
        END_IF
        RETURN
    END_IF
    CALL(evt_msg_print, 0, STRING("powerup_05"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(mapball_init)
    IF_INT_EQUAL(GSWF(1186), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_sui_aka"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i01_sui_ao"), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(powerupya_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_power_up"), 0, PTR(powerupya_table_talk), 0)
    IF_INT_EQUAL(GSWF(1203), 1)
        IF_INT_EQUAL(GSWF(1204), 0)
            CALL(evt_npc_set_position, STRING("パワーアップ屋"), -320, 0, -10)
            SET_VALUE(LW(0), PTR(door_data_01))
            CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
            IF_INT_NOT_EQUAL(LW(1), 0)
                CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
            END_IF
            IF_INT_NOT_EQUAL(LW(2), 0)
                CALL(evt_hitobj_onoff, LW(2), 1, 0)
            END_IF
            RETURN
        END_IF
    END_IF
    SET_VALUE(LW(0), PTR(door_data_01))
    CALL_CHILD(evt_door_entry)
    RETURN
EVT_END()

EVT_BEGIN(johoyaperfect_talk)
    IF_INT_EQUAL(GSWF(4), 0)
        CALL(evt_msg_print, 0, STRING("johoyaperfect_00"), 0, STRING("me"))
        SET_VALUE(GSWF(4), 1)
    ELSE
        CALL(evt_msg_print, 0, STRING("johoyaperfect_00_01"), 0, STRING("me"))
    END_IF
    LOOP(0)
        CALL(johoya_data_alloc, STRING("jperfect_code"))
        CALL(evt_win_coin_on, 0, LW(12))
        CALL(evt_win_other_select, 17)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_msg_print, 0, STRING("johoyaperfect_03"), 0, STRING("me"))
            CALL(johoya_data_free)
            RETURN
        END_IF
        IF_INT_EQUAL(LW(4), 0)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_msg_print_insert, 0, STRING("johoyaperfect_06"), 0, STRING("me"), LW(3), LW(2))
            CALL(evt_msg_print_add, 0, LW(1))
            CALL(evt_msg_print_add, 0, STRING("johoyaperfect_07"))
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("johoyaperfect_01"), 0, STRING("me"), LW(3), LW(2), LW(4))
            CALL(evt_msg_select, 0, STRING("johoyaperfect_02"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_pouch_get_coin, LW(0))
                IF_INT_LESS(LW(0), LW(4))
                    CALL(evt_msg_print_add, 0, STRING("johoyaperfect_09"))
                    CALL(evt_win_coin_off, LW(12))
                    CALL(johoya_data_free)
                    RETURN
                END_IF
                MULTIPLY(LW(4), -1)
                CALL(evt_pouch_add_coin, LW(4))
                CALL(evt_win_coin_wait, LW(12))
                WAIT_MS(500)
                CALL(evt_win_coin_off, LW(12))
                CALL(evt_msg_print_add, 0, LW(1))
                CALL(johoya_perfect_newjoho_setreadflag)
                CALL(evt_msg_print_add, 0, STRING("johoyaperfect_07"))
            ELSE
                CALL(evt_win_coin_off, LW(12))
                CALL(evt_msg_print_add, 0, STRING("johoyaperfect_04"))
                CALL(johoya_data_free)
                RETURN
            END_IF
        END_IF
        CALL(johoya_data_free)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(kurihakase_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    SWITCH(GSW(0))
        CASE_EQUAL(8)
        CALL(evt_npc_set_position, STRING("me"), -135, 0, -240)
        CASE_EQUAL(9)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_EQUAL(10)
        CALL(evt_npc_set_position, STRING("me"), -220, 0, 305)
        CASE_EQUAL(11)
        IF_INT_EQUAL(GSWF(1207), 0)
            CALL(evt_npc_set_position, STRING("me"), -220, 0, 305)
        ELSE
            CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        END_IF
        CASE_BETWEEN(11, 17)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_BETWEEN(376, 402)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_EQUAL(18)
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -225)
        CASE_EQUAL(64)
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -225)
        CASE_EQUAL(120)
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -225)
        CASE_EQUAL(174)
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -225)
        CASE_EQUAL(221)
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -225)
        CASE_EQUAL(271)
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -225)
        CASE_EQUAL(347)
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -225)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), -140, 0, -250)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(kurihakase_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(11)
        CALL(evt_msg_print, 0, STRING("mac_0_085"), 0, STRING("me"))
        CASE_LESS(23)
        CALL(evt_msg_print, 0, STRING("gor_02_013"), 0, STRING("me"))
        CASE_LESS(57)
        CALL(evt_msg_print, 0, STRING("gor_02_014"), 0, STRING("me"))
        CASE_BETWEEN(56, 64)
        CALL(evt_msg_print, 0, STRING("mac_1_000"), 0, STRING("me"))
        CASE_LESS(71)
        CALL(evt_msg_print, 0, STRING("gor_02_015"), 0, STRING("me"))
        CASE_LESS(113)
        CALL(evt_msg_print, 0, STRING("gor_02_016"), 0, STRING("me"))
        CASE_BETWEEN(112, 120)
        CALL(evt_msg_print, 0, STRING("mac_2_000"), 0, STRING("me"))
        CASE_LESS(127)
        CALL(evt_msg_print, 0, STRING("gor_02_017"), 0, STRING("me"))
        CASE_LESS(165)
        CALL(evt_msg_print, 0, STRING("gor_02_018"), 0, STRING("me"))
        CASE_BETWEEN(165, 174)
        CALL(evt_msg_print, 0, STRING("mac_3_000"), 0, STRING("me"))
        CASE_LESS(178)
        IF_INT_EQUAL(GSWF(1341), 0)
            CALL(evt_msg_print, 0, STRING("gor_02_019"), 0, STRING("me"))
        ELSE
            IF_INT_EQUAL(GSWF(1218), 0)
                CALL(evt_party_stop, 0)
                CALL(evt_msg_print_party, STRING("mac_3_008"))
                CALL(evt_msg_print, 0, STRING("mac_3_009"), 0, STRING("me"))
                WAIT_MS(1500)
                CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリハカセ"), 0, 0, 0, 0, 0, 0, 0, 60)
                WAIT_MS(1000)
                CALL(evt_msg_print, 0, STRING("mac_3_010"), 0, STRING("me"))
                SET_VALUE(GSWF(1218), 1)
                CALL(evt_party_run, 0)
            ELSE
                CALL(evt_msg_print, 0, STRING("gor_02_020"), 0, STRING("me"))
            END_IF
        END_IF
        CASE_BETWEEN(212, 221)
        CALL(evt_msg_print, 0, STRING("mac_4_000"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_021"), 0, STRING("me"))
        CASE_LESS(224)
        CALL(evt_msg_print, 0, STRING("gor_02_022"), 0, STRING("me"))
        CASE_EQUAL(224)
        CALL(evt_msg_print, 0, STRING("mac_4_048"), 0, STRING("me"))
        SET_VALUE(GSWF(1214), 1)
        CASE_EQUAL(225)
        CALL(evt_msg_print, 0, STRING("mac_4_060"), 0, STRING("me"))
        CASE_BETWEEN(262, 271)
        CALL(evt_msg_print, 0, STRING("mac_5_000"), 0, STRING("me"))
        CASE_LESS(269)
        CALL(evt_msg_print, 0, STRING("gor_02_023"), 0, STRING("me"))
        CASE_LESS(282)
        CALL(evt_msg_print, 0, STRING("gor_02_024"), 0, STRING("me"))
        CASE_LESS(340)
        CALL(evt_msg_print, 0, STRING("gor_02_025"), 0, STRING("me"))
        CASE_BETWEEN(340, 347)
        CALL(evt_msg_print, 0, STRING("mac_6_000"), 0, STRING("me"))
        CASE_EQUAL(348)
        CALL(evt_sub_load_progresstime, GSW(100), LW(0))
        IF_INT_EQUAL(GSWF(1220), 1)
            CALL(evt_msg_print, 0, STRING("mac_6_006"), 0, STRING("クリハカセ"))
            RETURN
        END_IF
        IF_INT_LESS(LW(0), 180000)
            SWITCH(LW(0))
                CASE_BETWEEN(0, 29999)
                CALL(evt_msg_print, 0, STRING("mac_6_006_01"), 0, STRING("クリハカセ"))
                CASE_BETWEEN(30000, 59999)
                CALL(evt_msg_print, 0, STRING("mac_6_006_02"), 0, STRING("クリハカセ"))
                CASE_BETWEEN(60000, 89999)
                CALL(evt_msg_print, 0, STRING("mac_6_006_03"), 0, STRING("クリハカセ"))
                CASE_BETWEEN(90000, 119999)
                CALL(evt_msg_print, 0, STRING("mac_6_006_04"), 0, STRING("クリハカセ"))
                CASE_BETWEEN(120000, 149999)
                CALL(evt_msg_print, 0, STRING("mac_6_006_05"), 0, STRING("クリハカセ"))
                CASE_BETWEEN(150000, 179999)
                CALL(evt_msg_print, 0, STRING("mac_6_006_06"), 0, STRING("クリハカセ"))
            END_SWITCH
        ELSE
            CALL(evt_cam_letter_box_disable, 1)
            CALL(evt_cam_letter_box_onoff, 1, 0)
            WAIT_MS(1000)
            SPAWN_EVENT(kurihakase_after3minutes)
        END_IF
        CASE_LESS(354)
        IF_INT_EQUAL(GSWF(3884), 0)
            CALL(evt_pouch_check_item, 11, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print, 0, STRING("gor_02_026_01"), 0, STRING("me"))
            ELSE
                CALL(evt_msg_print, 0, STRING("gor_02_026"), 0, STRING("me"))
            END_IF
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_02_027"), 0, STRING("me"))
        END_IF
        CASE_LESS(374)
        CALL(evt_msg_print, 0, STRING("gor_02_028"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(403)
        IF_INT_EQUAL(GSWF(1249), 0)
            CALL(evt_msg_print, 0, STRING("gor_02_029"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(1000)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("gor_02_029_01"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_I_N"))
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("gor_02_029_02"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            SET_VALUE(GSWF(1249), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_02_029_03"), 0, STRING("me"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(oyabun_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    RETURN
EVT_END()


EVT_BEGIN(oyabun_talk)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(120)
        IF_INT_EQUAL(GSWF(1209), 0)
            CALL(evt_msg_print, 0, STRING("gor_02_000"), 0, STRING("me"))
            SET_VALUE(GSWF(1209), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_02_001"), 0, STRING("me"))
        END_IF
        CASE_EQUAL(121)
        IF_INT_EQUAL(GSWF(1211), 1)
            CALL(evt_msg_print, 0, STRING("mac_2_040"), 0, STRING("me"))
            RETURN
        END_IF
        IF_INT_EQUAL(GSWF(1210), 0)
            CALL(evt_snd_bgmoff, 1024)
            CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_DANGER3"))
            CALL(evt_snd_envoff, 1024)
            CALL(evt_msg_print, 0, STRING("mac_2_012"), 0, STRING("me"))
            BEGIN_INLINE_EVENT
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
            END_INLINE_EVENT
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(800)
            CALL(evt_msg_print, 0, STRING("mac_2_013"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(800)
            CALL(evt_msg_print, 0, STRING("mac_2_014"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(800)
            CALL(evt_msg_print, 0, STRING("mac_2_015"), 0, STRING("me"))
            SET_VALUE(GSWF(1210), 1)
        ELSE
            CALL(evt_snd_envoff, 1024)
            CALL(evt_msg_print, 0, STRING("mac_2_016"), 0, STRING("me"))
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_2"), LW(0), LW(1), LW(2), 0)
        END_INLINE_EVENT
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_2_017"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("mac_2_017_yesno"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("mac_2_018"))
            CALL(evt_msg_select, 0, STRING("mac_2_018_yesno"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_pouch_get_coin, LW(3))
                IF_INT_LESS(LW(3), 64)
                    CALL(evt_msg_print_add, 0, STRING("mac_2_020"))
                    CALL(evt_snd_envoff, 513)
                    CALL(evt_snd_envon, 288, 0)
                    RETURN
                ELSE
                    CALL(evt_msg_continue)
                    CALL(evt_sub_status_gauge_force_open)
                    WAIT_MS(300)
                    CALL(evt_pouch_add_coin, -64)
                    LOOP(0)
                        CALL(evt_sub_status_gauge_check_update, LW(0))
                        IF_INT_EQUAL(LW(0), 0)
                            BREAK_LOOP
                        END_IF
                        WAIT_FRAMES(1)
                    END_LOOP
                    CALL(status_gauge_force_off)
                    WAIT_MS(300)
                    CALL(evt_msg_print, 0, STRING("mac_2_019"), 0, STRING("me"))
                    GOTO(38)
                END_IF
            ELSE
                CALL(evt_msg_print_add, 0, STRING("mac_2_021"))
                CALL(evt_snd_envoff, 513)
                CALL(evt_snd_envon, 288, 0)
                RETURN
            END_IF
        ELSE
            CALL(evt_msg_print_add, 0, STRING("mac_2_022"))
            CALL(evt_snd_envoff, 513)
            CALL(evt_snd_envon, 288, 0)
            RETURN
        END_IF
        LABEL(38)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_cam3d_evt_set, LW(0), 72, -753, LW(0), 52, -600, 500, 11)
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_2_038"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("mac_2_025_yesno"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_continue)
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(800)
        ELSE
            CALL(evt_msg_print_add, 0, STRING("mac_2_039"))
        END_IF
        SET_VALUE(GSWF(1211), 1)
        CALL(evt_cam3d_evt_off, 500, 11)
        CASE_BETWEEN(122, 126)
        CALL(evt_msg_print, 0, STRING("gor_02_002"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_02_003"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_003_01"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_003_02"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_003_03"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_003_04"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_003_05"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_02_003_06"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(tozokudan1_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    IF_INT_EQUAL(GSWF(1213), 1)
        CALL(evt_hitobj_onoff, STRING("A_center_kabe_atr"), 1, 0)
        CALL(evt_npc_set_position, STRING("me"), -153, 0, -459)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    ELSE
        CALL(evt_npc_set_position, STRING("me"), -82, 0, 134)
    END_IF
    SET_VALUE(GW(10), 0)
    RETURN
EVT_END()

EVT_BEGIN(tozokudan1_talk)
    IF_INT_EQUAL(GSWF(1213), 0)
        IF_INT_EQUAL(GW(10), 1)
            CALL(evt_msg_print, 0, STRING("gor_02_006_01"), 0, STRING("me"))
            RETURN
        END_IF
        IF_INT_EQUAL(GSWF(1212), 0)
            CALL(evt_msg_print, 0, STRING("gor_02_004"), 0, STRING("me"))
            SET_VALUE(GSWF(1212), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("gor_02_005"), 0, STRING("me"))
        END_IF
        CALL(evt_msg_select, 0, STRING("gor_02_005_select"))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_pouch_get_coin, LW(0))
            IF_INT_LESS(LW(0), 10)
                CALL(evt_msg_print_add, 0, STRING("gor_02_006_02"))
                RETURN
            END_IF
            CALL(evt_msg_print_add, 0, STRING("gor_02_006"))
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
            CALL(evt_npc_move_position, STRING("me"), -102, 66, 0, FLOAT(120.0f), 0)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
            CALL(evt_hitobj_onoff, STRING("A_center_kabe_atr"), 1, 0)
            SET_VALUE(GW(10), 1)
            CALL(evt_pouch_add_coin, -10)
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("gor_02_007"))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("gor_02_008"))
            CALL(evt_npc_set_battle_info, STRING("me"), 5)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_npc_battle_start, STRING("me"))
            CALL(evt_npc_wait_battle_end)
            CALL(evt_mario_set_pos, -150, 0, 135)
            CALL(evt_party_set_pos, 0, -180, 0, 135)
            CALL(evt_mario_set_dir, 90, 0, 0)
            CALL(evt_party_set_dir, 0, 90, -1)
            WAIT_MS(500)
            CALL(evt_npc_get_battle_result, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                CALL(evt_msg_print, 0, STRING("gor_02_009"), 0, STRING("me"))
                CALL(evt_hitobj_onoff, STRING("A_center_kabe_atr"), 1, 0)
                CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
                CALL(evt_npc_set_ry, STRING("me"), 90)
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_MONBAN_ESCAPE1"), LW(0), LW(1), LW(2), 0)
                CALL(evt_npc_move_position, STRING("me"), 150, 155, 0, FLOAT(240.0f), 0)
                CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
                CALL(evt_npc_set_position, STRING("me"), -153, 0, -459)
                SET_VALUE(GSWF(1213), 1)
            ELSE
                CALL(evt_msg_print, 0, STRING("gor_02_008_00"), 0, STRING("me"))
            END_IF
            CALL(evt_mario_key_onoff, 1)
        END_SWITCH
    ELSE
        SWITCH(GSW(0))
            CASE_LESS(56)
            CALL(evt_msg_print, 0, STRING("gor_02_010"), 0, STRING("me"))
            CASE_LESS(112)
            CALL(evt_msg_print, 0, STRING("gor_02_010_01"), 0, STRING("me"))
            CASE_LESS(164)
            CALL(evt_msg_print, 0, STRING("gor_02_010_02"), 0, STRING("me"))
            CASE_LESS(215)
            CALL(evt_msg_print, 0, STRING("gor_02_010_03"), 0, STRING("me"))
            CASE_LESS(261)
            CALL(evt_msg_print, 0, STRING("gor_02_010_04"), 0, STRING("me"))
            CASE_LESS(339)
            CALL(evt_msg_print, 0, STRING("gor_02_010_05"), 0, STRING("me"))
            CASE_LESS(373)
            CALL(evt_msg_print, 0, STRING("gor_02_010_06"), 0, STRING("me"))
            CASE_LESS(401)
            CALL(evt_msg_print, 0, STRING("gor_02_010_07"), 0, STRING("me"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("gor_02_010_08"), 0, STRING("me"))
        END_SWITCH
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(tozokudan2_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    IF_INT_EQUAL(GSW(1420), 1)
        CALL(evt_npc_set_position, STRING("me"), 445, 0, 120)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("me"), 10, 0, -460)
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x00000031, 0xFFFFFFFF,
};

static EVT_BEGIN(evt_chk)
    CALL(evt_pouch_check_item, 49, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(tozokudan2_house_door)
    SET_VALUE(GF(0), 1)
    SET_VALUE(GF(1), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_06_naka"), 1)
    SET_VALUE(LW(0), PTR(door_data_06))
    SPAWN_EVENT(door_open_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 0)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(0)
            END_IF
            SET_VALUE(GF(0), 0)
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            SPAWN_EVENT(door_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 1)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(1)
            END_IF
            SET_VALUE(GF(1), 0)
        END_INLINE_EVENT
    END_IF
    WAIT_WHILE(GF(0))
    WAIT_WHILE(GF(1))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_06_naka"), 1)
    RETURN
EVT_END()

EVT_BEGIN(mario_yokeru)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("盗賊団２"), LW(10), LW(11), LW(12))
    SET_VALUE(LW(3), LW(10))
    SET_VALUE(LW(4), LW(10))
    SUBTRACT(LW(3), 20)
    ADD(LW(4), 20)
    IF_INT_LESS(LW(2), LW(12))
        IF_INT_GREATER_OR_EQUAL(LW(0), LW(3))
            IF_INT_LESS_OR_EQUAL(LW(0), LW(4))
                IF_INT_LESS_OR_EQUAL(LW(0), LW(10))
                    BEGIN_INLINE_EVENT
                        ADD(LW(2), 15)
                        CALL(evt_party_move_pos, 0, 415, LW(2), 300)
                        CALL(evt_party_set_dir, 0, 90, 200)
                    END_INLINE_EVENT
                    BEGIN_INLINE_EVENT
                        ADD(LW(2), 30)
                        CALL(evt_party_move_pos, 1, 415, LW(2), 300)
                        CALL(evt_party_set_dir, 1, 90, 200)
                    END_INLINE_EVENT
                    CALL(evt_mario_mov_pos, 415, LW(2), 300)
                    CALL(evt_mario_set_dir, 90, 200, 0)
                ELSE
                    BEGIN_INLINE_EVENT
                        ADD(LW(2), 15)
                        CALL(evt_party_move_pos, 0, 480, LW(2), 300)
                        CALL(evt_party_set_dir, 0, 270, 200)
                    END_INLINE_EVENT
                    BEGIN_INLINE_EVENT
                        ADD(LW(2), 30)
                        CALL(evt_party_move_pos, 1, 480, LW(2), 300)
                        CALL(evt_party_set_dir, 1, 270, 200)
                    END_INLINE_EVENT
                    CALL(evt_mario_mov_pos, 480, LW(2), 300)
                    CALL(evt_mario_set_dir, 270, 200, 0)
                END_IF
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_unlock_mario)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊団２"), 1073741856)
    CALL(evt_msg_print, 0, STRING("irai_01_03"), 0, STRING("盗賊団２"))
    CALL(evt_sub_get_coin, 20)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    SPAWN_EVENT(mario_yokeru)
    CALL(evt_npc_jump_position_nohit, STRING("盗賊団２"), 445, 8, 80, 300, 30)
    WAIT_MS(500)
    SPAWN_EVENT(tozokudan2_house_door)
    CALL(evt_npc_move_position, STRING("盗賊団２"), 445, -85, 0, FLOAT(60.0f), 0)
    SET_VALUE(GSW(1420), 0)
    SET_VALUE(GSWF(5375), 1)
    SET_VALUE(LW(0), PTR(door_data_06))
    CALL_CHILD(evt_door_entry)
    CALL(evt_npc_flag_onoff, 0, STRING("盗賊団２"), 1073741856)
    CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock)
    WAIT_MS(500)
    SPAWN_EVENT(evt_unlock_mario)
    RETURN
EVT_END()

EVT_BEGIN(evt_unlock_npc)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊団２"), 1073741856)
    CALL(evt_msg_print, 0, STRING("irai_01_03"), 0, STRING("盗賊団２"))
    CALL(evt_sub_get_coin, 20)
    SPAWN_EVENT(mario_yokeru)
    CALL(evt_npc_jump_position_nohit, STRING("盗賊団２"), 445, 8, 80, 300, 30)
    WAIT_MS(500)
    CALL(evt_mobj_set_position, STRING("mobj_lock00"), 0, -1000, 0)
    SET_VALUE(GSWF(1219), 1)
    CALL(evt_npc_get_position, STRING("盗賊団２"), LW(0), LW(1), LW(2))
    WAIT_MS(100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_LOCK_SWING1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_UNLOCK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_pouch_remove_item, 49, LW(0))
    SPAWN_EVENT(tozokudan2_house_door)
    CALL(evt_npc_move_position, STRING("盗賊団２"), 445, -85, 0, FLOAT(60.0f), 0)
    SET_VALUE(GSW(1420), 0)
    SET_VALUE(GSWF(5375), 1)
    SET_VALUE(LW(0), PTR(door_data_06))
    CALL_CHILD(evt_door_entry)
    CALL(evt_npc_flag_onoff, 0, STRING("盗賊団２"), 1073741856)
    CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(tozokudan2_talk)
    IF_INT_EQUAL(GSW(1420), 1)
        CALL(evt_pouch_check_item, 49, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(evt_msg_print, 0, STRING("irai_01_02"), 0, STRING("me"))
            CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
            IF_INT_NOT_EQUAL(LW(0), -1)
                CALL_CHILD(evt_unlock_npc)
            END_IF
            RETURN
        ELSE
            IF_INT_EQUAL(GSWF(5406), 0)
                CALL(evt_msg_print, 0, STRING("irai_01_00"), 0, STRING("me"))
                SET_VALUE(GSWF(5406), 1)
            ELSE
                CALL(evt_msg_print, 0, STRING("irai_01_01"), 0, STRING("me"))
            END_IF
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_02_011"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_02_011_01"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_011_02"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_011_03"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_011_04"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_011_05"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_011_06"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_02_011_07"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(tozokudan3_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    CALL(evt_npc_set_position, STRING("me"), 220, 0, -185)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    RETURN
EVT_END()

EVT_BEGIN(tozokudan3_talk)
    IF_INT_EQUAL(GSWF(1222), 0)
        CALL(evt_msg_print, 0, STRING("irai_oku_00"), 0, STRING("me"))
        SET_VALUE(GSWF(1222), 1)
    ELSE
        CALL(evt_msg_print, 0, STRING("irai_oku_01"), 0, STRING("me"))
    END_IF
    RETURN
EVT_END()


EVT_BEGIN(sanders_open_door)
    IF_INT_EQUAL(GSWF(1216), 1)
        RETURN
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_npc_set_ry, STRING("me"), 270)
    CALL(evt_npc_move_position, STRING("me"), 0, 25, 0, FLOAT(60.0f), 0)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_MOBJ_UNLOCK1"), 0)
    WAIT_MS(200)
    CALL(evt_npc_move_position, STRING("me"), 50, 25, 0, FLOAT(60.0f), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
    CALL(set_talk_memory_ry)
    SET_VALUE(GSWF(1216), 1)
    CALL(evt_msg_print, 0, STRING("mac_4_056_01"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(sanders_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    SWITCH(GSW(0))
        CASE_LESS(224)
        CALL(evt_npc_set_position, STRING("me"), 0, 0, -50)
        CASE_LESS(227)
        IF_INT_EQUAL(GSWF(1216), 0)
            CALL(evt_npc_set_position, STRING("me"), 0, 0, -50)
        ELSE
            CALL(evt_npc_set_position, STRING("me"), 50, 0, -30)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(sanders_talk)
    SWITCH(GSW(0))
        CASE_LESS(224)
        CALL(evt_msg_print, 0, STRING("mac_4_049_01"), 0, STRING("me"))
        CALL_CHILD(sanders_open_door)
        CASE_EQUAL(224)
        IF_INT_EQUAL(GSWF(1214), 0)
            CALL(evt_msg_print, 0, STRING("mac_4_050"), 0, STRING("me"))
            CALL_CHILD(sanders_open_door)
            SET_VALUE(GSWF(1215), 1)
        ELSE
            CALL(evt_party_stop, 0)
            CALL(evt_msg_print, 0, STRING("mac_4_051"), 0, STRING("me"))
            CALL(evt_party_set_dir_npc, 0, STRING("me"))
            CALL(evt_msg_print_party, STRING("mac_4_052"))
            CALL(evt_party_run, 0)
            CALL(evt_msg_print, 0, STRING("mac_4_053"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("mac_4_054"), 0, STRING("me"))
            CALL(evt_msg_print_party, STRING("mac_4_055"))
            CALL(evt_msg_print, 0, STRING("mac_4_056"), 0, STRING("me"))
            CALL_CHILD(sanders_open_door)
            SET_VALUE(GSW(0), 225)
        END_IF
        CASE_EQUAL(225)
        CALL(evt_msg_print, 0, STRING("mac_4_057"), 0, STRING("me"))
        CASE_EQUAL(226)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
        CALL(evt_msg_print, 0, STRING("mac_4_077"), 0, STRING("me"))
        BEGIN_INLINE_EVENT
            CALL(evt_party_move_pos2, 0, -20, -10, FLOAT(120.0f))
            CALL(evt_party_set_dir, 0, 90, -1)
        END_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 20, -30, FLOAT(120.0f))
        WAIT_FRAMES(1)
        CALL(evt_mario_set_dir, 90, 200, 0)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        WAIT_MS(500)
        CALL(evt_item_entry, STRING("item00"), 69, LW(0), LW(1), LW(2), 17, -1, 0)
        CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
        SUBTRACT(LW(3), 15)
        ADD(LW(4), 22)
        ADD(LW(5), 5)
        CALL(evt_item_set_position, STRING("item00"), LW(3), LW(4), LW(5))
        CALL(evt_mario_set_pose, STRING("M_C_3"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        SUBTRACT_FLOAT(LW(0), 15)
        ADD_FLOAT(LW(1), 2)
        ADD_FLOAT(LW(2), 5)
        CALL(evt_item_set_position, STRING("item00"), LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_4_078"), 0, STRING("me"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(500)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        BEGIN_INLINE_EVENT_GET_ID(GW(0))
            LOOP(0)
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                SUBTRACT_FLOAT(LW(0), 15)
                ADD_FLOAT(LW(1), 2)
                ADD_FLOAT(LW(2), 5)
                CALL(evt_item_set_position, STRING("item00"), LW(0), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 225, 30)
        WAIT_MS(500)
        DELETE_EVENT(GW(0))
        CALL(evt_snd_bgmoff_f, 1024, 2000)
        CALL(evt_msg_print, 0, STRING("mac_4_079"), 0, STRING("me"))
        CALL(evt_item_get_position, STRING("item00"), LW(0), LW(1), LW(2))
        CALL(evt_item_set_position, STRING("item00"), 0, -1000, 0)
        CALL(evt_item_entry, STRING("tegami"), 80, LW(0), LW(1), LW(2), 17, -1, 0)
        BEGIN_INLINE_EVENT
            CALL(evt_item_get_position, STRING("tegami"), LW(2), LW(3), LW(4))
            ADD(LW(4), 10)
            SET_VALUE(LW(5), LW(2))
            ADD(LW(5), 30)
            CALL(evt_sub_intpl_msec_init, 0, 0, 1000, 300)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            SET_FLOAT_VALUE(LW(6), LW(5))
            SUBTRACT_FLOAT(LW(6), LW(2))
            MULTIPLY_FLOAT(LW(6), LW(0))
            ADD_FLOAT(LW(6), LW(2))
            CALL(evt_item_set_position, STRING("tegami"), LW(6), LW(3), LW(4))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
        END_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("me"), 90)
        WAIT_MS(500)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_ORGEL1"))
        CALL(evt_msg_print, 0, STRING("mac_4_080"), 0, STRING("me"))
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("mac_4_081"), 0, 0)
        CALL(evt_msg_print, 0, STRING("mac_4_082"), 0, STRING("me"))
        CALL(evt_item_set_position, STRING("tegami"), 0, -1000, 0)
        WAIT_MS(300)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_move_position, STRING("me"), 0, -140, 0, FLOAT(60.0f), 0)
        SPAWN_EVENT(evt_gor_door_open_sanders)
        CALL(evt_npc_move_position, STRING("me"), 0, -90, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_move_position, STRING("me"), 0, -193, 0, FLOAT(60.0f), 0)
        WAIT_MS(3000)
        CALL(evt_mario_set_dir, 270, 200, 0)
        WAIT_MS(2000)
        CALL(evt_mario_set_dir, 90, 200, 0)
        WAIT_MS(3000)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("mac_4_083"), 0, 0)
        WAIT_MS(2000)
        SPAWN_EVENT(evt_gor_door_close_sanders)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_move_position, STRING("me"), 0, -80, 0, FLOAT(60.0f), 0)
        CALL(evt_npc_move_position, STRING("me"), 50, -30, 0, FLOAT(60.0f), 0)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
        WAIT_MS(500)
        CALL(evt_snd_bgmoff_f, 513, 2000)
        CALL(evt_msg_print, 0, STRING("mac_4_084"), 0, STRING("me"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("mac_4_085"), 0, STRING("me"))
        CALL(evt_msg_print_party, STRING("mac_4_086"))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CALL(evt_party_stop, 1)
        CALL(evt_mario_set_party_pos, 1, 15, LW(0), LW(1), LW(2))
        CALL(evt_party_run, 1)
        CALL(evt_pouch_remove_item, 69, LW(0))
        CALL(evt_party_run, 0)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        SET_VALUE(GSW(0), 227)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kuribo4_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    CALL(evt_npc_set_position, STRING("me"), -370, 0, 130)
    RETURN
EVT_END()

EVT_BEGIN(kuribo4_regl)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(70.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

// TODO: type
static u32 item_tbl[2] = {
    0x0000005F, 0xFFFFFFFF,
};




EVT_BEGIN(kuribo4_talk)
    IF_INT_EQUAL(GSW(1420), 2)
        IF_INT_EQUAL(GSWF(5405), 1)
            CALL(evt_pouch_check_item, 95, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 1)
                CALL(evt_msg_print, 0, STRING("irai_02_03"), 0, STRING("me"))
                CALL(evt_win_item_select, 0, PTR(item_tbl), LW(0), 0)
                IF_INT_NOT_EQUAL(LW(0), -1)
                    CALL(evt_msg_print, 0, STRING("irai_02_04"), 0, STRING("me"))
                    CALL(evt_pouch_remove_item, 95, LW(0))
                    SET_VALUE(GSWF(5406), 1)
                END_IF
                RETURN
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(1420), 13)
        IF_INT_EQUAL(GSWF(5406), 1)
            CALL(evt_pouch_check_item, 107, LW(0))
            IF_INT_GREATER(LW(0), 0)
                CALL(iri_13_make_item_tbl)
                IF_INT_EQUAL(GSWF(5407), 0)
                    CALL(evt_msg_print, 0, STRING("irai_13_001"), 0, 0)
                    CALL(evt_mario_set_pose, STRING("M_N_5B"))
                    WAIT_MS(500)
                    CALL(evt_mario_set_pose, STRING("M_N_C"))
                    WAIT_MS(500)
                    CALL(evt_mario_set_pose, STRING("M_S_1"))
                    CALL(evt_msg_print, 0, STRING("irai_13_002"), 0, 0)
                    CALL(iri_13_update_item_tbl, LW(1), LW(0))
                    LOOP(0)
                        CALL(evt_win_item_select, 0, LW(1), LW(0), 0)
                        IF_INT_NOT_EQUAL(LW(0), 107)
                            BREAK_LOOP
                        END_IF
                        CALL(evt_pouch_remove_item, 107, LW(0))
                        CALL(iri_13_update_item_tbl, LW(1), LW(0))
                        IF_INT_EQUAL(LW(0), 0)
                            BREAK_LOOP
                        END_IF
                        CALL(evt_msg_print, 0, STRING("irai_13_003"), 0, 0)
                    END_LOOP
                    SET_VALUE(GSWF(5407), 1)
                ELSE
                    LOOP(0)
                        CALL(iri_13_update_item_tbl, LW(1), LW(0))
                        IF_INT_EQUAL(LW(0), 0)
                            BREAK_LOOP
                        END_IF
                        CALL(evt_msg_print, 0, STRING("irai_13_005"), 0, 0)
                        CALL(evt_win_item_select, 0, LW(1), LW(0), 0)
                        IF_INT_NOT_EQUAL(LW(0), 107)
                            BREAK_LOOP
                        END_IF
                        CALL(evt_pouch_remove_item, 107, LW(0))
                    END_LOOP
                END_IF
                CALL(evt_pouch_check_item, 107, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(evt_msg_print, 0, STRING("irai_13_004"), 0, 0)
                END_IF
                CALL(iri_13_free_item_tbl)
                RETURN
            END_IF
        END_IF
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_02_030_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_02_030_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_02_030_02_01"), 0, STRING("me"))
        CASE_LESS(175)
        CALL(evt_msg_print, 0, STRING("gor_02_030_03"), 0, STRING("me"))
        CASE_LESS(176)
        CALL(evt_msg_print, 0, STRING("gor_02_030_03_01"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_030_03_02"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_030_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_030_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_030_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_030_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_02_030_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(chusan3_init)
    RETURN
EVT_END()

EVT_BEGIN(chusan3_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_02_031_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_02_031_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_02_031_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_031_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_031_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_031_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_031_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_031_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_02_031_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(roten3_init)
    RETURN
EVT_END()

EVT_BEGIN(roten3_talk)
    IF_INT_EQUAL(GSW(1420), 3)
        SET_VALUE(LW(1), 0)
        SET_VALUE(LW(2), 0)
        SET_VALUE(LW(3), 0)
        CALL(evt_msg_print, 0, STRING("irai_03_00"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("irai_03_00_s"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("irai_03_02"))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("irai_03_01"))
            RETURN
        END_IF
        CALL(evt_msg_select, 0, STRING("irai_03_02_s"))
        IF_INT_EQUAL(LW(0), 1)
            SET_VALUE(LW(1), 1)
        END_IF
        CALL(evt_msg_print_add, 0, STRING("irai_03_03"))
        CALL(evt_msg_select, 0, STRING("irai_03_03_s"))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LW(2), 1)
        END_IF
        CALL(evt_msg_print_add, 0, STRING("irai_03_04"))
        CALL(evt_msg_select, 0, STRING("irai_03_04_s"))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LW(3), 1)
        END_IF
        CALL(evt_msg_print_add, 0, STRING("irai_03_05"))
        CALL(evt_sub_get_coin, 20)
        SET_VALUE(GSW(1420), 0)
        SET_VALUE(GSWF(5377), 1)
        CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_02_032_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_02_032_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_02_032_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_032_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_032_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_032_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_032_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_032_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_02_032_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borodo1_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    SWITCH(GSW(0))
        CASE_BETWEEN(215, 261)
        CALL(evt_npc_set_position, STRING("me"), 550, 0, 275)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 90, 0, 150)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borodo1_regl)
    SWITCH(GSW(0))
        CASE_BETWEEN(215, 261)
        CASE_DEFAULT
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(70.0f), FLOAT(30.0f), 0)
        CALL(urouro_main_func, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borodo1_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_02_033_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_02_033_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_02_033_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_033_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_033_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_033_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_033_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_033_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_02_033_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(borodo2_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    IF_INT_EQUAL(GSW(1420), 4)
        IF_INT_EQUAL(GSWF(5406), 1)
            CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
            RETURN
        END_IF
        CALL(evt_npc_set_position, STRING("me"), -475, 0, -180)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("me"), -475, 0, -180)
    RETURN
EVT_END()

static EVT_BEGIN(borodo2_escape)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_npc_set_ry, STRING("me"), 180)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("me"), -475, -85, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("me"), -530, -85, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("me"), -530, 20, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(borodo2_talk)
    IF_INT_EQUAL(GSW(1420), 4)
        IF_INT_EQUAL(GSWF(5405), 1)
            IF_INT_EQUAL(GSWF(5406), 0)
                CALL(evt_msg_print, 0, STRING("irai_04_003"), 0, STRING("me"))
                SET_VALUE(GSWF(5406), 1)
                SPAWN_EVENT(borodo2_escape)
                RETURN
            END_IF
        END_IF
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(23)
        CALL(evt_msg_print, 0, STRING("gor_02_033_00_2"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_02_033_01_2"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_02_033_02_2"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_033_03_2"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_033_04_2"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_033_05_2"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_033_06_2"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_033_07_2"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_02_033_08_2"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kuragarisan_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    RETURN
EVT_END()


EVT_BEGIN(kuragarisan_write_name)
    CALL(evt_npc_get_position, STRING("me"), LW(13), LW(14), LW(15))
    CALL(evt_npc_get_dir, STRING("me"), LW(12))
    CALL(evt_party_stop, 0)
    CALL(evt_party_outofscreen, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_party_set_pos, 0, -516, 0, -146)
    END_IF
    SET_VALUE(GF(0), 1)
    SET_VALUE(GF(1), 1)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(2), -320)
            CALL(evt_party_move_pos2, 0, -480, -335, FLOAT(60.0f))
        END_IF
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 40)
        SUBTRACT(LW(2), 40)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(10000)
        SET_VALUE(GF(1), 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_3_011"), 0, STRING("me"))
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_3_012"), 0, STRING("me"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_3_013"), 0, STRING("me"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NO1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_B"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_3_014"), 0, STRING("me"))
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_3_015"), 0, STRING("me"))
    LOOP(0)
        IF_INT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(GF(1), 0)
            DEBUG_PUT_MSG(STRING("タイムアウトしました"))
            DELETE_EVENT(LW(10))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 40)
            SUBTRACT(LW(2), 40)
            CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_party_set_dir, 0, 90, 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 5)
    SUBTRACT(LW(2), 19)
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, 60, 0)
    CALL(evt_npc_set_ry, STRING("me"), 270)
    WAIT_MS(300)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_KURAGARI_PEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1A"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1B"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1C"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("mac_3_016"), PTR(kuragarisan_msgcb), STRING("me"))
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 4)
        CALL(evt_pouch_get_yoshiname, LW(1))
        CALL(evt_msg_print_insert, 0, STRING("mac_3_017_yos"), 0, STRING("party"), LW(1))
    ELSE
        CALL(evt_msg_print_party, STRING("mac_3_017"))
    END_IF
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_get_party, LW(3))
    SWITCH(LW(3))
        CASE_EQUAL(1)
        SET_VALUE(LW(3), 22)
        CASE_EQUAL(2)
        SET_VALUE(LW(3), 23)
        CASE_EQUAL(4)
        SET_VALUE(LW(3), 15)
        CASE_EQUAL(5)
        SET_VALUE(LW(3), 31)
        CASE_DEFAULT
        SET_VALUE(LW(3), 0)
    END_SWITCH
    ADD(LW(0), 5)
    SUBTRACT(LW(2), LW(3))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, 60, 0)
    CALL(evt_npc_set_ry, STRING("me"), 270)
    WAIT_MS(300)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_KURAGARI_PEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1A"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1B"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1C"))
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("mac_3_018"))
    CALL(evt_msg_print, 0, STRING("mac_3_019"), 0, STRING("me"))
    CALL(evt_npc_move_position, STRING("me"), LW(13), LW(15), 0, 60, 0)
    CALL(evt_npc_set_ry, STRING("me"), LW(12))
    CALL(evt_npc_status_onoff, 0, STRING("me"), 2)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
    SET_VALUE(GSW(0), 176)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

EVT_BEGIN(kuragarisan_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(175)
        IF_INT_EQUAL(GSWF(1341), 0)
            CALL(evt_msg_print, 0, STRING("gor_02_034_03"), 0, STRING("me"))
        ELSE
            CALL_CHILD(kuragarisan_write_name)
        END_IF
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_02_034_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_02_034_01"), 0, STRING("me"))
        CASE_LESS(172)
        CALL(evt_msg_print, 0, STRING("gor_02_034_02"), 0, STRING("me"))
        CASE_LESS(176)
        CALL(evt_msg_print, 0, STRING("gor_02_034_03"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_034_04"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_02_034_05"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_02_034_06"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_02_034_07"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_02_034_08"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_02_034_09"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(401)
        CALL(evt_msg_print, 0, STRING("gor_02_034_10"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(luigi_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(56, 111)
        CALL(evt_npc_set_position, STRING("me"), -400, 0, 0)
        CALL(evt_npc_set_ry_lr, STRING("me"), 1)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(luigi_talk)
    CALL_CHILD(luigi_gor_first_talk)
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[368] = {
    (u32)"パワーアップ屋", 0x00080600, 0x00000000, (u32)&powerupya_init,
    0x00000000, (u32)&unk_evt_gor_0003f020, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"クリハカセ",
    0x00080600, 0x00000000, (u32)&kurihakase_init, 0x00000000,
    (u32)&kurihakase_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"情報屋完璧", 0x00080600,
    0x00000000, (u32)npc_init_evt, 0x00000000, (u32)&johoyaperfect_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"盗賊団親分", 0x00080600, 0x00000000,
    (u32)&oyabun_init, 0x00000000, (u32)&oyabun_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"盗賊団１", 0x00080600, 0x00000000, (u32)&tozokudan1_init,
    0x00000000, (u32)&tozokudan1_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"盗賊団２",
    0x00080600, 0x00000000, (u32)&tozokudan2_init, 0x00000000,
    (u32)&tozokudan2_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"盗賊団３", 0x00080600,
    0x00000000, (u32)&tozokudan3_init, 0x00000000, (u32)&tozokudan3_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"サンダース", 0x00080600, 0x00000000,
    (u32)&sanders_init, 0x00000000, (u32)&sanders_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"クリボー４", 0x00080600, 0x00000000, (u32)&kuribo4_init,
    (u32)&kuribo4_regl, (u32)&kuribo4_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"チューさん３",
    0x00080600, 0x00000000, (u32)&chusan3_init, 0x00000000,
    (u32)&chusan3_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ロテン３", 0x00080600,
    0x00000000, (u32)&roten3_init, 0x00000000, (u32)&roten3_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ボロドー１", 0x00080600, 0x00000000,
    (u32)&borodo1_init, (u32)&borodo1_regl, (u32)&borodo1_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"ボロドー２", 0x00080600, 0x00000000, (u32)&borodo2_init,
    0x00000000, (u32)&borodo2_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"クラガリさん",
    0x00080600, 0x00000000, (u32)&kuragarisan_init, 0x00000000,
    (u32)&kuragarisan_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ルイージ", 0x00080600,
    0x00000000, (u32)&luigi_init, 0x00000000, (u32)&luigi_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(evt_door_01_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_01_01)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_01)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01B"), LW(2), 0, 0)
    RETURN
EVT_END()

static char* door_data_01_npc[2] = {
    "パワーアップ屋", NULL,
};

static EVT_BEGIN(evt_door_01_open)
    SPAWN_EVENT(unk_evt_gor_0003d138)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL_CHILD(mapball_init)
    CALL(evt_item_delete_check, STRING("item_star1"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_star1"), LW(0), LW(1), LW(2))
        ADD(LW(1), -1000)
        CALL(evt_item_set_position, STRING("item_star1"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_item_delete_check, STRING("item_star2"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_star2"), LW(0), LW(1), LW(2))
        ADD(LW(1), -1000)
        CALL(evt_item_set_position, STRING("item_star2"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_item_delete_check, STRING("item_star3"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_star3"), LW(0), LW(1), LW(2))
        ADD(LW(1), -1000)
        CALL(evt_item_set_position, STRING("item_star3"), LW(0), LW(1), LW(2))
    END_IF
    SET_VALUE(GSWF(1250), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_01_close)
    SPAWN_EVENT(unk_evt_gor_0003d1a4)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    CALL(evt_item_delete_check, STRING("item_star1"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_star1"), LW(0), LW(1), LW(2))
        ADD(LW(1), 1000)
        CALL(evt_item_set_position, STRING("item_star1"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_item_delete_check, STRING("item_star2"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_star2"), LW(0), LW(1), LW(2))
        ADD(LW(1), 1000)
        CALL(evt_item_set_position, STRING("item_star2"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_item_delete_check, STRING("item_star3"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_star3"), LW(0), LW(1), LW(2))
        ADD(LW(1), 1000)
        CALL(evt_item_set_position, STRING("item_star3"), LW(0), LW(1), LW(2))
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 door_data_01[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_01_doa",
    (u32)"A_i01_doa02", (u32)"A_i01_doa01", (u32)"S_ie_01_soto", 0x00000000,
    (u32)"S_ie_01_naka", (u32)"A_ie_01C", (u32)&evt_door_01_00, (u32)&evt_door_01_01,
    (u32)&evt_wall_01, (u32)&door_data_01_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_01_open, (u32)&evt_door_01_close,
};

static EVT_BEGIN(evt_door_02_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_i02_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_02_01)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_i02_doa"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_02)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02B"), LW(2), 0, 0)
    RETURN
EVT_END()

static char* door_data_02_npc[2] = {
    "クリハカセ", NULL,
};


EVT_BEGIN(evt_door_02_open)
    SPAWN_EVENT(unk_evt_gor_0003d138)
    SWITCH(GSW(0))
        CASE_OR_EQUAL(18)
        CASE_OR_EQUAL(64)
        CASE_OR_EQUAL(120)
        CASE_OR_EQUAL(174)
        CASE_OR_EQUAL(221)
        CASE_OR_EQUAL(271)
        CASE_OR_EQUAL(347)
        CALL(evt_snd_envoff, 18432)
        CALL(evt_snd_env_lpf, 0, 800)
        CASE_END
        CASE_EQUAL(348)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_FRANKURI1"))
        CALL(evt_snd_envoff, 18432)
        CALL(evt_snd_env_lpf, 0, 800)
        CASE_DEFAULT
        IF_INT_NOT_EQUAL(GSW(0), 402)
            CALL(evt_snd_bgmoff, 18432)
            CALL(evt_snd_envoff, 18432)
            CALL(evt_snd_env_lpf, 0, 800)
        END_IF
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 8)
        SPAWN_EVENT(kurihakase_first_talk)
    END_IF
    CALL(evt_mapobj_rotate, 1, STRING("S_open_book_r"), 0, 0, 180)
    SWITCH(GSW(0))
        CASE_BETWEEN(56, 70)
        CALL(evt_mapobj_trans, 1, STRING("S_open_book"), -15, 0, 0)
        CASE_BETWEEN(164, 177)
        CALL(evt_mapobj_trans, 1, STRING("S_open_book"), -15, 0, 0)
        CASE_BETWEEN(339, 351)
        CALL(evt_mapobj_trans, 1, STRING("S_open_book"), -15, 0, 0)
    END_SWITCH
    IF_INT_NOT_EQUAL(GSW(0), 18)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tizu_on"), 1)
    END_IF
    CALL(monosiri_check, LW(0), LW(1))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i02_memo"), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_door_02_close)
    SPAWN_EVENT(unk_evt_gor_0003d1a4)
    SWITCH(GSW(0))
        CASE_OR_EQUAL(347)
        CASE_OR_EQUAL(348)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
        CALL(evt_snd_envon, 160, 0)
        CALL(evt_snd_env_lpf, 0, 0)
        CASE_END
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 160, 0)
        CALL(evt_snd_envon, 160, 0)
        CALL(evt_snd_env_lpf, 0, 0)
    END_SWITCH
    IF_INT_EQUAL(GSWF(1220), 1)
        SET_VALUE(GSWF(1220), 0)
    END_IF
    IF_INT_EQUAL(GSW(0), 9)
        SPAWN_EVENT(kurihakase_wall_remove)
        GOTO(100)
    END_IF
    IF_INT_EQUAL(GSW(0), 19)
        SPAWN_EVENT(kurihakase_lecture_badge)
        GOTO(100)
    END_IF
    LABEL(100)
    WAIT_MS(500)
    CALL(evt_door_param, LW(0), 5, LW(1), LW(2), LW(3))
    CALL(evt_mario_party_door_move, LW(1), LW(3), FLOAT(135.0f))
    RETURN
EVT_END()

// TODO: type
static u32 door_data_02[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_i02_doa",
    (u32)"A_ie_02_doa02", (u32)"A_ie_02_doa01", (u32)"S_ie_02_soto", 0x00000000,
    (u32)"S_ie_02_naka", (u32)"A_ie_02C", (u32)&evt_door_02_00, (u32)&evt_door_02_01,
    (u32)&evt_wall_02, (u32)&door_data_02_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_02_open, (u32)&evt_door_02_close,
};

EVT_BEGIN(evt_door_02_entry)
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 0)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LW(15), 8)
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_door_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(3), 0, PTR(evt_door_open), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_03_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doam"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_03_01)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doam"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_03)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03B"), LW(2), 0, 0)
    RETURN
EVT_END()

static char* door_data_03_npc[2] = {
    "サンダース", NULL,
};

static char* door_data_03_mobj[2] = {
    "pblk2", NULL,
};

static EVT_BEGIN(evt_door_03_close)
    SPAWN_EVENT(unk_evt_gor_0003d1a4)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    IF_INT_EQUAL(GSW(0), 225)
        IF_INT_EQUAL(GSWF(1217), 0)
            CALL(evt_door_end_wait)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            ADD(LW(0), 40)
            CALL(evt_party_move_pos2, 0, LW(0), LW(2), 60)
            CALL(evt_party_set_dir, 0, 270, -1)
            CALL(evt_mario_set_dir, 90, 300, 0)
            CALL(evt_msg_print_party, STRING("mac_4_058"))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_THINK1_2"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_set_pose, STRING("M_I_Z"))
            WAIT_MS(1000)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print_party, STRING("mac_4_059"))
            SET_VALUE(GSWF(1217), 1)
            CALL(evt_mario_key_onoff, 1)
        END_IF
    END_IF
    IF_INT_NOT_EQUAL(GSW(0), 227)
        RETURN
    END_IF
    CALL(evt_door_end_wait)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    CALL(evt_party_stop, 1)
    CALL(evt_party_get_pos, 1, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(2), 39)
        CALL(evt_party_set_pos, 1, -17, 0, 57)
        CALL(evt_party_move_pos2, 1, -26, 108, FLOAT(120.0f))
    END_IF
    CALL(evt_party_set_dir, 1, 90, -1)
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_msg_print, 0, STRING("mac_4_087"), 0, STRING("extparty"))
    CALL(evt_party_set_dir, 1, 270, -1)
    CALL(evt_party_move_pos2, 1, -36, 140, FLOAT(120.0f))
    CALL(evt_party_move_pos2, 1, -165, 140, FLOAT(120.0f))
    CALL(evt_mario_clear_party, 1)
    SET_VALUE(GSW(0), 228)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_03_open)
    SPAWN_EVENT(unk_evt_gor_0003d138)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

// TODO: type
static u32 door_data_03[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_03_doam",
    (u32)"A_ie_03_doa_03", (u32)"A_ie_03_doa_01", (u32)"S_ie_03_soto", 0x00000000,
    (u32)"S_ie_03_naka", (u32)"A_ie_03C", (u32)&evt_door_03_00, (u32)&evt_door_03_01,
    (u32)&evt_wall_03, (u32)&door_data_03_npc, (u32)&door_data_03_mobj, 0x00000000,
    (u32)&evt_door_03_open, (u32)&evt_door_03_close,
};

EVT_BEGIN(evt_door_sanders_open)
    IF_INT_EQUAL(GSWF(1216), 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("gor_02_012"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    SPAWN_EVENT(evt_door_open)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_sanders_close)
    IF_INT_EQUAL(GSWF(1216), 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("gor_02_012"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    SPAWN_EVENT(evt_door_close)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_03_entry)
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
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_door_sanders_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(3), 0, PTR(evt_door_sanders_open), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_03_caseopen)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_hitobj_onoff, STRING("A_i03_rol"), 1, 0)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.01f), 1000)
    END_INLINE_EVENT
    CALL(evt_mario_wait_landon)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_MARIO_ROLL_DOWN1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_MARIO_ROLL_DOWN2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL_CHILD(evt_door_open_roll)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_03_roll_open)
    SPAWN_EVENT(unk_evt_gor_0003d138)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_i01_dan_mC"), LW(1), LW(2), LW(3))
        SUBTRACT(LW(2), 40)
        ADD(LW(3), 30)
        LOOP(3)
            CALL(evt_sub_random, 4, LW(4))
            SUBTRACT(LW(4), 2)
            ADD(LW(4), LW(1))
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 10, LW(4), LW(2), LW(3), FLOAT(1.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set_xyz, 65, 10, -55, 0, 150, 11)
    WAIT_MS(150)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_hitobj_onoff, STRING("A_i03_rol"), 1, 1)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
    CALL(evt_mario_mov_pos2, 65, -55, FLOAT(120.0f))
    IF_INT_EQUAL(GSWF(1216), 0)
        WAIT_MS(500)
        CALL(evt_mario_set_dir, 270, 0, 0)
        CALL(evt_mario_cancel_roll_motion)
    END_IF
    IF_INT_EQUAL(GSWF(1216), 0)
        CALL(evt_npc_set_ry, STRING("サンダース"), 90)
    END_IF
    IF_INT_EQUAL(GSWF(1216), 0)
        CALL(evt_party_set_dir, 0, 270, 0)
        CALL(evt_party_set_dispdir, 0, 0)
    END_IF
    IF_INT_EQUAL(GSWF(1216), 0)
        CALL(evt_msg_print, 0, STRING("mac_4_049"), 0, STRING("サンダース"))
    END_IF
    CALL(evt_cam_ctrl_onoff, 4, 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    SET_VALUE(LW(15), 0)
    CALL(evt_run_case_evt, 0, 1, LW(3), 0, PTR(evt_door_03_caseopen), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_03_roll_close)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_03_roll)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03B"), LW(2), 0, 0)
    RETURN
EVT_END()

// TODO: type
static u32 door_data_03_roll[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_03_doam",
    0x00000000, (u32)"A_i03_rol", (u32)"S_ie_03_soto", 0x00000000,
    (u32)"S_ie_03_naka", (u32)"A_ie_03C", (u32)&evt_door_03_00, (u32)&evt_door_03_01,
    (u32)&evt_wall_03_roll, (u32)&door_data_03_npc, (u32)&door_data_03_mobj, 0x00000000,
    (u32)&evt_door_03_roll_open, (u32)&evt_door_03_roll_close,
};

EVT_BEGIN(evt_door_03_roll_entry)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    SET_VALUE(LW(15), 0)
    CALL(evt_run_case_evt, 0, 1, LW(3), 0, PTR(evt_door_03_caseopen), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_01)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_doa"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_04)
    IF_INT_EQUAL(LW(6), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i04_msk"), 4096)
    END_IF
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    MULTIPLY_FLOAT(LW(3), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_B"), LW(2), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_ura_B"), LW(3), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_open)
    SPAWN_EVENT(unk_evt_gor_0003d138)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_close)
    SPAWN_EVENT(unk_evt_gor_0003d1a4)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

static char* door_data_04_npc[2] = {
    "盗賊団３", NULL,
};

static char* door_data_04_item[4] = {
    "item_star4", "item_star5", "item_star6", NULL,
};

// TODO: type
static u32 door_data_04[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_04_doa",
    (u32)"A_ie_04_doa02", (u32)"A_ie_04_doa01", (u32)"S_ie_04_soto", 0x00000000,
    (u32)"S_ie_04_naka", (u32)"A_ie_04C", (u32)&evt_door_04_00, (u32)&evt_door_04_01,
    (u32)&evt_wall_04, (u32)&door_data_04_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_04_open, (u32)&evt_door_04_close,
};

EVT_BEGIN(evt_door_04_junbi)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("gor_02_035"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_04_00_ura)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_doa_02"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_04_01_ura)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_doa_02"), LW(0), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_04_ura)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    MULTIPLY_FLOAT(LW(3), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_ura_B"), LW(3), 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door_data_04_ura[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_04_doa_02",
    (u32)"A_ie_04_doa_02_01", (u32)"A_ie_04_doa_02_02", (u32)"S_ie_04_soto", 0x00000000,
    (u32)"S_ie_04_naka", (u32)"A_ie_04C", (u32)&evt_door_04_00_ura, (u32)&evt_door_04_01_ura,
    (u32)&evt_wall_04_ura, (u32)&door_data_04_npc, 0x00000000, (u32)&door_data_04_item,
    (u32)&evt_door_04_open, (u32)&evt_door_04_close,
};

EVT_BEGIN(evt_door_05_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_05_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_05)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_05sB"), LW(2), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_05uB"), LW(3), 0, 0)
    RETURN
EVT_END()

char* door_data_05_npc[4] = {
    "盗賊団親分", "盗賊団１", "盗賊団２", NULL,
};

char* door_data_05_npc_01[3] = {
    "盗賊団親分", "盗賊団１", NULL,
};

EVT_BEGIN(evt_door_05_open)
    SPAWN_EVENT(unk_evt_gor_0003d138)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_05_close)
    SPAWN_EVENT(unk_evt_gor_0003d1a4)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door_data_05[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_05_doa",
    (u32)"A_ie_05_doa02", (u32)"A_ie_05_doa01", (u32)"S_ie_05_soto", 0x00000000,
    (u32)"S_ie_05_naka", (u32)"A_ie_05C", (u32)&evt_door_05_00, 0x00000000,
    (u32)&evt_wall_05, (u32)&door_data_05_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_05_open, (u32)&evt_door_05_close,
};

EVT_BEGIN(evt_door_06_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_06_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_06_00)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
        ELSE
            WAIT_MS(200)
        END_IF
        CALL(evt_mapobj_get_position, STRING("S_i06mB"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        CALL(evt_sub_intpl_msec_init, 2, 0, -180, 500)
    ELSE
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, -180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_i06mB"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_06_01)
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_mapobj_get_position, STRING("S_i06nB"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, 0, -180, 500)
    ELSE
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, -180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_i06nB"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_06_02)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(200)
        ELSE
        END_IF
        CALL(evt_mapobj_get_position, STRING("S_i06oB"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, 0, -180, 500)
    ELSE
        CALL(evt_sub_intpl_msec_init, 2, -180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_i06oB"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_06)
    IF_INT_EQUAL(LW(6), 0)
        SPAWN_EVENT(evt_wall_06_00)
        SPAWN_EVENT(evt_wall_06_01)
        SPAWN_EVENT(evt_wall_06_02)
    END_IF
    RETURN
EVT_END()

char* door_data_06_npc[2] = {
    "盗賊団２", NULL,
};

// TODO: type
u32 door_data_06_npc_01[1] = {
    0x00000000,
};

EVT_BEGIN(evt_door_06_open)
    SPAWN_EVENT(unk_evt_gor_0003d138)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_06_close)
    SPAWN_EVENT(unk_evt_gor_0003d1a4)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door_data_06[18] = {
    0x00000002, 0x00000003, 0x00000001, (u32)"S_ie_06_doa",
    (u32)"A_ie_06_doa02", (u32)"A_ie_06_doa01", (u32)"S_ie_06_soto", 0x00000000,
    (u32)"S_ie_06_naka", (u32)"A_ie_06C", (u32)&evt_door_06_00, 0x00000000,
    (u32)&evt_wall_06, (u32)&door_data_06_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_06_open, (u32)&evt_door_06_close,
};

EVT_BEGIN(evt_gor_door_open_sanders)
    CALL(evt_mapobj_get_position, STRING("S_ie_03_doao"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 2, 0, 100, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doao"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 2, 100, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doao"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("S_ie_03_doao"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_gor_door_open_sub)
    CALL_CHILD(evt_gor_door_open_sanders)
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 2, 0, 90, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doao"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("S_ie_03_doao"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_DOWN1"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()





EVT_BEGIN(evt_gor_door_dark_sub)
    CALL(evt_map_set_tevcallback, 1, PTR(_door_color_tev))
    CALL(evt_map_set_tevcallback, 2, PTR(_door_color2_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_del_mC"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_kmn_mC"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_yuka_mD"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_doam"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_sikiri_mC"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_del_mC"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_kmn_mC"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_yuka_mD"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_doam"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_sikiri_mC"), 256)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 300)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_npc_set_color, STRING("サンダース"), 255, 255, 255, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 255, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(_door_color_main_func, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_del_mC"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_kmn_mC"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_yuka_mD"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_doam"), 1)
    CALL(evt_sub_intpl_msec_init, 1, 0, -90, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_i03_sikiri_mC"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 0, 255, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(_door_color2_main_func, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_sikiri_mC"), 1)
    RETURN
EVT_END()



EVT_BEGIN(evt_gor_door_open)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(2500)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    SPAWN_EVENT(evt_gor_door_open_sub)
    SPAWN_EVENT(evt_gor_door_dark_sub)
    CALL(evt_gor_door_in_info, LW(0), LW(1), LW(2))
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(135.0f))
    CALL(evt_party_move_beside_mario, 0)
    CALL(evt_party_move_beside_mario, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_gor_door_out_info, LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        ADD(LW(4), 2)
        CALL(evt_cam3d_evt_set_xyz, LW(0), LW(4), LW(2), 0, 150, 11)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_gor_door_out_info, LW(0), LW(1), LW(2))
    CALL(evt_mario_party_door_move, LW(0), LW(2), FLOAT(135.0f))
    WAIT_WHILE(GF(0))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_gor_door_close_sanders)
    CALL(evt_mapobj_get_position, STRING("S_ie_03_doao"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 2, 0, 100, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doao"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 2, 100, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doao"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("S_ie_03_doao"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_gor_door_close_sub)
    CALL(evt_sub_intpl_msec_init, 2, 90, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_doao"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL_CHILD(evt_gor_door_close_sanders)
    RETURN
EVT_END()

EVT_BEGIN(evt_gor_door_light_sub)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 300)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(_door_color2_main_func, LW(0), LW(0), LW(0), 255)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_sikiri_mC"), 1)
    CALL(evt_sub_intpl_msec_init, 1, -90, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_i03_sikiri_mC"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_del_mC"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_kmn_mC"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_yuka_mD"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_doam"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 300)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_npc_set_color, STRING("サンダース"), 255, 255, 255, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 0, 255, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(_door_color_main_func, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_del_mC"), 251658240)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_kmn_mC"), 251658240)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_yuka_mD"), 251658240)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_doam"), 251658240)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_sikiri_mC"), 251658240)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_del_mC"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_kmn_mC"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_yuka_mD"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_doam"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_sikiri_mC"), 256)
    RETURN
EVT_END()

EVT_BEGIN(evt_gor_door_close)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(2500)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    SPAWN_EVENT(evt_gor_door_close_sub)
    SPAWN_EVENT(evt_gor_door_light_sub)
    BEGIN_INLINE_EVENT
        CALL(evt_gor_door_in_info, LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        ADD(LW(4), 2)
        CALL(evt_cam3d_evt_set_xyz, LW(0), LW(4), LW(2), 0, 150, 11)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_gor_door_in_info, LW(0), LW(1), LW(2))
    CALL(evt_mario_party_door_move, LW(0), LW(2), FLOAT(135.0f))
    WAIT_WHILE(GF(0))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()


EVT_BEGIN(evt_gor_offscren_door_entry)
    CALL(_door_color_init_func)
    SET_VALUE(LW(4), STRING("A_ie_03_doa_02"))
    SET_VALUE(LW(6), STRING("A_ie_03_doa_04"))
    CALL(evt_run_case_evt, 9, 1, STRING("A_ie_03_doa_02"), 0, PTR(evt_gor_door_open), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_ie_03_doa_04"), 0, PTR(evt_gor_door_close), 0)
    RETURN
EVT_END()

static EVT_BEGIN(mario_kirimomi_event)
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    CALL(evt_snd_sfxon, STRING("SFX_STG0_MARIO_ROUND1"), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_dispflag_onoff, 1, 256)
    CALL(evt_mario_get_dispdir, LW(8))
    SET_VALUE(LW(9), 1800)
    ADD(LW(9), LW(8))
    CALL(evt_sub_intpl_msec_init, 11, LW(8), LW(9), 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mario_set_dispdir, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 0, 256)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    RETURN
EVT_END()

EVT_BEGIN(cam_yurasu_event)
    CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 100)
    RETURN
EVT_END()



EVT_BEGIN(steel_coin)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kmn"), 2)
    CALL(evt_npc_set_ry, STRING("盗賊"), 270)
    CALL(evt_npc_set_position, STRING("盗賊"), -300, 0, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊"), 1073741856)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 0)
        CALL(evt_party_set_pos, 0, -640, 0, 0)
        CALL(evt_party_move_pos2, 0, -565, 0, FLOAT(150.0f))
        CALL(evt_party_stop, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_mario_set_pos, -600, 0, 0)
        CALL(evt_mario_mov_pos2, -525, 0, FLOAT(150.0f))
    END_INLINE_EVENT
    SET_VALUE(GF(0), 1)
    SET_VALUE(GF(1), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("盗賊"), -620, 0, 0, FLOAT(300.0f), 0)
        CALL(evt_npc_set_position, STRING("盗賊"), 0, -1000, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("盗賊"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_BORODO_RUN1"), LW(0), LW(1), LW(2), LW(10))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("盗賊"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(10), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("盗賊"), LW(0), LW(1), LW(2))
            CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
            SUBTRACT(LW(0), 20)
            IF_INT_LESS_OR_EQUAL(LW(0), LW(10))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_BORODO_STEAL1"), LW(0), LW(1), LW(2), 0)
        SPAWN_EVENT(cam_yurasu_event)
        SPAWN_EVENT(mario_kirimomi_event)
        CALL(evt_msg_toge, 2, -600, 30, 0)
        CALL(evt_msg_print, 0, STRING("mac_0_062"), 0, 0)
        SET_VALUE(GF(1), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    WAIT_WHILE(GF(1))
    CALL(evt_mario_set_dir, 270, 200, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -550, 0, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -450, 103, 330, -525, 43, -6, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("mac_0_063"))
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(100)
    END_IF
    WAIT_MS(500)
    CALL(evt_party_set_dir, 0, 90, -1)
    CALL(evt_mario_set_dir, 270, 200, 0)
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("mac_0_064"))
    CALL(evt_mario_set_pose, STRING("M_I_N"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 200, 1, -1000)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_StatusOpen)
    WAIT_MS(400)
    CALL(evt_pouch_get_coin, LW(0))
    SET_VALUE(LW(1), LW(0))
    DIVIDE(LW(1), 2)
    SUBTRACT(LW(0), LW(1))
    CALL(evt_pouch_set_coin, LW(0))
    SET_VALUE(LW(2), LW(1))
    AND_RAW(LW(2), 255)
    SET_VALUE(GSW(2), LW(2))
    SET_VALUE(LW(2), LW(1))
    AND_RAW(LW(2), 65280)
    DIVIDE(LW(2), 256)
    SET_VALUE(GSW(3), LW(2))
    DEBUG_PUT_MSG(STRING("盗まれた金額の下位と上位"))
    DEBUG_PUT_REG(GSW(2))
    DEBUG_PUT_REG(GSW(3))
    WAIT_MS(2000)
    CALL(evt_StatusClose)
    CALL(evt_party_set_dir, 0, 270, -1)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_065"))
    LABEL(100)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_set_dir, 0, 90, -1)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 8)
    RETURN
EVT_END()

EVT_BEGIN(kurihakase_first_talk)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_02_naka"), 2)
    CALL(evt_npc_set_position, STRING("クリハカセ"), -115, 0, -225)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    CALL(evt_door_end_wait)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(500)
    CALL(evt_mario_dispflag_onoff, 1, 4)
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_party_move_pos2, 0, -190, -100, FLOAT(120.0f))
    CALL(evt_party_set_dir, 0, 90, 200)
    CALL(evt_msg_print_party, STRING("mac_0_066"))
    CALL(evt_cam3d_evt_set, -115, 65, -20, -115, 30, -217, 666, 11)
    WAIT_MS(666)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_FRANKURI1"))
    CALL(evt_snd_envoff, 1024)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_066_01"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 95, 82, -185, 42, -217, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -190, -215, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, -1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("クリハカセ"), -140, -225, 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -225, -210, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_067"), 0, STRING("クリハカセ"))
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_067_01"), 0, STRING("クリハカセ"))
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_067_02"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -170, 84, 29, -170, 40, -217, 200, 11)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_067_03"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -155, 72, -20, -155, 37, -217, 200, 11)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_067_04"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 59, -69, -140, 33, -217, 200, 11)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    WAIT_MS(700)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリハカセ"), 0, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_067_05"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 95, 82, -185, 42, -217, 500, 11)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_067_06"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_0_068"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_0_069"), 0, STRING("クリハカセ"))
    CALL(evt_mario_set_pose, STRING("M_I_G"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_PAUSE2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_V_2"))
    WAIT_MS(500)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("mac_0_069_01"), 0, STRING("クリハカセ"))
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_MARIO_FALL_CRUSH1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 500)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_D_8"))
    WAIT_MS(1500)
    WAIT_WHILE(GF(0))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_pose, STRING("M_U_3"))
        WAIT_MS(500)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_0_069_02"), 0, STRING("party"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_0_069_03"), 0, STRING("クリハカセ"))
    CALL(evt_party_move_pos2, 0, -185, -215, FLOAT(40.0f))
    CALL(evt_cam3d_evt_set, -185, 64, -21, -185, 30, -218, 300, 11)
    CALL(evt_msg_print_party, STRING("mac_0_070"))
    CALL(evt_cam3d_evt_set, -140, 64, -21, -140, 30, -218, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_071"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -250, 95, 122, -185, 45, -217, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_072"))
    CALL(evt_msg_print, 0, STRING("mac_0_073"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 64, -21, -140, 30, -218, 0, 11)
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_0_073_01"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -65, 95, 107, -185, 45, -217, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_074"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_0_075"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 64, -21, -185, 30, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_076"))
    CALL(evt_cam3d_evt_set, -140, 64, -21, -140, 30, -218, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_077"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 95, 82, -185, 42, -217, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_078"))
    CALL(evt_npc_get_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("クリハカセ"), LW(0), LW(1), LW(2), 225, 30)
    CALL(evt_msg_print, 0, STRING("mac_0_079"), 0, STRING("クリハカセ"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -170, -225, FLOAT(80.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_dispflg_onoff, 0, 16, 1)
        CALL(evt_party_move_pos2, 0, -190, -205, FLOAT(60.0f))
        CALL(evt_party_set_dir, 0, 90, -1)
        CALL(evt_party_dispflg_onoff, 0, 16, 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -160, 70, -19, -160, 40, -217, 500, 11)
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_C_3"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    ADD(LW(1), 20)
    ADD(LW(2), 10)
    CALL(evt_item_entry, STRING("item00"), 96, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_item_set_scale, STRING("item00"), FLOAT(0.55f))
    WAIT_MS(660)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_item_set_position, STRING("item00"), -140, 0, -220)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_080"), 0, STRING("クリハカセ"))
    CALL(evt_mario_set_pose, STRING("M_C_3"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    ADD(LW(1), 20)
    ADD(LW(2), 10)
    CALL(evt_item_set_position, STRING("item00"), LW(0), LW(1), LW(2))
    WAIT_MS(660)
    CALL(evt_item_set_position, STRING("item00"), 0, -1000, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_set, -155, 78, 30, -155, 40, -217, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_081"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_0_081_01"))
    CALL(evt_npc_get_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("クリハカセ"), LW(0), LW(1), LW(2), 225, 30)
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("クリハカセ"), 1073741856)
    CALL(evt_msg_print, 0, STRING("mac_0_082"), 0, STRING("クリハカセ"))
    CALL(evt_npc_get_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("クリハカセ"), 0, -1000, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_party_pos, 1, 11, LW(0), LW(1), LW(2))
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 1)
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 9)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()


EVT_BEGIN(kurihakase_wall_remove)
    CALL(evt_door_end_wait)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_cam3d_evt_set, -270, 100, 595, -270, 30, 201, 2000, 11)
    CALL(evt_party_get_pos, 1, LW(0), LW(1), LW(2))
    CALL(evt_mario_clear_party, 1)
    CALL(evt_npc_set_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 1073741856)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -265, 187, 0, FLOAT(180.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_0_083"), 0, STRING("クリハカセ"))
    WAIT_MS(500)
    CALL(evt_paper_entry, STRING("OFF_d_saku"))
    CALL(evt_offscreen_entry, STRING("ofs"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_saku"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_saku04"), STRING("ofs"))
    CALL(set_depthPlaneOffset, STRING("img"), FLOAT(-2.0f))
    CALL(evt_offscreen_get_boundingbox, STRING("ofs"), LW(0), LW(1), LW(2), LW(3))
    IF_INT_GREATER(LW(2), LW(3))
        SET_VALUE(LW(3), LW(2))
        MULTIPLY(LW(3), 48)
        DIVIDE(LW(3), 64)
    ELSE
        SET_VALUE(LW(3), LW(2))
        MULTIPLY(LW(3), 64)
        DIVIDE(LW(3), 48)
    END_IF
    CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("ofs"), 1, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("Z_1"))
    CALL(evt_img_onoff, STRING("img"), 1)
    CALL(evt_img_set_shadow, STRING("img"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_saku04"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_OFF_FENCE_OPEN1"), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon, STRING("SFX_OFF_FENCE_FALL1"), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_saku04"))
    CALL(evt_img_release, STRING("img"))
    CALL(evt_offscreen_delete, STRING("ofs"))
    CALL(evt_hitobj_onoff, STRING("A_saku_04"), 1, 0)
    CALL(evt_cam3d_evt_set, -210, 85, 595, -210, 15, 201, 1000, 11)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -220, 305, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_get_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("クリハカセ"), LW(0), LW(1), LW(2), 225, 30)
    CALL(evt_npc_jump_position_nohit, STRING("クリハカセ"), LW(0), LW(1), LW(2), 225, 30)
    CALL(evt_msg_print, 0, STRING("mac_0_084"), 0, STRING("クリハカセ"))
    CALL(evt_npc_flag_onoff, 0, STRING("クリハカセ"), 1073741856)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 10)
    SPAWN_EVENT(kurihakase_lecture)
    RETURN
EVT_END()

EVT_BEGIN(kurihakase_lecture)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SET_FLOAT_VALUE(LW(3), LW(0))
    SET_FLOAT_VALUE(LW(4), LW(1))
    SET_FLOAT_VALUE(LW(5), LW(2))
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(3), LW(5), FLOAT(-300.0f), FLOAT(250.0f), FLOAT(-200.0f), FLOAT(250.0f), LW(6))
        IF_INT_EQUAL(LW(6), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
        SET_FLOAT_VALUE(LW(3), LW(0))
        SET_FLOAT_VALUE(LW(5), LW(1))
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
        CALL(evt_mario_set_dir, 90, 200, 0)
        WAIT_MS(800)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_0_084_01"), 0, STRING("クリハカセ"))
    WAIT_WHILE(GF(0))
    CALL(evt_cam3d_evt_set, -245, 100, 639, -245, 40, 294, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -290, 280, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -270, 305, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_084_02"), 0, STRING("クリハカセ"))
    CALL(evt_msg_select, 0, STRING("mac_0_084_02_yesno"))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(50)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("mac_0_084_03"))
    CALL(evt_msg_select, 0, STRING("mac_0_084_03_yesno"))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(50)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("mac_0_084_04"))
    GOTO(100)
    LABEL(50)
    CALL(evt_msg_print_add, 0, STRING("mac_0_084_05"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 33408)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    CALL(evt_npc_flag_onoff, 0, STRING("クリスチーヌ"), 128)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_npc_jump_position_nohit, STRING("クリスチーヌ"), LW(0), LW(1), LW(2), 200, 20)
    CALL(evt_npc_flag_onoff, 0, STRING("クリスチーヌ"), 32768)
    CALL(evt_msg_print, 0, STRING("mac_0_084_06"), 0, STRING("クリスチーヌ"))
    CALL(evt_npc_set_battle_info, STRING("クリハカセ"), 3)
    CALL(evt_npc_battle_start, STRING("クリハカセ"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_npc_flag_onoff, 0, STRING("クリスチーヌ"), 32768)
    CALL(evt_mario_set_prev_party_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 0, -1000, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_084_07"), 0, STRING("クリハカセ"))
    LABEL(100)
    SET_VALUE(GSW(0), 11)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kurihakase_door)
    CALL(evt_mapobj_get_position, STRING("S_i02_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 104, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_i02_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 11, 104, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_i02_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("S_i02_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()


EVT_BEGIN(kurihakase_lecture_badge)
    CALL(evt_door_end_wait)
    SET_VALUE(GF(0), 1)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mario_set_pose, STRING("M_I_O"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_121_01"), 0, 0)
    WAIT_WHILE(GF(0))
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -290, -15, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -265, 0, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_02_naka"), 1)
    CALL(evt_npc_set_position, STRING("クリハカセ"), -215, 0, -93)
    CALL(evt_item_entry, STRING("item00"), 247, 0, -1000, 0, 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
            ADD(LW(1), 20)
            CALL(evt_item_set_position, STRING("item00"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 1073741856)
    SPAWN_EVENT(kurihakase_door)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -215, 0, 0, FLOAT(120.0f), 0)
    WAIT_MS(1000)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -235, 0, 0, FLOAT(120.0f), 0)
    WAIT_MS(500)
    DELETE_EVENT(LW(10))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_npc_move_position, STRING("クリハカセ"), -215, 0, 0, FLOAT(120.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_0_121_02"), 0, STRING("クリハカセ"))
    CALL(evt_msg_select, 0, STRING("mac_0_121_02_yesno"))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(10)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("mac_0_121_03"))
    CALL(evt_msg_select, 0, STRING("mac_0_121_02_yesno"))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(10)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("mac_0_121_05"))
    GOTO(20)
    LABEL(10)
    CALL(evt_msg_print_add, 0, STRING("mac_0_121_04"))
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_BITS_SET(LW(0), 4096)
            CALL(evt_msg_continue)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(lecture)
    CALL(evt_msg_print, 0, STRING("mac_0_121_05"), 0, STRING("クリハカセ"))
    LABEL(20)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -215, 0, 0, FLOAT(120.0f), 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL_CHILD(kurihakase_door)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_02_naka"), 1)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -215, -93, 0, FLOAT(120.0f), 0)
    WAIT_WHILE(GF(0))
    CALL(evt_npc_set_position, STRING("クリハカセ"), -140, 0, -225)
    CALL(evt_npc_flag_onoff, 0, STRING("クリハカセ"), 1073741856)
    SET_VALUE(GSW(0), 20)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(Mario_Party_position)
    CALL(evt_mario_set_pos, -190, 0, -215)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_pos, 0, -225, 0, -215)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_party_set_breed_pose, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_party_set_breed_pose, 0, 1)
    RETURN
EVT_END()

EVT_BEGIN(book_open)
    CALL(evt_mapobj_get_position, STRING("S_open_book_r"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_OPEN_BOOK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 360, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_open_book_r"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    RETURN
EVT_END()

EVT_BEGIN(book_close)
    CALL(evt_sub_intpl_msec_init, 11, 180, 360, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_open_book_r"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            CALL(evt_mapobj_get_position, STRING("S_open_book_r"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHUT_BOOK1"), LW(0), LW(1), LW(2), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    RETURN
EVT_END()

EVT_BEGIN(book_center_init)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_open_book"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_open_book"), -15, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_open_book_r"), 0, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(epigraphy_map)
    CALL(evt_pouch_get_max_ap, LW(0))
    CALL(evt_pouch_set_ap, LW(0))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_clear_party, 1)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    SPAWN_EVENT(Mario_Party_position)
    CALL(evt_cam3d_evt_set, -170, 75, 717, -170, 75, -8, 0, 11)
    CALL(evt_npc_entry, STRING("ピーチダミー"), STRING("c_peach"))
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_102"), 0, 0)
    WAIT_MS(2000)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(LW(0), PTR(door_data_02))
    SPAWN_EVENT(evt_door_open_quick)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(1000)
    CALL(evt_fade_entry, 1, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("mac_0_103"))
    CALL(evt_msg_print, 0, STRING("mac_0_103_01"), 0, STRING("クリハカセ"))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_103_02"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_103_03"), 0, STRING("クリハカセ"))
    CALL(evt_msg_select, 0, STRING("mac_0_103_03_yesno"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("mac_0_103_04"))
        CALL(evt_msg_select, 0, STRING("mac_0_103_03_yesno"))
        IF_INT_EQUAL(LW(0), 0)
            GOTO(10)
        END_IF
        CALL(evt_msg_print_add, 0, STRING("mac_0_103_06"))
        GOTO(20)
    END_IF
    LABEL(10)
    CALL(evt_msg_print_add, 0, STRING("mac_0_103_05"))
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(evt_party_move_pos2, 0, -220, -90, FLOAT(120.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_npc_move_position, STRING("クリハカセ"), -190, -210, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_move_position, STRING("クリハカセ"), -210, -90, 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -210, -80, FLOAT(120.0f))
    WAIT_FRAMES(1)
    CALL(evt_npc_set_battle_info, STRING("クリハカセ"), 4)
    CALL(evt_npc_battle_start, STRING("クリハカセ"))
    CALL(evt_npc_wait_battle_end)
    DELETE_EVENT(LW(10))
    DELETE_EVENT(LW(11))
    CALL(evt_npc_set_position, STRING("クリハカセ"), -140, 0, -225)
    CALL_CHILD(Mario_Party_position)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_104"), 0, STRING("クリハカセ"))
    LABEL(20)
    CALL(evt_msg_print_party, STRING("mac_0_105"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_0_106"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -250, 133, 93, -180, 43, -210, 0, 11)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("mac_0_107"))
    CALL(evt_msg_print, 0, STRING("mac_0_108"), 0, STRING("クリハカセ"))
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    WAIT_MS(200)
    CALL_CHILD(book_close)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_109"), 0, STRING("クリハカセ"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_110"), 0, STRING("クリハカセ"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(400)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_111"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_112"))
    CALL(evt_msg_print, 0, STRING("mac_0_113"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_0_114"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -150, 100, 82, -150, 47, -217, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 1073741856)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -150, -205, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tizu_on"), 1)
    CALL(evt_npc_get_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(evt_item_entry, STRING("item_map"), 96, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item_map"), 2)
    CALL(evt_item_set_scale, STRING("item_map"), FLOAT(0.55f))
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("クリハカセ"), LW(0), LW(1), LW(2))
            ADD(LW(2), 10)
            CALL(evt_item_set_position, STRING("item_map"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("クリハカセ"), -165, -215, 0, FLOAT(60.0f), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("クリハカセ"), 1073741856)
    WAIT_MS(500)
    DELETE_EVENT(LW(10))
    CALL(evt_mario_set_pose, STRING("M_C_3"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    ADD(LW(1), 20)
    ADD(LW(2), 10)
    CALL(evt_item_set_position, STRING("item_map"), LW(0), LW(1), LW(2))
    WAIT_MS(700)
    CALL(evt_item_set_position, STRING("item_map"), 0, -1000, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 10)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
    CALL(evt_cam3d_evt_set, -180, 100, 82, -180, 47, -217, 500, 11)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("mac_0_117"))
    CALL(evt_msg_print, 0, STRING("mac_0_118"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_0_119"))
    CALL(evt_mario_set_dir, 270, 200, 0)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_120"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    SET_VALUE(GSWF(1246), 1)
    SET_VALUE(GSW(0), 19)
    RETURN
EVT_END()

EVT_BEGIN(epigraphy_map_after_stage1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    SPAWN_EVENT(Mario_Party_position)
    CALL(evt_cam3d_evt_set, -170, 75, 717, -170, 75, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_1_004"), 0, 0)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(LW(0), PTR(door_data_02))
    SPAWN_EVENT(evt_door_open_quick)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_trans, 1, STRING("S_open_book"), -15, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_open_book_r"), 0, 0, 180)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(1000)
    CALL(evt_fade_entry, 1, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL_CHILD(book_close)
    CALL(evt_msg_print, 0, STRING("mac_1_005"), 0, STRING("クリハカセ"))
    WAIT_MS(333)
    CALL(evt_msg_print_party, STRING("mac_1_006"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_1_007"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_1_008"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_1_009"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_1_010"))
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("mac_1_010_01"), 0, STRING("クリハカセ"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 10)
    ADD(LW(2), 20)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
    CALL(evt_msg_print_party, STRING("mac_1_010_02"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_1_010_03"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    SET_VALUE(GSW(0), 65)
    RETURN
EVT_END()

EVT_BEGIN(epigraphy_map_after_stage2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    SPAWN_EVENT(Mario_Party_position)
    CALL(evt_cam3d_evt_set, -170, 75, 717, -170, 75, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_2_002_01"), 0, 0)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(LW(0), PTR(door_data_02))
    SPAWN_EVENT(evt_door_open_quick)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    CALL(evt_fade_entry, 1, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL_CHILD(book_close)
    CALL(evt_msg_print, 0, STRING("mac_2_002_02"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_2_002_03"))
    CALL(evt_cam3d_evt_set, -140, 70, 4, -140, 32, -218, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_2_002_04"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -208, 63, -40, -208, 32, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_2_002_05"))
    CALL(evt_cam3d_evt_set, -140, 63, -40, -140, 32, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_002_06"), 0, STRING("クリハカセ"))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED2"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_FALL_CRUSH1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_D_8"))
    CALL(evt_party_set_breed_pose, 0, 8)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 500)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_mario_set_pose, STRING("M_U_3"))
    CALL(evt_party_set_breed_pose, 0, 1)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -208, 70, 4, -208, 32, -218, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print_party, STRING("mac_2_002_07"))
    CALL(evt_cam3d_evt_set, -140, 63, -40, -140, 32, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_002"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_2_003"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_004"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -208, 70, 4, -208, 32, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_2_005"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_2_006"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_2_007"))
    CALL(evt_msg_print, 0, STRING("mac_2_008"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_2_009"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -208, 66, -20, -208, 33, -218, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("mac_2_010"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("mac_2_011"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    SET_VALUE(GSW(0), 121)
    RETURN
EVT_END()

EVT_BEGIN(epigraphy_map_after_stage3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    SPAWN_EVENT(Mario_Party_position)
    CALL(evt_cam3d_evt_set, -170, 75, 717, -170, 75, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_3_001_01"), 0, 0)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(LW(0), PTR(door_data_02))
    CALL_CHILD(evt_door_open_quick)
    WAIT_FRAMES(1)
    CALL_CHILD(book_center_init)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(1000)
    CALL(evt_fade_entry, 1, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_3_001_02"), 0, STRING("クリハカセ"))
    WAIT_MS(250)
    CALL(evt_msg_print_party, STRING("mac_3_001_03"))
    WAIT_MS(250)
    CALL(evt_msg_print, 0, STRING("mac_3_001_04"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_3_001_05"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(250)
    CALL(evt_msg_print_party, STRING("mac_3_001_06"))
    CALL(evt_msg_print, 0, STRING("mac_3_001_07"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -208, 70, 4, -208, 32, -218, 0, 11)
    WAIT_MS(250)
    CALL(evt_msg_print_party, STRING("mac_3_001_07"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 500, 11)
    WAIT_MS(750)
    CALL(evt_msg_print, 0, STRING("mac_3_002"), 0, STRING("クリハカセ"))
    WAIT_MS(250)
    CALL(evt_msg_print_party, STRING("mac_3_003"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(750)
    CALL(evt_msg_print, 0, STRING("mac_3_004"), 0, STRING("クリハカセ"))
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 500, 11)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("クリハカセ"), -145, -285, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -135, -225, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_mapobj_get_position, STRING("S_open_book"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_PUT_BOOK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_open_book"), 1)
    WAIT_MS(500)
    CALL_CHILD(book_open)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -145, -225, 0, FLOAT(60.0f), 0)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリハカセ"), 0, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_3_005"), 0, STRING("クリハカセ"))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(GSWF(1341), 0)
        CALL(evt_msg_print_party, STRING("mac_3_006"))
    ELSE
        CALL(evt_msg_print_party, STRING("mac_3_007"))
        CALL(evt_msg_print, 0, STRING("mac_3_009"), 0, STRING("クリハカセ"))
        WAIT_MS(1500)
        CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリハカセ"), 0, 0, 0, 0, 0, 0, 0, 60)
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("mac_3_010"), 0, STRING("クリハカセ"))
    END_IF
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    SET_VALUE(GSW(0), 175)
    RETURN
EVT_END()

EVT_BEGIN(epigraphy_map_after_stage4)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    SPAWN_EVENT(Mario_Party_position)
    CALL(evt_cam3d_evt_set, -170, 75, 717, -170, 75, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_4_001_01"), 0, 0)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(LW(0), PTR(door_data_02))
    SPAWN_EVENT(evt_door_open_quick)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(1000)
    CALL(evt_fade_entry, 1, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_4_001_02"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_4_001_03"))
    CALL(evt_msg_print, 0, STRING("mac_4_001_04"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_4_001_05"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_4_001_06"))
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_4_001_07"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_4_001_08"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -260, 125, 256, -180, 75, -54, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_4_001_09"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -208, 77, 28, -208, 35, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_4_001_10"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_4_001_11"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_4_001_12"))
    CALL(evt_msg_print, 0, STRING("mac_4_002"), 0, STRING("クリハカセ"))
    CALL(evt_msg_print_party, STRING("mac_4_002"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_4_003"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    SET_VALUE(GSW(0), 222)
    RETURN
EVT_END()

EVT_BEGIN(epigraphy_map_after_stage5)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    SPAWN_EVENT(Mario_Party_position)
    CALL(evt_cam3d_evt_set, -170, 75, 717, -170, 75, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_5_002"), 0, STRING("クリハカセ"))
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(LW(0), PTR(door_data_02))
    SPAWN_EVENT(evt_door_open_quick)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(1000)
    CALL(evt_fade_entry, 1, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL_CHILD(book_close)
    CALL(evt_msg_print_party, STRING("mac_5_002"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_003"), 0, STRING("クリハカセ"))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_5_004"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_005"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -208, 77, 28, -208, 35, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_5_006"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_007"), 0, STRING("クリハカセ"))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_5_008"))
    CALL(evt_mario_set_dir, 270, 270, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_009"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    SET_VALUE(GSW(0), 272)
    RETURN
EVT_END()

EVT_BEGIN(epigraphy_map_after_stage6)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    SPAWN_EVENT(Mario_Party_position)
    CALL(evt_cam3d_evt_set, -170, 75, 717, -170, 75, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_6_002_00"), 0, STRING("クリハカセ"))
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(LW(0), PTR(door_data_02))
    CALL_CHILD(evt_door_open_quick)
    WAIT_FRAMES(1)
    CALL_CHILD(book_center_init)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(1000)
    CALL(evt_fade_entry, 1, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_6_002"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -140, 60, -40, -140, 30, -218, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_6_003"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -208, 77, 28, -208, 35, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_6_004"))
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_6_005"), 0, STRING("クリハカセ"))
    CALL(evt_npc_move_position, STRING("クリハカセ"), -145, -285, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    WAIT_MS(200)
    CALL(evt_npc_move_position, STRING("クリハカセ"), -135, -225, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_mapobj_get_position, STRING("S_open_book"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_PUT_BOOK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_open_book"), 1)
    WAIT_MS(500)
    CALL_CHILD(book_open)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_6_005_01"), 0, STRING("クリハカセ"))
    CALL(evt_sub_save_playtime, GSW(100))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSWF(1220), 1)
    SET_VALUE(GSW(0), 348)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(kurihakase_after3minutes)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("mac_6_007"), 0, STRING("クリハカセ"))
    WAIT_MS(500)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print_party, STRING("mac_6_008"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_6_009"), 0, STRING("クリハカセ"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_cam3d_evt_set, LW(0), 95, 73, LW(0), 43, -217, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_6_010"))
    CALL(evt_cam3d_evt_set, -157, 95, 73, -147, 43, -217, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_6_011"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, -197, 121, 185, -197, 50, -217, 0, 11)
    CALL(evt_pouch_check_item, 11, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        WAIT_MS(500)
        CALL(evt_msg_print_party, STRING("mac_6_012"))
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("mac_6_013"), 0, STRING("クリハカセ"))
    ELSE
        WAIT_MS(500)
        CALL(evt_msg_print_party, STRING("mac_6_012_01"))
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("mac_6_013_01"), 0, STRING("クリハカセ"))
    END_IF
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_6_014"))
    CALL(evt_cam3d_evt_set, -157, 95, 73, -147, 43, -217, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_6_015"), 0, STRING("クリハカセ"))
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 350)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry, -190, 30, 325)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

// TODO: type
static u32 hitDmgRetPoints[52] = {
    (u32)"A_ground_01", 0x43858000, 0x00000000, 0x43150000,
    (u32)"dokan", 0xC3840000, 0x00000000, 0x43A30000,
    (u32)"A_ground_03", 0xC3840000, 0x00000000, 0x43A30000,
    (u32)"Ship_Change01", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"Ship_Change02", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"A_ground_06", 0x43858000, 0x00000000, 0x43150000,
    (u32)"A_ground_07", 0x43858000, 0x00000000, 0x43150000,
    (u32)"A_ground_08", 0x43858000, 0x00000000, 0x43150000,
    (u32)"A_ground_02", 0x437C0000, 0xC26C0000, 0xC3948000,
    (u32)"A_ground_09", 0x437C0000, 0xC26C0000, 0xC3948000,
    (u32)"A_ground_10", 0x437C0000, 0xC26C0000, 0xC3948000,
    (u32)"S_i04_kabe_01A1", 0x00000000, 0xC47A0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(gor_goki1_urouro)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_goki_01"), 0, 270, 0)
    LOOP(0)
        SET_VALUE(LW(0), LW(4))
        SET_VALUE(LW(1), LW(5))
        CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
        CALL(evt_npc_add_dirdist, LW(0), LW(1), LW(3), FLOAT(50.0f))
        CALL(evt_gor_villager_urouro_check_func, LW(0), LW(1), 0, 0, FLOAT(70.0f))
        CALL(evt_sub_get_dir, LW(4), LW(5), LW(0), LW(1), LW(3))
        IF_INT_LESS(LW(3), 180)
            SET_VALUE(LW(6), 270)
        ELSE
            SET_VALUE(LW(6), 90)
        END_IF
        CALL(evt_goki_sub_intpl_init, 13, LW(4), LW(0), LW(5), LW(1), 50)
        LOOP(50)
            CALL(evt_goki_sub_intpl_get_value)
            SET_VALUE(LW(2), LW(0))
            SET_VALUE(LW(3), LW(1))
            ADD(LW(2), -320)
            ADD(LW(3), -480)
            CALL(evt_mapobj_trans, 1, STRING("S_goki_01"), LW(2), 0, LW(3))
            CALL(evt_mapobj_rotate, 1, STRING("S_goki_01"), 0, LW(6), 0)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(4), LW(0))
        SET_VALUE(LW(5), LW(1))
        CALL(evt_sub_random, 3000, LW(2))
        ADD(LW(2), 500)
        WAIT_MS(LW(2))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(gor_goki2_urouro)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    LOOP(0)
        SET_VALUE(LW(0), LW(4))
        SET_VALUE(LW(1), LW(5))
        CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
        CALL(evt_npc_add_dirdist, LW(0), LW(1), LW(3), FLOAT(50.0f))
        CALL(evt_gor_villager_urouro_check_func, LW(0), LW(1), 0, 0, FLOAT(70.0f))
        CALL(evt_sub_get_dir, LW(4), LW(5), LW(0), LW(1), LW(3))
        IF_INT_GREATER(LW(3), 180)
            SET_VALUE(LW(6), 0)
        ELSE
            SET_VALUE(LW(6), 180)
        END_IF
        CALL(evt_goki_sub_intpl_init, 13, LW(4), LW(0), LW(5), LW(1), 50)
        LOOP(50)
            CALL(evt_goki_sub_intpl_get_value)
            SET_VALUE(LW(2), LW(0))
            SET_VALUE(LW(3), LW(1))
            ADD(LW(2), -400)
            ADD(LW(3), 290)
            CALL(evt_mapobj_trans, 1, STRING("S_goki_02"), LW(2), 0, LW(3))
            CALL(evt_mapobj_rotate, 1, STRING("S_goki_02"), 0, LW(6), 0)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(4), LW(0))
        SET_VALUE(LW(5), LW(1))
        CALL(evt_sub_random, 3000, LW(2))
        ADD(LW(2), 500)
        WAIT_MS(LW(2))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box01)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box01"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item01"), 254, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item01"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box01"))
    CALL(evt_item_get_item, STRING("item01"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: type
u32 stg0_list[1] = {
    0x00000008,
};

// TODO: type
u32 stg1_list[4] = {
    0x00000012, 0x00000014, 0x00000016, 0x00000017,
};

// TODO: type
u32 stg2_list[4] = {
    0x0000001D, 0x0000001E, 0x00000021, 0x00000022,
};

// TODO: type
u32 stg3_list[1] = {
    0x00000041,
};

// TODO: type
u32 stg5_list[2] = {
    0x0000005D, 0x00000063,
};

// TODO: type
u32 stg6_list[1] = {
    0x0000006B,
};

// TODO: type
u32 stg7_list[6] = {
    0x00000075, 0x00000076, 0x0000001C, 0x00000078,
    0x00000077, 0x00000079,
};

// TODO: type
u32 stg8_list[10] = {
    0x00000083, 0x00000084, 0x00000085, 0x00000086,
    0x00000087, 0x00000090, 0x00000091, 0x00000092,
    0x00000093, 0x00000096,
};

// TODO: type
u32 follow[24] = {
    0x00000017, (u32)&stg0_list, 0x00000001, 0x00000038,
    (u32)&stg1_list, 0x00000004, 0x00000070, (u32)&stg2_list,
    0x00000004, 0x000000A4, (u32)&stg3_list, 0x00000001,
    0x00000105, (u32)&stg5_list, 0x00000002, 0x00000153,
    (u32)&stg6_list, 0x00000001, 0x00000175, (u32)&stg7_list,
    0x00000006, 0x00000191, (u32)&stg8_list, 0x0000000A,
};


EVT_BEGIN(evt_monosiri)
    CALL(evt_mario_key_onoff, 0)
    CALL(monosiri_check, LW(0), LW(1))
    CALL(monosiri_on, LW(1), LW(2))
    CALL(evt_msg_print_insert, 0, STRING("monosiri_follow_00"), 0, 0, LW(2))
    CALL(evt_mario_key_onoff, 1)
    CALL(monosiri_check, LW(0), LW(1))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i02_memo"), 1)
        CALL(evt_exit_case_evt)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_monosiri_follow)
    CALL(monosiri_check, LW(0), LW(1))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_run_case_evt, 9, 1, STRING("A_i02_gomibako"), 0, PTR(evt_monosiri), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_cloud)
    SET_VALUE(GSWF(1221), 1)
    SET_VALUE(LW(0), PTR(door_data_04_ura))
    CALL_CHILD(evt_door_entry)
    RETURN
EVT_END()

EVT_BEGIN(iri_19_init)
    IF_INT_EQUAL(GSW(1420), 19)
        IF_INT_EQUAL(GSWF(5405), 1)
            CALL(evt_item_entry, STRING("wr"), 73, -65, 120, 196, 16, GSWF(5406), 0)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_npc_entry)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), -240, 0, -260)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073742880)
    CALL(evt_npc_status_onoff, 1, STRING("クリスチーヌ"), 2)
    CALL(evt_map_blend_set_flag, 1, STRING("クリスチーヌ"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("クリスチーヌ"), 4096)
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt_sub)
    WAIT_MS(2000)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("クリスチーヌ"), -170, -225, 0, 60, 0)
        CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
        WAIT_FRAMES(1)
        CALL(evt_npc_wait_pera, STRING("クリハカセ"))
        CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_T_1"))
        WAIT_MS(1500)
        CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_S_1"))
        WAIT_MS(500)
        CALL(evt_npc_set_anim, STRING("クリハカセ"), STRING("T_1"))
        WAIT_MS(1500)
        CALL(evt_npc_set_anim, STRING("クリハカセ"), STRING("S_1"))
        CALL(evt_npc_move_position, STRING("クリスチーヌ"), -240, -260, 0, 60, 0)
        WAIT_MS(4000)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
    CALL(evt_snd_envoff, 512)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_open_book"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tizu_on"), 1)
    CALL(evt_npc_set_position, STRING("クリハカセ"), -140, 0, -225)
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 32768)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(0), PTR(door_data_02))
    CALL_CHILD(evt_door_open_quick)
    CALL(evt_cam3d_evt_set, -185, 90, 78, -185, 40, -218, 0, 11)
    CALL(evt_npc_flag_onoff, 0, STRING("クリハカセ"), 32768)
    SPAWN_EVENT(epilogue_evt_sub)
    CALL(evt_fade_end_wait)
    CALL(evt_msg_print, 0, STRING("ep_36"), 0, 0)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff_f, 512, 3000)
    CALL(evt_snd_envoff_f, 512, 3000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_bero_mapchange, STRING("hei_00"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(dearu_event)
    IF_INT_EQUAL(GSWF(1186), 0)
        SET_VALUE(GSWF(1203), 1)
    ELSE
        SET_VALUE(GSWF(1203), 0)
    END_IF
    RETURN
EVT_END()


EVT_BEGIN(front_wall_keshi)
    LOOP(0)
        CALL(camera_front_check, LF(0))
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_gai_kabe_mae"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_gai_kabe_mae"), 1)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()




EVT_BEGIN(tobiori_check)
    CALL(get_camcurve_init)
    LOOP(0)
        LOOP(0)
            CALL(get_camcurve, LW(10))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_EQUAL(LW(10), 1)
                IF_INT_GREATER_OR_EQUAL(LW(0), 370)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DEBUG_PUT_MSG(STRING("カメラ移動開始"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_05_soto"), 1)
        CALL(camera_move_check)
        DEBUG_PUT_MSG(STRING("カメラ移動終了"))
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_05_soto"), 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gor_02_init_evt)
    SWITCH(GSW(0))
        CASE_OR_EQUAL(18)
        CASE_OR_EQUAL(64)
        CASE_OR_EQUAL(120)
        CASE_OR_EQUAL(174)
        CASE_OR_EQUAL(221)
        CASE_OR_EQUAL(271)
        CASE_OR_EQUAL(347)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_FRANKURI1"))
        CALL(evt_snd_envoff, 512)
        CASE_END
        CASE_DEFAULT
        IF_INT_NOT_EQUAL(GSW(0), 402)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR3"))
        END_IF
    END_SWITCH
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 268)
        SPAWN_EVENT(dearu_event)
    ELSE
        SET_VALUE(GSWF(1203), 0)
    END_IF
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_EQUAL(GSW(0), 7)
        CALL(evt_npc_entry, STRING("盗賊"), STRING("c_borodo_n"))
        CALL(evt_npc_set_tribe, STRING("盗賊"), STRING("盗賊"))
        CALL(evt_npc_set_position, STRING("盗賊"), 0, -1000, 0)
        CALL(evt_npc_set_ry, STRING("盗賊"), 90)
    END_IF
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("noname"), 1, 0)
    CALL(evt_map_playanim, STRING("ie_01_light"), 1, 0)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -220, 0, 50, STRING("S_yuka_A01"), 125, GSWF(5574))
    CALL(evt_mobj_kururing_floor, STRING("kururin2"), 500, 124, -75, STRING("S_i06_kabe_01A"), 125, GSWF(5578))
    CALL(evt_mobj_itembox, STRING("mobj_box01"), 175, -50, 350, 0, 0, PTR(mobj_box01), GSWF(1248))
    CALL(evt_mobj_powerupblk, STRING("pblk1"), -250, 288, -500, 0, GSWF(5525), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk2"), -30, 60, -215, 0, GSWF(5526), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk3"), 540, 60, -260, 0, GSWF(5529), 0)
    CALL(evt_item_entry, STRING("item_star1"), 125, -3, 40, -325, 16, GSWF(5575), 0)
    CALL(evt_item_entry, STRING("item_star2"), 125, 65, 150, -165, 16, GSWF(5576), 0)
    CALL(evt_item_entry, STRING("item_star3"), 125, 50, 0, -390, 16, GSWF(5577), 0)
    SET_VALUE(LW(0), PTR(door_data_02))
    CALL_CHILD(evt_door_02_entry)
    SET_VALUE(LW(0), PTR(door_data_03))
    CALL_CHILD(evt_door_03_entry)
    SET_VALUE(LW(0), PTR(door_data_03_roll))
    CALL_CHILD(evt_door_03_roll_entry)
    IF_INT_LESS(GSW(0), 23)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_04_naka"), 1)
        CALL(evt_hitobj_onoff, STRING("A_ie_04C"), 1, 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_ie_04_doa01"), 0, PTR(evt_door_04_junbi), 0)
    ELSE
        SET_VALUE(LW(0), PTR(door_data_04))
        CALL_CHILD(evt_door_entry)
    END_IF
    SET_VALUE(LW(0), PTR(door_data_05))
    CALL(evt_door_data_copy, LW(0), GW(11))
    SET_VALUE(LW(0), GW(11))
    IF_INT_NOT_EQUAL(GSW(1420), 1)
        CALL(evt_door_set_param, GW(11), 0, PTR(door_data_05_npc))
    ELSE
        CALL(evt_door_set_param, GW(11), 0, PTR(door_data_05_npc_01))
    END_IF
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
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_door_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 4)
        CALL(evt_run_case_evt, 9, 1, LW(3), 0, PTR(evt_door_open), 0)
    END_IF
    IF_INT_EQUAL(GSWF(1219), 0)
        CALL(evt_mobj_lock, STRING("mobj_lock00"), 49, 446, 18, 50, 0, PTR(evt_chk), PTR(evt_unlock), GSWF(1219))
    ELSE
        SET_VALUE(LW(0), PTR(door_data_06))
        CALL(evt_door_data_copy, LW(0), LW(1))
        SET_VALUE(LW(0), LW(1))
        IF_INT_EQUAL(GSWF(5375), 0)
            CALL(evt_door_set_param, LW(0), 0, PTR(door_data_06_npc))
        ELSE
            CALL(evt_door_set_param, LW(0), 0, PTR(door_data_06_npc_01))
        END_IF
        CALL_CHILD(evt_door_entry)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 10)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_saku04"), 1)
        CALL(evt_hitobj_onoff, STRING("A_saku_04"), 1, 0)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 7)
        IF_INT_LESS_OR_EQUAL(GSW(0), 10)
            CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
            CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
            CALL(evt_npc_set_position, STRING("クリスチーヌ"), 0, -1000, 0)
            CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 10)
        SPAWN_EVENT(kurihakase_lecture)
    END_IF
    SWITCH(GSW(0))
        CASE_EQUAL(18)
        SPAWN_EVENT(epigraphy_map)
        CASE_EQUAL(64)
        SPAWN_EVENT(epigraphy_map_after_stage1)
        CASE_EQUAL(120)
        SPAWN_EVENT(epigraphy_map_after_stage2)
        CASE_EQUAL(174)
        SPAWN_EVENT(epigraphy_map_after_stage3)
        CASE_EQUAL(221)
        SPAWN_EVENT(epigraphy_map_after_stage4)
        CASE_EQUAL(271)
        SPAWN_EVENT(epigraphy_map_after_stage5)
        CASE_EQUAL(347)
        SPAWN_EVENT(epigraphy_map_after_stage6)
    END_SWITCH
    SPAWN_EVENT(evt_gor_offscren_door_entry)
    SPAWN_EVENT(gor_goki1_urouro)
    SPAWN_EVENT(gor_goki2_urouro)
    SPAWN_EVENT(gor_irai_init)
    IF_INT_EQUAL(GSWF(1221), 0)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 0, STRING("S_i04_kabe_msk"), STRING("A_i04_kabe_msk"), PTR(evt_cloud))
        BEGIN_INLINE_EVENT
            LOOP(0)
                IF_INT_EQUAL(GSWF(1221), 1)
                    BREAK_LOOP
                END_IF
                CALL(evt_cloud_get_mode, LW(0))
                IF_INT_NOT_EQUAL(LW(0), -1)
                    IF_INT_EQUAL(LW(0), 0)
                        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i04_ura_01B"), 1)
                    ELSE
                        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i04_ura_01B"), 1)
                    END_IF
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    ELSE
        SET_VALUE(LW(0), PTR(door_data_04_ura))
        CALL_CHILD(evt_door_entry)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i04_kabe_msk"), 1)
        CALL(evt_hitobj_onoff, STRING("A_i04_kabe_msk"), 1, 0)
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_sanbashi_01"), STRING("Ship_Change01"))
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_sanbashi_02"), STRING("Ship_Change02"))
    SPAWN_EVENT(front_wall_keshi)
    SPAWN_EVENT(tobiori_check)
    SPAWN_EVENT(evt_sub_mail_event)
    CALL_CHILD(evt_monosiri_follow)
    SPAWN_EVENT(iri_19_init)
    IF_INT_EQUAL(GSW(0), 402)
        CALL(evt_mario_kill_party, 0)
        CALL_CHILD(epilogue_npc_entry)
        SPAWN_EVENT(epilogue_evt)
    END_IF
    RETURN
EVT_END()
void _door_color_tev(struct TevCallbackParam* param) {
    s32 stage = param->field_C;
    u8* door = (u8*)_door[0];

    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColor(GX_TEVREG0, *(GXColor*)door);
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->field_C++;
}
void _door_color2_tev(struct TevCallbackParam* param) {
    s32 stage = param->field_C;
    u8* door = (u8*)_door[0];

    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColor(GX_TEVREG0, *(GXColor*)(door + 4));
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->field_C++;
}
USER_FUNC(_door_color_init_func) {
    u8* door = _mapAlloc(8);

    _door[0] = (u32)door;
    ((u8*)_door[0])[0] = 0xFF;
    ((u8*)_door[0])[1] = 0xFF;
    ((u8*)_door[0])[2] = 0xFF;
    ((u8*)_door[0])[3] = 0xFF;
    ((u8*)_door[0])[4] = 0xFF;
    ((u8*)_door[0])[5] = 0xFF;
    ((u8*)_door[0])[6] = 0xFF;
    ((u8*)_door[0])[7] = 0xFF;
    return EVT_RETURN_DONE;
}
USER_FUNC(_door_color_main_func) {
    s32* args = event->args;
    u8* door = (u8*)_door[0];

    door[0] = evtGetValue(event, args[0]);
    door[1] = evtGetValue(event, args[1]);
    door[2] = evtGetValue(event, args[2]);
    door[3] = evtGetValue(event, args[3]);
    return EVT_RETURN_DONE;
}
USER_FUNC(_door_color2_main_func) {
    s32* args = event->args;
    u8* door = (u8*)_door[0];

    door[4] = evtGetValue(event, args[0]);
    door[5] = evtGetValue(event, args[1]);
    door[6] = evtGetValue(event, args[2]);
    door[7] = evtGetValue(event, args[3]);
    return EVT_RETURN_DONE;
}
static void alpha_tev(TevCallbackParam* param) {
    s32 stage = param->field_C;
    u8* ap = (u8*)_ap[0];

    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, *ap });
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->field_C++;
}
static USER_FUNC(alpha_init_func) {
    u8* p = _mapAlloc(1);

    _ap[0] = (u32)p;
    p[0] = 0xFF;
    return EVT_RETURN_DONE;
}
static USER_FUNC(alpha_set_func) {
    s32 value = evtGetValue(event, event->args[0]);

    *(u8*)_ap[0] = value;
    return EVT_RETURN_DONE;
}
static USER_FUNC(effect_stardust) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    f32 a = evtGetFloat(event, args[1]);
    f32 b = evtGetFloat(event, args[2]);
    f32 c = evtGetFloat(event, args[3]);
    f32 d = evtGetFloat(event, args[4]);
    f32 e = evtGetFloat(event, args[5]);
    s32 f = evtGetValue(event, args[6]);
    s32 g = evtGetValue(event, args[7]);

    effStardustEntry(type, f, g, a, b, c, d, e);
    return EVT_RETURN_DONE;
}
static USER_FUNC(wariai_enzan_1) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]) - 1.0f;
    f32 b = evtGetFloat(event, args[1]);
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);

    f32 num = d * (b - a);

    evtSetValue(event, args[4], (s32)((num + a * c) / b));
    return EVT_RETURN_DONE;
}
static USER_FUNC(wariai_enzan_1f) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]) - 1.0f;
    f32 b = evtGetFloat(event, args[1]);
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);

    f32 num = d * (b - a);

    evtSetFloat(event, args[4], (num + a * c) / b);
    return EVT_RETURN_DONE;
}

// The original intplGetValue prototype orders the float ranges before the integer step. That order is
// ABI-identical to include/system.h's declaration (r3, f1, f2, r4, r5) but makes MWCC materialise both
// float-constant addresses (lis/addi/lfs 0(rN)) and load the float args before the integer steps, as the
// retail bytes do; the cast selects it.
#ifdef MUST_MATCH
static inline f32 intpl_f(s32 mode, f32 start, f32 end, s32 step, s32 steps) {
    return ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(mode, start, end, step, steps);
}
#else
static inline f32 intpl_f(s32 mode, f32 start, f32 end, s32 step, s32 steps) {
    return intplGetValue(mode, step, steps, start, end);
}
#endif
static USER_FUNC(wariai_enzan_1f_intpl) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]) - 1.0f;
    f32 b = evtGetFloat(event, args[1]);
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);
    s32 out = args[4];
    f32 t = evtGetFloat(event, args[5]);
    f32 result;
    f32 rd;

    if (t >= 14.0f) {
        f32 u = t - 14.0f;
        if (u >= 14.0f) {
            evtSetFloat(event, out, c);
            return EVT_RETURN_DONE;
        }
        result = 1.0f - intpl_f((s32)u, 1.0f, 0.0f, (s32)(b - a), (s32)b);
    } else {
        result = intpl_f((s32)t, 0.0f, 1.0f, (s32)(b - a), (s32)b);
    }
    rd = result * d;
    evtSetFloat(event, out, rd + (1.0f - result) * c);
    return EVT_RETURN_DONE;
}
static USER_FUNC(wariai_enzan_0) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]) - 1.0f;
    f32 b = evtGetFloat(event, args[1]) - 1.0f;
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);

    f32 num = d * (b - a);

    evtSetValue(event, args[4], (s32)((num + a * c) / b));
    return EVT_RETURN_DONE;
}
static USER_FUNC(wariai_enzan_0f) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]) - 1.0f;
    f32 b = evtGetFloat(event, args[1]) - 1.0f;
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);

    f32 num = d * (b - a);

    evtSetFloat(event, args[4], (num + a * c) / b);
    return EVT_RETURN_DONE;
}
static USER_FUNC(enzan_cam_turn_keisan) {
    s32* args = event->args;
    f32 angle = (f32)(evtGetValue(event, args[0]) % 360);
    s32 arg1 = args[1];
    s32 arg2 = args[2];
    Vec p1;
    Vec p0;
    Mtx mtx;

    p1.x = (f32)evtGetValue(event, arg1);
    p1.z = (f32)evtGetValue(event, arg2);
    p1.y = 0.0f;
    p0.x = (f32)evtGetValue(event, args[3]);
    p0.z = (f32)evtGetValue(event, args[4]);
    p0.y = 0.0f;
    PSVECSubtract(&p1, &p0, &p1);
    PSMTXRotRad(mtx, 'Y', 0.017453292f * angle);
    PSMTXMultVec(mtx, &p1, &p1);
    PSVECAdd(&p1, &p0, &p1);
    evtSetValue(event, arg1, (s32)p1.x);
    evtSetValue(event, arg2, (s32)p1.z);
    return EVT_RETURN_DONE;
}
static USER_FUNC(enzan_cam_turn_keisan_f) {
    f32 ratio = evtGetFloat(event, event->args[0]) / 100.0f;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 px = -362.0f;
    f32 py = -62.0f * ratio + 193.0f;
    f32 pz = -354.0f * ratio + 686.0f;
    Vec pos = (Vec){px, py, pz};
    Vec base = (Vec){-362.0f, 37.0f, -200.0f};
    Mtx mtx;

    PSVECSubtract(&pos, &base, &pos);
    PSMTXRotRad(mtx, 'Y', 0.017453292f * (720.0f * ratio));
    PSMTXMultVec(mtx, &pos, &pos);
    PSVECAdd(&pos, &base, &pos);
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = pos;
    cam->storedAt = cam->cameraAt;
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0;
    return EVT_RETURN_DONE;
}
static USER_FUNC(enzan_set_i_f) {
    s32* args = event->args;
    s32 out = args[0];
    f32 value = evtGetFloat(event, args[1]);

    evtSetValue(event, out, (s32)value);
    return EVT_RETURN_DONE;
}

// TODO: type
static USER_FUNC(enzan_set_f_i) {
    s32* args = event->args;
    s32 out = args[0];

    evtSetFloat(event, out, (f32)evtGetValue(event, args[1]));
    return EVT_RETURN_DONE;
}
static USER_FUNC(enzan_lw_save) {
    s32 i;

    for (i = 0; i < 16; i++) {
        xlw[i] = event->lwData[i];
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(enzan_lw_load) {
    s32 i;

    for (i = 0; i < 16; i++) {
        event->lwData[i] = xlw[i];
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_set_bgtpl) {
    char name[0x48];
    const char* tpl = (const char*)evtGetValue(event, event->args[0]);

    if (tpl == NULL) {
        tpl = gp->currentMapName;
    }
    sprintf(name, "./b/%s.tpl", tpl);
    bgReEntry(name);
    return EVT_RETURN_DONE;
}
static USER_FUNC(bgScrollControll) {
    if (evtGetValue(event, event->args[0]) == 0) {
        bgTransOffsetOff();
    } else {
        bgTransOffsetOn();
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(bgm_start_wait) {
    return psndBGMStartCheck(1) != 0 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
static USER_FUNC(setmertn) {
    event->ownerNPC = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}
USER_FUNC(stonechangertn) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);

    if (strcmp(name, "S_i01_sui_ao") == 0) {
        mapGrpFlagOff("S_i01_sui_ao", 1);
        mapGrpFlagOn("S_i01_sui_aka", 1);
    } else if (strcmp(name, "S_i01_sui_aka") == 0) {
        mapGrpFlagOn("S_i01_sui_ao", 1);
        mapGrpFlagOff("S_i01_sui_aka", 1);
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(unk_gor_0000574c) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    evtSetFloat(event, args[0], *(f32*)mario->unk1AC);
    return EVT_RETURN_DONE;
}
USER_FUNC(status_gauge_force_off) {
    statusWinForceOff();
    return EVT_RETURN_DONE;
}
USER_FUNC(set_talk_memory_ry) {
    NpcEntry* npc = npcNameToPtr("サンダース");

    *(f32*)npc->field_148 = 270.0f;
    return EVT_RETURN_DONE;
}
USER_FUNC(iri_13_make_item_tbl) {
    s32 count = pouchCheckItem(0x6B);

    if (count > 0) {
        iri_13_item_tbl[0] = (u32)_mapAlloc((count + 1) * sizeof(s32));
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(iri_13_update_item_tbl) {
    s32* args = event->args;
    s32 count = pouchCheckItem(0x6B);
    s32 i;

    if (count > 0) {
        for (i = 0; i < count; i++) {
            ((s32*)iri_13_item_tbl[0])[i] = 0x6B;
        }
        ((s32*)iri_13_item_tbl[0])[i] = -1;
    }
    evtSetValue(event, args[0], iri_13_item_tbl[0]);
    evtSetValue(event, args[1], count);
    return EVT_RETURN_DONE;
}
USER_FUNC(iri_13_free_item_tbl) {
    _mapFree((void*)iri_13_item_tbl[0]);
    iri_13_item_tbl[0] = 0;
    return EVT_RETURN_DONE;
}
void kuragarisan_msgcb(s32 type, void* param) {
    void* p = *(void**)param;

    switch (type) {
    case 3:
        if (((u16*)p)[4] == 1) {
            NpcEntry* npc = npcNameToPtr("クラガリさん");

            npc->direction = 90.0f;
        }
        break;
    }
}
USER_FUNC(evt_gor_door_in_info) {
    s32* args = event->args;
    Vec in = { 0.0f, 0.0f, 0.0f };
    Vec out = { 0.0f, 0.0f, 0.0f };

    door_position_sub(3, "S_ie_03_doao", "A_ie_03_doa_02", "A_ie_03_doa_04", &in, &out, 0);
    evtSetValue(event, args[0], (s32)in.x);
    evtSetValue(event, args[1], (s32)in.y);
    evtSetValue(event, args[2], (s32)in.z);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_gor_door_out_info) {
    s32* args = event->args;
    Vec in = { 0.0f, 0.0f, 0.0f };
    Vec out = { 0.0f, 0.0f, 0.0f };

    door_position_sub(3, "S_ie_03_doao", "A_ie_03_doa_02", "A_ie_03_doa_04", &in, &out, 0);
    evtSetValue(event, args[0], (s32)out.x);
    evtSetValue(event, args[1], (s32)out.y);
    evtSetValue(event, args[2], (s32)out.z);
    return EVT_RETURN_DONE;
}
static USER_FUNC(evt_StatusOpen) {
    statusWinForceOpen();
    return EVT_RETURN_DONE;
}
static USER_FUNC(evt_StatusClose) {
    statusWinForceOff();
    return EVT_RETURN_DONE;
}
static USER_FUNC(set_depthPlaneOffset) {
    s32* args = event->args;
    u8* obj = (u8*)imgNameToPtr((const char*)evtGetValue(event, args[0]), gp->inBattle != 0);

    *(f32*)(obj + 0x11C) = evtGetFloat(event, args[1]);
    return EVT_RETURN_DONE;
}
USER_FUNC(lecture) {
    if (isFirstCall) {
        winLectureOn();
    }
    if (winLectureCheck() != 0) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(monosiri_check) {
    s32* args = event->args;
    s32 stage = evtGetValue(event, GSW(0));
    u32 i;
    s32 j;
    FollowPath* entry;

    for (i = 0, entry = (FollowPath*)follow; i < 8; i++, entry++) {
        if (stage < entry->unk0) {
            break;
        }
        for (j = 0; j < entry->count; j++) {
            if (swGet(entry->ptr[j] + 0x117A) == 0) {
                evtSetValue(event, args[0], 1);
                evtSetValue(event, args[1], entry->ptr[j]);
                return EVT_RETURN_DONE;
            }
        }
    }
    if (swGet(0x4BD) != 0 && swGet(0x1181) == 0) {
        evtSetValue(event, args[0], 1);
        evtSetValue(event, args[1], 7);
        return EVT_RETURN_DONE;
    }
    if (swGet(0x13DD) != 0 && swGet(0x1225) == 0) {
        evtSetValue(event, args[0], 1);
        evtSetValue(event, args[1], 0xAB);
        return EVT_RETURN_DONE;
    }
    if (swGet(0x8B2) != 0 && swGet(0x11C6) == 0) {
        evtSetValue(event, args[0], 1);
        evtSetValue(event, args[1], 0x4C);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, args[0], 0);
    evtSetValue(event, args[1], 0);
    return EVT_RETURN_DONE;
}
USER_FUNC(monosiri_on) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    TattleEntry* tattle = battleGetUnitMonosiriPtr(id);

    swSet(id + 0x117A);
    evtSetValue(event, args[1], (s32)msgSearch(tattle->enemyTag));
    return EVT_RETURN_DONE;
}
USER_FUNC(camera_front_check) {
    s32* args = event->args;
    f32 angle = reviseAngle((100.0f * camGetPtr(CAMERA_3D)->viewYaw + 5.0f) / 100.0f);

    if (angle >= 0.0f && angle < 90.0f) {
        evtSetValue(event, *args++, 1);
    }
    if (angle >= 90.0f && angle < 270.0f) {
        evtSetValue(event, *args++, 0);
    }
    if (angle >= 270.0f && angle < 360.0f) {
        evtSetValue(event, *args, 1);
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(get_camcurve_init) {
    char* p = _mapAlloc(0x100);

    camstring[0] = (u32)p;
    p[0] = 0;
    return EVT_RETURN_DONE;
}
USER_FUNC(get_camcurve) {
    s32* args = event->args;
    struct CamRoadWork* road = camRoadGetWorkPtr();
    s32 flag = 0;

    if (((s32*)road)[0xD8 / 4] != 0 && ((char**)road)[0xFC / 4] != NULL) {
        if (strcmp(((char**)road)[0xFC / 4], "|MarioSt_CameraRoad|C_ground_ok") == 0 &&
            strcmp((char*)camstring[0], "|MarioSt_CameraRoad|C_ue") == 0) {
            flag = 1;
        }
        strcpy((char*)camstring[0], ((char**)road)[0xFC / 4]);
    }
    evtSetValue(event, args[0], flag);
    return EVT_RETURN_DONE;
}
USER_FUNC(camera_move_check) {
    f32 angle;

    if (isFirstCall) {
        event->userdataf[0] = -1000.0f;
    }
    angle = reviseAngle((100.0f * camGetPtr(CAMERA_3D)->viewYaw + 5.0f) / 100.0f);
    if (event->userdataf[0] != angle) {
        event->userdataf[0] = angle;
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

// clang-format on

#include "hom_00.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
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
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "evt/evt_window.h"
#include "driver/envdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_mahojin.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "party/party_motion.h"
#include <dolphin/os.h>

#pragma pool_data off

#pragma force_active on
extern s32 hom_10_evt_resha_left_13_data_38E4[];
extern s32 hom_10_evt_resha_right_13_data_3858[];
extern s32 hom_10_evt_resha_start2_13_data_2E14[];
extern s32 hom_10_evt_resha_start_13_data_2A44[];
extern s32 hom_10_evt_resha_stop_13_data_3548[];
static smartEntry* ind_lp;
static void sun_tev(TevCallbackParam* param);
static USER_FUNC(test_glare);
s32 evt_train_door[13];
s32 evt_train_nidome[307];

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(n_2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("tobira_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("tobira_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("tobira_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("tobira_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[3] = {
    {"n_bero_1", 1, 8, 0x4E20, 100000, 0, 0, 45, NULL, -1, evt_train_door, "rsh_01_b", "s_bero", 1, 1, "n_1_door_close", "n_1_door_open"},
    {"n_bero_2", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_00", "s_bero", 5, 5, n_2_door_close, n_2_door_open},
    {NULL},
};

BeroEntry bero_data_nidome[2] = {
    {"n_bero_1", 1, 8, 0x4E20, 100000, 0, 0, 45, NULL, -1, evt_train_nidome, "rsh_01_b", "s_bero", 1, 1, "n_1_door_close", "n_1_door_open"},
    {NULL},
};

EVT_BEGIN(evt_carry_up_bridge)
    IF_INT_EQUAL(GSWF(3574), 0)
        CALL(evt_msg_print, 0, STRING("stg6_hom_04"), 0, STRING("me"))
        CALL(evt_cam3d_evt_set, -65, 33, 575, 213, 64, 102, 3000, 11)
        WAIT_MS(6000)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_MS(200)
    END_IF
    IF_INT_EQUAL(GSWF(3574), 0)
        SET_VALUE(GSWF(3574), 1)
        CALL(evt_msg_print, 0, STRING("stg6_hom_05"), 0, STRING("me"))
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_hom_10"), 0, STRING("me"))
    END_IF
    CALL(evt_msg_select, 0, STRING("stg6_hom_10_yn"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("stg6_hom_06"))
        CALL(evt_item_entry, STRING("a"), 24, 0, 0, 0, 17, -1, 0)
        CALL(evt_item_get_item, STRING("a"))
        CALL(evt_item_get_item_end_wait, STRING("a"))
        CALL(evt_msg_print, 0, STRING("stg6_hom_08"), 0, STRING("me"))
        SET_VALUE(GSW(0), 311)
    ELSE
        CALL(evt_msg_print_add, 0, STRING("stg6_hom_09"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(init_powan_toron_konari_papa_gurume)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(325)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    IF_INT_GREATER_OR_EQUAL(GSW(0), 310)
        CALL(evt_npc_set_position, STRING("トロン"), 0, -1000, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(talk_powan)
    CALL(evt_msg_print, 0, STRING("stg6_hom_00"), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(talk_toron)
    CALL(evt_msg_print, 0, STRING("stg6_hom_01"), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(talk_gurume)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_211"), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(talk_papa)
    CALL(evt_msg_print, 0, STRING("stg6_hom_02"), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(talk_konari)
    CALL(evt_msg_print, 0, STRING("stg6_hom_03"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(init_ekiin1)
    RETURN
EVT_END()

EVT_BEGIN(talk_ekiin1)
    SWITCH(GSW(0))
        CASE_LESS(311)
        CALL_CHILD(evt_carry_up_bridge)
        CASE_LESS(325)
        CALL(evt_msg_print, 0, STRING("stg6_hom_11"), 0, 0)
        CASE_LESS(326)
        CALL(evt_msg_print, 0, STRING("stg6_hom_12"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg6_hom_13"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_ekiin2)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(325)
        CALL(evt_npc_set_position, STRING("me"), -865, 80, 100)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_ekiin2)
    SWITCH(GSW(0))
        CASE_LESS(325)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03"), 0, 0)
        CASE_LESS(326)
        CALL(evt_msg_print, 0, STRING("stg6_eki_04"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg6_hom_16"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npc_data[207] = {
    (u32)"ポワン探偵", 0x40000600, 0x00000000, (u32)&init_powan_toron_konari_papa_gurume,
    0x00000000, (u32)&talk_powan, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"トロン",
    0x40000600, 0x00000000, (u32)&init_powan_toron_konari_papa_gurume, 0x00000000,
    (u32)&talk_toron, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"グルメキノピオ", 0x40000600,
    0x00000000, (u32)&init_powan_toron_konari_papa_gurume, 0x00000000, (u32)&talk_gurume,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ナリキンパパ", 0x40000600, 0x00000000,
    (u32)&init_powan_toron_konari_papa_gurume, 0x00000000, (u32)&talk_papa, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"コナリキン", 0x40000600, 0x00000000, (u32)&init_powan_toron_konari_papa_gurume,
    0x00000000, (u32)&talk_konari, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"駅員1",
    0x40000600, 0x00000000, (u32)&init_ekiin1, 0x00000000,
    (u32)&talk_ekiin1, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"駅員2", 0x40000600,
    0x00000000, (u32)&init_ekiin2, 0x00000000, (u32)&talk_ekiin2,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ランペル", 0x40000600, 0x00000000,
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
    0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(evt_bridge_down)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_cam3d_evt_set_xyz, -350, 15, 0, 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set_xyz, 100, 15, 0, 0, 5000, 0)
    WAIT_MS(5300)
    CALL(evt_eff_fukidashi, 1, STRING("a"), STRING("駅員1"), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("駅員1"), 90)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -65, 33, 575, 213, 64, 102, 2000, 11)
    WAIT_MS(2000)
    CALL(evt_map_playanim, STRING("hasi"), 0, 0)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG6_BRIDGE_MOVE1"), LW(10))
        CALL(evt_map_checkanim, STRING("hasi"), LW(0), LW(1))
        WAIT_MS(LW(1))
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_BRIDGE_MOVE2"), 0)
    END_INLINE_EVENT
    CALL(evt_map_checkanim, STRING("hasi"), LW(0), LW(1))
    MULTIPLY(LW(1), 2)
    DIVIDE(LW(1), 3)
    WAIT_MS(LW(1))
    CALL(evt_cam3d_evt_set, 275, 260, 1400, 275, 210, 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_map_checkanim, STRING("hasi"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.001f), 200)
    WAIT_MS(2000)
    CALL(evt_bero_mapchange, STRING("eki_00"), STRING("s_bero"))
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x00000018, 0xFFFFFFFF,
};

static EVT_BEGIN(evt_chk)
    CALL(evt_pouch_check_item, 24, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_lock)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(GSW(0), 312)
    RETURN
EVT_END()

EVT_BEGIN(evt_train_door)
    IF_INT_NOT_EQUAL(GSW(0), 325)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_train)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_4"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_5"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_6"), 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(10), LW(11), LW(12), LW(13), LW(14), LW(15))
    CALL(evt_cam3d_evt_set, LW(10), LW(11), LW(12), LW(13), LW(14), LW(15), 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos, -820, 30, 500)
        CALL(evt_mario_set_dir_npc, STRING("駅員2"))
    END_INLINE_EVENT
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL(evt_party_move_pos, 0, -810, 70, 300)
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL(evt_party_move_pos, 0, -810, 20, 500)
    CALL(evt_party_set_dir_npc, 0, STRING("駅員2"))
    CALL(evt_npc_status_onoff, 1, STRING("駅員2"), 2)
    CALL(evt_npc_move_position, STRING("駅員2"), -800, 80, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_reverse_ry, STRING("駅員2"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("駅員2"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_map_playanim, STRING("n_1_door_close"), 0, 0)
    CALL(evt_map_checkanim, STRING("n_1_door_close"), LW(0), LW(1))
    WAIT_MS(LW(1))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg6_eki_05"), 0, STRING("駅員2"))
    WAIT_MS(400)
    CALL(evt_cam3d_evt_set, 318, 80, 347, -134, 65, 35, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(1000)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_CROSSING_OPEN1"), -170, 30, 0, LW(10))
    CALL(evt_map_playanim, STRING("shadanki"), 0, 0)
    CALL(evt_map_checkanim, STRING("shadanki"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_snd_sfxoff, LW(10))
    SET_VALUE(GF(15), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("eki_03_S"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("group614"), 1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    SPAWN_EVENT(hom_10_evt_resha_start_13_data_2A44)
    WAIT_MS(4500)
    CALL(evt_cam3d_evt_set, 50, 125, 500, 50, 75, 0, 1500, 12)
    WAIT_MS(1500)
    WAIT_MS(4500)
    CALL(evt_npc_set_ry, STRING("駅員1"), 90)
    CALL(evt_cam3d_evt_set, -150, 25, 250, 273, 75, -10, 1500, 13)
    WAIT_MS(1500)
    WAIT_MS(4000)
    SET_VALUE(GSWF(3582), 1)
    SET_VALUE(GSWF(3577), 1)
    SET_VALUE(GSW(0), 326)
    WAIT_FRAMES(1)
    CALL(evt_bero_mapchange, STRING("rsh_02_c"), STRING("hom_hassya"))
    RETURN
EVT_END()

EVT_BEGIN(evt_train_nidome)
    IF_INT_EQUAL(GSWF(3578), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_4"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_5"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_6"), 1)
    ELSE
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_hitobj_onoff, STRING("n_bero_1"), 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_map_playanim, STRING("n_1_door_open"), 0, 0)
    CALL(evt_map_checkanim, STRING("n_1_door_open"), LW(0), LW(1))
    WAIT_MS(LW(1))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos, -820, 12, 500)
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL(evt_party_move_pos, 0, -800, 40, 300)
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL(evt_party_move_pos, 0, -800, 20, 500)
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_party_stop, 1)
    WAIT_MS(200)
    CALL(evt_party_move_pos, 1, -810, 40, 300)
    CALL(evt_party_stop, 1)
    WAIT_MS(200)
    CALL(evt_party_move_pos, 1, -810, 30, 500)
    CALL(evt_party_set_dir, 1, 270, 200)
    CALL(evt_npc_status_onoff, 1, STRING("駅員2"), 2)
    CALL(evt_npc_move_position, STRING("駅員2"), -800, 80, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_reverse_ry, STRING("駅員2"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("駅員2"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_map_playanim, STRING("n_1_door_close"), 0, 0)
    CALL(evt_map_checkanim, STRING("n_1_door_close"), LW(0), LW(1))
    WAIT_MS(LW(1))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg6_eki_05"), 0, STRING("駅員2"))
    WAIT_MS(400)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_CROSSING_OPEN1"), -170, 30, 0, LW(10))
    CALL(evt_map_playanim, STRING("shadanki"), 0, 0)
    CALL(evt_map_checkanim, STRING("shadanki"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_snd_sfxoff, LW(10))
    SET_VALUE(GF(15), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("eki_03_S"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("group614"), 1)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    IF_INT_EQUAL(GSWF(3578), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(6000)
            CALL(evt_npc_set_ry, STRING("駅員2"), 90)
        END_INLINE_EVENT
        SPAWN_EVENT(hom_10_evt_resha_start_13_data_2A44)
    ELSE
        BEGIN_INLINE_EVENT
            WAIT_MS(6000)
            CALL(evt_npc_set_ry, STRING("駅員2"), 270)
        END_INLINE_EVENT
        SPAWN_EVENT(hom_10_evt_resha_start2_13_data_2E14)
    END_IF
    WAIT_MS(10000)
    SET_VALUE(GSWF(3582), 1)
    SET_VALUE(GSWF(3577), 1)
    WAIT_FRAMES(1)
    SET_VALUE(GSWF(3579), 0)
    IF_INT_EQUAL(GSWF(3578), 0)
        SET_VALUE(GSW(870), 1)
    ELSE
        SET_VALUE(GSW(870), 3)
    END_IF
    CALL(evt_bero_mapchange, STRING("rsh_02_c"), STRING("nidome_start"))
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_hom_touchaku)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 275, 260, 1400, 275, 210, 0, 0, 11)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("トロン"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("グルメキノピオ"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ナリキンパパ"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("コナリキン"), 0, -1000, 0)
    CALL(evt_npc_set_ry, STRING("トロン"), 90)
    CALL(evt_npc_set_ry, STRING("ポワン探偵"), 90)
    CALL(evt_npc_set_position, STRING("駅員2"), -750, 80, 100)
    CALL(evt_seq_wait, 2)
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_telop_entry, 11)
    END_INLINE_EVENT
    CALL_CHILD(hom_10_evt_resha_stop_13_data_3548)
    WAIT_MS(2500)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_4"), 1)
    CALL(evt_cam3d_evt_set, 458, 97, 366, 78, 110, 189, 3000, 11)
    LOOP(0)
        IF_INT_EQUAL(GW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_4"), 1)
    CALL(evt_cam3d_evt_set, -810, 232, 628, -810, 137, 87, 0, 11)
    WAIT_MS(2000)
    CALL(evt_npc_flag_onoff, 1, STRING("ポワン探偵"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("トロン"), 32768)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), -815, 80, 15)
    CALL(evt_npc_set_position, STRING("トロン"), -800, 80, 20)
    CALL(evt_set_dir_to_target, STRING("駅員2"), STRING("ポワン探偵"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -40, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("tobira"), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_npc_flag_onoff, 0, STRING("ポワン探偵"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("トロン"), 32768)
    CALL(evt_npc_status_onoff, 1, STRING("駅員2"), 2)
    CALL(evt_npc_move_position, STRING("トロン"), -800, 100, 0, FLOAT(60.0f), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("トロン"), -780, 100, 0, FLOAT(60.0f), 1)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("ポワン探偵"), -815, 100, 0, FLOAT(60.0f), 1)
    CALL(evt_set_dir_to_target, STRING("ポワン探偵"), STRING("トロン"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, -40, 0, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("tobira"), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    LOOP(3)
        CALL(evt_npc_reverse_ry, STRING("トロン"))
        WAIT_MS(500)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg6_hom_01_01"), 0, STRING("トロン"))
    CALL(evt_npc_set_anim, STRING("駅員2"), STRING("G_1"))
    CALL(evt_npc_set_anim, STRING("ポワン探偵"), STRING("O_1"))
    CALL(evt_npc_get_position, STRING("トロン"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_LANPEL_CHANGE1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_npc_set_position, STRING("トロン"), 0, -1000, 0)
        CALL(evt_npc_set_position, STRING("ランペル"), LW(0), LW(1), LW(2))
    END_INLINE_EVENT
    ADD(LW(2), 20)
    LOOP(3)
        CALL(evt_eff, 0, STRING("kemuri_test"), 7, LW(0), LW(1), LW(2), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(200)
    END_LOOP
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -780, 194, 481, -780, 125, 87, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg6_hom_01_02"), 0, STRING("ランペル"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -780, 259, 776, -780, 137, 87, 500, 11)
    CALL(evt_npc_sound_data_set, STRING("ランペル"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, STRING("SFX_STG6_LANPEL_JUMP1"), EVTDAT_POINTER_MAX)
    CALL(evt_npc_flag_onoff, 1, STRING("ランペル"), 32)
    CALL(evt_npc_status_onoff, 1, STRING("ランペル"), 2)
    CALL(evt_npc_set_ry, STRING("ランペル"), 90)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("ランペル"), LW(0), LW(1), LW(2))
        ADD(LW(0), 60)
        CALL(evt_npc_jump_position_nohit, STRING("ランペル"), LW(0), LW(1), LW(2), 400, 40)
        IF_INT_GREATER(LW(0), -500)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_snd_bgm_scope, 0, 0)
    SET_VALUE(GSW(0), 310)
    CALL(evt_bero_mapchange, STRING("rsh_02_b"), STRING("hom_touchaku"))
    RETURN
EVT_END()

EVT_BEGIN(evt_hom_touchaku_nidome)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 406, 317, 596, -389, 137, -44, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL_CHILD(hom_10_evt_resha_stop_13_data_3548)
    WAIT_MS(2500)
    LOOP(0)
        IF_INT_EQUAL(GW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    SET_VALUE(GSWF(3579), 1)
    CALL(evt_bero_mapchange, STRING("rsh_02_b"), STRING("hom_touchaku"))
    RETURN
EVT_END()
s32 tokei(EventEntry* event, BOOL isFirstCall) {
    OSCalendarTime td;
    u32 total;

    OSTicksToCalendarTime(OSGetTime(), &td);
#ifdef MUST_MATCH
    // MUST_MATCH: retail folds hour*3600 (60*60) so it schedules the hour load ahead of min's; * 3600 puts min first.
    total = td.hour * 60 * 60 + td.min * 60 + td.sec;
#else
    total = td.hour * 3600 + td.min * 60 + td.sec;
#endif
    event->lwData[0] = (s32)(1024.0f * -(360.0f * ((f32)total / 3600.0f) / 12.0f)) - 0x0DB58580;
    event->lwData[1] = (s32)(1024.0f * -(360.0f * ((f32)total / 60.0f) / 60.0f)) - 0x0DB58580;
    return EVT_RETURN_DONE;
}

static s32 test_glare(EventEntry* event, BOOL isFirstCall) {
    MapObject* obj = mapGetMapObj("sun");

    if (isFirstCall) {
        *(u32*)obj |= 0x20000000;
        return 0;
    }
    if (*(u16*)((u8*)obj + 0xEC) == 0x3FF || *(u16*)((u8*)obj + 0xEE) == 0x3FF ||
        *(u16*)((u8*)obj + 0xF0) == 0x3FF || *(u16*)((u8*)obj + 0xF2) == 0x3FF) {
        envGlareFilter(0, 0, 0, 0, 0);
    }
    else if (*(u16*)((u8*)obj + 0xEC) == 0 && *(u16*)((u8*)obj + 0xEE) == 0 &&
             *(u16*)((u8*)obj + 0xF0) == 0 && *(u16*)((u8*)obj + 0xF2) == 0) {
        envGlareFilter(0, 0, 0, 0, 0);
    }
    else if (evtGetValue(event, 0xFAA2B58F) != 0) {
        envGlareFilter(0, 0, 0, 0, 0);
    }
    else {
#ifdef MUST_MATCH
        // MUST_MATCH: the volatile pointer holds the four halfword loads in the original's order; without it
        // the scheduler moves the y0 `subi` one slot ahead of the y1 load.
        volatile u16* p = (volatile u16*)((u8*)obj + 0xEC);
        envGlareFilter(1, (s16)(p[1] - 0x32), (s16)(p[0] - 0x32), (s16)(p[3] + 0x32), (s16)(p[2] + 0x32));
#else
        envGlareFilter(1, (s16)(*(u16*)((u8*)obj + 0xEE) - 0x32), (s16)(*(u16*)((u8*)obj + 0xEC) - 0x32),
                       (s16)(*(u16*)((u8*)obj + 0xF2) + 0x32), (s16)(*(u16*)((u8*)obj + 0xF0) + 0x32));
#endif
    }
    return 0;
}

// The outer-loop prologue hoists `(i / 4) * 32` and `i % 4`; MWCC's list scheduler and the
// callee-saved colouring it induces are sensitive to the front-end expression tree here.
static void sun_tev(TevCallbackParam* param) {
    s32 field_C = param->field_C;
    s32 field_10 = param->field_10;
    s32 field_14 = param->field_14;
    s32 field_18 = param->field_18;
    u8* buf;
    GXTexObj texObj;
    f32 mtx[2][3];
    s32 i, j;
    s32 row;

    if (ind_lp == NULL) {
        ind_lp = smartAlloc(GXGetTexBufferSize(0x40, 0x40, GX_TF_IA8, GX_FALSE, 0), 1);
    }
    buf = (u8*)ind_lp->address;
    for (i = 0; i < 0x40; i++) {
#ifdef MUST_MATCH
        // MUST_MATCH: reusing `q` as the row base (rather than `row = (i / 4) * 32;`) puts MWCC's
        // callee-saved GPRs in the target's order (`row`/`gp`/`0x4330`/`i%4` = r31/r30/r29/r28).
        // `i % 4` must stay an inline expression in the inner loop so it colours r28; the #else
        // form is the natural one and scores 97.94.
        s32 q = i / 4;
        q = q * 32;
        row = q;
#else
        row = (i / 4) * 32;
#endif
        for (j = 0; j < 0x40; j++) {
            f32 s = (f32)sin(31.41592f * j / 64.0f - gp->retraceCount / 8.0f);
            s32 off = row + (j / 4) * 512;
            off += ((i % 4) + (j % 4) * 4) * 2;
            buf[off] = (s32)(128.0f + 3.0f * s);
            buf[off + 1] = 0x80;
        }
    }
    DCFlushRange(buf, 0x2000);
    GXInitTexObj(&texObj, buf, 0x40, 0x40, GX_TF_IA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, field_10);
    mtx[0][0] = 0.5f;
    mtx[0][1] = 0.0f;
    mtx[0][2] = 0.0f;
    mtx[1][0] = 0.0f;
    mtx[1][1] = 0.5f;
    mtx[1][2] = 0.0f;
    GXSetIndTexMtx(GX_ITM_0, mtx, 1);
    GXSetIndTexOrder(field_18, GX_TEXCOORD0, field_10);
    GXSetIndTexCoordScale(field_18, GX_ITS_1, GX_ITS_1);
    GXSetTevIndWarp(GX_TEVSTAGE0, field_18, 1, 0, GX_ITM_0);
    param->field_C = field_C;
    param->field_14 = field_14;
    param->field_18 = field_18 + 1;
    param->field_10 = field_10 + 1;
}

EVT_BEGIN(hom_00_init_evt)
    SET_VALUE(GF(15), 0)
    CALL(evt_npc_setup, PTR(npc_data))
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_HOM1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI1"))
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -175, 140, -170, 0, 0)
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 15, -125, 140, -170, 0, 0)
    IF_INT_EQUAL(GSWF(3578), 0)
        CALL_CHILD(hom_10_evt_resha_right_13_data_3858)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_4"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_5"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_6"), 1)
    ELSE
        CALL_CHILD(hom_10_evt_resha_left_13_data_38E4)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 339)
        CALL(evt_bero_get_entername, LW(0))
        CALL(evt_sub_get_system_flag, LW(1))
        IF_STR_NOT_EQUAL(LW(0), STRING("n_bero_1"))
            IF_STR_NOT_EQUAL(LW(0), STRING("n_bero_2"))
                IF_BITS_CLEAR(LW(1), 1)
                    CALL(evt_map_playanim, STRING("shadanki"), 2, 0)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(evt_map_set_tevcallback, 2, PTR(sun_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sun"), 33554432)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sun"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sun"), 268435456)
    BEGIN_INLINE_EVENT
        CALL(test_glare)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(tokei)
            CALL(evt_mapobj_rotate, 1, STRING("short"), 0, LW(0), 0)
            CALL(evt_mapobj_rotate, 0, STRING("long"), 0, LW(1), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    IF_INT_EQUAL(GSWF(3576), 1)
        SPAWN_EVENT(evt_bridge_down)
        RETURN
    END_IF
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_LESS(GSW(0), 310)
        SPAWN_EVENT(evt_hom_touchaku)
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("nidome"))
        SPAWN_EVENT(evt_hom_touchaku_nidome)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 340)
        SET_VALUE(LW(0), STRING("n_bero_1"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("n_bero_1"), PTR(bero_data_nidome))
        SET_VALUE(LW(0), STRING("n_bero_1"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    IF_INT_LESS(GSW(0), 312)
        CALL(evt_mobj_lock, STRING("lock"), 24, -260, 90, -190, 0, PTR(evt_chk), PTR(evt_lock), GSWF(3575))
        SET_VALUE(LW(0), 1)
        CALL_CHILD(bero_case_switch_off)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 325)
        CALL(evt_map_playanim, STRING("hasi"), 2, 0)
    END_IF
    IF_INT_EQUAL(GSW(0), 325)
        CALL(evt_hitobj_attr_onoff, 0, 1, STRING("n_bero_1"), 8388608)
        CALL(evt_run_case_evt, 8, 1, STRING("n_bero_1"), 0, PTR(evt_train), 0)
        CALL(evt_map_playanim, STRING("n_1_door_open"), 2, 0)
    END_IF
    SPAWN_EVENT(evt_sub_mail_event)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("train_2"), 2)
    RETURN
EVT_END()

// Original defines this unused table (force_active emits it); its line number gives the $633 suffix.
static const s32 texmtx_tbl[10] = { 30, 33, 36, 39, 42, 45, 48, 51, 54, 57 };

// clang-format on

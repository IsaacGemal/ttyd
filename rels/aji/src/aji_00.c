#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_emergency[];

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_19", "e_bero", 5, 5,
     (s32)w_door_close, (s32)w_door_open},
    {"e_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_01", "w_bero_1", 5, 5,
     (s32)e_door_close, (s32)e_door_open},
    {NULL},
};

static EVT_BEGIN(first_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER2"))
    CALL(evt_snd_envoff, 512)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_set_position, STRING("手下１"), 225, 0, -25)
    CALL(evt_npc_set_position, STRING("手下２"), 265, 0, 25)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_cam3d_evt_set, -225, 65, 342, -225, 45, -8, 1000, 11)
    WAIT_MS(250)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_cam3d_evt_set, 225, 65, 342, 225, 45, -8, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg7_aji_00_00_02"), 0, STRING("手下１"))
    CALL(evt_npc_move_position, STRING("手下１"), 200, -25, 0, FLOAT(80.0f), 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_00_00_03"), 0, STRING("手下２"))
    CALL(evt_npc_move_position, STRING("手下２"), 239, 25, 0, FLOAT(80.0f), 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_00_00_04"), 0, STRING("手下２"))
    CALL(evt_cam3d_evt_set, 175, 97, 542, 175, 71, -8, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 2000, 11)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("手下２"), -160, 0, 0, FLOAT(200.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("手下１"), -200, 0, 0, FLOAT(200.0f), 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_npc_set_battle_info, STRING("手下１"), 6)
    CALL(evt_npc_battle_start, STRING("手下１"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_set_pos, -200, 0, 0)
    CALL(evt_party_set_pos, 0, -240, 0, 0)
    CALL(evt_npc_set_position, STRING("手下１"), -180, 0, 0)
    CALL(evt_npc_set_position, STRING("手下２"), -150, 0, -55)
    CALL(evt_npc_set_ry, STRING("手下１"), 90)
    CALL(evt_npc_set_ry, STRING("手下２"), 90)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("手下１"), 225, -25, 0, FLOAT(180.0f), 0)
        CALL(evt_npc_set_position, STRING("手下１"), 0, -1000, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("手下２"), 265, 25, 0, FLOAT(180.0f), 0)
        CALL(evt_npc_set_position, STRING("手下２"), 0, -1000, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg7_aji_00_00_06"), 0, 0)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, -220, 65, 342, -220, 45, -8, 800, 11)
    WAIT_MS(800)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg7_aji_00_00_07"))
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_msg_print_party, STRING("stg7_aji_00_00_08"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    SET_VALUE(GSW(0), 361)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI2"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_telop_entry, 16)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_msg)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_64"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"手下１", 0x40000600},
    {"手下２", 0x40000600},
    {NULL},
};

EVT_BEGIN(aji_00_init_evt)
    IF_INT_LESS(GSW(0), 361)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN1"))
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    END_IF
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI2"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_LESS(GSW(0), 361)
        SPAWN_EVENT(first_evt)
    END_IF
    CALL(evt_mobj_recovery_blk, STRING("r_blk"), 20, -100, 60, -200, 0, 0)
    CALL(evt_mobj_save_blk, STRING("s_blk"), 100, 60, -200, 0, 0)
    SPAWN_EVENT(evt_emergency)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_map_playanim, STRING("hosi_01"), 1, 0)
    CALL(evt_map_playanim, STRING("hosi_02"), 1, 0)
    CALL(evt_map_playanim, STRING("hosi_03"), 1, 0)
    SWITCH(GSW(0))
        CASE_BETWEEN(376, 402)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_run_case_evt, 9, 1, STRING("w_bero"), 0, PTR(evt_door_msg), 0)
    END_SWITCH
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

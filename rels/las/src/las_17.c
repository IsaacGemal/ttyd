#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "npc_event.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 box_syutugen[];

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"e_bero_1", 1, 1, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "las_10", "sw_bero_2", 5, 5, (s32)e_door_close, (s32)e_door_open},
    {NULL},
};

static EVT_BEGIN(box_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("key"), 44, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("key"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(switch_evt)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(1250)
    CALL(evt_cam3d_evt_set, 0, 100, 363, 0, 30, -30, 500, 11)
    WAIT_MS(1000)
    CALL(evt_mobj_set_position, STRING("box"), 0, -30, 0)
    CALL(evt_mobj_flag_onoff, 0, STRING("box"), 16448)
    CALL(evt_mobj_hit_onoff, STRING("box"), 1)
    SET_VALUE(LW(0), STRING("box"))
    CALL_CHILD(box_syutugen)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(enemy1_dead_evt)
    CALL_CHILD(enemy_common_dead_event)
    IF_INT_EQUAL(GSW(1123), 0)
        SET_VALUE(GSW(1123), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(enemy2_dead_evt)
    CALL_CHILD(enemy_common_dead_event)
    IF_INT_EQUAL(GSW(1123), 1)
        SET_VALUE(GSW(1123), 2)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(enemy3_dead_evt)
    CALL_CHILD(enemy_common_dead_event)
    IF_INT_EQUAL(GSW(1123), 2)
        SET_VALUE(GSW(1123), 3)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(enemy4_dead_evt)
    CALL_CHILD(enemy_common_dead_event)
    IF_INT_EQUAL(GSW(1123), 3)
        SET_VALUE(GSW(1123), 4)
        SET_VALUE(GSWF(4350), 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_npc_change_fbat_mode, 7)
        CALL(evt_npc_stop_for_event)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(1000)
        CALL(evt_cam3d_evt_set, 235, 90, 387, -147, 90, -8, 500, 11)
        WAIT_MS(1000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        WAIT_MS(500)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_SWITCH_BIKKURI_APPEAR1"), -225, 60, 0, 0)
        CALL(evt_eff, 0, STRING("kemuri_test"), 7, -205, 60, 20, 1, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(100)
        CALL(evt_mobj_floatswitch_blue, 0, STRING("switch"), -225, 60, 0, PTR(switch_evt), GSWF(4351))
        WAIT_MS(500)
        WAIT_MS(1000)
        CALL(evt_cam3d_evt_off, 500, 11)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_npc_change_fbat_mode, 1)
        CALL(evt_npc_start_for_event)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(hint_plate)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    CALL(evt_msg_print, 0, STRING("stg8_las_42"), 0, 0)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_start_for_event)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(las_17_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_LESS(GSW(1123), 4)
        SET_VALUE(GSW(1123), 0)
    END_IF
    CALL(evt_npc_flag_onoff, 0, STRING("クリボー"), 4)
    CALL(evt_npc_flag_onoff, 0, STRING("トゲクリボー"), 4)
    CALL(evt_npc_flag_onoff, 0, STRING("ノコノコ"), 4)
    CALL(evt_npc_flag_onoff, 0, STRING("シンノスケ"), 4)
    CALL(evt_npc_change_interrupt, STRING("クリボー"), 8, PTR(enemy1_dead_evt))
    CALL(evt_npc_change_interrupt, STRING("トゲクリボー"), 8, PTR(enemy2_dead_evt))
    CALL(evt_npc_change_interrupt, STRING("ノコノコ"), 8, PTR(enemy3_dead_evt))
    CALL(evt_npc_change_interrupt, STRING("シンノスケ"), 8, PTR(enemy4_dead_evt))
    IF_INT_EQUAL(GSW(1123), 4)
        CALL(evt_mobj_floatswitch_blue, 0, STRING("switch"), -225, 60, 0, PTR(switch_evt), GSWF(4351))
        CALL(evt_npc_check_delete, STRING("クリボー"))
        CALL(evt_npc_check_delete, STRING("トゲクリボー"))
        CALL(evt_npc_check_delete, STRING("ノコノコ"))
        CALL(evt_npc_check_delete, STRING("シンノスケ"))
    END_IF
    CALL(evt_mobj_itembox, STRING("box"), 0, 0, 0, 0, 0, PTR(box_evt), GSWF(4368))
    IF_INT_EQUAL(GSWF(4351), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("box"), 16384)
        CALL(evt_mobj_hit_onoff, STRING("box"), 0)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("A_pure"), 0, PTR(hint_plate), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

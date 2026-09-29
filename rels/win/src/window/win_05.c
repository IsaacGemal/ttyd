#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
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
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_win_door_close[];
extern s32 evt_win_door_open[];

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(win_05_door_open)
    SET_VALUE(LW(8), STRING("S_doa_e_l"))
    SET_VALUE(LW(9), STRING("S_doa_e_r"))
    SPAWN_EVENT(evt_win_door_open)
    RETURN
EVT_END()

EVT_BEGIN(win_05_door_close)
    SET_VALUE(LW(8), STRING("S_doa_e_l"))
    SET_VALUE(LW(9), STRING("S_doa_e_r"))
    SPAWN_EVENT(evt_win_door_close)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[30] = {
    (u32)"w_bero", 0x00010000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"win_04", (u32)"e_bero",
    0x00040004, (u32)&win_05_door_close, (u32)&win_05_door_open, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static EVT_BEGIN(item_get00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 259, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(cloud_toujou)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("クラウダ"), 104, 13, 110)
    CALL(evt_cam3d_evt_set, 89, 95, 242, 43, 52, 0, 0, 11)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 255)
    CALL(evt_npc_wait_pera, STRING("クラウダ"))
    CALL(evt_seq_wait, 2)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("クラウダ"), LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg2_win_e04_00_04"), 0, STRING("クラウダ"))
    WAIT_MS(1000)
    SET_VALUE(GSW(0), 77)
    CALL(evt_fade_set_mapchange_type, 0, 10, 100, 9, 100)
    CALL(evt_bero_mapchange, STRING("win_03"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(win_05_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), -45, 0, -75, 0, 0, PTR(item_get00), GSWF(2684))
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 64, 0, 118, STRING("S_yukaA"), 125, GSWF(5611))
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_WIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG2_WIN2"))
    IF_INT_EQUAL(GSW(0), 76)
        SPAWN_EVENT(cloud_toujou)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

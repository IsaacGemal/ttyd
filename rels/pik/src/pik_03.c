#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

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

EVT_BEGIN(bero_in)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pos, 0, 0, 660)
    CALL(evt_party_set_pos, 0, -10, 0, 670)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 35, 610, FLOAT(120.0f))
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_cam_letter_box_disable, 0)
        CALL_CHILD(bero_case_entry)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 10, 620, FLOAT(120.0f))
    END_INLINE_EVENT
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(bero_out)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_yoshi_ride, 0, 0)
    CALL(evt_hitobj_attr_onoff, 0, 1, STRING("s_bero"), 8388608)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 35, 610, FLOAT(120.0f))
        CALL(evt_mario_mov_pos2, -10, 670, FLOAT(120.0f))
        CALL(evt_bero_mapchange, STRING("pik_02"), STRING("h_bero"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_party_move_pos2, 0, 10, 620, FLOAT(120.0f))
        CALL(evt_party_move_pos2, 0, 0, 660, FLOAT(120.0f))
    END_INLINE_EVENT
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

// TODO: type
static u32 bero_data[30] = {
    (u32)"s_bero", 0x00010000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, (u32)&bero_in,
    0x00000008, (u32)&bero_out, (u32)"pik_02", (u32)"h_bero",
    0x00040004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static EVT_BEGIN(evt_daiza)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(GSW(0), 338)
    CALL(evt_npc_stop_for_event)
    SET_VALUE(LW(0), 6)
    SET_VALUE(LW(1), STRING("star"))
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_bgmoff, 513)
    CALL_CHILD(evt_sub_starstone)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(pik_03_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_PIK3"))
    CALL(evt_snd_set_rev_mode, 2)
    IF_INT_LESS(GSW(0), 338)
        CALL(evt_item_entry, STRING("star"), 119, 0, 45, -950, 17, -1, 0)
    END_IF
    IF_INT_LESS(GSW(0), 338)
        CALL(evt_run_case_evt, 9, 1, STRING("dai_a"), 0, PTR(evt_daiza), 0)
    END_IF
    CALL(evt_item_entry, STRING("item01"), 327, -305, 330, -990, 16, GSWF(3277), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -265, 290, 550, 0, GSWF(5564), 0)
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou"), STRING("a_hi1"))
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("next"))
        CALL(evt_mario_set_pos, -25, 9, -925)
        CALL(evt_party_set_pos, 0, 25, 9, -925)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_party_set_dir, 0, 270, 0)
        CALL(evt_cam_road_reset)
        CALL(evt_mario_key_onoff, 0)
        BEGIN_INLINE_EVENT
            CALL(evt_seq_wait, 2)
            CALL(evt_fade_end_wait)
            CALL(evt_mario_key_onoff, 1)
        END_INLINE_EVENT
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

#include "dou_00.h"
#include "driver/envdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
s32 totsunyu_event[46];

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

static EVT_BEGIN(bero_in_event)
    IF_INT_EQUAL(GSW(0), 242)
        CALL_CHILD(totsunyu_event)
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, (s32*)&bero_in_event, -1, NULL, "muj_05", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "dou_01", "w_bero", 1, 1},
    {NULL},
};

static EVT_BEGIN(back_light)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_map_blend_set_flag, 0, STRING("mario"), 2048)
    CALL(evt_map_blend_off, 0)
    RETURN
EVT_END()

EVT_BEGIN(totsunyu_event)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 255)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_dou_00"), 0, 0)
    SPAWN_EVENT(back_light)
    WAIT_MS(200)
    SET_VALUE(GSW(0), 243)
    CALL(evt_mario_key_onoff, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_telop_entry, 10)
    END_INLINE_EVENT
    RETURN
EVT_END()

void water_tev(s32 arg0, void* arg1) {
    envSetWater(arg0, arg1);
}

EVT_BEGIN(dou_00_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    WAIT_FRAMES(1)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 30, 85, -75, 0, 0)
    CALL(evt_item_entry, STRING("item_01"), 137, -245, 0, 85, 16, GSWF(2990), 0)
    CALL(evt_map_set_tevcallback, 1, PTR(water_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s00_suimen"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s00_suimen"), 256)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

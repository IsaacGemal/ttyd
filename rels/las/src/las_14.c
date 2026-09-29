#include "las_14.h"
#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(evt_cloud_main);
static void _mapclsofs_all(void);  // TODO: real signature
static void _mapsetofs_all(void);  // TODO: real signature
static void _get_boundingbox(void);  // TODO: real signature
static void _img_alloc_capture(void);  // TODO: real signature

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"w_bero_1", 1, 1, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_10", "se_bero_1", 5, 5, w_door_close, w_door_open},
    {NULL},
};

static EVT_BEGIN(box_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("key"), 41, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("key"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static char* capturemap[3] = {
    "S_kabe1", "S_ten", "",
};

// TODO: type
static u32 v_list[24] = {
    0xC2480000, 0x42C80000, 0x42480000, 0x42480000,
    0x42C80000, 0x42480000, 0x42480000, 0x00000000,
    0x42480000, 0xC2480000, 0x00000000, 0x42480000,
    0xC2480000, 0x42C80000, 0xC2480000, 0x42480000,
    0x42C80000, 0xC2480000, 0x42480000, 0x00000000,
    0xC2480000, 0xC2480000, 0x00000000, 0xC2480000,
};

// TODO: write evt_cloud_main (static, 0x5fc bytes) here:
// its asm is in build/G8MJ01/las/asm/las_14.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_cloud_main_evt)
    CALL(evt_cloud_main)
    RETURN
EVT_END()

static EVT_BEGIN(cloud_evt)
    CALL(evt_mobj_flag_onoff, 0, STRING("box"), 16384)
    CALL(evt_mobj_hit_onoff, STRING("box"), 1)
    CALL(evt_hitobj_onoff, STRING("A_kura"), 1, 0)
    SET_VALUE(GSWF(4344), 1)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    RETURN
EVT_END()

EVT_BEGIN(touch_evt)
    RETURN
EVT_END()

static EVT_BEGIN(hint_plate)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg8_las_39"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write evt_las_cloud_ent (global, 0x140 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_14.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_las_cloud_init (global, 0xb8 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_14.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(las_14_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hitobj_onoff, STRING("A_taka"), 1, 0)
    IF_INT_EQUAL(GSWF(4344), 0)
        CALL(evt_las_cloud_init)
        CALL(evt_las_cloud_ent, PTR(capturemap), STRING("A_kura"), PTR(cloud_evt))
        CALL(evt_run_case_evt, 9, 1, STRING("A_kura"), 0, PTR(touch_evt), 0)
    ELSE
        CALL(evt_hitobj_onoff, STRING("A_kura"), 1, 0)
    END_IF
    CALL(evt_mobj_itembox, STRING("box"), 0, 0, 0, 0, 0, PTR(box_evt), GSWF(4365))
    IF_INT_EQUAL(GSWF(4344), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("box"), 16384)
        CALL(evt_mobj_hit_onoff, STRING("box"), 0)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("A_pure"), 0, PTR(hint_plate), 0)
    RETURN
EVT_END()

// TODO: type
static u8 cloud_once_flag[1] = {
    0x00,
};

// TODO: type
static u32 cloud[1] = {
    0x00000000,
};

// TODO: write _mapclsofs_all (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_14.s; this is where the original has it, just before the first script that uses it

// TODO: write _mapsetofs_all (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_14.s; this is where the original has it, just before the first script that uses it

// TODO: write _get_boundingbox (static, 0x238 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_14.s; this is where the original has it, just before the first script that uses it

// TODO: write _img_alloc_capture (static, 0x50 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_14.s; this is where the original has it, just before the first script that uses it
// clang-format on

#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

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

static EVT_BEGIN(e_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("e_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("e_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("s_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("s_door"), 0, LW(0), 0)
    RETURN
EVT_END()

// TODO: type
static u32 bero_data[45] = {
    (u32)"e_bero", 0x00010003, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"eki_03", (u32)"w_bero_2",
    0x00050005, (u32)&e_close, (u32)&e_open, (u32)"s_bero",
    0x00010003, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"eki_05", (u32)"n_bero", 0x00050005,
    (u32)&s_close, (u32)&s_open, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

EVT_BEGIN(evt_jump)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_jump_jumpstand, 360, -900, 70, 300, 700, 75, 310, -900, 65, 6000, 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(eki_04_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_cam_road_reset)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI2"))
    CALL(evt_mobj_jumpstand_red, 0, STRING("mobj_jump"), 360, -900, 70, PTR(evt_jump), 0)
    CALL(evt_item_entry, STRING("item01"), 152, -250, -815, 70, 16, GSWF(3744), 0)
    CALL(evt_item_entry, STRING("item02"), 262, -250, 230, 40, 16, GSWF(3747), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("futi_"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("hasira"), 2)
    IF_INT_LESS(GSW(0), 316)
        SET_VALUE(GSW(0), 316)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

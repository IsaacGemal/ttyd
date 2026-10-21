#include "las_06.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include <stdio.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(make_name);

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {
        "w_bero",
        0x0001,
        0x0006,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        NULL,
        0xFFFFFFFF,
        NULL,
        "las_05",
        "e_bero_1",
        0x0005,
        0x0005,
        w_door_close,
        w_door_open,
    },
    {
        "e_bero",
        0x0001,
        0x0006,
        0x00004E20,
        0x000186A0,
        0x00000000,
        0x00000000,
        0xFFFFFFFF,
        NULL,
        0xFFFFFFFF,
        NULL,
        "las_07",
        "w_bero_1",
        0x0005,
        0x0005,
        e_door_close,
        e_door_open,
    },
    {
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
    },
};

static char name[8];

#include "rel/common/make_name.h"

static EVT_BEGIN(rousoku_init)
    SET_VALUE(LW(0), 1)
    LOOP(18)
        CALL(make_name, LW(0), LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
        CALL(evt_mapobj_get_position, LW(1), LW(2), LW(3), LW(4))
        SUBTRACT_FLOAT(LW(3), FLOAT(7.0f))
        CALL(evt_eff, 0, STRING("fire"), 3, LW(2), LW(3), LW(4), FLOAT(1.0f), 1000, 0, 0, 0, 0, 0, 0)
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(las_06_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_badgeblk, STRING("mobj00"), -550, 480, 60, 150, 0, GSWF(4372), 0)
    CALL(evt_npc_check, STRING("スーパーキラー大砲１"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_link, STRING("スーパーキラー大砲１"), STRING("スーパーキラー大砲２"))
    END_IF
    CALL(evt_npc_check, STRING("スーパーキラー大砲３"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_link, STRING("スーパーキラー大砲３"), STRING("スーパーキラー大砲４"))
    END_IF
    CALL_CHILD(rousoku_init)
    RETURN
EVT_END()

// clang-format on
#pragma force_active off

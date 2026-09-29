#include "las_13.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "eff/eff_small_star.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 box_syutugen[];
static USER_FUNC(small_star);

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
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
    {"e_bero_1", 1, 1, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_10", "sw_bero_1", 5, 5, e_door_close, e_door_open},
    {NULL},
};

static EVT_BEGIN(box_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("key"), 40, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("key"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static USER_FUNC(small_star) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    effSmallStarEntry(0, 4, x, y + 12.5f, z, 0.0f, -1.0f, 0.0f);
    return 2;
}

EVT_BEGIN(switch_check)
    SET_VALUE(LW(0), 0)
    IF_INT_EQUAL(LSW(1), 2)
        ADD(LW(0), 1)
    END_IF
    IF_INT_EQUAL(LSW(2), 3)
        ADD(LW(0), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        CALL(evt_mario_key_onoff, 0)
        WAIT_MS(300)
        CALL(evt_mobj_get_position, STRING("switch1"), LW(0), LW(1), LW(2))
        CALL(small_star, LW(0), LW(1), LW(2))
        CALL(evt_mobj_get_position, STRING("switch2"), LW(0), LW(1), LW(2))
        CALL(small_star, LW(0), LW(1), LW(2))
        CALL(evt_mobj_delete, STRING("switch1"))
        CALL(evt_mobj_delete, STRING("switch2"))
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        CALL(evt_cam3d_evt_set, 0, 100, 363, 0, 30, -30, 500, 11)
        WAIT_MS(1000)
        CALL(evt_mobj_set_position, STRING("box"), 0, -30, 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("box"), 16448)
        CALL(evt_mobj_hit_onoff, STRING("box"), 1)
        SET_VALUE(LW(0), STRING("box"))
        CALL_CHILD(box_syutugen)
        CALL(evt_cam3d_evt_off, 500, 11)
        CALL(evt_mario_key_onoff, 1)
        SET_VALUE(GSWF(4343), 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(switch1_evt)
    IF_INT_LESS(LSW(1), 255)
        ADD(LSW(1), 1)
    END_IF
    CALL_CHILD(switch_check)
    RETURN
EVT_END()

static EVT_BEGIN(switch2_evt)
    IF_INT_EQUAL(LSW(1), 0)
        IF_INT_LESS(LSW(2), 255)
            ADD(LSW(2), 1)
        END_IF
    END_IF
    CALL_CHILD(switch_check)
    RETURN
EVT_END()

static EVT_BEGIN(hint_plate)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg8_las_38"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(las_13_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_EQUAL(GSWF(4343), 0)
        SET_VALUE(LSW(1), 0)
        SET_VALUE(LSW(2), 0)
        CALL(evt_mobj_floatswitch_red, 0, STRING("switch1"), 150, 60, 0, PTR(switch1_evt), GSWF(4343))
        CALL(evt_mobj_floatswitch_red, 0, STRING("switch2"), -150, 60, 0, PTR(switch2_evt), GSWF(4343))
    END_IF
    CALL(evt_mobj_itembox, STRING("box"), 0, 0, 0, 0, 0, PTR(box_evt), GSWF(4364))
    IF_INT_EQUAL(GSWF(4343), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("box"), 16384)
        CALL(evt_mobj_hit_onoff, STRING("box"), 0)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("A_pure"), 0, PTR(hint_plate), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

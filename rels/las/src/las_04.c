#include "las_04.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

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

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_03", "e_bero_1", 5, 5, w_door_close, w_door_open},
    {"e_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_05", "w_bero", 5, 5, e_door_close, e_door_open},
    {NULL},
};

// TODO: write coin_entry (global, 0x124 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(firebar_coin)
    SWITCH(LW(2))
        CASE_EQUAL(0)
        SET_VALUE(LW(3), 0)
        SET_VALUE(LW(4), 360)
        CASE_LESS(0)
        SET_VALUE(LW(3), 90)
        SET_VALUE(LW(4), 270)
        CASE_GREATER(0)
        SET_VALUE(LW(3), 270)
        SET_VALUE(LW(4), 450)
    END_SWITCH
    SET_VALUE(LW(5), LW(4))
    SUBTRACT(LW(5), LW(3))
    DIVIDE(LW(5), 6)
    SET_VALUE(LW(6), LW(3))
    SET_VALUE(LW(7), LW(5))
    DIVIDE(LW(7), 2)
    ADD(LW(6), LW(7))
    LOOP(5)
        CALL(evt_sub_random, LW(5), LW(7))
        ADD(LW(6), LW(7))
        CALL(coin_entry, LW(0), LW(1), LW(2), LW(6))
        WAIT_MS(200)
    END_LOOP
    RETURN
EVT_END()

// TODO: write firebar_cb (global, 0xd0 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

// TODO: type
u32 firebardesc[70] = {
    0x00050000, 0xC42F0000, 0x42A00000, 0xC25C0000,
    0x41C80000, 0x41C80000, 0x3D23D70A, (u32)&firebar_cb,
    0x00000000, 0x00000000, 0x00050000, 0xC3FA0000,
    0x42A00000, 0x425C0000, 0x41C80000, 0x41C80000,
    0xBD23D70A, (u32)&firebar_cb, 0x00000000, 0x00000000,
    0x00050000, 0xC3960000, 0x42A00000, 0xC25C0000,
    0x41C80000, 0x41C80000, 0x3D23D70A, (u32)&firebar_cb,
    0x00000000, 0x00000000, 0x00040000, 0x00000000,
    0x42A00000, 0x00000000, 0x41C80000, 0x41C80000,
    0x3CF5C28F, (u32)&firebar_cb, 0x00000000, 0x41F00000,
    0x00040000, 0x00000000, 0x42A00000, 0x00000000,
    0x41C80000, 0x41C80000, 0x3CF5C28F, (u32)&firebar_cb,
    0x40490FDB, 0x41F00000, 0x01050000, 0x44480000,
    0xC1200000, 0xC2480000, 0x41C80000, 0x41C80000,
    0x3FC00000, (u32)&firebar_cb, 0x43610000, 0x00000000,
    0x01050000, 0x44480000, 0x41700000, 0xC2480000,
    0x41C80000, 0x41C80000, 0x3FC00000, (u32)&firebar_cb,
    0x43610000, 0x00000000,
};

// TODO: write firebar_disp_evt (global, 0x158 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

// TODO: write firebar_entry (global, 0x4e8 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(firebar_evt)
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(firebardesc))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: type
static u32 dmgRetPoint[16] = {
    (u32)"A_oti_01", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"A_oti_02", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"A_oti_03", 0x00000000, 0xC47A0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(las_04_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS2"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hit_damage_return_set, PTR(dmgRetPoint))
    CALL_CHILD(evt_gazigazi_entry)
    SPAWN_EVENT(firebar_evt)
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("hamon"), 1, 0)
    CALL(evt_mobj_brick, STRING("mobj_01"), 0, 150, 0, 134, 11, 0, GSWF(4384))
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -950, 140, -100, 0, 0)
    RETURN
EVT_END()

// TODO: write firebar_disp (global, 0xd0 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

// TODO: write _firebar_trans (global, 0x330 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

// TODO: write _firebar_rot (global, 0x36c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

// TODO: write _firebar_hitCheck2 (global, 0x208 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it

// TODO: write _firebar_hitCheck (global, 0x208 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_04.s; this is where the original has it, just before the first script that uses it
// clang-format on

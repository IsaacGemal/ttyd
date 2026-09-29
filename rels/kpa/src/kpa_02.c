#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma force_active on
extern s32 kpa_dead[];
extern s32 kpa_fall_sub[];
extern s32 kpa_init[];
extern s32 kpa_read_item[];
extern s32 kpa_read_mobj[];
extern s32 kpa_stage_start[];
extern s32 kpa_status_init[];
extern s32 kpa_time[];
static s32 kpa_goal[38];

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

// TODO: type
u32 kpa_act_stg1_mobj[151] = {
    0x00000004, 0xF2375680, 0xF24C6E80, 0x00000004,
    0xF2394A80, 0xF24C6E80, 0x00000000, 0xF23A1280,
    0xF24C6E80, 0x00000004, 0xF23ADA80, 0xF24C6E80,
    0x00000004, 0xF23ADA80, 0xF24EC680, 0x00000004,
    0xF23BA280, 0xF24C6E80, 0x00000004, 0xF23C6A80,
    0xF24C6E80, 0x00000009, 0xF23EC280, 0xF24A7A80,
    0x00000007, 0xF240E880, 0xF24ADE80, 0x00000079,
    0x0000000A, 0xF2417E80, 0xF24A7A80, 0x00000000,
    0xF2449E80, 0xF24C6E80, 0x00000004, 0xF24A7A80,
    0xF24ADE80, 0x00000000, 0xF24B4280, 0xF24ADE80,
    0x00000007, 0xF24DCC80, 0xF24ADE80, 0x00000079,
    0x0000000A, 0xF24E6280, 0xF24A7A80, 0x00000004,
    0xF249B280, 0xF24D9A80, 0x00000004, 0xF24A7A80,
    0xF24D9A80, 0x00000004, 0xF24B4280, 0xF24D9A80,
    0x00000004, 0xF24C0A80, 0xF24D9A80, 0x00000000,
    0xF2524A80, 0xF24DFE80, 0x00000009, 0xF2537680,
    0xF24A7A80, 0x00000009, 0xF2563280, 0xF24A7A80,
    0x00000004, 0xF256FA80, 0xF249B280, 0x00000004,
    0xF256FA80, 0xF24A7A80, 0x00000004, 0xF257C280,
    0xF249B280, 0x00000004, 0xF257C280, 0xF24A7A80,
    0x00000004, 0xF257C280, 0xF24B4280, 0x00000004,
    0xF2588A80, 0xF249B280, 0x00000004, 0xF2588A80,
    0xF24A7A80, 0x00000004, 0xF2588A80, 0xF24B4280,
    0x00000004, 0xF2588A80, 0xF24C0A80, 0x00000004,
    0xF2595280, 0xF249B280, 0x00000004, 0xF2595280,
    0xF24A7A80, 0x00000004, 0xF2595280, 0xF24B4280,
    0x00000004, 0xF2595280, 0xF24C0A80, 0x00000004,
    0xF2595280, 0xF24CD280, 0x00000004, 0xF25A1A80,
    0xF249B280, 0x00000004, 0xF25A1A80, 0xF24A7A80,
    0x00000004, 0xF25A1A80, 0xF24B4280, 0x00000004,
    0xF25A1A80, 0xF24C0A80, 0x00000004, 0xF25A1A80,
    0xF24CD280, 0x00000004, 0xF25A1A80, 0xF24D9A80,
    0x00000004, 0xF25AE280, 0xF249B280, 0x00000004,
    0xF25AE280, 0xF24A7A80, 0x00000004, 0xF25AE280,
    0xF24B4280, 0x00000004, 0xF25AE280, 0xF24C0A80,
    0x00000004, 0xF25AE280, 0xF24CD280, 0x00000004,
    0xF25AE280, 0xF24D9A80, 0x0000000F, 0xF25DD080,
    0xF24A7A80, (u32)&kpa_goal, 0xFFFFFFFF,
};

// TODO: type
u32 kpa_act_stg1_item[31] = {
    0x00000000, 0xF23FBC80, 0xF24C3C80, 0x00000000,
    0xF2405280, 0xF24C3C80, 0x00000000, 0xF242AA80,
    0xF24CA080, 0x00000000, 0xF2434080, 0xF24CA080,
    0x00000000, 0xF246F680, 0xF24B7480, 0x00000000,
    0xF2478C80, 0xF24B7480, 0x00000000, 0xF24CD280,
    0xF24C6E80, 0x00000000, 0xF24D6880, 0xF24C6E80,
    0x00000000, 0xF24F8E80, 0xF24D0480, 0x00000000,
    0xF2502480, 0xF24D0480, 0xFFFFFFFF,
};

static EVT_BEGIN(kpa_fall)
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(evt_koopa_chk_dead, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LF(0), 1)
            BREAK_LOOP
        END_IF
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(1), -20)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_EQUAL(LF(0), 0)
        CALL_CHILD(kpa_fall_sub)
    ELSE
        CALL_CHILD(kpa_dead)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(kpa_goal)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(GSW(0), 116)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 4096)
        CALL(evt_cam_letter_box_disable, 0)
        CALL(evt_seq_set_seq, 5, 0, 0)
    ELSE
        BEGIN_INLINE_EVENT
            WAIT_MS(300)
            CALL(evt_cam_letter_box_disable, 0)
        END_INLINE_EVENT
        CALL(evt_bero_mapchange, STRING("nok_00"), STRING("w_bero"))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[23] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(kpa_02_init_evt)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL_CHILD(kpa_init)
    CALL(evt_mario_set_pos, -1450, 0, -5)
    SET_VALUE(LW(0), PTR(kpa_act_stg1_mobj))
    CALL_CHILD(kpa_read_mobj)
    SET_VALUE(LW(0), PTR(kpa_act_stg1_item))
    CALL_CHILD(kpa_read_item)
    SPAWN_EVENT(kpa_fall)
    CALL(kpa_status_init, 1, GSW(1221), GSW(1222))
    SET_VALUE(LSW(1), 0)
    SPAWN_EVENT(kpa_stage_start)
    SPAWN_EVENT(kpa_time)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

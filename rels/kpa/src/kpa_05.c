#include "kpa_05.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "forcedata.h"
#include "koopa.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off

#pragma force_active on
extern s32 kpa_dead[];
extern s32 kpa_fall_sub[];
extern s32 kpa_init[];
extern s32 kpa_read_item[];
extern s32 kpa_read_mobj[];
extern s32 kpa_stage_start[];
extern s32 kpa_status_init[];
extern s32 kpa_time[];
static USER_FUNC(kpa_get_level);
static s32 jumpstand_evt[60];

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

// TODO: type
static u32 kpa_act_stg3_mobj[118] = {
    0x0000000D, 0xF239E080, 0xF24C0A80, 0x00000003,
    0xF24A7E80, 0x0000000D, 0xF23FBC80, 0xF24C0A80,
    0x00000002, 0xF24A8280, 0x0000000E, 0xF2427880,
    0xF24DFE80, 0x00000003, 0xF24A7E80, 0x00000000,
    0xF2456680, 0xF24D9A80, 0x00000005, 0xF246F680,
    0xF24ADE80, 0x00000005, 0xF247BE80, 0xF24ADE80,
    0x00000005, 0xF2488680, 0xF24ADE80, 0x00000005,
    0xF2494E80, 0xF24ADE80, 0x00000005, 0xF24A1680,
    0xF24ADE80, 0x00000005, 0xF24ADE80, 0xF24ADE80,
    0x00000005, 0xF24BA680, 0xF24ADE80, 0x00000005,
    0xF24C6E80, 0xF24BA680, 0x00000001, 0xF24E6280,
    0xF24ADE80, 0x00000001, 0xF24F2A80, 0xF24ADE80,
    0x00000001, 0xF24F2A80, 0xF24BA680, 0x00000001,
    0xF24FF280, 0xF24BA680, 0x00000000, 0xF250BA80,
    0xF24A7A80, 0x00000001, 0xF250BA80, 0xF24BA680,
    0x0000000E, 0xF2508880, 0xF24C6E80, 0x00000006,
    0xF24A7C80, 0x00000001, 0xF2518280, 0xF24BA680,
    0x00000001, 0xF2524A80, 0xF24ADE80, 0x00000001,
    0xF2524A80, 0xF24BA680, 0x00000001, 0xF2531280,
    0xF24ADE80, 0x00000005, 0xF255CE80, 0xF24A7A80,
    0x00000005, 0xF2569680, 0xF24A7A80, 0x00000000,
    0xF2556A80, 0xF24D3680, 0x00000005, 0xF2595280,
    0xF24ADE80, 0x00000005, 0xF25A1A80, 0xF24ADE80,
    0x00000005, 0xF25AE280, 0xF24B4280, 0x00000005,
    0xF25AE280, 0xF24C0A80, 0x00000010, 0xF25C0E80,
    0xF24A7A80, (u32)&jumpstand_evt, 0x00000005, 0xF25D3A80,
    0xF24B4280, 0x00000005, 0xF25D3A80, 0xF24C0A80,
    0x00000005, 0xF25E0280, 0xF24ADE80, 0x00000005,
    0xF25ECA80, 0xF24ADE80, 0x0000000B, 0xF2618680,
    0xF24D9A80, 0xFFFFFFFF,
};

// TODO: type
u32 kpa_act_stg3_item[79] = {
    0x00000000, 0xF247BE80, 0xF24D3680, 0x00000000,
    0xF2488680, 0xF24D3680, 0x00000000, 0xF24A1680,
    0xF24D3680, 0x00000000, 0xF24ADE80, 0xF24D3680,
    0x00000000, 0xF24E6280, 0xF24A7A80, 0x00000000,
    0xF24F2A80, 0xF24A7A80, 0x00000000, 0xF24FF280,
    0xF24A7A80, 0x00000000, 0xF24FF280, 0xF24ADE80,
    0x00000000, 0xF24FF280, 0xF24B4280, 0x00000000,
    0xF250BA80, 0xF24B4280, 0x00000000, 0xF2518280,
    0xF24B4280, 0x00000000, 0xF2518280, 0xF24ADE80,
    0x00000000, 0xF2518280, 0xF24A7A80, 0x00000000,
    0xF2524A80, 0xF24A7A80, 0x00000000, 0xF2531280,
    0xF24A7A80, 0x00000000, 0xF253DA80, 0xF24A7A80,
    0x00000000, 0xF2556A80, 0xF24BD880, 0x00000000,
    0xF2563280, 0xF24BD880, 0x00000000, 0xF256FA80,
    0xF24BD880, 0x00000000, 0xF25C0E80, 0xF24BD880,
    0x00000000, 0xF25C0E80, 0xF24C6E80, 0x00000000,
    0xF25C0E80, 0xF24D0480, 0x00000000, 0xF25FF680,
    0xF24AAC80, 0x00000000, 0xF25FF680, 0xF24B4280,
    0x00000000, 0xF260BE80, 0xF24AAC80, 0x00000000,
    0xF260BE80, 0xF24B4280, 0xFFFFFFFF,
};

static USER_FUNC(kpa_get_level) {
    s32* args = event->args;

    evtSetValue(event, args[0], kpaGetLevel());

    return EVT_RETURN_DONE;
}

static EVT_BEGIN(kpa_fall)
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(evt_koopa_chk_dead, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LF(0), 1)
            BREAK_LOOP
        END_IF
        CALL(evt_mario_get_name_hitobj_ride, LW(0))
        IF_STR_EQUAL(LW(0), STRING("a_magu"))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD_FLOAT(LW(1), FLOAT(3.0f))
        ADD_FLOAT(LW(2), FLOAT(5.0f))
        CALL(kpa_get_level, LW(3))
        SWITCH(LW(3))
            CASE_EQUAL(0)
            SET_FLOAT_VALUE(LW(3), FLOAT(0.8f))
            CASE_EQUAL(1)
            SET_FLOAT_VALUE(LW(3), FLOAT(1.7f))
            CASE_EQUAL(2)
            SET_FLOAT_VALUE(LW(3), FLOAT(3.0f))
            CASE_EQUAL(3)
            SET_FLOAT_VALUE(LW(3), FLOAT(4.5f))
        END_SWITCH
        CALL(evt_eff, STRING(""), STRING("splash"), 2, LW(0), LW(1), LW(2), LW(3), 0, 0, 0, 0, 0, 0, 0)
        CALL_CHILD(kpa_fall_sub)
    ELSE
        CALL_CHILD(kpa_dead)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(jumpstand_evt)
    DELETE_EVENT(GW(15))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_koopa_get_level, GSW(1223))
        CALL(evt_bero_mapchange, STRING("kpa_06"), STRING("kpa_next_map"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(1), 400)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_cam_off)
    END_INLINE_EVENT
    CALL(evt_mobj_get_position, STRING("me"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(1))
    ADD(LW(3), 800)
    CALL(evt_mario_jump_jumpstand, LW(0), LW(1), LW(2), LW(0), LW(3), LW(2), LW(0), LW(3), LW(2), 4000, 3)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    { NULL },
};

EVT_BEGIN(kpa_05_init_evt)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL_CHILD(kpa_init)
    CALL(evt_mario_set_pos, -1500, 150, -5)
    CALL(evt_cam_road_reset)
    SET_VALUE(LW(0), PTR(kpa_act_stg3_mobj))
    CALL_CHILD(kpa_read_mobj)
    SET_VALUE(LW(0), PTR(kpa_act_stg3_item))
    CALL_CHILD(kpa_read_item)
    CALL(evt_map_playanim, STRING("anm_magu"), 1, 0)
    SPAWN_EVENT(kpa_fall)
    CALL(kpa_status_init, 3, GSW(1221), GSW(1222))
    SET_VALUE(LSW(1), 2)
    SPAWN_EVENT(kpa_stage_start)
    SPAWN_EVENT_GET_ID(PTR(kpa_time), GW(15))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 kpa_bgm_change[];
extern s32 kpa_dead[];
extern s32 kpa_fall_sub[];
extern s32 kpa_init[];
extern s32 kpa_read_mobj[];
extern s32 kpa_status_init[];
extern s32 kpa_time[];
static s32 kpa_goal[22];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 kpa_act_stg2_mobj[80] = {
    0x00000009, 0xF246F680, 0xF24A7A80, 0x00000004,
    0xF247BE80, 0xF249B280, 0x00000004, 0xF247BE80,
    0xF24A7A80, 0x00000004, 0xF2488680, 0xF249B280,
    0x00000004, 0xF2488680, 0xF24A7A80, 0x00000004,
    0xF2488680, 0xF24B4280, 0x00000004, 0xF2494E80,
    0xF24A7A80, 0x00000004, 0xF2494E80, 0xF24B4280,
    0x00000004, 0xF2494E80, 0xF24C0A80, 0x00000004,
    0xF24A1680, 0xF24A7A80, 0x00000004, 0xF24A1680,
    0xF24B4280, 0x00000004, 0xF24A1680, 0xF24C0A80,
    0x00000004, 0xF24A1680, 0xF24CD280, 0x00000004,
    0xF24ADE80, 0xF249B280, 0x00000004, 0xF24ADE80,
    0xF24A7A80, 0x00000004, 0xF24ADE80, 0xF24B4280,
    0x00000004, 0xF24ADE80, 0xF24C0A80, 0x00000004,
    0xF24ADE80, 0xF24CD280, 0x00000004, 0xF24ADE80,
    0xF24D9A80, 0x00000004, 0xF24BA680, 0xF249B280,
    0x00000004, 0xF24BA680, 0xF24A7A80, 0x00000004,
    0xF24BA680, 0xF24B4280, 0x00000004, 0xF24BA680,
    0xF24C0A80, 0x00000004, 0xF24BA680, 0xF24CD280,
    0x00000004, 0xF24BA680, 0xF24D9A80, 0x0000000F,
    0xF24E9480, 0xF24A7A80, (u32)&kpa_goal, 0xFFFFFFFF,
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
    SET_VALUE(GSW(0), 217)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_cam_letter_box_disable, 0)
    END_INLINE_EVENT
    CALL(evt_bero_mapchange, STRING("gor_00"), STRING(""))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {NULL},
};

EVT_BEGIN(kpa_04_init_evt)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL_CHILD(kpa_init)
    CALL(evt_mario_set_pos, -400, 500, -5)
    CALL(evt_mario_set_dir, 90, 0, 0)
    SET_VALUE(LW(0), PTR(kpa_act_stg2_mobj))
    CALL_CHILD(kpa_read_mobj)
    SPAWN_EVENT(kpa_fall)
    CALL(kpa_status_init, 2, GSW(1221), GSW(1222))
    DEBUG_PUT_REG(GSW(1222))
    SET_VALUE(LSW(1), 0)
    SET_VALUE(LSWF(2), 1)
    SPAWN_EVENT(kpa_bgm_change)
    CALL(evt_snd_envoff, 512)
    SPAWN_EVENT(kpa_time)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        RETURN
    END_IF
    CALL(evt_koopa_set_level, GSW(1223))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

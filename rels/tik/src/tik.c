#include "tik.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on

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

// TODO: write evt_tik_villager_urouro_check_func (global, 0x148 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(tik_villager_urouro)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_get_position, STRING("me"), LW(6), LW(7), LW(8))
    CALL(evt_sub_random, 1000, LW(0))
    ADD(LW(0), 100)
    WAIT_MS(LW(0))
    LOOP(0)
        CALL(evt_npc_reverse_ry, STRING("me"))
        CALL(evt_sub_random, 200, LW(0))
        ADD(LW(0), 400)
        WAIT_MS(LW(0))
        CALL(evt_npc_reverse_ry, STRING("me"))
        CALL(evt_sub_random, 200, LW(0))
        ADD(LW(0), 800)
        WAIT_MS(LW(0))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(50.0f))
        CALL(evt_tik_villager_urouro_check_func, LW(0), LW(2), LW(6), LW(8), FLOAT(70.0f))
        CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
        CALL(evt_sub_get_dir, LW(10), LW(12), LW(0), LW(2), LW(3))
        CALL(evt_npc_set_ry, STRING("me"), LW(3))
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(30.0f), 4)
        WAIT_MS(1000)
    END_LOOP
EVT_END()

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x220 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik.s; this is where the original has it, just before the first script that uses it
// clang-format on

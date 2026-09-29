#include "win.h"
#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on

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

EVT_BEGIN(evt_win_door_open_pure)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_mapobj_get_position, LW(8), LW(10), LW(11), LW(12))
            CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD3"), LW(10), LW(11), LW(12), 0)
        ELSE
            CALL(evt_mapobj_get_position, LW(8), LW(10), LW(11), LW(12))
            CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), LW(10), LW(11), LW(12), 0)
        END_IF
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_sub_intpl_msec_init, 11, 0, -70, LW(1))
    ELSE
        CALL(evt_sub_intpl_msec_init, 11, -70, 0, LW(1))
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_rotate, 1, LW(8), 0, LW(2), 0)
        CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_win_door_open)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 800)
    CALL_CHILD(evt_win_door_open_pure)
    RETURN
EVT_END()

EVT_BEGIN(evt_win_door_close_pure)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(8), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_sub_intpl_msec_init, 11, 70, 0, LW(1))
    ELSE
        CALL(evt_sub_intpl_msec_init, 11, 0, 70, LW(1))
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_rotate, 1, LW(8), 0, LW(2), 0)
        CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_win_door_close)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 70)
    SET_VALUE(LW(2), 0)
    SUBTRACT(LW(2), LW(0))
    CALL(evt_mapobj_rotate, 1, LW(8), 0, LW(2), 0)
    CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(0), 0)
    WAIT_MS(500)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 800)
    CALL_CHILD(evt_win_door_close_pure)
    RETURN
EVT_END()

EVT_BEGIN(_tree_anime)
    CALL(evt_mapobj_get_position, LW(2), LW(4), LW(5), LW(6))
    CALL(evt_snd_sfxon_3d, STRING("SFX_HIT_TREE1"), LW(4), LW(5), LW(6), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_HIT_TREE1_LEAVES1"), LW(4), LW(5), LW(6), 0)
    CALL(evt_mapobj_trans, 1, LW(2), 1, 0, 0)
    CALL(evt_mapobj_trans, 1, LW(3), 4, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), -1, 0, 0)
    CALL(evt_mapobj_trans, 1, LW(3), -4, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), 1, 0, 0)
    CALL(evt_mapobj_trans, 1, LW(3), 2, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), 0, 0, 0)
    CALL(evt_mapobj_trans, 1, LW(3), -2, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), 0, 0, 0)
    CALL(evt_mapobj_trans, 1, LW(3), 0, 0, 0)
    IF_INT_NOT_EQUAL(LW(10), 0)
        SPAWN_EVENT(LW(10))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(win_tree_entry)
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT4(LW(1), LW(2), LW(3), LW(10))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 13, 1, LW(1), 0, PTR(_tree_anime), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(win_puni_walk_se)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_MOVE2_1"), LW(0), LW(1), LW(2), 0)
        WAIT_FRAMES(5)
    END_LOOP
    RETURN
EVT_END()

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/win/asm/win.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/win/asm/win.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0xf4 bytes) here:
// its asm is in build/G8MJ01/win/asm/win.s; this is where the original has it, just before the first script that uses it
// clang-format on

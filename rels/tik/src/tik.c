#include "tik.h"
#include "data/mapdata.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include <dolphin/mtx.h>

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

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("tik_00", tik_00_init_evt);
    relSetEvtAddr("tik_01", tik_01_init_evt);
    relSetEvtAddr("tik_02", tik_02_init_evt);
    relSetEvtAddr("tik_03", tik_03_init_evt);
    relSetEvtAddr("tik_04", tik_04_init_evt);
    relSetEvtAddr("tik_05", tik_05_init_evt);
    relSetEvtAddr("tik_06", tik_06_init_evt);
    relSetEvtAddr("tik_07", tik_07_init_evt);
    relSetEvtAddr("tik_08", tik_08_init_evt);
    relSetEvtAddr("tik_09", tik_09_init_evt);
    relSetEvtAddr("tik_10", tik_10_init_evt);
    relSetEvtAddr("tik_11", tik_11_init_evt);
    relSetEvtAddr("tik_12", tik_12_init_evt);
    relSetEvtAddr("tik_13", tik_13_init_evt);
    relSetEvtAddr("tik_14", tik_14_init_evt);
    relSetEvtAddr("tik_15", tik_15_init_evt);
    relSetEvtAddr("tik_16", tik_16_init_evt);
    relSetEvtAddr("tik_17", tik_17_init_evt);
    relSetEvtAddr("tik_18", tik_18_init_evt);
    relSetEvtAddr("tik_19", tik_19_init_evt);
    relSetEvtAddr("tik_20", tik_20_init_evt);
    relSetEvtAddr("tik_21", tik_21_init_evt);
    relSetBtlAddr("tik", btlsetup_tik_tbl, btl_tik_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

USER_FUNC(evt_tik_villager_urouro_check_func) {
    s32* args = event->args;
    s32 arg0 = args[0];
    s32 arg1 = args[1];
    f32 x = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    f32 limit = evtGetFloat(event, args[4]);
    f32 x2 = evtGetFloat(event, arg0);
    f32 z2 = evtGetFloat(event, arg1);
    Vec to;
    Vec from;

    to = (Vec){ x2, 0.0f, z2 };
    from = (Vec){ x, 0.0f, z };
    if (PSVECDistance(&from, &to) <= limit) {
        return EVT_RETURN_DONE;
    }
    evtSetFloat(event, arg0, x);
    evtSetFloat(event, arg1, z);
    return EVT_RETURN_DONE;
}

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

// clang-format on

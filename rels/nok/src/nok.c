#include "nok.h"
#include "data/mapdata.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "nok_00.h"
#include "nok_01.h"
#include <dolphin/mtx.h>

#pragma pool_data off

#pragma force_active on

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

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

const Vec vec3_nok_00000a58 = {0.0f, 0.0f, 0.0f};
const Vec vec3_nok_00000a64 = {0.0f, 0.0f, 0.0f};

EVT_BEGIN(nokonoko_urouro_regl)
    CALL(evt_npc_get_position, STRING("me"), LW(6), LW(7), LW(8))
    CALL(evt_sub_random, 300, LW(0))
    ADD(LW(0), 300)
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
        CALL(evt_nokonoko_urouro_check_func, LW(0), LW(2), LW(6), LW(8), FLOAT(70.0f))
        CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
        CALL(evt_sub_get_dir, LW(10), LW(12), LW(0), LW(2), LW(3))
        CALL(evt_npc_set_ry, STRING("me"), LW(3))
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(30.0f), 0)
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("nok_00", nok_00_init_evt);
    relSetEvtAddr("nok_01", nok_01_init_evt);
    relSetBtlAddr("nok", btlsetup_nok_tbl, btl_nok_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

USER_FUNC(evt_nokonoko_urouro_check_func) {
    s32* args = event->args;
    s32 arg0 = args[0];
    s32 arg1 = args[1];
    f32 f2 = evtGetFloat(event, args[2]);
    f32 f3 = evtGetFloat(event, args[3]);
    f32 f4 = evtGetFloat(event, args[4]);
    f32 f0 = evtGetFloat(event, arg0);
    f32 f1 = evtGetFloat(event, arg1);
    Vec c;
    Vec d;
    Vec a;
    Vec b;

    a = vec3_nok_00000a58;
    a.x = f0;
    a.z = f1;
    c = a;
    b = vec3_nok_00000a64;
    b.x = f2;
    b.z = f3;
    d = b;
    if (PSVECDistance(&d, &c) <= f4) {
        return EVT_RETURN_DONE;
    }
    evtSetFloat(event, arg0, f2);
    evtSetFloat(event, arg1, f3);
    return EVT_RETURN_DONE;
}
// clang-format on

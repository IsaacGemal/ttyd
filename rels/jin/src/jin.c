#include "jin.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "data/mapdata.h"
#include "memory.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"
#include <string.h>

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

static char** pose_buf;
// clang-format on

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

USER_FUNC(evt_jin_vivian_pose_set) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 pose = evtGetValue(event, args[1]);

    partyPoseTbl[6][index] = (char*)pose;
    return EVT_RETURN_DONE;
}

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("jin_00", jin_00_init_evt);
    relSetEvtAddr("jin_01", jin_01_init_evt);
    relSetEvtAddr("jin_02", jin_02_init_evt);
    relSetEvtAddr("jin_03", jin_03_init_evt);
    relSetEvtAddr("jin_04", jin_04_init_evt);
    relSetEvtAddr("jin_05", jin_05_init_evt);
    relSetEvtAddr("jin_06", jin_06_init_evt);
    relSetEvtAddr("jin_07", jin_07_init_evt);
    relSetEvtAddr("jin_08", jin_08_init_evt);
    relSetEvtAddr("jin_09", jin_09_init_evt);
    relSetEvtAddr("jin_10", jin_10_init_evt);
    relSetEvtAddr("jin_11", jin_11_init_evt);
    relSetBtlAddr("jin", btlsetup_jin_tbl, btl_jin_setup_no_tbl);

    pose_buf = _mapAlloc(0x3c);
    memcpy(pose_buf, partyPoseTbl[6], 0x3c);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
    memcpy(partyPoseTbl[6], pose_buf, 0x3c);
    _mapFree(pose_buf);
}

void _unresolved(void) {
}

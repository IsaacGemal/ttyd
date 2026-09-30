#include "usu.h"
#include "data/mapdata.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party.h"
#include "usu_00.h"
#include "usu_01.h"
#include <string.h>

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

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

// A one-entry buffer holding Vivian's saved pose table row (15 char*).
static char* pose_buf[1];

USER_FUNC(evt_usu_vivian_pose_set) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 pose = evtGetValue(event, args[1]);

    partyPoseTbl[PARTY_MEMBER_VIVIAN][index] = (char*)pose;
    return EVT_RETURN_DONE;
}

void _prolog(void) {
    voidfunctionptr* ctor;
    char* buf;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("usu_00", usu_00_init_evt);
    relSetEvtAddr("usu_01", usu_01_init_evt);
    buf = _mapAlloc(0x3C);
    pose_buf[0] = buf;
    memcpy(buf, partyPoseTbl[PARTY_MEMBER_VIVIAN], 0x3C);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
    memcpy(partyPoseTbl[PARTY_MEMBER_VIVIAN], pose_buf[0], 0x3C);
    _mapFree(pose_buf[0]);
}

void _unresolved(void) {
}
// clang-format on

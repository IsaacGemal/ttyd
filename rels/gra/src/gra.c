#include "gra.h"
#include "data/mapdata.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "memory.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on

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

static void* pose_buf;

USER_FUNC(evt_gra_vivian_pose_set) {
    s32* args = event->args;
    s32 poseId = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    partyPoseTbl[PARTY_MEMBER_VIVIAN][poseId] = (char*)value;
    return EVT_RETURN_DONE;
}

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("gra_00", gra_00_init_evt);
    relSetEvtAddr("gra_01", gra_01_init_evt);
    relSetEvtAddr("gra_02", gra_02_init_evt);
    relSetEvtAddr("gra_03", gra_03_init_evt);
    relSetEvtAddr("gra_04", gra_04_init_evt);
    relSetEvtAddr("gra_05", gra_05_init_evt);
    relSetEvtAddr("gra_06", gra_06_init_evt);
    relSetBtlAddr("gra", btlsetup_gra_tbl, btl_gra_setup_no_tbl);
    pose_buf = _mapAlloc(0x3c);
    memcpy(pose_buf, &partyPoseTbl[PARTY_MEMBER_VIVIAN][0], 0x3c);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
    memcpy(&partyPoseTbl[PARTY_MEMBER_VIVIAN][0], pose_buf, 0x3c);
    _mapFree(pose_buf);
}

void _unresolved(void) {
}
// clang-format on

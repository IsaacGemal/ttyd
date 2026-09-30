#include "aji.h"
#include "data/mapdata.h"
#include "driver/npcdrv.h"
#include "driver/swdrv.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "party.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
// clang-format on

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("aji_00", aji_00_init_evt);
    relSetEvtAddr("aji_01", aji_01_init_evt);
    relSetEvtAddr("aji_02", aji_02_init_evt);
    relSetEvtAddr("aji_03", aji_03_init_evt);
    relSetEvtAddr("aji_04", aji_04_init_evt);
    relSetEvtAddr("aji_05", aji_05_init_evt);
    relSetEvtAddr("aji_06", aji_06_init_evt);
    relSetEvtAddr("aji_07", aji_07_init_evt);
    relSetEvtAddr("aji_08", aji_08_init_evt);
    relSetEvtAddr("aji_09", aji_09_init_evt);
    relSetEvtAddr("aji_10", aji_10_init_evt);
    relSetEvtAddr("aji_11", aji_11_init_evt);
    relSetEvtAddr("aji_12", aji_12_init_evt);
    relSetEvtAddr("aji_13", aji_13_init_evt);
    relSetEvtAddr("aji_14", aji_14_init_evt);
    relSetEvtAddr("aji_15", aji_15_init_evt);
    relSetEvtAddr("aji_16", aji_16_init_evt);
    relSetEvtAddr("aji_17", aji_17_init_evt);
    relSetEvtAddr("aji_18", aji_18_init_evt);
    relSetEvtAddr("aji_19", aji_19_init_evt);
    relSetBtlAddr("aji", btlsetup_aji_tbl, btl_aji_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    if (swGet(0x1079)) {
        marioSetFamicomMode(0);
        partySetFamicomMode(partyGetPtr(marioGetPartyId()), 0);
        swClear(0x1079);
    }
    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

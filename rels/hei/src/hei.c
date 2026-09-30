#include "hei.h"
#include "data/mapdata.h"
#include "evt/evt_cmd.h"
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

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("hei_00", hei_00_init_evt);
    relSetEvtAddr("hei_01", hei_01_init_evt);
    relSetEvtAddr("hei_02", hei_02_init_evt);
    relSetEvtAddr("hei_03", hei_03_init_evt);
    relSetEvtAddr("hei_04", hei_04_init_evt);
    relSetEvtAddr("hei_05", hei_05_init_evt);
    relSetEvtAddr("hei_06", hei_06_init_evt);
    relSetEvtAddr("hei_07", hei_07_init_evt);
    relSetEvtAddr("hei_08", hei_08_init_evt);
    relSetEvtAddr("hei_09", hei_09_init_evt);
    relSetEvtAddr("hei_10", hei_10_init_evt);
    relSetEvtAddr("hei_11", hei_11_init_evt);
    relSetEvtAddr("hei_12", hei_12_init_evt);
    relSetEvtAddr("hei_13", hei_13_init_evt);
    relSetEvtAddr("hei_bt00", hei_bt00_init_evt);
    relSetBtlAddr("hei", btlsetup_hei_tbl, btl_hei_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}
// clang-format on

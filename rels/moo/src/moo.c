#include "moo_03.h"
#include "moo_04.h"
#include "moo.h"
#include "data/mapdata.h"
#include "forcedata.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

#pragma force_active on
// clang-format off
extend_data(moo_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
extend_data(moo_1,
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
    relSetEvtAddr("moo_00", moo_00_init_evt);
    relSetEvtAddr("moo_01", moo_01_init_evt);
    relSetEvtAddr("moo_02", moo_02_init_evt);
    relSetEvtAddr("moo_03", moo_03_init_evt);
    relSetEvtAddr("moo_04", moo_04_init_evt);
    relSetEvtAddr("moo_05", moo_05_init_evt);
    relSetEvtAddr("moo_06", moo_06_init_evt);
    relSetEvtAddr("moo_07", moo_07_init_evt);
    relSetBtlAddr("moo", btlsetup_moo_tbl, btl_moo_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {}

#include "bom.h"
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

// TODO: offset$519 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 offset$519[8] = {
//     0x00000000, 0x41200000, 0x41C00000, 0x41200000,
//     0x41700000, 0x41A00000, 0x41A00000, 0x41200000,
// };

// TODO: write bress (global, 0x300 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_bress)
    CALL(bress)
    RETURN
EVT_END()

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0xe4 bytes) here:
// its asm is in build/G8MJ01/bom/asm/bom.s; this is where the original has it, just before the first script that uses it
// clang-format on

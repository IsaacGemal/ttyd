#include "aji.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

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

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x1f8 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji.s; this is where the original has it, just before the first script that uses it
// clang-format on

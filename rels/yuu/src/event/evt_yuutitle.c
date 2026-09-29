#include "evt_yuutitle.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write evt_yuutitle_main_func (global, 0x5c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuutitle.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(main_evt)
    CALL(evt_yuutitle_main_func)
    RETURN
EVT_END()

// TODO: type
static u32 _wp[1] = {
    0x00000000,
};

// TODO: write evt_yuutitle_free (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuutitle.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuutitle_alloc (global, 0xd8 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuutitle.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuutitle_disp_func (global, 0x18c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuutitle.s; this is where the original has it, just before the first script that uses it
// clang-format on

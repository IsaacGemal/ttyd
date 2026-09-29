#include "evt_yuucounter.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

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

// TODO: write evt_yuucounter_main_func (global, 0x5c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(main_evt)
    CALL(evt_yuucounter_main_func)
    RETURN
EVT_END()

// TODO: write unk_yuu_00009d5c (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(unk_evt_yuu_00019728)
    CALL(unk_yuu_00009d5c, 1, 4)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_yuu_00019740)
    RETURN
EVT_END()

// TODO: type
static u32 _wp[1] = {
    0x00000000,
};

// TODO: type
static u32 yuunpc_wp[1] = {
    0x00000000,
};

// TODO: write yuucounter_flag (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

// TODO: write yuucounter_flag_off (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

// TODO: write yuucounter_flag_on (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

// TODO: write yuucounter_set_data (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

// TODO: write yuucounter_get_data (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuucounter_free (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuucounter_alloc (global, 0x12c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuucounter_disp_func (global, 0x7c0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuucounter.s; this is where the original has it, just before the first script that uses it
// clang-format on

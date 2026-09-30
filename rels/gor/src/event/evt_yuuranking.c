#include "evt_yuuranking.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static void yuuranking_get_monte(void);  // TODO: real signature
static void yuu_kesan(void);  // TODO: real signature
static USER_FUNC(evt_yuuranking_main_func);
static void evt_yuuranking_disp_func(void);  // TODO: real signature
static void fontlenpitch(void);  // TODO: real signature

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write evt_yuuranking_main_func (static, 0x668 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(main_evt)
    CALL(evt_yuuranking_main_func)
    RETURN
EVT_END()

// TODO: type
static u32 jumptable_00045058[10] = {
    (u32)((u8*)&evt_yuuranking_main_func + 0x80), (u32)((u8*)&evt_yuuranking_main_func + 0x98), (u32)((u8*)&evt_yuuranking_main_func + 0xd8), (u32)((u8*)&evt_yuuranking_main_func + 0x320),
    (u32)((u8*)&evt_yuuranking_main_func + 0x350), (u32)((u8*)&evt_yuuranking_main_func + 0x374), (u32)((u8*)&evt_yuuranking_main_func + 0x4bc), (u32)((u8*)&evt_yuuranking_main_func + 0x4ec),
    (u32)((u8*)&evt_yuuranking_main_func + 0x510), (u32)((u8*)&evt_yuuranking_main_func + 0x644),
};

// TODO: type
static u32 _wp[1] = {
    0x00000000,
};

// TODO: snd_id$716 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// static u32 snd_id$716[1] = {
//     0x00000000,
// };

// TODO: write yuuranking_get_monte (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuranking_wait_button_10_text_A37C (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuuranking_free_10_text_A3F8 (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuuranking_alloc_10_text_A47C (global, 0xddc bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

// TODO: write yuu_kesan (static, 0xb8 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_yuuranking_disp_func (static, 0x2580 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_gor_0000dfc4 (global, 0xe4 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it

// TODO: write fontlenpitch (static, 0x78 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it
// clang-format on

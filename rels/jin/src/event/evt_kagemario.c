#include "evt_kagemario.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(test);
static USER_FUNC(check);
static void color_change_evt(void);  // TODO: real signature
static void callback(void);  // TODO: real signature

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

// TODO: write check (static, 0x44 bytes) here:
// its asm is in build/G8MJ01/jin/asm/event/evt_kagemario.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_check)
    CALL(check)
    RETURN
EVT_END()

// TODO: write test (static, 0x40 bytes) here:
// its asm is in build/G8MJ01/jin/asm/event/evt_kagemario.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_usu_kagemario_party_kill_14_data_AF00)
    SWITCH(GSW(0))
        CASE_BETWEEN(201, 205)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_kill_party, 0)
        CALL(evt_party_stop, 1)
        CALL(evt_party_set_pos, 1, 0, -1000, 0)
        CALL(evt_party_cont_onoff, 1, 0)
        CALL(evt_mario_clear_party, 1)
        CALL(test)
    END_SWITCH
    RETURN
EVT_END()

// TODO: write evt_kagemario_change_14_text_4CF4 (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/jin/asm/event/evt_kagemario.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_kagemario_init_14_text_4DB8 (global, 0xe4 bytes) here:
// its asm is in build/G8MJ01/jin/asm/event/evt_kagemario.s; this is where the original has it, just before the first script that uses it

// TODO: write color_change_evt (static, 0xbc bytes) here:
// its asm is in build/G8MJ01/jin/asm/event/evt_kagemario.s; this is where the original has it, just before the first script that uses it

// TODO: write callback (static, 0x124 bytes) here:
// its asm is in build/G8MJ01/jin/asm/event/evt_kagemario.s; this is where the original has it, just before the first script that uses it
// clang-format on

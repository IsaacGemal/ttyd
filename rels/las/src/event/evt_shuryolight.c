#include "evt_shuryolight.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static void evt_shuryolight_run_tue_evt(void);  // TODO: real signature
static void evt_shuryolight_run_power_evt(void);  // TODO: real signature
static void evt_shuryolight_get_status(void);  // TODO: real signature
static void evt_shuryolight_get_anim_time(void);  // TODO: real signature
static USER_FUNC(evt_shuryolight_wait_anim_end);
static USER_FUNC(evt_shuryolight_set_anim);
static void shuryolightRunEvt(void);  // TODO: real signature
static void shuryolightSetAnim(void);  // TODO: real signature
static void shuryolight_callback(void);  // TODO: real signature

// clang-format off
extend_data(188,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(189,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static char* green_light_anim[5] = {
    "L_1C", "L_1B", "L_1D", "L_1A", "L_1E",
};

static char* yellow_light_anim[5] = {
    "L_2C", "L_2B", "L_2D", "L_2A", "L_2E",
};

static char* red_light_anim[5] = {
    "L_3C", "L_3B", "L_3D", "L_3A", "L_3E",
};

static char* orange_light_anim[5] = {
    "L_4C", "L_4B", "L_4D", "L_4A", "L_4E",
};

// TODO: write evt_shuryolight_set_anim (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(shuryolight_pow_normal_17_data_53DB0)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 0, 1)
        CALL(evt_sub_random, 3000, LW(0))
        ADD(LW(0), 3000)
        WAIT_MS(LW(0))
        CALL(evt_sub_random, 100, LW(0))
        DIVIDE(LW(0), 30)
        IF_INT_GREATER(LW(0), 0)
            LOOP(LW(0))
                CALL(evt_shuryolight_set_anim, 0, 3)
                WAIT_MS(80)
                CALL(evt_shuryolight_set_anim, 0, 1)
                WAIT_MS(80)
            END_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_pow_tenmetu_17_data_53E60)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 0, 1)
        CALL(evt_sub_random, 3000, LW(0))
        CALL(evt_sub_random, 1000, LW(1))
        ADD(LW(0), LW(1))
        ADD(LW(0), 700)
        WAIT_MS(LW(0))
        CALL(evt_shuryolight_set_anim, 0, 3)
        WAIT_MS(200)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_head_normal_17_data_53EDC)
    SET_VALUE(LW(0), 3)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, LW(0), 1)
        WAIT_MS(400)
        CALL(evt_shuryolight_set_anim, LW(0), 3)
        SUBTRACT(LW(0), 1)
        IF_INT_LESS(LW(0), 1)
            SET_VALUE(LW(0), 3)
            WAIT_MS(400)
        END_IF
    END_LOOP
    RETURN
EVT_END()

// TODO: write evt_shuryolight_wait_anim_end (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(shuryolight_head_tenmetu_17_data_53F54)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 1, 4)
        CALL(evt_shuryolight_set_anim, 2, 4)
        CALL(evt_shuryolight_set_anim, 3, 4)
        CALL(evt_shuryolight_wait_anim_end, 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_head_off_17_data_53FA4)
    CALL(evt_shuryolight_set_anim, 1, 3)
    CALL(evt_shuryolight_set_anim, 2, 3)
    CALL(evt_shuryolight_set_anim, 3, 3)
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_tue_normal_17_data_53FDC)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 4, 0)
        CALL(evt_shuryolight_wait_anim_end, 4)
        CALL(evt_shuryolight_set_anim, 4, 1)
        WAIT_MS(1000)
        CALL(evt_shuryolight_set_anim, 4, 2)
        CALL(evt_shuryolight_wait_anim_end, 4)
        CALL(evt_shuryolight_set_anim, 4, 3)
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 slhp[1] = {
    0x00000000,
};

// TODO: write evt_shuryolight_delete_17_text_10588 (global, 0x108 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_shuryolight_run_tue_evt (static, 0x88 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_shuryolight_run_head_evt_17_text_10718 (global, 0x88 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_shuryolight_run_power_evt (static, 0x88 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_shuryolight_run_evt_17_text_10828 (global, 0x164 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_shuryolight_get_status (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_shuryolight_get_anim_time (static, 0x9c bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_shuryolight_init_17_text_10B60 (global, 0x358 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write shuryolightRunEvt (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write shuryolightSetAnim (static, 0x50 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it

// TODO: write shuryolight_callback (static, 0x80 bytes) here:
// its asm is in build/G8MJ01/las/asm/event/evt_shuryolight.s; this is where the original has it, just before the first script that uses it
// clang-format on

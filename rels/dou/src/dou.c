#include "dou.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
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

// TODO: type
u32 dou_normal_puka_data[16] = {
    0x80000000, 0xBF000000, 0xBF800000, 0xBFC00000,
    0xC0000000, 0xC00CCCCD, 0xC0200000, 0xC02CCCCD,
    0xC02CCCCD, 0xC02CCCCD, 0xC0200000, 0xC00CCCCD,
    0xC0000000, 0xBFC00000, 0xBF800000, 0xBF000000,
};

// TODO: write evt_dou_normal_puka (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/dou/asm/dou.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(dou_normal_puka)
    SET_VALUE(LW(0), 1)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(1))
        SET_FLOAT_INDEXED(LW(6), LW(1))
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_sub)
    DEBUG_PUT_MSG(STRING("しずむ"))
    CALL(evt_sub_intpl_msec_init, 4, 0, LW(2), 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        SET_FLOAT_INDEXED(LW(6), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(LW(3))
    DEBUG_PUT_MSG(STRING("うく"))
    CALL(evt_sub_intpl_msec_init, 1, LW(2), 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        SET_FLOAT_INDEXED(LW(6), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event)
    SET_VALUE(LW(7), LW(0))
    SET_VALUE(LW(8), LW(1))
    SET_VALUE(LW(2), -10)
    SET_VALUE(LW(3), 200)
    CALL_CHILD(dou_puka_event_sub)
    SET_VALUE(LW(2), -8)
    SET_VALUE(LW(3), 150)
    CALL_CHILD(dou_puka_event_sub)
    SET_VALUE(LW(2), -5)
    SET_VALUE(LW(3), 100)
    CALL_CHILD(dou_puka_event_sub)
    SET_VALUE(LW(2), -2)
    SET_VALUE(LW(3), 50)
    CALL_CHILD(dou_puka_event_sub)
    RETURN
EVT_END()

// TODO: write dou_mario_pos_osreport_func (global, 0x2c bytes) here:
// its asm is in build/G8MJ01/dou/asm/dou.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(dou_mario_pos_osreport)
    CALL(dou_mario_pos_osreport_func)
    RETURN
EVT_END()

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/dou/asm/dou.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/dou/asm/dou.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x180 bytes) here:
// its asm is in build/G8MJ01/dou/asm/dou.s; this is where the original has it, just before the first script that uses it
// clang-format on

#include "sys.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static void _unresolved(void);  // TODO: real signature
static void _epilog(void);  // TODO: real signature
s32 evt_start[189];

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

EVT_BEGIN(dummy_evt)
    RETURN
EVT_END()

// TODO: type
static u32 niji_tbl[2] = {
    0x00000013, 0x00000016,
};

static char* msg_tbl[9] = {
    "msg_stg_seq_prolouge", "msg_stg_seq_stg1", "msg_stg_seq_stg2", "msg_stg_seq_stg3",
    "msg_stg_seq_stg4", "msg_stg_seq_stg5", "msg_stg_seq_stg6", "msg_stg_seq_stg7",
    "msg_stg_seq_stg8",
};

// TODO: type
u32 prologue_jp_tbl[2] = {
    0x10111106, 0x12FF0000,
};

// TODO: type
u32 stage1_jp_tbl[2] = {
    0x04050607, 0x08FF0000,
};

// TODO: type
u32 stage2_jp_tbl[2] = {
    0x04050607, 0x09FF0000,
};

// TODO: type
u32 stage3_jp_tbl[2] = {
    0x04050607, 0x0AFF0000,
};

// TODO: type
u32 stage4_jp_tbl[2] = {
    0x04050607, 0x0BFF0000,
};

// TODO: type
u32 stage5_jp_tbl[2] = {
    0x04050607, 0x0CFF0000,
};

// TODO: type
u32 stage6_jp_tbl[2] = {
    0x04050607, 0x0DFF0000,
};

// TODO: type
u32 stage7_jp_tbl[2] = {
    0x04050607, 0x0EFF0000,
};

// TODO: type
u32 stage8_jp_tbl[2] = {
    0x04050607, 0x0FFF0000,
};

// TODO: type
u32 texid_jp_tbl[9] = {
    (u32)&prologue_jp_tbl, (u32)&stage1_jp_tbl, (u32)&stage2_jp_tbl, (u32)&stage3_jp_tbl,
    (u32)&stage4_jp_tbl, (u32)&stage5_jp_tbl, (u32)&stage6_jp_tbl, (u32)&stage7_jp_tbl,
    (u32)&stage8_jp_tbl,
};

// TODO: type
u32 prologue_us_tbl[3] = {
    0x11121314, 0x13071508, 0xFF000000,
};

// TODO: type
u32 stage1_us_tbl[2] = {
    0x04050607, 0x0809FF00,
};

// TODO: type
u32 stage2_us_tbl[2] = {
    0x04050607, 0x080AFF00,
};

// TODO: type
u32 stage3_us_tbl[2] = {
    0x04050607, 0x080BFF00,
};

// TODO: type
u32 stage4_us_tbl[2] = {
    0x04050607, 0x080CFF00,
};

// TODO: type
u32 stage5_us_tbl[2] = {
    0x04050607, 0x080DFF00,
};

// TODO: type
u32 stage6_us_tbl[2] = {
    0x04050607, 0x080EFF00,
};

// TODO: type
u32 stage7_us_tbl[2] = {
    0x04050607, 0x080FFF00,
};

// TODO: type
u32 stage8_us_tbl[2] = {
    0x04050607, 0x0810FF00,
};

// TODO: type
u32 texid_us_tbl[9] = {
    (u32)&prologue_us_tbl, (u32)&stage1_us_tbl, (u32)&stage2_us_tbl, (u32)&stage3_us_tbl,
    (u32)&stage4_us_tbl, (u32)&stage5_us_tbl, (u32)&stage6_us_tbl, (u32)&stage7_us_tbl,
    (u32)&stage8_us_tbl,
};

// TODO: type
u32 lang_tbl[2] = {
    (u32)&texid_jp_tbl, (u32)&texid_us_tbl,
};

char* lang_prefix[2] = {
    "jp", "us",
};

// TODO: write init (global, 0x8 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

// TODO: write bg (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

// TODO: write display (global, 0x404 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(sys_01_init_evt)
    CALL(init)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, 0, 0)
    CALL(evt_party_set_pos, 0, 0, 0, 0)
    CALL(evt_cam3d_evt_set, 0, 0, 500, 0, 0, 0, 0, 11)
    CALL(bg)
    CALL(evt_fade_entry, 32, 0, 0, 0, 0)
    CALL(evt_fade_end_wait)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        CALL(evt_bero_get_entername, LW(1))
        IF_STR_EQUAL(LW(1), STRING("prologue"))
            SET_VALUE(LW(0), 0)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_1"))
            SET_VALUE(LW(0), 1)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_2"))
            SET_VALUE(LW(0), 2)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_3"))
            SET_VALUE(LW(0), 3)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_4"))
            SET_VALUE(LW(0), 4)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_5"))
            SET_VALUE(LW(0), 5)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_6"))
            SET_VALUE(LW(0), 6)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_7"))
            SET_VALUE(LW(0), 7)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_8"))
            SET_VALUE(LW(0), 8)
        END_IF
        CALL(display, LW(0))
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_fade_set_mapchange_type, 0, 17, 1800, 16, 1800)
    CALL(evt_fade_set_mapchange_type, 1, 17, 1800, 16, 1800)
    SPAWN_EVENT(evt_start)
    RETURN
EVT_END()

// TODO: write start (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_start)
    CALL(evt_fade_end_wait)
    WAIT_MS(2000)
    CALL(evt_bero_get_entername, LW(1))
    IF_STR_NOT_EQUAL(LW(1), STRING("stg_3"))
        IF_STR_NOT_EQUAL(LW(1), STRING("stg_5"))
            IF_STR_NOT_EQUAL(LW(1), STRING("stg_6"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_FF_STG_START1"))
            END_IF
        END_IF
    END_IF
    CALL(evt_fade_entry, 35, 0, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(start)
    WAIT_MS(5000)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_BITS_SET(LW(0), 4864)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_fade_set_mapchange_type, 0, 19, 500, 18, 500)
    CALL(evt_fade_set_mapchange_type, 1, 19, 500, 18, 500)
    CALL(evt_bero_get_entername, LW(1))
    IF_STR_EQUAL(LW(1), STRING("prologue"))
        SET_VALUE(LW(0), 0)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_1"))
        SET_VALUE(LW(0), 1)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_2"))
        SET_VALUE(LW(0), 2)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_3"))
        SET_VALUE(LW(0), 3)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_4"))
        SET_VALUE(LW(0), 4)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_5"))
        SET_VALUE(LW(0), 5)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_6"))
        SET_VALUE(LW(0), 6)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_7"))
        SET_VALUE(LW(0), 7)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_8"))
        SET_VALUE(LW(0), 8)
    END_IF
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(evt_bero_mapchange, STRING("gor_10"), 0)
        CASE_EQUAL(1)
        CALL(evt_bero_mapchange, STRING("hei_00"), STRING("dokan_2"))
        CASE_EQUAL(2)
        CALL(evt_bero_mapchange, STRING("win_06"), STRING("dokan1"))
        CASE_EQUAL(3)
        CALL(evt_bero_mapchange, STRING("tou_20"), 0)
        CASE_EQUAL(4)
        CALL(evt_bero_mapchange, STRING("usu_00"), STRING("dokan_1"))
        CASE_EQUAL(5)
        CALL(evt_bero_mapchange, STRING("muj_20"), 0)
        CASE_EQUAL(6)
        CALL(evt_bero_mapchange, STRING("rsh_02_a"), 0)
        CASE_EQUAL(7)
        CALL(evt_bero_mapchange, STRING("bom_00"), STRING("dokan"))
        CASE_EQUAL(8)
        CALL(evt_bero_mapchange, STRING("las_00"), STRING("w_bero"))
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
u32 g_wp[1] = {
    0x00000000,
};

// TODO: write display_func (global, 0x10cc bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

// TODO: write getLightColor (global, 0xc0 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

// TODO: write sys_01_release (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

// TODO: write _unresolved (static, 0x4 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (static, 0xa8 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x114 bytes) here:
// its asm is in build/G8MJ01/sys/asm/sys.s; this is where the original has it, just before the first script that uses it
// clang-format on

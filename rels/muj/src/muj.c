#include "muj.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(tornade_alpha_func);

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
u32 muj_ship_puka_data[16] = {
    0x80000000, 0xBF000000, 0xBF800000, 0xBFC00000,
    0xC0000000, 0xC00CCCCD, 0xC0200000, 0xC02CCCCD,
    0xC02CCCCD, 0xC02CCCCD, 0xC0200000, 0xC00CCCCD,
    0xC0000000, 0xBFC00000, 0xBF800000, 0xBF000000,
};

// TODO: write muj_ship_puka_func (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(muj_ship_puka)
    SET_VALUE(LW(10), 0)
    LOOP(0)
        CALL(muj_ship_puka_func, LW(10), LW(1), LW(11))
        SET_FLOAT_INDEXED(LW(0), LW(11))
        ADD(LW(10), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

// TODO: write tornade_alpha_func (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(korutesu_apper)
    CALL(evt_npc_get_position, STRING("コルテス"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KOR_APPEAR1"), LW(0), LW(1), LW(2), LW(10))
        WAIT_MS(1000)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KOR_APPEAR2"), LW(0), LW(1), LW(2), LW(10))
        WAIT_MS(2000)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_eff64, STRING("tornade"), STRING("kameki_tornade_n64"), 0, LW(0), LW(1), LW(2), FLOAT(2.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(tornade_alpha_func, STRING("tornade"), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, FLOAT(0.0f), FLOAT(100.0f), 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(tornade_alpha_func, STRING("tornade"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_eff_softdelete, STRING("tornade"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(700)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(600)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(500)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(400)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(300)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(200)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(100)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(50)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
    END_INLINE_EVENT
    WAIT_MS(3000)
    RETURN
EVT_END()

EVT_BEGIN(korutesu_vanish)
    CALL(evt_npc_get_position, STRING("コルテス"), LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING("tornade"), STRING("kameki_tornade_n64"), 0, LW(0), LW(1), LW(2), FLOAT(2.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(tornade_alpha_func, STRING("tornade"), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, FLOAT(0.0f), FLOAT(100.0f), 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(tornade_alpha_func, STRING("tornade"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG5_KOR_GOOUT1"), LW(10))
        WAIT_MS(3000)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_eff_softdelete, STRING("tornade"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(50)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(100)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(200)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(300)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(400)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(500)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(600)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(700)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(800)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: write muj_mario_pos_osreport_func (global, 0x2c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(muj_mario_pos_osreport)
    CALL(muj_mario_pos_osreport_func)
    RETURN
EVT_END()

// TODO: write muj_npc_buruburu (global, 0x31c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

// TODO: write muj_npc_fuwafuwa (global, 0x23c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

// TODO: write muj_npc_guruguru (global, 0x358 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x144 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj.s; this is where the original has it, just before the first script that uses it
// clang-format on

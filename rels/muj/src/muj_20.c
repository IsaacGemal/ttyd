#include "muj_20.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_light.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_nannpc.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 muj_npc_buruburu[];
extern s32 muj_npc_fuwafuwa[];
extern s32 muj_npc_guruguru[];
extern s32 muj_ship_puka[];
static void draw(void);  // TODO: real signature
static void pressStartGX(void);  // TODO: real signature
static USER_FUNC(fune_eff);
static USER_FUNC(alpha_set_func);
static USER_FUNC(alpha_init_func);
static void alpha_tev(void);  // TODO: real signature

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(hiru_yoru_event)
    CALL(evt_map_playanim, STRING("hiru_yoru"), 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(4.0f))
        WAIT_MS(750)
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(3.4f))
        WAIT_MS(200)
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(2.8f))
        WAIT_MS(100)
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(1.5f))
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_map_checkanim, STRING("hiru_yoru"), LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yoru_hiru_event)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(4.0f))
        WAIT_MS(750)
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(3.4f))
        WAIT_MS(100)
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(2.8f))
        WAIT_MS(100)
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(1.5f))
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_map_checkanim, STRING("yoru_hiru"), LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: write urouro_func (global, 0x29c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(urouro)
    CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
    LOOP(0)
        CALL(urouro_func, LW(0), LW(11), LW(13), LW(1), LW(3), LW(4), LW(5), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_ESCAPE1"), LW(11), LW(12), LW(13), 0)
        END_INLINE_EVENT
        CALL(urouro_func, LW(0), LW(1), LW(3), LW(11), LW(13), LW(4), LW(5), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_ESCAPE1"), LW(11), LW(12), LW(13), 0)
        END_INLINE_EVENT
    END_LOOP
    RETURN
EVT_END()

// TODO: write urouro_fuwafuwa_func (global, 0x414 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(urouro_fuwafuwa)
    CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
    LOOP(0)
        CALL(urouro_fuwafuwa_func, LW(0), LW(11), LW(12), LW(13), LW(1), LW(2), LW(3), LW(4), LW(5), 0)
        CALL(urouro_fuwafuwa_func, LW(0), LW(1), LW(2), LW(3), LW(11), LW(12), LW(13), LW(4), LW(5), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(buruburu)
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_sub_random, 200, LW(10))
    ADD(LW(10), 100)
    WAIT_MS(LW(10))
    CALL(muj_npc_buruburu, LW(0), 0, FLOAT(1.0f), 10, 0)
    RETURN
EVT_END()

EVT_BEGIN(fuwafuwa)
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    LOOP(0)
        CALL(muj_npc_fuwafuwa, LW(0), 0, 10000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kirimomi)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 0, 0, 360, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_npc_set_ry, LW(10), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

// TODO: write marcoship_yure_func (global, 0x1e8 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write marcoship_yure_init_func (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(marco_ship_yure)
    CALL(marcoship_yure_init_func)
    CALL(evt_hit_bind_mapobj, STRING("A_a1"), STRING("S_a1"))
    LOOP(0)
        CALL(marcoship_yure_func, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_a1"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect01)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_01"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_01"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_01"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect02)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_02"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_02"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_02"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_02"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect03)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_03"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_03"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_03"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_03"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect04)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_04"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_04"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_04"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect05)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_05"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_05"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_05"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_05"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: write alpha_set_func (static, 0x3c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write alpha_init_func (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write alpha_tev (static, 0xe4 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(nami_alpha)
    CALL(alpha_init_func)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_umi_l_siro_b"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_umi_l_siro_b"), 256)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(alpha_set_func, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moonout_effect)
    CALL(alpha_init_func)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_hai02"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_hai03_moon"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hai02"), 256)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hai03_moon"), 256)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 1500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(alpha_set_func, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect)
    SPAWN_EVENT(moya_effect01)
    SPAWN_EVENT(moya_effect02)
    SPAWN_EVENT(moya_effect03)
    SPAWN_EVENT(moya_effect04)
    SPAWN_EVENT(moya_effect05)
    CALL(evt_map_playanim, STRING("onryou"), 0, 0)
    SPAWN_EVENT(moonout_effect)
    RETURN
EVT_END()

EVT_BEGIN(_kakushuku)
    CALL(evt_sub_intpl_msec_init, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(2), LW(0))
        DIVIDE_FLOAT(LW(2), FLOAT(100.0f))
        CALL(evt_npc_set_scale, STRING("エルモスA"), LW(2), LW(2), LW(2))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(elmos_a_kakushuku)
    SET_VALUE(LW(0), 100)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 150)
    SPAWN_EVENT(_kakushuku)
    WAIT_MS(500)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 100)
    SET_VALUE(LW(2), 150)
    SPAWN_EVENT(_kakushuku)
    LOOP(0)
        SET_VALUE(LW(0), 100)
        SET_VALUE(LW(1), 150)
        SET_VALUE(LW(2), 100)
        SPAWN_EVENT(_kakushuku)
        WAIT_MS(100)
        SET_VALUE(LW(0), 150)
        SET_VALUE(LW(1), 100)
        SET_VALUE(LW(2), 100)
        SPAWN_EVENT(_kakushuku)
        WAIT_MS(100)
        IF_INT_EQUAL(GF(10), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GF(11), 0)
    RETURN
EVT_END()

EVT_BEGIN(elmos_a_kaku)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 100)
    SET_VALUE(LW(2), 150)
    CALL_CHILD(_kakushuku)
    RETURN
EVT_END()

EVT_BEGIN(elmos_a_shuku)
    SET_VALUE(LW(0), 100)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 150)
    CALL_CHILD(_kakushuku)
    RETURN
EVT_END()

// TODO: type
static u32 ext_entry_data[63] = {
    (u32)"c_elmos", (u32)"S_1", 0x00000000, (u32)"c_elmos",
    (u32)"S_1", 0x3F800000, (u32)"c_elmos", (u32)"S_1",
    0x40000000, (u32)"c_elmos", (u32)"S_1", 0x40400000,
    (u32)"c_elmos", (u32)"S_1", 0x40800000, (u32)"c_elmos",
    (u32)"S_1", 0x40A00000, (u32)"c_elmos", (u32)"S_1",
    0x40C00000, (u32)"c_elmos", (u32)"S_1", 0x40E00000,
    (u32)"c_elmos", (u32)"S_1", 0x41000000, (u32)"c_elmos",
    (u32)"S_1", 0x41100000, (u32)"c_elmos", (u32)"S_1",
    0x41200000, (u32)"c_elmos", (u32)"S_1", 0x41300000,
    (u32)"c_elmos", (u32)"S_1", 0x41400000, (u32)"c_elmos",
    (u32)"S_1", 0x41500000, (u32)"c_elmos", (u32)"S_1",
    0x41600000, (u32)"c_elmos", (u32)"S_1", 0x41700000,
    (u32)"c_elmos", (u32)"S_1", 0x41800000, (u32)"c_elmos",
    (u32)"S_1", 0x41880000, (u32)"c_elmos", (u32)"S_1",
    0x41900000, (u32)"c_elmos", (u32)"S_1", 0x41980000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 elmos_animtbl[11] = {
    0x00000001, 0x00020003, 0x00040005, 0x00060007,
    0x00080009, 0x000A000B, 0x000C000D, 0x000E000F,
    0x00100011, 0x00120013, 0xFFFF0000,
};

// TODO: write elmos1000tai_init (global, 0x23c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(nannpc_mode_setup)
    CALL(evt_nannpc_init, PTR(ext_entry_data), 0, 0, 0, 1000)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    SET_VALUE(LW(0), 0)
    LOOP(1000)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(elmos_animtbl))
        ADD(LW(0), 1)
    END_LOOP
    CALL(elmos1000tai_init)
    RETURN
EVT_END()

// TODO: type
u32 mist_pos[91] = {
    0xFFFFFE70, 0x0000001E, 0xFFFFFF38, 0xFFFFFE70,
    0x0000001E, 0xFFFFFE70, 0xFFFFFE70, 0x0000001E,
    0xFFFFFDA8, 0xFFFFFE70, 0x0000001E, 0xFFFFFCE0,
    0xFFFFFE70, 0x0000001E, 0xFFFFFC18, 0xFFFFFE70,
    0x0000001E, 0xFFFFFB50, 0xFFFFFF38, 0x0000001E,
    0xFFFFFF38, 0xFFFFFF38, 0x0000001E, 0xFFFFFE70,
    0xFFFFFF38, 0x0000001E, 0xFFFFFDA8, 0xFFFFFF38,
    0x0000001E, 0xFFFFFCE0, 0xFFFFFF38, 0x0000001E,
    0xFFFFFC18, 0xFFFFFF38, 0x0000001E, 0xFFFFFB50,
    0x00000000, 0x0000001E, 0xFFFFFF38, 0x00000000,
    0x0000001E, 0xFFFFFE70, 0x00000000, 0x0000001E,
    0xFFFFFDA8, 0x00000000, 0x0000001E, 0xFFFFFCE0,
    0x00000000, 0x0000001E, 0xFFFFFC18, 0x00000000,
    0x0000001E, 0xFFFFFB50, 0x000000C8, 0x0000001E,
    0xFFFFFF38, 0x000000C8, 0x0000001E, 0xFFFFFE70,
    0x000000C8, 0x0000001E, 0xFFFFFDA8, 0x000000C8,
    0x0000001E, 0xFFFFFCE0, 0x000000C8, 0x0000001E,
    0xFFFFFC18, 0x000000C8, 0x0000001E, 0xFFFFFB50,
    0x00000190, 0x0000001E, 0xFFFFFF38, 0x00000190,
    0x0000001E, 0xFFFFFE70, 0x00000190, 0x0000001E,
    0xFFFFFDA8, 0x00000190, 0x0000001E, 0xFFFFFCE0,
    0x00000190, 0x0000001E, 0xFFFFFC18, 0x00000190,
    0x0000001E, 0xFFFFFB50, 0xFFFFD8F0,
};

// TODO: write mist_func (global, 0x21c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(mist_setup)
    LOOP(0)
        SET_VALUE(LW(10), 0)
        LOOP(0)
            SET_INT_READ_ADDR(PTR(mist_pos))
            READ_INT_INDEXED(LW(0), LW(10))
            IF_INT_EQUAL(LW(0), -10000)
                BREAK_LOOP
            END_IF
            ADD(LW(10), 1)
            READ_INT_INDEXED(LW(1), LW(10))
            ADD(LW(10), 1)
            READ_INT_INDEXED(LW(2), LW(10))
            ADD(LW(10), 1)
            BEGIN_INLINE_EVENT
                CALL(mist_func, LW(0), LW(1), LW(2))
            END_INLINE_EVENT
        END_LOOP
        WAIT_MS(2000)
    END_LOOP
    RETURN
EVT_END()

// TODO: write marcoship_shizumu_func (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(marcoship_shizumu)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_QUAKE1"), LW(10))
    SET_FLOAT_VALUE(LW(2), FLOAT(0.0f))
    CALL(evt_hit_bind_mapobj, STRING("A_a1"), STRING("S_a1"))
    CALL(evt_sub_intpl_msec_init, 1, 0, -300, 8000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(marcoship_shizumu_func, LW(0))
        SET_FLOAT_VALUE(LW(3), LW(0))
        SUBTRACT_FLOAT(LW(3), LW(2))
        CALL(evt_npc_get_position, STRING("マルコ"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("マルコ"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("オドオド水夫"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("オドオド水夫"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("水夫A"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("水夫A"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("水夫ボム兵"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("水夫ボム兵"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("店員"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("店員"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("宿店員"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("宿店員"), LW(4), LW(5), LW(6))
        CALL(evt_hit_bind_update, STRING("A_a1"))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        SET_FLOAT_VALUE(LW(2), LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(10))
    RETURN
EVT_END()

// TODO: write elmos1000tai_start (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_set_nami_anim_rate (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_set_fune_anim_rate (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_map_anim_rate (global, 0x128 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write marcoship_stop_wait_func (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write marcoship_stop_func (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(onryo_event)
    BEGIN_INLINE_EVENT
        CALL(evt_map_anim_rate)
    END_INLINE_EVENT
    SPAWN_EVENT(marco_ship_yure)
    SPAWN_EVENT(nannpc_mode_setup)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
    CALL(evt_hitobj_onoff, STRING("A_b"), 1, 0)
    IF_INT_EQUAL(GSW(0), 230)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_set_pos, 5, 90, -100)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        GOTO(100)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 0, 600, 2155, 0, 350, -108, 0, 11)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
    CALL(evt_seq_wait, 2)
    WAIT_MS(4000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_00"), 0, 0)
    CALL_CHILD(hiru_yoru_event)
    WAIT_MS(3000)
    CALL_CHILD(yoru_hiru_event)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_28"), 0, 0)
    CALL_CHILD(hiru_yoru_event)
    WAIT_MS(3000)
    CALL_CHILD(yoru_hiru_event)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_29"), 0, 0)
    CALL_CHILD(hiru_yoru_event)
    WAIT_MS(1000)
    SET_VALUE(GSW(0), 230)
    LABEL(100)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 5, 90, -100)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_a1"), 2)
    CALL(evt_npc_set_position, STRING("マルコ"), 116, 116, -100)
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), 25, 90, -163)
    CALL(evt_npc_set_ry, STRING("オドオド水夫"), 90)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 21, 90, -55)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_set_position, STRING("サンダース"), -11, 90, -163)
    CALL(evt_npc_set_ry, STRING("サンダース"), 90)
    CALL(evt_npc_set_position, STRING("めがね水夫"), -15, 90, -40)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 90)
    CALL(evt_npc_set_position, STRING("水夫A"), -65, 114, -145)
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫A"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("水夫A"))
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), -65, 114, -44)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫ボム兵"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("水夫ボム兵"))
    CALL(evt_npc_set_position, STRING("店員"), -115, 163, -153)
    CALL(evt_npc_set_ry, STRING("店員"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("店員"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("店員"))
    CALL(evt_npc_set_position, STRING("宿店員"), -119, 163, -62)
    CALL(evt_npc_set_ry, STRING("宿店員"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("宿店員"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("宿店員"))
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 1073743392)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("サンダース"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("めがね水夫"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫A"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫ボム兵"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("店員"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("宿店員"), 32768)
    CALL(evt_npc_sound_data_set, STRING("マルコ"), STRING("SFX_STG5_MARCO_MOVE1L"), STRING("SFX_STG5_MARCO_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(evt_cam3d_evt_set, -10, 340, 558, -11, 131, -108, 0, 11)
    WAIT_MS(1000)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("サンダース"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("めがね水夫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫A"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫ボム兵"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("店員"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("宿店員"), 32768)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_01"), 0, STRING("マルコ"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 480, 352, 214, 58, 150, -96, 0, 11)
    WAIT_MS(1500)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("マルコ"), 107, -100, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_02"), 0, STRING("マルコ"))
    CALL(evt_cam3d_evt_set, 0, 300, 985, 0, 175, -108, 0, 11)
    CALL(evt_snd_bgmoff, 26624)
    SET_VALUE(GW(0), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 512)
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("S_2"))
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("small_01"), 0, STRING("マルコ"))
        LOOP(0)
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
            CALL(evt_msg_repeat)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_set_fune_anim_rate, FLOAT(0.7f))
    CALL(evt_set_nami_anim_rate, FLOAT(0.7f))
    WAIT_MS(1000)
    WAIT_MS(1500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_03"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_04"), 0, STRING("オドオド水夫"))
    CALL(marcoship_stop_func)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff_f, 512, 2000)
        WAIT_MS(2000)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_EVT_STG5_GHOST1"), 2000)
        CALL(evt_snd_envoff, 18432)
    END_INLINE_EVENT
    CALL(evt_set_fune_anim_rate, FLOAT(0.4f))
    CALL(evt_set_nami_anim_rate, FLOAT(0.4f))
    CALL(evt_msg_print_add, 0, STRING("stg5_sea_04_01"))
    CALL(evt_cam3d_evt_set, 0, 295, 370, 0, 145, -108, 0, 11)
    SPAWN_EVENT(nami_alpha)
    CALL(evt_set_fune_anim_rate, FLOAT(0.0f))
    CALL(evt_set_nami_anim_rate, FLOAT(0.1f))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_fne_nami"), 1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_05"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(1000)
    SET_VALUE(GW(0), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 512)
    CALL(evt_msg_close, STRING("small_01"))
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("T_1"))
    WAIT_MS(2000)
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("S_1"))
    CALL(marcoship_stop_wait_func)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("マルコ"), 68, -71, 0, FLOAT(120.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_06"), 0, STRING("マルコ"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_07"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_cam3d_evt_set, -50, 470, 588, -50, 390, -107, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("R_1"))
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -12, 90, -80)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -52, 315, -75, 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("S_1"))
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("R_1"))
    CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -78, 315, -79, 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("S_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -23, 315, -79)
    CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, 180, 0)
    WAIT_MS(1200)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -78, 315, -79)
    CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, 0, 0)
    WAIT_MS(1800)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -23, 315, -79)
    CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, 180, 0)
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("エルモスA"), -45, 336, -73)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_APPEAR1_2"), -45, 336, -73, 0)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 90)
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("エルモスA"), LW(10), LW(11), LW(12))
    SET_VALUE(LW(13), LW(10))
    SET_VALUE(LW(14), LW(11))
    SET_VALUE(LW(15), LW(12))
    ADD(LW(13), 5)
    ADD(LW(14), -5)
    ADD(LW(15), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_08"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_npc_set_position, STRING("エルモスA"), 0, -1000, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 180, 0, 200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(200)
    SPAWN_EVENT(moya_effect)
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_NPC_QUESTION_MARK1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, LW(0), LW(1), LW(2), 30, 0, 90)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    WAIT_MS(500)
    CALL(evt_npc_set_position, STRING("エルモスA"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_APPEAR1_2"), LW(10), LW(11), LW(12), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_09"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -50, 412, 340, -50, 360, -108, 100, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_NPC_BIKKURI_MARK1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 0, 0, LW(0), LW(1), LW(2), 30, 0, 60)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER2"))
    END_INLINE_EVENT
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(5)
            CALL(evt_cam3d_evt_set, -50, 412, 340, -50, 360, -108, 100, 11)
            WAIT_MS(100)
            CALL(evt_cam3d_evt_set, -50, 423, 438, -50, 360, -108, 100, 11)
            WAIT_MS(100)
        END_LOOP
        CALL(evt_cam3d_evt_set, -50, 412, 340, -50, 360, -108, 100, 11)
        WAIT_MS(100)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg5_sea_11"), 0, STRING("ガラの悪い水夫"))
    WAIT_WHILE(GF(0))
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("R_1"))
    CALL(muj_npc_guruguru, STRING("ガラの悪い水夫"), -52, -90, 0, FLOAT(-12.0f), 3000)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
        CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -52, 315, -70, 0, FLOAT(180.0f), 0, 0, 0)
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
        CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -43, 90, -72, 0, FLOAT(180.0f), 0, 0, 0)
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
        CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), 20, 90, -55, 0, FLOAT(180.0f), 0, 0, 0)
        CALL(evt_npc_jump_position_nohit, STRING("ガラの悪い水夫"), 20, 90, -55, 100, 30)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -10, 340, 558, -11, 131, -108, 1000, 11)
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print, 0, STRING("stg5_sea_12"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_13"), 0, STRING("マルコ"))
    WAIT_MS(500)
    SPAWN_EVENT(mist_setup)
    CALL(evt_npc_set_position, STRING("エルモスA"), 168, 160, -84)
    CALL(evt_npc_set_position, STRING("エルモスB"), 105, 100, -200)
    CALL(evt_npc_set_position, STRING("エルモスC"), 10, 110, -200)
    CALL(evt_npc_set_position, STRING("エルモスD"), -70, 105, -200)
    CALL(evt_npc_set_position, STRING("エルモスE"), 125, 105, 0)
    CALL(evt_npc_set_position, STRING("エルモスF"), 10, 100, 0)
    CALL(evt_npc_set_position, STRING("エルモスG"), -80, 110, 0)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスB"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスC"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスD"), 90)
    CALL(evt_npc_set_ry, STRING("エルモスE"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスF"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスG"), 90)
    CALL(evt_npc_set_color, STRING("エルモスA"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスB"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスC"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスD"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスE"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスF"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスG"), 255, 255, 255, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_APPEAR1_2"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_color, STRING("エルモスA"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスB"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスC"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスD"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスE"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスF"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスG"), 255, 255, 255, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    SET_VALUE(LW(0), STRING("エルモスA"))
    SPAWN_EVENT_GET_ID(PTR(fuwafuwa), GW(1))
    SET_VALUE(LW(0), STRING("エルモスB"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 850)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスC"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -25)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 1000)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスD"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -22)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 900)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスE"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -30)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 1050)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスF"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 950)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスG"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -24)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 900)
    SPAWN_EVENT(urouro_fuwafuwa)
    CALL(evt_cam3d_evt_set, 2, 700, 1614, -1, 261, -108, 0, 11)
    CALL(evt_npc_set_position, STRING("マルコ"), 116, 116, -100)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_VOICE1"), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_KORTESS_VOICE1"), 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_14"), 0, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 0, 358, 572, -1, 145, -108, 0, 11)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_15"), 0, 0)
    WAIT_MS(500)
    SET_VALUE(LW(0), STRING("オドオド水夫"))
    CALL(evt_npc_set_anim, LW(0), STRING("R_2"))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 300)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("ガラの悪い水夫"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -25)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 330)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("サンダース"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -30)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 350)
    SPAWN_EVENT_GET_ID(PTR(urouro), GW(2))
    SET_VALUE(LW(0), STRING("めがね水夫"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -22)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 340)
    SPAWN_EVENT_GET_ID(PTR(urouro), GW(3))
    SET_VALUE(LW(0), STRING("店員"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -22)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 300)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("宿店員"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -24)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 340)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("水夫A"))
    SPAWN_EVENT(buruburu)
    SET_VALUE(LW(0), STRING("水夫ボム兵"))
    SPAWN_EVENT(buruburu)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 1536)
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("O_1"))
    CALL(evt_mario_set_pose, STRING("M_I_O"))
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 512)
    CALL(evt_msg_print, 0, STRING("stg5_sea_16"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_msg_print, 0, STRING("stg5_sea_17"), 0, STRING("オドオド水夫"))
    CALL(evt_cam3d_evt_set, 115, 300, 323, 115, 165, -105, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_18"), 0, STRING("マルコ"))
    CALL(elmos1000tai_start)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_APPEAR2"), 0)
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_KORTESS_VOICE1"), 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_19"), 0, 0)
    DELETE_EVENT(GW(1))
    SET_VALUE(GF(10), 1)
    SET_VALUE(GF(11), 1)
    SPAWN_EVENT(elmos_a_kakushuku)
    WAIT_MS(500)
    CALL(evt_npc_set_position, STRING("エルモスA"), 80, 126, -100)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 90)
    WAIT_MS(500)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("マルコ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_NPC_BIKKURI_MARK1"), LW(0), LW(1), LW(2), 0)
        ADD(LW(1), 20)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 0, 0, LW(0), LW(1), LW(2), 30, 0, 90)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_20"), 0, STRING("マルコ"))
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("B_1"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_21"), 0, STRING("マルコ"))
    SET_VALUE(GF(10), 0)
    WAIT_WHILE(GF(11))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_QUAKE1"), LW(10))
        CALL(evt_cam_shake, 4, FLOAT(0.002f), FLOAT(0.0f), 200)
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 200)
        CALL(evt_cam_shake, 4, FLOAT(0.006f), FLOAT(0.0f), 200)
        CALL(evt_cam_shake, 4, FLOAT(0.008f), FLOAT(0.0f), 200)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 0)
    END_INLINE_EVENT
    CALL_CHILD(elmos_a_shuku)
    CALL(evt_npc_set_position, STRING("エルモスA"), 168, 160, -84)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_APPEAR1"), 168, 160, -84, 0)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 270)
    WAIT_MS(300)
    CALL_CHILD(elmos_a_kaku)
    SET_VALUE(LW(0), STRING("エルモスA"))
    SPAWN_EVENT(fuwafuwa)
    SET_VALUE(LW(0), STRING("マルコ"))
    CALL(evt_npc_set_anim, LW(0), STRING("R_2"))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_flag_onoff, 1, LW(0), 1073741856)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 340)
    SPAWN_EVENT_GET_ID(PTR(urouro), GW(0))
    CALL(evt_cam3d_evt_set, 0, 358, 572, -1, 145, -108, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_22"), 0, STRING("サンダース"))
    CALL(evt_msg_print, 0, STRING("stg5_sea_23"), 0, STRING("サンダース"))
    CALL(evt_msg_print, 0, STRING("stg5_sea_24"), 0, STRING("サンダース"))
    DELETE_EVENT(GW(2))
    SET_VALUE(LW(10), STRING("サンダース"))
    SPAWN_EVENT(kirimomi)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_npc_get_position, STRING("サンダース"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_FALL1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("サンダース"), -20, -100, -300, 600, 100)
    CALL(evt_eff, 0, STRING("splash"), 0, -20, 30, -150, FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_25"), 0, STRING("めがね水夫"))
    DELETE_EVENT(GW(3))
    SET_VALUE(LW(10), STRING("めがね水夫"))
    SPAWN_EVENT(kirimomi)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_npc_get_position, STRING("めがね水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_FALL1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("めがね水夫"), -30, -100, 100, 600, 100)
    CALL(evt_eff, 0, STRING("splash"), 0, -30, 30, 0, FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_26"), 0, STRING("マルコ"))
    BEGIN_INLINE_EVENT
        DELETE_EVENT(GW(0))
        CALL(evt_npc_set_ry, STRING("マルコ"), 270)
        CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 20, 90, -90, 500, 50)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_mapobj_get_position, STRING("S_a1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_SINK1"), LW(0), LW(1), LW(2), LW(10))
    SPAWN_EVENT(marcoship_shizumu)
    WAIT_MS(4500)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_bgmoff, 512)
    SET_VALUE(GSW(0), 231)
    CALL(evt_fade_set_mapchange_type, 0, 10, 0, 9, 300)
    CALL(evt_bero_mapchange, STRING("muj_00"), STRING(""))
    RETURN
EVT_END()

// TODO: write splash_eff (global, 0x98 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(muj_20_koopa_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_npc_set_position, STRING("飛クッパ"), 0, 500, -700)
    CALL(evt_npc_set_rotate, STRING("飛クッパ"), 0, 0, 180)
    CALL(evt_npc_set_anim, STRING("飛クッパ"), STRING("HKPA_O_3"))
    CALL(evt_cam3d_evt_set, 0, 322, 480, 0, 218, -113, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    SET_VALUE(GF(0), 0)
    CALL(evt_snd_sfxon, STRING("SFX_KUPPA_AIRSHIP_FALL1"), GW(0))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("飛クッパ"), LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(1), 50)
                CALL(evt_snd_sfxoff, GW(0))
                CALL(evt_snd_sfxon, STRING("SFX_KUPPA_AIRSHIP_DOWN_SEA1"), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("飛クッパ"), LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(splash_eff, LW(0), LW(1), LW(2))
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 1, 500, -100, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_npc_set_position, STRING("飛クッパ"), 0, LW(0), -700)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GF(0), 1)
    WAIT_MS(2000)
    SET_VALUE(GSW(0), 216)
    CALL(evt_bero_mapchange, STRING("kpa_03"), STRING(""))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kaizoku_hata)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, -4000, 4000, 2500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_b_02_hata"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(500)
        CALL(evt_sub_intpl_msec_init, 11, 4000, -4000, 2500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_b_02_hata"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(500)
    END_LOOP
    RETURN
EVT_END()

// TODO: write kaizoku_fune_eff (global, 0x1d4 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(gor_muj_ship)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 0, 600, 2155, 0, 350, -108, 0, 11)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 0, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_umi_l_siro_b"), 1)
    BEGIN_INLINE_EVENT
        CALL(kaizoku_fune_eff)
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    SPAWN_EVENT(kaizoku_hata)
    BEGIN_INLINE_EVENT
        WAIT_MS(3500)
        CALL_CHILD(hiru_yoru_event)
        WAIT_MS(3500)
        CALL_CHILD(yoru_hiru_event)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_key_get_buttontrg, 0, LW(0))
            IF_INT_EQUAL(LW(0), 256)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_bero_mapchange, STRING("muj_00"), STRING(""))
    END_INLINE_EVENT
    SET_RAW_VALUE(LW(0), GW(1))
    SET_FLOAT_VALUE(LW(1), FLOAT(5.0f))
    SPAWN_EVENT(muj_ship_puka)
    CALL(evt_sub_intpl_msec_init, 0, -1000, 1000, 15000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), GW(1), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(0), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_bero_mapchange, STRING("muj_00"), STRING(""))
    RETURN
EVT_END()

static EVT_BEGIN(muj_gor_ship)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 0, 600, 2155, 0, 350, -108, 0, 11)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 180, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 180, 0)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_umi_l_siro_b"), 1)
    BEGIN_INLINE_EVENT
        CALL(kaizoku_fune_eff)
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    SPAWN_EVENT(kaizoku_hata)
    BEGIN_INLINE_EVENT
        WAIT_MS(3500)
        CALL_CHILD(hiru_yoru_event)
        WAIT_MS(3500)
        CALL_CHILD(yoru_hiru_event)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_key_get_buttontrg, 0, LW(0))
            IF_INT_EQUAL(LW(0), 256)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_bero_mapchange, STRING("gor_00"), STRING(""))
    END_INLINE_EVENT
    SET_RAW_VALUE(LW(0), GW(1))
    SET_FLOAT_VALUE(LW(1), FLOAT(5.0f))
    SPAWN_EVENT(muj_ship_puka)
    CALL(evt_sub_intpl_msec_init, 0, 1000, -1000, 15000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), GW(1), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 180, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(0), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 180, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_bero_mapchange, STRING("gor_00"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(anm_kaji)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, 500, -500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_f_drin"), LW(0), 0, 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, -500, 500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_f_drin"), LW(0), 0, 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(anm_wakka)
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), FLOAT(2.0f))
        MULTIPLY_FLOAT(LW(0), GW(10))
        ADD_FLOAT(LW(1), LW(0))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_wa_l"), 0, 0, LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_wa_r"), 0, 0, LW(1))
        IF_FLOAT_GREATER(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(anm_entotsu)
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), FLOAT(2.0f))
        MULTIPLY_FLOAT(LW(0), GW(10))
        ADD_FLOAT(LW(1), LW(0))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_en01"), 0, LW(1), 0)
        IF_FLOAT_GREATER(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(anm_neji)
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), FLOAT(2.0f))
        MULTIPLY_FLOAT(LW(0), GW(10))
        ADD_FLOAT(LW(1), LW(0))
        CALL(evt_mapobj_rotate, 1, STRING("S_skr"), LW(1), 0, 0)
        IF_FLOAT_GREATER(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: write unk_muj_00004328 (global, 0x1e8 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(anm_yure)
    CALL(evt_mapobj_trans, 1, STRING("S_fune_m"), GW(0), GW(1), GW(2))
    CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), GW(0), GW(1), GW(2))
    WAIT_MS(1500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_OPN_SHIP_MOVE1"), GW(0), GW(1), GW(2), GW(4))
    CALL(evt_snd_sfx_dist, GW(4), 60)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, 500, -500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(unk_muj_00004328)
            CALL(evt_mapobj_trans, 1, STRING("S_fune_m"), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_rotate, 1, STRING("S_fune_m"), 0, 180, LW(0))
            CALL(evt_snd_sfx_pos, GW(4), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(7), LW(8), LW(9))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, -500, 500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(unk_muj_00004328)
            CALL(evt_mapobj_trans, 1, STRING("S_fune_m"), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_rotate, 1, STRING("S_fune_m"), 0, 180, LW(0))
            CALL(evt_snd_sfx_pos, GW(4), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(7), LW(8), LW(9))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

// TODO: write logo (global, 0x2e4 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_prologue)
    WAIT_MS(500)
    CALL(evt_snd_bgmon, 512, STRING("BGM_OPENING1"))
    CALL(evt_snd_envon, 272, STRING("ENV_OPN_MUJ1"))
    WAIT_MS(3500)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    BEGIN_INLINE_EVENT
        CALL(logo)
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(-1800.0f))
    CALL(evt_sub_intpl_msec_init, 11, -1800, 0, 10000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(3), LW(0))
        SUBTRACT_FLOAT(LW(3), LW(2))
        SET_FLOAT_VALUE(GW(10), LW(3))
        SET_FLOAT_VALUE(LW(2), LW(0))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(5)
            WAIT_MS(1000)
            IF_INT_NOT_EQUAL(GF(1), 0)
                RETURN
            END_IF
        END_LOOP
        IF_INT_EQUAL(GF(1), 0)
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_SAILING1"))
        END_IF
    END_INLINE_EVENT
    LOOP(0)
        IF_INT_EQUAL(GF(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxon, STRING("SFX_PRESS_START1"), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_snd_bgmoff_f, 512, 6000)
        CALL(evt_snd_envoff_f, 512, 6000)
        CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
        CALL(evt_bero_mapchange, STRING("sys_01"), STRING("prologue"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(5000)
        CALL(evt_snd_sfxoff, GW(4))
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(0.0f))
    CALL(evt_sub_intpl_msec_init, 11, 0, 1800, 10000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(3), LW(0))
        SUBTRACT_FLOAT(LW(3), LW(2))
        SET_FLOAT_VALUE(GW(10), LW(3))
        SET_FLOAT_VALUE(LW(2), LW(0))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        IF_INT_EQUAL(GF(2), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_npc_entry)
    CALL(evt_npc_entry, STRING("コルテス"), STRING("c_korutesu3"))
    CALL(evt_npc_set_tribe, STRING("コルテス"), STRING("コルテス"))
    CALL(evt_npc_set_position, STRING("コルテス"), FLOAT(-830.0f), FLOAT(147.0f), FLOAT(-98.0f))
    CALL(evt_npc_set_ry, STRING("コルテス"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 32768)
    RETURN
EVT_END()

EVT_BEGIN(fune_move)
    BEGIN_INLINE_EVENT
        CALL(kaizoku_fune_eff)
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_EPILOGUE_KOR_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(10))
    CALL(evt_snd_sfx_dist, LW(10), 30)
    SET_FLOAT_VALUE(LW(0), FLOAT(-700.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        ADD_FLOAT(LW(0), FLOAT(2.0f))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), 0, 0)
        CALL(evt_mapobj_get_position, STRING("S_b"), LW(3), LW(4), LW(5))
        CALL(evt_snd_sfx_pos, LW(10), LW(3), LW(4), LW(5))
        ADD_FLOAT(LW(1), FLOAT(1.0f))
        IF_INT_GREATER_OR_EQUAL(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        CALL(evt_sub_get_sincos, LW(1), LW(2), LW(3))
        MULTIPLY_FLOAT(LW(2), FLOAT(3.0f))
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 0, LW(2))
        CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(0), 0, 0)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(130.0f))
        CALL(evt_npc_set_position, STRING("コルテス"), LW(2), 147, -98)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(45.0f))
        CALL(evt_npc_set_position, STRING("サンダース"), LW(2), 90, -50)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(17.0f))
        CALL(evt_npc_set_position, STRING("水夫A"), LW(2), 90, -169)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(-106.0f))
        CALL(evt_npc_set_position, STRING("水夫ボム兵"), LW(2), 160, -18)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_snd_envon, 272, STRING("ENV_END_MUJ3"))
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_npc_set_position, STRING("サンダース"), FLOAT(-655.0f), FLOAT(90.0f), FLOAT(-50.0f))
    CALL(evt_npc_set_ry, STRING("サンダース"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("サンダース"), 32768)
    CALL(evt_npc_set_position, STRING("水夫A"), FLOAT(-683.0f), FLOAT(90.0f), FLOAT(-169.0f))
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫A"), 32768)
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), FLOAT(-806.0f), FLOAT(160.0f), FLOAT(-18.0f))
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫ボム兵"), 32768)
    CALL(evt_cam3d_evt_set, 25, 305, 1091, 25, 150, -108, 0, 11)
    CALL(evt_seq_wait, 2)
    SPAWN_EVENT(fune_move)
    WAIT_MS(10000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_bero_mapchange, STRING("muj_01"), STRING(""))
    RETURN
EVT_END()

// TODO: write fune_eff (static, 0x2a0 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(muj_20_init_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight1"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight2"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight3"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight4"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight5"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight6"), STRING("S_b"), 1)
    END_INLINE_EVENT
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_02"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_03"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_05"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_02"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_03"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_05"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_m"), 1)
    IF_INT_EQUAL(GSW(0), 215)
        CALL(evt_mario_set_mode, 2)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_umi_l_siro_b"), 1)
        CALL(evt_map_playanim, STRING("nami"), 1, 0)
        CALL(evt_map_playanim, STRING("nami_02"), 1, 0)
        CALL(evt_map_playanim, STRING("fune_01"), 1, 0)
        CALL(evt_map_playanim, STRING("kumo"), 1, 0)
        CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
        SPAWN_EVENT(muj_20_koopa_evt)
        RETURN
    END_IF
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("nami_02"), 1, 0)
    CALL(evt_map_playanim, STRING("fune_01"), 1, 0)
    CALL(evt_map_playanim, STRING("kumo"), 1, 0)
    IF_INT_EQUAL(GSWF(3125), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(gor_muj_ship)
    END_IF
    IF_INT_EQUAL(GSWF(1178), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(muj_gor_ship)
    END_IF
    IF_INT_EQUAL(GSW(0), 229)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_SAILING1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(onryo_event)
    END_IF
    IF_INT_EQUAL(GSW(0), 230)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_SAILING1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(onryo_event)
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("prologue"))
        CALL(evt_cam3d_evt_set, 0, 367, 2093, 0, 343, -115, 0, 11)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_set_pos, 0, -1000, 0)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai03_sun"), 0, -260, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_umi_l_siro_b"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_m"), 1)
        BEGIN_INLINE_EVENT
            CALL(fune_eff)
        END_INLINE_EVENT
        SET_FLOAT_VALUE(GW(0), FLOAT(-2000.0f))
        SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
        SET_FLOAT_VALUE(GW(2), FLOAT(-3000.0f))
        SPAWN_EVENT(anm_kaji)
        SPAWN_EVENT(anm_wakka)
        SPAWN_EVENT(anm_entotsu)
        SPAWN_EVENT(anm_neji)
        SPAWN_EVENT(anm_yure)
        SPAWN_EVENT(evt_prologue)
    END_IF
    IF_INT_EQUAL(GSW(0), 402)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
        CALL(evt_hitobj_onoff, STRING("A_a1"), 1, 0)
        CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
        CALL(evt_mario_kill_party, 0)
        CALL_CHILD(epilogue_npc_entry)
        SPAWN_EVENT(epilogue_evt)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 _ap[1] = {
    0x00000000,
};

// TODO: type
u32 mapAnimRate[4] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 marcoship[1] = {
    0x00000000,
};

// TODO: write draw (static, 0x1ac bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write pressStartGX (static, 0x4d8 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it

// TODO: write elmos1000tai_seq00 (global, 0x150 bytes) here:
// its asm is in build/G8MJ01/muj/asm/muj_20.s; this is where the original has it, just before the first script that uses it
// clang-format on

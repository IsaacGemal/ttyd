#include "tik_05.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/imgdrv.h"
#include "driver/itemdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_mahojin.h"
#include "eff/eff_itemget.h"
#include "eff/eff_mist2.h"
#include "eff/eff_starstone.h"
#include "eff/eff_treasure_map.h"
#include "eff/n64/eff_crystal_n64.h"
#include "eff/n64/eff_pokopi_pcharge_n64.h"
#include "eff/n64/eff_stardust2_n64.h"
#include "eff/n64/eff_stardust_n64.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "mario/mario_cam.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "math_ext.h"
#include "math_trig.h"

// LOCAL_PROTO: inline MSL sqrtf; the SDK math.h drags in an overloaded sqrt.
#define FP_NAN 1
#define NAN (*(f32*)__float_nan)
static inline int tik_fpclassifyf(float value) {
    unsigned long integer = *(unsigned long*)&value;
    switch (integer & 0x7f800000) {
    case 0x7f800000:
        if ((integer & 0x7fffff) != 0) {
            return FP_NAN;
        }
        return 2;
    case 0:
        if ((integer & 0x7fffff) != 0) {
            return 5;
        }
        return 3;
    }
    return 4;
}

static inline f32 sqrtf(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
#ifdef MUST_MATCH
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        x = x * (0.5 * guess * (3.0 - guess * guess * x));
        return x;
#else
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return x * guess;
#endif
    } else if (x < 0.0) {
        return NAN;
    } else if (tik_fpclassifyf(x) == FP_NAN) {
        return NAN;
    }
    return x;
}
#include "memory.h"
#include "party/party_motion.h"
#include "system.h"
#include "dolphin/exi.h"
#include "dolphin/gx.h"
#include "dolphin/gx/GXVert.h"
#include "dolphin/mtx.h"
#include "dolphin/os.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(evtflash);
static USER_FUNC(cam_kaiten_set_para);
static USER_FUNC(cam_kaiten_get_para);
static USER_FUNC(cam_kaiten_set_direct);
static USER_FUNC(cam_kaiten_init);
static USER_FUNC(alpha_set_func);
static USER_FUNC(alpha_init_func);
static void alpha_tev(MapTevCallback* param);
static USER_FUNC(starstone_set_ry);
s32 look_large_door[25];
extern BrokenWork broken_work;

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

static EVT_BEGIN(n_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("door_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("door_a"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("door_b"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("door_a"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[4] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_06", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, look_large_door, -1, NULL, "tik_04", "w_bero", 1, 1},
    {"n_bero", 1, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_00", "w_bero", 5, 5, n_door_close, n_door_open},
    {NULL},
};

static EVT_BEGIN(kurihakase_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(378, 379)
        CALL(evt_npc_set_position, STRING("me"), -30, 23, -190)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CASE_BETWEEN(380, 387)
        CALL(evt_npc_set_position, STRING("me"), -30, 23, -170)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(kurihakase_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(378, 379)
        CALL(evt_msg_print, 0, STRING("mac_7_008"), 0, STRING("me"))
        CASE_BETWEEN(380, 387)
        CALL(evt_msg_print, 0, STRING("mac_7_014"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[46] = {
    (u32)"クリハカセ", 0x00000600, 0x00000000, (u32)&kurihakase_init,
    0x00000000, (u32)&kurihakase_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

const u32 zero_tik_00009b58 = 0x00000000;


EVT_BEGIN(mahojin_alpha)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hosi_2"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("hosi_2"), 256)
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    LOOP(0)
        ADD_FLOAT(LW(0), GW(1))
        IF_FLOAT_GREATER_OR_EQUAL(LW(0), FLOAT(255.0f))
            SET_FLOAT_VALUE(LW(0), FLOAT(255.0f))
        END_IF
        IF_FLOAT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        END_IF
        CALL(alpha_set_func, LW(0))
        IF_INT_EQUAL(GF(10), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mapobj_flag_onoff, 0, 0, STRING("hosi_2"), 251658240)
        CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hosi_2"), 256)
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(mahojin_alpha_up)
    SET_VALUE(GW(1), FLOAT(6.0f))
    RETURN
EVT_END()

EVT_BEGIN(mahojin_alpha_down)
    SET_VALUE(GW(1), FLOAT(-6.0f))
    RETURN
EVT_END()

EVT_BEGIN(large_door_stg0_init)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set, -300, 700, 1135, -300, 250, 23, 0, 11)
    RETURN
EVT_END()

EVT_BEGIN(large_door_stg0)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_pos, 620, 81, 40)
    CALL(evt_party_set_pos, 0, 640, 81, 40)
    CALL(evt_party_set_pos, 1, 660, 81, 40)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, 300, 700, 1135, 300, 250, 23, 6000, 0)
    WAIT_MS(6500)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 1, 450, 30, FLOAT(120.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 520, 30, FLOAT(120.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 490, 30, FLOAT(120.0f))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_095_kur"), 0, STRING("extparty"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 390, 260, 11, 44, 147, -528, 1000, 11)
    WAIT_MS(1800)
    CALL(evt_cam3d_evt_set, 523, 154, 254, 448, 108, 22, 0, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_0_096_kur"), 0, STRING("extparty"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    SET_VALUE(GSW(0), 16)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(look_large_door)
    SWITCH(GSW(0))
        CASE_EQUAL(15)
        SPAWN_EVENT(large_door_stg0)
        SET_VALUE(LW(0), 1)
        RETURN
        CASE_EQUAL(378)
        SET_VALUE(GSW(0), 379)
        SET_VALUE(LW(0), 0)
        RETURN
    END_SWITCH
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(kaidan_event)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER(LW(0), -130)
            IF_INT_LESS(LW(0), 130)
                IF_INT_LESS(LW(2), 0)
                    CALL(evt_mario_key_onoff, 0)
                    CALL(evt_mario_normalize)
                    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                    IF_INT_LESS(LW(0), 0)
                        CALL(evt_mario_set_dir, 90, 0, 0)
                    ELSE
                        CALL(evt_mario_set_dir, 270, 0, 0)
                    END_IF
                    WAIT_MS(800)
                    CALL(evt_party_stop, 0)
                    CALL(evt_msg_print_party, STRING("mac_0_097"))
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 17)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_mahojin_move)
    SET_VALUE(GF(10), 1)
    SET_VALUE(GF(0), 1)
    SET_VALUE(GF(1), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 0, -250, FLOAT(60.0f))
        CALL(evt_mario_set_dir, 90, 200, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_force_reset_outofscreen, 0)
        CALL(evt_party_move_pos2, 0, 30, -190, FLOAT(150.0f))
        CALL(evt_party_set_dir, 0, 270, 200)
        SET_VALUE(GF(1), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    WAIT_WHILE(GF(1))
    WAIT_MS(800)
    RETURN
EVT_END()

EVT_BEGIN(evt_mahojin_move2)
    CALL(evt_party_set_pos, 0, 30, 19, -190)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_party_set_pos, 1, -30, 19, -190)
    CALL(evt_party_set_dir, 1, 90, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_fogset)
    CALL(evt_map_get_fog, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 600, 1000, 4000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), LW(5))
        ADD(LW(2), LW(0))
        CALL(evt_map_set_fog, LW(4), LW(2), 2000, LW(7), LW(8), LW(9))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_fogreset)
    CALL(evt_map_get_fog, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    SET_VALUE(GW(2), LW(5))
    SET_VALUE(GW(3), LW(6))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, LW(5), 300, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(GW(2), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, LW(6), 1500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(GW(3), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        LOOP(0)
            CALL(evt_map_set_fog, LW(4), GW(2), GW(3), LW(7), LW(8), LW(9))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(3000)
    DELETE_EVENT(LW(10))
    RETURN
EVT_END()

EVT_BEGIN(mahojin_se_event)
    WAIT_MS(3500)
    WAIT_MS(1600)
    RETURN
EVT_END()

EVT_BEGIN(mahojin_se_event_end)
    RETURN
EVT_END()


EVT_BEGIN(disp_lighteffect)
    LOOP(0)
        CALL(evt_item_get_position, STRING("item00"), LW(0), LW(1), LW(2))
        CALL(disp_lighteffect_func, STRING("map_ball"), LW(0), LW(1), LW(2), GW(10))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(disp_treasure_map)
    LOOP(0)
        CALL(evt_item_get_position, STRING("item00"), LW(0), LW(1), LW(2))
        CALL(disp_treasure_map_func, STRING("map_map"), LW(0), LW(1), LW(2), GW(10), GW(11))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u32 broken_wp[1] = {
    (u32)&broken_work,
};

f32 param_tbl[5] = {
    90.0f, 23.0f, 0.98f, 19.4f,
    2.0f,
};

f32 add_tbl[5] = {
    1.0f, 0.1f, 0.01f, 0.1f,
    1.0f,
};

const u32 dat_tik_00009bb8 = 0xFFFFFF00;

static char* name_tbl[5] = {
    "delay", "v0", "vscale", "pow", "vr",
};

const u32 dat_tik_00009bd8 = 0xFFFFFFFF;
const u32 dat_tik_00009bdc = 0xFFFFFFFF;













EVT_BEGIN(evt_mapwrite)
    WAIT_MS(800)
    IF_INT_EQUAL(GW(15), 0)
        CALL(evt_cam3d_evt_set, 0, 65, 40, 0, 65, -250, 1000, 11)
        WAIT_MS(1000)
        CALL(evt_msg_print_party, STRING("mac_0_098"))
        WAIT_MS(1000)
        DELETE_EVENT(LW(10))
        BEGIN_INLINE_EVENT
            CALL(evt_mario_set_dir, 90, 200, 0)
        END_INLINE_EVENT
    ELSE
        CALL(evt_mario_set_pose, STRING("M_I_N"))
        CALL(evt_cam3d_evt_set, 0, 102, 300, 0, 110, -250, 2000, 11)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 15)
    CALL(evt_item_entry, STRING("item00"), 96, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_item_set_scale, STRING("item00"), FLOAT(0.0f))
    SET_VALUE(GW(11), 0)
    BEGIN_INLINE_EVENT
        CALL(effect_position)
    END_INLINE_EVENT
    WAIT_FRAMES(1)
    IF_INT_EQUAL(GW(15), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_MAP_KAKAGERU2_2"), LW(0), LW(1), LW(2), 0)
    END_IF
    CALL(evt_eff64, STRING("map_wave"), STRING("crystal_n64"), 0, 0, -1000, 0, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_eff64, STRING("map_ball"), STRING("pokopi_pcharge_n64"), 1, 0, -1000, 0, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(200)
    CALL(evt_eff, STRING("map_map"), STRING("treasure_map"), 0, 0, -1000, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_item_get_position, STRING("item00"), LW(0), LW(1), LW(2))
    IF_INT_EQUAL(GW(15), 0)
        CALL(cam_kaiten_init, 0, 65, 40, 0, 65, -250, -24, 102, -800, 0, 110, -250)
        BEGIN_INLINE_EVENT_GET_ID(LW(7))
            LOOP(0)
                CALL(cam_kaiten_set_direct)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LW(10), LF(7))
            SET_FLOAT_VALUE(LW(11), LW(10))
            ADD_FLOAT(LW(11), FLOAT(720.0f))
            SET_VALUE(LW(15), 0)
            LOOP(0)
                CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                CALL(cam_mahojin_kaiten_main, LW(10), LW(11), 14000, LW(6), LW(9))
                SET_FLOAT_VALUE(GW(10), LW(6))
                MULTIPLY_FLOAT(GW(10), FLOAT(-1.0f))
                CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                IF_INT_EQUAL(LW(9), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LF(6), LW(0))
            CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LF(6), LW(1))
            CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 14000)
            LOOP(0)
                CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                CALL(evt_sub_intpl_msec_get_value_para, LW(7), LW(9))
                CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                IF_INT_EQUAL(LW(9), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(cam_kaiten_get_para, 0, LF(0), LW(0), LF(2), LF(3), LF(4), LF(5), LF(6), LF(7))
            CALL(cam_kaiten_get_para, 1, LF(0), LW(1), LF(2), LF(3), LF(4), LF(5), LF(6), LF(8))
            CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 14000)
            LOOP(0)
                CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                CALL(evt_sub_intpl_msec_get_value_para, LW(1), LW(9))
                CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                IF_INT_EQUAL(LW(9), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LW(0), LF(5), LF(6), LF(7))
            CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LW(1), LF(5), LF(6), LF(8))
            CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 14000)
            LOOP(0)
                CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                CALL(evt_sub_intpl_msec_get_value_para, LW(4), LW(9))
                CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
                IF_INT_EQUAL(LW(9), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(2000)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_yuka1"), 1)
            WAIT_MS(3250)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("door"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_yuka1"), 1)
            WAIT_MS(3500)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_yuka1"), 1)
            WAIT_MS(2700)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("door"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_yuka1"), 1)
        END_INLINE_EVENT
    END_IF
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 4000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(11), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_item_get_position, STRING("item00"), LW(0), LW(1), LW(2))
    IF_INT_EQUAL(GW(15), 0)
        CALL(evt_item_move_3d_position, STRING("item00"), LW(0), 115, LW(2), 14000, 11)
    ELSE
        CALL(evt_item_move_3d_position, STRING("item00"), LW(0), 115, LW(2), 9333, 11)
    END_IF
    CALL(evt_item_get_position, STRING("item00"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        ADD(LW(2), 20)
        CALL(evt_eff64, STRING("map_line"), STRING("pokopi_pcharge_n64"), 7, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_eff, 0, STRING("laser"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    WAIT_MS(500)
    CALL(evt_fade_entry, 21, 150, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_fade_entry, 20, 150, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(200)
    CALL(evt_fade_entry, 21, 3200, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(map_disp_set, 0)
    WAIT_FRAMES(1)
    CALL(evt_paper_entry, STRING("OFF_d_tizu_tex"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_tizu_tex"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 4600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(img_alpha, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    SWITCH(GW(15))
        CASE_EQUAL(0)
        SET_VALUE(LW(0), STRING("0_A_1"))
        SET_VALUE(LW(1), STRING("0_A_2"))
        CASE_EQUAL(1)
        SET_VALUE(LW(0), STRING("1_A_1"))
        SET_VALUE(LW(1), STRING("1_A_2"))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("2_A_1"))
        SET_VALUE(LW(1), STRING("2_A_2"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("3_A_1"))
        SET_VALUE(LW(1), STRING("3_A_2"))
        CASE_EQUAL(4)
        SET_VALUE(LW(0), STRING("4_A_1"))
        SET_VALUE(LW(1), STRING("4_A_2"))
        CASE_EQUAL(5)
        SET_VALUE(LW(0), STRING("5_A_1"))
        SET_VALUE(LW(1), STRING("5_A_2"))
        CASE_EQUAL(6)
        SET_VALUE(LW(0), STRING("6_A_1"))
        SET_VALUE(LW(1), STRING("6_A_2"))
        CASE_EQUAL(7)
        SET_VALUE(LW(0), STRING("7_A_1"))
        SET_VALUE(LW(1), STRING("7_A_2"))
    END_SWITCH
    CALL(evt_img_onoff, STRING("img"), 1)
    LOOP(5)
        CALL(evt_img_set_paper_anim, STRING("img"), LW(0))
        CALL(evt_img_wait_animend, STRING("img"))
    END_LOOP
    CALL(evt_img_set_paper_anim, STRING("img"), LW(1))
    CALL(evt_img_wait_animend, STRING("img"))
    WAIT_MS(1000)
    WAIT_MS(1000)
    SWITCH(GW(15))
        CASE_EQUAL(0)
        SET_VALUE(LW(5), 55)
        SET_VALUE(LW(6), 130)
        SET_VALUE(LW(7), -140)
        SET_VALUE(LW(8), STRING("TIZ_stg_1"))
        SET_VALUE(LW(9), STRING("SFX_EVT_MAP_STG1_APPEAR1"))
        CASE_EQUAL(1)
        SET_VALUE(LW(5), 0)
        SET_VALUE(LW(6), 130)
        SET_VALUE(LW(7), -140)
        SET_VALUE(LW(8), STRING("TIZ_stg_2"))
        SET_VALUE(LW(9), STRING("SFX_EVT_MAP_STG2_APPEAR1"))
        CASE_EQUAL(2)
        SET_VALUE(LW(5), 30)
        SET_VALUE(LW(6), 145)
        SET_VALUE(LW(7), -140)
        SET_VALUE(LW(8), STRING("TIZ_stg_3"))
        SET_VALUE(LW(9), STRING("SFX_EVT_MAP_STG3_APPEAR1"))
        CASE_EQUAL(3)
        SET_VALUE(LW(5), -50)
        SET_VALUE(LW(6), 95)
        SET_VALUE(LW(7), -140)
        SET_VALUE(LW(8), STRING("TIZ_stg_4"))
        SET_VALUE(LW(9), STRING("SFX_EVT_MAP_STG4_APPEAR1"))
        CASE_EQUAL(4)
        SET_VALUE(LW(5), 10)
        SET_VALUE(LW(6), 50)
        SET_VALUE(LW(7), -140)
        SET_VALUE(LW(8), STRING("TIZ_stg_5"))
        SET_VALUE(LW(9), STRING("SFX_EVT_MAP_STG5_APPEAR1"))
        CASE_EQUAL(5)
        SET_VALUE(LW(5), 80)
        SET_VALUE(LW(6), 90)
        SET_VALUE(LW(7), -140)
        SET_VALUE(LW(8), STRING("TIZ_stg_6"))
        SET_VALUE(LW(9), STRING("SFX_EVT_MAP_STG6_APPEAR1"))
        CASE_EQUAL(6)
        SET_VALUE(LW(5), -71)
        SET_VALUE(LW(6), 145)
        SET_VALUE(LW(7), -138)
        SET_VALUE(LW(8), STRING("TIZ_stg_7"))
        SET_VALUE(LW(9), STRING("SFX_EVT_MAP_STG7_APPEAR1"))
    END_SWITCH
    ADD(LW(6), -10)
    ADD(LW(7), -20)
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_MAP_NEXT_APPEAR1"), LW(5), LW(6), LW(7), LW(4))
    CALL(evt_snd_sfx_pit, LW(4), 1365)
    CALL(evt_eff64, STRING("tiz_dust2"), STRING("stardust2_n64"), 5, LW(5), LW(6), LW(7), 50, 50, 30, 600, 0, 0, 0, 0)
    CALL(tiz_stone, 0)
    WAIT_MS(500)
    CALL(evt_npc_entry, STRING("tiz_obj"), LW(8))
    ADD(LW(6), 10)
    CALL(evt_npc_set_position, STRING("tiz_obj"), LW(5), LW(6), LW(7))
    CALL(evt_npc_set_scale, STRING("tiz_obj"), FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.5f))
    CALL(evt_npc_set_camid, STRING("tiz_obj"), 7)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_color, STRING("tiz_obj"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_anim, STRING("tiz_obj"), STRING("A_1"))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_snd_sfxon__, LW(9), 0)
    CALL(evt_npc_wait_anim, STRING("tiz_obj"), FLOAT(1.0f))
    ADD(LW(6), 10)
    ADD(LW(7), 20)
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_MAP_NEXT_APPEAR1"), LW(5), LW(6), LW(7), 0)
    CALL(evt_eff64, STRING("tiz_dust"), STRING("stardust_n64"), 0, LW(5), LW(6), LW(7), FLOAT(15.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(tiz_stone, 0)
    WAIT_MS(500)
    CALL(evt_eff, STRING("tiz_stone"), STRING("starstone"), GW(15), LW(5), LW(6), LW(7), FLOAT(0.7f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 10000, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(tiz_stone, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(2000)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_eff_softdelete, STRING("tiz_stone"))
    CALL(evt_npc_delete, STRING("tiz_obj"))
    SWITCH(GW(15))
        CASE_EQUAL(0)
        CALL(evt_snd_sfxon, STRING("SFX_EVT_MAP_KAKAGERU2_3"), 0)
        CASE_OR_EQUAL(1)
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(4)
        CASE_OR_EQUAL(5)
        CASE_OR_EQUAL(6)
        CALL(evt_snd_sfxon, STRING("SFX_EVT_MAP_KAKAGERU1_2"), 0)
        CASE_END
    END_SWITCH
    CALL(map_disp_set, 1)
    WAIT_FRAMES(1)
    CALL(evt_fade_entry, 20, 1000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("indirect"), 0, LW(0), LW(1), LW(2), 120, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(tiz_broken)
    CALL(evt_eff_softdelete, STRING("map_wave"))
    CALL(evt_eff_softdelete, STRING("map_ball"))
    CALL(evt_eff_softdelete, STRING("map_line"))
    CALL(evt_mario_set_pose, STRING("M_P_2"))
    CALL(evt_item_get_position, STRING("item00"), LW(0), LW(1), LW(2))
    ADD(LW(2), -10)
    CALL(evt_eff, STRING("map_itemget"), STRING("itemget"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_MAP_KAKAGERU1_3"), 0)
    CALL(map_bgmode, STRING("map_itemget"), STRING("map_map"))
    WAIT_MS(500)
    SWITCH(GW(15))
        CASE_EQUAL(0)
        CALL(evt_msg_print, 0, STRING("mac_0_099"), 0, 0)
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("mac_1_001_01"), 0, 0)
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("mac_2_001_01"), 0, 0)
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("mac_3_000_01"), 0, 0)
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("mac_4_000_01"), 0, 0)
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("mac_5_000_01"), 0, 0)
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("mac_6_000_01"), 0, 0)
    END_SWITCH
    CALL(map_bgmode_end, STRING("map_itemget"), STRING("map_map"))
    CALL(evt_eff_softdelete, STRING("map_itemget"))
    SET_VALUE(GF(11), 1)
    CALL(evt_eff_softdelete, STRING("mahojin"))
    SPAWN_EVENT(mahojin_se_event_end)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(GW(11), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    SPAWN_EVENT(evt_fogreset)
    IF_INT_NOT_EQUAL(GW(15), 0)
        CALL(evt_cam3d_evt_set, 0, 75, 50, 0, 80, -250, 1000, 11)
    END_IF
    BEGIN_INLINE_EVENT
        WAIT_MS(1700)
        CALL(evt_mario_set_pose, STRING("M_L_3"))
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_item_move_3d_position, STRING("item00"), LW(0), LW(1), LW(2), 2000, 11)
    CALL(evt_item_delete, STRING("item00"))
    CALL(evt_eff_delete, STRING("map_map"))
    WAIT_MS(500)
    IF_INT_NOT_EQUAL(GW(15), 0)
        CALL(evt_party_jump_pos, 0, 30, 28, -250, 300, 0, -1000)
        WAIT_MS(1000)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(fake_fade)
    CALL(evt_cam3d_evt_set, 0, 225, 469, 0, 100, -270, 2000, 11)
    WAIT_MS(1200)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_fade_set_mapchange_type, 1, 10, 0, 1, 500)
    RETURN
EVT_END()



EVT_BEGIN(evt_starstone_up)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_MAP_KAKAGERU1"), LW(4), LW(5), LW(6), 0)
    CALL(evt_mario_set_pose, STRING("M_P_2"))
    CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
    ADD(LW(5), 60)
    ADD(LW(6), -20)
    CALL(evt_eff, STRING("itemget"), STRING("itemget"), 9, LW(4), LW(5), LW(6), 0, 0, 0, 0, 0, 0, 0, 0)
    SET_VALUE(LW(0), GW(15))
    SUBTRACT(LW(0), 1)
    ADD(LW(6), 5)
    CALL(evt_eff, STRING("stone"), STRING("starstone"), LW(0), LW(4), LW(5), LW(6), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone"))
    WAIT_MS(1000)
    CALL(evt_eff_softdelete, STRING("itemget"))
    CALL(starstone_get_position, STRING("stone"), LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_softdelete, STRING("stone"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    RETURN
EVT_END()





EVT_BEGIN(evt_starstone_set)
    WAIT_MS(7000)
    SET_VALUE(LW(0), GW(15))
    SUBTRACT(LW(0), 1)
    CALL(evt_eff, STRING("stone"), STRING("starstone"), LW(0), 0, -1000, 0, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(starstone_setting, STRING("stone"), LW(0))
    CALL(eff_starstone_sound_off, STRING("stone"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(starstone_circle_link_func, STRING("stone"), GW(15), LW(0), 50, LW(2), 0, FLOAT(140.0f))
    CALL(starstone_set_alpha, STRING("stone"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(starstone_set_alpha, STRING("stone"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    LOOP(0)
        CALL(starstone_set_ry, STRING("stone"), LW(0))
        ADD_FLOAT(LW(0), FLOAT(4.0f))
        IF_FLOAT_GREATER(LW(0), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
        END_IF
        IF_INT_EQUAL(GF(11), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_eff_softdelete, STRING("stone"))
    RETURN
EVT_END()


EVT_BEGIN(mahojin_event)
    SET_VALUE(GW(15), 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    SET_VALUE(GF(10), 1)
    CALL(evt_mario_mov_pos2, 0, -250, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 0, 0)
    WAIT_MS(500)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    CALL(evtflash, 20, 300, 80)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 0, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_MAP_KAKAGERU2"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL(evt_mario_set_pose, STRING("M_I_N"))
    CALL(evt_party_set_pos, 1, -30, 19, -190)
    CALL(evt_party_set_pos, 0, 30, 19, -190)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        LOOP(0)
            CALL(evt_mario_set_dir, 90, 200, 0)
            CALL(evt_sub_random, 500, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
            CALL(evt_mario_set_dir, 270, 200, 0)
            CALL(evt_sub_random, 500, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        LOOP(0)
            CALL(evt_party_set_dir, 0, 90, -1)
            CALL(evt_sub_random, 500, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
            CALL(evt_party_set_dir, 0, 270, -1)
            CALL(evt_sub_random, 500, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(12))
        LOOP(0)
            CALL(evt_party_set_dir, 1, 90, -1)
            CALL(evt_sub_random, 500, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
            CALL(evt_party_set_dir, 1, 270, -1)
            CALL(evt_sub_random, 500, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
        END_LOOP
    END_INLINE_EVENT
    SPAWN_EVENT(evt_fogset)
    WAIT_MS(6000)
    DELETE_EVENT(LW(11))
    DELETE_EVENT(LW(12))
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, -1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 1, 90, -1)
    END_INLINE_EVENT
    CALL_CHILD(evt_mapwrite)
    BEGIN_INLINE_EVENT
        CALL(evt_party_jump_pos, 0, 30, 28, -250, 300, 0, -1000)
    END_INLINE_EVENT
    CALL(evt_party_jump_pos, 1, -30, 28, -250, 300, 0, -1000)
    CALL(evt_cam3d_evt_set, 0, 100, -2, 0, 65, -250, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_100"))
    CALL(evt_msg_print, 0, STRING("mac_0_101"), 0, STRING("extparty"))
    CALL(evt_cam3d_evt_set, 0, 60, 447, 0, 120, -250, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_101_04"), 0, STRING("extparty"))
    CALL(evt_mario_set_pose, STRING("M_Z_1"))
    WAIT_MS(500)
    CALL(evt_mario_set_dir_npc, STRING("クリハカセ"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    CALL(evt_pouch_get_starstone, 0)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff_f, 512, 1000)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_fade_set_mapchange_type, 1, 10, 0, 1, 500)
    SET_VALUE(GSW(0), 18)
    CALL(evt_bero_mapchange, STRING("gor_02"), 0)
    RETURN
EVT_END()

EVT_BEGIN(mahojin_event1)
    SET_VALUE(GW(15), 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL_CHILD(evt_mahojin_move)
    CALL(evt_msg_print_party, STRING("mac_1_001"))
    CALL_CHILD(evt_starstone_up)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 1, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    SET_VALUE(GF(11), 0)
    SPAWN_EVENT(evt_starstone_set)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL_CHILD(evt_mahojin_move2)
    SPAWN_EVENT(evt_fogset)
    WAIT_MS(6000)
    CALL_CHILD(evt_mapwrite)
    CALL(evt_msg_print_party, STRING("mac_1_002"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1200)
    CALL(evt_msg_print_party, STRING("mac_1_003"))
    SET_VALUE(GSW(0), 64)
    CALL_CHILD(fake_fade)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff_f, 512, 1000)
    CALL(evt_bero_mapchange, STRING("gor_02"), 0)
    RETURN
EVT_END()

EVT_BEGIN(mahojin_event2)
    SET_VALUE(GW(15), 2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL_CHILD(evt_mahojin_move)
    CALL(evt_msg_print_party, STRING("mac_2_001"))
    CALL_CHILD(evt_starstone_up)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 2, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    SET_VALUE(GF(11), 0)
    SPAWN_EVENT(evt_starstone_set)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL_CHILD(evt_mahojin_move2)
    SPAWN_EVENT(evt_fogset)
    WAIT_MS(6000)
    CALL_CHILD(evt_mapwrite)
    CALL(evt_msg_print_party, STRING("mac_2_002"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_THINK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_Z"))
    SET_VALUE(GSW(0), 120)
    CALL_CHILD(fake_fade)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff_f, 512, 1000)
    CALL(evt_bero_mapchange, STRING("gor_02"), 0)
    RETURN
EVT_END()

EVT_BEGIN(mahojin_event3)
    SET_VALUE(GW(15), 3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL_CHILD(evt_mahojin_move)
    CALL_CHILD(evt_starstone_up)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 3, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    SET_VALUE(GF(11), 0)
    SPAWN_EVENT(evt_starstone_set)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL_CHILD(evt_mahojin_move2)
    SPAWN_EVENT(evt_fogset)
    WAIT_MS(6000)
    CALL_CHILD(evt_mapwrite)
    CALL(evt_msg_print_party, STRING("mac_3_001"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    SET_VALUE(GSW(0), 174)
    CALL_CHILD(fake_fade)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff_f, 512, 1000)
    CALL(evt_bero_mapchange, STRING("gor_02"), 0)
    RETURN
EVT_END()

EVT_BEGIN(mahojin_event4)
    SET_VALUE(GW(15), 4)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL_CHILD(evt_mahojin_move)
    CALL_CHILD(evt_starstone_up)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 4, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    SET_VALUE(GF(11), 0)
    SPAWN_EVENT(evt_starstone_set)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL_CHILD(evt_mahojin_move2)
    SPAWN_EVENT(evt_fogset)
    WAIT_MS(6000)
    CALL_CHILD(evt_mapwrite)
    CALL(evt_msg_print_party, STRING("mac_4_001"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    SET_VALUE(GSW(0), 221)
    CALL_CHILD(fake_fade)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff_f, 512, 1000)
    CALL(evt_bero_mapchange, STRING("gor_02"), 0)
    RETURN
EVT_END()

EVT_BEGIN(mahojin_event5)
    SET_VALUE(GW(15), 5)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL_CHILD(evt_mahojin_move)
    CALL_CHILD(evt_starstone_up)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 5, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    SET_VALUE(GF(11), 0)
    SPAWN_EVENT(evt_starstone_set)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL_CHILD(evt_mahojin_move2)
    SPAWN_EVENT(evt_fogset)
    WAIT_MS(6000)
    CALL_CHILD(evt_mapwrite)
    CALL(evt_msg_print_party, STRING("mac_5_001"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    SET_VALUE(GSW(0), 271)
    CALL_CHILD(fake_fade)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff_f, 512, 1000)
    CALL(evt_bero_mapchange, STRING("gor_02"), 0)
    RETURN
EVT_END()

EVT_BEGIN(mahojin_event6)
    SET_VALUE(GW(15), 6)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL_CHILD(evt_mahojin_move)
    CALL_CHILD(evt_starstone_up)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 6, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    SET_VALUE(GF(11), 0)
    SPAWN_EVENT(evt_starstone_set)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL_CHILD(evt_mahojin_move2)
    SPAWN_EVENT(evt_fogset)
    WAIT_MS(6000)
    CALL_CHILD(evt_mapwrite)
    CALL(evt_msg_print_party, STRING("mac_6_001"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_THINK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    SET_VALUE(GSW(0), 347)
    CALL_CHILD(fake_fade)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff_f, 512, 1000)
    CALL(evt_bero_mapchange, STRING("gor_02"), 0)
    RETURN
EVT_END()

EVT_BEGIN(starstone_circle_link)
    LOOP(0)
        CALL(starstone_circle_link_func, STRING("stone01"), 1, LW(0), GW(10), LW(2), GW(11), GW(12))
        CALL(starstone_circle_link_func, STRING("stone02"), 2, LW(0), GW(10), LW(2), GW(11), GW(12))
        CALL(starstone_circle_link_func, STRING("stone03"), 3, LW(0), GW(10), LW(2), GW(11), GW(12))
        CALL(starstone_circle_link_func, STRING("stone04"), 4, LW(0), GW(10), LW(2), GW(11), GW(12))
        CALL(starstone_circle_link_func, STRING("stone05"), 5, LW(0), GW(10), LW(2), GW(11), GW(12))
        CALL(starstone_circle_link_func, STRING("stone06"), 6, LW(0), GW(10), LW(2), GW(11), GW(12))
        CALL(starstone_circle_link_func, STRING("stone07"), 7, LW(0), GW(10), LW(2), GW(11), GW(12))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(lastdungeon_anim_init)
    CALL(evt_map_playanim, STRING("anm_hibi"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_hosi_1"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_hosi_2"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_hosi_3"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_hosi_4"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_hosi_5"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_hosi_6"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_hosi_7"), 2, 0)
    CALL(evt_map_playanim, STRING("anm_tobira"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_hibi"), 2, 0)
    RETURN
EVT_END()

EVT_BEGIN(lastdungeon_init)
    CALL(evt_hit_bind_mapobj, STRING("a_door_a"), STRING("door_a"))
    CALL(evt_hit_bind_mapobj, STRING("a_door_b"), STRING("door_b"))
    CALL(evt_mapobj_rotate, 0, STRING("door_b"), 0, 120, 0)
    CALL(evt_mapobj_rotate, 0, STRING("door_a"), 0, -120, 0)
    CALL(evt_hit_bind_update, STRING("a_door_a"))
    CALL(evt_hit_bind_update, STRING("a_door_b"))
    CALL(evt_hitobj_attr_onoff, 0, 1, LW(3), 8388612)
    RETURN
EVT_END()

EVT_BEGIN(lastdungeon)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(2), -50)
        CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(120.0f))
        CALL(evt_mario_dispflag_onoff, 1, 1)
        CALL(evt_party_dispflg_onoff, 0, 1, 1)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_EVT_MAP_KAKAGERU3_3"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), 5000)
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 200)
        WAIT_MS(200)
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 200)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 120, 0, 5000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 0, STRING("door_b"), 0, LW(0), 0)
        MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
        CALL(evt_mapobj_rotate, 0, STRING("door_a"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(2000)
    CALL(evt_bero_mapchange, STRING("sys_01"), STRING("stg_8"))
    RETURN
EVT_END()








EVT_BEGIN(mahojin_event7)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_mov_pos2, 0, -250, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(800)
    CALL(evt_cam_letter_box_disable, 0)
    SET_VALUE(GF(10), 1)
    CALL(evt_msg_print, 0, STRING("mac_7_009"), 0, STRING("クリハカセ"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_MAP_KAKAGERU3_1"), 0)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 8, 0, 28, -250, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(mahojin_se_event)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL(evt_party_set_pos, 0, 30, 25, -190)
    CALL(evt_party_set_dir, 0, 270, 0)
    WAIT_MS(2000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING("map_wave"), STRING("crystal_n64"), 6, LW(0), LW(1), LW(2), FLOAT(1.0f), 60, 0, 0, 0, 0, 0, 0)
    CALL(crystal_bgmode, STRING("map_wave"))
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 65, 40, 0, 65, -250, 1000, 11)
    WAIT_MS(2000)
    CALL(evt_mario_dispflag_onoff, 1, 4)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING("charge"), STRING("pokopi_pcharge_n64"), 8, LW(0), LW(1), LW(2), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("stone01"), STRING("starstone"), 0, 0, 50, -390, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone01"))
    CALL(evt_eff, STRING("stone02"), STRING("starstone"), 1, 112, 50, -340, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone02"))
    CALL(evt_eff, STRING("stone03"), STRING("starstone"), 2, 140, 50, -220, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone03"))
    CALL(evt_eff, STRING("stone04"), STRING("starstone"), 3, 62, 50, -120, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone04"))
    CALL(evt_eff, STRING("stone05"), STRING("starstone"), 4, -62, 50, -120, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone05"))
    CALL(evt_eff, STRING("stone06"), STRING("starstone"), 5, -140, 50, -220, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone06"))
    CALL(evt_eff, STRING("stone07"), STRING("starstone"), 6, -112, 50, -340, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(eff_starstone_sound_off, STRING("stone07"))
    CALL(evt_mario_get_pos, 0, LW(0), GW(10), LW(2))
    SET_FLOAT_VALUE(GW(11), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(12), FLOAT(40.0f))
    SPAWN_EVENT_GET_ID(PTR(starstone_circle_link), LW(8))
    CALL(starstone_set_alpha, STRING("stone01"), 0)
    CALL(starstone_set_alpha, STRING("stone02"), 0)
    CALL(starstone_set_alpha, STRING("stone03"), 0)
    CALL(starstone_set_alpha, STRING("stone04"), 0)
    CALL(starstone_set_alpha, STRING("stone05"), 0)
    CALL(starstone_set_alpha, STRING("stone06"), 0)
    CALL(starstone_set_alpha, STRING("stone07"), 0)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(starstone_set_ry, STRING("stone01"), LW(0))
            CALL(starstone_set_ry, STRING("stone02"), LW(0))
            CALL(starstone_set_ry, STRING("stone03"), LW(0))
            CALL(starstone_set_ry, STRING("stone04"), LW(0))
            CALL(starstone_set_ry, STRING("stone05"), LW(0))
            CALL(starstone_set_ry, STRING("stone06"), LW(0))
            CALL(starstone_set_ry, STRING("stone07"), LW(0))
            ADD_FLOAT(LW(0), FLOAT(2.0f))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(5500)
        SET_VALUE(LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            IF_INT_GREATER_OR_EQUAL(LW(0), 128)
                IF_INT_EQUAL(LW(2), 0)
                    CALL(evt_mario_set_pose, STRING("M_P_2"))
                    SET_VALUE(LW(2), 1)
                END_IF
            END_IF
            CALL(starstone_set_alpha, STRING("stone01"), LW(0))
            CALL(starstone_set_alpha, STRING("stone02"), LW(0))
            CALL(starstone_set_alpha, STRING("stone03"), LW(0))
            CALL(starstone_set_alpha, STRING("stone04"), LW(0))
            CALL(starstone_set_alpha, STRING("stone05"), LW(0))
            CALL(starstone_set_alpha, STRING("stone06"), LW(0))
            CALL(starstone_set_alpha, STRING("stone07"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(cam_mahojin_kaiten_main, 1080, 0, 14000, GW(11), LW(9))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        CALL(evt_sub_intpl_msec_init, 11, 45, 115, 14000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(10), LW(1))
            CALL(pcharge_set_position, STRING("charge"), LW(4), GW(10), LW(6))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_eff_softdelete, STRING("charge"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(7500)
        CALL(evt_sub_intpl_msec_init, 11, 40, 140, 4500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(12), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(cam_kaiten_init, 0, 65, 40, 0, 65, -250, 0, 102, 300, 0, 110, -250)
    BEGIN_INLINE_EVENT_GET_ID(LW(7))
        LOOP(0)
            CALL(cam_kaiten_set_direct)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LW(10), LF(7))
        CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LW(11), LF(7))
        ADD_FLOAT(LW(11), FLOAT(720.0f))
        SET_VALUE(LW(15), 0)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(cam_mahojin_kaiten_main, LW(10), LW(11), 14000, LW(6), LW(9))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LF(6), LW(0))
        CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LF(6), LW(1))
        CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 14000)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(evt_sub_intpl_msec_get_value_para, LW(7), LW(9))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LW(0), LF(2), LF(3), LF(4), LF(5), LF(6), LF(7))
        CALL(cam_kaiten_get_para, 1, LF(0), LW(1), LF(2), LF(3), LF(4), LF(5), LF(6), LF(8))
        CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 14000)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(evt_sub_intpl_msec_get_value_para, LW(1), LW(9))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LW(0), LF(5), LF(6), LF(7))
        CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LW(1), LF(5), LF(6), LF(8))
        CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 14000)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(evt_sub_intpl_msec_get_value_para, LW(4), LW(9))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_yuka1"), 1)
        WAIT_MS(3250)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("door"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_yuka1"), 1)
        WAIT_MS(3500)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_yuka1"), 1)
        WAIT_MS(2700)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("door"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_yuka1"), 1)
    END_INLINE_EVENT
    WAIT_MS(14000)
    CALL(evt_mario_dispflag_onoff, 0, 4)
    DELETE_EVENT(LW(7))
    DELETE_EVENT(LW(8))
    WAIT_MS(1000)
    CALL(starstone_get_position, STRING("stone01"), LW(0), LW(1), LW(2))
    CALL(starstone_set_position, STRING("stone01"), 0, -1000, 0)
    CALL(starstone_set_alpha, STRING("stone01"), 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(650)
    CALL(starstone_get_position, STRING("stone02"), LW(0), LW(1), LW(2))
    CALL(starstone_set_position, STRING("stone02"), 0, -1000, 0)
    CALL(starstone_set_alpha, STRING("stone02"), 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(650)
    CALL(starstone_get_position, STRING("stone03"), LW(0), LW(1), LW(2))
    CALL(starstone_set_position, STRING("stone03"), 0, -1000, 0)
    CALL(starstone_set_alpha, STRING("stone03"), 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(650)
    CALL(starstone_get_position, STRING("stone04"), LW(0), LW(1), LW(2))
    CALL(starstone_set_position, STRING("stone04"), 0, -1000, 0)
    CALL(starstone_set_alpha, STRING("stone04"), 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(650)
    CALL(starstone_get_position, STRING("stone05"), LW(0), LW(1), LW(2))
    CALL(starstone_set_position, STRING("stone05"), 0, -1000, 0)
    CALL(starstone_set_alpha, STRING("stone05"), 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(650)
    CALL(starstone_get_position, STRING("stone06"), LW(0), LW(1), LW(2))
    CALL(starstone_set_position, STRING("stone06"), 0, -1000, 0)
    CALL(starstone_set_alpha, STRING("stone06"), 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(650)
    CALL(starstone_get_position, STRING("stone07"), LW(0), LW(1), LW(2))
    CALL(starstone_set_position, STRING("stone07"), 0, -1000, 0)
    CALL(starstone_set_alpha, STRING("stone07"), 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING(""), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1300)
    CALL(starstone_setting, STRING("stone01"), 0)
    CALL(starstone_setting, STRING("stone02"), 1)
    CALL(starstone_setting, STRING("stone03"), 2)
    CALL(starstone_setting, STRING("stone04"), 3)
    CALL(starstone_setting, STRING("stone05"), 4)
    CALL(starstone_setting, STRING("stone06"), 5)
    CALL(starstone_setting, STRING("stone07"), 6)
    CALL(evt_cam3d_evt_set, 0, 634, 421, 0, 58, -270, 0, 11)
    CALL(evt_mario_set_pose, STRING("M_I_1"))
    CALL(evt_mario_get_pos, 0, LW(0), GW(10), LW(2))
    SET_FLOAT_VALUE(GW(10), FLOAT(50.0f))
    SET_FLOAT_VALUE(GW(11), FLOAT(0.0f))
    SPAWN_EVENT_GET_ID(PTR(starstone_circle_link), LW(8))
    WAIT_MS(1000)
    CALL(starstone_get_position, STRING("stone01"), LW(0), LW(1), LW(2))
    CALL(starstone_set_alpha, STRING("stone01"), 255)
    CALL(evt_eff64, STRING("sd_1_1"), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("sd_1_2"), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(stardust_setting, STRING("sd_1_1"), STRING("sd_1_2"))
    WAIT_MS(500)
    CALL(starstone_get_position, STRING("stone02"), LW(0), LW(1), LW(2))
    CALL(starstone_set_alpha, STRING("stone02"), 255)
    CALL(evt_eff64, STRING("sd_2_1"), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("sd_2_2"), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(stardust_setting, STRING("sd_2_1"), STRING("sd_2_2"))
    WAIT_MS(500)
    CALL(starstone_get_position, STRING("stone03"), LW(0), LW(1), LW(2))
    CALL(starstone_set_alpha, STRING("stone03"), 255)
    CALL(evt_eff64, STRING("sd_3_1"), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("sd_3_2"), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(stardust_setting, STRING("sd_3_1"), STRING("sd_3_2"))
    WAIT_MS(500)
    CALL(starstone_get_position, STRING("stone04"), LW(0), LW(1), LW(2))
    CALL(starstone_set_alpha, STRING("stone04"), 255)
    CALL(evt_eff64, STRING("sd_4_1"), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("sd_4_2"), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(stardust_setting, STRING("sd_4_1"), STRING("sd_4_2"))
    WAIT_MS(500)
    CALL(starstone_get_position, STRING("stone05"), LW(0), LW(1), LW(2))
    CALL(starstone_set_alpha, STRING("stone05"), 255)
    CALL(evt_eff64, STRING("sd_5_1"), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("sd_5_2"), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(stardust_setting, STRING("sd_5_1"), STRING("sd_5_2"))
    WAIT_MS(500)
    CALL(starstone_get_position, STRING("stone06"), LW(0), LW(1), LW(2))
    CALL(starstone_set_alpha, STRING("stone06"), 255)
    CALL(evt_eff64, STRING("sd_6_1"), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("sd_6_2"), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(stardust_setting, STRING("sd_6_1"), STRING("sd_6_2"))
    WAIT_MS(500)
    CALL(starstone_get_position, STRING("stone07"), LW(0), LW(1), LW(2))
    CALL(starstone_set_alpha, STRING("stone07"), 255)
    CALL(evt_eff64, STRING("sd_7_1"), STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("sd_7_2"), STRING("stardust_n64"), 2, LW(0), LW(1), LW(2), FLOAT(30.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(stardust_setting, STRING("sd_7_1"), STRING("sd_7_2"))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 65, 40, 0, 65, -250, 0, 11)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.015f), 130)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.015f), 130)
    CALL(evt_msg_print_party, STRING("mac_7_010"))
    CALL(evt_cam3d_evt_set, -55, 62, -375, -55, 56, -575, 500, 11)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_MAP_KAKAGERU3_2"), 0)
    CALL(evt_map_playanim, STRING("anm_hosi_1"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hosi_1"), LW(0), LW(1))
    WAIT_MS(LW(1))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -55, 115, -375, -55, 118, -575, 250, 11)
    WAIT_MS(250)
    CALL(evt_map_playanim, STRING("anm_hosi_2"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hosi_2"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_cam3d_evt_set, -43, 166, -375, -43, 169, -575, 250, 11)
    WAIT_MS(250)
    CALL(evt_map_playanim, STRING("anm_hosi_3"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hosi_3"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_cam3d_evt_set, 0, 191, -375, 0, 194, -575, 250, 11)
    WAIT_MS(250)
    CALL(evt_map_playanim, STRING("anm_hosi_4"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hosi_4"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_cam3d_evt_set, 43, 166, -375, 43, 169, -575, 250, 11)
    WAIT_MS(250)
    CALL(evt_map_playanim, STRING("anm_hosi_5"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hosi_5"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_cam3d_evt_set, 55, 115, -375, 55, 118, -575, 250, 11)
    WAIT_MS(250)
    CALL(evt_map_playanim, STRING("anm_hosi_6"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hosi_6"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_cam3d_evt_set, 55, 62, -375, 55, 65, -575, 250, 11)
    WAIT_MS(250)
    CALL(evt_map_playanim, STRING("anm_hosi_7"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hosi_7"), LW(0), LW(1))
    WAIT_MS(LW(1))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 125, 0, 0, 130, -400, 500, 11)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 0, 125, 0, 0, 130, -400, 500, 11)
    WAIT_MS(1000)
    CALL(evt_map_playanim, STRING("anm_tobira"), 1, 0)
    BEGIN_INLINE_EVENT
        LOOP(2)
            CALL(evt_cam3d_evt_set, 0, 123, 99, 0, 130, -400, 100, 11)
            WAIT_MS(100)
            CALL(evt_cam3d_evt_set, 0, 125, 0, 0, 130, -400, 100, 11)
            WAIT_MS(1983)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 1, 400)
        WAIT_MS(1683)
        CALL(evt_env_blur_on, 1, 400)
        WAIT_MS(1683)
    END_INLINE_EVENT
    WAIT_MS(4166)
    CALL(evt_cam3d_evt_set, 0, 75, 50, 0, 80, -250, 500, 11)
    CALL(evt_mario_set_reardir)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.015f), 333)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_hibi"), 0, 0)
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_eff, STRING("kemuri00"), STRING("mist2"), 0, 50, 40, -580, 20, 10, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff, STRING("kemuri01"), STRING("mist2"), 0, -50, 40, -580, 20, 10, 0, 0, 0, 0, 0, 0)
        CALL(kemuri_set_color, STRING("kemuri00"), 70, 0, 50)
        CALL(kemuri_set_color, STRING("kemuri01"), 70, 0, 50)
        CALL(evt_sub_intpl_msec_init, 0, -580, -520, 3500)
        WAIT_MS(500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(kemuri_set_position, STRING("kemuri00"), 50, 40, LW(0))
            CALL(kemuri_set_position, STRING("kemuri01"), -50, 40, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_eff_softdelete, STRING("kemuri00"))
        CALL(evt_eff_softdelete, STRING("kemuri01"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_eff, STRING("kemuri02"), STRING("mist2"), 0, 90, 40, -580, 20, 10, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff, STRING("kemuri03"), STRING("mist2"), 0, -90, 40, -580, 20, 10, 0, 0, 0, 0, 0, 0)
        CALL(kemuri_set_color, STRING("kemuri02"), 70, 0, 50)
        CALL(kemuri_set_color, STRING("kemuri03"), 70, 0, 50)
        CALL(evt_sub_intpl_msec_init, 0, -580, -560, 3500)
        WAIT_MS(500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(kemuri_set_position, STRING("kemuri02"), 90, 40, LW(0))
            CALL(kemuri_set_position, STRING("kemuri03"), -90, 40, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_eff_softdelete, STRING("kemuri02"))
        CALL(evt_eff_softdelete, STRING("kemuri03"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_eff, STRING("kemuri04"), STRING("mist2"), 0, 110, 40, -580, 20, 10, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff, STRING("kemuri05"), STRING("mist2"), 0, -110, 40, -580, 20, 10, 0, 0, 0, 0, 0, 0)
        CALL(kemuri_set_color, STRING("kemuri04"), 70, 0, 50)
        CALL(kemuri_set_color, STRING("kemuri05"), 70, 0, 50)
        WAIT_MS(500)
        WAIT_MS(3500)
        CALL(evt_eff_softdelete, STRING("kemuri04"))
        CALL(evt_eff_softdelete, STRING("kemuri05"))
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_eff, STRING("las_mon"), STRING("las_mon"), 0, 0, 20, -560, 0, 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 120, 8000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 0, STRING("door_b"), 0, LW(0), 0)
            MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
            CALL(evt_mapobj_rotate, 0, STRING("door_a"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), 8000)
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 200)
        WAIT_MS(200)
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 200)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(8000)
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(mahojin_effect_alpha, STRING("mahojin"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_eff_softdelete, STRING("mahojin"))
        CALL(evt_eff_softdelete, STRING("las_mon"))
        SPAWN_EVENT(mahojin_se_event_end)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(8000)
        SET_VALUE(LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(starstone_set_alpha, STRING("stone01"), LW(0))
            CALL(starstone_set_alpha, STRING("stone02"), LW(0))
            CALL(starstone_set_alpha, STRING("stone03"), LW(0))
            CALL(starstone_set_alpha, STRING("stone04"), LW(0))
            CALL(starstone_set_alpha, STRING("stone05"), LW(0))
            CALL(starstone_set_alpha, STRING("stone06"), LW(0))
            CALL(starstone_set_alpha, STRING("stone07"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(8))
        CALL(starstone_set_position, STRING("stone01"), 0, -1000, 0)
        CALL(starstone_set_position, STRING("stone02"), 0, -1000, 0)
        CALL(starstone_set_position, STRING("stone03"), 0, -1000, 0)
        CALL(starstone_set_position, STRING("stone04"), 0, -1000, 0)
        CALL(starstone_set_position, STRING("stone05"), 0, -1000, 0)
        CALL(starstone_set_position, STRING("stone06"), 0, -1000, 0)
        CALL(starstone_set_position, STRING("stone07"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, -625, 550, 370, 0, 90, -545, 0, 11)
    CALL(evt_mario_dispflag_onoff, 1, 4)
    CALL(evt_mario_set_dir, 270, 0, 0)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 465, -5, -150, 18, 135, -525, 0, 11)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 102, 300, 0, 110, -250, 0, 11)
    WAIT_MS(4100)
    CALL(evt_cam3d_evt_set, 0, 60, 75, 0, 65, -250, 500, 11)
    WAIT_MS(1500)
    CALL(evt_msg_print, 0, STRING("mac_7_011"), 0, STRING("クリハカセ"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(800)
    CALL(evt_cam3d_evt_set, 0, 73, 75, 0, 78, -250, 500, 11)
    CALL(evt_party_jump_pos, 0, 30, 40, -230, 300, 0, -1000)
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_msg_print_party, STRING("mac_7_013"))
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_cam3d_evt_off, 500, 11)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 300, 0, -1000)
    SET_VALUE(GSW(0), 380)
    CALL(evt_party_run, 0)
    CALL(evt_mario_dispflag_onoff, 0, 4)
    CALL(evt_mario_adjust_dir)
    CALL(evt_mario_key_onoff, 1)
    SPAWN_EVENT(lastdungeon_init)
    CALL(evt_run_case_evt, 10, 1, STRING("n_bero"), 0, PTR(lastdungeon), 0)
    SET_VALUE(GF(10), 0)
    SPAWN_EVENT(mahojin_alpha)
    CALL(evt_exit_case_evt)
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_TIK2"), 3000)
    END_INLINE_EVENT
    RETURN
EVT_END()



EVT_BEGIN(tik_05_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_TIK2"), 3000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_mapobj_trans, 1, STRING("hosi_1"), 0, -5, 0)
    CALL(alpha_init_func)
    SET_VALUE(GF(10), 0)
    SPAWN_EVENT(mahojin_alpha)
    CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_alpha_up), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("a_dai"), 0, PTR(mahojin_alpha_down), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_doa_l"), 2)
    SWITCH(GSW(0))
        CASE_BETWEEN(15, 17)
        SPAWN_EVENT(kaidan_event)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event), 0)
        CASE_EQUAL(63)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event1), 0)
        CASE_EQUAL(119)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event2), 0)
        CASE_EQUAL(173)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event3), 0)
        CASE_EQUAL(220)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event4), 0)
        CASE_EQUAL(270)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event5), 0)
        CASE_EQUAL(346)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event6), 0)
        CASE_BETWEEN(378, 379)
        CALL(evt_run_case_evt, 0, 1, STRING("a_dai"), 0, PTR(mahojin_event7), 0)
    END_SWITCH
    SWITCH(GSW(0))
        CASE_EQUAL(15)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL_CHILD(large_door_stg0_init)
        CASE_LESS(380)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_off)
        CASE_EQUAL(380)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_off)
        SPAWN_EVENT(lastdungeon_anim_init)
        SPAWN_EVENT(lastdungeon_init)
        CALL(evt_run_case_evt, 10, 1, STRING("n_bero"), 0, PTR(lastdungeon), 0)
        CASE_GREATER(380)
        SPAWN_EVENT(lastdungeon_anim_init)
    END_SWITCH
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou1"), STRING("hikou1_a"))
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou2"), STRING("hikou2_a"))
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -580, 380, -225, 0, GSWF(5532), 0)
    CALL(evt_mobj_jumpstand_blue, 0, STRING("mobj_jump00"), 250, 550, 80, -125, 0, 0)
    CALL(evt_mobj_jumpstand_blue, 0, STRING("mobj_jump01"), 250, -550, 80, -125, 0, 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -350, 60, 175, 0, 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 0, 0, 400, STRING("jimen"), 125, GSWF(5592))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("hasira"), 2)
    CALL(evt_sub_animgroup_async, STRING("MOBJ_EFF_las_mon"))
    CALL(evt_sub_animgroup_async, STRING("EFF_treasure_map"))
    CALL(evt_sub_animgroup_async, STRING("MOBJ_EFF_mahojin"))
    CALL(evt_sub_animgroup_async, STRING("MOBJ_EFF_laser"))
    CALL(evt_sub_animgroup_async, STRING("OFF_d_tizu_tex"))
    CALL(evt_sub_animgroup_async, STRING("TIZ_stg_1"))
    CALL(evt_sub_animgroup_async, STRING("TIZ_stg_2"))
    CALL(evt_sub_animgroup_async, STRING("TIZ_stg_3"))
    CALL(evt_sub_animgroup_async, STRING("TIZ_stg_4"))
    CALL(evt_sub_animgroup_async, STRING("TIZ_stg_5"))
    CALL(evt_sub_animgroup_async, STRING("TIZ_stg_6"))
    CALL(evt_sub_animgroup_async, STRING("TIZ_stg_7"))
    CALL(is_dol_viewer)
    RETURN
EVT_END()

// The uninitialised globals are laid out in reverse declaration order.
static u8* _ap;

static CamKaiten* ckp0;

static CamKaiten* ckp1;

static CamKaiten* ckpw;

BrokenWork broken_work;

// TODO: type
u32 select[1];




USER_FUNC(starstone_set_position) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* entry = effNameToPtr((char*)name);

    ((effStarStoneData*)entry->userdata)->x = x;
    ((effStarStoneData*)entry->userdata)->y = y;
    ((effStarStoneData*)entry->userdata)->z = z;
    return EVT_RETURN_DONE;
}

USER_FUNC(starstone_get_position) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    effStarStoneData* data = ((EffectEntry*)effNameToPtr((char*)name))->userdata;
    f32 y = data->y;
    f32 z = data->z;

    evtSetFloat(event, args[1], data->x);
    evtSetFloat(event, args[2], y);
    evtSetFloat(event, args[3], z);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_ry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 ry = evtGetFloat(event, args[1]);
    EffectEntry* entry = effNameToPtr((char*)name);
    effStarStoneData* data = entry->userdata;

    data->rotY = ry - 360.0f * ((s32)ry / 360);
    return EVT_RETURN_DONE;
}

USER_FUNC(starstone_set_alpha) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    u8 alpha = evtGetValue(event, args[1]);
    effStarStoneData* data = ((EffectEntry*)effNameToPtr((char*)name))->userdata;

    data->tevReg0A = alpha;
    return EVT_RETURN_DONE;
}

USER_FUNC(starstone_set_scale) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 scale = evtGetFloat(event, args[1]);
    effStarStoneData* data = ((EffectEntry*)effNameToPtr((char*)name))->userdata;

    data->scale = scale;
    return EVT_RETURN_DONE;
}

USER_FUNC(starstone_circle_link_func) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 idx = (f32)(evtGetValue(event, args[1]) - 1);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    f32 angle = evtGetFloat(event, args[5]);
    f32 radius = evtGetFloat(event, args[6]);
    EffectEntry* entry = effNameToPtr((char*)name);
    f32 rad;
    f32 s;
    f32 c;

    angle = angle - 360.0f * (f32)((s32)angle / 360);
    rad = 6.2831855f * (360.0f * idx / 7.0f + angle) / 360.0f;
    s = (f32)sin(rad);
    c = (f32)cos(rad);

    ((effStarStoneData*)entry->userdata)->x = s * radius + x;
    ((effStarStoneData*)entry->userdata)->y = y;
    ((effStarStoneData*)entry->userdata)->z = -c * radius + z;
    return EVT_RETURN_DONE;
}

USER_FUNC(eff_starstone_sound_off) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    effStarStoneData* data = ((EffectEntry*)effNameToPtr((char*)name))->userdata;

    data->flags &= ~4;
    return EVT_RETURN_DONE;
}

// The original builds the colour in one stack slot and copies it into the
// argument slot, so the build goes through an inline that returns by value.
static inline GXColor alpha_tev_color(u8 a) {
    GXColor c = *(GXColor*)&zero_tik_00009b58;
    c.a = a;
    return c;
}

static void alpha_tev(MapTevCallback* param) {
    u32 stage = param->tevStage;
    u8* ap = _ap;

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColor(GX_TEVREG0, alpha_tev_color(*ap));
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->tevStage = param->tevStage + 1;
}

static USER_FUNC(alpha_init_func) {
    _ap = _mapAlloc(1);
    *_ap = 0xFF;
    return EVT_RETURN_DONE;
}

static USER_FUNC(alpha_set_func) {
    s32* args = event->args;
    *_ap = evtGetValue(event, args[0]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_init) {
    s32* args = event->args;
    Vec a;
    Vec p;
    f32 dx;
    f32 dz;
    f32 dist;

    ckp0 = _mapAlloc(0x20);
    memset(ckp0, 0, 0x20);
    ckp1 = _mapAlloc(0x20);
    memset(ckp1, 0, 0x20);
    ckpw = _mapAlloc(0x20);
    memset(ckp1, 0, 0x20);

    ckp0->pos.x = evtGetFloat(event, args[0]);
    ckp0->pos.y = evtGetFloat(event, args[1]);
    ckp0->pos.z = evtGetFloat(event, args[2]);
    ckp0->at.x = evtGetFloat(event, args[3]);
    ckp0->at.y = evtGetFloat(event, args[4]);
    ckp0->at.z = evtGetFloat(event, args[5]);
    ckp1->pos.x = evtGetFloat(event, args[6]);
    ckp1->pos.y = evtGetFloat(event, args[7]);
    ckp1->pos.z = evtGetFloat(event, args[8]);
    ckp1->at.x = evtGetFloat(event, args[9]);
    ckp1->at.y = evtGetFloat(event, args[10]);
    ckp1->at.z = evtGetFloat(event, args[11]);

    p = ckp0->pos;
    a = ckp0->at;
    a.y = 0.0f;
    p.y = 0.0f;
    dist = PSVECDistance(&p, &a);
    dx = a.x - p.x;
    dz = a.z - p.z;
    ckp0->angle = 57.29579f * (f32)atan2(-dx, dz);
    ckp0->dist = dist;

    p = ckp1->pos;
    a = ckp1->at;
    a.y = 0.0f;
    p.y = 0.0f;
    dist = PSVECDistance(&p, &a);
    dx = a.x - p.x;
    dz = a.z - p.z;
    ckp1->angle = 57.29579f * (f32)atan2(-dx, dz);
    ckp1->dist = dist;

    ckpw->pos = ckp0->pos;
    ckpw->at = ckp0->at;
    ckpw->angle = ckp0->angle;
    ckpw->dist = ckp0->dist;

    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_set_direct) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 s;
    f32 c;

    s = (f32)sin(6.2831855f * ckpw->angle / 360.0f);
    c = (f32)cos(6.2831855f * ckpw->angle / 360.0f);
    ckpw->pos.x = ckpw->at.x + ckpw->dist * s;
    ckpw->pos.z = ckpw->at.z - ckpw->dist * c;

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = ckpw->pos;
    cam->storedAt = ckpw->at;
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 11;
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_get_para) {
    s32* args = event->args;
    CamKaiten* kp;

    switch (evtGetValue(event, args[0])) {
    case 0:
        kp = ckp0;
        break;
    case 1:
        kp = ckp1;
        break;
    case 2:
    default:
        kp = ckpw;
        break;
    }

    evtSetFloat(event, args[1], kp->pos.x);
    evtSetFloat(event, args[2], kp->pos.y);
    evtSetFloat(event, args[3], kp->pos.z);
    evtSetFloat(event, args[4], kp->at.x);
    evtSetFloat(event, args[5], kp->at.y);
    evtSetFloat(event, args[6], kp->at.z);
    evtSetFloat(event, args[7], kp->angle);
    evtSetFloat(event, args[8], kp->dist);
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_set_para) {
    s32* args = event->args;
    CamKaiten* kp;

    switch (evtGetValue(event, args[0])) {
    case 0:
        kp = ckp0;
        break;
    case 1:
        kp = ckp1;
        break;
    case 2:
    default:
        kp = ckpw;
        break;
    }

    kp->pos.x = evtGetFloat(event, args[1]);
    kp->pos.y = evtGetFloat(event, args[2]);
    kp->pos.z = evtGetFloat(event, args[3]);
    kp->at.x = evtGetFloat(event, args[4]);
    kp->at.y = evtGetFloat(event, args[5]);
    kp->at.z = evtGetFloat(event, args[6]);
    kp->angle = evtGetFloat(event, args[7]);
    kp->dist = evtGetFloat(event, args[8]);
    return EVT_RETURN_DONE;
}

USER_FUNC(disp_lighteffect_func) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    f32 angle = evtGetFloat(event, args[4]);
    EffectEntry* entry = effNameToPtr((char*)name);
    f32 s;
    f32 c;
    f32 rad = 6.2831855f * -angle / 360.0f;

    s = (f32)sin(rad);
    c = (f32)cos(rad);

    ((MahojinWork*)entry->userdata)->position.x = 30.0f * s + x;
    ((MahojinWork*)entry->userdata)->position.y = y;
    ((MahojinWork*)entry->userdata)->position.z = 30.0f * -c + z;
    return EVT_RETURN_DONE;
}

USER_FUNC(disp_treasure_map_func) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    f32 angle = evtGetFloat(event, args[4]);
    u8 alpha = evtGetValue(event, args[5]);
    EffectEntry* entry = effNameToPtr((char*)name);

    ((EffTreasureMapData*)entry->userdata)->x = x;
    ((EffTreasureMapData*)entry->userdata)->y = y;
    ((EffTreasureMapData*)entry->userdata)->z = z;
    ((EffTreasureMapData*)entry->userdata)->angle = 180.0f + angle;
    ((EffTreasureMapData*)entry->userdata)->alpha = alpha;
    return EVT_RETURN_DONE;
}

USER_FUNC(mahojin_effect_alpha) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    u8 alpha = evtGetValue(event, args[1]);
    MahojinWork* data = ((EffectEntry*)effNameToPtr((char*)name))->userdata;

    data->alpha = alpha;
    return EVT_RETURN_DONE;
}

USER_FUNC(effect_position) {
    ItemEntry* item = itemNameToPtr("item00");
    EffectEntry* entry;
    f32 rad;
    f32 s, c;

    rad = -(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
    s = 5.0f * (f32)sin(rad);
    c = 5.0f * (f32)cos(rad);
    entry = effNameToPtr("map_wave");
    if (entry != NULL) {
        ((EffCrystalData*)entry->userdata)->x = item->position.x + s;
        ((EffCrystalData*)entry->userdata)->y = item->position.y;
        ((EffCrystalData*)entry->userdata)->z = item->position.z + c;
    }
    s = 10.0f * (f32)sin(rad);
    c = 10.0f * (f32)cos(rad);
    entry = effNameToPtr("map_map");
    if (entry != NULL) {
        ((EffTreasureMapData*)entry->userdata)->x = item->position.x + s;
        ((EffTreasureMapData*)entry->userdata)->y = item->position.y;
        ((EffTreasureMapData*)entry->userdata)->z = item->position.z + c;
        ((EffTreasureMapData*)entry->userdata)->angle = 180.0f - camGetPtr(CAMERA_3D)->viewYaw;
        ((EffTreasureMapData*)entry->userdata)->alpha = evtGetValue(event, GW(11));
        ((EffTreasureMapData*)entry->userdata)->scale = 0.9f;
    }
    s = 15.0f * (f32)sin(rad);
    c = 15.0f * (f32)cos(rad);
    entry = effNameToPtr("map_ball");
    if (entry != NULL) {
        ((effPokopiPchargeData*)entry->userdata)->x = item->position.x + s;
        ((effPokopiPchargeData*)entry->userdata)->y = item->position.y;
        ((effPokopiPchargeData*)entry->userdata)->z = item->position.z + c;
    }
    return EVT_RETURN_BLOCK;
}

// The original builds the colour in one stack slot and copies it into the
// argument slot, so the build goes through an inline that returns by value.
static inline GXColor img_alpha_color(u8 a) {
    GXColor c = *(GXColor*)&dat_tik_00009bb8;
    c.a = a;
    return c;
}

USER_FUNC(img_alpha) {
    s32* args = event->args;
    s32 alpha = evtGetValue(event, args[0]);
    ImageEntry* entry = imgNameToPtr("img", gp->inBattle != 0);
    Mtx trans;
    Mtx scale;
    Mtx rot;

    animPoseSetMaterialFlagOn(entry->poseId, 0x40);
    animPoseSetMaterialEvtColor(entry->poseId, img_alpha_color(alpha));
    PSMTXTrans(trans, 0.0f, 1.5f, 0.0f);
    PSMTXScale(scale, 0.9f, 0.9f, 0.9f);
    PSMTXRotRad(rot, 'x', -0.5235988f);
    PSMTXConcat(trans, scale, trans);
    PSMTXConcat(trans, rot, entry->matrix);
    return EVT_RETURN_DONE;
}

USER_FUNC(tiz_stone) {
    s32* args = event->args;
    f32 t = (f32)evtGetValue(event, args[0]);
    EffectEntry* entry;

    t = t / 10000.0f;

    entry = effNameToPtr("tiz_dust");
    if (entry != NULL) {
        effStardustN64SetDrawCam(entry, 7);
    }
    entry = effNameToPtr("tiz_stone");
    if (entry != NULL) {
        ((effStarStoneData*)entry->userdata)->camId = 7;
        ((effStarStoneData*)entry->userdata)->tevReg0A = (u8)(255.0f * t);
        ((effStarStoneData*)entry->userdata)->rotY = 5.0f * (360.0f * t);
    }
    entry = effNameToPtr("tiz_dust2");
    if (entry != NULL) {
        effStardust2N64SetDrawCam(entry, 7);
    }
    return EVT_RETURN_DONE;
}

void tiz_broken_draw(CameraId camId, void* param) {
    CameraEntry* cam = camGetPtr(camId);
    GXTexObj texObj;
    Mtx m1;
    Mtx m0;
    Mtx m2;
    Mtx mtx;
    f32 y;
    f32 angle;
    s32 j;
    s32 i;
    BrokenEntry* entry;

    if (((BrokenWork*)broken_wp[0])->texCopied == 0) {
        GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
        GXSetTexCopyDst(0x130, 0xF0, 5, 1);
        GXCopyTex(((BrokenWork*)broken_wp[0])->texBuffer->address, 0);
        GXPixModeSync();
        ((BrokenWork*)broken_wp[0])->texCopied = 1;
    }

    GXSetNumChans(0);
    GXInitTexObj(&texObj, ((BrokenWork*)broken_wp[0])->texBuffer->address, 0x130, 0xF0, 5, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetBlendMode(0, 1, 0, 7);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(13, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 13, 1, 4, 0);
    GXSetCurrentMtx(0);
    PSMTXIdentity(mtx);

    entry = (BrokenEntry*)((BrokenWork*)broken_wp[0])->entryBuffer->address;
    for (i = 0; i < 0x1E; i++, entry++) {
        y = 232.0f - (f32)(i * 0x10);
        for (j = 0; j < 0x26; j++, entry++) {
            if (entry->timer == 0) {
                PSMTXScale(m0, 0.026315789f, 0.033333335f, 1.0f);
                PSMTXTrans(m1, (f32)j, (f32)i, 0.0f);
                PSMTXConcat(m0, m1, m1);
                GXLoadTexMtxImm(m1, 0x1E, 1);
                PSMTXTrans(m1, -296.0f + (f32)(j * 0x10), y, 0.0f);
                switch (entry->quadrant) {
                case 0:
                    angle = -45.0f;
                    break;
                case 1:
                    angle = 45.0f;
                    break;
                case 2:
                    angle = 225.0f;
                    break;
                case 3:
                    angle = 135.0f;
                    break;
                }
                PSMTXTrans(m2, entry->x, entry->y, entry->z);
                PSMTXConcat(m1, m2, m1);
                PSMTXRotRad(m2, 'z', 0.017453292f * angle);
                PSMTXConcat(m1, m2, m1);
                PSMTXRotRad(m2, 'y', 0.017453292f * entry->spinAngle);
                PSMTXConcat(m1, m2, m1);
                PSMTXRotRad(m2, 'z', 0.017453292f * -angle);
                PSMTXConcat(m1, m2, m1);
                PSMTXConcat(cam->view, m1, m1);
                GXLoadPosMtxImm(m1, 0);
                GXBegin(0x80, 0, 4);
                GXPosition3f32(-8.0f, 8.0f, 0.0f);
                GXTexCoord2f32(0.0f, 0.0f);
                GXPosition3f32(8.0f, 8.0f, 0.0f);
                GXTexCoord2f32(1.0f, 0.0f);
                GXPosition3f32(8.0f, -8.0f, 0.0f);
                GXTexCoord2f32(1.0f, 1.0f);
                GXPosition3f32(-8.0f, -8.0f, 0.0f);
                GXTexCoord2f32(0.0f, 1.0f);
            }
        }
    }
}

// Inline degree/radian sine and cosine approximations (the same polynomials the
// NPC blur path uses).
static inline f32 tikSinf(f32 rad) {
    const f32 pi = 3.1415927f;
    const f32 halfpi = 1.5707964f;
    const f32 threehalves = 4.712389f;
    f32 f0, f1, f2, sinv = 0.00761f;
    f32 c1 = 0.16605f;
    f32 c2 = 1.0f;

    if (rad <= pi) {
        if (rad <= halfpi) {
            sinv = sinv * (rad * rad) - c1;
            sinv = sinv * (rad * rad) + c2;
            sinv = sinv * rad;
        } else {
            f1 = halfpi - (rad - halfpi);
            f0 = f1 * f1;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * f1;
        }
    } else if (rad < threehalves) {
        f1 = rad - pi;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    } else {
        f1 = halfpi - (rad - threehalves);
        f2 = f1 * f1;
        sinv = sinv * f2 - c1;
        sinv = sinv * f2 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 tikCosf(f32 rad) {
    const f32 pi = 3.1415927f;
    const f32 halfpi = 1.5707964f;
    const f32 threehalves = 4.712389f;
    f32 f0, cosv = 0.03705f;
    f32 c1 = 0.4967f;
    f32 c2 = 1.0f;

    if (rad <= pi) {
        if (rad <= halfpi) {
            cosv = cosv * (rad * rad) - c1;
            cosv = cosv * (rad * rad) + c2;
        } else {
            f0 = halfpi - (rad - halfpi);
            f0 = f0 * f0;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < threehalves) {
        f0 = (rad - pi) * (rad - pi);
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = halfpi - (rad - threehalves);
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

// Matches retail. The polynomial seed/coefficient hoisting needs `opt_propagation off`;
// `opt_lifetimes off` keeps the two seed loads (0.00761/0.03705) inside the init loop. The
// zero test is written as truthiness (`f3 || f4`) because that is the only form MWCC emits
// with the value first in `fcmpu` (`fcmpu f3, f27`), which retail uses.
#pragma opt_propagation off // keeps the polynomial coefficients hoisted like the target
#pragma opt_lifetimes off // keeps the two seed loads inside the loop like the target
USER_FUNC(tiz_broken) {
    s32 j, i;
    s32 active = 0;
    // The update loop needs its own entry pointer and counter to colour r5/r6 like the
    // target; reusing `entry`/`i` pins them to the init loop's callee-saved registers.
    s32 i2;
    BrokenEntry* entry;
    BrokenEntry* entry2;
    f32 f0, f1, f2, f3, f4, f5, f6, f7;
    f32 angle;
    f32 rad;
    f32 k;

    if (isFirstCall) {
        memset((void*)broken_wp[0], 0, 0xC);
        ((BrokenWork*)broken_wp[0])->texBuffer = smartAlloc(GXGetTexBufferSize(0x130, 0xF0, 5, 0, 0), 1);
        ((BrokenWork*)broken_wp[0])->entryBuffer = smartAlloc(0xF594, 1);
        ((BrokenWork*)broken_wp[0])->texCopied = 0;

        entry = (BrokenEntry*)((BrokenWork*)broken_wp[0])->entryBuffer->address;
        for (i = 0; i < 0x1E; i++) {
            for (j = 0; j < 0x26; j++) {
                f3 = (f32)j / 37.0f - 0.5f;
                f4 = 0.5f + (f32)(-i) / 29.0f;
                if (f4 > 0.0f) {
                    if (f3 < 0.0f) {
                        entry->quadrant = 0;
                    } else {
                        entry->quadrant = 1;
                    }
                } else {
                    if (f3 < 0.0f) {
                        entry->quadrant = 2;
                    } else {
                        entry->quadrant = 3;
                    }
                }
                if (f3 || f4) {
                    f6 = sqrtf(f3 * f3 + f4 * f4);
                    f0 = f3 / f6;
                    f2 = f4 / f6;
                    f7 = f6;
                } else {
                    f7 = 0.0f;
                    f0 = 0.0f;
                    f2 = 0.0f;
                }
                entry->state = 0;
                entry->timer = (s32)(param_tbl[0] * f7);
                entry->x = 0.0f;
                entry->y = 0.0f;
                entry->z = 0.0f;
                entry->spinSpeed = 0.0f;
                entry->spinAngle = 0.0f;
                entry->impulseX = -f0 * param_tbl[1];
                entry->impulseY = -f2 * param_tbl[1];
                entry->impulseZ = 10.0f;
                angle = angleABf(0.0f, 0.0f, f3, f4);
                rad = 6.2831855f * angle / 360.0f;
                k = param_tbl[3];
                entry->velX = k * tikSinf(rad);
                entry->velY = k * -tikCosf(rad);
                entry++;
            }
            entry++;
        }
    }

    entry2 = (BrokenEntry*)((BrokenWork*)broken_wp[0])->entryBuffer->address;
    for (i2 = 0; i2 < 0x1E; i2++) {
        for (j = 0; j < 0x26; j++) {
            switch (entry2->state) {
            case 0:
                if (entry2->timer != 0) {
                    entry2->timer--;
                } else {
                    entry2->x += entry2->velX;
                    entry2->y += entry2->velY;
                    entry2->x += entry2->impulseX;
                    entry2->y += entry2->impulseY;
                    entry2->z += entry2->impulseZ;
                    entry2->impulseX *= param_tbl[2];
                    entry2->impulseY *= param_tbl[2];
                    entry2->impulseZ *= param_tbl[2];
                    entry2->spinSpeed += param_tbl[4];
                    entry2->spinAngle += entry2->spinSpeed;
                    if (__fabsf(entry2->x) > 340.0 || __fabsf(entry2->y) > 260.0) {
                        entry2->state++;
                    }
                }
                break;
            case 1:
                active++;
                break;
            }
            entry2++;
        }
        entry2++;
    }

    if (active >= 0x474) {
        smartFree(((BrokenWork*)broken_wp[0])->texBuffer);
        smartFree(((BrokenWork*)broken_wp[0])->entryBuffer);
        return EVT_RETURN_DONE;
    }
    dispEntry(8, 0.0f, 0, tiz_broken_draw, NULL);
    return EVT_RETURN_BLOCK;
}

#pragma opt_lifetimes reset
#pragma opt_propagation reset

static USER_FUNC(evtflash) {
    s32* args = event->args;
    s32 arg0 = evtGetValue(event, args[0]);
    s32 arg1 = evtGetValue(event, args[1]);
    s32 arg2 = evtGetValue(event, args[2]);
    GXColor color;

    if (isFirstCall) {
        event->userdata[2] = 0;
    }
    switch (event->userdata[2]) {
    case 0:
        color = *(GXColor*)&dat_tik_00009bd8;
        fadeEntry(10, arg0, color);
        event->userdata[2]++;
    case 1:
        if (fadeIsFinish() != TRUE) {
            return EVT_RETURN_BLOCK;
        }
        event->userdata[0] = arg1;
        event->savedTime = (s32)gp->renderTime;
        event->userdata[2]++;
        if (event->userdata[0] == 0) {
            event->userdata[2]++;
        }
        return EVT_RETURN_BLOCK;
    case 2: {
        u32 elapsed = (u32)((s32)gp->renderTime - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed < (u32)event->userdata[0]) {
            return EVT_RETURN_BLOCK;
        }
        event->userdata[2]++;
        return EVT_RETURN_BLOCK;
    }
    case 3:
        color = *(GXColor*)&dat_tik_00009bdc;
        fadeEntry(9, arg2, color);
        event->userdata[2]++;
    case 4:
        if (fadeIsFinish() != TRUE) {
            return EVT_RETURN_BLOCK;
        }
        return EVT_RETURN_DONE;
    default:
        return EVT_RETURN_DONE;
    }
}

USER_FUNC(map_disp_set) {
    s32* args = event->args;
    if (evtGetValue(event, args[0]) != 0) {
        N_mapDispOn();
    } else {
        N_mapDispOff();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(crystal_bgmode) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    EffectEntry* entry = effNameToPtr((char*)name);

    ((EffCrystalData*)entry->userdata)->camId = CAMERA_3D_EFFECTA;
    return EVT_RETURN_DONE;
}

USER_FUNC(map_bgmode) {
    s32* args = event->args;
    s32 name1 = evtGetValue(event, args[0]);
    s32 name2 = evtGetValue(event, args[1]);
    EffectEntry* entry = effNameToPtr((char*)name1);

    ((effItemGetData*)entry->userdata)->camId = CAMERA_3D_EFFECTA;
    ((effItemGetData*)entry->userdata)->baseSize = 4.3f;
    entry = effNameToPtr((char*)name2);
    ((EffTreasureMapData*)entry->userdata)->cameraId = CAMERA_3D_EFFECTA;
    marioSetCamId(CAMERA_3D_EFFECTA);
    return EVT_RETURN_DONE;
}

USER_FUNC(map_bgmode_end) {
    s32* args = event->args;
    s32 name1 = evtGetValue(event, args[0]);
    s32 name2 = evtGetValue(event, args[1]);
    EffectEntry* entry = effNameToPtr((char*)name1);
    effItemGetData* data = entry->userdata;

    data->camId = CAMERA_3D;
    entry = effNameToPtr((char*)name2);
    ((EffTreasureMapData*)entry->userdata)->cameraId = CAMERA_3D;
    marioSetCamId(CAMERA_3D);
    return EVT_RETURN_DONE;
}

USER_FUNC(cam_mahojin_kaiten_main) {
    s32 duration;
    s32* args = event->args;
    f32 start = evtGetFloat(event, args[0]);
    f32 end = evtGetFloat(event, args[1]);
    s32 frames;
    s32 active;
    f32 value;
    f32 curve;
    f32 step;
    s32 i;
    s32 up;
    s32 down;

    duration = evtGetValue(event, args[2]);
    frames = sysMsec2Frame(duration);
    active = 1;

    if (event->lwData[15] == 0) {
        event->lwData[14] = 0;
        event->lwData[15] = 1;
        event->savedTime = gp->renderTime;
    }

    up = frames * 20 / 100;
    down = frames * 80 / 100;
    curve = 0.0f;
    step = 0.0f;
    for (i = 0; i < frames; i++) {
        if (i <= up) {
            step += 1.0f;
        } else if (i >= down) {
            step -= 1.0f;
        }
        curve += step;
        if (i == event->lwData[14]) {
            value = curve;
        }
    }

    {
        u32 elapsed = (u32)((s32)gp->renderTime - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000);
        if ((s32)elapsed > duration) {
            elapsed = duration;
        }
        event->lwData[14] = sysMsec2Frame(elapsed);
    }
    if (frames == event->lwData[14]) {
        active = 0;
    }

    evtSetFloat(event, args[3], start + (value * (end - start)) / curve);
    evtSetValue(event, args[4], active);
    return EVT_RETURN_DONE;
}

USER_FUNC(starstone_setting) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);
    EffectEntry* e1 = effNameToPtr((char*)name);
    EffectEntry* e2 = effNameToPtr("mahojin");
    effStarStoneData* d1 = e1->userdata;

    d1->flags |= 0x20;
    ((MahojinWork*)e2->userdata)[index + 2].drawArg = e1;
    ((MahojinWork*)e2->userdata)[index + 2].drawFunc = effStarStoneDraw;
    return EVT_RETURN_DONE;
}

USER_FUNC(kemuri_set_position) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* entry = effNameToPtr((char*)name);

    ((EffMist2Data*)entry->userdata)->x = x;
    ((EffMist2Data*)entry->userdata)->y = y;
    ((EffMist2Data*)entry->userdata)->z = z;
    return EVT_RETURN_DONE;
}

USER_FUNC(kemuri_set_color) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    u8 r = evtGetValue(event, args[1]);
    u8 g = evtGetValue(event, args[2]);
    u8 b = evtGetValue(event, args[3]);
    EffectEntry* entry = effNameToPtr((char*)name);

    ((EffMist2Data*)entry->userdata)->colorR = r;
    ((EffMist2Data*)entry->userdata)->colorG = g;
    ((EffMist2Data*)entry->userdata)->colorB = b;
    ((EffMist2Data*)entry->userdata)->unk18 = 1.0f + (f32)irand(0x1E) / 10.0f;
    return EVT_RETURN_DONE;
}

USER_FUNC(pcharge_set_position) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* entry = effNameToPtr((char*)name);

    ((effPokopiPchargeData*)entry->userdata)->x = x;
    ((effPokopiPchargeData*)entry->userdata)->y = y;
    ((effPokopiPchargeData*)entry->userdata)->z = z;
    return EVT_RETURN_DONE;
}

USER_FUNC(stardust_setting) {
    s32* args = event->args;
    s32 name1 = evtGetValue(event, args[0]);
    s32 name2 = evtGetValue(event, args[1]);
    EffectEntry* e1 = effNameToPtr((char*)name1);
    EffectEntry* e2 = effNameToPtr((char*)name2);

    effStardustN64SetDrawCam(e1, 5);
    effStardustN64SetDrawCam(e2, 5);
    return EVT_RETURN_DONE;
}

USER_FUNC(is_dol_viewer) {
    u32 type;

    EXIGetType(0, 2, &type);
    if (type == EXI_IS_VIEWER) {
        animPoseRelease(animPoseEntry("MOBJ_EFF_las_mon", 0));
        animPoseRelease(animPoseEntry("EFF_treasure_map", 0));
        animPoseRelease(animPoseEntry("MOBJ_EFF_mahojin_omote", 0));
        animPoseRelease(animPoseEntry("MOBJ_EFF_mahojin_ura", 0));
        animPoseRelease(animPoseEntry("MOBJ_EFF_laser", 0));
        animPoseRelease(animPoseEntry("OFF_d_tizu_tex", 0));
        animPoseRelease(animPoseEntry("TIZ_stg_1", 0));
        animPoseRelease(animPoseEntry("TIZ_stg_2", 0));
        animPoseRelease(animPoseEntry("TIZ_stg_3", 0));
        animPoseRelease(animPoseEntry("TIZ_stg_4", 0));
        animPoseRelease(animPoseEntry("TIZ_stg_5", 0));
        animPoseRelease(animPoseEntry("TIZ_stg_6", 0));
        animPoseRelease(animPoseEntry("TIZ_stg_7", 0));
    }
    return EVT_RETURN_DONE;
}

// clang-format on

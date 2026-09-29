#include "evt/evt_bero.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 move_dai[];

// clang-format off
extend_data(100,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(101,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_22", "e_bero", 5, 5, (s32)w_door_close, (s32)w_door_open},
    {"s_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_23", "n_bero_2", 1, 1},
    {"s_bero_2", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_25", "n_bero_1", 1, 1},
    {NULL},
};

static char* haguruma_hontai[] = {
    "S_hgm_01a", "S_hgm_02a", "S_hgm_03a", "S_hgm_04a",
    "S_hgm_06a", "S_hgm_07a", "S_hgm_08a", "S_hgm_10a",
    "S_hgm_11a", "S_hgm_14a", "S_hgm_15a", "S_hgm_16a",
    "S_hgm_19a", "S_hgm_20a", "S_hgm_24a",
};

static char* haguruma_hontai2[] = {
    "S_hgm_05a", "S_hgm_09a", "S_hgm_12a", "S_hgm_13a",
    "S_hgm_17a", "S_hgm_18a",
};

static char* haguruma_kage[] = {
    "S_hgm_01b", "S_hgm_02b", "S_hgm_03b", "S_hgm_04b",
    "S_hgm_06b", "S_hgm_07b", "S_hgm_08b", "S_hgm_10b",
    "S_hgm_11b", "S_hgm_14b", "S_hgm_15b", "S_hgm_16b",
};

static char* haguruma_kage2[] = {
    "S_hgm_05b", "S_hgm_09b", "S_hgm_12b", "S_hgm_13b",
    "S_hgm_17b",
};

static EVT_BEGIN(haguruma_evt)
    SET_FLOAT_VALUE(LW(0), FLOAT(20.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(4), FLOAT(0.0f))
    LOOP(0)
        SET_INT_READ_ADDR(PTR(haguruma_hontai))
        LOOP(15)
            READ_INT(LW(5))
            CALL(evt_mapobj_rotate, 0, LW(5), 0, 0, LW(0))
        END_LOOP
        SET_INT_READ_ADDR(PTR(haguruma_kage))
        LOOP(12)
            READ_INT(LW(5))
            CALL(evt_mapobj_rotate, 0, LW(5), 0, 0, LW(0))
        END_LOOP
        SET_INT_READ_ADDR(PTR(haguruma_hontai2))
        LOOP(6)
            READ_INT(LW(5))
            CALL(evt_mapobj_rotate, 0, LW(5), 0, 0, LW(1))
        END_LOOP
        SET_INT_READ_ADDR(PTR(haguruma_kage2))
        LOOP(5)
            READ_INT(LW(5))
            CALL(evt_mapobj_rotate, 0, LW(5), 0, 0, LW(1))
        END_LOOP
        CALL(evt_sub_get_sincos, LW(2), LW(5), LW(6))
        MULTIPLY_FLOAT(LW(5), FLOAT(41.0f))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_21a"), 0, 0, LW(5))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_21b"), 0, 0, LW(5))
        CALL(evt_sub_get_sincos, LW(3), LW(5), LW(6))
        MULTIPLY_FLOAT(LW(5), FLOAT(22.0f))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_22a"), 0, 0, LW(5))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_22b"), 0, 0, LW(5))
        CALL(evt_sub_get_sincos, LW(4), LW(5), LW(6))
        MULTIPLY_FLOAT(LW(5), FLOAT(110.0f))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_23a"), 0, 0, LW(5))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_23b"), 0, 0, LW(5))
        ADD_FLOAT(LW(0), FLOAT(1.0f))
        IF_INT_GREATER_OR_EQUAL(LW(0), 360)
            SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
        END_IF
        ADD_FLOAT(LW(1), FLOAT(-1.0f))
        IF_INT_LESS_OR_EQUAL(LW(1), -360)
            ADD_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        ADD_FLOAT(LW(2), FLOAT(0.9f))
        IF_INT_GREATER_OR_EQUAL(LW(2), 360)
            SUBTRACT_FLOAT(LW(2), FLOAT(360.0f))
        END_IF
        ADD_FLOAT(LW(3), FLOAT(0.9f))
        IF_INT_GREATER_OR_EQUAL(LW(3), 360)
            SUBTRACT_FLOAT(LW(3), FLOAT(360.0f))
        END_IF
        ADD_FLOAT(LW(4), FLOAT(1.0f))
        IF_INT_GREATER_OR_EQUAL(LW(4), 360)
            SUBTRACT_FLOAT(LW(4), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(cloud_blk_evt)
    SET_VALUE(GSWF(4356), 1)
    CALL(evt_mobj_flag_onoff, 0, STRING("blk"), 256)
    RETURN
EVT_END()

EVT_BEGIN(break_floor_evt)
    SET_VALUE(GSW(0), 396)
    RETURN
EVT_END()

EVT_BEGIN(jump_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            LOOP(90)
                CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
                IF_INT_GREATER_OR_EQUAL(LW(1), -201)
                    IF_INT_LESS_OR_EQUAL(LW(1), -199)
                        RETURN
                    END_IF
                END_IF
                CALL(evt_party_get_name_hitobj_ride, 0, LW(0))
                IF_STR_EQUAL(LW(0), STRING("MOBJ_jump"))
                    CALL(evt_party_jump_pos, 0, -455, -200, 50, 600, 0, -1000)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_IF
    END_INLINE_EVENT
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_jump_jumpstand, -500, -200, 0, -545, 100, 0, -450, -200, 0, 1000, 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(dai_evt)
    SET_VALUE(LW(0), STRING("S_move_dai"))
    SET_VALUE(LW(1), STRING("A_move_dai"))
    SET_VALUE(LW(2), 7200)
    CALL_CHILD(move_dai)
    RETURN
EVT_END()

static EVT_BEGIN(box_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("key"), 48, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("key"))
    SET_VALUE(GSW(0), 397)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(las_24_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST3"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS7"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hgm_05a"), 2)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hgm_19a"), 2)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hgm_24a"), 2)
    SPAWN_EVENT(haguruma_evt)
    CALL(evt_map_playanim, STRING("kusari"), 1, 0)
    IF_INT_LESS(GSW(0), 396)
        IF_INT_EQUAL(GSWF(4356), 0)
            CALL(evt_cloud_init)
            CALL(evt_cloud_ent, 0, STRING("S_hako"), STRING("A_hako"), PTR(cloud_blk_evt))
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hako"), 1)
            CALL(evt_hitobj_onoff, STRING("A_hako"), 1, 0)
        END_IF
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hako"), 1)
        CALL(evt_hitobj_onoff, STRING("A_hako"), 1, 0)
    END_IF
    CALL(evt_mobj_lv_blk, STRING("blk"), -495, 100, 0, 0, GSWF(4357), STRING("MOBJ_Lv2BigBigBlock"))
    IF_INT_EQUAL(GSWF(4357), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("blk"), 64)
        IF_INT_EQUAL(GSWF(4356), 0)
            CALL(evt_mobj_flag_onoff, 1, STRING("blk"), 256)
            CALL(evt_mobj_set_scale, STRING("blk"), FLOAT(0.99805f), FLOAT(0.99805f), FLOAT(0.99805f))
        END_IF
    END_IF
    CALL(evt_mobj_breaking_floor, STRING("brkflr"), -500, 100, 0, 0, PTR(break_floor_evt), GSWF(4358))
    CALL(evt_mobj_jumpstand_red, 0, STRING("jump"), -500, -200, 0, PTR(jump_evt), 0)
    CALL(evt_hit_bind_mapobj, STRING("A_move_dai"), STRING("S_move_dai"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_move_dai"), 1)
    CALL(evt_hitobj_onoff, STRING("A_move_dai"), 0, 0)
    CALL(evt_mobj_floatswitch_red, 0, STRING("dai_switch"), -150, -440, 75, PTR(dai_evt), 0)
    CALL(evt_mobj_badgeblk, STRING("mobj_hatena_gat"), -625, -200, -100, 121, 0, GSWF(4390), 0)
    CALL(evt_mobj_brick, STRING("mobj_kakusi"), -625, -115, -100, 161, 11, 0, GSWF(4391))
    CALL(evt_mobj_itembox, STRING("box"), -255, -380, 75, 0, 0, PTR(box_evt), GSWF(4370))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

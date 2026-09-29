#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 move_dai[];

// clang-format off
extend_data(92,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(93,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door_dummy)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[4] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_20", "e_bero", 5, 5, w_door_close, w_door_open},
    {"e_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_24", "w_bero", 5, 5, e_door_close, e_door_open},
    {"s_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_23", "n_bero_1", 1, 1},
    {NULL},
};

// TODO: type
static u32 haguruma_hontai[13] = {
    (u32)"S_hgm_01a", (u32)"S_hgm_02a", (u32)"S_hgm_03a", (u32)"S_hgm_04a",
    (u32)"S_hgm_06a", (u32)"S_hgm_07a", (u32)"S_hgm_08a", (u32)"S_hgm_10a",
    (u32)"S_hgm_12a", (u32)"S_hgm_14a", (u32)"S_hgm_16a", (u32)"S_hgm_19a",
    (u32)"S_hgm_20a",
};

// TODO: type
static u32 haguruma_hontai2[7] = {
    (u32)"S_hgm_05a", (u32)"S_hgm_09a", (u32)"S_hgm_11a", (u32)"S_hgm_13a",
    (u32)"S_hgm_15a", (u32)"S_hgm_17a", (u32)"S_hgm_18a",
};

// TODO: type
static u32 haguruma_kage[11] = {
    (u32)"S_hgm_01b", (u32)"S_hgm_02b", (u32)"S_hgm_03b", (u32)"S_hgm_04b",
    (u32)"S_hgm_06b", (u32)"S_hgm_07b", (u32)"S_hgm_08b", (u32)"S_hgm_10b",
    (u32)"S_hgm_12b", (u32)"S_hgm_14b", (u32)"S_hgm_16b",
};

// TODO: type
static u32 haguruma_kage2[6] = {
    (u32)"S_hgm_05b", (u32)"S_hgm_09b", (u32)"S_hgm_11b", (u32)"S_hgm_13b",
    (u32)"S_hgm_15b", (u32)"S_hgm_17b",
};

static EVT_BEGIN(haguruma_evt)
    SET_FLOAT_VALUE(LW(0), FLOAT(20.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(4), FLOAT(0.0f))
    LOOP(0)
        SET_INT_READ_ADDR(PTR(haguruma_hontai))
        LOOP(13)
            READ_INT(LW(5))
            CALL(evt_mapobj_rotate, 0, LW(5), 0, 0, LW(0))
        END_LOOP
        SET_INT_READ_ADDR(PTR(haguruma_kage))
        LOOP(11)
            READ_INT(LW(5))
            CALL(evt_mapobj_rotate, 0, LW(5), 0, 0, LW(0))
        END_LOOP
        SET_INT_READ_ADDR(PTR(haguruma_hontai2))
        LOOP(7)
            READ_INT(LW(5))
            CALL(evt_mapobj_rotate, 0, LW(5), 0, 0, LW(1))
        END_LOOP
        SET_INT_READ_ADDR(PTR(haguruma_kage2))
        LOOP(6)
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

EVT_BEGIN(kaitendai_evt)
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(0.01f))
    CALL(evt_hit_bind_mapobj, STRING("A_hgm_dai"), STRING("S_hgm_dai"))
    CALL(evt_hit_bind_mapobj, STRING("A_hgm_dai_02"), STRING("S_hgm_dai_02"))
    CALL(evt_hit_bind_mapobj, STRING("A_hgm_dai_03"), STRING("S_hgm_dai_03"))
    CALL(evt_hit_bind_mapobj, STRING("A_hgm_dai_04"), STRING("S_hgm_dai_04"))
    CALL(evt_hit_bind_mapobj, STRING("A_hgm_dai_05"), STRING("S_hgm_dai_05"))
    CALL(evt_mapobj_get_position, STRING("A_hgm_dai_01"), LW(13), LW(14), LW(15))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_FLOOR_ROUND_START1"), LW(13), LW(14), LW(15), 0)
    LOOP(0)
        SET_VALUE(LF(0), 0)
        SET_VALUE(LF(1), 0)
        CALL(evt_mario_get_name_hitobj_ride, LW(2))
        IF_STR_EQUAL(LW(2), STRING("A_hgm_dai_02"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_02"), LW(3), LW(4), LW(5))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(2), STRING("A_hgm_dai_03"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_03"), LW(3), LW(4), LW(5))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(2), STRING("A_hgm_dai_04"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_04"), LW(3), LW(4), LW(5))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(2), STRING("A_hgm_dai_05"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_05"), LW(3), LW(4), LW(5))
            SET_VALUE(LF(0), 1)
        END_IF
        CALL(evt_party_get_name_hitobj_ride, 0, LW(6))
        IF_STR_EQUAL(LW(6), STRING("A_hgm_dai_02"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_02"), LW(7), LW(8), LW(9))
            SET_VALUE(LF(1), 1)
        END_IF
        IF_STR_EQUAL(LW(6), STRING("A_hgm_dai_03"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_03"), LW(7), LW(8), LW(9))
            SET_VALUE(LF(1), 1)
        END_IF
        IF_STR_EQUAL(LW(6), STRING("A_hgm_dai_04"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_04"), LW(7), LW(8), LW(9))
            SET_VALUE(LF(1), 1)
        END_IF
        IF_STR_EQUAL(LW(6), STRING("A_hgm_dai_05"))
            CALL(evt_hit_get_position, STRING("A_hgm_dai_05"), LW(7), LW(8), LW(9))
            SET_VALUE(LF(1), 1)
        END_IF
        CALL(evt_mapobj_rotate, 1, STRING("S_hgm_dai"), 0, 0, LW(0))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_dai_02"), 0, 0, LW(1))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_dai_03"), 0, 0, LW(1))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_dai_04"), 0, 0, LW(1))
        CALL(evt_mapobj_rotate, 0, STRING("S_hgm_dai_05"), 0, 0, LW(1))
        CALL(evt_hit_bind_update, STRING("A_hgm_dai"))
        CALL(evt_hit_bind_update, STRING("A_hgm_dai_02"))
        CALL(evt_hit_bind_update, STRING("A_hgm_dai_03"))
        CALL(evt_hit_bind_update, STRING("A_hgm_dai_04"))
        CALL(evt_hit_bind_update, STRING("A_hgm_dai_05"))
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_hit_get_position, LW(2), LW(10), LW(11), LW(12))
            SUBTRACT_FLOAT(LW(10), LW(3))
            SUBTRACT_FLOAT(LW(11), LW(4))
            SUBTRACT_FLOAT(LW(12), LW(5))
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            ADD_FLOAT(LW(3), LW(10))
            ADD_FLOAT(LW(4), LW(11))
            ADD_FLOAT(LW(5), LW(12))
            CALL(evt_mario_set_pos, LW(3), LW(4), LW(5))
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_hit_get_position, LW(6), LW(10), LW(11), LW(12))
            SUBTRACT_FLOAT(LW(10), LW(7))
            SUBTRACT_FLOAT(LW(11), LW(8))
            SUBTRACT_FLOAT(LW(12), LW(9))
            CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
            ADD_FLOAT(LW(3), LW(10))
            ADD_FLOAT(LW(4), LW(11))
            ADD_FLOAT(LW(5), LW(12))
            CALL(evt_party_set_pos, 0, LW(3), LW(4), LW(5))
        END_IF
        ADD_FLOAT(LW(0), FLOAT(0.5f))
        IF_INT_GREATER(LW(0), 360)
            SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
        END_IF
        ADD_FLOAT(LW(1), FLOAT(-0.5f))
        IF_INT_LESS(LW(1), -360)
            ADD_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dai_evt)
    SET_VALUE(LW(0), STRING("S_move_dai"))
    SET_VALUE(LW(1), STRING("A_move_dai"))
    SET_VALUE(LW(2), 2200)
    CALL_CHILD(move_dai)
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x0000002F, 0xFFFFFFFF,
};

static EVT_BEGIN(key_evt)
    SET_VALUE(GSW(0), 393)
    RETURN
EVT_END()

static EVT_BEGIN(key_check_evt)
    CALL(evt_pouch_check_item, 47, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(unlock_evt)
    SET_VALUE(GSW(0), 394)
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 130, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(las_22_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST3"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS6"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_LESS(GSW(0), 390)
        CALL(evt_npc_check_delete, STRING("スーパーマホーン"))
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 395)
        SPAWN_EVENT(kaitendai_evt)
    END_IF
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hgm_01a"), 2)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hgm_05a"), 2)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hgm_08a"), 2)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hgm_19a"), 2)
    SPAWN_EVENT(haguruma_evt)
    CALL(evt_map_playanim, STRING("kusari"), 1, 0)
    CALL(evt_hit_bind_mapobj, STRING("A_move_dai"), STRING("S_move_dai"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_move_dai"), 1)
    CALL(evt_hitobj_onoff, STRING("A_move_dai"), 0, 0)
    CALL(evt_mobj_switch_red, 0, STRING("dai_switch"), 100, -409, -10, PTR(dai_evt), 0)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), -660, -500, 100, 0, 0, PTR(mobj_box00), GSWF(4377))
    CALL(evt_mobj_badgeblk, STRING("mobj_hatena_gat"), -150, 220, -100, 151, 0, GSWF(4378), 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -650, 100, -100, 0, 0)
    IF_INT_LESS(GSW(0), 393)
        CALL(evt_item_entry, STRING("key"), 47, -25, -80, -100, 16, -1, PTR(key_evt))
    END_IF
    IF_INT_LESS(GSW(0), 394)
        CALL(evt_mobj_lock, STRING("lock"), 47, -695, 50, 0, 90, PTR(key_check_evt), PTR(unlock_evt), GSWF(4355))
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

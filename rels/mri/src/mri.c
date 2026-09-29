#include "mri.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "rel/mri/mri.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_puni_all_save[];
extern s32 evt_puni_get_lsw_index[];
extern s32 evt_puni_get_mode[];
extern s32 evt_puni_get_trap_index[];
extern s32 evt_puni_init_puni[];
extern s32 evt_puni_set_lsw_index[];
extern s32 evt_puni_set_position[];
extern s32 evt_puni_set_trap_index[];

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

EVT_BEGIN(evt_mri_door_open_pure)
    IF_INT_EQUAL(LW(9), 0)
        SET_VALUE(LW(2), -5)
    ELSE
        SET_VALUE(LW(2), 5)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 0, LW(2), 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 0, -60, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(2), 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_open)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_bero_mario_go)
        CALL(evt_bero_exec_onoff, 0, 268435456)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_open_pure)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_close_pure)
    IF_INT_EQUAL(LW(9), 0)
        SET_VALUE(LW(2), -5)
    ELSE
        SET_VALUE(LW(2), 5)
    END_IF
    CALL(evt_mapobj_trans, 1, LW(10), LW(2), 0, -60)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_ELEC2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, -60, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(2), 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, LW(2), 0, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_close)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_close_pure)
    RETURN
EVT_END()

// TODO: write evt_mri_door_naname_func (global, 0x1b0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_mri_door_naname_open_pure)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(0.0f), FLOAT(0.0f), FLOAT(6.5f), FLOAT(-6.5f), 200, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(6.5f), FLOAT(-6.5f), FLOAT(-34.5f), FLOAT(-46.5f), 500, 11)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_naname_open)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_naname_open_pure)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_naname_close_pure)
    CALL(evt_mapobj_trans, 1, LW(10), FLOAT(-34.5f), FLOAT(0.0f), FLOAT(-46.5f))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_ELEC2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(-34.5f), FLOAT(-46.5f), FLOAT(6.5f), FLOAT(-6.5f), 500, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(6.5f), FLOAT(-6.5f), FLOAT(0.0f), FLOAT(0.0f), 200, 11)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_naname_close)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_naname_close_pure)
    RETURN
EVT_END()

EVT_BEGIN(_ueki_anime)
    CALL(evt_mapobj_get_position, LW(2), LW(4), LW(5), LW(6))
    CALL(evt_snd_sfxon_3d, STRING("SFX_GRASS_SHAKE1"), LW(4), LW(5), LW(6), 0)
    CALL(evt_mapobj_trans, 1, LW(2), 2, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), -2, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), 1, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), -1, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), 0, 0, 0)
    IF_INT_NOT_EQUAL(LW(10), 0)
        SPAWN_EVENT(LW(10))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(mri_ueki_entry)
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT3(LW(1), LW(2), LW(10))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 9, 1, LW(1), 0, PTR(_ueki_anime), 0)
        CALL(evt_run_case_evt, 13, 1, LW(1), 0, PTR(_ueki_anime), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_save)
    DEBUG_PUT_MSG(STRING("プニの状態をセーブします"))
    CALL(evt_puni_all_save)
    RETURN
EVT_END()

// TODO: type
u32 mri_itemsel_table[2] = {
    0x00000040, 0xFFFFFFFF,
};

EVT_BEGIN(mri_punipuni_setup)
    CALL(evt_item_entry, LW(10), 64, 0, -20000, 0, 16, -1, 0)
    CALL(evt_item_flag_on, LW(10), 2)
    RETURN
EVT_END()

EVT_BEGIN(mri_punipuni_itemselect)
    SET_VALUE(LW(1), 0)
    CALL(evt_pouch_check_item, 64, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("sys_no_item_to_put_on"), 0, 0)
        RETURN
    END_IF
    CALL(evt_win_item_select, 0, PTR(mri_itemsel_table), LW(0), 0)
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    SET_VALUE(LW(1), 1)
    RETURN
EVT_END()

EVT_BEGIN(mri_punipuni_itemset)
    CALL(evt_target_flag_onoff, LW(11), 0, 3)
    CALL(evt_pouch_remove_item, 64, LW(0))
    CALL(evt_item_get_position, LW(10), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNIDAMA_SET1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("sys_stg2_punitama_on"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(mri_punipuni_itemremove)
    CALL(evt_target_flag_onoff, LW(11), 1, 1)
    CALL(evt_pouch_add_item, 64, LW(0))
    CALL(evt_item_set_position, LW(10), 0, -20000, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("sys_stg2_punitama_off"), 0, 0)
    RETURN
EVT_END()

// TODO: write hakari_color (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(mri_hikari_color)
    CALL(evt_mapobj_flag_onoff, 0, 0, LW(0), 256)
    GET_INT_INDEXED(LW(1), LW(11))
    LOOP(0)
        GET_INT_INDEXED(LW(1), LW(10))
        IF_INT_LESS_OR_EQUAL(LW(10), LW(2))
            IF_INT_GREATER(LW(11), LW(2))
                CALL(evt_mapobj_get_position, LW(0), LW(4), LW(5), LW(6))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNISWITCH_RIDE1"), LW(4), LW(5), LW(6), 0)
            END_IF
        ELSE
            IF_INT_LESS_OR_EQUAL(LW(11), LW(2))
                CALL(evt_mapobj_get_position, LW(0), LW(4), LW(5), LW(6))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNISWITCH_RIDE1"), LW(4), LW(5), LW(6), 0)
            END_IF
        END_IF
        SET_VALUE(LW(11), LW(10))
        CALL(hakari_color, LW(0), LW(10), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_countdown)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 118)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2880), 1)
        CALL(evt_sub_countdown_start, 300000, 300000)
        SET_VALUE(GSWF(2880), 0)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("mri_13"))
        CALL(evt_sub_countdown_set_restart_time, 300000)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("mri_06"))
        CALL(evt_sub_countdown_set_restart_time, 250000)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("mri_01"))
        CALL(evt_sub_countdown_set_restart_time, 100000)
    END_IF
    LOOP(0)
        CALL(evt_sub_countdown_get_status, LW(0))
        CALL(evt_bero_exec_get, LW(1))
        IF_INT_EQUAL(LW(0), 1)
            IF_BITS_CLEAR(LW(1), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SUSPEND_TYPES_OTHER(4)
    CALL(evt_mario_key_onoff, 0)
    WAIT_FRAMES(5)
    IF_BITS_SET(LW(1), 1)
        CALL(evt_mario_key_onoff, 1)
        RESUME_TYPES_OTHER(4)
        RETURN
    END_IF
    SET_VALUE(GSW(0), 127)
    CALL(evt_snd_bgmoff_f, 512, 500)
    CALL(evt_snd_envoff_f, 512, 500)
    CALL(evt_snd_sfxon, STRING("SFX_STG2_QUAKE1"), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 500)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 500)
    CALL(evt_cam_shake, 4, FLOAT(0.015f), FLOAT(0.0f), 500)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 10000)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(1), 150)
        CALL(evt_cam_get_pos, 4, LW(10), LW(11), LW(12))
        LOOP(0)
            CALL(evt_eff64, STRING(""), STRING("hokori_n64"), 0, LW(10), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_fade_entry, 17, 4000, 0, 0, 0)
    WAIT_MS(4000)
    CALL(evt_seq_set_seq, 5, 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(mri_waterfall_effect)
    ADD(LW(1), -10)
    LOOP(0)
        CALL(evt_eff64, STRING(""), STRING("water_damage_n64"), 15, LW(0), LW(1), LW(2), LW(3), 18, 0, 0, 0, 0, 0, 0)
        WAIT_MS(150)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_guide_hagure_check)
    CALL(evt_puni_get_lsw_index, LSW(0), 0, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_puni_get_trap_index, 1, LW(4))
    SET_VALUE(LW(1), 0)
    IF_INT_NOT_EQUAL(LW(4), 0)
        SET_VALUE(LW(1), 3)
        DEBUG_PUT_MSG(STRING("ガイド・トラップにはまっている"))
    END_IF
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(1), 1)
        DEBUG_PUT_MSG(STRING("ガイド・別のエリアにはぐれている"))
    END_IF
    CALL(evt_puni_get_mode, 1, LW(0))
    IF_INT_EQUAL(LW(0), 16)
        SET_VALUE(LW(1), 2)
        DEBUG_PUT_MSG(STRING("ガイド・同じエリアではぐれている"))
    END_IF
    DEBUG_PUT_MSG(STRING("ガイド・普通の状態"))
    RETURN
EVT_END()

EVT_BEGIN(mri_guide_goryu)
    CALL_CHILD(mri_guide_hagure_check)
    IF_INT_EQUAL(LW(1), 0)
        RETURN
    END_IF
    CALL(evt_puni_set_lsw_index, LSW(0), 0, 0, 0, 0, 0)
    CALL(evt_puni_set_trap_index, 1, 0)
    CALL(evt_puni_init_puni, 1, 30, 0, -1000, 0, 0)
    CALL(evt_puni_flag_onoff, 1, 1, 65536)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_get_dir, LW(3))
    ADD(LW(2), -10)
    IF_INT_LESS_OR_EQUAL(LW(0), 180)
        ADD(LW(0), -10)
    ELSE
        ADD(LW(0), 10)
    END_IF
    CALL(evt_puni_set_position, 1, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_walk_se)
    LOOP(0)
        CALL(evt_npc_get_position, LW(0), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_MOVE2_1"), LW(10), LW(11), LW(12), 0)
        WAIT_FRAMES(5)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_jump_se)
    CALL(evt_npc_get_position, LW(0), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    RETURN
EVT_END()

// TODO: write _prolog (global, 0x20c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_mri_get16bitdata (global, 0x5c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write mri_npc_buruburu (global, 0x234 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write toge_searchParent (global, 0xd0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write togeModeParent (global, 0xcc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write togeMoveParent (global, 0x20 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write puniModeWinjump (global, 0x164 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write togeMakeRetsu (global, 0x188 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_mri_toge_make_retsu (global, 0x24c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_mri_toge_retsu_stop (global, 0xbc bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_mri_toge_enter_init (global, 0x94 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_mri_puni_win_init (global, 0x8c bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_mri_puni_win_attack (global, 0x260 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_mri_puni_win_end (global, 0x2b0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri.s; this is where the original has it, just before the first script that uses it
// clang-format on

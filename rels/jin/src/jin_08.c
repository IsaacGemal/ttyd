#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(e_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door07"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door07"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_roll_in)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -50)
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(120.0f))
    ADD(LW(0), -50)
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(60.0f))
    ADD(LW(0), -50)
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(30.0f))
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"e_bero_1", 1, 2, 0x00004E20, 0x000186A0, 0, 0, -1, NULL, -1, NULL,
     "jin_07", "w_bero_1", 5, 5, &e_close, &e_open},
    {"e_bero_2", 0, 0, 2, 0x000000C8, 0x0000004B, -50, 120, NULL, -1, NULL,
     "jin_07", "w_bero_2", 1, 1, 0, 0},
    {NULL},
};

static EVT_BEGIN(evt_open)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_get_position, STRING("hako"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_TBOX_OPEN1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_sub_intpl_msec_init, 11, 900, 0, 800)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa"), LW(0), 0, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("hako"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("n_character"), 68, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_item_get_item, STRING("n_character"))
    SET_VALUE(GSW(0), 209)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_open_kagi)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_get_position, STRING("hako2"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_TBOX_OPEN1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -900, 800)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa2"), LW(0), 0, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("hako2"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("key00"), 23, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_item_get_item, STRING("key00"))
    SET_VALUE(GSWF(2230), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock)
    SET_VALUE(LW(0), STRING("e_bero_1"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_pouch_remove_item, 23, LW(0))
    RETURN
EVT_END()

EVT_BEGIN(oumu_init)
    RETURN
EVT_END()

EVT_BEGIN(oumu_talk)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    SWITCH(GSW(0))
        CASE_LESS(213)
        SWITCH(LSW(2))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 0, STRING("stg4_jin_40"), 0, STRING("オウム"))
            ADD(LSW(2), 1)
            CASE_EQUAL(1)
            CALL(evt_msg_print, 0, STRING("stg4_jin_41"), 0, STRING("オウム"))
            ADD(LSW(2), 1)
            CASE_EQUAL(2)
            CALL(evt_msg_print, 0, STRING("stg4_jin_42"), 0, STRING("オウム"))
        END_SWITCH
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_jin_48_01"), 0, STRING("オウム"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_jin_48_01_1"), 0, STRING("オウム"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(oumu_juyo_talk)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    IF_INT_EQUAL(GSWF(2232), 0)
        CALL(evt_msg_print, 0, STRING("stg4_jin_43"), 0, STRING("オウム"))
        SET_VALUE(GSWF(2232), 1)
        SET_VALUE(LSWF(1), 1)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg4_jin_44"), 0, STRING("オウム"))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(oumu_check)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("オウム"), LW(3), LW(4), LW(5))
        CALL(evt_sub_area_check, 1, LW(3), LW(5), 50, LW(0), LW(2), LW(6))
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 6)
            SET_VALUE(LF(1), 1)
        ELSE
            CALL(evt_party_get_status, 6, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 2)
                SET_VALUE(LF(1), 1)
            ELSE
                SET_VALUE(LF(1), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            IF_INT_EQUAL(LW(6), 1)
                IF_INT_EQUAL(LF(0), 0)
                    CALL(evt_mario_balloon_tenten)
                    SET_VALUE(LF(0), 1)
                END_IF
            ELSE
                IF_INT_EQUAL(LF(0), 1)
                    CALL(evt_mario_balloon_off)
                    SET_VALUE(LF(0), 0)
                END_IF
            END_IF
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_key_get_buttontrg, 0, LW(0))
                IF_BITS_SET(LW(0), 256)
                    CALL(evt_mario_balloon_off)
                    SET_VALUE(LF(0), 0)
                    CALL(evt_mario_key_onoff, 0)
                    CALL_CHILD(oumu_talk)
                    CALL(evt_mario_key_onoff, 1)
                END_IF
            END_IF
        ELSE
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_mario_balloon_off)
                SET_VALUE(LF(0), 0)
            END_IF
            IF_INT_EQUAL(LW(6), 1)
                IF_INT_GREATER_OR_EQUAL(GSW(0), 206)
                    IF_INT_LESS(GSW(0), 215)
                        CALL(evt_mario_key_onoff, 0)
                        CALL(evt_party_cont_onoff, 0, 0)
                        CALL(mainwindow_opendisable)
                        WAIT_MS(1000)
                        CALL_CHILD(oumu_juyo_talk)
                        CALL(evt_party_cont_onoff, 0, 1)
                        LOOP(0)
                            CALL(evt_party_get_status, 6, LW(0))
                            IF_INT_EQUAL(LW(0), 0)
                                BREAK_LOOP
                            END_IF
                            WAIT_FRAMES(1)
                        END_LOOP
                        CALL(evt_msg_toge, 1, 0, 0, 0)
                        CALL(evt_msg_print, 0, STRING("stg4_jin_45"), 0, STRING("オウム"))
                        CALL(evt_mario_key_onoff, 1)
                        CALL(mainwindow_openenable)
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"オウム", 0x40000600, 0, &oumu_init, 0, &oumu_talk},
    {NULL},
};

// TODO: type
static u32 lock_tbl[2] = {
    0x00000017, 0xFFFFFFFF,
};

static EVT_BEGIN(lock_item_check)
    CALL(evt_pouch_check_item, 23, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(lock_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box01)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box01"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item01"), 260, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item01"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box01"))
    CALL(evt_item_get_item, STRING("item01"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box02)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box02"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item02"), 147, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item02"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box02"))
    CALL(evt_item_get_item, STRING("item02"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(jin_08_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_LESS(GSW(0), 209)
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa"), 90, 0, 0)
        CALL(evt_run_case_evt, 9, 1, STRING("a_isu"), 0, PTR(evt_open), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa"), 0, 0, 0)
    END_IF
    IF_INT_EQUAL(GSWF(2230), 0)
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa2"), 0, 0, 0)
        CALL(evt_run_case_evt, 9, 1, STRING("a_hoko2"), 0, PTR(evt_open_kagi), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa2"), -90, 0, 0)
    END_IF
    CALL(evt_mobj_lock, STRING("mobj_lock00"), 23, 195, 10, 25, 270, PTR(lock_item_check), PTR(evt_unlock), GSWF(2231))
    IF_INT_EQUAL(GSWF(2231), 1)
        SET_VALUE(LW(0), STRING("e_bero_1"))
        CALL_CHILD(bero_case_switch_on)
    ELSE
        SET_VALUE(LW(0), STRING("e_bero_1"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 115, 0, 105, STRING("group87"), 125, GSWF(5637))
    CALL(evt_mobj_itembox, STRING("mobj_box01"), 60, 0, -65, 0, 0, PTR(mobj_box01), GSWF(2239))
    CALL(evt_mobj_itembox, STRING("mobj_box02"), 120, 0, -65, 0, 0, PTR(mobj_box02), GSWF(2240))
    SPAWN_EVENT(oumu_check)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

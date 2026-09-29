#include "aji_06.h"
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
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_emergency[];
s32 peach_evt_pato_on[46];
s32 peach_evt_pato_off[18];
s32 peach_evt_tyogo_entry[6];
s32 peach_evt_wait_memo[18];
u32 g_Kusuri1_No[1];

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

static EVT_BEGIN(s_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.16602f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_2"), LW(0), 0, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_1"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.16602f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_2"), LW(0), 0, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_1"), LW(0), 0, 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"s_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_01", "n_bero_1", 5, 5, s_door_close, s_door_open},
    {NULL},
};

static EVT_BEGIN(bero_peach_evt)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(GSW(0), 262)
        SET_VALUE(LSWF(11), 1)
        CALL(evt_msg_print, 0, STRING("peach_aji_226"), 0, 0)
        SET_VALUE(LSWF(11), 0)
        SET_VALUE(LW(0), 1)
    ELSE
        SET_VALUE(LW(0), 0)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static BeroEntry bero_peach_data[2] = {
    {"s_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, bero_peach_evt, "aji_01", "n_bero_1", 5, 5, s_door_close, s_door_open},
    {NULL},
};

// TODO: type
u8 g_Kusuri_Col[4] = {
    0x00, 0x00, 0x00, 0xFF,
};

// TODO: type
u32 g_Kusuri_Src_Col[4] = {
    0xFF0000FF, 0x0000FFFF, 0xFFFF00FF, 0x00FF00FF,
};

EVT_BEGIN(g_Kusuri2_No)
EVT_END()

// TODO: type
u32 g_Kusuri3_No[1] = {
    0x00000002,
};

// TODO: type
u32 g_Kusuri4_No[1] = {
    0x00000003,
};

EVT_BEGIN(peach_evt_kaihei_open)
    SET_VALUE(LW(5), LW(0))
    CALL(evt_mapobj_get_position, LW(5), LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(1), 45)
        RETURN
    END_IF
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_OPEN1"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_sub_intpl_msec_init, 11, 0, -25, 800)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 0, LW(5), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(3))
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kaihei_open_all)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_kaihei1"))
        CALL_CHILD(peach_evt_kaihei_open)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_kaihei2"))
        CALL_CHILD(peach_evt_kaihei_open)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_kaihei3"))
        CALL_CHILD(peach_evt_kaihei_open)
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("S_kaihei4"))
    CALL_CHILD(peach_evt_kaihei_open)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kaihei_close)
    SET_VALUE(LW(5), LW(0))
    CALL(evt_mapobj_get_position, LW(5), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_OPEN1"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_sub_intpl_msec_init, 11, -25, 0, 800)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 0, LW(5), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_snd_sfxoff, LW(3))
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kaihei_close_all)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_kaihei1"))
        CALL_CHILD(peach_evt_kaihei_close)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_kaihei2"))
        CALL_CHILD(peach_evt_kaihei_close)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_kaihei3"))
        CALL_CHILD(peach_evt_kaihei_close)
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("S_kaihei4"))
    CALL_CHILD(peach_evt_kaihei_close)
    RETURN
EVT_END()

// TODO: type
u32 kusuri_tyogo_list[8] = {
    0x00000001, 0x00000002, 0x00000003, 0x00000002,
    0x00000003, 0x00000001, 0x00000003, 0x00000004,
};

// TODO: type
u32 kusuri_list[5] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF,
};

// TODO: type
u32 kusuri_id[4] = {
    0x00000066, 0x00000065, 0x00000067, 0x00000068,
};

// TODO: write kusuri_list_get_no (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write kusuri_list_add (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write kusuri_list_init (global, 0x2c bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(peach_evt_kusuri_select)
    CALL(kusuri_list_init)
    SET_VALUE(LF(0), 0)
    IF_INT_NOT_EQUAL(LSWF(2), 0)
        CALL(kusuri_list_add, 2)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_NOT_EQUAL(LSWF(1), 0)
        CALL(kusuri_list_add, 1)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_NOT_EQUAL(LSWF(3), 0)
        CALL(kusuri_list_add, 3)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_NOT_EQUAL(LSWF(4), 0)
        CALL(kusuri_list_add, 4)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_win_item_select, 0, PTR(kusuri_list), LW(1), 0)
    ELSE
        SET_VALUE(LW(1), -1)
    END_IF
    IF_INT_NOT_EQUAL(LW(1), -1)
        CALL(kusuri_list_get_no, LW(1), LW(0))
    ELSE
        SET_VALUE(LW(0), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_pick_kusuri)
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LSWF(1), 1)
        CALL(evt_item_delete, STRING("kusuri_aka"))
        CASE_EQUAL(2)
        SET_VALUE(LSWF(2), 1)
        CALL(evt_item_delete, STRING("kusuri_ao"))
        CASE_EQUAL(3)
        SET_VALUE(LSWF(3), 1)
        CALL(evt_item_delete, STRING("kusuri_ki"))
        CASE_EQUAL(4)
        SET_VALUE(LSWF(4), 1)
        CALL(evt_item_delete, STRING("kusuri_mid"))
    END_SWITCH
    IF_INT_EQUAL(LSWF(5), 1)
        CALL(evt_del_case_evt, 0, GW(5))
        DELETE_EVENT(GW(0))
        SET_VALUE(LW(0), STRING("S_pato1"))
        CALL_CHILD(peach_evt_pato_off)
    END_IF
    SET_VALUE(LSWF(5), 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_check_all_kusuri_set)
    SET_VALUE(LW(0), LSW(1))
    ADD(LW(0), LSW(2))
    ADD(LW(0), LSW(3))
    ADD(LW(0), LSW(4))
    IF_INT_EQUAL(LSWF(10), 1)
        RETURN
    END_IF
    IF_INT_EQUAL(LW(0), 10)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_print, 0, STRING("peach_aji_227"), 0, 0)
        CALL(evt_msg_select, 0, STRING("peach_aji_227_sel"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("peach_aji_229"))
            SET_VALUE(LSWF(5), 1)
            CALL(evt_run_case_evt, 9, 1, STRING("A_sui1"), 0, PTR(peach_evt_tyogo_entry), GW(5))
            SET_VALUE(LW(0), STRING("S_pato1"))
            SPAWN_EVENT_GET_ID(PTR(peach_evt_pato_on), GW(0))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("peach_aji_228"))
            SET_VALUE(LSWF(5), 0)
        END_IF
        CALL(evt_mario_key_onoff, 1)
    END_IF
    RETURN
EVT_END()

// TODO: write kusuri_color_set (global, 0x4b8 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(peach_evt_kusuri_dasu)
    SET_VALUE(LW(5), LW(0))
    SET_VALUE(LW(6), LW(1))
    CALL(evt_map_playanim, STRING("mizu"), 0, 0)
    CALL(evt_map_checkanim, STRING("mizu"), LW(0), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_MEDICINE1"), 0, 0, -100, 0)
    IF_INT_EQUAL(LSWF(7), 1)
        SET_VALUE(LW(3), LW(2))
        MULTIPLY_FLOAT(LW(3), FLOAT(0.8f))
        BEGIN_INLINE_EVENT
            CALL(kusuri_color_set, LW(6), LW(3))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_sub_intpl_msec_init, 11, 0, 125, LW(3))
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            ADD_FLOAT(LW(0), GW(10))
            CALL(evt_mapobj_scale, 0, STRING("S_naka"), 1, LW(0), 1)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(2)
            END_IF
            ADD_FLOAT(GW(10), FLOAT(0.125f))
        END_INLINE_EVENT
    END_IF
    DIVIDE(LW(2), 3)
    CALL(evt_sub_intpl_msec_init, 11, 0, 1000, LW(2))
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    CALL(evt_mapobj_scale, 0, LW(5), 1, LW(0), 1)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    WAIT_MS(LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 1000, 0, LW(2))
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    CALL(evt_mapobj_scale, 0, LW(5), 1, LW(0), 1)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    SET_VALUE(LSWF(7), 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_seikai_evt)
    LOOP(3)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
        WAIT_MS(100)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        WAIT_MS(100)
    END_LOOP
    RETURN
EVT_END()

// TODO: write peach_evt_seikai_check (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(peach_evt_kaihei1)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(LSWF(6), 0)
        IF_INT_EQUAL(LSW(1), 0)
            CALL_CHILD(peach_evt_kusuri_select)
            SET_VALUE(LSW(1), LW(0))
            SWITCH(LSW(1))
                CASE_EQUAL(1)
                SET_VALUE(LW(0), STRING("kusuri_aka"))
                SET_VALUE(LW(1), 102)
                SET_VALUE(LSWF(1), 0)
                CASE_EQUAL(2)
                SET_VALUE(LW(0), STRING("kusuri_ao"))
                SET_VALUE(LW(1), 101)
                SET_VALUE(LSWF(2), 0)
                CASE_EQUAL(3)
                SET_VALUE(LW(0), STRING("kusuri_ki"))
                SET_VALUE(LW(1), 103)
                SET_VALUE(LSWF(3), 0)
                CASE_EQUAL(4)
                SET_VALUE(LW(0), STRING("kusuri_mid"))
                SET_VALUE(LW(1), 104)
                SET_VALUE(LSWF(4), 0)
            END_SWITCH
            IF_INT_GREATER(LSW(1), 0)
                CALL(evt_item_entry, LW(0), LW(1), -110, 31, -107, 17, -1, 0)
                CALL(evt_pouch_check_item, LW(1), LW(2))
                IF_INT_GREATER(LW(2), 0)
                    CALL(evt_pouch_remove_item, LW(1), LW(2))
                END_IF
            END_IF
            SET_VALUE(LW(0), LSW(1))
            SUBTRACT(LW(0), 1)
            POKE_INT(LW(0), PTR(g_Kusuri1_No))
            CALL_CHILD(peach_evt_check_all_kusuri_set)
            CALL(evt_map_set_flush_onoff, 0, 1, STRING("S_mizu1"))
            CALL(evt_map_set_flush_color, 0, STRING("S_mizu1"), 255, 0, 0, 255)
        ELSE
            SET_VALUE(LW(0), LSW(1))
            CALL_CHILD(peach_evt_pick_kusuri)
            SET_VALUE(LSW(1), 0)
        END_IF
    ELSE
        SET_VALUE(LF(0), 0)
        IF_INT_EQUAL(GW(12), 0)
            IF_INT_EQUAL(LSWF(8), 1)
                SET_VALUE(LSWF(7), 1)
                CALL(peach_evt_seikai_check, GW(11), LSW(1), LW(0))
                IF_INT_EQUAL(LW(0), 1)
                    ADD(GW(13), 1)
                    SET_VALUE(LF(0), 1)
                ELSE
                    SUBTRACT(GW(13), 1)
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(0), STRING("S_mizu1"))
        SET_VALUE(LW(1), LSW(1))
        CALL_CHILD(peach_evt_kusuri_dasu)
        IF_INT_EQUAL(LF(0), 1)
            SET_VALUE(LW(0), STRING("hika1"))
            SET_VALUE(LW(1), STRING("kesu1"))
            SPAWN_EVENT(peach_evt_seikai_evt)
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kaihei2)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(LSWF(6), 0)
        IF_INT_EQUAL(LSW(2), 0)
            CALL_CHILD(peach_evt_kusuri_select)
            SET_VALUE(LSW(2), LW(0))
            SWITCH(LSW(2))
                CASE_EQUAL(1)
                SET_VALUE(LW(0), STRING("kusuri_aka"))
                SET_VALUE(LW(1), 102)
                SET_VALUE(LSWF(1), 0)
                CASE_EQUAL(2)
                SET_VALUE(LW(0), STRING("kusuri_ao"))
                SET_VALUE(LW(1), 101)
                SET_VALUE(LSWF(2), 0)
                CASE_EQUAL(3)
                SET_VALUE(LW(0), STRING("kusuri_ki"))
                SET_VALUE(LW(1), 103)
                SET_VALUE(LSWF(3), 0)
                CASE_EQUAL(4)
                SET_VALUE(LW(0), STRING("kusuri_mid"))
                SET_VALUE(LW(1), 104)
                SET_VALUE(LSWF(4), 0)
            END_SWITCH
            IF_INT_GREATER(LSW(2), 0)
                CALL(evt_item_entry, LW(0), LW(1), -65, 31, -107, 17, -1, 0)
                CALL(evt_pouch_check_item, LW(1), LW(2))
                IF_INT_GREATER(LW(2), 0)
                    CALL(evt_pouch_remove_item, LW(1), LW(2))
                END_IF
            END_IF
            SET_VALUE(LW(0), LSW(2))
            SUBTRACT(LW(0), 1)
            POKE_INT(LW(0), PTR(g_Kusuri2_No))
            CALL_CHILD(peach_evt_check_all_kusuri_set)
        ELSE
            SET_VALUE(LW(0), LSW(2))
            CALL_CHILD(peach_evt_pick_kusuri)
            SET_VALUE(LSW(2), 0)
        END_IF
    ELSE
        SET_VALUE(LF(0), 0)
        IF_INT_EQUAL(GW(12), 1)
            IF_INT_EQUAL(LSWF(8), 1)
                SET_VALUE(LSWF(7), 1)
                CALL(peach_evt_seikai_check, GW(11), LSW(2), LW(0))
                IF_INT_EQUAL(LW(0), 1)
                    ADD(GW(13), 1)
                    SET_VALUE(LF(0), 1)
                ELSE
                    SUBTRACT(GW(13), 1)
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(0), STRING("S_mizu2"))
        SET_VALUE(LW(1), LSW(2))
        CALL_CHILD(peach_evt_kusuri_dasu)
        IF_INT_EQUAL(LF(0), 1)
            SET_VALUE(LW(0), STRING("hika2"))
            SET_VALUE(LW(1), STRING("kesu2"))
            SPAWN_EVENT(peach_evt_seikai_evt)
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kaihei3)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(LSWF(6), 0)
        IF_INT_EQUAL(LSW(3), 0)
            CALL_CHILD(peach_evt_kusuri_select)
            SET_VALUE(LSW(3), LW(0))
            SWITCH(LSW(3))
                CASE_EQUAL(1)
                SET_VALUE(LW(0), STRING("kusuri_aka"))
                SET_VALUE(LW(1), 102)
                SET_VALUE(LSWF(1), 0)
                CASE_EQUAL(2)
                SET_VALUE(LW(0), STRING("kusuri_ao"))
                SET_VALUE(LW(1), 101)
                SET_VALUE(LSWF(2), 0)
                CASE_EQUAL(3)
                SET_VALUE(LW(0), STRING("kusuri_ki"))
                SET_VALUE(LW(1), 103)
                SET_VALUE(LSWF(3), 0)
                CASE_EQUAL(4)
                SET_VALUE(LW(0), STRING("kusuri_mid"))
                SET_VALUE(LW(1), 104)
                SET_VALUE(LSWF(4), 0)
            END_SWITCH
            IF_INT_GREATER(LSW(3), 0)
                CALL(evt_item_entry, LW(0), LW(1), -10, 31, -107, 17, -1, 0)
                CALL(evt_pouch_check_item, LW(1), LW(2))
                IF_INT_GREATER(LW(2), 0)
                    CALL(evt_pouch_remove_item, LW(1), LW(2))
                END_IF
            END_IF
            SET_VALUE(LW(0), LSW(3))
            SUBTRACT(LW(0), 1)
            POKE_INT(LW(0), PTR(g_Kusuri3_No))
            CALL_CHILD(peach_evt_check_all_kusuri_set)
        ELSE
            SET_VALUE(LW(0), LSW(3))
            CALL_CHILD(peach_evt_pick_kusuri)
            SET_VALUE(LSW(3), 0)
        END_IF
    ELSE
        SET_VALUE(LF(0), 0)
        IF_INT_EQUAL(GW(12), 2)
            IF_INT_EQUAL(LSWF(8), 1)
                SET_VALUE(LSWF(7), 1)
                CALL(peach_evt_seikai_check, GW(11), LSW(3), LW(0))
                IF_INT_EQUAL(LW(0), 1)
                    ADD(GW(13), 1)
                    SET_VALUE(LF(0), 1)
                ELSE
                    SUBTRACT(GW(13), 1)
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(0), STRING("S_mizu3"))
        SET_VALUE(LW(1), LSW(3))
        CALL_CHILD(peach_evt_kusuri_dasu)
        IF_INT_EQUAL(LF(0), 1)
            SET_VALUE(LW(0), STRING("hika3"))
            SET_VALUE(LW(1), STRING("kesu3"))
            SPAWN_EVENT(peach_evt_seikai_evt)
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kaihei4)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(LSWF(6), 0)
        IF_INT_EQUAL(LSW(4), 0)
            CALL_CHILD(peach_evt_kusuri_select)
            SET_VALUE(LSW(4), LW(0))
            SWITCH(LSW(4))
                CASE_EQUAL(1)
                SET_VALUE(LW(0), STRING("kusuri_aka"))
                SET_VALUE(LW(1), 102)
                SET_VALUE(LSWF(1), 0)
                CASE_EQUAL(2)
                SET_VALUE(LW(0), STRING("kusuri_ao"))
                SET_VALUE(LW(1), 101)
                SET_VALUE(LSWF(2), 0)
                CASE_EQUAL(3)
                SET_VALUE(LW(0), STRING("kusuri_ki"))
                SET_VALUE(LW(1), 103)
                SET_VALUE(LSWF(3), 0)
                CASE_EQUAL(4)
                SET_VALUE(LW(0), STRING("kusuri_mid"))
                SET_VALUE(LW(1), 104)
                SET_VALUE(LSWF(4), 0)
            END_SWITCH
            IF_INT_GREATER(LSW(4), 0)
                CALL(evt_item_entry, LW(0), LW(1), 35, 31, -107, 17, -1, 0)
                CALL(evt_pouch_check_item, LW(1), LW(2))
                IF_INT_GREATER(LW(2), 0)
                    CALL(evt_pouch_remove_item, LW(1), LW(2))
                END_IF
            END_IF
            SET_VALUE(LW(0), LSW(4))
            SUBTRACT(LW(0), 1)
            POKE_INT(LW(0), PTR(g_Kusuri4_No))
            CALL_CHILD(peach_evt_check_all_kusuri_set)
        ELSE
            SET_VALUE(LW(0), LSW(4))
            CALL_CHILD(peach_evt_pick_kusuri)
            SET_VALUE(LSW(4), 0)
        END_IF
    ELSE
        SET_VALUE(LF(0), 0)
        IF_INT_EQUAL(GW(12), 3)
            IF_INT_EQUAL(LSWF(8), 1)
                SET_VALUE(LSWF(7), 1)
                CALL(peach_evt_seikai_check, GW(11), LSW(4), LW(0))
                IF_INT_EQUAL(LW(0), 1)
                    ADD(GW(13), 1)
                    SET_VALUE(LF(0), 1)
                ELSE
                    SUBTRACT(GW(13), 1)
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(0), STRING("S_mizu4"))
        SET_VALUE(LW(1), LSW(4))
        CALL_CHILD(peach_evt_kusuri_dasu)
        IF_INT_EQUAL(LF(0), 1)
            SET_VALUE(LW(0), STRING("hika4"))
            SET_VALUE(LW(1), STRING("kesu4"))
            SPAWN_EVENT(peach_evt_seikai_evt)
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_stop_renji)
    IF_INT_EQUAL(GW(11), 8)
        IF_INT_EQUAL(LSWF(8), 1)
            SET_VALUE(LSWF(7), 1)
        END_IF
    END_IF
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_pato_on)
    SET_VALUE(LW(15), LW(0))
    CALL(evt_mapobj_get_position, LW(15), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_SIREN1"), LW(0), LW(1), LW(2), GW(4))
    CALL(evt_mapobj_flag_onoff, 0, 0, LW(15), 1)
    LOOP(0)
        CALL(evt_mapobj_rotate, 0, LW(15), 0, LW(0), 0)
        ADD(LW(0), 12)
        IF_INT_GREATER_OR_EQUAL(LW(0), 360)
            SUBTRACT(LW(0), 360)
        END_IF
        WAIT_MS(30)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_pato_off)
    CALL(evt_mapobj_flag_onoff, 0, 1, LW(0), 1)
    CALL(evt_mapobj_rotate, 0, LW(0), 0, 0, 0)
    CALL(evt_snd_sfxoff, GW(4))
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_hari_on)
    CALL(evt_sub_intpl_msec_init, 11, 0, -130, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 0, STRING("hari"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(LW(4), -130)
    LOOP(0)
        CALL(evt_sub_random, 15, LW(2))
        ADD(LW(2), -130)
        CALL(evt_sub_intpl_msec_init, 4, LW(4), LW(2), 600)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 0, STRING("hari"), 0, 0, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        CALL(evt_sub_random, 20, LW(3))
        SET_VALUE(LW(4), -130)
        SUBTRACT(LW(4), LW(3))
        CALL(evt_sub_intpl_msec_init, 11, LW(2), LW(4), 600)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 0, STRING("hari"), 0, 0, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_hari_off)
    CALL(evt_sub_intpl_msec_init, 4, -130, 0, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 0, STRING("hari"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    RETURN
EVT_END()

// TODO: write peach_set_scale (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(peach_evt_sugata_modoru)
    WAIT_MS(15000)
    SWITCH(LSW(5))
        CASE_EQUAL(0)
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MINIMINI1"), LW(3), LW(4), LW(5), 0)
        CALL(evt_sub_intpl_msec_init, 11, 2000, 1000, 1000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
        CALL(peach_set_scale, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        CASE_EQUAL(1)
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_DEKADEKA1"), LW(3), LW(4), LW(5), 0)
        CALL(evt_sub_intpl_msec_init, 11, 500, 1000, 1000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
        CALL(peach_set_scale, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

// TODO: write peach_chg_pose (global, 0x30 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write peach_evt_toumei_onoff (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(peach_evt_kusuri_nomu)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_adjust_dir)
    CALL(peach_chg_pose, STRING("P_M_1"))
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bika"), 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_DRINK_MEDICINE1"), LW(0), LW(1), LW(2), 0)
    CALL(peach_chg_pose, STRING("P_M_3"))
    WAIT_MS(500)
    CALL(evt_mapobj_scale, 0, STRING("S_naka"), 0, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 15)
    ADD(LW(2), 10)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 13, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(100)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_PEACH_SURPRISED1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bika"), 1)
    IF_INT_EQUAL(LSWF(9), 0)
        WAIT_MS(200)
        CALL(evt_sub_random, 1, LW(0))
        SET_VALUE(LW(2), LW(0))
        SWITCH(LW(2))
            CASE_EQUAL(0)
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_DEKADEKA1"), LW(3), LW(4), LW(5), 0)
            CALL(evt_sub_intpl_msec_init, 11, 1000, 2000, 1000)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            CALL(peach_set_scale, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
            CASE_EQUAL(1)
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MINIMINI1"), LW(3), LW(4), LW(5), 0)
            CALL(evt_sub_intpl_msec_init, 11, 1000, 500, 1000)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            CALL(peach_set_scale, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
        END_SWITCH
        SET_VALUE(LSW(5), LW(2))
        CALL(evt_msg_print, 0, STRING("peach_aji_234"), 0, STRING("mario"))
        CALL(evt_msg_print, 0, STRING("peach_aji_235"), 0, 0)
        CALL(evt_msg_print, 0, STRING("peach_aji_236"), 0, STRING("mario"))
        SPAWN_EVENT(peach_evt_sugata_modoru)
        CALL_CHILD(peach_evt_kaihei_open_all)
        CALL(evt_run_case_evt, 9, 1, STRING("A_sui1"), 0, PTR(peach_evt_tyogo_entry), GW(5))
        CALL(evt_run_case_evt, 9, 1, STRING("A_sui2"), 0, PTR(peach_evt_kaihei1), GW(6))
        CALL(evt_run_case_evt, 9, 1, STRING("A_sui3"), 0, PTR(peach_evt_kaihei2), GW(7))
        CALL(evt_run_case_evt, 9, 1, STRING("A_sui4"), 0, PTR(peach_evt_kaihei3), GW(8))
        CALL(evt_run_case_evt, 9, 1, STRING("A_sui5"), 0, PTR(peach_evt_kaihei4), GW(9))
        SET_VALUE(LW(0), STRING("S_pato1"))
        SPAWN_EVENT_GET_ID(PTR(peach_evt_pato_on), GW(0))
    ELSE
        CALL(evt_snd_bgmoff, 512)
        WAIT_MS(300)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_TOUMEI1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_peach_set_condition, 2)
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("peach_aji_237"), 0, 0)
        CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("P_S_3"), STRING("P_T_3"))
        CALL(evt_msg_print, 0, STRING("peach_aji_238"), 0, STRING("mario"))
        WAIT_MS(2000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 15)
        CALL(evt_mario_mov_pos2, LW(0), LW(2), 30)
        CALL(evt_msg_print, 0, STRING("peach_aji_239"), 0, STRING("mario"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
        CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_mario_mov_pos2, 114, 76, FLOAT(72.0f))
        CALL(evt_mario_mov_pos2, 90, 104, FLOAT(72.0f))
        CALL(evt_mario_mov_pos2, -80, 115, FLOAT(72.0f))
        WAIT_MS(1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_CHANGE_CLOTH1"), LW(0), LW(1), LW(2), LW(3))
        CALL(evt_snd_sfx_dist, LW(3), 0)
        WAIT_MS(2000)
        CALL(peach_evt_toumei_onoff, 1)
        CALL(evt_mario_mov_pos2, 100, 115, FLOAT(72.0f))
        CALL(evt_msg_print, 0, STRING("peach_aji_240"), 0, 0)
        CALL(evt_msg_print, 0, STRING("peach_aji_241"), 0, STRING("mario"))
        CALL(evt_msg_print, 0, STRING("peach_aji_242"), 0, 0)
        CALL(evt_cam3d_evt_off, 500, 11)
        SET_VALUE(GSW(0), 263)
    END_IF
    DELETE_EVENT(GW(2))
    SET_VALUE(LSWF(10), 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

// TODO: write kusuri_color_set_rgb (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kusuri_color_seiko)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 255)
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 64)
    DIVIDE_FLOAT(LW(3), FLOAT(100.0f))
    LOOP(0)
        LOOP(100)
            SUBTRACT_FLOAT(LW(1), LW(3))
            ADD_FLOAT(LW(2), LW(3))
            CALL(kusuri_color_set_rgb, LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(100)
            ADD_FLOAT(LW(1), LW(3))
            SUBTRACT_FLOAT(LW(2), LW(3))
            CALL(kusuri_color_set_rgb, LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

// TODO: write kusuri_color_get (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kusuri_color_sippai)
    CALL(kusuri_color_get, LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(0))
    SET_VALUE(LW(4), LW(1))
    SET_VALUE(LW(5), LW(2))
    MULTIPLY_FLOAT(LW(0), FLOAT(0.7f))
    MULTIPLY_FLOAT(LW(1), FLOAT(0.7f))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.7f))
    MULTIPLY_FLOAT(LW(3), FLOAT(0.3f))
    MULTIPLY_FLOAT(LW(4), FLOAT(0.3f))
    MULTIPLY_FLOAT(LW(5), FLOAT(0.3f))
    SUBTRACT_FLOAT(LW(3), LW(0))
    SUBTRACT_FLOAT(LW(4), LW(1))
    SUBTRACT_FLOAT(LW(5), LW(2))
    DIVIDE_FLOAT(LW(3), FLOAT(100.0f))
    DIVIDE_FLOAT(LW(4), FLOAT(100.0f))
    DIVIDE_FLOAT(LW(5), FLOAT(100.0f))
    LOOP(0)
        LOOP(100)
            ADD_FLOAT(LW(0), LW(3))
            ADD_FLOAT(LW(1), LW(4))
            ADD_FLOAT(LW(2), LW(5))
            CALL(kusuri_color_set_rgb, LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(100)
            SUBTRACT_FLOAT(LW(0), LW(3))
            SUBTRACT_FLOAT(LW(1), LW(4))
            SUBTRACT_FLOAT(LW(2), LW(5))
            CALL(kusuri_color_set_rgb, LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u32 peach_evt_bika_move_data[33] = {
    0x00000000, 0x0000004D, 0x00000906, 0x00000000,
    0x00000000, 0x0000008D, 0x00000780, 0x00000001,
    0x00000000, 0x000000CB, 0x00000744, 0x00000002,
    0x00000000, 0x0000008D, 0x00000744, 0x00000001,
    0x00000001, 0x000000CB, 0x00000744, 0x00000002,
    0x00000000, 0x0000004D, 0x00000EC4, 0x00000000,
    0x00000001, 0x000000CB, 0x00000EC4, 0x00000002,
    0x00000000, 0x0000010A, 0x00000762, 0x00000003,
    0x00000000,
};

// TODO: write yuka_anim_reverse_end (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write yuka_anim_reverse_play (global, 0x218 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write kusuri_color_init (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(peach_evt_tyogo_sub)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_map_pauseanim, 0, STRING("yuka"))
    CALL(evt_snd_sfxoff, GW(3))
    CALL_CHILD(peach_evt_wait_memo)
    CALL(evt_msg_print, 0, STRING("peach_aji_230"), 0, 0)
    SET_VALUE(LW(0), STRING("S_pato2"))
    SPAWN_EVENT_GET_ID(PTR(peach_evt_pato_on), GW(0))
    SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
    SET_VALUE(GW(11), 0)
    SET_VALUE(GW(12), 0)
    SET_VALUE(GW(13), 0)
    SET_VALUE(LSWF(8), 0)
    SET_VALUE(LSWF(7), 0)
    CALL(kusuri_color_init)
    CALL(evt_mario_key_onoff, 1)
    SET_INT_READ_ADDR(PTR(peach_evt_bika_move_data))
    READ_INT(LW(2))
    LOOP(0)
        READ_INT4(LW(3), LW(4), GW(12), LW(5))
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(6), LW(4))
            SUBTRACT(LW(6), 400)
            WAIT_MS(LW(6))
            SET_VALUE(LSWF(8), 1)
        END_INLINE_EVENT
        IF_INT_EQUAL(LW(5), 0)
            CALL(evt_map_replayanim, 0, STRING("yuka"))
        ELSE
            BEGIN_INLINE_EVENT
                CALL(yuka_anim_reverse_play)
            END_INLINE_EVENT
        END_IF
        CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_MOVE1"), 0, 0, -100, GW(3))
        CALL(evt_sub_intpl_msec_init, 0, LW(2), LW(3), LW(4))
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, STRING("S_bika"), LW(0), 48, -152)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        IF_INT_EQUAL(LW(5), 0)
            CALL(evt_map_pauseanim, 0, STRING("yuka"))
        ELSE
            CALL(yuka_anim_reverse_end)
            WAIT_FRAMES(1)
        END_IF
        CALL(evt_snd_sfxoff, GW(3))
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            CALL(evt_sub_get_stopwatch, LW(0), LW(1))
            IF_INT_GREATER_OR_EQUAL(LW(1), 3000)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LSWF(7), 1)
                LOOP(0)
                    IF_INT_EQUAL(LSWF(7), 0)
                        BREAK_LOOP
                    END_IF
                    WAIT_FRAMES(1)
                END_LOOP
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LSWF(8), 0)
        ADD(GW(11), 1)
        IF_INT_EQUAL(GW(11), 8)
            SET_VALUE(GW(12), 4)
            BREAK_LOOP
        END_IF
        SET_VALUE(LW(2), LW(3))
    END_LOOP
    CALL(evt_map_replayanim, 0, STRING("yuka"))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_MOVE1"), 0, 0, -100, GW(3))
    CALL(evt_sub_intpl_msec_init, 0, 266, 345, 2370)
    LABEL(10)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), LW(0), 48, -152)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(10)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_map_pauseanim, 0, STRING("yuka"))
    CALL(evt_snd_sfxoff, GW(3))
    DELETE_EVENT(GW(0))
    SET_VALUE(LW(0), STRING("S_pato2"))
    CALL_CHILD(peach_evt_pato_off)
    CALL_CHILD(peach_evt_wait_memo)
    CALL(evt_msg_print, 0, STRING("peach_aji_230_01"), 0, 0)
    SET_VALUE(LW(0), STRING("S_pato3"))
    SPAWN_EVENT_GET_ID(PTR(peach_evt_pato_on), GW(0))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui6"), 0, PTR(peach_evt_stop_renji), 0)
    SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_HOT1"), 104, 0, -100, LW(3))
    CALL(evt_map_playanim, STRING("ren1"), 0, 0)
    CALL(evt_map_checkanim, STRING("ren1"), LW(1), LW(2))
    WAIT_MS(LW(2))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_map_playanim, STRING("ren2"), 1, 0)
    SET_VALUE(LSWF(8), 1)
    SET_VALUE(LSWF(7), 0)
    LOOP(0)
        IF_INT_EQUAL(LSWF(7), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_sub_get_stopwatch, LW(0), LW(1))
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_sfxoff, LW(3))
    CALL(evt_map_playanim, STRING("ren2"), 2, 0)
    WAIT_FRAMES(1)
    CALL(evt_map_playanim, STRING("ren3"), 0, 0)
    CALL(evt_map_checkanim, STRING("ren3"), LW(2), LW(3))
    WAIT_MS(LW(3))
    DELETE_EVENT(GW(0))
    SET_VALUE(LW(0), STRING("S_pato3"))
    CALL_CHILD(peach_evt_pato_off)
    IF_INT_GREATER(LW(1), 25000)
        IF_INT_LESS(LW(1), 35000)
            DEBUG_PUT_MSG(STRING("ほどよい　かねつじかん"))
            ADD(GW(13), 1)
        ELSE
            DEBUG_PUT_MSG(STRING("かねつしすぎ"))
            DIVIDE(LW(1), 1000)
            DEBUG_PUT_REG(LW(1))
        END_IF
    ELSE
        DEBUG_PUT_MSG(STRING("かねつぶそく"))
        DIVIDE(LW(1), 1000)
        DEBUG_PUT_REG(LW(1))
    END_IF
    IF_INT_GREATER_OR_EQUAL(GW(13), 9)
        SPAWN_EVENT_GET_ID(PTR(kusuri_color_seiko), GW(2))
    ELSE
        SPAWN_EVENT_GET_ID(PTR(kusuri_color_sippai), GW(2))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_map_replayanim, 0, STRING("yuka"))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_MOVE1"), 0, 0, -100, GW(3))
    CALL(evt_sub_intpl_msec_init, 0, 345, 439, 2820)
    LABEL(11)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), LW(0), 48, -152)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(11)
    END_IF
    SET_VALUE(LSWF(8), 0)
    SET_VALUE(LSWF(7), 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_tyogo)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_ELEVATOR_ACCESS1"), LW(0), LW(1), LW(2), 0)
    CALL_CHILD(peach_evt_kaihei_close_all)
    DELETE_EVENT(GW(0))
    SET_VALUE(LW(0), STRING("S_pato1"))
    CALL_CHILD(peach_evt_pato_off)
    SET_VALUE(LSWF(6), 1)
    CALL(evt_mapobj_get_position, STRING("S_sui_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_SWITCH1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 800)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("S_bika"), 0, 0, LW(0))
    CALL(evt_mapobj_rotate, 1, STRING("S_kururi"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 0, 0, 0)
    SPAWN_EVENT_GET_ID(PTR(peach_evt_hari_on), GW(1))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_map_replayanim, 0, STRING("yuka"))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_MOVE1"), 0, 0, -100, GW(3))
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mapobj_get_position, STRING("S_syata1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_OPEN1"), LW(0), LW(1), LW(2), LW(3))
        CALL(evt_sub_intpl_msec_init, 11, 0, -30, 500)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 0, STRING("S_syata1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        CALL(evt_snd_sfxoff, LW(3))
        WAIT_MS(500)
        CALL(evt_mapobj_get_position, STRING("S_syata1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_MACHINE_OPEN1"), LW(0), LW(1), LW(2), LW(3))
        CALL(evt_sub_intpl_msec_init, 11, -30, 0, 500)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 0, STRING("S_syata1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
        CALL(evt_snd_sfxoff, LW(3))
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 0, -42, 1260)
    LABEL(3)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(3)
    END_IF
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 0, 0, 48, 1440)
    LABEL(4)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 0, LW(0), -42)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(4)
    END_IF
    CALL(evt_sub_intpl_msec_init, 0, -42, -152, 3300)
    LABEL(5)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 0, 48, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(5)
    END_IF
    CALL_CHILD(peach_evt_tyogo_sub)
    CALL(evt_sub_intpl_msec_init, 0, -152, -42, 3300)
    LABEL(12)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 439, 48, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(12)
    END_IF
    CALL(evt_sub_intpl_msec_init, 0, 48, 0, 1440)
    LABEL(13)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 439, LW(0), -42)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(13)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_syata2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("S_syata2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, 0, -30, 500)
        LABEL(14)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 0, STRING("S_syata2"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(14)
        END_IF
        WAIT_MS(500)
        CALL(evt_mapobj_get_position, STRING("S_syata2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("S_syata2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, -30, 0, 500)
        LABEL(15)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 0, STRING("S_syata2"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(15)
        END_IF
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, -42, 0, 1260)
    LABEL(16)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 439, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(16)
    END_IF
    CALL(evt_map_pauseanim, 0, STRING("yuka"))
    CALL(evt_snd_sfxoff, GW(3))
    DELETE_EVENT(GW(1))
    SPAWN_EVENT(peach_evt_hari_off)
    SET_VALUE(LSWF(6), 0)
    IF_INT_EQUAL(GW(13), 9)
        SET_VALUE(LSWF(9), 1)
    ELSE
        SET_VALUE(LSWF(9), 0)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(peach_evt_wait_memo)
    CALL(evt_msg_print, 0, STRING("peach_aji_231"), 0, 0)
    CALL(evt_msg_select, 0, STRING("peach_aji_227_sel"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("peach_aji_231_02"))
        CALL(evt_msg_print, 0, STRING("peach_aji_232"), 0, STRING("mario"))
        CALL(evt_msg_print, 0, STRING("peach_aji_233"), 0, 0)
        SET_VALUE(LSWF(10), 1)
        CALL(evt_run_case_evt, 9, 1, STRING("A_bika1"), 0, PTR(peach_evt_kusuri_nomu), 0)
        CALL(evt_del_case_evt, 0, GW(6))
        CALL(evt_del_case_evt, 0, GW(7))
        CALL(evt_del_case_evt, 0, GW(8))
        CALL(evt_del_case_evt, 0, GW(9))
    ELSE
        CALL(evt_msg_print_add, 0, STRING("peach_aji_231_01"))
        CALL(evt_mapobj_scale, 0, STRING("S_naka"), 0, 0, 0)
        CALL(evt_mapobj_trans, 0, STRING("S_bika"), 0, -100, 0)
        SET_VALUE(LSWF(10), 0)
        DELETE_EVENT(GW(2))
        CALL_CHILD(peach_evt_kaihei_open_all)
        CALL(evt_run_case_evt, 9, 1, STRING("A_sui1"), 0, PTR(peach_evt_tyogo_entry), GW(5))
        SET_VALUE(LW(0), STRING("S_pato1"))
        SPAWN_EVENT_GET_ID(PTR(peach_evt_pato_on), GW(0))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_tyogo_entry)
    SPAWN_EVENT(peach_evt_tyogo)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kusuri_aka)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_item_get_item, STRING("kusuri_aka"))
    CALL(evt_item_get_item_end_wait, STRING("kusuri_aka"))
    SET_VALUE(LSWF(1), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kusuri_ao)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_item_get_item, STRING("kusuri_ao"))
    CALL(evt_item_get_item_end_wait, STRING("kusuri_ao"))
    SET_VALUE(LSWF(2), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kusuri_ki)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_item_get_item, STRING("kusuri_ki"))
    CALL(evt_item_get_item_end_wait, STRING("kusuri_ki"))
    SET_VALUE(LSWF(3), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_kusuri_mid)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_item_get_item, STRING("kusuri_mid"))
    CALL(evt_item_get_item_end_wait, STRING("kusuri_mid"))
    SET_VALUE(LSWF(4), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_wait_memo)
    LOOP(0)
        IF_INT_EQUAL(LSWF(11), 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LSWF(11), 0)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_memo1)
    SET_VALUE(LSWF(11), 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("peach_aji_225_1"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LSWF(11), 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_memo2)
    SET_VALUE(LSWF(11), 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("peach_aji_225_2"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LSWF(11), 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_memo3)
    SET_VALUE(LSWF(11), 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("peach_aji_225_3"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LSWF(11), 0)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_memo4)
    SET_VALUE(LSWF(11), 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("peach_aji_225_4"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LSWF(11), 0)
    RETURN
EVT_END()

// TODO: write kusuri_4_color_tev (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write kusuri_3_color_tev (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write kusuri_2_color_tev (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write kusuri_1_color_tev (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write kusuri_naka_color_tev (global, 0xd4 bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(peach_evt_stg5)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_dami"), 1)
    CALL(evt_mapobj_get_position, STRING("S_dami1"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 3)
    CALL(evt_item_entry, STRING("kusuri_aka"), 102, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mapobj_get_position, STRING("S_dami2"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 3)
    CALL(evt_item_entry, STRING("kusuri_ao"), 101, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mapobj_get_position, STRING("S_dami3"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 3)
    CALL(evt_item_entry, STRING("kusuri_ki"), 103, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mapobj_get_position, STRING("S_dami4"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 3)
    CALL(evt_item_entry, STRING("kusuri_mid"), 104, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_kusuri1"), 0, PTR(peach_evt_kusuri_aka), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_kusuri2"), 0, PTR(peach_evt_kusuri_ao), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_kusuri3"), 0, PTR(peach_evt_kusuri_ki), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_kusuri4"), 0, PTR(peach_evt_kusuri_mid), 0)
    CALL(evt_map_playanim, STRING("yuka"), 1, 0)
    CALL(evt_map_pauseanim, 0, STRING("yuka"))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui2"), 0, PTR(peach_evt_kaihei1), GW(6))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui3"), 0, PTR(peach_evt_kaihei2), GW(7))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui4"), 0, PTR(peach_evt_kaihei3), GW(8))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui5"), 0, PTR(peach_evt_kaihei4), GW(9))
    CALL(evt_map_set_tevcallback, 1, PTR(kusuri_naka_color_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_naka"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_naka"), 256)
    CALL(evt_map_set_tevcallback, 2, PTR(kusuri_1_color_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_mizu1"), 33554432)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_mizu1"), 256)
    CALL(evt_map_set_tevcallback, 3, PTR(kusuri_2_color_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_mizu2"), 50331648)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_mizu2"), 256)
    CALL(evt_map_set_tevcallback, 4, PTR(kusuri_3_color_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_mizu3"), 67108864)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_mizu3"), 256)
    CALL(evt_map_set_tevcallback, 5, PTR(kusuri_4_color_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_mizu4"), 83886080)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_mizu4"), 256)
    SET_VALUE(LSWF(11), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_memo1"), 0, PTR(peach_evt_memo1), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_memo2"), 0, PTR(peach_evt_memo2), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_memo3"), 0, PTR(peach_evt_memo3), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_memo4"), 0, PTR(peach_evt_memo4), 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_adjust_dir)
    SET_VALUE(LW(0), STRING("s_bero"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_bero_overwrite, STRING("s_bero"), PTR(bero_peach_data))
    SET_VALUE(LW(0), STRING("s_bero"))
    CALL_CHILD(bero_case_switch_on)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_msg_print, 0, STRING("peach_aji_221"), 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("peach_aji_222"), 0, STRING("mario"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_dispdir, LW(0))
        IF_INT_EQUAL(LW(0), 180)
            CALL(evt_mario_set_dir, 270, 200, 0)
            WAIT_MS(300)
            CALL(evt_mario_set_dir, 90, 200, 0)
            WAIT_MS(300)
        ELSE
            CALL(evt_mario_set_dir, 90, 200, 0)
            WAIT_MS(300)
            CALL(evt_mario_set_dir, 270, 200, 0)
            WAIT_MS(300)
        END_IF
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("peach_aji_223"), 0, 0)
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("peach_aji_224"), 0, STRING("mario"))
    CALL(evt_msg_print, 0, STRING("peach_aji_225"), 0, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_COOKING1"))
    CALL_CHILD(peach_evt_kaihei_open_all)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_matigai_kusuri)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("peach_aji_256"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_tadashii_kusuri)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_adjust_dir)
    CALL(evt_del_case_evt, 0, LW(0))
    CALL(evt_del_case_evt, 0, LW(1))
    CALL(evt_del_case_evt, 0, LW(2))
    CALL(peach_chg_pose, STRING("P_M_5"))
    WAIT_MS(500)
    CALL(evt_item_delete, STRING("kusuri_mid"))
    WAIT_MS(200)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_DRINK_MEDICINE1"), LW(0), LW(1), LW(2), 0)
    CALL(peach_chg_pose, STRING("P_M_7"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_TOUMEI2"), LW(0), LW(1), LW(2), 0)
    ADD(LW(1), 10)
    ADD(LW(2), 10)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 7, LW(0), LW(1), LW(2), FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(200)
    CALL(peach_chg_pose, STRING("P_M_8"))
    WAIT_MS(500)
    CALL(evt_peach_set_condition, 0)
    CALL(evt_mario_get_dir, LW(0))
    IF_INT_EQUAL(LW(0), 90)
        CALL(evt_mario_set_dir, 270, 200, 0)
        WAIT_MS(300)
        CALL(evt_mario_set_dir, 90, 200, 0)
        WAIT_MS(300)
    ELSE
        CALL(evt_mario_set_dir, 90, 200, 0)
        WAIT_MS(300)
        CALL(evt_mario_set_dir, 270, 200, 0)
        WAIT_MS(300)
    END_IF
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("P_S_1"), STRING("P_T_1"))
    CALL(evt_msg_print, 0, STRING("peach_aji_257"), 0, STRING("mario"))
    CALL(evt_msg_print, 0, STRING("peach_aji_258"), 0, 0)
    SET_VALUE(GSWF(4210), 1)
    CALL_CHILD(peach_evt_kaihei_close_all)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(peach_evt_door)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(GSWF(4210), 0)
        CALL(evt_msg_print, 0, STRING("peach_aji_255_01"), 0, 0)
    ELSE
        SET_VALUE(LW(1), 0)
        SPAWN_EVENT(bero_open_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL_CHILD(s_door_open)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        BEGIN_INLINE_EVENT
            CALL(evt_mario_mov_pos2, 0, 300, FLOAT(150.0f))
        END_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_fade_entry, 10, 300, 0, 0, 0)
        CALL(evt_fade_end_wait)
        WAIT_MS(2000)
        CALL(evt_fade_set_mapchange_type, 0, 10, 0, 9, 300)
        CALL(evt_bero_mapchange, STRING("aji_17"), 0)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(peach_evt_stg5_1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_dami"), 1)
    CALL(evt_item_entry, STRING("kusuri_aka"), 102, -110, 31, -107, 17, -1, 0)
    CALL(evt_item_entry, STRING("kusuri_ao"), 101, -65, 31, -107, 17, -1, 0)
    CALL(evt_item_entry, STRING("kusuri_ki"), 103, -10, 31, -107, 17, -1, 0)
    CALL(evt_item_entry, STRING("kusuri_mid"), 104, 35, 31, -107, 17, -1, 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui2"), 0, PTR(peach_evt_matigai_kusuri), LW(0))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui3"), 0, PTR(peach_evt_matigai_kusuri), LW(1))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui4"), 0, PTR(peach_evt_matigai_kusuri), LW(2))
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui5"), 0, PTR(peach_evt_tadashii_kusuri), LW(3))
    CALL(evt_set_case_wrk, LW(3), 0, LW(0))
    CALL(evt_set_case_wrk, LW(3), 1, LW(1))
    CALL(evt_set_case_wrk, LW(3), 2, LW(2))
    CALL(evt_set_case_wrk, LW(3), 3, LW(3))
    CALL(evt_mapobj_trans, 0, STRING("S_kaihei1"), 0, -25, 0)
    CALL(evt_mapobj_trans, 0, STRING("S_kaihei2"), 0, -25, 0)
    CALL(evt_mapobj_trans, 0, STRING("S_kaihei3"), 0, -25, 0)
    CALL(evt_mapobj_trans, 0, STRING("S_kaihei4"), 0, -25, 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    SET_VALUE(LW(0), STRING("s_bero"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_run_case_evt, 9, 1, STRING("s_bero"), 0, PTR(peach_evt_door), 0)
    CALL(evt_msg_print, 0, STRING("peach_aji_251"), 0, 0)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("P_S_3"), STRING("P_T_3"))
    CALL(evt_msg_print, 0, STRING("peach_aji_252"), 0, STRING("mario"))
    CALL(evt_msg_print, 0, STRING("peach_aji_253"), 0, 0)
    CALL(evt_msg_print, 0, STRING("peach_aji_254"), 0, STRING("mario"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_mov_pos2, -170, 125, FLOAT(72.0f))
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_PEACH_CHANGE_CLOTH1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(2000)
    CALL(peach_evt_toumei_onoff, 0)
    CALL(evt_mario_mov_pos2, 0, 90, FLOAT(72.0f))
    CALL(evt_msg_print, 0, STRING("peach_aji_255"), 0, STRING("mario"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kikai_init)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_pato1"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_pato2"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_pato3"), 1)
    CALL(evt_mapobj_scale, 0, STRING("S_mizu1"), 1, 0, 1)
    CALL(evt_mapobj_scale, 0, STRING("S_mizu2"), 1, 0, 1)
    CALL(evt_mapobj_scale, 0, STRING("S_mizu3"), 1, 0, 1)
    CALL(evt_mapobj_scale, 0, STRING("S_mizu4"), 1, 0, 1)
    CALL(evt_mapobj_scale, 0, STRING("S_naka"), 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_bika"), 0, 0, 180)
    CALL(evt_mapobj_trans, 1, STRING("S_bika"), 0, 2, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_kururi"), 0, 0, 180)
    CALL(evt_mapobj_rotate, 0, STRING("hari"), 0, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_dami"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hika1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kesu1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hika2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kesu2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hika3"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kesu3"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hika4"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kesu4"), 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_memo1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_32_01"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_memo2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_32_02"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_memo3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_32_03"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_memo4)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_32_04"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(aji_06_init_evt)
    IF_INT_LESS(GSW(0), 352)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_PEACH1"))
        CALL(evt_snd_envoff, 512)
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI2"))
    END_IF
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tenjo"))
        CALL(evt_cam_letter_box_disable, 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_party_cont_onoff, 1, 0)
        CALL(evt_mario_set_pos, 175, 165, -110)
        CALL(evt_party_set_pos, 0, 185, 165, -100)
        CALL(evt_party_set_pos, 1, 195, 165, -90)
        BEGIN_INLINE_EVENT
            CALL(evt_seq_wait, 2)
            CALL(evt_mario_cont_onoff, 1)
            CALL(evt_party_cont_onoff, 0, 1)
            CALL(evt_party_cont_onoff, 1, 1)
            CALL(evt_mario_wait_landon)
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_cam_letter_box_disable, 0)
        END_INLINE_EVENT
    END_IF
    SPAWN_EVENT(evt_emergency)
    SPAWN_EVENT(kikai_init)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        CALL(evt_npc_check_delete, STRING("軍団ザコ魔法使い"))
    END_IF
    IF_INT_EQUAL(GSW(0), 262)
        CALL(evt_mario_set_mode, 1)
        CALL(evt_mario_kill_party, 0)
        CALL(evt_npc_check_delete, STRING("軍団ザコ魔法使い"))
        SPAWN_EVENT(peach_evt_stg5)
    END_IF
    IF_INT_EQUAL(GSW(0), 263)
        CALL(evt_mario_set_mode, 1)
        CALL(evt_mario_kill_party, 0)
        CALL(peach_evt_toumei_onoff, 1)
        CALL(evt_npc_check_delete, STRING("軍団ザコ魔法使い"))
    END_IF
    IF_INT_EQUAL(GSW(0), 264)
        CALL(evt_mario_set_mode, 1)
        CALL(evt_mario_kill_party, 0)
        CALL(peach_evt_toumei_onoff, 1)
        CALL(evt_npc_check_delete, STRING("軍団ザコ魔法使い"))
        SPAWN_EVENT(peach_evt_stg5_1)
    END_IF
    CALL(evt_mario_get_mode, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_memo1"), 0, PTR(evt_memo1), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_memo2"), 0, PTR(evt_memo2), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_memo3"), 0, PTR(evt_memo3), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_memo4"), 0, PTR(evt_memo4), 0)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui_1_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui_2_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui_3_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui_4_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui_5_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui_6_2"), 1)
    RETURN
EVT_END()

// TODO: type
u32 g_Kusuri_R[1] = {
    0x00000000,
};

// TODO: type
u32 g_Kusuri_G[1] = {
    0x00000000,
};

// TODO: type
u32 g_Kusuri_B[1] = {
    0x00000000,
};

// TODO: type
u32 g_Kusuri1_No[1] = {
    0x00000000,
};

// TODO: type
u32 yuka_anim[2] = {
    0x00000000, 0x00000000,
};

// TODO: write yuka_anim_reverse_pause (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it

// TODO: write yuka_anim_reverse_replay (global, 0x1c bytes) here:
// its asm is in build/G8MJ01/aji/asm/aji_06.s; this is where the original has it, just before the first script that uses it
// clang-format on

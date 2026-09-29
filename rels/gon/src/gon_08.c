#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
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

static EVT_BEGIN(door01_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door__01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door01_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door__01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door02_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door__02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door02_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door__02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door03_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door__03"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door03_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door__03"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"e_bero_1", 1, 12, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_04", "w_bero_2", 5, 5, door01_close, door01_open},
    {"e_bero_2", 1, 12, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_09", "w_bero", 5, 5, door02_close, door02_open},
    {"w_bero", 1, 12, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gon_12", "e_bero", 5, 5, door03_close, door03_open},
    {NULL},
};

static EVT_BEGIN(init_0)
    IF_INT_EQUAL(0, 0)
        IF_INT_NOT_EQUAL(LSWF(3), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(600.0f))
            CALL(evt_mobj_set_y_position, STRING("blk_0"), LW(5))
        END_IF
    ELSE
        IF_INT_EQUAL(LSWF(3), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(5))
            ADD_FLOAT(LW(5), FLOAT(600.0f))
            CALL(evt_mobj_set_y_position, STRING("blk_0"), LW(5))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(main_0)
    SET_TYPE(239)
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    IF_INT_NOT_EQUAL(1, 0)
        SET_VALUE(LW(10), 0)
        IF_INT_EQUAL(0, 0)
            IF_INT_EQUAL(LSWF(3), 0)
                SET_VALUE(LW(10), 0)
            ELSE
                SET_VALUE(LW(10), 1)
            END_IF
        ELSE
            IF_INT_EQUAL(LSWF(3), 0)
                SET_VALUE(LW(10), 1)
            ELSE
                SET_VALUE(LW(10), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            ADD_FLOAT(LW(3), FLOAT(600.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
            SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
        ELSE
            CALL(evt_mobj_get_y_position, STRING("blk_0"), LW(2))
            SET_FLOAT_VALUE(LW(3), LW(2))
            SUBTRACT_FLOAT(LW(3), FLOAT(600.0f))
            SET_FLOAT_VALUE(LW(8), FLOAT(-0.2f))
            SET_FLOAT_VALUE(LW(9), 0)
        END_IF
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(10)
            CALL(evt_sub_rumble_onoff, 0, 0)
            WAIT_MS(300)
            CALL(evt_sub_rumble_onoff, 1, 0)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(10)
            CALL(evt_sub_intpl_init, 11, 10, 3, 4)
            LABEL(0)
            CALL(evt_sub_intpl_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            CALL(evt_cam_shake, 4, FLOAT(0.0f), LW(0), 100)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
        END_SIBLING_EVENT
        WAIT_MS(500)
        CALL(evt_mobj_get_position, STRING("blk_0"), LW(12), LW(13), LW(14))
        CALL(evt_mobj_get_kindname, STRING("blk_0"), LW(11))
        SET_VALUE(LF(0), 0)
        IF_STR_EQUAL(LW(11), STRING("MOBJ_green_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(11), STRING("MOBJ_purple_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(11), STRING("MOBJ_orange_big02_FlowBlock"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LF(1), 0)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_MOVE1"), LW(12), LW(13), LW(14), LW(15))
            ELSE
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_S_FLOAT_MOVE1"), LW(12), LW(13), LW(14), LW(15))
            END_IF
        ELSE
            SET_VALUE(LF(1), 1)
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_DOWN1"), LW(12), LW(13), LW(14), LW(15))
            ELSE
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_S_FLOAT_DOWN1"), LW(12), LW(13), LW(14), LW(15))
            END_IF
        END_IF
        SET_FLOAT_VALUE(LW(4), LW(2))
        SET_VALUE(LF(0), 0)
        LOOP(0)
            CALL(evt_mario_get_party, LW(5))
            IF_INT_EQUAL(LW(5), 0)
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("blk_0"))
                    GOTO(1)
                END_IF
            ELSE
                CALL(evt_mario_get_name_hitobj_head, LW(6))
                IF_STR_EQUAL(LW(6), STRING("MOBJ_blk_0"))
                    GOTO(1)
                END_IF
                IF_STR_EQUAL(LW(7), STRING("MOBJ_blk_0"))
                    GOTO(1)
                END_IF
            END_IF
            GOTO(2)
            LABEL(1)
            IF_INT_EQUAL(LW(10), 1)
                SET_FLOAT_VALUE(LW(5), LW(2))
                SET_FLOAT_VALUE(LW(2), LW(3))
                SET_FLOAT_VALUE(LW(3), LW(5))
                SET_FLOAT_VALUE(LW(9), FLOAT(2.0f))
                SET_FLOAT_VALUE(LW(8), 0)
                SET_FLOAT_VALUE(LW(10), 0)
                IF_INT_EQUAL(LSWF(3), 0)
                    SET_VALUE(LSWF(3), 1)
                ELSE
                    SET_VALUE(LSWF(3), 0)
                END_IF
                CALL(evt_mobj_exec_cancel, STRING("sw_0"))
            END_IF
            LABEL(2)
            ADD_FLOAT(LW(4), LW(9))
            ADD_FLOAT(LW(9), LW(8))
            IF_FLOAT_GREATER_OR_EQUAL(LW(9), FLOAT(4.0f))
                SET_FLOAT_VALUE(LW(9), FLOAT(4.0f))
            END_IF
            CALL(evt_mobj_get_position, STRING("blk_0"), LW(11), LW(12), LW(13))
            CALL(evt_snd_sfx_pos, LW(15), LW(11), LW(12), LW(13))
            IF_INT_EQUAL(LW(10), 0)
                IF_INT_GREATER_OR_EQUAL(LW(4), LW(3))
                    SET_FLOAT_VALUE(LW(4), LW(3))
                    SET_VALUE(LF(0), 1)
                END_IF
            ELSE
                IF_INT_LESS_OR_EQUAL(LW(4), LW(3))
                    SET_FLOAT_VALUE(LW(4), LW(3))
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            CALL(evt_mobj_set_y_position, STRING("blk_0"), LW(4))
            IF_INT_EQUAL(LF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(LF(1), 0)
            CALL(evt_snd_sfxoff, LW(15))
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_mobj_get_position, STRING("blk_0"), LW(13), LW(14), LW(15))
            CALL(evt_mobj_get_kindname, STRING("blk_0"), LW(12))
            SET_VALUE(LF(0), 0)
            IF_STR_EQUAL(LW(12), STRING("MOBJ_green_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(12), STRING("MOBJ_purple_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_STR_EQUAL(LW(12), STRING("MOBJ_orange_big02_FlowBlock"))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LW(10), 1)
                CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLOCK_B_FLOAT_STOP1"), LW(13), LW(14), LW(15), 0)
            END_IF
        END_INLINE_EVENT
        IF_INT_EQUAL(LW(10), 1)
            BEGIN_SIBLING_EVENT
                CALL(evt_sub_rumble_onoff, 0, 0)
                WAIT_MS(300)
                CALL(evt_sub_rumble_onoff, 1, 0)
            END_SIBLING_EVENT
            BEGIN_SIBLING_EVENT
                CALL(evt_sub_intpl_init, 11, 10, 3, 10)
                LABEL(2)
                CALL(evt_sub_intpl_get_value)
                DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
                CALL(evt_cam_shake, 4, FLOAT(0.0f), LW(0), 100)
                IF_INT_EQUAL(LW(1), 1)
                    GOTO(2)
                END_IF
            END_SIBLING_EVENT
            WAIT_MS(1000)
        END_IF
        IF_INT_EQUAL(LSWF(3), 0)
            SET_VALUE(LSWF(3), 1)
        ELSE
            SET_VALUE(LSWF(3), 0)
        END_IF
    ELSE
        CALL(evt_mobj_exec_cancel, STRING("sw_0"))
        SET_VALUE(LW(0), 0)
        LOOP(0)
            WAIT_FRAMES(2)
            SET_VALUE(LW(1), LW(0))
            MODULO(LW(1), 2)
            ADD(LW(0), 1)
            IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

static s32 key_tbl[] = {
    15, -1,
};

static EVT_BEGIN(evt_chk)
    CALL(evt_pouch_check_item, 15, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock)
    SET_VALUE(LW(0), STRING("e_bero_2"))
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(GSW(0), 52)
    RETURN
EVT_END()

EVT_BEGIN(nokkosu_init)
    IF_INT_EQUAL(GSW(1420), 7)
        CALL(evt_npc_set_position, STRING("me"), -230, 500, -120)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(nokkosu_talk)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_msg_print, 0, STRING("irai_07_00"), 0, STRING("me"))
    LOOP(0)
        CALL(evt_mario_party_use_check, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_wait_landon, 0)
    CALL(evt_party_outofscreen, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 5)
        ADD(LW(2), -15)
        CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        CALL(evt_mario_goodbye_party, 0)
        CALL(evt_mario_hello_party, 0, 2)
    END_IF
    CALL(evt_msg_print, 0, STRING("irai_07_01"), 0, STRING("party"))
    CALL(evt_msg_print, 0, STRING("irai_07_02"), 0, STRING("me"))
    CALL(evt_msg_print, 0, STRING("irai_07_03"), 0, STRING("party"))
    CALL(evt_msg_print, 0, STRING("irai_07_04"), 0, STRING("me"))
    CALL(evt_msg_print, 0, STRING("irai_07_05"), 0, STRING("party"))
    CALL(evt_msg_print, 0, STRING("irai_07_06"), 0, STRING("me"))
    CALL(evt_item_entry, STRING("irai_item"), 91, 0, -1000, 0, 16, -1, 0)
    CALL(evt_item_flag_on, STRING("irai_item"), 2)
    CALL(evt_item_get_item, STRING("irai_item"))
    CALL(evt_msg_print, 0, STRING("irai_07_07"), 0, STRING("me"))
    CALL(evt_npc_jump_position_nohit, STRING("me"), -66, 100, -112, 800, 220)
    CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    CALL(evt_sub_save_playtime, GSW(1421))
    SET_VALUE(GSW(1420), 0)
    SET_VALUE(GSWF(5381), 1)
    CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"ノッコス", 0x00000600, 0, &nokkosu_init, NULL, &nokkosu_talk},
    {NULL},
};

EVT_BEGIN(gon_08_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_npc_entry, STRING("\"カロン00\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン00\""), -169, 100, -66)
    CALL(evt_npc_set_rotate, STRING("\"カロン00\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン00\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン00\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン00\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン00\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン00\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン00\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン01\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン01\""), 129, 0, 132)
    CALL(evt_npc_set_rotate, STRING("\"カロン01\""), 0, 180, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン01\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン01\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン01\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン01\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン01\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン01\""), 7)
    CALL(evt_npc_entry, STRING("\"カロン02\""), STRING("c_karon_g"))
    CALL(evt_npc_set_position, STRING("\"カロン02\""), 191, 100, -113)
    CALL(evt_npc_set_rotate, STRING("\"カロン02\""), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("\"カロン02\""), STRING("KRN_Z_2"))
    CALL(evt_npc_pera_onoff, STRING("\"カロン02\""), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("\"カロン02\""), 1048576)
    CALL(evt_npc_flag_onoff, 1, STRING("\"カロン02\""), 33554432)
    CALL(evt_npc_set_width, STRING("\"カロン02\""), 30)
    CALL(evt_npc_set_height, STRING("\"カロン02\""), 7)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_item_entry, STRING("item_00"), 151, -215, 500, 10, 16, GSWF(1505), 0)
    CALL(evt_mobj_float_blk, STRING("blk_0"), 0, 0, 0, 0, 2, 0, LSWF(3))
    CALL_CHILD(init_0)
    CALL(evt_mobj_switch_float_blk, STRING("sw_0"), 0, 100, -185, STRING("blk_0"), 0, PTR(main_0), LSWF(3))
    CALL(evt_mobj_flag_onoff, 1, STRING("blk_0"), 128)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("fuyu_boxS"), 1)
    CALL(evt_hitobj_onoff, STRING("a_fuyu_box"), 1, 0)
    IF_INT_LESS(GSW(0), 52)
        CALL(evt_mobj_lock, STRING("mobj_lock00"), 15, 240, 710, 0, 270, PTR(evt_chk), PTR(evt_unlock), GSWF(1496))
        SET_VALUE(LW(0), STRING("e_bero_2"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

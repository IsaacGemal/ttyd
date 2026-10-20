#include "muj_01.h"
#include "rel/muj/muj.h"
#include "driver/envdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_kinopio.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_shop.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_urouro.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include <dolphin/mtx.h>
#include <stdio.h>
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/mobjdrv.h"
#include "eff/eff_starstone.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "memory.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(starstone_set_ry);
static USER_FUNC(starstone_set_pos);
static USER_FUNC(starstone_entry);
static USER_FUNC(anim_tevcallback_mobj);
static USER_FUNC(anim_tevcallback);
static USER_FUNC(yami_view);
static USER_FUNC(delete_itemsel_table);
static USER_FUNC(make_itemsel_table);
s32 settoku_event_main[95];
static s32* itemsel_table_ram;

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(custom_door_dark2)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_map_set_blend, 1, 255, 255, 255, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_map_set_blend, 1, 255, 255, 255, 0)
    RETURN
EVT_END()

EVT_BEGIN(custom_door_light2)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_map_set_blend, 1, 255, 255, 255, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_map_blend_off, 1)
    RETURN
EVT_END()

static EVT_BEGIN(garawaru_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(232, 238)
        CALL(evt_npc_set_position, STRING("me"), -155, 0, 170)
        CASE_EQUAL(239)
        CALL(evt_npc_set_position, STRING("me"), -110, 0, 225)
        CASE_BETWEEN(240, 254)
        CALL(evt_npc_set_position, STRING("me"), -155, 0, 170)
        CASE_GREATER_OR_EQUAL(269)
        CALL(evt_npc_set_position, STRING("me"), -155, 0, 170)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(garawaru_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(233)
        CALL(evt_msg_print, 0, STRING("stg5_muj_22_1"), 0, STRING("me"))
        CASE_BETWEEN(234, 238)
        CALL(evt_msg_print, 0, STRING("stg5_muj_32_02"), 0, STRING("me"))
        CASE_EQUAL(239)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_71_05"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_71_04"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(240, 241)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_90_02"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_90_02_01"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(242, 254)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_90_13"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_90_132_01"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(268, 272)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_149"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_1492_01"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(273, 275)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_150"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_1502_01"), 0, STRING("me"))
        END_IF
        CASE_EQUAL(276)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_151"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_1512_01"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(279, 373)
        SET_VALUE(LF(0), 0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 354)
            IF_INT_LESS_OR_EQUAL(GSW(0), 354)
                IF_INT_EQUAL(GSWF(3877), 1)
                    SET_VALUE(GSWF(3878), 1)
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            CALL(evt_mario_get_party, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 3)
                CALL(evt_msg_print, 0, STRING("stg5_muj_152"), 0, STRING("me"))
            ELSE
                CALL(evt_msg_print, 0, STRING("stg5_muj_1522_01"), 0, STRING("me"))
            END_IF
        ELSE
            CALL(evt_mario_get_party, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 3)
                CALL(evt_msg_print, 0, STRING("stg7_muj_01"), 0, 0)
            ELSE
                CALL(evt_msg_print, 0, STRING("stg7_muj_012_01"), 0, 0)
            END_IF
        END_IF
        CASE_LESS(401)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_153"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_1532_01"), 0, STRING("me"))
        END_IF
        CASE_DEFAULT
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_153_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_153_01_san"), 0, STRING("me"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(marco_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(232, 239)
        CALL(evt_npc_set_position, STRING("me"), -64, 0, 170)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CASE_BETWEEN(243, 254)
        CALL(evt_npc_set_position, STRING("me"), -64, 0, 170)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(marco_kyorokyoro)
    CALL(evt_sub_intpl_msec_init, 0, LW(0), LW(1), 150)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_npc_set_rotate, STRING("マルコ"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static USER_FUNC(make_itemsel_table) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    s32 i;
    s32 count;
    s32* p;

    itemsel_table_ram = _mapAlloc(0x54);
    p = itemsel_table_ram;
    for (count = 0, i = 0; i < MAX_HELD_ITEMS; i++) {
        s32 item = pouch->heldItems[i];
        if (item > 0) {
            *p++ = item;
            count++;
        }
    }
    *p = -1;
    evtSetValue(event, args[0], count);
    event->lwData[10] = (s32)itemsel_table_ram;
    return EVT_RETURN_DONE;
}


static USER_FUNC(delete_itemsel_table) {
    _mapFree(itemsel_table_ram);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_reqdir) {
    s32* args = event->args;
    f32 reqdir = marioGetPtr()->dispReqDir;

    evtSetFloat(event, args[0], reqdir);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(marco_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(234, 237)
        IF_INT_EQUAL(GSWF(3129), 0)
            CALL_CHILD(settoku_event_main)
        END_IF
    END_SWITCH
    SWITCH(GSW(0))
        CASE_EQUAL(233)
        CALL(evt_msg_print, 0, STRING("stg5_muj_22_0"), 0, STRING("me"))
        CASE_BETWEEN(234, 234)
        IF_INT_EQUAL(GSWF(3127), 0)
            CALL(evt_msg_print, 0, STRING("stg5_muj_31"), 0, STRING("me"))
            SET_VALUE(GSWF(3127), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_32"), 0, STRING("me"))
        END_IF
        CASE_EQUAL(235)
        CALL(evt_msg_print, 0, STRING("stg5_muj_31_01"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("stg5_muj_31_02"), 0, STRING("me"))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_cam3d_evt_set, LW(0), 97, 459, LW(0), 45, 163, 500, 11)
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("stg5_muj_31_02_01"), 0, STRING("me"))
        SET_VALUE(GSW(0), 236)
        CALL(evt_cam3d_evt_off, 500, 11)
        WAIT_MS(500)
        CASE_EQUAL(236)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("マルコ"), LW(10), LW(11), LW(12))
        CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 32)
        IF_INT_LESS_OR_EQUAL(LW(0), LW(10))
            ADD(LW(10), -30)
            CALL(evt_mario_mov_pos2, LW(10), LW(12), FLOAT(120.0f))
            CALL(evt_mario_set_dir, 90, 200, 0)
            CALL(evt_party_move_behind_mario, 0)
        ELSE
            ADD(LW(10), 30)
            CALL(evt_mario_mov_pos2, LW(10), LW(12), FLOAT(120.0f))
            CALL(evt_mario_set_dir, 270, 200, 0)
            CALL(evt_party_move_behind_mario, 0)
        END_IF
        CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 32)
        CALL(evt_msg_print, 0, STRING("stg5_muj_31_02_02"), 0, STRING("me"))
        CALL(make_itemsel_table, LW(11))
        IF_INT_EQUAL(LW(11), 0)
            CALL(delete_itemsel_table)
            RETURN
        END_IF
        CALL(evt_win_item_select, 0, LW(10), LW(0), 0)
        CALL(delete_itemsel_table)
        IF_INT_EQUAL(LW(0), -1)
            RETURN
        END_IF
        CALL(evt_mario_set_pose, STRING("M_C_3"))
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        ADD(LW(4), 20)
        ADD(LW(5), 10)
        CALL(evt_mario_get_reqdir, LW(6))
        IF_FLOAT_EQUAL(LW(6), FLOAT(0.0f))
            ADD(LW(3), -20)
        ELSE
            ADD(LW(3), 20)
        END_IF
        CALL(evt_item_entry, STRING("watasu"), LW(0), LW(3), LW(4), LW(5), 16, -1, 0)
        CALL(evt_item_flag_on, STRING("watasu"), 2)
        WAIT_MS(500)
        CALL(evt_npc_set_anim, STRING("me"), STRING("I_1"))
        WAIT_MS(500)
        CALL(evt_item_delete, STRING("watasu"))
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
        END_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("me"), STRING("I_2"))
        WAIT_MS(500)
        SET_VALUE(LF(1), 0)
        IF_INT_GREATER_OR_EQUAL(LW(0), 165)
            IF_INT_LESS(LW(0), 177)
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(0), 148)
            IF_INT_LESS(LW(0), 161)
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(0), 179)
            IF_INT_LESS(LW(0), 236)
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        CALL(evt_pouch_remove_item, LW(0), LW(1))
        IF_INT_EQUAL(LF(0), 0)
            CALL(evt_msg_print, 0, STRING("stg5_muj_31_02_03"), 0, STRING("me"))
            RETURN
        END_IF
        IF_INT_NOT_EQUAL(LW(0), 166)
            CALL(evt_msg_print, 0, STRING("stg5_muj_31_02_04"), 0, STRING("me"))
            RETURN
        END_IF
        CALL(evt_msg_print, 0, STRING("stg5_muj_31_02_05"), 0, STRING("me"))
        CALL(evt_npc_set_anim, STRING("me"), STRING("I_1"))
        WAIT_MS(500)
        CALL(evt_npc_set_anim, STRING("me"), STRING("I_2"))
        WAIT_MS(500)
        CALL(evt_item_entry, STRING("itemwine"), 70, 0, -1000, 0, 16, -1, 0)
        CALL(evt_item_flag_on, STRING("itemwine"), 2)
        CALL(evt_item_get_item, STRING("itemwine"))
        CALL(evt_msg_print, 0, STRING("stg5_muj_31_03"), 0, STRING("me"))
        SET_VALUE(GSW(0), 237)
        CASE_EQUAL(237)
        CALL(evt_msg_print, 0, STRING("stg5_muj_31_03"), 0, STRING("me"))
        CASE_EQUAL(238)
        CALL(evt_msg_print, 0, STRING("stg5_muj_32"), 0, STRING("me"))
        CASE_EQUAL(239)
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_cam3d_evt_set, -60, 95, 491, -60, 35, 146, 500, 11)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
        CALL(evt_mario_mov_pos2, -34, 162, FLOAT(120.0f))
        CALL(evt_mario_set_dir, 270, 200, 0)
        CALL(evt_npc_set_ry_lr, STRING("me"), 1)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
        CALL(evt_mario_get_party, LW(0))
        SET_VALUE(LF(10), 0)
        IF_INT_NOT_EQUAL(LW(0), 3)
            SET_VALUE(LF(10), 1)
            CALL(evt_mario_hello_exparty_pos, 15, -25, 0, 210)
        END_IF
        CALL(evt_party_stop, 0)
        CALL(evt_party_stop, 1)
        SET_VALUE(GF(0), 1)
        BEGIN_INLINE_EVENT
            IF_INT_EQUAL(LW(0), 3)
                CALL(evt_party_move_pos2, 0, 0, 170, FLOAT(120.0f))
            ELSE
                CALL(evt_party_move_pos2, 0, 20, 170, FLOAT(120.0f))
            END_IF
            CALL(evt_party_set_dir, 0, 270, -1)
            SET_VALUE(GF(0), 0)
        END_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg5_muj_71"), 0, STRING("me"))
        WAIT_WHILE(GF(0))
        IF_INT_EQUAL(LF(10), 0)
            CALL(evt_msg_print_party, STRING("stg5_muj_72"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_72_bom"), 0, STRING("extparty"))
        END_IF
        CALL(evt_msg_print, 0, STRING("stg5_muj_79"), 0, STRING("me"))
        IF_INT_EQUAL(LF(10), 0)
            CALL(evt_msg_print, 0, STRING("stg5_muj_80"), 0, STRING("party"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_80"), 0, STRING("extparty"))
        END_IF
        CALL(evt_set_dir_to_target, STRING("ガラの悪い水夫"), STRING("me"))
        CALL(evt_msg_print, 0, STRING("stg5_muj_81"), 0, STRING("ガラの悪い水夫"))
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SAILER_FART1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg5_muj_81_01"), 0, STRING("ガラの悪い水夫"))
        CALL(evt_msg_print, 0, STRING("stg5_muj_81_02"), 0, STRING("ガラの悪い水夫"))
        CALL(evt_msg_print, 0, STRING("stg5_muj_82"), 0, STRING("me"))
        IF_INT_EQUAL(LF(10), 0)
            CALL(evt_msg_print, 0, STRING("stg5_muj_83"), 0, STRING("party"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_83"), 0, STRING("extparty"))
        END_IF
        CALL(evt_party_run, 1)
        CALL(evt_msg_print, 0, STRING("stg5_muj_84"), 0, STRING("me"))
        LOOP(3)
            SET_VALUE(LW(0), 0)
            SET_VALUE(LW(1), 180)
            CALL_CHILD(marco_kyorokyoro)
            WAIT_MS(160)
            SET_VALUE(LW(0), 180)
            SET_VALUE(LW(1), 0)
            CALL_CHILD(marco_kyorokyoro)
            WAIT_MS(160)
        END_LOOP
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("stg5_muj_85"), 0, STRING("me"))
        CALL(evt_cam3d_evt_set, -60, 144, 753, -60, 40, 162, 500, 11)
        WAIT_MS(500)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_print, 0, STRING("stg5_muj_86"), 0, STRING("ガラの悪い水夫"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_set_dir_to_target, STRING("水夫A"), STRING("me"))
            WAIT_MS(500)
            CALL(evt_npc_get_position, STRING("水夫A"), LW(0), LW(1), LW(2))
            ADD(LW(0), 20)
            ADD(LW(1), 40)
            CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
            CALL(evt_msg_print, 0, STRING("stg5_muj_87"), 0, 0)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_set_dir_to_target, STRING("水夫ボム兵"), STRING("me"))
            WAIT_MS(1000)
            CALL(evt_msg_print, 0, STRING("stg5_muj_88"), 0, STRING("水夫ボム兵"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_set_dir_to_target, STRING("オドオド水夫"), STRING("me"))
            WAIT_MS(1500)
            CALL(evt_npc_get_position, STRING("オドオド水夫"), LW(0), LW(1), LW(2))
            ADD(LW(0), -30)
            ADD(LW(1), 40)
            CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
            CALL(evt_msg_togelr, 0)
            CALL(evt_msg_print, 0, STRING("stg5_muj_88_01"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_msg_print, 0, STRING("stg5_muj_89"), 0, STRING("me"))
        LOOP(3)
            SET_VALUE(LW(0), 0)
            SET_VALUE(LW(1), 180)
            CALL_CHILD(marco_kyorokyoro)
            WAIT_MS(160)
            SET_VALUE(LW(0), 180)
            SET_VALUE(LW(1), 0)
            CALL_CHILD(marco_kyorokyoro)
            WAIT_MS(160)
        END_LOOP
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90"), 0, STRING("me"))
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg5_sys_00"), 0, 0)
        IF_INT_EQUAL(LF(10), 1)
            CALL(evt_mario_goodbye_party, 1)
        END_IF
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CALL(evt_party_stop, 1)
        CALL(evt_mario_set_party_pos, 1, 9, LW(0), LW(1), LW(2))
        WAIT_MS(1000)
        CALL(evt_party_run, 1)
        CALL(evt_cam3d_evt_off, 500, 11)
        SET_VALUE(GSW(0), 240)
        CALL(evt_party_run, 0)
        CASE_BETWEEN(242, 254)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_10"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(megane_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(234, 250)
        CALL(evt_npc_set_position, STRING("me"), 115, 0, 247)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(megane_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(234, 238)
        CALL(evt_msg_print, 0, STRING("stg5_muj_32_01"), 0, STRING("me"))
        CASE_EQUAL(239)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_71_02"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_71_03"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(240, 241)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_01"), 0, STRING("me"))
        CASE_BETWEEN(242, 254)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(odoodo_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(234, 254)
        CALL(evt_npc_set_position, STRING("me"), 95, 0, 82)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(odoodo_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(234, 238)
        CALL(evt_msg_print, 0, STRING("stg5_muj_32_00"), 0, STRING("me"))
        CASE_EQUAL(239)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_EQUAL(LW(0), 3)
            CALL(evt_msg_print, 0, STRING("stg5_muj_71_00"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_71_01"), 0, STRING("me"))
        END_IF
        CASE_BETWEEN(240, 241)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_00"), 0, STRING("me"))
        CASE_BETWEEN(242, 254)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_11"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_a_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(232, 261)
        CALL(evt_npc_set_position, STRING("me"), -193, 0, 77)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_a_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(233)
        CALL(evt_msg_print, 0, STRING("stg5_muj_22_2"), 0, STRING("me"))
        CASE_BETWEEN(234, 238)
        CALL(evt_msg_print, 0, STRING("stg5_muj_32_03"), 0, STRING("me"))
        CASE_BETWEEN(239, 241)
        CALL(evt_msg_print, 0, STRING("stg5_muj_71_06"), 0, STRING("me"))
        CASE_BETWEEN(242, 254)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_14"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(255)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_16"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_bomb_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(232, 261)
        CALL(evt_npc_set_position, STRING("me"), -33, 0, 74)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_bomb_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(233)
        CALL(evt_msg_print, 0, STRING("stg5_muj_22_3"), 0, STRING("me"))
        CASE_BETWEEN(234, 238)
        CALL(evt_msg_print, 0, STRING("stg5_muj_32_04"), 0, STRING("me"))
        CASE_BETWEEN(239, 241)
        CALL(evt_msg_print, 0, STRING("stg5_muj_71_07"), 0, STRING("me"))
        CASE_BETWEEN(242, 254)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_15"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(255)
        CALL(evt_msg_print, 0, STRING("stg5_muj_90_17"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(269)
        CALL(evt_npc_set_position, STRING("me"), 25, 0, 150)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifu_d_regl)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(269)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
        CALL(urouro_main_func, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(268, 272)
        CALL(evt_msg_print, 0, STRING("stg5_muj_159"), 0, STRING("me"))
        CASE_BETWEEN(273, 275)
        CALL(evt_msg_print, 0, STRING("stg5_muj_160"), 0, STRING("me"))
        CASE_EQUAL(276)
        CALL(evt_msg_print, 0, STRING("stg5_muj_161"), 0, STRING("me"))
        CASE_BETWEEN(279, 377)
        CALL(evt_msg_print, 0, STRING("stg5_muj_162"), 0, STRING("me"))
        CASE_BETWEEN(378, 402)
        CALL(evt_msg_print, 0, STRING("stg5_muj_163"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg5_muj_163_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borosuifu_a_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(269)
        CALL(evt_npc_set_position, STRING("me"), 95, 0, 82)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borosuifu_a_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(268, 272)
        CALL(evt_msg_print, 0, STRING("stg5_muj_154"), 0, STRING("me"))
        CASE_BETWEEN(273, 275)
        CALL(evt_msg_print, 0, STRING("stg5_muj_155"), 0, STRING("me"))
        CASE_EQUAL(276)
        CALL(evt_msg_print, 0, STRING("stg5_muj_156"), 0, STRING("me"))
        CASE_BETWEEN(279, 377)
        CALL(evt_msg_print, 0, STRING("stg5_muj_157"), 0, STRING("me"))
        CASE_BETWEEN(378, 402)
        CALL(evt_msg_print, 0, STRING("stg5_muj_158"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg5_muj_158_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borosuifu_b_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(269)
        CALL(evt_npc_set_position, STRING("me"), -350, 0, 0)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borosuifu_b_regl)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(269)
        LOOP(0)
            CALL(evt_npc_move_position, STRING("me"), 350, 0, 0, FLOAT(40.0f), 0)
            WAIT_MS(1000)
            CALL(evt_npc_move_position, STRING("me"), -350, 0, 0, FLOAT(40.0f), 0)
            WAIT_MS(1000)
        END_LOOP
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borosuifu_b_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(268, 272)
        CALL(evt_msg_print, 0, STRING("stg5_muj_164"), 0, STRING("me"))
        CASE_BETWEEN(273, 275)
        CALL(evt_msg_print, 0, STRING("stg5_muj_165"), 0, STRING("me"))
        CASE_EQUAL(276)
        CALL(evt_msg_print, 0, STRING("stg5_muj_166"), 0, STRING("me"))
        CASE_BETWEEN(279, 377)
        CALL(evt_msg_print, 0, STRING("stg5_muj_167"), 0, STRING("me"))
        CASE_BETWEEN(378, 402)
        CALL(evt_msg_print, 0, STRING("stg5_muj_168"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg5_muj_168_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(mony_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(269, 272)
        CALL(evt_npc_set_position, STRING("me"), 105, 0, 274)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(mony_talk)
    CALL(evt_msg_print, 0, STRING("stg5_muj_147"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(peton_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(269, 272)
        CALL(evt_npc_set_position, STRING("me"), 45, 0, 274)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_talk)
    CALL(evt_msg_print, 0, STRING("stg5_muj_148"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(musume_talk)
    CALL(evt_msg_print, 0, STRING("stg5_muj_168_01"), 0, STRING("３人娘Ａ"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_168_02"), 0, STRING("３人娘Ｂ"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_168_03"), 0, STRING("３人娘Ｃ"))
    RETURN
EVT_END()

static EVT_BEGIN(musume_a_entry)
    CALL(evt_npc_entry, STRING("３人娘Ａ"), STRING("c_dance_r"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ａ"), STRING("踊り子"))
    CALL(evt_npc_set_position, STRING("３人娘Ａ"), 200, -10, 360)
    CALL(evt_npc_set_ry, STRING("３人娘Ａ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ａ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ａ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

static EVT_BEGIN(musume_b_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｂ"), STRING("c_dance_p"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｂ"), STRING("踊り子ピンク"))
    CALL(evt_npc_set_position, STRING("３人娘Ｂ"), 235, -7, 310)
    CALL(evt_npc_set_ry, STRING("３人娘Ｂ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ｂ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ｂ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

static EVT_BEGIN(musume_c_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｃ"), STRING("c_dance_k"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｃ"), STRING("踊り子黄緑"))
    CALL(evt_npc_set_position, STRING("３人娘Ｃ"), 280, -10, 300)
    CALL(evt_npc_set_ry, STRING("３人娘Ｃ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ｃ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ｃ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[17] = {
    {"ガラの悪い水夫", 0x00000600, 0x00000000, (void*)&garawaru_init, NULL, (void*)&garawaru_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"マルコ", 0x00000600, 0x00000000, (void*)&marco_init, NULL, (void*)&marco_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"めがね水夫", 0x00000600, 0x00000000, (void*)&megane_init, NULL, (void*)&megane_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"オドオド水夫", 0x00000600, 0x00000000, (void*)&odoodo_init, NULL, (void*)&odoodo_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"店員", 0x00000600, 0x00000000, (void*)npc_init_evt, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"宿店員", 0x00000600, 0x00000000, (void*)npc_init_evt, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"水夫A", 0x00000600, 0x00000000, (void*)&suifu_a_init, NULL, (void*)&suifu_a_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"水夫ボム兵", 0x00000600, 0x00000000, (void*)&suifu_bomb_init, NULL, (void*)&suifu_bomb_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"水夫D", 0x00000600, 0x00000000, (void*)&suifu_d_init, (void*)&suifu_d_regl, (void*)&suifu_d_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"ボロ水夫A", 0x00000600, 0x00000000, (void*)&borosuifu_a_init, NULL, (void*)&borosuifu_a_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"ボロ水夫B", 0x00000600, 0x00000000, (void*)&borosuifu_b_init, (void*)&borosuifu_b_regl, (void*)&borosuifu_b_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"モニー", 0x00000600, 0x00000000, (void*)&mony_init, NULL, (void*)&mony_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"ピートン", 0x00000600, 0x00000000, (void*)&peton_init, NULL, (void*)&peton_talk, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"エルモスA", 0x00000600, 0x00000000, (void*)npc_init_evt, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"エルモスB", 0x00000600, 0x00000000, (void*)npc_init_evt, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {"エルモスC", 0x00000600, 0x00000000, (void*)npc_init_evt, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (NpcTerritoryType)0x00000000, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0x00000000},
    {0},
};

static EVT_BEGIN(bero_in_event)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, bero_in_event, -1, NULL, "muj_00", "e_bero", 1, 1},
    {"e_bero1", 0, 0, 0x4E20, 100000, 0, 0, -1, bero_in_event, -1, NULL, "muj_02", "w_bero", 1, 1},
    {NULL},
};

USER_FUNC(muj_get_door_check) {
    s32* args = event->args;
    DoorEntry* door = (DoorEntry*)evtGetValue(event, args[0]);
    f32 range = evtGetFloat(event, args[1]);
    Vec hitPos;
    Vec marioPos;

    hitObjGetPos(door->openHit, &hitPos);
    marioPos = marioGetPtr()->position;
    hitPos.y = 0.0f;
    marioPos.y = 0.0f;
    if (PSVECDistance(&hitPos, &marioPos) > range) {
        evtSetValue(event, args[2], 0);
    } else {
        evtSetValue(event, args[2], 1);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(muj_00_door_open)
    CALL(evt_mario_party_use_check, LW(4))
    IF_INT_EQUAL(LW(4), 4)
        RETURN
    END_IF
    CALL(muj_get_door_check, LW(0), FLOAT(30.0f), LW(1))
    IF_INT_EQUAL(LW(1), 0)
        RETURN
    END_IF
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_door_param, LW(0), 7, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SET_VALUE(LF(0), 0)
        SPAWN_EVENT(LW(1))
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_save_mapflag, LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_door_param, LW(0), 10, LW(1), LW(2), LW(3))
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        ADD(LW(5), 2)
        CALL(evt_cam3d_evt_set_xyz, LW(1), LW(5), LW(3), 0, 150, 11)
    END_INLINE_EVENT
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 1)
    END_IF
    CALL(door_dark_flag, LW(0), 1)
    IF_BITS_CLEAR(LW(15), 2)
        SPAWN_EVENT(door_dark)
    END_IF
    SPAWN_EVENT(door_open_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 0)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(0)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(evt_door_param, LW(0), 10, LW(1), LW(2), LW(3))
    CALL(evt_mario_party_door_move, LW(1), LW(3), FLOAT(135.0f))
    CALL(evt_cam3d_evt_xyz_off, 0, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(muj_00_door_close)
    CALL(evt_mario_party_use_check, LW(4))
    IF_INT_EQUAL(LW(4), 4)
        RETURN
    END_IF
    CALL(muj_get_door_check, LW(0), FLOAT(30.0f), LW(1))
    IF_INT_EQUAL(LW(1), 0)
        RETURN
    END_IF
    CALL_CHILD(evt_door_close)
    RETURN
EVT_END()

static const char* mise_npc[2] = {
    "店員", NULL,
};

EVT_BEGIN(mise_data_open)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mise_kan"), 4096)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mise_hasi04"), 4096)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mise_hasi05"), 4096)
    SPAWN_EVENT(custom_door_dark2)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(mise_data_close)
    BEGIN_INLINE_EVENT
        CALL_CHILD(custom_door_light2)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mise_kan"), 4096)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mise_hasi04"), 4096)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mise_hasi05"), 4096)
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

static DoorEntry mise_data[1] = {
    {0, 3, 0, NULL, "A_mise_in_doa", "A_mise_doa", NULL, NULL, "S_mise", NULL, NULL, 0, NULL, mise_npc, NULL, NULL, mise_data_open, mise_data_close},
};

static const char* yado_npc[2] = {
    "宿店員", NULL,
};

EVT_BEGIN(mise_open_event)
    SPAWN_EVENT(door_dark)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yado"), 2048)
    SPAWN_EVENT(custom_door_dark2)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(mise_close_event)
    SPAWN_EVENT(door_light)
    SPAWN_EVENT(custom_door_light2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yado"), 2048)
    WAIT_MS(1000)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

static DoorEntry yado_data[1] = {
    {0, 3, 0, NULL, "A_yado_in_doa", "A_yado_doa", NULL, NULL, "S_yado_in", NULL, NULL, 0, NULL, yado_npc, NULL, NULL, mise_open_event, mise_close_event},
};

EVT_BEGIN(evt_door_entry_custom00)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LW(15), 17)
        CALL(evt_run_case_evt, 8, 1, LW(2), 0, PTR(muj_00_door_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 17)
        CALL(evt_run_case_evt, 8, 1, LW(3), 0, PTR(muj_00_door_open), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_door_entry_custom01)
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LW(15), 19)
        CALL(evt_run_case_evt, 8, 1, LW(2), 0, PTR(muj_00_door_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 19)
        CALL(evt_run_case_evt, 8, 1, LW(3), 0, PTR(muj_00_door_open), 0)
    END_IF
    RETURN
EVT_END()

static char* obj_list[12] = {
    "S_item_01", "A_item_01", "S_item_02", "A_item_02", "S_item_03", "A_item_03", "S_item_04",
    "A_item_04", "S_item_05", "A_item_05", "S_item_06", "A_item_06",
};

static s32 goods_list[] = {
    0x83, 0x0F, 0x84, 0x05,
    0x8A, 0x0A, 0xA2, 0x02,
    0x9A, 0x03, 0x95, 0x0C,
};

static s32 trade_list[] = {
    0x83, 0x000A0000, 0x84, 0x00020000,
    0x8A, 0x00050000, 0xA2, 0x00010000,
    0x9A, 0x00010000, 0x95, 0x00060000,
    0x99, 0x00050000, 0x9F, 0x000D0000,
    0xC7, 0x00140000, 0xC8, 0x00230000,
    0xC9, 0x000F0000, 0, 0,
};

static char* shopper_data[38] = {
    "店員", NULL, NULL, "shop_00", "shop_01", "shop_02", "shop_03", "shop_04", "shop_05", "shop_06",
    "shop_07", "shop_08", "shop_09", "shop_10", "shop_11", "shop_12", "shop_13", "shop_14",
    "shop_15", "shop_16", "shop_17", "shop_18", "shop_19", "shop_20", "shop_21", "shop_22",
    "shop_23", "shop_24", "shop_25", "shop_26", "shop_27", "shop_28", "shop_29", "shop_30",
    "shop_31", "shop_32", "shop_33", "shop_34",
};

// TODO: type
static u32 kino_dt[38] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000000A, 0x43C98000, 0x41880000, 0xC3360000,
    0x43B18000, 0x00000000, 0xC31B0000, 0x43550000,
    0x42F40000, 0xC2C20000, 0x43AA8000, 0x42340000,
    0xC3320000, 0xC1F00000, 0x43B00000, 0x00000000,
    0xC3070000, (u32)"宿店員", (u32)"yado_00", (u32)"yado_01",
    (u32)"yado_02", (u32)"yado_03", (u32)"yado_04", (u32)"yado_05",
    (u32)"yado_06", (u32)"yado_07", (u32)"yado_08", (u32)"yado_09",
    (u32)"yado_10", (u32)"yado_11", (u32)"yado_12", (u32)"yado_13",
    0x00000000, 0x00000000,
};

EVT_BEGIN(kyoshu_event)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_set_position, STRING("水夫A"), -193, 0, 77)
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), -33, 0, 74)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 270)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -155, 0, 170)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_set_position, STRING("マルコ"), -64, 0, 170)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    CALL(evt_mario_set_pos, -110, 0, 230)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_pos, 0, -167, 0, 268)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_cam3d_evt_set, -110, 119, 615, -110, 40, 172, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_15"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_16"), 0, STRING("マルコ"))
    CALL(evt_msg_select, 0, STRING("stg5_muj_16_yesno"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("stg5_muj_17"))
    ELSE
        CALL(evt_msg_print_add, 0, STRING("stg5_muj_18"))
        BEGIN_INLINE_EVENT
            LOOP(3)
                CALL(evt_mario_set_dir, 270, 150, 0)
                WAIT_MS(160)
                CALL(evt_mario_set_dir, 90, 150, 0)
                WAIT_MS(160)
            END_LOOP
            CALL(evt_mario_dispflag_onoff, 1, 2)
            CALL(evt_mario_set_pose, STRING("M_I_N"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_msg_print, 0, STRING("stg5_muj_19"), 0, STRING("ガラの悪い水夫"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("stg5_muj_20"), 0, STRING("水夫A"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(1000)
            CALL(evt_msg_print, 0, STRING("stg5_muj_21"), 0, STRING("水夫ボム兵"))
        END_INLINE_EVENT
        WAIT_MS(2500)
        CALL(evt_msg_print, 0, STRING("stg5_muj_22"), 0, STRING("マルコ"))
        CALL(evt_mario_dispflag_onoff, 0, 2)
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 233)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
    RETURN
EVT_END()

EVT_BEGIN(maruco_buruburu)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 1024)
    LOOP(0)
        CALL(muj_npc_buruburu, STRING("マルコ"), 0, FLOAT(1.0f), 10, 10000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(garawaru_buruburu)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 1024)
    LOOP(0)
        CALL(muj_npc_buruburu, STRING("ガラの悪い水夫"), 0, FLOAT(1.0f), 10, 10000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(suifu_a_buruburu)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫A"), 1024)
    LOOP(0)
        CALL(muj_npc_buruburu, STRING("水夫A"), 0, FLOAT(1.0f), 10, 10000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(suifu_bom_buruburu)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫ボム兵"), 1024)
    LOOP(0)
        CALL(muj_npc_buruburu, STRING("水夫ボム兵"), 0, FLOAT(1.0f), 10, 10000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(nakamaware_event)
    IF_INT_EQUAL(GSWF(3126), 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_cam3d_evt_set, -85, 134, 714, -85, 38, 172, 500, 11)
        SPAWN_EVENT(kyoshu_event)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), -33, 0, 74)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 270)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -122, 0, 170)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_set_position, STRING("マルコ"), -64, 0, 170)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 131088)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 131088)
    CALL(evt_npc_flag_onoff, 1, STRING("エルモスA"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("エルモスB"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("エルモスC"), 1073741856)
    LOOP(0)
        WAIT_FRAMES(1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), -390)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_npc_set_position, STRING("水夫A"), -220, 0, 0)
    CALL(evt_npc_move_position, STRING("水夫A"), -330, 0, 0, FLOAT(120.0f), 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_05"), 0, STRING("水夫A"))
    CALL(evt_mario_mov_pos2, -385, 0, FLOAT(120.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("水夫A"), 90)
        CALL(evt_npc_move_position, STRING("水夫A"), -193, 77, 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER3"))
    CALL(evt_cam3d_evt_set, -95, 128, 605, -95, 50, 162, 500, 11)
    CALL(evt_mario_mov_pos2, -247, 26, FLOAT(120.0f))
    CALL(evt_mario_mov_pos2, -170, 174, FLOAT(120.0f))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_06"), 0, STRING("マルコ"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_07"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_npc_get_position, STRING("マルコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), -5)
    CALL(evt_npc_move_position, STRING("マルコ"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_08"), 0, STRING("マルコ"))
    CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
    ADD(LW(0), 5)
    CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_09"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_npc_set_autotalkpose, STRING("マルコ"), STRING("K_1"), STRING("K_2"))
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("K_1"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_10"), 0, STRING("マルコ"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_11"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_npc_set_autotalkpose, STRING("マルコ"), STRING("S_1"), STRING("T_1"))
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("S_1"))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 90)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
        CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), 128, 166, 0, FLOAT(100.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    CALL(evt_npc_move_position, STRING("マルコ"), 98, 166, 0, FLOAT(100.0f), 0)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_12"), 0, STRING("マルコ"))
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_MARCO_ESCAPE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
        CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 32)
        CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), -200, 166, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
        SPAWN_EVENT_GET_ID(PTR(garawaru_buruburu), GW(0))
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("マルコ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_MARCO_ESCAPE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 32)
    CALL(evt_npc_move_position, STRING("マルコ"), -185, 170, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    SPAWN_EVENT_GET_ID(PTR(maruco_buruburu), GW(1))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -170, 128, 605, -170, 50, 162, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_13"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(1000)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST1"))
    CALL(evt_cam3d_evt_set, 181, 134, 692, 181, 38, 150, 500, 11)
    WAIT_MS(600)
    CALL(evt_npc_set_position, STRING("水夫A"), -224, 0, 48)
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), -197, 0, 83)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 90)
    SPAWN_EVENT_GET_ID(PTR(suifu_a_buruburu), GW(2))
    SPAWN_EVENT_GET_ID(PTR(suifu_bom_buruburu), GW(3))
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    CALL(evt_party_stop, 0)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 5)
        CALL(evt_party_set_pos, 0, -180, -3, 265)
    ELSE
        CALL(evt_party_set_pos, 0, -180, -3, 300)
    END_IF
    CALL(evt_party_set_dir, 0, 90, -1)
    WAIT_MS(400)
    BEGIN_INLINE_EVENT
        WAIT_MS(250)
        CALL(evt_npc_set_position, STRING("エルモスB"), 460, 50, 45)
        CALL(evt_npc_glide_position, STRING("エルモスB"), 135, 50, 45, 0, FLOAT(120.0f), 0, 11, 0)
        CALL(evt_npc_glide_position, STRING("エルモスB"), -46, 10, 151, 0, FLOAT(120.0f), 0, 11, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_npc_set_position, STRING("エルモスC"), 430, 50, 45)
        CALL(evt_npc_glide_position, STRING("エルモスC"), 105, 50, 45, 0, FLOAT(120.0f), 0, 11, 0)
        CALL(evt_npc_glide_position, STRING("エルモスC"), 5, 10, 169, 0, FLOAT(120.0f), 0, 11, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_position, STRING("エルモスA"), 445, 20, 45)
        CALL(evt_npc_get_position, STRING("エルモスA"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_glide_position, STRING("エルモスA"), 120, 20, 45, 0, FLOAT(120.0f), 0, 11, 0)
        CALL(evt_npc_get_position, STRING("エルモスA"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_glide_position, STRING("エルモスA"), -28, 10, 229, 0, FLOAT(120.0f), 0, 11, 0)
    END_INLINE_EVENT
    WAIT_MS(3200)
    CALL(evt_cam3d_evt_set, -100, 136, 714, -100, 40, 172, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_14"), 0, STRING("マルコ"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_G"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_dispflag_onoff, 0, 2)
    BEGIN_INLINE_EVENT
        WAIT_MS(75)
        CALL(evt_npc_glide_position, STRING("エルモスB"), -146, 10, 151, 0, FLOAT(120.0f), 0, 11, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_npc_glide_position, STRING("エルモスC"), -95, 10, 169, 0, FLOAT(120.0f), 0, 11, 0)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("エルモスA"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), -128, 10, 229, 0, FLOAT(120.0f), 0, 11, 0)
    CALL(evt_npc_release_fieldnpc, 1)
    CALL(evt_snd_bgmoff, 512)
    WAIT_MS(100)
    CALL(evt_npc_set_battle_info, STRING("エルモスA"), 6)
    CALL(evt_npc_battle_start, STRING("エルモスA"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_snd_bgmoff, 512)
    DELETE_EVENT(GW(0))
    DELETE_EVENT(GW(1))
    DELETE_EVENT(GW(2))
    DELETE_EVENT(GW(3))
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 32)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 32)
    CALL(evt_npc_set_position, STRING("エルモスA"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("エルモスB"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("エルモスC"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 1024)
    SPAWN_EVENT(kyoshu_event)
    RETURN
EVT_END()

EVT_BEGIN(settoku_event)
    CALL(evt_npc_set_position, STRING("水夫A"), -203, 0, 61)
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), -220, 0, 160)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 90)
    CALL(evt_npc_set_position, STRING("マルコ"), -152, 0, 150)
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    CALL(evt_npc_set_position, STRING("めがね水夫"), -45, 0, 150)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), -10, 0, 150)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -182, 0, 210)
    RETURN
EVT_END()

EVT_BEGIN(settoku_event_main)
    CALL(evt_set_dir_to_target, STRING("水夫A"), STRING("マルコ"))
    CALL(evt_set_dir_to_target, STRING("水夫ボム兵"), STRING("マルコ"))
    CALL(evt_set_dir_to_target, STRING("ガラの悪い水夫"), STRING("マルコ"))
    CALL(evt_set_dir_to_target, STRING("めがね水夫"), STRING("マルコ"))
    CALL(evt_set_dir_to_target, STRING("オドオド水夫"), STRING("マルコ"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_29"), 0, STRING("マルコ"))
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 32)
    CALL(evt_npc_move_position, STRING("オドオド水夫"), -26, 180, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_move_position, STRING("オドオド水夫"), -105, 180, 0, FLOAT(120.0f), 0)
    CALL(evt_set_dir_to_target, STRING("オドオド水夫"), STRING("mario"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_30"), 0, STRING("オドオド水夫"))
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 32)
    SET_VALUE(GSWF(3129), 1)
    IF_INT_EQUAL(GSW(0), 235)
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_print, 0, STRING("stg5_muj_31"), 0, STRING("マルコ"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(delete_npc_regl_evt)
    CALL(evt_npc_change_interrupt, STRING("ボロ水夫B"), 1, 0)
    CALL(evt_npc_get_reglid, STRING("ボロ水夫B"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_change_interrupt, STRING("水夫D"), 1, 0)
    CALL(evt_npc_get_reglid, STRING("水夫D"), LW(0))
    DELETE_EVENT(LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(sekai_yami_shake)
    LOOP(0)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 500)
    END_LOOP
    RETURN
EVT_END()

const Vec vec3_muj_0000f360 = {0.0f, 100.0f, 0.0f};
const Vec vec3_muj_0000f36c = {0.0f, 1.0f, 0.0f};
const Vec vec3_muj_0000f378 = {0.0f, 0.0f, 0.0f};
// dtk named this word as a float (3.003967f); it is the shop icons' GXColor.
const GXColor float_3p004_muj_0000f384 = {0x40, 0x40, 0x40, 0xFF};

static USER_FUNC(yami_view) {
    Vec camPos;
    Vec up;
    Vec target;
    Mtx mtx;
    f32 value = evtGetFloat(event, event->args[0]);

    target = vec3_muj_0000f378;
    target.z = value;
    up = vec3_muj_0000f36c;
    camPos = vec3_muj_0000f360;
    camPos.z = 100.0f + value;
    C_MTXLookAt(mtx, &camPos, &up, &target);
    envSetYamiView(mtx);
    return EVT_RETURN_DONE;
}

static USER_FUNC(anim_tevcallback) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    NpcEntry* npc = npcNameToPtr_NoAssert((const char*)name);

    if (npc == NULL) {
        return EVT_RETURN_DONE;
    }
    animPoseSetMaterialFlagOn(npc->poseId, (flag & 0xF) << 24);
    return EVT_RETURN_DONE;
}

static USER_FUNC(anim_tevcallback_mobj) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    MapObjectEntry* mobj = mobjNameToPtrNoAssert((const char*)name);

    if (mobj == NULL) {
        return EVT_RETURN_DONE;
    }
    animPoseSetMaterialFlagOn(mobj->poseId, (flag & 0xF) << 24);
    return EVT_RETURN_DONE;
}

USER_FUNC(sekai_yami_omise) {
    char name[32];
    s32 i;

    for (i = 0; i < 6; i++) {
        sprintf(name, "shp%d", i);
#ifdef MUST_MATCH
        // Volatile read: retail reloads the colour each iteration instead of hoisting it.
        iconNameToPtr(name)->color = *(volatile GXColor*)&float_3p004_muj_0000f384;
#else
        iconNameToPtr(name)->color = float_3p004_muj_0000f384;
#endif
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(sekai_yami_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ボロ水夫A"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 0, 0, 170)
    CALL(evt_npc_status_onoff, 1, STRING("ガラの悪い水夫"), 2)
    CALL(evt_npc_status_onoff, 1, STRING("水夫D"), 2)
    CALL(evt_npc_status_onoff, 1, STRING("オドオド水夫"), 2)
    CALL(evt_npc_status_onoff, 1, STRING("店員"), 2)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback, STRING("ガラの悪い水夫"), 1)
    CALL(anim_tevcallback, STRING("水夫D"), 1)
    CALL(anim_tevcallback, STRING("オドオド水夫"), 1)
    CALL(anim_tevcallback, STRING("店員"), 1)
    CALL(anim_tevcallback_mobj, STRING("mobj_save"), 1)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(yami_view, -800)
    CALL(sekai_yami_omise)
    CALL(evt_cam3d_evt_set, 0, 120, 795, 0, 70, 348, 0, 11)
    SPAWN_EVENT(sekai_yami_shake)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_QUAKE1"), LW(0))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), 155, 170, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
            CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), -150, 170, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_move_position, STRING("水夫D"), -70, 76, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
            CALL(evt_npc_move_position, STRING("水夫D"), 120, 76, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_move_position, STRING("オドオド水夫"), -60, 82, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
            CALL(evt_npc_move_position, STRING("オドオド水夫"), -90, 82, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_position, STRING("店員"), 4, 0, -69)
        LOOP(0)
            CALL(evt_npc_move_position, STRING("店員"), -124, -69, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
            CALL(evt_npc_move_position, STRING("店員"), 4, -69, 0, FLOAT(120.0f), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg8_dark_06"), 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        LOOP(2)
            CALL(evt_snd_sfxon, STRING("SFX_STG8_LIGHT1"), 0)
            CALL(evt_fade_entry, 10, 0, 255, 255, 255)
            WAIT_MS(100)
            CALL(evt_fade_entry, 9, 0, 255, 255, 255)
            WAIT_MS(100)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_snd_sfxoff, LW(0))
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("pik_01"), STRING("sekai_yami2"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static USER_FUNC(starstone_entry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* effect = effStarStoneEntry(4, x, y, z, 1.0f);

    effSetName(effect, (const char*)name);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_pos) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    ((effStarStoneData*)eff->userdata)->x = x;
    ((effStarStoneData*)eff->userdata)->y = y;
    ((effStarStoneData*)eff->userdata)->z = z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_ry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 ry = evtGetFloat(event, args[1]);
    effStarStoneData* data = effNameToPtr((const char*)name)->userdata;

    data->rotY = ry;
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(minnnanokoe_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -50, 0, 200)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 32768)
    CALL(evt_npc_set_position, STRING("水夫D"), 50, 0, 200)
    CALL(evt_npc_set_ry, STRING("水夫D"), 270)
    CALL(evt_npc_set_position, STRING("ボロ水夫A"), -10, 0, 100)
    CALL(evt_npc_set_ry, STRING("ボロ水夫A"), 270)
    CALL(evt_npc_set_position, STRING("ボロ水夫B"), 25, 0, 75)
    CALL(evt_npc_set_ry, STRING("ボロ水夫B"), 270)
    CALL(evt_npc_set_position, STRING("店員"), 0, -1000, 0)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback, STRING("ガラの悪い水夫"), 1)
    CALL(anim_tevcallback, STRING("水夫D"), 1)
    CALL(anim_tevcallback, STRING("ボロ水夫A"), 1)
    CALL(anim_tevcallback, STRING("ボロ水夫B"), 1)
    CALL(anim_tevcallback_mobj, STRING("mobj_save"), 1)
    CALL(yami_view, -800)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(sekai_yami_omise)
    CALL(evt_cam3d_evt_set, 0, 95, 548, 0, 55, 200, 0, 11)
    CALL(starstone_entry, STRING("starstone"), 0, 50, 0)
    SET_VALUE(GW(0), 50)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(starstone_set_pos, STRING("starstone"), 0, GW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(starstone_set_ry, STRING("starstone"), LW(0))
            SUBTRACT_FLOAT(LW(0), FLOAT(3.0f))
            IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 5000, 5500, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
            CALL(evt_sub_intpl_msec_init, 11, 5500, 5000, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(2)
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 32768)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_38"), 0, STRING("水夫D"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_39"), 0, STRING("ボロ水夫A"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_40"), 0, STRING("ボロ水夫B"))
    CALL(evt_cam3d_evt_set, 0, 300, 1180, 0, 100, 200, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2), 300, 10)
    CALL(evt_npc_jump_position_nohit, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2), 300, 10)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_41"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_42"), 0, STRING("水夫D"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_43"), 0, STRING("ボロ水夫A"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_44"), 0, STRING("ボロ水夫B"))
    CALL(evt_npc_jump_position_nohit, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2), 300, 10)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_45"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_46"), 0, STRING("水夫D"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_47"), 0, STRING("ボロ水夫A"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_48"), 0, STRING("ボロ水夫B"))
    CALL(evt_cam3d_evt_set, 0, 95, 548, 0, 55, 200, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_49"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_50"), 0, STRING("水夫D"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_51"), 0, STRING("ボロ水夫A"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_52"), 0, STRING("ボロ水夫B"))
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("pik_01"), STRING("minnnanokoe"))
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_npc_entry)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 0, 0, 200)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
    CALL(evt_npc_entry, STRING("サンダース"), STRING("c_bomt_n"))
    CALL(evt_npc_set_tribe, STRING("サンダース"), STRING("サンダース"))
    CALL(evt_npc_set_position, STRING("サンダース"), 0, -1000, 0)
    CALL(evt_npc_set_ry, STRING("サンダース"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("サンダース"), 1024)
    CALL(evt_npc_get_reglid, STRING("水夫D"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_change_interrupt, STRING("水夫D"), 1, 0)
    CALL(evt_npc_set_position, STRING("水夫D"), 24, 0, 70)
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt_sub)
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_T_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_S_1"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("T_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("S_1"))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリスチーヌ"), 0, 0, 0, 0, 0, 0, 0, 36)
    WAIT_MS(800)
    CALL(evt_cam3d_evt_set, -40, 143, 791, -40, 39, 200, 300, 11)
    WAIT_MS(300)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ガラの悪い水夫"))
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("サンダース"), -235, 0, 200)
    CALL(evt_npc_move_position, STRING("サンダース"), -80, 200, 0, 40, 0)
    LOOP(0)
        WAIT_MS(500)
        CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_T_1"))
        WAIT_MS(1500)
        CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_S_1"))
        WAIT_MS(500)
        CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("T_1"))
        WAIT_MS(1500)
        CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("S_1"))
        WAIT_MS(500)
        CALL(evt_npc_set_anim, STRING("サンダース"), STRING("T_1"))
        WAIT_MS(1500)
        CALL(evt_npc_set_anim, STRING("サンダース"), STRING("S_1"))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -40, 0, 160)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 32768)
    CALL(evt_cam3d_evt_set, -20, 143, 791, -20, 39, 200, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 32768)
    SPAWN_EVENT(epilogue_evt_sub)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("ep_42"), 0, 0)
    WAIT_MS(1000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_mario_chk_join_party, LW(0), 7)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_bero_mapchange, STRING("gor_01"), STRING(""))
    ELSE
        CALL(evt_bero_mapchange, STRING("pik_04"), STRING(""))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(yakan_anime)
    LOOP(0)
        CALL(evt_map_playanim, STRING("yakan"), 1, 0)
        CALL(evt_map_checkanim, STRING("yakan"), LW(0), LW(1))
        WAIT_MS(LW(1))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(muj_01_init_evt)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami2"))
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_VOICE1"))
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ3"))
    END_IF
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_set_tribe, STRING("モニー"), STRING("モニー（指輪）"))
    CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
    CALL(evt_npc_sound_data_set, STRING("マルコ"), STRING("SFX_STG5_MARCO_MOVE1L"), STRING("SFX_STG5_MARCO_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 382)
        IF_INT_EQUAL(LF(0), 0)
            CALL_CHILD(musume_a_entry)
            CALL_CHILD(musume_b_entry)
            CALL_CHILD(musume_c_entry)
        END_IF
    END_IF
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("fuku_hata"), 1, 0)
    SPAWN_EVENT(yakan_anime)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 100, 60, -25, 0, 0)
    CALL(evt_item_entry, STRING("item_00"), 125, 475, 0, 100, 16, GSWF(5639), 0)
    CALL(evt_item_entry, STRING("item_01"), 125, -200, -16, 485, 16, GSWF(5640), 0)
    SET_VALUE(LW(0), PTR(mise_data))
    SPAWN_EVENT(evt_door_entry_custom00)
    SET_VALUE(LW(0), PTR(yado_data))
    SPAWN_EVENT(evt_door_entry_custom01)
    CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list), PTR(shopper_data), PTR(trade_list))
    CALL(evt_kinopio_setup, PTR(kino_dt))
    SWITCH(GSW(0))
        CASE_EQUAL(232)
        SPAWN_EVENT(nakamaware_event)
        CASE_GREATER_OR_EQUAL(234)
        IF_INT_EQUAL(GSWF(3129), 0)
            SPAWN_EVENT(settoku_event)
        END_IF
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(1))
        IF_STR_EQUAL(LW(1), STRING("sekai_yami2"))
            CALL_CHILD(delete_npc_regl_evt)
            SPAWN_EVENT(sekai_yami_evt)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("minnnanokoe"))
            CALL_CHILD(delete_npc_regl_evt)
            SPAWN_EVENT(minnnanokoe_evt)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 402)
        CALL(evt_mario_kill_party, 0)
        CALL_CHILD(epilogue_npc_entry)
        SPAWN_EVENT(epilogue_evt)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_eff64, STRING("sunlight"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mise"), 2)
    RETURN
EVT_END()

// clang-format on

#include "gor_01.h"
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/envdrv.h"
#include "evt/evt_badgeshop.h"
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
#include "evt/evt_lecture.h"
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
#include "evt/evt_win.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "system.h"
#include "statuswindow.h"
#include "data/item_data.h"
#include "driver/animdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include "driver/npcdrv.h"
#include "driver/swdrv.h"
#include "eff/eff_fire.h"
#include "eff/eff_kemuri.h"
#include "eff/eff_mahojin.h"
#include "eff/eff_starstone.h"
#include "evt/evt_johoya.h"
#include "pmario_sound.h"
#include "kpa_act.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 all_juyoitem_lecture_rel_10_data_4C6C0[];
extern s32 all_party_lecture_10_data_4B0DC[];
extern s32 evt_goki_sub_intpl_get_value[];
extern s32 evt_goki_sub_intpl_init[];

// Forward declarations: these const globals are defined later in the file at the
// source positions their .rodata lives at, but the reordered functions above them
// reference them.
extern const Vec vec3_gor_00016020;
extern const Vec vec3_gor_0001602c;
extern const Vec vec3_gor_00016038;
extern const Vec vec3_gor_000169f8;
extern const Vec vec3_gor_00016a04;
extern s32 evt_gor_villager_urouro_check_func[];
extern s32 gor_villager_urouro[];
extern s32 keijiban_data_alloc[];
extern s32 keijiban_data_free[];
extern s32 keijiban_joho_check[];
extern s32 keijiban_joho_makelabel[];
extern s32 luigi_gor_first_talk[];
static USER_FUNC(set_fook_evt);
static USER_FUNC(starstone_set_ry);
static USER_FUNC(starstone_set_pos);
static USER_FUNC(starstone_entry);
static USER_FUNC(anim_tevcallback_mobj);
static USER_FUNC(anim_tevcallback);
static USER_FUNC(yami_view);
static USER_FUNC(delete_npc_regl_evt);
static s32 npc_process(void);
static USER_FUNC(evt_osreport);
static USER_FUNC(mario_keyoff_wait);
static USER_FUNC(get_value);
static USER_FUNC(disp_off);
static USER_FUNC(evt_StatusClose);
static USER_FUNC(evt_StatusOpen);
s32 nancy_talk[175];
s32 chusan1_naguru[79];
u32 kickEnt[46];
u32 torqueEnt[46];
u32 kick2Ent[46];
static s32 borodo2_talk[280];
static char* door_data_01_npc[2];
s32 marco_hitorigoto[67];
s32 christine_nakama[760];
s32 maffia_assault[797];
s32 christine_nakama_after[82];
s32 iri_09_tentyo_talk[172];
s32 iri_16_tyutyu_talk[445];
s32 cooking_evt[852];
// .bss order is last-declared-first: the original has item_tbl at 0x0 and
// iri_09_item_tbl at 0x54, so iri_09_item_tbl is declared first.
void* iri_09_item_tbl[1];
static u32 item_tbl[21];

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

static EVT_BEGIN(sell_evt)
    CALL(evt_pouch_get_havebadgecnt, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_06"))
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("badgeshop_07"))
    CALL(evt_win_coin_on, 0, LW(8))
    LABEL(0)
    CALL(evt_win_other_select, 12)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("badgeshop_22"), 0, STRING("バッジ店員"))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(evt_sub_get_language, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_insert, 0, STRING("badgeshop_08"), 0, STRING("バッジ店員"), LW(3), LW(2))
    ELSE
        CALL(evt_msg_print_insert, 0, STRING("badgeshop_08"), 0, STRING("バッジ店員"), LW(2), LW(3))
    END_IF
    CALL(evt_msg_select, 0, STRING("badgeshop_24"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_09"))
        GOTO(0)
    END_IF
    CALL(N_evt_pouch_remove_item_index, LW(1), LW(4), LW(0))
    CALL(evt_badgeShop_throw_inc, LW(1))
    CALL(evt_pouch_add_coin, LW(3))
    CALL(evt_win_coin_wait, LW(8))
    WAIT_MS(500)
    CALL(evt_win_coin_off, LW(8))
    CALL(evt_pouch_get_havebadgecnt, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_11"))
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("badgeshop_10"))
    CALL(evt_msg_select, 0, STRING("badgeshop_24"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_22"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_win_coin_on, 0, LW(8))
    GOTO(0)
    RETURN
EVT_END()
USER_FUNC(unk_gor_00004db4) {
    s32* args = event->args;

    evtSetValue(event, args[0], U_badgeShop_SpecialCheck());
    return EVT_RETURN_DONE;
}

EVT_BEGIN(zaiko_buy_evt)
    CALL(evt_badgeShop_throw_get_kind_cnt, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(unk_gor_00004db4, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("badgeshop_37"))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("badgeshop_40"))
        END_IF
        RETURN
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_win_coin_on, 0, LW(8))
    LABEL(0)
    CALL(evt_win_other_select, 11)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("badgeshop_22"), 0, STRING("バッジ店員"))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(evt_sub_get_language, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_insert, 0, STRING("badgeshop_00"), 0, STRING("バッジ店員"), LW(3), LW(2), LW(4))
    ELSE
        CALL(evt_msg_print_insert, 0, STRING("badgeshop_00"), 0, STRING("バッジ店員"), LW(2), LW(3), LW(4))
    END_IF
    CALL(evt_msg_select, 0, STRING("badgeshop_24"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_continue)
        GOTO(0)
    END_IF
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_msg_print_add, 0, STRING("badgeshop_01"))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(evt_pouch_add_item, LW(1), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_02"))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(evt_badgeShop_throw_dec, LW(1))
    MULTIPLY(LW(3), -1)
    CALL(evt_pouch_add_coin, LW(3))
    CALL(evt_win_coin_wait, LW(8))
    WAIT_MS(500)
    CALL(evt_win_coin_off, LW(8))
    CALL(evt_badgeShop_throw_get_kind_cnt, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_11"))
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("badgeshop_38"))
    CALL(evt_msg_select, 0, STRING("badgeshop_24"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_22"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_win_coin_on, 0, LW(8))
    GOTO(0)
    RETURN
EVT_END()

static EVT_BEGIN(buy_evt)
    CALL(evt_msg_print_add, 0, STRING("badgeshop_36"))
    CALL(evt_msg_select, 0, STRING("badgeshop_35"))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(unk_gor_00004db4, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_badgeShop_throw_get_kind_cnt, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print_add, 0, STRING("badgeshop_37"))
            ELSE
                CALL(evt_msg_print_add, 0, STRING("badgeshop_39"))
            END_IF
        ELSE
            CALL(evt_msg_print_add, 0, STRING("badgeshop_05"))
        END_IF
        CASE_EQUAL(1)
        CALL_CHILD(zaiko_buy_evt)
        CASE_EQUAL(2)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_22"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(badgeshopper_talk_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(unitwin_get_work_ptr, LW(10))
    CALL(evt_msg_print, 0, STRING("badgeshop_04"), 0, STRING("バッジ店員"))
    CALL(evt_msg_select, 0, STRING("badgeshop_23"))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL_CHILD(buy_evt)
        CASE_EQUAL(1)
        CALL_CHILD(sell_evt)
        CASE_EQUAL(2)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_22"))
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(badgemaster_init)
    IF_INT_EQUAL(GSW(1420), 16)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_IF
    IF_INT_EQUAL(GSWF(5390), 1)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 127)
        IF_INT_LESS_OR_EQUAL(GSW(0), 148)
            CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(badgemaster_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_024_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_024_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_024_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_024_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_024_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_024_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_024_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_024_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_024_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kinoji_init)
    RETURN
EVT_END()

EVT_BEGIN(kinoji_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_019"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_019_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_019_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_019_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_019_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_019_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_019_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_019_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_019_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(nancy_init_check)
    CALL(evt_npc_change_interrupt, STRING("me"), 6, 0)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        SET_VALUE(LW(3), 0)
        IF_INT_GREATER(LW(10), -580)
            IF_INT_LESS(LW(10), -400)
                IF_INT_GREATER(LW(11), -10)
                    IF_INT_LESS(LW(11), 5)
                        IF_INT_GREATER(LW(12), 0)
                            IF_INT_LESS(LW(12), 180)
                                SET_VALUE(LW(3), 1)
                            END_IF
                        END_IF
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(3), 1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_npc_get_reglid, STRING("ナンシー"), LW(0))
    DELETE_EVENT(LW(0))
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_mario_set_dir_npc, STRING("ナンシー"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_party_wait_landon, 0)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir_npc, 0, STRING("ナンシー"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_party_wait_landon, 1)
        CALL(evt_party_stop, 1)
        CALL(evt_party_set_dir_npc, 1, STRING("ナンシー"))
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("gor_01_000"), 0, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), LW(2))
    ADD(LW(1), 335)
    ADD(LW(2), -8)
    IF_INT_LESS_OR_EQUAL(LW(0), -520)
        SET_VALUE(LW(0), -520)
    END_IF
    CALL(evt_cam3d_evt_set, LW(0), 99, LW(1), LW(0), 39, LW(2), 500, 11)
    WAIT_MS(800)
    SET_VALUE(LW(3), 90)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    IF_INT_GREATER(LW(0), LW(10))
        ADD(LW(3), 180)
    END_IF
    CALL(evt_npc_set_ry, STRING("me"), LW(3))
    CALL(evt_msg_print, 0, STRING("gor_01_001"), 0, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(700)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("gor_01_002"), 0, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(700)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(700)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(700)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("gor_01_003"), 0, STRING("me"))
    WAIT_MS(300)
    CALL(evt_npc_restart_regular_event, STRING("me"))
    CALL(evt_cam3d_evt_off, 800, 11)
    WAIT_MS(800)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(0), -400)
                BREAK_LOOP
            END_IF
            IF_INT_LESS_OR_EQUAL(LW(0), -550)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    LOOP(0)
        CALL(evt_mario_get_state, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(60)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_wait_landon)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 5)
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_LENS_BREAK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("gor_01_004"), 0, 0)
    CALL(evt_npc_get_reglid, STRING("me"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), LW(2))
    ADD(LW(1), 250)
    ADD(LW(2), 30)
    IF_INT_LESS_OR_EQUAL(LW(0), -500)
        SET_VALUE(LW(0), -500)
    END_IF
    CALL(evt_cam3d_evt_set, LW(0), 80, LW(1), LW(0), 40, LW(2), 500, 11)
    WAIT_MS(400)
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("gor_01_005"), 0, STRING("me"))
    CALL(evt_mario_set_dir_npc, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    IF_INT_LESS(LW(0), LW(10))
        IF_INT_LESS(LW(10), -550)
            GOTO(100)
        END_IF
        SUBTRACT(LW(10), 30)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    ELSE
        LABEL(100)
        ADD(LW(10), 30)
        CALL(evt_npc_set_ry, STRING("me"), 270)
    END_IF
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_npc_move_position, STRING("me"), LW(10), LW(12), 0, FLOAT(180.0f), 0)
    CALL(evt_mario_set_dir_npc, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    IF_INT_LESS(LW(0), LW(10))
        CALL(evt_npc_set_ry, STRING("me"), 90)
    ELSE
        CALL(evt_npc_set_ry, STRING("me"), 270)
    END_IF
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NO1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_B"))
    WAIT_MS(800)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("gor_01_006"), 0, STRING("me"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NO1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_C"))
    WAIT_MS(400)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(400)
    CALL(evt_npc_set_ry, STRING("me"), 270)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SET_VALUE(GF(0), 0)
    IF_INT_LESS_OR_EQUAL(LW(0), -540)
        SET_VALUE(GF(0), 1)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_mov_pos2, -530, LW(2), FLOAT(180.0f))
            SET_VALUE(GF(0), 0)
            CALL(evt_mario_set_dir, 270, 200, 0)
        END_INLINE_EVENT
    END_IF
    CALL(evt_npc_move_position, STRING("me"), -573, 56, 0, FLOAT(180.0f), 0)
    WAIT_WHILE(GF(0))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("gor_01_007"), 0, STRING("me"))
    CALL(evt_npc_restart_regular_event, STRING("me"))
    CALL(evt_npc_change_interrupt, STRING("me"), 6, PTR(nancy_talk))
    CALL(evt_cam3d_evt_off, 0, 11)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSWF(1187), 1)
    RETURN
EVT_END()

EVT_BEGIN(nancy_init)
    SET_VALUE(LSWF(1), 0)
    IF_INT_EQUAL(GSWF(1188), 0)
        IF_INT_EQUAL(GSWF(1187), 0)
            CALL(evt_npc_set_ry, STRING("me"), 270)
            CALL(evt_npc_set_position, STRING("me"), -500, 0, 75)
            SPAWN_EVENT(nancy_init_check)
        ELSE
            CALL(evt_npc_set_ry, STRING("me"), 90)
            CALL(evt_npc_set_position, STRING("me"), -573, 0, 56)
        END_IF
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("w_bero"), 8388612)
    ELSE
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_set_position, STRING("me"), -405, 10, -100)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(nancy_door_anime)
    CALL(evt_mapobj_get_position, STRING("S_ie_01_naka"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_01_naka"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 0, 108, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 11, 108, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("S_ie_01_naka"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_01_naka"), 1)
    SET_VALUE(GF(0), 0)
    RETURN
EVT_END()
USER_FUNC(nancy_block_init_func) {
    NpcEntry* npc = evtNpcNameToPtr(event, "ナンシー");
    f32 z = npc->position.z;

    ((f32*)event->lwData)[10] = z;
    ((f32*)event->lwData)[11] = z;
    npc->flags |= 0x400000;
    return EVT_RETURN_DONE;
}

USER_FUNC(nancy_block_func) {
    MarioWork* mario = marioGetPtr();
    NpcEntry* npc = evtNpcNameToPtr(event, "ナンシー");
    f32 value;
    f32 limit;
    f32 clamped;

    ((f32*)event->lwData)[10] = npc->position.z;
    if (distABf(npc->position.x, npc->position.z, mario->position.x, mario->position.z) < 30.0f) {
        return EVT_RETURN_DONE;
    }
    if (distABf(npc->position.x, npc->position.z, mario->position.x, mario->position.z) > 130.0f) {
        return EVT_RETURN_DONE;
    }
    value = mario->position.z;
    limit = ((f32*)event->lwData)[11];
    clamped = value;
    if (value > limit + 40.0f) {
        clamped = limit + 40.0f;
    }
    if (value < limit - 40.0f) {
        clamped = limit - 40.0f;
    }
    if (npc->position.z <= clamped) {
        npc->position.z += 4.0f;
        if (npc->position.z > clamped) {
            npc->position.z = clamped;
        }
    }
    if (npc->position.z > clamped) {
        npc->position.z -= 4.0f;
        if (npc->position.z <= clamped) {
            npc->position.z = clamped;
        }
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(nancy_regl)
    IF_INT_EQUAL(GSWF(1187), 0)
        IF_INT_EQUAL(GSWF(1188), 0)
            CALL(urouro_init_func, STRING("me"), -500, 75, FLOAT(50.0f), FLOAT(45.0f), 0)
            CALL(urouro_main_func, STRING("me"))
        END_IF
    ELSE
        IF_INT_EQUAL(GSWF(1188), 0)
            CALL(nancy_block_init_func)
            LOOP(0)
                CALL(nancy_block_func)
                WAIT_FRAMES(1)
            END_LOOP
        END_IF
    END_IF
    RETURN
EVT_END()

// TODO: type
u32 item_tbl_nancy[2] = {
    0x00000033, 0xFFFFFFFF,
};

EVT_BEGIN(nancy_talk)
    IF_INT_EQUAL(GSWF(1188), 0)
        CALL(evt_pouch_check_item, 51, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(evt_msg_print, 0, STRING("gor_01_008"), 0, STRING("me"))
            CALL(evt_win_item_select, 0, PTR(item_tbl_nancy), LW(1), 0)
            IF_INT_EQUAL(LW(1), -1)
                SET_VALUE(LW(0), 0)
            END_IF
        END_IF
        IF_INT_LESS(LW(0), 1)
            CALL(evt_mario_set_pose, STRING("M_N_B"))
            WAIT_MS(800)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            WAIT_MS(200)
            CALL(evt_msg_print, 0, STRING("gor_01_009"), 0, STRING("me"))
        ELSE
            CALL(evt_npc_get_reglid, STRING("me"), LW(10))
            DELETE_EVENT(LW(10))
            WAIT_MS(500)
            CALL(evt_npc_reverse_ry, STRING("me"))
            WAIT_MS(700)
            CALL(evt_npc_reverse_ry, STRING("me"))
            WAIT_MS(1000)
            CALL(evt_pouch_remove_item, 51, LW(0))
            CALL(evt_msg_print, 0, STRING("gor_01_010"), 0, STRING("me"))
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
            CALL(evt_npc_set_ry, STRING("me"), 90)
            CALL(evt_npc_move_position, STRING("me"), -380, 50, 0, FLOAT(120.0f), 0)
            CALL(evt_npc_jump_position_nohit, STRING("me"), -380, 10, 20, 200, 30)
            CALL(evt_npc_set_ry, STRING("me"), 270)
            WAIT_MS(60)
            CALL(evt_msg_print, 0, STRING("gor_01_011"), 0, STRING("me"))
            WAIT_MS(500)
            SET_VALUE(GF(0), 1)
            SPAWN_EVENT(nancy_door_anime)
            WAIT_MS(500)
            CALL(evt_npc_move_position, STRING("me"), -370, -30, 0, FLOAT(80.0f), 0)
            WAIT_WHILE(GF(0))
            CALL(evt_npc_set_position, STRING("me"), -455, 10, -170)
            CALL(evt_npc_set_ry, STRING("me"), 270)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
            SET_VALUE(LW(0), STRING("w_bero"))
            CALL_CHILD(bero_case_switch_on)
            CALL(evt_hitobj_attr_onoff, 1, 1, STRING("w_bero"), 8388612)
            SET_VALUE(GSWF(1188), 1)
            CALL(evt_door_set_param, GW(6), 0, PTR(door_data_01_npc))
        END_IF
    ELSE
        CALL_CHILD(cooking_evt)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(marco_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 269)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(marco_talk)
    DELETE_EVENT(GW(0))
    CALL(evt_msg_close, STRING("mac_4_004"))
    SWITCH(GSW(0))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("mac_4_005"), 0, STRING("me"))
        SET_VALUE(GSWF(1192), 1)
        SPAWN_EVENT_GET_ID(PTR(marco_hitorigoto), GW(0))
        CASE_EQUAL(222)
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        ADD(LW(0), LW(10))
        DIVIDE(LW(0), 2)
        CALL(evt_cam3d_evt_set, LW(0), 92, 128, LW(0), 40, -168, 500, 11)
        WAIT_MS(500)
        IF_INT_EQUAL(GSWF(1192), 0)
            CALL(evt_msg_print, 0, STRING("mac_4_007"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("mac_4_008"), 0, STRING("me"))
        END_IF
        CALL(evt_msg_select, 0, STRING("mac_4_007_select"))
        CALL(evt_mario_dispflag_onoff, 0, 2)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_msg_print_add, 0, STRING("mac_4_009"))
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("mac_4_010"))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("mac_4_011"))
            CASE_EQUAL(3)
            CALL(evt_msg_print_add, 0, STRING("mac_4_012"))
        END_SWITCH
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        CALL(evt_npc_get_position, STRING("me"), LW(3), LW(1), LW(2))
        SET_VALUE(LW(11), LW(3))
        SET_VALUE(LW(12), LW(3))
        CALL(evt_npc_set_ry, STRING("me"), 270)
        SUBTRACT(LW(11), 70)
        SUBTRACT(LW(12), 50)
        CALL(evt_cam3d_evt_set, LW(11), 63, 152, LW(11), 45, -169, 1000, 11)
        BEGIN_INLINE_EVENT
            CALL(evt_party_move_pos2, 0, 215, -160, FLOAT(120.0f))
            CALL(evt_party_set_dir_npc, 0, STRING("me"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_mario_mov_pos2, 250, -160, FLOAT(100.0f))
            CALL(evt_mario_set_dir_npc, STRING("me"))
        END_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("me"), LW(12), LW(2), 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(700)
        CALL(evt_msg_print, 0, STRING("mac_4_013"), 0, STRING("me"))
        WAIT_MS(500)
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(700)
        CALL(evt_msg_print, 0, STRING("mac_4_014"), 0, STRING("me"))
        WAIT_MS(500)
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("mac_4_015"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_npc_reverse_ry, STRING("me"))
        CALL(evt_msg_print, 0, STRING("mac_4_016"), 0, STRING("me"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_017"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_018"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_C_3"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 20)
        ADD(LW(1), 20)
        ADD(LW(2), 10)
        CALL(evt_item_entry, STRING("map00"), 96, LW(0), LW(1), LW(2), 16, -1, 0)
        CALL(evt_item_flag_on, STRING("map00"), 2)
        CALL(evt_item_set_scale, STRING("map00"), FLOAT(0.55f))
        WAIT_MS(500)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        SET_VALUE(LW(3), LW(0))
        ADD(LW(3), -10)
        CALL(evt_npc_move_position, STRING("me"), LW(3), LW(2), 0, FLOAT(80.0f), 0)
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("mac_4_019"), 0, STRING("me"))
        CALL(evt_item_delete, STRING("map00"))
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NO1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_B"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_020"), 0, STRING("me"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_021"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_022"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_023"), 0, STRING("me"))
        CALL(evt_npc_set_ry, STRING("me"), 90)
        WAIT_MS(500)
        CALL(evt_del_case_evt, 0, GW(8))
        CALL(evt_item_delete, STRING("ドクロジュエル"))
        WAIT_MS(500)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        WAIT_MS(500)
        CALL(evt_cam3d_evt_off, 500, 11)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_move_position, STRING("me"), 260, -100, 0, FLOAT(120.0f), 0)
            CALL(evt_npc_move_position, STRING("me"), 110, -100, 0, FLOAT(120.0f), 0)
            CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        END_INLINE_EVENT
        WAIT_MS(3000)
        SET_VALUE(GSW(0), 223)
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_031_08_1"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_031_08_2"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_031_08_3"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_031_08_4"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(master_init)
    IF_INT_EQUAL(GSW(0), 222)
        CALL(evt_npc_set_position, STRING("me"), 259, 0, -275)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_20_master_tubuyaki)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("マスター"), LW(3), LW(4), LW(5))
        CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(6))
        IF_INT_GREATER_OR_EQUAL(LW(6), 100)
            CALL(evt_msg_print, 0, STRING("irai_20_030"), 0, STRING("マスター"))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(master_talk)
    IF_INT_EQUAL(GSW(1420), 20)
        IF_INT_EQUAL(GSWF(5405), 0)
        ELSE
            IF_INT_EQUAL(GSWF(5406), 0)
                CALL(evt_msg_print, 0, STRING("irai_20_027"), 0, STRING("me"))
                CALL(evt_mario_set_pose, STRING("M_I_2"))
                WAIT_MS(1000)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_cam3d_evt_set, 320, 99, -12, 322, 43, -271, 1000, 11)
                WAIT_MS(1000)
                CALL(evt_msg_print, 0, STRING("irai_20_028"), 0, STRING("me"))
                CALL(evt_mario_set_pose, STRING("M_N_5B"))
                WAIT_MS(500)
                CALL(evt_cam3d_evt_off, 1000, 11)
                WAIT_MS(1000)
                CALL(evt_msg_print, 0, STRING("irai_20_029"), 0, STRING("me"))
                SPAWN_EVENT(iri_20_master_tubuyaki)
                SET_VALUE(GSWF(5406), 1)
                RETURN
            ELSE
                CALL(evt_msg_print, 0, STRING("irai_20_031"), 0, STRING("me"))
                RETURN
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(1420), 25)
        IF_INT_EQUAL(GSWF(5406), 1)
            IF_INT_EQUAL(GSWF(5408), 0)
                CALL(evt_msg_print, 0, STRING("irai_25_00_1"), 0, 0)
                SET_VALUE(GSWF(5407), 1)
                RETURN
            END_IF
        END_IF
    END_IF
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(223)
        CALL(evt_msg_print, 0, STRING("gor_01_016"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(224)
        CALL(evt_msg_print, 0, STRING("mac_4_047"), 0, STRING("me"))
        SET_VALUE(GSWF(1214), 1)
        CASE_EQUAL(225)
        SET_VALUE(GF(5), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(0), 320)
            SET_VALUE(GF(5), 1)
        END_IF
        IF_INT_EQUAL(GF(5), 1)
            BEGIN_INLINE_EVENT
                CALL(evt_party_move_pos2, 0, 270, -275, FLOAT(150.0f))
                CALL(evt_party_set_dir, 0, 90, 0)
            END_INLINE_EVENT
            CALL(evt_mario_mov_pos2, 295, -270, FLOAT(150.0f))
            CALL(evt_mario_set_dir, 90, 0, 0)
        ELSE
            BEGIN_INLINE_EVENT
                CALL(evt_party_move_pos2, 0, 370, -275, FLOAT(150.0f))
                CALL(evt_party_set_dir, 0, 270, 0)
            END_INLINE_EVENT
            CALL(evt_mario_mov_pos2, 345, -270, FLOAT(150.0f))
            CALL(evt_mario_set_dir, 270, 0, 0)
        END_IF
        IF_INT_EQUAL(GSWF(1194), 1)
            CALL(evt_snd_bgmoff_f, 1024, 2000)
            CALL(evt_msg_print, 0, STRING("mac_4_071"), 0, STRING("me"))
            GOTO(100)
        END_IF
        IF_INT_EQUAL(GSWF(1193), 0)
            CALL(evt_msg_print, 0, STRING("mac_4_061"), 0, STRING("me"))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_5"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(800)
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(800)
            CALL(evt_msg_print, 0, STRING("mac_4_062"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            WAIT_MS(2000)
            CALL(evt_msg_print, 0, STRING("mac_4_063"), 0, STRING("me"))
            SET_VALUE(GSWF(1193), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("mac_4_064"), 0, STRING("me"))
        END_IF
        CALL(evt_msg_select, 0, STRING("mac_4_063_yesno"))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_msg_print_add, 0, STRING("mac_4_065"))
            RETURN
        END_IF
        CALL(evt_msg_print_add, 0, STRING("mac_4_066"))
        CALL(evt_cam3d_evt_set, 300, 95, 27, 300, 43, -268, 1500, 11)
        CALL(evt_snd_bgmoff_f, 1024, 2000)
        WAIT_MS(1500)
        CALL(evt_msg_print, 0, STRING("mac_4_067"), 0, STRING("me"))
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("mac_4_068"), 0, STRING("me"))
        WAIT_MS(1000)
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(1000)
        CALL(evt_msg_print_party, STRING("mac_4_069"))
        SET_VALUE(GSWF(1194), 1)
        CALL(evt_cam3d_evt_off, 0, 11)
        CALL(evt_msg_print, 0, STRING("mac_4_070"), 0, STRING("me"))
        LABEL(100)
        CALL(evt_msg_select, 0, STRING("mac_4_070_yesno"))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_msg_print_add, 0, STRING("mac_4_072"))
            CALL(evt_snd_bgmoff_f, 513, 1000)
            CALL(evt_snd_bgmon_f, 288, 0, 1000)
            RETURN
        END_IF
        CALL(evt_msg_print_add, 0, STRING("mac_4_073"))
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
        IF_INT_EQUAL(GF(5), 1)
            CALL(evt_npc_set_ry, STRING("me"), 90)
            CALL(evt_npc_move_position, STRING("me"), 345, -260, 0, FLOAT(40.0f), 0)
            CALL(evt_npc_sound_data_set, STRING("me"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, 0, 0)
            CALL(evt_npc_jump_position_nohit, STRING("me"), 345, -20, -260, 500, 0)
            WAIT_MS(1000)
            CALL(evt_npc_jump_position_nohit, STRING("me"), 345, 0, -260, 500, 0)
            CALL(evt_npc_sound_data_reset, STRING("me"))
            CALL(evt_npc_set_ry, STRING("me"), 270)
        ELSE
            CALL(evt_npc_set_ry, STRING("me"), 270)
            CALL(evt_npc_move_position, STRING("me"), 295, -260, 0, FLOAT(40.0f), 0)
            CALL(evt_npc_jump_position_nohit, STRING("me"), 295, -20, -260, 500, 0)
            WAIT_MS(1000)
            CALL(evt_npc_jump_position_nohit, STRING("me"), 295, 0, -260, 500, 0)
            WAIT_MS(500)
            CALL(evt_npc_set_ry, STRING("me"), 90)
        END_IF
        CALL(evt_npc_move_position, STRING("me"), 325, -275, 0, FLOAT(40.0f), 0)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_item_entry, STRING("letter"), 69, LW(0), LW(1), LW(2), 16, -1, 0)
        CALL(evt_item_flag_on, STRING("letter"), 10)
        CALL(evt_item_get_item, STRING("letter"))
        CALL(evt_cam3d_evt_set, 300, 95, 27, 300, 43, -268, 500, 11)
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_075"), 0, STRING("me"))
        CALL(evt_cam3d_evt_off, 500, 11)
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_076"), 0, STRING("me"))
        CALL(evt_msg_print_party, STRING("mac_4_077"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(800)
        SET_VALUE(GSW(0), 226)
        CALL(evt_snd_bgmoff_f, 513, 1000)
        CALL(evt_snd_bgmon_f, 288, 0, 1000)
        CASE_EQUAL(226)
        CALL(evt_msg_print, 0, STRING("mac_4_076"), 0, STRING("me"))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_016"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_016_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_016_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_016_03"), 0, STRING("me"))
        CASE_LESS(223)
        CALL(evt_msg_print, 0, STRING("gor_01_016_04"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_016_05"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_016_06"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_016_07"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_016_08"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_016_09"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(shoptender_init)
    IF_INT_LESS(GSW(0), 223)
        IF_INT_EQUAL(GSWF(1191), 1)
            CALL(evt_npc_set_position, STRING("me"), -75, 0, -30)
            CALL(evt_npc_set_ry, STRING("me"), 90)
        END_IF
    ELSE
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(shoptender_talk)
    DEBUG_PUT_MSG(STRING("店員TALK"))
    IF_INT_EQUAL(GSWF(1188), 0)
        DEBUG_PUT_MSG(STRING("店員TALK２"))
        IF_INT_EQUAL(GSWF(1191), 1)
            CALL(evt_pouch_check_item, 51, LW(0))
            IF_INT_LESS(LW(0), 1)
                CALL(evt_msg_print, 0, STRING("gor_01_015"), 0, STRING("me"))
            ELSE
                CALL(evt_msg_print, 0, STRING("gor_01_015_01"), 0, STRING("me"))
            END_IF
            RETURN
        END_IF
        SET_VALUE(LW(0), GSWF(1189))
        ADD(LW(0), GSWF(1190))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("gor_01_014"), 0, STRING("me"))
            RETURN
        END_IF
        CALL(evt_msg_print, 0, STRING("gor_01_012"), 0, STRING("me"))
        IF_INT_EQUAL(GSWF(1187), 1)
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(700)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
            CALL(evt_msg_print, 0, STRING("gor_01_013"), 0, STRING("me"))
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
            CALL(evt_cam3d_evt_set, 358, 123, -301, -189, 75, -301, 2600, 11)
            CALL(evt_npc_set_ry, STRING("me"), 0)
            CALL(evt_npc_move_position, STRING("me"), LW(10), -330, 0, FLOAT(90.0f), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("T_1"))
            WAIT_MS(1000)
            CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
            CALL(evt_npc_set_anim, STRING("店長"), STRING("T_1"))
            WAIT_MS(1000)
            CALL(evt_npc_set_anim, STRING("店長"), STRING("S_1"))
            CALL(evt_npc_set_anim, STRING("me"), STRING("T_1"))
            WAIT_MS(500)
            CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
            WAIT_MS(500)
            CALL(evt_cam3d_evt_off, 2600, 11)
            CALL(evt_npc_set_ry, STRING("me"), 180)
            CALL(evt_npc_move_position, STRING("me"), LW(10), LW(12), 0, FLOAT(90.0f), 0)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
            CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("gor_01_013_01"), 0, STRING("me"))
            IF_INT_LESS(GSW(0), 56)
                SET_VALUE(GSWF(1189), 1)
            ELSE
                SET_VALUE(GSWF(1190), 1)
            END_IF
            CALL(evt_sub_save_playtime, GSW(108))
        END_IF
    ELSE
        DEBUG_PUT_MSG(STRING("店員TALK３"))
        SWITCH(GSW(0))
            CASE_LESS(112)
            CALL(evt_msg_print, 0, STRING("gor_01_015_02"), 0, STRING("me"))
            CASE_LESS(164)
            CALL(evt_msg_print, 0, STRING("gor_01_015_03"), 0, STRING("me"))
            CASE_LESS(215)
            CALL(evt_msg_print, 0, STRING("gor_01_015_04"), 0, STRING("me"))
            CASE_LESS(261)
            CALL(evt_msg_print, 0, STRING("gor_01_015_05"), 0, STRING("me"))
        END_SWITCH
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(yadotender_init)
    RETURN
EVT_END()

EVT_BEGIN(yadotender_talk)
    RETURN
EVT_END()

EVT_BEGIN(roten1_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    RETURN
EVT_END()

EVT_BEGIN(roten1_regl)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), 450, 150, 0, FLOAT(32.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("me"), -350, 150, 0, FLOAT(32.0f), 0)
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(roten1_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_025_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_025_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_025_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_025_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_025_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_025_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_025_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_025_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_025_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(roten2_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    RETURN
EVT_END()

EVT_BEGIN(roten2_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_025_00_2"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_025_01_2"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_025_02_2"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_025_03_2"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_025_04_2"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_025_05_2"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_025_06_2"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_025_07_2"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_025_08_2"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kuribo1_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    RETURN
EVT_END()

EVT_BEGIN(kuribo1_regl)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), 375, 390, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("me"), -375, 390, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kuribo1_talk)
    IF_INT_EQUAL(GSW(1420), 4)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_04_001"), 0, STRING("me"))
            SET_VALUE(GSWF(5405), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_04_002"), 0, STRING("me"))
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_026_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_026_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_026_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_026_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_026_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_026_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_026_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_026_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_026_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kuribo2_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    RETURN
EVT_END()

EVT_BEGIN(kuribo2_regl)
    CALL_CHILD(gor_villager_urouro)
    RETURN
EVT_END()

EVT_BEGIN(kuribo2_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_027_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_027_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_027_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_027_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_027_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_027_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_027_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_027_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_027_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kuribo3_init)
    RETURN
EVT_END()

EVT_BEGIN(kuribo3_regl)
    CALL_CHILD(gor_villager_urouro)
    RETURN
EVT_END()

EVT_BEGIN(kuribo3_talk)
    IF_INT_EQUAL(GSW(1420), 2)
        IF_INT_EQUAL(GSWF(5406), 0)
            IF_INT_EQUAL(GSWF(5405), 0)
                CALL(evt_msg_print, 0, STRING("irai_02_00"), 0, STRING("me"))
                CALL(evt_item_entry, STRING("irai_item"), 95, 0, -1000, 0, 16, -1, 0)
                CALL(evt_item_flag_on, STRING("irai_item"), 2)
                CALL(evt_item_get_item, STRING("irai_item"))
                SET_VALUE(GSWF(5405), 1)
            ELSE
                CALL(evt_msg_print, 0, STRING("irai_02_01"), 0, STRING("me"))
            END_IF
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_02_02"), 0, STRING("me"))
            CALL(evt_sub_get_coin, 20)
            SET_VALUE(GSW(1420), 0)
            SET_VALUE(GSWF(5376), 1)
            CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_028_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_028_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_028_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_028_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_028_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_028_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_028_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_028_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_028_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(chusan1_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    IF_INT_EQUAL(GSW(1420), 5)
        CALL(evt_npc_set_position, STRING("me"), -80, 0, 40)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        SET_VALUE(GF(9), 1)
        SPAWN_EVENT(chusan1_naguru)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(chusan1_regl)
    IF_INT_NOT_EQUAL(GSW(1420), 5)
        DEBUG_PUT_MSG(STRING("マルチュロ：普段のうろうろ"))
        CALL_CHILD(gor_villager_urouro)
    END_IF
    DEBUG_PUT_MSG(STRING("マルチュロ：依頼時は何もしない"))
    RETURN
EVT_END()

EVT_BEGIN(chusan1_dead)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_REQUEST_DAMAGED1"), LW(0), LW(1), LW(2), 0)
    IF_INT_GREATER_OR_EQUAL(GW(9), 11)
        SET_VALUE(GW(9), 0)
    END_IF
    ADD(GW(9), 1)
    BEGIN_INLINE_EVENT
        SWITCH(GW(9))
            CASE_EQUAL(1)
            CALL(evt_msg_print, 0, STRING("irai_05_004"), 0, STRING("me"))
            CASE_EQUAL(2)
            CALL(evt_msg_print, 0, STRING("irai_05_005"), 0, STRING("me"))
            CASE_EQUAL(3)
            CALL(evt_msg_print, 0, STRING("irai_05_006"), 0, STRING("me"))
            CASE_EQUAL(4)
            CALL(evt_msg_print, 0, STRING("irai_05_007"), 0, STRING("me"))
            CASE_EQUAL(5)
            CALL(evt_msg_print, 0, STRING("irai_05_008"), 0, STRING("me"))
            CASE_EQUAL(6)
            CALL(evt_msg_print, 0, STRING("irai_05_009"), 0, STRING("me"))
            CASE_EQUAL(7)
            CALL(evt_msg_print, 0, STRING("irai_05_010"), 0, STRING("me"))
            CASE_EQUAL(8)
            CALL(evt_msg_print, 0, STRING("irai_05_011"), 0, STRING("me"))
            CASE_EQUAL(9)
            CALL(evt_msg_print, 0, STRING("irai_05_012"), 0, STRING("me"))
            CASE_EQUAL(10)
            CALL(evt_msg_print, 0, STRING("irai_05_013"), 0, STRING("me"))
            CASE_EQUAL(11)
            CALL(evt_msg_print, 0, STRING("irai_05_014"), 0, STRING("me"))
        END_SWITCH
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()
USER_FUNC(chusan1_alwaysencont) {
    s32* args = event->args;
    NpcWork* work = npcGetWorkPtr();
    s32 value = evtGetValue(event, args[0]);

    if (value != 0) {
        work->flags |= 1;
    } else {
        work->flags &= ~1;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(chusan1_naguru)
    WAIT_WHILE(GF(9))
    CALL(evt_mario_key_onoff, 0)
    CALL(chusan1_alwaysencont, 1)
    CALL(evt_npc_get_reglid, STRING("チューさん１"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_reaction_flag_onoff, 1, STRING("チューさん１"), 2)
    CALL(evt_npc_change_interrupt, STRING("チューさん１"), 8, PTR(chusan1_dead))
    CALL(evt_mario_set_enable_key, 512)
    WAIT_MS(15000)
    CALL(evt_mario_set_enable_key, 0)
    CALL(evt_npc_reaction_flag_onoff, 0, STRING("チューさん１"), 2)
    IF_INT_GREATER_OR_EQUAL(GW(9), 11)
        CALL(evt_msg_print, 0, STRING("irai_05_015"), 0, STRING("チューさん１"))
        SET_VALUE(GSW(1420), 0)
        SET_VALUE(GSWF(5379), 1)
        CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, STRING("irai_05_016"), 0, STRING("チューさん１"))
        SET_VALUE(GF(9), 1)
        SPAWN_EVENT(chusan1_naguru)
    END_IF
    CALL(chusan1_alwaysencont, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(chusan1_talk)
    IF_INT_EQUAL(GSW(1420), 5)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
        CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
        BEGIN_INLINE_EVENT
            ADD(LW(10), 60)
            CALL(evt_party_move_pos2, 0, LW(10), LW(12), FLOAT(120.0f))
            CALL(evt_party_set_dir, 0, 270, 200)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            ADD(LW(10), 90)
            CALL(evt_party_move_pos2, 1, LW(10), LW(12), FLOAT(120.0f))
            CALL(evt_party_set_dir, 1, 270, 200)
        END_INLINE_EVENT
        ADD(LW(10), 30)
        CALL(evt_mario_mov_pos2, LW(10), LW(12), FLOAT(120.0f))
        CALL(evt_mario_set_dir, 270, 200, 0)
        CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
        WAIT_MS(500)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_05_001"), 0, STRING("me"))
            SET_VALUE(GSWF(5405), 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_05_002"), 0, STRING("me"))
        END_IF
        CALL(evt_msg_print, 0, STRING("irai_05_003"), 0, STRING("me"))
        SET_VALUE(GW(9), 0)
        SET_VALUE(GF(9), 0)
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_029_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_029_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_029_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_029_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_029_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_029_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_029_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_029_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_029_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(chusan2_init)
    RETURN
EVT_END()

EVT_BEGIN(chusan2_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_030_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_030_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_030_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_030_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_030_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_030_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_030_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_030_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_030_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(bomhei_init)
    RETURN
EVT_END()

EVT_BEGIN(bomhei_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_01_031_00"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_01_031_01"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_01_031_02"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_01_031_03"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("gor_01_031_04"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_01_031_05"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_01_031_06"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_01_031_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_01_031_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(bottakuru_init)
    IF_INT_LESS(GSW(0), 56)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        RETURN
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741824)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    SET_VALUE(GSWF(1206), 0)
    RETURN
EVT_END()

EVT_BEGIN(bottakuru_talk)
    CALL(U_evt_badgeShop_get_special_zaiko, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("bottakuru_16"), 0, STRING("me"))
        RETURN
    ELSE
        CALL(evt_badgeShop_bottakuru_get_kind_cnt, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("bottakuru_03"), 0, STRING("me"))
            RETURN
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1205), 0)
        CALL(evt_msg_print, 0, STRING("bottakuru_00"), 0, STRING("me"))
        SET_VALUE(GSWF(1205), 1)
    ELSE
        IF_INT_EQUAL(GSWF(1206), 0)
            CALL(evt_msg_print, 0, STRING("bottakuru_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("bottakuru_02"), 0, STRING("me"))
        END_IF
    END_IF
    CALL(evt_msg_select, 0, STRING("bottakuru_yesno"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("bottakuru_04"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    LABEL(0)
    CALL(evt_win_coin_on, 0, LW(12))
    CALL(evt_win_other_select, 14)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print, 0, STRING("bottakuru_14"), 0, STRING("me"))
        RETURN
    END_IF
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print, 0, STRING("bottakuru_05"), 0, STRING("me"))
        GOTO(0)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(1), 240)
        CALL(evt_sub_get_language, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_insert, 0, STRING("bottakuru_07"), 0, STRING("me"), LW(3), LW(2), LW(4))
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("bottakuru_07"), 0, STRING("me"), LW(2), LW(3), LW(4))
        END_IF
    ELSE
        CALL(evt_sub_get_language, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_insert, 0, STRING("bottakuru_06"), 0, STRING("me"), LW(3), LW(2))
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("bottakuru_06"), 0, STRING("me"), LW(2), LW(3))
        END_IF
    END_IF
    CALL(evt_msg_select, 0, STRING("bottakuru_yesno"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print_add, 0, STRING("bottakuru_08"))
        RETURN
    END_IF
    CALL(evt_pouch_add_item, LW(1), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(evt_win_coin_off, LW(12))
        IF_INT_GREATER_OR_EQUAL(LW(1), 240)
            CALL(evt_msg_print_add, 0, STRING("bottakuru_12"))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("bottakuru_13"))
        END_IF
        RETURN
    END_IF
    CALL(evt_badgeShop_bottakuru_dec, LW(1))
    MULTIPLY(LW(3), -1)
    CALL(evt_pouch_add_coin, LW(3))
    CALL(evt_win_coin_wait, LW(12))
    WAIT_MS(500)
    CALL(evt_win_coin_off, LW(12))
    SET_VALUE(GSWF(1206), 1)
    CALL(U_evt_badgeShop_get_special_zaiko, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("bottakuru_15"))
        RETURN
    ELSE
        CALL(evt_badgeShop_bottakuru_get_kind_cnt, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("bottakuru_09"))
            RETURN
        END_IF
    END_IF
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("bottakuru_09"))
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("bottakuru_10"))
    CALL(evt_msg_select, 0, STRING("bottakuru_yesno"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("bottakuru_11"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    GOTO(0)
    RETURN
EVT_END()

static EVT_BEGIN(luigi_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(112, 163)
        CALL(evt_npc_set_position, STRING("me"), 380, 0, 300)
        CALL(evt_npc_set_ry_lr, STRING("me"), 0)
        CASE_BETWEEN(215, 260)
        CALL(evt_npc_set_position, STRING("me"), 110, 0, -240)
        CALL(evt_npc_set_ry_lr, STRING("me"), 1)
        CASE_GREATER_OR_EQUAL(373)
        CALL(evt_npc_set_position, STRING("me"), 230, 154, -300)
        CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(luigi_talk)
    CALL_CHILD(luigi_gor_first_talk)
    RETURN
EVT_END()

static EVT_BEGIN(luigi_npcEnt)
    SWITCH(GSW(0))
        CASE_BETWEEN(112, 163)
        CALL(evt_npc_entry, STRING("キック"), STRING("c_lp_gesso"))
        CALL(evt_npc_set_tribe, STRING("キック"), STRING("キック"))
        CALL(evt_npc_setup, PTR(kickEnt))
        CASE_BETWEEN(215, 260)
        CALL(evt_npc_entry, STRING("トルク"), STRING("c_lp_met"))
        CALL(evt_npc_set_tribe, STRING("トルク"), STRING("トルク"))
        CALL(evt_npc_setup, PTR(torqueEnt))
        CASE_GREATER_OR_EQUAL(373)
        CALL(evt_npc_entry, STRING("キック２"), STRING("c_lp_gesso"))
        CALL(evt_npc_set_tribe, STRING("キック２"), STRING("キック２"))
        CALL(evt_npc_setup, PTR(kick2Ent))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kick_init)
    CALL(evt_npc_set_position, STRING("me"), 420, 0, 300)
    CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(kick_talk)
    CALL(evt_msg_print, 0, STRING("luigi_npc_01"), 0, STRING("me"))
    RETURN
EVT_END()

// TODO: type
u32 kickEnt[46] = {
    (u32)"キック", 0x00080600, 0x00000000, (u32)&kick_init,
    0x00000000, (u32)&kick_talk, 0x00000000, 0x00000000,
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

EVT_BEGIN(torque_init)
    CALL(evt_npc_set_position, STRING("me"), 160, 0, -240)
    CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(torque_talk)
    CALL(evt_msg_print, 0, STRING("luigi_npc_03"), 0, STRING("me"))
    RETURN
EVT_END()

// TODO: type
u32 torqueEnt[46] = {
    (u32)"トルク", 0x00080600, 0x00000000, (u32)&torque_init,
    0x00000000, (u32)&torque_talk, 0x00000000, 0x00000000,
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

EVT_BEGIN(kick2_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(373, 400)
        CALL(evt_npc_set_position, STRING("me"), 270, 154, -300)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(kick2_talk)
    CALL(evt_msg_print, 0, STRING("luigi_npc_06"), 0, STRING("me"))
    RETURN
EVT_END()

// TODO: type
u32 kick2Ent[46] = {
    (u32)"キック２", 0x00080600, 0x00000000, (u32)&kick2_init,
    0x00000000, (u32)&kick2_talk, 0x00000000, 0x00000000,
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

static EVT_BEGIN(borodo2_init)
    IF_INT_EQUAL(GSW(1420), 4)
        IF_INT_EQUAL(GSWF(5405), 1)
            IF_INT_EQUAL(GSWF(5406), 1)
                IF_INT_EQUAL(GSWF(5407), 1)
                    IF_INT_EQUAL(GSWF(5408), 0)
                        CALL(evt_npc_entry, STRING("ボロドー２"), STRING("c_borodo_n"))
                        CALL(evt_npc_set_tribe, STRING("ボロドー２"), STRING("盗賊"))
                        CALL(evt_npc_flag_onoff, 1, STRING("ボロドー２"), 1536)
                        CALL(evt_npc_set_position, STRING("ボロドー２"), 402, 0, -302)
                        CALL(evt_npc_set_ry, STRING("ボロドー２"), 90)
                        CALL(evt_npc_change_interrupt, STRING("ボロドー２"), 6, PTR(borodo2_talk))
                        RETURN
                    END_IF
                END_IF
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(borodo2_talk)
    CALL(evt_npc_get_reglid, STRING("クリボー１"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_msg_print, 0, STRING("irai_04_005"), 0, STRING("me"))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("クリボー１"), 1073741856)
    CALL(evt_npc_set_position, STRING("クリボー１"), 229, 0, -400)
    CALL(evt_npc_set_ry, STRING("クリボー１"), 90)
    CALL(evt_npc_move_position, STRING("クリボー１"), 420, -400, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_move_position, STRING("クリボー１"), 420, -350, 0, FLOAT(120.0f), 0)
    CALL(evt_set_dir_to_target, STRING("クリボー１"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("irai_04_006"), 0, STRING("クリボー１"))
    CALL(evt_sub_get_coin, 20)
    CALL(evt_npc_set_ry, STRING("クリボー１"), 270)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("irai_04_008"), 0, STRING("クリボー１"))
    CALL(evt_msg_print, 0, STRING("irai_04_009"), 0, STRING("me"))
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("me"), 400, -400, 0, FLOAT(120.0f), 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("クリボー１"), 420, -400, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_set_ry, STRING("クリボー１"), 270)
    WAIT_MS(200)
    WAIT_WHILE(GF(0))
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(600)
        CALL(evt_npc_move_position, STRING("クリボー１"), 392, -400, 0, FLOAT(120.0f), 0)
        WAIT_MS(800)
        CALL(evt_npc_move_position, STRING("クリボー１"), 362, -400, 0, FLOAT(120.0f), 0)
        WAIT_MS(800)
        CALL(evt_npc_move_position, STRING("クリボー１"), 332, -400, 0, FLOAT(120.0f), 0)
        WAIT_MS(800)
        CALL(evt_npc_move_position, STRING("クリボー１"), 302, -400, 0, FLOAT(120.0f), 0)
        WAIT_MS(800)
        CALL(evt_npc_move_position, STRING("クリボー１"), 272, -400, 0, FLOAT(120.0f), 0)
        WAIT_MS(800)
        CALL(evt_npc_move_position, STRING("クリボー１"), 242, -400, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_set_position, STRING("クリボー１"), -375, 0, 390)
        CALL(evt_npc_restart_regular_event, STRING("クリボー１"))
        CALL(evt_npc_flag_onoff, 0, STRING("クリボー１"), 1073741856)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_ry, STRING("me"), 270)
    CALL(evt_npc_move_position, STRING("me"), 390, -395, 0, FLOAT(120.0f), 0)
    WAIT_MS(800)
    CALL(evt_npc_move_position, STRING("me"), 360, -395, 0, FLOAT(120.0f), 0)
    WAIT_MS(800)
    CALL(evt_npc_move_position, STRING("me"), 330, -395, 0, FLOAT(120.0f), 0)
    WAIT_MS(800)
    CALL(evt_npc_move_position, STRING("me"), 300, -395, 0, FLOAT(120.0f), 0)
    WAIT_MS(800)
    CALL(evt_npc_move_position, STRING("me"), 270, -395, 0, FLOAT(120.0f), 0)
    WAIT_MS(800)
    CALL(evt_npc_move_position, STRING("me"), 242, -395, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    WAIT_WHILE(GF(0))
    SET_VALUE(GSWF(5408), 1)
    SET_VALUE(GSW(1420), 0)
    SET_VALUE(GSWF(5378), 1)
    CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(thief_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(8)
        CALL(evt_npc_set_position, STRING("me"), 500, 0, -175)
    END_SWITCH
    RETURN
EVT_END()
static USER_FUNC(evt_StatusOpen) {
    statusWinForceOpen();
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_StatusClose) {
    statusWinForceOff();
    return EVT_RETURN_DONE;
}

EVT_BEGIN(thief_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(8, 22)
        IF_INT_EQUAL(GSWF(1198), 0)
            SET_VALUE(LF(0), 0)
            IF_INT_EQUAL(GSW(2), 0)
                IF_INT_EQUAL(GSW(3), 0)
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            IF_INT_EQUAL(LF(0), 0)
                CALL(evt_msg_print, 0, STRING("mac_0_065_01"), 0, STRING("me"))
                BEGIN_INLINE_EVENT
                    CALL(evt_StatusOpen)
                    WAIT_MS(1000)
                    CALL(evt_pouch_get_coin, LW(0))
                    ADD(LW(0), GSW(2))
                    SET_VALUE(LW(1), GSW(3))
                    MULTIPLY(LW(1), 256)
                    ADD(LW(0), LW(1))
                    CALL(evt_pouch_set_coin, LW(0))
                    WAIT_MS(2000)
                    CALL(evt_StatusClose)
                END_INLINE_EVENT
            ELSE
                CALL(evt_msg_print, 0, STRING("mac_0_065_02"), 0, STRING("me"))
            END_IF
            SET_VALUE(GSWF(1198), 1)
        ELSE
            IF_INT_EQUAL(GSWF(1199), 0)
                CALL(evt_msg_print, 0, STRING("mac_0_065_03"), 0, STRING("me"))
            ELSE
                CALL(evt_msg_print, 0, STRING("mac_0_065_04"), 0, STRING("me"))
            END_IF
        END_IF
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("mac_0_065_04"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("mac_0_065_05"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("mac_0_065_06"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("mac_0_065_07"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("mac_0_065_08"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("mac_0_065_09"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("mac_0_065_10"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("mac_0_065_11"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("mac_0_065_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[506] = {
    (u32)"バッジ店長", 0x00080600, 0x00000000, (u32)&badgemaster_init,
    0x00000000, (u32)&badgemaster_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"バッジ店員",
    0x00080600, 0x00000000, (u32)npc_init_evt, 0x00000000,
    (u32)&badgeshopper_talk_evt, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"キノじい", 0x00080600,
    0x00000000, (u32)&kinoji_init, 0x00000000, (u32)&kinoji_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ナンシー", 0x00080600, 0x00000000,
    (u32)&nancy_init, (u32)&nancy_regl, (u32)&nancy_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"マルコ", 0x00080600, 0x00000000, (u32)&marco_init,
    0x00000000, (u32)&marco_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"マスター",
    0x00080600, 0x00000000, (u32)&master_init, 0x00000000,
    (u32)&master_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"店長", 0x00080600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"店員", 0x00080600, 0x00000000,
    (u32)&shoptender_init, 0x00000000, (u32)&shoptender_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"宿店員", 0x00080600, 0x00000000, (u32)&yadotender_init,
    0x00000000, (u32)&yadotender_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ロテン１",
    0x00080600, 0x00000000, (u32)&roten1_init, (u32)&roten1_regl,
    (u32)&roten1_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ロテン２", 0x00080600,
    0x00000000, (u32)&roten2_init, 0x00000000, (u32)&roten2_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"クリボー１", 0x00080600, 0x00000000,
    (u32)&kuribo1_init, (u32)&kuribo1_regl, (u32)&kuribo1_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"クリボー２", 0x00000600, 0x00000000, (u32)&kuribo2_init,
    0x00000000, (u32)&kuribo2_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"クリボー３",
    0x00080600, 0x00000000, (u32)&kuribo3_init, (u32)&kuribo3_regl,
    (u32)&kuribo3_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"チューさん１", 0x00080600,
    0x00000000, (u32)&chusan1_init, (u32)&chusan1_regl, (u32)&chusan1_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"チューさん２", 0x00080600, 0x00000000,
    (u32)&chusan2_init, 0x00000000, (u32)&chusan2_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"水夫ボム兵", 0x00080600, 0x00000000, (u32)&bomhei_init,
    0x00000000, (u32)&bomhei_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ボッタクール",
    0x00080600, 0x00000000, (u32)&bottakuru_init, 0x00000000,
    (u32)&bottakuru_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ルイージ", 0x00080600,
    0x00000000, (u32)&luigi_init, 0x00000000, (u32)&luigi_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"盗賊", 0x00080600, 0x00000000,
    (u32)&thief_init, 0x00000000, (u32)&thief_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"キノピコ", 0x00080600, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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

EVT_BEGIN(christine_nakama_before)
    IF_INT_NOT_EQUAL(GSW(0), 6)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    SPAWN_EVENT(christine_nakama)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[4] = {
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gor_02", "w_bero", 1, 1},
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gor_03", "e_bero", 1, 1},
    {"s_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, christine_nakama_before, -1, NULL, "gor_00", "n_bero", 1, 1},
    {NULL},
};

static EVT_BEGIN(evt_door_01_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_01_01)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01_doa"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_01)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01uB"), LW(2), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_01sB"), LW(3), 0, 0)
    RETURN
EVT_END()

static char* door_data_01_npc[2] = {
    "ナンシー", NULL,
};

char* door_data_01_item[2] = {
    "iri_18", NULL,
};

static EVT_BEGIN(evt_door_01_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_01_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
static u32 door_data_01[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_01_doa",
    (u32)"A_ie_01_doa02", (u32)"A_ie_01_doa01", (u32)"S_ie_01_soto", (u32)"A_ie_01_A",
    (u32)"S_ie_01_naka", (u32)"A_ie_01_C", (u32)&evt_door_01_00, (u32)&evt_door_01_01,
    (u32)&evt_wall_01, 0x00000000, 0x00000000, (u32)&door_data_01_item,
    (u32)&evt_door_01_open, (u32)&evt_door_01_close,
};

// TODO: type
u32 door_data_01_01[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_01_doa",
    (u32)"A_ie_01_doa02", (u32)"A_ie_01_doa01", (u32)"S_ie_01_soto", (u32)"A_ie_01_A",
    (u32)"S_ie_01_naka", (u32)"A_ie_01_C", (u32)&evt_door_01_00, (u32)&evt_door_01_01,
    (u32)&evt_wall_01, (u32)&door_data_01_npc, 0x00000000, (u32)&door_data_01_item,
    (u32)&evt_door_01_open, (u32)&evt_door_01_close,
};

EVT_BEGIN(evt_door_02_00_f1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f1_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_f1)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    SET_FLOAT_VALUE(LW(4), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(1.0f))
    MULTIPLY_FLOAT(LW(4), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f1B"), 0, 0, LW(2))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2uB"), 0, 0, LW(3))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2sB"), 0, 0, LW(4))
    RETURN
EVT_END()

static char* door_data_02_npc[3] = {
    "店長", "店員", NULL,
};

char* door_data_02_npc_01[2] = {
    "店長", NULL,
};

EVT_BEGIN(evt_door_02f1_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_02f1_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door_data_02_f1[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_02_f1_doa",
    (u32)"A_ie_02_f1_doa02", (u32)"A_ie_02_f1_doa01", (u32)"S_ie_02_f1_soto", 0x00000000,
    (u32)"S_ie_02_f1_naka", (u32)"A_ie_02_f1_C", (u32)&evt_door_02_00_f1, 0x00000000,
    (u32)&evt_wall_02_f1, (u32)&door_data_02_npc, 0x00000000, 0x00000000,
    (u32)&evt_door_02f1_open, (u32)&evt_door_02f1_close,
};

// TODO: type
u32 door_data_02_f1_01[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_02_f1_doa",
    (u32)"A_ie_02_f1_doa02", (u32)"A_ie_02_f1_doa01", (u32)"S_ie_02_f1_soto", 0x00000000,
    (u32)"S_ie_02_f1_naka", (u32)"A_ie_02_f1_C", (u32)&evt_door_02_00_f1, 0x00000000,
    (u32)&evt_wall_02_f1, (u32)&door_data_02_npc_01, 0x00000000, 0x00000000,
    (u32)&evt_door_02f1_open, (u32)&evt_door_02f1_close,
};

EVT_BEGIN(evt_door_02_f2_junbityu)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("gor_01_024_00_00"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_02_00_f2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2_doaD"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_f2)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    SET_FLOAT_VALUE(LW(4), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(1.0f))
    MULTIPLY_FLOAT(LW(4), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f1B"), 0, 0, LW(2))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2uB"), 0, 0, LW(3))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_02_f2sB"), 0, 0, LW(4))
    RETURN
EVT_END()

char* badgemise_npc[3] = {
    "バッジ店長", "バッジ店員", NULL,
};

static USER_FUNC(disp_off) {
    UnitWinWork* p = (UnitWinWork*)evtGetValue(event, event->args[0]);

    p->flags &= ~1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(get_value) {
    s32* args = event->args;
    UnitWinWork* work = (UnitWinWork*)evtGetValue(event, args[0]);

    evtSetValue(event, args[1], (s32)itemDataTable[work->items[work->index]].id);
    evtSetValue(event, args[2], work->items[work->index]);
    evtSetValue(event, args[3], itemDataTable[work->items[work->index]].discount_price);
    evtSetValue(event, args[4], itemDataTable[work->items[work->index]].bp_cost);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_badge_dummy)
    CALL(evt_mario_normalize)
    SET_VALUE(LW(10), LW(0))
    CALL(disp_off, LW(10))
    CALL(evt_set_dir_to_target, STRING("バッジ店員"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("バッジ店員"))
    CALL(evt_win_coin_on, 0, LW(8))
    CALL(get_value, LW(10), LW(11), LW(12), LW(13), LW(14))
    CALL(evt_sub_get_language, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_insert, 0, STRING("badgeshop_00"), 0, STRING("バッジ店員"), LW(13), LW(11), LW(14))
    ELSE
        CALL(evt_msg_print_insert, 0, STRING("badgeshop_00"), 0, STRING("バッジ店員"), LW(11), LW(13), LW(14))
    END_IF
    CALL(evt_msg_select, 0, STRING("badgeshop_24"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_22"))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_LESS(LW(0), LW(13))
        CALL(evt_msg_print_add, 0, STRING("badgeshop_01"))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(evt_pouch_add_item, LW(12), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(evt_msg_print_add, 0, STRING("badgeshop_02"))
        CALL(evt_win_coin_off, LW(8))
        RETURN
    END_IF
    CALL(set_new_goods_list, LW(10), 0)
    CALL(evt_badgeShop_special_dec, LW(12))
    CALL(unitwin_set_taken, LW(10), LW(12))
    MULTIPLY(LW(13), -1)
    CALL(evt_pouch_add_coin, LW(13))
    CALL(evt_win_coin_wait, LW(8))
    WAIT_MS(200)
    CALL(evt_win_coin_off, LW(8))
    CALL(evt_msg_continue)
    RETURN
EVT_END()

// TODO: type
u32 uwnobj_list[18] = {
    (u32)"S_item_07", (u32)"A_item_08", (u32)&evt_badge_dummy, (u32)"S_item_08",
    (u32)"A_item_09", (u32)&evt_badge_dummy, (u32)"S_item_09", (u32)"A_item_10",
    (u32)&evt_badge_dummy, (u32)"S_item_10", (u32)"A_item_11", (u32)&evt_badge_dummy,
    (u32)"S_item_11", (u32)"A_item_12", (u32)&evt_badge_dummy, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(evt_open_02_01_f2)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_unitwin_alloc, 1)
    CALL(badgeShop_getBargainTable, LW(0))
    CALL(evt_unitwin_selltable_setup, PTR(uwnobj_list), LW(0))
    RETURN
EVT_END()

static USER_FUNC(mario_keyoff_wait) {
    switch (!marioChkKey()) {
    case 0:
        break;
    default:
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_YIELD;
}

EVT_BEGIN(evt_close_sub)
    CALL(mario_keyoff_wait)
    CALL(evt_unitwin_free)
    RETURN
EVT_END()

EVT_BEGIN(evt_close_02_01_f2)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    SPAWN_EVENT(evt_close_sub)
    RETURN
EVT_END()

// TODO: type
u32 door_data_02_f2[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_02_f2_doaD",
    (u32)"A_ie_02_f2_doa02", (u32)"A_ie_02_f2_doa01", (u32)"S_ie_02_f2_soto", 0x00000000,
    (u32)"S_ie_02_f2_naka", (u32)"A_ie_02_f2_C", (u32)&evt_door_02_00_f2, 0x00000000,
    (u32)&evt_wall_02_f2, (u32)&badgemise_npc, 0x00000000, 0x00000000,
    (u32)&evt_open_02_01_f2, (u32)&evt_close_02_01_f2,
};

EVT_BEGIN(house03_f1f2_fade)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(1), 100)
            SET_VALUE(GW(10), 2)
        ELSE
            SET_VALUE(GW(10), 1)
        END_IF
        IF_INT_EQUAL(GW(10), 1)
            SET_VALUE(GW(14), 1)
            SET_VALUE(GW(15), -1)
        ELSE
            SET_VALUE(GW(14), -1)
            SET_VALUE(GW(15), 1)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(luigi_f1_fade_check)
    SWITCH(GSW(0))
        CASE_BETWEEN(112, 163)
        CASE_BETWEEN(215, 260)
        CALL(evt_npc_set_color, STRING("ルイージ"), LW(0), LW(0), LW(0), 255)
        CALL(evt_npc_set_color, STRING("トルク"), LW(0), LW(0), LW(0), 255)
        CASE_GREATER_OR_EQUAL(373)
    END_SWITCH
    RETURN
EVT_END()

USER_FUNC(house03_f1_fade_func) {
    s32 value = evtGetValue(event, event->args[0]);

    mapGrpSetColor("S_i03_f1_naka", (GXColor){value, value, value, 0xFF});
    return EVT_RETURN_DONE;
}


EVT_BEGIN(house03_f1_fade)
    LOOP(0)
        IF_INT_EQUAL(GW(14), 1)
            ADD(LW(0), 10)
            IF_INT_GREATER_OR_EQUAL(LW(0), 255)
                SET_VALUE(LW(0), 255)
            END_IF
        END_IF
        IF_INT_EQUAL(GW(14), -1)
            ADD(LW(0), -10)
            IF_INT_LESS_OR_EQUAL(LW(0), 0)
                SET_VALUE(LW(0), 0)
            END_IF
        END_IF
        CALL(house03_f1_fade_func, LW(0))
        CALL(evt_npc_set_color, STRING("水夫ボム兵"), LW(0), LW(0), LW(0), 255)
        CALL(evt_npc_set_color, STRING("チューさん２"), LW(0), LW(0), LW(0), 255)
        CALL_CHILD(luigi_f1_fade_check)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(luigi_f2_fade_check)
    SWITCH(GSW(0))
        CASE_BETWEEN(112, 163)
        CASE_BETWEEN(215, 260)
        CASE_GREATER_OR_EQUAL(373)
        CALL(evt_npc_set_color, STRING("ルイージ"), LW(0), LW(0), LW(0), 255)
        CALL(evt_npc_set_color, STRING("キック２"), LW(0), LW(0), LW(0), 255)
    END_SWITCH
    RETURN
EVT_END()
USER_FUNC(house03_f2_fade_func) {
    u8 value = evtGetValue(event, event->args[0]);

    mapGrpSetColor("S_i03_f2_naka", (GXColor){value, value, value, 0xFF});
    mapGrpSetColor("S_ie_03_f21_doa", (GXColor){value, value, value, 0xFF});
    return EVT_RETURN_DONE;
}



EVT_BEGIN(house03_f2_fade)
    LOOP(0)
        IF_INT_EQUAL(GW(15), 1)
            ADD(LW(0), 10)
            IF_INT_GREATER_OR_EQUAL(LW(0), 255)
                SET_VALUE(LW(0), 255)
            END_IF
        END_IF
        IF_INT_EQUAL(GW(15), -1)
            ADD(LW(0), -10)
            IF_INT_LESS_OR_EQUAL(LW(0), 0)
                SET_VALUE(LW(0), 0)
            END_IF
        END_IF
        CALL(house03_f2_fade_func, LW(0))
        CALL(evt_npc_set_color, STRING("宿店員"), LW(0), LW(0), LW(0), 255)
        CALL(evt_npc_set_color, STRING("キノじい"), LW(0), LW(0), LW(0), 255)
        CALL_CHILD(luigi_f2_fade_check)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(house03_suiteki)
    LOOP(0)
        CALL(evt_map_playanim, STRING("sizuku"), 0, 0)
        CALL(evt_map_checkanim, STRING("sizuku"), LW(0), LW(1))
        WAIT_MS(LW(1))
        CALL(evt_mapobj_get_position, STRING("S_i03_f2_mizu"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_AMB_WATERDROP2"), LW(0), 300, LW(2), LW(3))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER(LW(1), 100)
            CALL(evt_snd_sfx_vol, LW(3), 127)
        ELSE
            CALL(evt_snd_sfx_vol, LW(3), 30)
        END_IF
    END_LOOP
    RETURN
EVT_END()
USER_FUNC(chk_marco_fuki) {
    s32* args = event->args;
    NpcEntry* npc;

    marioGetPtr();
    npc = fbatNpcTalkCheck();
    if (npc != NULL && strcmp(npc->description, "マルコ") == 0) {
        evtSetValue(event, args[0], 1);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, args[0], 0);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(marco_hitorigoto)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 223)
        RETURN
    END_IF
    CALL(evt_mario_wait_movable)
    SET_VALUE(LSWF(2), 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(chk_marco_fuki, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                IF_INT_EQUAL(LSWF(2), 1)
                    CALL(evt_msg_close, STRING("mac_4_004"))
                END_IF
                SET_VALUE(LSWF(2), 0)
            ELSE
                SET_VALUE(LSWF(2), 1)
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(LSWF(2), 1)
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LSWF(2), 1)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_print, 0, STRING("mac_4_004"), 0, STRING("マルコ"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_door_03_00_f1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f1_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_03_01_f1)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f1_doa"), LW(0), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(house_03_f1_open)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_f21_doa"), 6144)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_i03_f2_soto"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_door_end_wait)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_i03_f2_soto"), 1)
    END_INLINE_EVENT
    SET_VALUE(GW(10), 1)
    SET_VALUE(GW(14), 0)
    SET_VALUE(GW(15), 0)
    SPAWN_EVENT_GET_ID(PTR(house03_f1f2_fade), GW(11))
    SET_VALUE(LW(0), 255)
    SPAWN_EVENT_GET_ID(PTR(house03_f1_fade), GW(12))
    SET_VALUE(LW(0), 0)
    SPAWN_EVENT_GET_ID(PTR(house03_f2_fade), GW(13))
    SPAWN_EVENT_GET_ID(PTR(marco_hitorigoto), GW(0))
    SPAWN_EVENT_GET_ID(PTR(house03_suiteki), GW(7))
    RETURN
EVT_END()

EVT_BEGIN(house_03_f1_close)
    BEGIN_INLINE_EVENT
        WAIT_MS(1600)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_f1_doa"), 6144)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_f21_doa"), 6144)
        CALL(evt_mapobj_color, 1, STRING("S_ie_03_f21_doa"), 255, 255, 255, 255)
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    SET_VALUE(GW(10), 0)
    DELETE_EVENT(GW(0))
    CALL(evt_msg_close, STRING("mac_4_004"))
    DELETE_EVENT(GW(7))
    DELETE_EVENT(GW(11))
    DELETE_EVENT(GW(12))
    DELETE_EVENT(GW(13))
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_03_f1)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    SET_FLOAT_VALUE(LW(4), LW(0))
    SET_FLOAT_VALUE(LW(5), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(0.5f))
    MULTIPLY_FLOAT(LW(4), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(5), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f1B"), LW(2), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_i03_f2_sitaB"), LW(3), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_i03_f2_ueB"), LW(4), 0, 0)
    RETURN
EVT_END()

static char* door_data_03_npc[10] = {
    "マスター", "キノじい", "宿店員", "チューさん２", "水夫ボム兵", "マルコ", "ルイージ", "トルク", "キック２", NULL,
};

char* door_data_03_item[2] = {
    "ドクロジュエル", NULL,
};

// TODO: type
u32 door_data_03_f1[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_03_f1_doa",
    (u32)"A_ie_03_f1_doa02", (u32)"A_ie_03_f1_doa01", (u32)"S_ie_03_soto", 0x00000000,
    (u32)"S_ie_03_naka", 0x00000000, (u32)&evt_door_03_00_f1, (u32)&evt_door_03_01_f1,
    (u32)&evt_wall_03_f1, (u32)&door_data_03_npc, 0x00000000, (u32)&door_data_03_item,
    (u32)&house_03_f1_open, (u32)&house_03_f1_close,
};

EVT_BEGIN(evt_door_03_00_f2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f21_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(house_03_f2_open)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_f1_doa"), 6144)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    SET_VALUE(GW(10), 2)
    SET_VALUE(GW(14), 0)
    SET_VALUE(GW(15), 0)
    SPAWN_EVENT_GET_ID(PTR(house03_f1f2_fade), GW(11))
    SET_VALUE(LW(0), 0)
    SPAWN_EVENT_GET_ID(PTR(house03_f1_fade), GW(12))
    SET_VALUE(LW(0), 255)
    SPAWN_EVENT_GET_ID(PTR(house03_f2_fade), GW(13))
    SPAWN_EVENT_GET_ID(PTR(marco_hitorigoto), GW(0))
    SPAWN_EVENT_GET_ID(PTR(house03_suiteki), GW(7))
    RETURN
EVT_END()

EVT_BEGIN(house_03_f2_close)
    BEGIN_INLINE_EVENT
        WAIT_MS(1600)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_f1_doa"), 6144)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_f21_doa"), 6144)
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    SET_VALUE(GW(10), 0)
    DELETE_EVENT(GW(0))
    CALL(evt_msg_close, STRING("mac_4_004"))
    DELETE_EVENT(GW(7))
    DELETE_EVENT(GW(11))
    DELETE_EVENT(GW(12))
    DELETE_EVENT(GW(13))
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_03_f2)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    SET_FLOAT_VALUE(LW(4), LW(0))
    SET_FLOAT_VALUE(LW(5), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(0.5f))
    MULTIPLY_FLOAT(LW(4), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(5), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f1B"), LW(2), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_i03_f2_sitaB"), LW(3), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_i03_f2_ueB"), LW(4), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f1_doa"), LW(5), 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door_data_03_f2[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_03_f21_doa",
    (u32)"S_ie_03_f2_doa02", (u32)"S_ie_03_f2_doa01", (u32)"S_ie_03_soto", 0x00000000,
    (u32)"S_ie_03_naka", 0x00000000, (u32)&evt_door_03_00_f2, 0x00000000,
    (u32)&evt_wall_03_f2, (u32)&door_data_03_npc, 0x00000000, (u32)&door_data_03_item,
    (u32)&house_03_f2_open, (u32)&house_03_f2_close,
};

static EVT_BEGIN(evt_door_04_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_01)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04_doa"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_04)
    SET_FLOAT_VALUE(LW(2), LW(0))
    SET_FLOAT_VALUE(LW(3), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(1.0f))
    MULTIPLY_FLOAT(LW(3), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04uB"), LW(2), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_ie_04sB"), LW(3), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_goki_01"), 6144)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    IF_INT_EQUAL(GSWF(1198), 1)
        SET_VALUE(GSWF(1199), 1)
    END_IF
    RETURN
EVT_END()

static char* door_data_04_npc[2] = {
    "盗賊", NULL,
};

static char* door_data_04_item[2] = {
    "item_01", NULL,
};

// TODO: type
static u32 door_data_04[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"S_ie_04_doa",
    (u32)"A_ie_04_doa02", (u32)"A_ie_04_doa01", (u32)"S_ie_04_soto", 0x00000000,
    (u32)"S_ie_04_naka", (u32)"A_ie_04_C", (u32)&evt_door_04_00, (u32)&evt_door_04_01,
    (u32)&evt_wall_04, (u32)&door_data_04_npc, 0x00000000, (u32)&door_data_04_item,
    (u32)&evt_door_04_open, (u32)&evt_door_04_close,
};

static EVT_BEGIN(koushukei_dai)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, 10, -10, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 0, STRING("S_rop"), LW(0), 0, 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_FRAMES(1)
        CALL(evt_sub_intpl_msec_init, 11, -10, 10, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 0, STRING("S_rop"), LW(0), 0, 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static char* obj_list[12] = {
    "S_item_01", "A_item_01", "S_item_02", "A_item_02", "S_item_03", "A_item_03", "S_item_04",
    "A_item_04", "S_item_05", "A_item_05", "S_item_06", "A_item_06",
};

// TODO: type
static u32 goods_list[12] = {
    0x00000094, 0x00000005, 0x0000009A, 0x00000005,
    0x00000099, 0x00000003, 0x00000084, 0x0000000A,
    0x0000008A, 0x00000008, 0x000000A2, 0x00000005,
};

// TODO: type
u32 goods_list2[12] = {
    0x00000033, 0x0000000A, 0x0000009A, 0x00000005,
    0x00000099, 0x00000003, 0x00000084, 0x0000000A,
    0x0000008A, 0x00000008, 0x000000A2, 0x00000005,
};

// TODO: type
u32 goods_luigi1[12] = {
    0x00000037, 0x00000040, 0x0000009A, 0x00000005,
    0x00000099, 0x00000003, 0x00000084, 0x0000000A,
    0x0000008A, 0x00000008, 0x000000A2, 0x00000005,
};

// TODO: type
u32 goods_luigi2[12] = {
    0x00000038, 0x00000080, 0x0000009A, 0x00000005,
    0x00000099, 0x00000003, 0x00000084, 0x0000000A,
    0x0000008A, 0x00000008, 0x000000A2, 0x00000005,
};

// TODO: type
u32 goods_luigi3[12] = {
    0x00000039, 0x00000080, 0x0000009A, 0x00000005,
    0x00000099, 0x00000003, 0x00000084, 0x0000000A,
    0x0000008A, 0x00000008, 0x000000A2, 0x00000005,
};

// TODO: type
u32 goods_luigi4[12] = {
    0x0000003A, 0x00000080, 0x0000009A, 0x00000005,
    0x00000099, 0x00000003, 0x00000084, 0x0000000A,
    0x0000008A, 0x00000008, 0x000000A2, 0x00000005,
};

// TODO: type
u32 goods_luigi5[12] = {
    0x0000003B, 0x00000100, 0x0000009A, 0x00000005,
    0x00000099, 0x00000003, 0x00000084, 0x0000000A,
    0x0000008A, 0x00000008, 0x000000A2, 0x00000005,
};

// TODO: type
static u32 trade_list[38] = {
    0x00000094, 0x00020000, 0x0000009A, 0x00020000,
    0x00000099, 0x00010000, 0x00000084, 0x00050000,
    0x0000008A, 0x00040000, 0x000000A2, 0x00020000,
    0x000000A5, 0x00370000, 0x000000A6, 0x00020000,
    0x000000A7, 0x000C0000, 0x000000A8, 0x00050000,
    0x000000A9, 0x00040000, 0x000000AA, 0x00040000,
    0x000000AB, 0x00350000, 0x000000AC, 0x00080000,
    0x000000AD, 0x000B0000, 0x000000AE, 0x00020000,
    0x000000AF, 0x00020000, 0x000000B0, 0x00040000,
    0x00000000, 0x00000000,
};

static char* shopper_data[38] = {
    "店長", NULL, NULL, "gor_shop1_00", "gor_shop1_01", "gor_shop1_02", "gor_shop1_03",
    "gor_shop1_04", "gor_shop1_05", "gor_shop1_06", "gor_shop1_07", "gor_shop1_08", "gor_shop1_09",
    "gor_shop1_10", "gor_shop1_11", "gor_shop1_12", "gor_shop1_13", "gor_shop1_14", "gor_shop1_15",
    "gor_shop1_16", "gor_shop1_17", "gor_shop1_18", "gor_shop1_19", "gor_shop1_20", "gor_shop1_21",
    "gor_shop1_22", "gor_shop1_23", "gor_shop1_24", "gor_shop1_25", "gor_shop1_26", "gor_shop1_27",
    "gor_shop1_28", "gor_shop1_29", "gor_shop1_30", "gor_shop1_31", "gor_shop1_32", "gor_shop1_33",
    "gor_shop1_34",
};

EVT_BEGIN(gor_01_shop_setup)
    IF_INT_EQUAL(GSWF(1188), 0)
        IF_INT_EQUAL(GSWF(1191), 0)
            CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list), PTR(shopper_data), PTR(trade_list))
        ELSE
            CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list2), PTR(shopper_data), PTR(trade_list))
        END_IF
    ELSE
        CALL(evt_pouch_check_item, 55, LW(1))
        CALL(evt_pouch_check_item, 56, LW(2))
        CALL(evt_pouch_check_item, 57, LW(3))
        CALL(evt_pouch_check_item, 58, LW(4))
        CALL(evt_pouch_check_item, 59, LW(5))
        SWITCH(GSW(0))
            CASE_LESS(215)
            SET_VALUE(LW(11), 0)
            CASE_LESS(261)
            SET_VALUE(LW(11), 1)
            CASE_LESS(339)
            SET_VALUE(LW(11), 2)
            CASE_LESS(373)
            SET_VALUE(LW(11), 3)
            CASE_LESS(401)
            SET_VALUE(LW(11), 4)
            CASE_DEFAULT
            SET_VALUE(LW(11), 5)
        END_SWITCH
        SET_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(1), 0)
            ADD(LW(12), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(2), 0)
            ADD(LW(12), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(3), 0)
            ADD(LW(12), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(4), 0)
            ADD(LW(12), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(5), 0)
            ADD(LW(12), 1)
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(12), LW(11))
            SET_VALUE(LW(10), PTR(goods_list))
        ELSE
            SWITCH(LW(12))
                CASE_EQUAL(0)
                SET_VALUE(LW(10), PTR(goods_luigi1))
                CASE_EQUAL(1)
                SET_VALUE(LW(10), PTR(goods_luigi2))
                CASE_EQUAL(2)
                SET_VALUE(LW(10), PTR(goods_luigi3))
                CASE_EQUAL(3)
                SET_VALUE(LW(10), PTR(goods_luigi4))
                CASE_EQUAL(4)
                SET_VALUE(LW(10), PTR(goods_luigi5))
            END_SWITCH
        END_IF
        CALL(evt_shop_setup, PTR(obj_list), LW(10), PTR(shopper_data), PTR(trade_list))
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 kino_dt[38] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000005, 0x43B80000, 0x432A0000, 0xC3938000,
    0x43A50000, 0x431A0000, 0xC37A0000, 0x43020000,
    0x43B90000, 0xC29E0000, 0x439C8000, 0x435C0000,
    0xC3840000, 0xC1F00000, 0x43630000, 0x431A0000,
    0xC3480000, (u32)"宿店員", (u32)"yado_00", (u32)"yado_01",
    (u32)"yado_02", (u32)"yado_03", (u32)"yado_04", (u32)"yado_05",
    (u32)"yado_06", (u32)"yado_07", (u32)"yado_08", (u32)"yado_09",
    (u32)"yado_10", (u32)"yado_11", (u32)"yado_12", (u32)"yado_13",
    0x00000000, 0x00000000,
};

EVT_BEGIN(bar_door_anime)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_03_naka"), 1)
    CALL(evt_mapobj_get_position, STRING("S_ie_03_f1_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 108, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f1_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 11, 108, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_ie_03_f1_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_get_position, STRING("S_ie_03_f1_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_naka"), 1)
    SET_VALUE(GF(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(christine_nakama_init)
    CALL(evt_npc_entry, STRING("マフィア１"), STRING("c_m_buka"))
    CALL(evt_npc_set_tribe, STRING("マフィア１"), STRING("マフィア"))
    CALL(evt_npc_entry, STRING("マフィア２"), STRING("c_m_buka"))
    CALL(evt_npc_set_tribe, STRING("マフィア２"), STRING("マフィア"))
    CALL(evt_npc_entry, STRING("盗賊団１"), STRING("c_monban"))
    CALL(evt_npc_set_tribe, STRING("盗賊団１"), STRING("盗賊団"))
    CALL(evt_npc_entry, STRING("盗賊団２"), STRING("c_monban"))
    CALL(evt_npc_set_tribe, STRING("盗賊団２"), STRING("盗賊団"))
    CALL(evt_npc_set_position, STRING("マフィア１"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("マフィア２"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("盗賊団１"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("盗賊団２"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(christine_nakama)
    SET_VALUE(GF(10), 1)
    SPAWN_EVENT(maffia_assault)
    CALL(evt_mario_clear_party, 1)
    CALL(evt_cam3d_evt_set, 0, 134, 984, 0, 38, 442, 0, 11)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_pouch_party_join, 1)
        CALL(evt_mario_set_party_pos, 0, 1, 25, 0, 625)
        CALL(evt_party_set_dir, 0, 270, 0)
        CALL(evt_party_move_pos2, 0, 25, 500, FLOAT(150.0f))
        WAIT_FRAMES(1)
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_dispflag_onoff, 1, 4)
    CALL(evt_mario_set_pos, -25, 0, 625)
    CALL(evt_mario_mov_pos2, -25, 500, FLOAT(150.0f))
    CALL(evt_mario_adjust_dir)
    CALL(evt_mario_dispflag_onoff, 0, 4)
    CALL(evt_npc_set_position, STRING("キノじい"), 160, 0, 500)
    CALL(evt_npc_set_ry, STRING("キノじい"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("キノじい"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("キノじい"), 1073741856)
    BEGIN_INLINE_EVENT
        WAIT_MS(60)
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_034"))
    CALL(evt_cam3d_evt_set, 0, 55, 800, 0, 45, 440, 400, 11)
    WAIT_MS(400)
    CALL(evt_msg_print_party, STRING("mac_0_034_00"))
    CALL(evt_party_move_pos2, 0, -8, 495, FLOAT(120.0f))
    WAIT_MS(100)
    CALL(evt_party_set_pose, 0, STRING("PKR_A_2A"))
    CALL(evt_snd_sfxon, STRING("SFX_STG0_KURI_KISS1"), 0)
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 15)
    ADD(LW(1), 35)
    ADD(LW(2), 10)
    CALL(evt_snd_sfxon, STRING("SFX_STG0_KURI_KISS2"), 0)
    CALL(evt_eff, 0, STRING("kiss"), 1, LW(0), LW(1), LW(2), 45, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_party_move_pos2, 0, 25, 500, FLOAT(120.0f))
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("mac_0_035"))
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("mac_0_036"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("mac_0_037"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    SET_VALUE(GF(10), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_038"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_0_039"))
    CALL(evt_mario_set_pose, STRING("M_C_3"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    ADD(LW(1), 20)
    ADD(LW(2), 10)
    CALL(evt_item_entry, STRING("item00"), 96, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_item_set_scale, STRING("item00"), FLOAT(0.55f))
    WAIT_MS(500)
    CALL(evt_party_move_pos2, 0, 0, 500, FLOAT(120.0f))
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("mac_0_040"))
    CALL(evt_item_delete, STRING("item00"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("mac_0_041"))
    CALL(evt_mario_set_dir, 90, 200, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_party_set_dir, 0, 90, 200)
        WAIT_MS(500)
        CALL(evt_party_move_pos2, 0, 25, 500, FLOAT(120.0f))
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_0_042"), 0, STRING("キノじい"))
    SET_VALUE(GF(10), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 25, 55, 800, 25, 45, 440, 800, 11)
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("キノじい"), 75, 500, 0, FLOAT(55.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_0_043"), 0, STRING("キノじい"))
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_044"), 0, STRING("キノじい"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_045"), 0, STRING("キノじい"))
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 200, 1, -1000)
    WAIT_MS(500)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_WHILE(GF(11))
    CALL(evt_msg_print, 0, STRING("mac_0_046"), 0, STRING("キノじい"))
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_047"), 0, STRING("キノじい"))
    CALL(evt_mario_set_pose, STRING("M_I_G"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_047_01"), 0, STRING("キノじい"))
    CALL(evt_npc_flag_onoff, 0, STRING("キノじい"), 512)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("キノじい"), 93, -46, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_set_ry, STRING("キノじい"), 90)
        WAIT_MS(500)
        SPAWN_EVENT(bar_door_anime)
        CALL(evt_npc_move_position, STRING("キノじい"), 74, -25, 0, FLOAT(180.0f), 0)
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("キノじい"), 105, -95, 0, FLOAT(180.0f), 0)
        WAIT_WHILE(GF(0))
        CALL(evt_npc_set_position, STRING("キノじい"), 320, 153, -200)
        CALL(evt_npc_set_ry, STRING("キノじい"), 270)
        CALL(evt_npc_flag_onoff, 0, STRING("キノじい"), 1073741856)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_get_position, STRING("キノじい"), LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_0_047_02"), 0, STRING("キノじい"))
        WAIT_MS(500)
        CALL(evt_npc_get_position, STRING("キノじい"), LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("mac_0_047_02"), 0, STRING("キノじい"))
        WAIT_MS(1000)
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 0, 55, 800, 0, 45, 440, 500, 11)
    WAIT_MS(1000)
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("mac_0_048"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("mac_0_049"))
    CALL(evt_mario_set_pose, STRING("M_I_Z"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("mac_0_050"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("mac_0_051"))
    CALL(evt_cam3d_evt_set, 27, 45, 800, 27, 35, 440, 800, 11)
    WAIT_MS(800)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 1)
    CALL_CHILD(all_party_lecture_10_data_4B0DC)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR1"))
    CALL(evt_msg_print_party, STRING("mac_0_053"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_dispflag_onoff, 0, 256)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 7)
    SPAWN_EVENT(christine_nakama_after)
    RETURN
EVT_END()
USER_FUNC(maffia_kemuri) {
    s32* args = event->args;
    f32 x = evtGetValue(event, args[0]);
    f32 y = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);

    effKemuriEntry(1, x, y, z, 0.0f);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(maffia_turn)
    CALL(evt_snd_sfxon, STRING("SFX_STG0_THIEF_FIGHT1"), 0)
    CALL(evt_npc_get_position, STRING("盗賊団１"), LW(0), LW(1), LW(2))
    CALL(maffia_kemuri, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("盗賊団２"), LW(0), LW(1), LW(2))
    CALL(maffia_kemuri, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_STG0_THIEF_FIGHT2"), 0)
    CALL(evt_sub_intpl_msec_init, 0, 0, 720, 360)
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(10), 360)
            SUBTRACT(LW(10), 360)
        END_IF
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_rotate, STRING("盗賊団１"), 0, LW(10), 0)
        CALL(evt_npc_set_rotate, STRING("盗賊団２"), 0, LW(10), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_rotate, STRING("盗賊団１"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("盗賊団２"), 0, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(maffia_assault)
    SET_VALUE(GF(11), 1)
    CALL(evt_npc_set_position, STRING("マフィア１"), 75, 0, -400)
    CALL(evt_npc_set_ry, STRING("マフィア１"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア１"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア１"), 1073741856)
    CALL(evt_npc_set_position, STRING("マフィア２"), 95, 0, -390)
    CALL(evt_npc_set_ry, STRING("マフィア２"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア２"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア２"), 1073741856)
    WAIT_WHILE(GF(10))
    SET_VALUE(GF(10), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("マフィア２"), 45, -390, 0, FLOAT(300.0f), 0)
        WAIT_MS(1300)
        CALL(evt_npc_move_position, STRING("マフィア２"), 25, -390, 0, FLOAT(300.0f), 0)
        CALL(evt_npc_set_ry, STRING("マフィア２"), 90)
        CALL(evt_npc_move_position, STRING("マフィア２"), 25, -60, 0, FLOAT(300.0f), 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("マフィア１"), 25, -400, 0, FLOAT(300.0f), 0)
    WAIT_MS(1500)
    CALL(evt_npc_move_position, STRING("マフィア１"), 5, -390, 0, FLOAT(300.0f), 0)
    CALL(evt_npc_set_ry, STRING("マフィア１"), 90)
    CALL(evt_npc_move_position, STRING("マフィア１"), 5, -60, 0, FLOAT(300.0f), 0)
    WAIT_WHILE(GF(10))
    SET_VALUE(GF(10), 1)
    CALL(evt_npc_set_position, STRING("盗賊団１"), 225, 0, 10)
    CALL(evt_npc_set_ry, STRING("盗賊団１"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊団１"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊団１"), 1073741856)
    CALL(evt_npc_set_position, STRING("盗賊団２"), 255, 0, 30)
    CALL(evt_npc_set_ry, STRING("盗賊団２"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊団２"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("盗賊団２"), 1073741856)
    SET_VALUE(GF(12), 1)
    SET_VALUE(GF(13), 1)
    SET_VALUE(GF(14), 1)
    SET_VALUE(GF(15), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("盗賊団１"), 125, 10, 0, FLOAT(100.0f), 0)
        SET_VALUE(GF(12), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("盗賊団２"), 165, 30, 0, FLOAT(100.0f), 0)
        SET_VALUE(GF(13), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("マフィア１"), 5, 15, 0, FLOAT(360.0f), 0)
        CALL(evt_npc_move_position, STRING("マフィア１"), 90, 15, 0, FLOAT(360.0f), 0)
        SET_VALUE(GF(14), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("マフィア２"), 25, 30, 0, FLOAT(360.0f), 0)
        CALL(evt_npc_move_position, STRING("マフィア２"), 200, 35, 0, FLOAT(360.0f), 0)
        CALL(evt_npc_set_ry, STRING("マフィア２"), 270)
        SET_VALUE(GF(15), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(12))
    WAIT_WHILE(GF(13))
    WAIT_WHILE(GF(14))
    WAIT_WHILE(GF(15))
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア１"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("マフィア２"), 1024)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("マフィア２"), STRING("A_1"))
        CALL(evt_npc_move_position, STRING("マフィア２"), 180, 15, 0, FLOAT(360.0f), 0)
        CALL(evt_npc_move_position, STRING("マフィア２"), 200, 15, 0, FLOAT(360.0f), 0)
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("マフィア２"), 180, 15, 0, FLOAT(360.0f), 0)
        CALL(evt_npc_move_position, STRING("マフィア２"), 200, 15, 0, FLOAT(360.0f), 0)
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("マフィア２"), 180, 15, 0, FLOAT(360.0f), 0)
        CALL(evt_npc_move_position, STRING("マフィア２"), 200, 15, 0, FLOAT(360.0f), 0)
        CALL(evt_npc_set_anim, STRING("マフィア２"), STRING("S_1"))
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("マフィア１"), STRING("A_1"))
    CALL(evt_npc_move_position, STRING("マフィア１"), 110, 15, 0, FLOAT(360.0f), 0)
    CALL(evt_npc_set_anim, STRING("盗賊団１"), STRING("D_1"))
    CALL(evt_npc_set_anim, STRING("盗賊団２"), STRING("D_1"))
    SPAWN_EVENT(maffia_turn)
    CALL(evt_npc_move_position, STRING("マフィア１"), 90, 15, 0, FLOAT(360.0f), 0)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("マフィア１"), 110, 15, 0, FLOAT(360.0f), 0)
    SPAWN_EVENT(maffia_turn)
    CALL(evt_npc_move_position, STRING("マフィア１"), 90, 15, 0, FLOAT(360.0f), 0)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("マフィア１"), 110, 15, 0, FLOAT(360.0f), 0)
    SPAWN_EVENT(maffia_turn)
    CALL(evt_npc_move_position, STRING("マフィア１"), 90, 15, 0, FLOAT(360.0f), 0)
    CALL(evt_npc_set_anim, STRING("マフィア１"), STRING("S_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_054"), 0, STRING("盗賊団１"))
    CALL(evt_npc_set_anim, STRING("盗賊団１"), STRING("K_1"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("盗賊団１"), STRING("Y_1"))
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア１"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("マフィア２"), 1024)
    CALL(evt_npc_set_rotate_offset, STRING("盗賊団１"), 0, 0, 0)
    CALL(evt_sub_intpl_msec_init, 0, 0, 90, 150)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_rotate, STRING("盗賊団１"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxon, STRING("SFX_STG0_THIEF_DOWN1"), 0)
    SET_VALUE(LW(10), STRING("盗賊団２"))
    CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(0))
    ADD(LW(3), -20)
    CALL(evt_npc_move_position, LW(10), LW(3), LW(2), 0, FLOAT(200.0f), 0)
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("盗賊団２"), STRING("D_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_055"), 0, STRING("盗賊団２"))
    CALL(evt_npc_set_anim, STRING("盗賊団２"), STRING("S_1"))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(10), STRING("マフィア１"))
        CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
        SET_VALUE(LW(3), LW(0))
        ADD(LW(3), -20)
        CALL(evt_npc_move_position, LW(10), LW(3), LW(2), 0, FLOAT(200.0f), 0)
    END_INLINE_EVENT
    SET_VALUE(LW(10), STRING("マフィア２"))
    CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(0))
    ADD(LW(3), -95)
    CALL(evt_npc_set_ry, LW(10), 90)
    CALL(evt_npc_move_position, LW(10), LW(3), LW(2), 0, FLOAT(200.0f), 0)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_056"), 0, STRING("マフィア１"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_057"), 0, STRING("マフィア２"))
    SET_VALUE(GF(14), 1)
    SET_VALUE(GF(15), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(10), STRING("マフィア１"))
        CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
        SET_VALUE(LW(3), LW(0))
        ADD(LW(3), -300)
        CALL(evt_npc_set_ry, LW(10), 270)
        CALL(evt_npc_move_position, LW(10), LW(3), LW(2), 0, FLOAT(360.0f), 0)
        CALL(evt_npc_set_position, LW(10), 0, -1000, 0)
        SET_VALUE(GF(14), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(10), STRING("マフィア２"))
        CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
        SET_VALUE(LW(3), LW(0))
        ADD(LW(3), -300)
        CALL(evt_npc_set_ry, LW(10), 270)
        CALL(evt_npc_move_position, LW(10), LW(3), LW(2), 0, FLOAT(360.0f), 0)
        CALL(evt_npc_set_position, LW(10), 0, -1000, 0)
        SET_VALUE(GF(15), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(14))
    WAIT_WHILE(GF(15))
    WAIT_MS(1000)
    CALL(evt_npc_set_anim, STRING("盗賊団２"), STRING("D_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_058"), 0, STRING("盗賊団２"))
    CALL(evt_npc_set_anim, STRING("盗賊団２"), STRING("S_1"))
    CALL(evt_sub_intpl_msec_init, 0, 90, 0, 150)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_rotate, STRING("盗賊団１"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_anim, STRING("盗賊団１"), STRING("K_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_059"), 0, STRING("盗賊団１"))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("盗賊団２"), STRING("D_1"))
    CALL(evt_msg_print, 0, STRING("mac_0_060"), 0, STRING("盗賊団２"))
    CALL(evt_npc_set_anim, STRING("盗賊団２"), STRING("S_1"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_061"), 0, STRING("盗賊団１"))
    WAIT_MS(200)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(10), STRING("盗賊団１"))
        CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
        SET_VALUE(LW(3), LW(0))
        ADD(LW(3), 300)
        CALL(evt_npc_set_ry, LW(10), 90)
        CALL(evt_npc_move_position, LW(10), LW(3), LW(2), 0, FLOAT(120.0f), 0)
        CALL(evt_npc_set_position, LW(10), 0, -1000, 0)
    END_INLINE_EVENT
    SET_VALUE(LW(10), STRING("盗賊団２"))
    CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(0))
    ADD(LW(3), 300)
    CALL(evt_npc_set_ry, LW(10), 90)
    CALL(evt_npc_move_position, LW(10), LW(3), LW(2), 0, FLOAT(120.0f), 0)
    CALL(evt_npc_set_position, LW(10), 0, -1000, 0)
    SET_VALUE(GF(11), 0)
    RETURN
EVT_END()

EVT_BEGIN(christine_nakama_after)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 32)
        RETURN
    END_IF
    LOOP(0)
        IF_INT_EQUAL(GSWF(236), 1)
            RETURN
        END_IF
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), 440)
            IF_INT_GREATER_OR_EQUAL(LW(2), -10)
                IF_INT_LESS_OR_EQUAL(LW(2), 190)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_0_061_01"), 0, STRING("party"))
    SET_VALUE(GSWF(1200), 1)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(box_copy)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("ダミー"), LW(0), LW(1), LW(2))
        CALL(evt_mobj_set_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(ultrahammer_check)
    CALL(evt_npc_entry, STRING("ダミー"), STRING("c_kinopio"))
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ダミー"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 1, STRING("ダミー"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ダミー"), 128)
    LOOP(0)
        SET_VALUE(LW(1), 0)
        CALL(evt_mario_get_motion, LW(10))
        IF_INT_EQUAL(LW(10), 20)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(0), 315)
            IF_INT_LESS_OR_EQUAL(LW(0), 360)
                IF_INT_GREATER_OR_EQUAL(LW(1), 60)
                    IF_INT_LESS_OR_EQUAL(LW(1), 78)
                        IF_INT_GREATER_OR_EQUAL(LW(2), -40)
                            IF_INT_LESS_OR_EQUAL(LW(2), 0)
                                BREAK_LOOP
                            END_IF
                        END_IF
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mobj_flag_onoff, 0, STRING("mobj_box00"), 64)
    SPAWN_EVENT(box_copy)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TBOX_FALL1"), LW(0), LW(1), LW(2), LW(10))
    CALL(evt_npc_jump_position_nohit, STRING("ダミー"), 275, 0, 75, 500, FLOAT(100.0f))
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    ADD(LW(0), -25)
    ADD(LW(1), -5)
    ADD(LW(2), 30)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    ADD(LW(0), 0)
    ADD(LW(1), -5)
    ADD(LW(2), 30)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    ADD(LW(0), 25)
    ADD(LW(1), -5)
    ADD(LW(2), 30)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.02f), 500)
    END_INLINE_EVENT
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TBOX_FALL2"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(GSWF(1196), 1)
    RETURN
EVT_END()

EVT_BEGIN(ultrahammer_get)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(lect_juyoitem_rel_set, PTR(all_juyoitem_lecture_rel_10_data_4C6C0))
    CALL(evt_item_entry, STRING("hammer"), 11, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_important_item, STRING("hammer"))
    CALL(evt_item_get_item, STRING("hammer"))
    CALL(evt_item_get_item_end_wait, STRING("hammer"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// The original's branch layout (beq BODY; b END) is what MWCC emits for
// `switch (evtGetValue(...)) { case 0: ... }`; the natural `if (x == 0)` gives the inverted `bne END`.
static USER_FUNC(evt_osreport) {
    s32* args = event->args;

    switch (evtGetValue(event, args[0])) {
        case 0:
            evtSetValue(event, args[1], (s32)search_evt_moji(evtGetValue(event, GSW(0))));
            break;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(keijiban_omote_joho)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(keijiban_data_alloc, STRING("kj_omote"))
    CALL(keijiban_joho_check, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(keijiban_joho_makelabel, 0, LW(1))
        CALL(evt_msg_print, 0, LW(1), 0, 0)
    ELSE
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_osreport, 0, LW(1))
            CALL(evt_msg_print, 1, STRING("<plate>\nゆうこうな　データが\n見つかりませんでした\nメッセージを　とうろく　してください\n<key>\n<scrl_auto>\n<once_stop>\n"), 0, 0)
            CALL(evt_msg_print_add_insert, 1, STRING("げんざいの　イベントばんごうは\n『%s』\nです\n<key>"), LW(1))
        ELSE
            CALL(evt_osreport, 0, LW(1))
            CALL(evt_msg_print, 1, STRING("<plate>\nゆうこうな　データが\nじゅうふくして　みつかりました\nゆうこうな　メッセージは　１つにしてください\n<key>\n<scrl_auto>\n<once_stop>"), 0, 0)
            SET_VALUE(LW(2), 0)
            SET_VALUE(LW(3), LW(0))
            LOOP(LW(0))
                CALL(keijiban_joho_makelabel, LW(2), LW(1))
                CALL(evt_msg_print_add_insert, 1, STRING("『%s』\n<key>\n<scrl_auto>\n<once_stop>\n"), LW(1))
                ADD(LW(2), 1)
            END_LOOP
            CALL(evt_msg_print_add_insert, 1, STRING("いじょう　%dこが\nじゅうふくしている　メッセージです\n<key>"), LW(3))
        END_IF
    END_IF
    CALL(keijiban_data_free)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(keijiban_ura_joho)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(keijiban_data_alloc, STRING("kj_ura"))
    CALL(keijiban_joho_check, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(keijiban_joho_makelabel, 0, LW(1))
        CALL(evt_msg_print, 0, LW(1), 0, 0)
    ELSE
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_osreport, 0, LW(1))
            CALL(evt_msg_print, 1, STRING("<plate>\nゆうこうな　データが\n見つかりませんでした\nメッセージを　とうろく　してください\n<key>\n<scrl_auto>\n<once_stop>\n"), 0, 0)
            CALL(evt_msg_print_add_insert, 1, STRING("げんざいの　イベントばんごうは\n『%s』\nです\n<key>"), LW(1))
        ELSE
            CALL(evt_osreport, 0, LW(1))
            CALL(evt_msg_print, 1, STRING("<plate>\nゆうこうな　データが\nじゅうふくして　みつかりました\nゆうこうな　メッセージは　１つにしてください\n<key>\n<scrl_auto>\n<once_stop>"), 0, 0)
            SET_VALUE(LW(2), 0)
            SET_VALUE(LW(3), LW(0))
            LOOP(LW(0))
                CALL(keijiban_joho_makelabel, LW(2), LW(1))
                CALL(evt_msg_print_add_insert, 1, STRING("『%s』\n<key>\n<scrl_auto>\n<once_stop>\n"), LW(1))
                ADD(LW(2), 1)
            END_LOOP
            CALL(evt_msg_print_add_insert, 1, STRING("いじょう　%dこが\nじゅうふくしている　メッセージです\n<key>"), LW(3))
        END_IF
    END_IF
    CALL(keijiban_data_free)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(gate_disp)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER(LW(2), -100)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_gai_kabe_k"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_gai_kabe_k_01"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_gai_kabe_k"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_gai_kabe_k_01"), 1)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(gor_goki1_urouro)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    LOOP(0)
        SET_VALUE(LW(0), LW(4))
        SET_VALUE(LW(1), LW(5))
        CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
        CALL(evt_npc_add_dirdist, LW(0), LW(1), LW(3), FLOAT(50.0f))
        CALL(evt_gor_villager_urouro_check_func, LW(0), LW(1), 0, 0, FLOAT(70.0f))
        CALL(evt_sub_get_dir, LW(4), LW(5), LW(0), LW(1), LW(3))
        IF_INT_GREATER(LW(3), 180)
            SET_VALUE(LW(6), 0)
        ELSE
            SET_VALUE(LW(6), 180)
        END_IF
        CALL(evt_goki_sub_intpl_init, 13, LW(4), LW(0), LW(5), LW(1), 50)
        LOOP(50)
            CALL(evt_goki_sub_intpl_get_value)
            SET_VALUE(LW(2), LW(0))
            SET_VALUE(LW(3), LW(1))
            ADD(LW(2), 480)
            ADD(LW(3), -180)
            CALL(evt_mapobj_trans, 1, STRING("S_goki_01"), LW(2), 0, LW(3))
            CALL(evt_mapobj_rotate, 1, STRING("S_goki_01"), 0, LW(6), 0)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(4), LW(0))
        SET_VALUE(LW(5), LW(1))
        CALL(evt_sub_random, 3000, LW(2))
        ADD(LW(2), 500)
        WAIT_MS(LW(2))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(gor_goki2_urouro)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    LOOP(0)
        SET_VALUE(LW(0), LW(4))
        SET_VALUE(LW(1), LW(5))
        CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
        CALL(evt_npc_add_dirdist, LW(0), LW(1), LW(3), FLOAT(50.0f))
        CALL(evt_gor_villager_urouro_check_func, LW(0), LW(1), 0, 0, FLOAT(65.0f))
        CALL(evt_sub_get_dir, LW(4), LW(5), LW(0), LW(1), LW(3))
        IF_INT_GREATER(LW(3), 180)
            SET_VALUE(LW(6), 0)
        ELSE
            SET_VALUE(LW(6), 180)
        END_IF
        CALL(evt_goki_sub_intpl_init, 13, LW(4), LW(0), LW(5), LW(1), 50)
        LOOP(50)
            CALL(evt_goki_sub_intpl_get_value)
            SET_VALUE(LW(2), LW(0))
            SET_VALUE(LW(3), LW(1))
            ADD(LW(2), -515)
            ADD(LW(3), -250)
            CALL(evt_mapobj_trans, 1, STRING("S_goki_02"), LW(2), 0, LW(3))
            CALL(evt_mapobj_rotate, 1, STRING("S_goki_02"), 0, LW(6), 0)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(4), LW(0))
        SET_VALUE(LW(5), LW(1))
        CALL(evt_sub_random, 3000, LW(2))
        ADD(LW(2), 500)
        WAIT_MS(LW(2))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(koopa_npc_entry)
    CALL(evt_npc_entry, STRING("ババ"), STRING("c_kamek_be"))
    CALL(evt_npc_set_tribe, STRING("ババ"), STRING("カメックババ"))
    CALL(evt_npc_set_position, STRING("ババ"), 0, -1000, 0)
    CALL(evt_npc_set_ry, STRING("ババ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("ババ"), 1536)
    CALL(evt_npc_status_onoff, 1, STRING("ババ"), 2)
    CALL(evt_npc_set_width, STRING("ババ"), 30)
    CALL(evt_npc_set_height, STRING("ババ"), 40)
    RETURN
EVT_END()
static s32 npc_process(void) {
    NpcWork* work = npcGetWorkPtr();
    NpcEntry* npc = work->entries;
    s32 i;

    for (i = 0; i < work->total; i++, npc++) {
        if (npc->flags & 1) {
            if (npc->position.y <= -1000.0f) {
                npc->flags |= 0x800;
            } else {
                npc->flags &= ~0x800;
            }
        }
    }
    return 0;
}

EVT_BEGIN(gor_01_koopa_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(delete_npc_regl_evt, STRING("ロテン１"))
    CALL(delete_npc_regl_evt, STRING("クリボー１"))
    CALL(delete_npc_regl_evt, STRING("クリボー２"))
    CALL(delete_npc_regl_evt, STRING("クリボー３"))
    CALL(delete_npc_regl_evt, STRING("チューさん１"))
    CALL(evt_mario_set_pos, 400, 0, 100)
    CALL(evt_npc_set_position, STRING("クリボー１"), 269, 0, 65)
    CALL(evt_npc_set_position, STRING("ロテン１"), 98, 0, 17)
    CALL(evt_npc_set_position, STRING("チューさん１"), 145, 0, 109)
    CALL(evt_npc_set_position, STRING("クリボー２"), 0, -1000, 0)
    CALL(evt_koopa_weary, 1)
    CALL(evt_npc_set_ry, STRING("クリボー１"), 90)
    CALL(evt_npc_set_ry, STRING("ロテン１"), 90)
    CALL(evt_npc_set_ry, STRING("チューさん１"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("クリボー１"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ロテン１"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("チューさん１"), 32)
    CALL(evt_cam3d_evt_set, 200, 160, 616, 200, 65, 74, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_fade_end_wait)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_msg_print, 0, STRING("kpa_stg6_000"), 0, STRING("クリボー１"))
        CALL(evt_npc_status_onoff, 1, STRING("クリボー１"), 2)
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        CALL(evt_npc_move_position, STRING("クリボー１"), 187, 65, 0, FLOAT(90.0f), 0)
        CALL(evt_sub_get_stopwatch, LW(0), LW(1))
        CALL(evt_npc_set_ry, STRING("クリボー１"), 90)
        WAIT_MS(300)
        CALL(evt_npc_move_position, STRING("クリボー１"), 142, 65, 0, FLOAT(90.0f), 0)
        CALL(evt_npc_set_ry, STRING("クリボー１"), 90)
        WAIT_MS(500)
        CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_PEAPLE_ESCAPE1"), 142, 0, 65, 0)
        CALL(evt_npc_move_position, STRING("クリボー１"), 142, 185, 0, FLOAT(150.0f), 0)
        CALL(evt_npc_move_position, STRING("クリボー１"), 380, 185, 0, FLOAT(150.0f), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("クリボー１"), 128)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_msg_print, 0, STRING("kpa_stg6_001"), 0, STRING("ロテン１"))
        CALL(evt_npc_status_onoff, 1, STRING("ロテン１"), 2)
        CALL(evt_npc_move_position, STRING("ロテン１"), 27, -55, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_move_position, STRING("ロテン１"), 27, -250, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("ロテン１"), 128)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_msg_print, 0, STRING("kpa_stg6_002"), 0, STRING("チューさん１"))
        CALL(evt_npc_move_position, STRING("チューさん１"), 145, 340, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("チューさん１"), 128)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1300)
        CALL(evt_npc_get_position, STRING("ロテン１"), LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("チューさん１"), LW(3), LW(4), LW(5))
        ADD(LW(0), LW(3))
        ADD(LW(1), LW(4))
        ADD(LW(2), LW(5))
        DIVIDE(LW(0), 2)
        DIVIDE(LW(1), 2)
        DIVIDE(LW(2), 2)
        CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_PEAPLE_ESCAPE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_SATIATED2_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_mov_pos2, 200, 100, FLOAT(60.0f))
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_2"), STRING("KPA_T_4"))
    CALL(evt_msg_print, 0, STRING("kpa_stg6_003"), 0, STRING("mario"))
    CALL(evt_npc_set_position, STRING("ババ"), 600, 280, 75)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("kpa_stg6_004"), 0, STRING("ババ"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_SURPRISED1_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_koopa_weary, 0)
    CALL(kpa_chg_pose, STRING("KPA_S_1"))
    CALL(evt_mario_set_dir, 90, 500, 0)
    WAIT_MS(700)
    CALL(evt_cam3d_evt_set, -225, 120, 755, 416, 150, -12, 0, 11)
    WAIT_FRAMES(1)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_kamek_kemuri1, STRING("ババ"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_glide_position, STRING("ババ"), 302, 25, 75, 0, FLOAT(80.0f), FLOAT(40.0f), 11, 0)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 255, 164, 587, 255, 120, 341, 2600, 11)
    WAIT_MS(3500)
    CALL(evt_msg_print, 0, STRING("kpa_stg6_005"), 0, STRING("ババ"))
    WAIT_MS(500)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    CALL(evt_msg_print, 0, STRING("kpa_stg6_006"), 0, STRING("mario"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("kpa_stg6_007"), 0, STRING("ババ"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("kpa_stg6_008"), 0, STRING("mario"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, STRING("ババ"), 8224)
        CALL(evt_npc_kamek_move_position, STRING("ババ"), 600, 100, 10, 0, FLOAT(120.0f), 0)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 500, 100, FLOAT(120.0f))
    WAIT_MS(2000)
    CALL(evt_npc_set_position, STRING("ルイージ"), 50, 0, 100)
    CALL(evt_npc_status_onoff, 1, STRING("ルイージ"), 2)
    CALL(evt_npc_move_position, STRING("ルイージ"), 200, 100, 0, 60, 0)
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("ルイージ"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("ルイージ"))
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ルイージ"), 1, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1200)
    CALL(evt_npc_move_position, STRING("ルイージ"), 50, 100, 0, 60, 0)
    WAIT_MS(1000)
    SET_VALUE(GSW(0), 342)
    CALL(evt_bero_mapchange, STRING("kpa_05"), STRING(""))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(sekai_yami_npc)
    LOOP(0)
        CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
        IF_INT_LESS_OR_EQUAL(GW(0), LW(3))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_get_reglid, LW(0), LW(1))
    DELETE_EVENT(LW(1))
    CALL(evt_eff_fukidashi, 1, STRING(""), LW(0), 0, 0, 0, 0, 0, 0, 0, 48)
    WAIT_MS(1000)
    LOOP(0)
        CALL(evt_sub_random, 400, LW(1))
        ADD(LW(1), 100)
        WAIT_MS(LW(1))
        CALL(evt_npc_reverse_ry, LW(0))
        WAIT_FRAMES(1)
        CALL(evt_npc_wait_pera, LW(0))
    END_LOOP
    RETURN
EVT_END()
static USER_FUNC(delete_npc_regl_evt) {
    NpcEntry* npc = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));

    if (npc->regularEvtId != 0) {
        evtDeleteID(npc->regularEvtId);
        npc->moveEvent = NULL;
        npc->regularEvtId = 0;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(yami_view) {
    f32 value = evtGetFloat(event, event->args[0]);
    Vec camPos;
    Vec camUp;
    Vec target;
    Mtx mtx;

    target = vec3_gor_00016038;
    target.z = value;
    camUp = vec3_gor_0001602c;
    camPos = vec3_gor_00016020;
    camPos.z = 100.0f + value;
    C_MTXLookAt(mtx, (Point3d*)&camPos, &camUp, (Point3d*)&target);
    envSetYamiView(mtx);
    return EVT_RETURN_DONE;
}

const Vec vec3_gor_00016020 = {0.0f, 100.0f, 0.0f};
const Vec vec3_gor_0001602c = {0.0f, 1.0f, 0.0f};
const Vec vec3_gor_00016038 = {0.0f, 0.0f, 0.0f};
static USER_FUNC(anim_tevcallback) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 mask = evtGetValue(event, args[1]);
    NpcEntry* npc = npcNameToPtr_NoAssert(name);

    if (npc == NULL) {
        return EVT_RETURN_DONE;
    }
    animPoseSetMaterialFlagOn(npc->poseId, (mask & 0xF) << 24);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(sekai_yami_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("キノじい"), 0, 0, 403)
    CALL(evt_cam3d_evt_set, 0, 350, 1530, 0, 150, 44, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback, STRING("キノじい"), 1)
    CALL(anim_tevcallback, STRING("ロテン１"), 1)
    CALL(anim_tevcallback, STRING("クリボー１"), 1)
    CALL(anim_tevcallback, STRING("クリボー２"), 1)
    CALL(anim_tevcallback, STRING("チューさん１"), 1)
    CALL(anim_tevcallback, STRING("ボッタクール"), 1)
    CALL(anim_tevcallback_mobj, STRING("mobj_save"), 1)
    CALL(anim_tevcallback_mobj, STRING("mobj_box00"), 1)
    SET_VALUE(GW(0), 700)
    SET_VALUE(LW(0), STRING("キノじい"))
    SPAWN_EVENT(sekai_yami_npc)
    SET_VALUE(LW(0), STRING("ロテン１"))
    SPAWN_EVENT(sekai_yami_npc)
    SET_VALUE(LW(0), STRING("クリボー１"))
    SPAWN_EVENT(sekai_yami_npc)
    SET_VALUE(LW(0), STRING("チューさん１"))
    SPAWN_EVENT(sekai_yami_npc)
    SET_VALUE(LW(0), STRING("ボッタクール"))
    SPAWN_EVENT(sekai_yami_npc)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_WORLD_DARK1"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 700, -850, 5000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(yami_view, LW(0))
        SET_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(1200)
    CALL(evt_sub_intpl_msec_init, 11, 255, 64, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_bg_set_color, LW(0), LW(0), LW(0), 255)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(2000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("nok_01"), STRING("sekai_yami1"))
    RETURN
EVT_END()
static USER_FUNC(anim_tevcallback_mobj) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 mask = evtGetValue(event, args[1]);
    MapObjectEntry* obj = mobjNameToPtrNoAssert(name);

    if (obj == NULL) {
        return EVT_RETURN_DONE;
    }
    animPoseSetMaterialFlagOn(obj->poseId, (mask & 0xF) << 24);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_entry) {
    s32* args = event->args;
    f32 s;
    f32 c;
    f32 angle;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    char name[16];
    s32 i;

    for (i = 0; i < 7; i++) {
        EffectEntry* eff;
        f32 px;
        f32 pz;
        angle = 6.2831855f * (f32)i / 7.0f;
        s = sin(angle);
        c = cos(angle);
        sprintf(name, "starstone%d", i);
        px = 140.0f * s + x;
        pz = z - 140.0f * c;
        eff = effStarStoneEntry(i, px, y, pz, 1.0f);
        effSetName(eff, name);
        ((effStarStoneData*)eff->userdata)->flags &= ~4;
        ((effStarStoneData*)eff->userdata)->flags |= 0x20;
        ((MahojinWork*)effNameToPtr("mahojin")->userdata)[i + 2].drawArg = eff;
        ((MahojinWork*)effNameToPtr("mahojin")->userdata)[i + 2].drawFunc = (void (*)(CameraId, void*))effStarStoneDraw;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_pos) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    char name[16];
    s32 i;

    for (i = 0; i < 7; i++) {
        f32 s;
        f32 c;
        f32 angle = 6.2831855f * (f32)i / 7.0f;
        EffectEntry* eff;

        s = sin(angle);
        c = cos(angle);
        sprintf(name, "starstone%d", i);
        eff = effNameToPtr(name);
        ((effStarStoneData*)eff->userdata)->x = 140.0f * s + x;
        ((effStarStoneData*)eff->userdata)->y = y;
        ((effStarStoneData*)eff->userdata)->z = z - 140.0f * c;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_ry) {
    f32 ry = evtGetFloat(event, event->args[0]);
    char name[16];
    s32 i;

    for (i = 0; i < 7; i++) {
        sprintf(name, "starstone%d", i);
        ((effStarStoneData*)effNameToPtr(name)->userdata)->rotY = ry;
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(minnnanokoe_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ロテン１"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("クリボー１"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("クリボー２"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("クリボー３"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("チューさん１"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ボッタクール"), 0, -1000, 0)
    CALL(evt_npc_check_delete, STRING("バッジ店長"))
    CALL(evt_npc_check_delete, STRING("バッジ店員"))
    CALL(evt_npc_check_delete, STRING("店長"))
    CALL(evt_npc_check_delete, STRING("店員"))
    CALL(evt_npc_check_delete, STRING("ナンシー"))
    CALL(evt_npc_check_delete, STRING("キノじい"))
    CALL(evt_npc_check_delete, STRING("マスター"))
    CALL(evt_npc_check_delete, STRING("マルコ"))
    CALL(evt_npc_check_delete, STRING("宿店員"))
    CALL(evt_npc_check_delete, STRING("ロテン２"))
    CALL(evt_npc_check_delete, STRING("チューさん２"))
    CALL(evt_npc_check_delete, STRING("水夫ボム兵"))
    CALL(evt_npc_check_delete, STRING("ルイージ"))
    CALL(evt_npc_check_delete, STRING("盗賊"))
    CALL(evt_npc_check_delete, STRING("キノピオ"))
    CALL(evt_npc_check_delete, STRING("キック２"))
    CALL(evt_cam3d_evt_set, 0, 645, 824, 0, 55, 283, 0, 11)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback_mobj, STRING("mobj_save"), 1)
    CALL(anim_tevcallback_mobj, STRING("mobj_box00"), 1)
    CALL(yami_view, -800)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(evt_seq_wait, 2)
    CALL(evt_eff, STRING("mahojin"), STRING("mahojin"), 8, 0, 5, 250, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(wait_mahojin)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_STARSTONE_EFFECT1"), 0)
    WAIT_MS(7000)
    CALL(starstone_entry, 0, -50, 250)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(-50.0f))
    SET_FLOAT_VALUE(GW(2), FLOAT(250.0f))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(starstone_set_pos, GW(0), GW(1), GW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(starstone_set_ry, LW(0))
            SUBTRACT_FLOAT(LW(0), FLOAT(3.0f))
            IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_VOICE1"))
    CALL(evt_sub_intpl_msec_init, 11, -50, 30, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 3000, 2500, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(1), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_sub_intpl_msec_init, 11, 2500, 3000, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(1), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
        END_LOOP
        SET_VALUE(GF(1), 1)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, 0, 110, 1093, 0, 110, 293, 1000, 11)
    WAIT_MS(1000)
    SET_VALUE(GF(0), 1)
    LOOP(0)
        IF_INT_NOT_EQUAL(GF(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_cam3d_evt_set, 0, -75, 999, 0, 300, 293, 2000, 1)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 25, 800, 2000)
    LABEL(3)
    CALL(evt_sub_intpl_msec_get_value)
    SET_VALUE(GW(1), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(3)
    END_IF
    WAIT_MS(2000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("nok_01"), STRING("minnnanokoe"))
    RETURN
EVT_END()
USER_FUNC(wait_mahojin) {
    s32 id = ((MahojinWork*)effNameToPtr("mahojin")->userdata)->animId[0];

    if (id != -1) {
        if (animPoseGetLoopTimes(id) != 0.0f) {
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(iri_09_init)
    IF_INT_EQUAL(GSW(1420), 9)
        CALL(set_fook_evt, PTR(shopper_data), PTR(iri_09_tentyo_talk))
    END_IF
    RETURN
EVT_END()
USER_FUNC(iri_09_item_tbl_alloc) {
    s32 n = evtGetValue(event, event->args[0]);

    iri_09_item_tbl[0] = _mapAlloc((n + 1) * 4);
    return EVT_RETURN_DONE;
}

USER_FUNC(iri_09_item_tbl_make) {
    s32* args = event->args;
    s32 n = evtGetValue(event, args[0]);
    s32 i;

    for (i = 0; i < n; i++) {
        ((s32*)iri_09_item_tbl[0])[i] = 0x8F;
    }
    ((s32*)iri_09_item_tbl[0])[i] = -1;
    evtSetValue(event, args[1], (s32)iri_09_item_tbl[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(iri_09_item_tbl_free) {
    _mapFree(iri_09_item_tbl[0]);
    iri_09_item_tbl[0] = NULL;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(iri_09_tentyo_talk)
    IF_INT_EQUAL(GSWF(5383), 1)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(5405), 0)
        CALL(evt_msg_print, 0, STRING("irai_09_001"), 0, STRING("店長"))
        SET_VALUE(GSWF(5405), 1)
        SET_VALUE(LW(0), -1)
    ELSE
        CALL(evt_msg_print, 0, STRING("irai_09_002"), 0, STRING("店長"))
        CALL(evt_pouch_check_item, 143, LW(0))
        IF_INT_GREATER(LW(0), 0)
            CALL(iri_09_item_tbl_alloc, LW(0))
            LOOP(0)
                CALL(evt_msg_continue)
                CALL(iri_09_item_tbl_make, LW(0), LW(1))
                CALL(evt_win_item_select, 0, LW(1), LW(2), 0)
                IF_INT_EQUAL(LW(2), -1)
                    SET_VALUE(LF(0), 1)
                    BREAK_LOOP
                END_IF
                CALL(evt_msg_print, 0, STRING("irai_09_003"), 0, STRING("店長"))
                CALL(evt_pouch_remove_item, 143, LW(1))
                ADD(GSW(1469), 1)
                IF_INT_EQUAL(GSW(1469), 5)
                    SET_VALUE(LF(0), 0)
                    BREAK_LOOP
                END_IF
                CALL(evt_pouch_check_item, 143, LW(0))
                IF_INT_LESS_OR_EQUAL(LW(0), 0)
                    SET_VALUE(LF(0), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
            CALL(iri_09_item_tbl_free)
            IF_INT_LESS(GSW(1469), 5)
                IF_INT_EQUAL(LF(0), 0)
                    CALL(evt_msg_print_add, 0, STRING("irai_09_006_1"))
                ELSE
                    CALL(evt_msg_print, 0, STRING("irai_09_006_1"), 0, STRING("店長"))
                END_IF
            ELSE
                CALL(evt_msg_print_add, 0, STRING("irai_09_004"))
                CALL(evt_item_entry, STRING("uk"), 150, 0, -1000, 0, 17, -1, 0)
                CALL(evt_item_get_item, STRING("uk"))
                CALL(evt_item_get_item_end_wait, STRING("uk"))
                CALL(evt_msg_print, 0, STRING("irai_09_006"), 0, STRING("店長"))
                SET_VALUE(GSW(1420), 0)
                SET_VALUE(GSWF(5383), 1)
                CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
                SET_VALUE(LW(0), -1)
                RETURN
            END_IF
        ELSE
            CALL(evt_msg_print_add, 0, STRING("irai_09_008"))
        END_IF
        SET_VALUE(LW(0), -100)
    END_IF
    RETURN
EVT_END()

// TODO: type
u32 iri_16_item_tbl[2] = {
    0x0000006E, 0xFFFFFFFF,
};

static EVT_BEGIN(iri_16_init)
    IF_INT_EQUAL(GSW(1420), 16)
        CALL(evt_npc_entry, STRING("チュチュリーナ"), STRING("c_tyutyu"))
        CALL(evt_npc_set_tribe, STRING("チュチュリーナ"), STRING("チュチュリーナ"))
        CALL(evt_npc_set_position, STRING("チュチュリーナ"), -516, 189, -110)
        CALL(evt_npc_set_ry, STRING("チュチュリーナ"), 90)
        CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 1536)
        CALL(evt_npc_change_interrupt, STRING("チュチュリーナ"), 6, PTR(iri_16_tyutyu_talk))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_16_tyutyu_talk)
    IF_INT_EQUAL(GSW(1420), 16)
        CALL(evt_snd_bgmoff_f, 1024, 2000)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_CYUCYURINA1"))
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_16_00"), 0, STRING("チュチュリーナ"))
            SET_VALUE(GSWF(5405), 1)
        ELSE
            IF_INT_EQUAL(GSWF(5407), 0)
                CALL(evt_msg_print, 0, STRING("irai_16_01"), 0, STRING("チュチュリーナ"))
            ELSE
                CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 32)
                CALL(evt_party_stop, 0)
                BEGIN_INLINE_EVENT
                    CALL(evt_party_move_pos2, 0, -448, -110, 60)
                    CALL(evt_party_set_dir_npc, 0, STRING("チュチュリーナ"))
                END_INLINE_EVENT
                CALL(evt_mario_mov_pos2, -485, -110, 60)
                CALL(evt_mario_set_dir_npc, STRING("チュチュリーナ"))
                WAIT_MS(500)
                CALL(evt_set_dir_to_target, STRING("チュチュリーナ"), STRING("mario"))
                WAIT_FRAMES(1)
                CALL(evt_npc_wait_pera, STRING("チュチュリーナ"))
                CALL(evt_msg_print, 0, STRING("irai_16_02"), 0, STRING("チュチュリーナ"))
                CALL(evt_win_item_select, 0, PTR(iri_16_item_tbl), LW(0), 0)
                IF_INT_NOT_EQUAL(LW(0), -1)
                    CALL(evt_pouch_remove_item, 110, LW(0))
                    CALL(evt_msg_print, 0, STRING("irai_16_03"), 0, STRING("チュチュリーナ"))
                    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_C_1"))
                    WAIT_MS(1000)
                    CALL(evt_msg_print, 0, STRING("irai_16_04"), 0, STRING("チュチュリーナ"))
                    CALL(evt_mario_set_pose, STRING("M_N_5B"))
                    WAIT_MS(600)
                    CALL(evt_mario_set_pose, STRING("M_S_1"))
                    CALL(evt_msg_print, 0, STRING("irai_16_05"), 0, STRING("チュチュリーナ"))
                    CALL(evt_pouch_get_havebadgecnt, LW(0))
                    IF_INT_LESS(LW(0), 200)
                        CALL(evt_msg_continue)
                        CALL(evt_item_entry, STRING("mb"), 316, 0, -1000, 0, 17, -1, 0)
                        CALL(evt_item_get_item, STRING("mb"))
                        CALL(evt_item_get_item_end_wait, STRING("mb"))
                        SET_VALUE(GSWF(1201), 1)
                    ELSE
                        CALL(evt_msg_print_add, 0, STRING("irai_16_05_1"))
                        BEGIN_INLINE_EVENT
                            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                            LOOP(0)
                                CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(3), LW(4), LW(5))
                                IF_INT_GREATER(LW(3), LW(0))
                                    BREAK_LOOP
                                END_IF
                                WAIT_FRAMES(1)
                            END_LOOP
                            CALL(evt_mario_set_dir_npc, STRING("チュチュリーナ"))
                        END_INLINE_EVENT
                        CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 1073741856)
                        CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
                        CALL(evt_npc_set_ry, STRING("チュチュリーナ"), 90)
                        WAIT_FRAMES(1)
                        CALL(evt_npc_wait_pera, STRING("チュチュリーナ"))
                        CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), -150, 300, LW(2), 800, 100)
                        WAIT_MS(2000)
                        CALL(evt_npc_set_ry, STRING("チュチュリーナ"), 270)
                        WAIT_FRAMES(1)
                        CALL(evt_npc_wait_pera, STRING("チュチュリーナ"))
                        BEGIN_INLINE_EVENT
                            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                            LOOP(0)
                                CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(3), LW(4), LW(5))
                                IF_INT_LESS(LW(3), LW(0))
                                    BREAK_LOOP
                                END_IF
                                WAIT_FRAMES(1)
                            END_LOOP
                            CALL(evt_mario_set_dir_npc, STRING("チュチュリーナ"))
                        END_INLINE_EVENT
                        CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), LW(0), LW(1), LW(2), 800, 50)
                        CALL(evt_set_dir_to_target, STRING("チュチュリーナ"), STRING("mario"))
                        WAIT_FRAMES(1)
                        CALL(evt_npc_wait_pera, STRING("チュチュリーナ"))
                        CALL(evt_npc_flag_onoff, 0, STRING("チュチュリーナ"), 1073741856)
                        CALL(evt_msg_print, 0, STRING("irai_16_05_2"), 0, STRING("チュチュリーナ"))
                        CALL(evt_item_entry, STRING("iri_16"), 316, -150, 153, -120, 16, GSWF(1201), 0)
                    END_IF
                    CALL(evt_msg_print, 0, STRING("irai_16_06"), 0, STRING("チュチュリーナ"))
                    CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 32)
                    CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
                    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
                    SUBTRACT(LW(3), 15)
                    CALL(evt_npc_move_position, STRING("チュチュリーナ"), LW(3), LW(5), 0, 40, 0)
                    WAIT_FRAMES(1)
                    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_K_1"))
                    CALL(evt_mario_set_pose, STRING("M_I_O"))
                    BEGIN_INLINE_EVENT
                        CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
                        ADD(LW(1), 30)
                        CALL(evt_eff, 0, STRING("kiss"), 0, LW(0), LW(1), LW(2), -45, 0, 0, 0, 0, 0, 0, 0)
                        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_REQUEST_CHURINA_KISS1"), LW(0), LW(1), LW(2), 0)
                    END_INLINE_EVENT
                    WAIT_MS(400)
                    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_S_1"))
                    WAIT_MS(500)
                    CALL(evt_mario_set_pose, STRING("M_S_1"))
                    CALL(evt_npc_move_position, STRING("チュチュリーナ"), LW(0), LW(2), 0, 40, 0)
                    WAIT_MS(1000)
                    CALL(evt_cam3d_evt_set, -525, 290, 230, -525, 225, -140, 800, 11)
                    WAIT_MS(800)
                    SET_VALUE(LW(8), STRING("チュチュリーナ"))
                    SET_VALUE(LW(9), 7)
                    CALL_CHILD(all_party_lecture_10_data_4B0DC)
                    SET_VALUE(GSW(1420), 0)
                    SET_VALUE(GSWF(5390), 1)
                    CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
                    CALL(evt_cam3d_evt_off, 500, 11)
                    CALL(evt_party_run, 0)
                END_IF
                CALL(evt_party_run, 0)
            END_IF
        END_IF
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        RETURN
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(marco_jewel)
    CALL(evt_mario_key_onoff, 0)
    DELETE_EVENT(GW(0))
    CALL(evt_msg_close, STRING("mac_4_004"))
    CALL(evt_msg_print, 0, STRING("mac_4_006"), 0, STRING("マルコ"))
    SPAWN_EVENT_GET_ID(PTR(marco_hitorigoto), GW(0))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_npc_entry)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 55, 154, -110)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073742880)
    CALL(evt_npc_entry, STRING("チュチュリーナ"), STRING("c_tyutyu"))
    CALL(evt_npc_set_tribe, STRING("チュチュリーナ"), STRING("チュチュリーナ"))
    CALL(evt_npc_set_position, STRING("チュチュリーナ"), 0, -1000, 0)
    CALL(evt_npc_set_ry, STRING("チュチュリーナ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 1073742880)
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt_sub)
    SET_VALUE(LW(0), PTR(door_data_03_f2))
    SPAWN_EVENT(door_open_se)
    CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SPAWN_EVENT(evt_door_03_00_f2)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("クリスチーヌ"), -50, -110, 0, 60, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_MS(1100)
    SET_VALUE(LW(0), PTR(door_data_03_f2))
    SPAWN_EVENT(door_close_se)
    CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SPAWN_EVENT(evt_door_03_00_f2)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_WHILE(GF(0))
    SET_VALUE(LW(0), PTR(door_data_02_f2))
    SPAWN_EVENT(door_open_se)
    CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SPAWN_EVENT(evt_door_02_00_f2)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_move_position, STRING("バッジ店長"), -114, -100, 0, 60, 0)
    CALL(evt_set_dir_to_target, STRING("バッジ店長"), STRING("クリスチーヌ"))
    SET_VALUE(LW(0), PTR(door_data_02_f2))
    SPAWN_EVENT(door_close_se)
    CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SPAWN_EVENT(evt_door_02_00_f2)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        CALL(evt_npc_set_anim, STRING("バッジ店長"), STRING("CHU_T_1"))
        WAIT_MS(1500)
        CALL(evt_npc_set_anim, STRING("バッジ店長"), STRING("CHU_S_1"))
        CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_T_1"))
        SET_VALUE(GF(15), 0)
        WAIT_MS(1500)
        CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_S_1"))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_CYUCYURINA1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_npc_set_position, STRING("バッジ店長"), -114, 154, -173)
    CALL(evt_npc_flag_onoff, 1, STRING("バッジ店長"), 1073742880)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_02_f2_naka"), 1)
    CALL(evt_unitwin_alloc, 1)
    CALL(badgeShop_getBargainTable, LW(0))
    CALL(evt_unitwin_selltable_setup, PTR(uwnobj_list), LW(0))
    CALL(evt_cam3d_evt_set, -80, 225, 400, -80, 200, -100, 0, 11)
    CALL(evt_seq_wait, 2)
    SET_VALUE(GF(15), 1)
    SPAWN_EVENT_GET_ID(PTR(epilogue_evt_sub), LW(0))
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("ep_43"), 0, 0)
    WAIT_WHILE(GF(15))
    DELETE_EVENT(LW(0))
    WAIT_MS(1000)
    CALL(evt_npc_set_anim, STRING("バッジ店長"), STRING("CHU_S_1"))
    CALL(evt_npc_set_anim, STRING("クリスチーヌ"), STRING("PKR_S_1"))
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 128)
    CALL(evt_npc_get_position, STRING("バッジ店長"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_dir, STRING("バッジ店長"), LW(3))
    CALL(evt_npc_set_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("チュチュリーナ"), LW(3))
    CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 32768)
    CALL(evt_npc_set_anim, STRING("バッジ店長"), STRING("CHU_Z_1"))
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_Z_1"))
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(20.0f))
    SET_VALUE(LW(2), 0)
    LOOP(0)
        ADD_FLOAT(LW(0), LW(1))
        IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
            ADD(LW(2), 1)
            IF_INT_GREATER_OR_EQUAL(LW(2), 5)
                BREAK_LOOP
            END_IF
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(2), 4)
            SUBTRACT_FLOAT(LW(1), FLOAT(1.0f))
            IF_INT_LESS_OR_EQUAL(LW(1), FLOAT(5.0f))
                SET_FLOAT_VALUE(LW(1), FLOAT(5.0f))
            END_IF
        END_IF
        IF_INT_EQUAL(LW(2), 2)
            IF_INT_EQUAL(LW(0), FLOAT(180.0f))
                CALL(evt_npc_flag_onoff, 1, STRING("バッジ店長"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("チュチュリーナ"), 128)
            END_IF
        END_IF
        CALL(evt_npc_set_rotate, STRING("バッジ店長"), 0, LW(0), 0)
        CALL(evt_npc_set_rotate, STRING("チュチュリーナ"), 0, LW(0), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_rotate, STRING("チュチュリーナ"), 0, 0, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("チュチュリーナ"), 32768)
    CALL(evt_npc_set_anim, STRING("バッジ店長"), STRING("CHU_S_1"))
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_S_1"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 275, 1397, 0, 200, -100, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(0), -50)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), -84, 302, -167, 700, 130)
    CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), 140, 354, -216, 700, 70)
    CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), 290, 354, -216, 700, 70)
    CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), 490, 504, -216, 700, 120)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff_f, 512, 3000)
    CALL(evt_snd_envoff_f, 512, 3000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_bero_mapchange, STRING("pik_04"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(gor_01_init_evt)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSW(0), 341)
        CALL(evt_mario_set_mode, 2)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami1"))
            SET_VALUE(LF(1), 1)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SET_VALUE(LF(1), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 402)
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR1"))
    ELSE
        IF_INT_EQUAL(LF(0), 0)
            IF_INT_EQUAL(LF(1), 1)
                CALL(evt_bero_get_entername, LW(0))
                IF_STR_EQUAL(LW(0), STRING("sekai_yami1"))
                    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_WITCH"))
                    CALL(evt_snd_envoff, 512)
                END_IF
                IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
                    CALL(evt_snd_bgmoff, 512)
                    CALL(evt_snd_envoff, 512)
                    SET_VALUE(LF(1), 1)
                END_IF
            ELSE
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
                CALL(evt_snd_bgmon, 160, 0)
                CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR1"))
            END_IF
        ELSE
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_KUPPA1"))
            CALL(evt_snd_bgm_scope, 0, 1)
            CALL(evt_snd_bgmon, 160, 0)
            CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR1"))
        END_IF
    END_IF
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_EQUAL(GSW(0), 6)
        CALL_CHILD(christine_nakama_init)
    END_IF
    IF_INT_EQUAL(GSW(0), 7)
        IF_INT_EQUAL(GSWF(1200), 0)
            SPAWN_EVENT(christine_nakama_after)
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1191), 0)
        IF_INT_EQUAL(GSWF(1189), 1)
            IF_INT_GREATER_OR_EQUAL(GSW(0), 56)
                SET_VALUE(GSWF(1191), 1)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1191), 0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 56)
            SET_VALUE(LW(0), GSWF(1189))
            ADD(LW(0), GSWF(1190))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(evt_sub_load_progresstime, GSW(108), LW(0))
                IF_INT_GREATER_OR_EQUAL(LW(0), 90000)
                    SET_VALUE(GSWF(1191), 1)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL_CHILD(luigi_npcEnt)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_sound_data_set, STRING("マルコ"), STRING("SFX_STG5_MARCO_MOVE1L"), STRING("SFX_STG5_MARCO_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    IF_INT_EQUAL(LF(0), 1)
        CALL_CHILD(koopa_npc_entry)
    END_IF
    IF_INT_NOT_EQUAL(LF(0), 1)
        CALL(evt_mobj_save_blk, STRING("mobj_save"), 185, 60, -55, 0, 0)
    END_IF
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -455, 0, -260, STRING("S_ground"), 125, GSWF(5569))
    CALL(evt_mobj_kururing_floor, STRING("kururin2"), 0, 0, 450, STRING("S_ground"), 125, GSWF(5573))
    CALL(evt_item_entry, STRING("item_00"), 125, 490, 0, 245, 16, GSWF(5570), 0)
    CALL(evt_item_entry, STRING("item_01"), 125, 550, 0, -150, 16, GSWF(5571), 0)
    CALL(evt_item_entry, STRING("item_02"), 125, -565, 162, -160, 16, GSWF(5572), 0)
    IF_INT_LESS(GSW(0), 223)
        CALL(evt_item_entry, STRING("ドクロジュエル"), 71, 315, 20, -125, 16, -1, 0)
        CALL(evt_item_flag_on, STRING("ドクロジュエル"), 2)
        CALL(evt_item_status_on, STRING("ドクロジュエル"), 33554432)
        CALL(evt_run_case_evt, 9, 1, STRING("A_jewel"), 0, PTR(marco_jewel), GW(8))
    END_IF
    IF_INT_EQUAL(GSWF(1188), 0)
        SET_VALUE(LW(0), PTR(door_data_01))
    ELSE
        SET_VALUE(LW(0), PTR(door_data_01_01))
    END_IF
    CALL(evt_door_data_copy, LW(0), GW(6))
    SET_VALUE(LW(0), GW(6))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_02_f1))
    IF_INT_LESS(GSW(0), 223)
        IF_INT_EQUAL(GSWF(1191), 1)
            SET_VALUE(LW(0), PTR(door_data_02_f1_01))
        END_IF
    END_IF
    CALL_CHILD(evt_door_entry)
    IF_INT_LESS(GSW(0), 19)
        CALL(evt_run_case_evt, 9, 1, STRING("A_ie_02_f2_doa01"), 0, PTR(evt_door_02_f2_junbityu), 0)
    ELSE
        SET_VALUE(LW(0), PTR(door_data_02_f2))
        CALL_CHILD(evt_door_entry)
    END_IF
    SET_VALUE(LW(0), PTR(door_data_03_f1))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_03_f2))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_04))
    CALL_CHILD(evt_door_entry)
    CALL_CHILD(gor_01_shop_setup)
    CALL(evt_kinopio_setup, PTR(kino_dt))
    CALL(evt_map_playanim, STRING("hae"), 1, 0)
    SPAWN_EVENT(koushukei_dai)
    SPAWN_EVENT(gate_disp)
    SPAWN_EVENT(gor_goki1_urouro)
    SPAWN_EVENT(gor_goki2_urouro)
    IF_INT_EQUAL(LF(1), 0)
        IF_INT_EQUAL(GSW(1420), 1)
            CALL(evt_item_entry, STRING("item_key00"), 49, -280, 153, -100, 0, GSWF(5405), 0)
        END_IF
        CALL_CHILD(borodo2_init)
        SPAWN_EVENT(iri_09_init)
        SPAWN_EVENT(iri_16_init)
        IF_INT_EQUAL(GSWF(5390), 1)
            CALL(evt_item_entry, STRING("iri_16"), 316, -150, 153, -120, 16, GSWF(1201), 0)
        END_IF
        IF_INT_EQUAL(GSWF(5392), 1)
            CALL(evt_item_entry, STRING("iri_18"), 183, -440, 30, -80, 16, GSWF(1202), 0)
        END_IF
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("A_keizi_mae"), 0, PTR(keijiban_omote_joho), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_keizi_ura"), 0, PTR(keijiban_ura_joho), 0)
    CALL(evt_pouch_check_item, 11, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        IF_INT_EQUAL(GSWF(1196), 0)
            CALL(evt_mobj_itembox, STRING("mobj_box00"), 340, 110, -20, 1, 0, PTR(ultrahammer_get), GSWF(1195))
            SPAWN_EVENT(ultrahammer_check)
        ELSE
            CALL(evt_mobj_itembox, STRING("mobj_box00"), 275, 0, 75, 1, 0, PTR(ultrahammer_get), GSWF(1195))
        END_IF
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_saku"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_dai"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_02_f1A"), 2)
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mario_kill_party, 0)
        SPAWN_EVENT(gor_01_koopa_evt)
    END_IF
    IF_INT_EQUAL(LF(1), 1)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami1"))
            SPAWN_EVENT(sekai_yami_evt)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SPAWN_EVENT(minnnanokoe_evt)
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 402)
        CALL(evt_mario_kill_party, 0)
        CALL_CHILD(epilogue_npc_entry)
        SPAWN_EVENT(epilogue_evt)
    END_IF
    SPAWN_EVENT(evt_sub_mail_event)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_roten"), 1)
    CALL(evt_hitobj_onoff, STRING("A_roten"), 1, 0)
    RETURN
EVT_END()

char* R_simple_recipe_table[78] = {
    "ファイアフラワー", "あつあつスープ", "あつあつスープ", "びりびりキノコ", "キノコいため", "キノコいため", "キノコ", "キノコいため", "キノコいため",
    "スーパーキノコ", "キノコいため", "キノコいため", "ウルトラキノコ", "キノコステーキ", "キノコステーキ", "きんきゅうキノコ", "キノコホイルやき",
    "キノコホイルやき", "しなびたキノコ", "", "キノコいため", "ハニーシロップ", "フレッシュジュース", "フレッシュジュース", "メイプルシロップ",
    "フレッシュジュース", "フレッシュジュース", "ローヤルゼリー", "フレッシュジュース", "フレッシュジュース", "じわじわキノコ", "キノコホイルやき",
    "キノコホイルやき", "じわじわシロップ", "フレッシュジュース", "フレッシュジュース", "ドライフラワー", "", "あつあつスープ", "ふしぎなタマゴ", "めだまやき",
    "めだまやき", "おうごんはっぱ", "ナンシーティー", "ナンシーティー", "トロピコマンゴー", "フレッシュジュース", "フレッシュジュース", "生パスタ", "スパゲティ",
    "スパゲティ", "おかしのもと", "ムースケーキ", "ムースケーキ", "レッド・カラリン", "", "スッキリドリンク", "カメカメはっぱ", "カメカメティー",
    "カメカメティー", "つくしんぼ", "", "あつあつスープ", "ピチピーチ", "フレッシュジュース", "フレッシュジュース", "ヤシの実", "スッキリドリンク",
    "スッキリドリンク", "ゆきうさぎ", "", "あつあつスープ", "どくキノコ", "", "キノコいため", "たんこぶ", "", "ナンシースペシャル",
};

char* R_cookbook_recipe_table[867] = {
    "かみなりドッカン", "かみなりゴロゴロ", "しれんのナベ", "キノコ", "かみなりゴロゴロ", "びりびりキノコ", "キノコ", "かみなりドッカン", "びりびりキノコ",
    "キノコ", "ファイアフラワー", "キノコホイルやき", "キノコ", "びりびりキノコ", "キノコホイルやき", "キノコ", "ふにゃふにゃくん", "しなびたキノコ",
    "スーパーキノコ", "かみなりゴロゴロ", "びりびりキノコ", "スーパーキノコ", "かみなりドッカン", "びりびりキノコ", "スーパーキノコ", "ファイアフラワー",
    "ナンシーディナー", "スーパーキノコ", "びりびりキノコ", "キノコホイルやき", "スーパーキノコ", "ふにゃふにゃくん", "しなびたキノコ", "スーパーキノコ",
    "キノコ", "キノコホイルやき", "ウルトラキノコ", "かみなりゴロゴロ", "びりびりキノコ", "ウルトラキノコ", "かみなりドッカン", "びりびりキノコ",
    "ウルトラキノコ", "ファイアフラワー", "ナンシースペシャル", "ウルトラキノコ", "びりびりキノコ", "キノコステーキ", "ウルトラキノコ", "ふにゃふにゃくん",
    "しなびたキノコ", "ウルトラキノコ", "キノコ", "キノコステーキ", "ウルトラキノコ", "スーパーキノコ", "キノコステーキ", "きんきゅうキノコ", "かみなりゴロゴロ",
    "びりびりキノコ", "きんきゅうキノコ", "かみなりドッカン", "びりびりキノコ", "きんきゅうキノコ", "ファイアフラワー", "ナンシーディナー", "きんきゅうキノコ",
    "びりびりキノコ", "キノコホイルやき", "きんきゅうキノコ", "ふにゃふにゃくん", "しなびたキノコ", "きんきゅうキノコ", "キノコ", "キノコステーキ",
    "きんきゅうキノコ", "スーパーキノコ", "キノコステーキ", "きんきゅうキノコ", "ウルトラキノコ", "キノコステーキ", "しなびたキノコ", "ファイアフラワー",
    "キノコいため", "しなびたキノコ", "びりびりキノコ", "キノコいため", "しなびたキノコ", "デカデカドリンク", "キノコ", "しなびたキノコ", "キノコ",
    "キノコいため", "しなびたキノコ", "スーパーキノコ", "キノコホイルやき", "しなびたキノコ", "ウルトラキノコ", "キノコステーキ", "しなびたキノコ",
    "きんきゅうキノコ", "キノコステーキ", "ハニーシロップ", "こおりのいぶき", "アイスキャンディ", "ハニーシロップ", "びりびりキノコ", "ハニーキノコ",
    "ハニーシロップ", "キノコ", "ハニーキノコ", "ハニーシロップ", "スーパーキノコ", "ハニーキノコＳ", "ハニーシロップ", "ウルトラキノコ", "ハニーキノコＺ",
    "ハニーシロップ", "きんきゅうキノコ", "ハニーキノコＳ", "メイプルシロップ", "こおりのいぶき", "ナンシーフラッペ", "メイプルシロップ", "びりびりキノコ",
    "メイプルキノコ", "メイプルシロップ", "キノコ", "メイプルキノコ", "メイプルシロップ", "スーパーキノコ", "メイプルキノコＳ", "メイプルシロップ",
    "ウルトラキノコ", "メイプルキノコＺ", "メイプルシロップ", "きんきゅうキノコ", "メイプルキノコＳ", "メイプルシロップ", "ハニーシロップ", "フレッシュジュース",
    "ローヤルゼリー", "こおりのいぶき", "ナンシーフラッペ", "ローヤルゼリー", "びりびりキノコ", "ローヤルキノコ", "ローヤルゼリー", "キノコ", "ローヤルキノコ",
    "ローヤルゼリー", "スーパーキノコ", "ローヤルキノコＳ", "ローヤルゼリー", "ウルトラキノコ", "ローヤルキノコＺ", "ローヤルゼリー", "きんきゅうキノコ",
    "ローヤルキノコＳ", "ローヤルゼリー", "ハニーシロップ", "フレッシュジュース", "ローヤルゼリー", "メイプルシロップ", "ナンシーティー", "じわじわキノコ",
    "かみなりゴロゴロ", "びりびりキノコ", "じわじわキノコ", "かみなりドッカン", "びりびりキノコ", "じわじわキノコ", "キラキラおとし", "ナンシーディナー",
    "じわじわキノコ", "ファイアフラワー", "ナンシーディナー", "じわじわキノコ", "びりびりキノコ", "キノコホイルやき", "じわじわキノコ", "キノコ",
    "キノコホイルやき", "じわじわキノコ", "スーパーキノコ", "ナンシーディナー", "じわじわキノコ", "ウルトラキノコ", "ナンシースペシャル", "じわじわキノコ",
    "きんきゅうキノコ", "ナンシーディナー", "じわじわキノコ", "ハニーシロップ", "ハニーキノコ", "じわじわキノコ", "メイプルシロップ", "メイプルキノコ",
    "じわじわキノコ", "ローヤルゼリー", "メイプルキノコＳ", "じわじわシロップ", "キノコ", "キノコホイルやき", "じわじわシロップ", "スーパーキノコ",
    "ナンシーディナー", "じわじわシロップ", "ウルトラキノコ", "ナンシースペシャル", "じわじわシロップ", "きんきゅうキノコ", "ナンシーディナー", "じわじわシロップ",
    "ハニーシロップ", "フレッシュジュース", "じわじわシロップ", "メイプルシロップ", "フレッシュジュース", "じわじわシロップ", "ローヤルゼリー", "フレッシュジュース",
    "じわじわシロップ", "じわじわキノコ", "じわじわキノコ", "イレカエール", "かみなりゴロゴロ", "ゆらゆらじしん", "イレカエール", "かみなりドッカン",
    "キラキラおとし", "イレカエール", "キラキラおとし", "かみなりドッカン", "イレカエール", "こおりのいぶき", "ファイアフラワー", "イレカエール",
    "ファイアフラワー", "こおりのいぶき", "イレカエール", "ゆらゆらじしん", "かみなりゴロゴロ", "イレカエール", "テレサのふく", "てきヨケール", "イレカエール",
    "びりびりキノコ", "スーパーキノコ", "イレカエール", "てきヨケール", "テレサのふく", "イレカエール", "まどわせのこな", "しかえしのこな", "イレカエール",
    "しかえしのこな", "まどわせのこな", "イレカエール", "ねむれよいこよ", "スッキリドリンク", "イレカエール", "デカデカドリンク", "ミニミニくん", "イレカエール",
    "カチカチコウラ", "ふにゃふにゃくん", "イレカエール", "ミニミニくん", "デカデカドリンク", "イレカエール", "ふにゃふにゃくん", "カチカチコウラ",
    "イレカエール", "キノコ", "ハニーシロップ", "イレカエール", "スーパーキノコ", "メイプルシロップ", "イレカエール", "ウルトラキノコ", "ローヤルゼリー",
    "イレカエール", "きんきゅうキノコ", "スーパーキノコ", "イレカエール", "しなびたキノコ", "キノコ", "イレカエール", "スッキリドリンク", "ねむれよいこよ",
    "イレカエール", "ハニーシロップ", "キノコ", "イレカエール", "メイプルシロップ", "スーパーキノコ", "イレカエール", "ローヤルゼリー", "ウルトラキノコ",
    "イレカエール", "じわじわキノコ", "どくキノコ", "イレカエール", "じわじわシロップ", "ハニーシロップ", "ドライフラワー", "ファイアフラワー", "あつあつスープ",
    "ドライフラワー", "びりびりキノコ", "かみなりドッカン", "ドライフラワー", "キノコ", "うちゅうしょく", "ドライフラワー", "スーパーキノコ", "うちゅうしょく",
    "ドライフラワー", "ウルトラキノコ", "うちゅうしょく", "ドライフラワー", "きんきゅうキノコ", "うちゅうしょく", "ドライフラワー", "しなびたキノコ",
    "うちゅうしょく", "ドライフラワー", "じわじわキノコ", "うちゅうしょく", "ドライフラワー", "ホットドッグ", "うちゅうしょく", "ふしぎなタマゴ",
    "ファイアフラワー", "バクハツタマゴ", "ふしぎなタマゴ", "キノコ", "キセツのオムレツ", "ふしぎなタマゴ", "スーパーキノコ", "キセツのオムレツ",
    "ふしぎなタマゴ", "ウルトラキノコ", "キセツのオムレツ", "ふしぎなタマゴ", "きんきゅうキノコ", "キセツのオムレツ", "ふしぎなタマゴ", "ドライフラワー",
    "うちゅうしょく", "おうごんはっぱ", "こおりのいぶき", "ゆきうさぎ", "おうごんはっぱ", "キノコ", "きんきゅうキノコ", "おうごんはっぱ", "スーパーキノコ",
    "きんきゅうキノコ", "おうごんはっぱ", "ウルトラキノコ", "きんきゅうキノコ", "おうごんはっぱ", "きんきゅうキノコ", "きんきゅうキノコ", "おうごんはっぱ",
    "じわじわキノコ", "こうちゃキノコ", "おうごんはっぱ", "ドライフラワー", "うちゅうしょく", "トロピコマンゴー", "びりびりキノコ", "ナンシーディナー",
    "トロピコマンゴー", "スーパーキノコ", "ナンシーディナー", "トロピコマンゴー", "ウルトラキノコ", "ナンシーディナー", "トロピコマンゴー", "ハニーシロップ",
    "フルーツパフェ", "トロピコマンゴー", "メイプルシロップ", "フルーツパフェ", "トロピコマンゴー", "ローヤルゼリー", "フルーツパフェ", "トロピコマンゴー",
    "じわじわシロップ", "フルーツパフェ", "トロピコマンゴー", "ドライフラワー", "うちゅうしょく", "生パスタ", "びりびりキノコ", "ナンシーディナー", "生パスタ",
    "キノコ", "ナンシーディナー", "生パスタ", "スーパーキノコ", "ナンシーディナー", "生パスタ", "ウルトラキノコ", "ナンシースペシャル", "生パスタ",
    "きんきゅうキノコ", "ナンシーディナー", "生パスタ", "しなびたキノコ", "ナンシーディナー", "生パスタ", "ドライフラワー", "うちゅうしょく", "生パスタ",
    "ふしぎなタマゴ", "ナンシーディナー", "おかしのもと", "ファイアフラワー", "ファイアキャンディ", "おかしのもと", "びりびりキノコ", "びりびりキャンディ",
    "おかしのもと", "まどわせのこな", "メロメロケーキ", "おかしのもと", "キノコ", "キノコケーキ", "おかしのもと", "スーパーキノコ", "キノコケーキ",
    "おかしのもと", "ウルトラキノコ", "キノコクレープ", "おかしのもと", "きんきゅうキノコ", "キノコケーキ", "おかしのもと", "ハニーシロップ", "ハニーあめ",
    "おかしのもと", "メイプルシロップ", "ナンシークッキー", "おかしのもと", "ローヤルゼリー", "ローヤルキャンディ", "おかしのもと", "じわじわキノコ",
    "キノコケーキ", "おかしのもと", "じわじわシロップ", "ナンシークッキー", "おかしのもと", "ドライフラワー", "うちゅうしょく", "おかしのもと", "ふしぎなタマゴ",
    "ナンシークッキー", "おかしのもと", "トロピコマンゴー", "マンゴープディング", "レッド・カラリン", "ファイアフラワー", "あつあつスープ", "レッド・カラリン",
    "しなびたキノコ", "キノコ", "レッド・カラリン", "スッキリドリンク", "すみぢる", "レッド・カラリン", "ハニーシロップ", "スッキリドリンク", "レッド・カラリン",
    "メイプルシロップ", "スッキリドリンク", "レッド・カラリン", "ローヤルゼリー", "スッキリドリンク", "レッド・カラリン", "生パスタ", "カラリーナパスタ",
    "レッド・カラリン", "おかしのもと", "ファイアキャンディ", "カメカメはっぱ", "キノコ", "きんきゅうキノコ", "カメカメはっぱ", "スーパーキノコ",
    "きんきゅうキノコ", "カメカメはっぱ", "ウルトラキノコ", "きんきゅうキノコ", "カメカメはっぱ", "きんきゅうキノコ", "きんきゅうキノコ", "カメカメはっぱ",
    "ハニーシロップ", "フレッシュジュース", "カメカメはっぱ", "メイプルシロップ", "フレッシュジュース", "カメカメはっぱ", "ローヤルゼリー", "フレッシュジュース",
    "カメカメはっぱ", "じわじわキノコ", "こうちゃキノコ", "カメカメはっぱ", "じわじわシロップ", "フレッシュジュース", "カメカメはっぱ", "ドライフラワー",
    "うちゅうしょく", "カメカメはっぱ", "おうごんはっぱ", "ヘルスィーサラダ", "カメカメはっぱ", "トロピコマンゴー", "カメまん", "カメカメはっぱ", "生パスタ",
    "カメスパ", "カメカメはっぱ", "レッド・カラリン", "すみぢる", "つくしんぼ", "キノコ", "ナンシーディナー", "つくしんぼ", "スーパーキノコ",
    "ナンシーディナー", "つくしんぼ", "ウルトラキノコ", "ナンシースペシャル", "つくしんぼ", "きんきゅうキノコ", "ナンシーディナー", "つくしんぼ",
    "ドライフラワー", "うちゅうしょく", "つくしんぼ", "ふしぎなタマゴ", "キセツのオムレツ", "つくしんぼ", "カメカメはっぱ", "ヘルスィーサラダ", "ピチピーチ",
    "びりびりキノコ", "ナンシーディナー", "ピチピーチ", "まどわせのこな", "メロメロケーキ", "ピチピーチ", "スーパーキノコ", "ナンシーディナー", "ピチピーチ",
    "ウルトラキノコ", "ナンシーディナー", "ピチピーチ", "しなびたキノコ", "キノコ", "ピチピーチ", "ハニーシロップ", "フルーツパフェ", "ピチピーチ",
    "メイプルシロップ", "フルーツパフェ", "ピチピーチ", "ローヤルゼリー", "フルーツパフェ", "ピチピーチ", "ドライフラワー", "うちゅうしょく", "ピチピーチ",
    "トロピコマンゴー", "フルーツパフェ", "ピチピーチ", "おかしのもと", "ピーチタルト", "ピチピーチ", "レッド・カラリン", "スッキリドリンク", "ヤシの実",
    "ファイアフラワー", "ココナッツボム", "ヤシの実", "ドライフラワー", "うちゅうしょく", "ヤシの実", "トロピコマンゴー", "フレッシュジュース", "ヤシの実",
    "生パスタ", "ナンシーディナー", "ヤシの実", "おかしのもと", "ココナツキャンディ", "ヤシの実", "カメカメはっぱ", "フレッシュジュース", "ヤシの実",
    "ピチピーチ", "フレッシュジュース", "キノコいため", "キラキラおとし", "ほしふるディナー", "キノコいため", "ドライフラワー", "うちゅうしょく",
    "キノコホイルやき", "キラキラおとし", "ほしふるディナー", "キノコホイルやき", "ドライフラワー", "うちゅうしょく", "キノコステーキ", "キラキラおとし",
    "ほしふるディナー", "キノコステーキ", "ドライフラワー", "うちゅうしょく", "しっぱいりょうり", "ドライフラワー", "うちゅうしょく", "ハニーキノコ",
    "ドライフラワー", "うちゅうしょく", "メイプルキノコ", "ドライフラワー", "うちゅうしょく", "ローヤルキノコ", "ドライフラワー", "うちゅうしょく",
    "ハニーキノコＳ", "ドライフラワー", "うちゅうしょく", "メイプルキノコＳ", "ドライフラワー", "うちゅうしょく", "ローヤルキノコＳ", "ドライフラワー",
    "うちゅうしょく", "ハニーキノコＺ", "ドライフラワー", "うちゅうしょく", "メイプルキノコＺ", "ドライフラワー", "うちゅうしょく", "ローヤルキノコＺ",
    "ドライフラワー", "うちゅうしょく", "ナンシーディナー", "カチカチコウラ", "カチカチりょうり", "ナンシーディナー", "ドライフラワー", "うちゅうしょく",
    "ナンシースペシャル", "カチカチコウラ", "カチカチりょうり", "ナンシースペシャル", "ドライフラワー", "うちゅうしょく", "ナンシーデラックス", "カチカチコウラ",
    "カチカチりょうり", "ナンシーデラックス", "ドライフラワー", "うちゅうしょく", "ナンシーダイナマイ", "ドライフラワー", "バクハツタマゴ", "ナンシーティー",
    "レッド・カラリン", "すみぢる", "ゆきうさぎ", "あつあつスープ", "いつまでもふたりで", "キノコケーキ", "ドライフラワー", "うちゅうしょく", "キノコクレープ",
    "ドライフラワー", "うちゅうしょく", "ムースケーキ", "ドライフラワー", "うちゅうしょく", "フルーツパフェ", "ドライフラワー", "うちゅうしょく",
    "バクハツタマゴ", "ドライフラワー", "うちゅうしょく", "イカスミパスタ", "ドライフラワー", "うちゅうしょく", "スパゲティ", "ドライフラワー", "うちゅうしょく",
    "スパゲティ", "レッド・カラリン", "カラリーナパスタ", "スパゲティ", "カメカメはっぱ", "カメスパ", "こうちゃキノコ", "レッド・カラリン", "すみぢる",
    "どくキノコ", "イレカエール", "ウルトラキノコ", "どくキノコ", "ドライフラワー", "うちゅうしょく", "どくキノコ", "おうごんはっぱ", "こうちゃキノコ",
    "ショコラケーキ", "ドライフラワー", "うちゅうしょく", "マンゴープディング", "ドライフラワー", "うちゅうしょく", "マンゴープディング", "ふしぎなタマゴ",
    "はつこいプディング", "はつこいプディング", "ドライフラワー", "うちゅうしょく", "ほしふるディナー", "ドライフラワー", "うちゅうしょく", "ほしふるディナー",
    "フルーツパフェ", "ナンシーディナー", "しれんのナベ", "ドライフラワー", "どくキノコ", "いつまでもふたりで", "ドライフラワー", "うちゅうしょく",
    "いつまでもふたりで", "どくキノコ", "しれんのナベ", "すみぢる", "じわじわキノコ", "どくキノコ", "すみぢる", "生パスタ", "イカスミパスタ", "すみぢる",
    "おかしのもと", "ショコラケーキ", "すみぢる", "ムースケーキ", "ショコラケーキ", "すみぢる", "スパゲティ", "イカスミパスタ", "キセツのオムレツ",
    "ドライフラワー", "うちゅうしょく", "カメカメティー", "レッド・カラリン", "すみぢる", "カメスパ", "ドライフラワー", "うちゅうしょく", "カメスパ",
    "レッド・カラリン", "カラリーナパスタ", "カメスパ", "すみぢる", "イカスミパスタ", "カラリーナパスタ", "ドライフラワー", "うちゅうしょく", "カラリーナパスタ",
    "ヤシの実", "ナンシーディナー", "カラリーナパスタ", "すみぢる", "イカスミパスタ", "メロメロケーキ", "ドライフラワー", "うちゅうしょく", "ピーチタルト",
    "ドライフラワー", "うちゅうしょく", "びりびりキャンディ", "ドライフラワー", "うちゅうしょく", "ファイアキャンディ", "ドライフラワー", "うちゅうしょく",
    "ハニーあめ", "ドライフラワー", "うちゅうしょく", "ココナツキャンディ", "ドライフラワー", "うちゅうしょく", "ローヤルキャンディ", "ドライフラワー",
    "うちゅうしょく", "ナンシークッキー", "ドライフラワー", "うちゅうしょく", "ヘルスィーサラダ", "ドライフラワー", "うちゅうしょく", "ヘルスィーサラダ",
    "生パスタ", "ナンシーディナー", "ヘルスィーサラダ", "キノコいため", "ナンシーディナー", "ヘルスィーサラダ", "キノコホイルやき", "ナンシースペシャル",
    "ヘルスィーサラダ", "キノコステーキ", "ナンシーデラックス", "ヘルスィーサラダ", "イカスミパスタ", "ナンシースペシャル", "ヘルスィーサラダ", "スパゲティ",
    "ナンシーディナー", "ヘルスィーサラダ", "カメスパ", "ナンシーディナー", "ヘルスィーサラダ", "カラリーナパスタ", "ナンシースペシャル", "カメまん",
    "ドライフラワー", "うちゅうしょく", "フレッシュジュース", "レッド・カラリン", "すみぢる", "たんこぶ", "ドライフラワー", "うちゅうしょく", "たんこぶ",
    "おうごんはっぱ", "ナンシーデラックス", "きんかい", "イレカエール", "キノコ", "きんかい×３", "イレカエール", "きんかい", "ココナッツボム",
    "ドライフラワー", "ヤシの実", "ココナッツボム", "バクハツタマゴ", "ナンシーダイナマイ", "カチカチりょうり", "ドライフラワー", "カチカチコウラ",
};

// TODO: type
u32 R_mystery_cook_list[20] = {
    0x00000084, 0x00000099, 0x000000EB, 0x0000008A,
    0x00000097, 0x0000008D, 0x00000087, 0x000000B3,
    0x000000DF, 0x000000C0, 0x000000DE, 0x000000D3,
    0x000000CF, 0x000000B7, 0x000000C1, 0x00000081,
    0x000000D1, 0x000000B0, 0x000000E8, 0x000000C5,
};

// TODO: type
s32 special_cook_list[16] = {
    0x000000C2, 0x000000C3, 0x000000C4, 0x000000CB,
    0x000000D2, 0x000000D5, 0x000000D6, 0x000000D7,
    0x000000D8, 0x000000D9, 0x000000DA, 0x000000DB,
    0x000000DC, 0x000000DD, 0x000000E0, 0xFFFFFFFF,
};
static USER_FUNC(set_fook_evt) {
    s32* args = event->args;
    void** table = (void**)evtGetValue(event, args[0]);

    table[1] = (void*)evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

const Vec vec3_gor_000169f8 = {-380.0f, 25.0f, -170.0f};
const Vec vec3_gor_00016a04 = {-380.0f, 60.0f, -170.0f};
s32 naniga_okorukana_cook(void) {
    s32 r = rand() % 100;

    if (r < 50) {
        return R_mystery_cook_list[(s32)((f32)(u32)(r * 20) / 50.0f)];
    }
    return 0xB6;
}

EVT_BEGIN(cooking_evt_sub)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_COOKING1"))
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 1, STRING("ナンシー"), 32)
    CALL(evt_npc_status_onoff, 1, STRING("ナンシー"), 2)
    CALL(evt_npc_move_position, STRING("ナンシー"), -485, -150, 0, 80, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ナンシー"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("ナンシー"), 32768)
    CALL(evt_npc_set_ry, STRING("ナンシー"), 90)
    CALL(evt_npc_set_anim, STRING("ナンシー"), STRING("S_1R"))
    WAIT_FRAMES(1)
    CALL(evt_npc_flag_onoff, 0, STRING("ナンシー"), 32768)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("gor_01_cook_016"), 0, STRING("ナンシー"))
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("ナンシー"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_COOK_COOKING1"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_npc_set_anim, STRING("ナンシー"), STRING("C_1"))
    WAIT_MS(2500)
    CALL(evt_snd_sfxoff, LW(3))
    CALL(evt_npc_flag_onoff, 1, STRING("ナンシー"), 1024)
    CALL(evt_npc_move_position, STRING("ナンシー"), -430, -150, 0, 80, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ナンシー"), 1024)
    CALL(evt_npc_set_anim, STRING("ナンシー"), STRING("C_1"))
    CALL(evt_npc_get_position, STRING("ナンシー"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_COOK_COOKING2"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(1500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_COOK_COOKING2"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(1500)
    CALL(evt_npc_flag_onoff, 1, STRING("ナンシー"), 1024)
    CALL(evt_npc_move_position, STRING("ナンシー"), -380, -150, 0, 80, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ナンシー"), 1024)
    CALL(evt_npc_set_anim, STRING("ナンシー"), STRING("S_1R"))
    WAIT_MS(200)
    CALL(evt_npc_get_position, STRING("ナンシー"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_COOK_COOKING3"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_npc_set_anim, STRING("ナンシー"), STRING("C_1"))
    WAIT_MS(2800)
    CALL(evt_snd_sfxoff, LW(3))
    WAIT_MS(200)
    CALL(cook_effect, LW(14))
    WAIT_MS(1300)
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(cook_effect, LW(15))
        WAIT_MS(1300)
    END_IF
    CALL(chk_special_cook, LW(13), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_COOK_LIGHT1"), -380, 40, -170, LW(0))
        CALL(evt_eff64, STRING("rad"), STRING("radiation_n64"), 3, -380, 40, -170, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1300)
        CALL(evt_eff_delete, STRING("rad"))
        CALL(evt_snd_sfxoff, LW(0))
        WAIT_MS(500)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("ナンシー"), 1024)
    CALL(evt_npc_move_position, STRING("ナンシー"), -405, -100, 0, 80, 0)
    CALL(evt_set_dir_to_target, STRING("ナンシー"), STRING("mario"))
    CALL(evt_npc_status_onoff, 0, STRING("ナンシー"), 2)
    CALL(evt_npc_flag_onoff, 0, STRING("ナンシー"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("ナンシー"), 512)
    WAIT_MS(300)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    WAIT_MS(500)
    RETURN
EVT_END()

// TODO: type
u32 iri_18_item_tbl[2] = {
    0x0000003C, 0xFFFFFFFF,
};
s32 itemNameToID(const char* name) {
    s32 i;

    for (i = 0; i < 0x153; i++) {
        if (strcmp(name, itemDataTable[i].id) == 0) {
            break;
        }
    }
    if (i >= 0x153) {
        return 0;
    }
    return i;
}

#pragma opt_dead_assignments off // needed: keeps the inlined itemNameToID tail's mr r3, r30 after subfe
USER_FUNC(cook_chk) {
    s32* args = event->args;
    s32 item = evtGetValue(event, args[0]);
    ItemData* p = &itemDataTable[item];
    u32 i;

    if (item == 0xA3) {
        s32 result = naniga_okorukana_cook();

        evtSetValue(event, args[1], result);
        evtSetValue(event, args[2], result);
        return EVT_RETURN_DONE;
    }
    for (i = 0; i < 0x1A; i++) {
        if (strcmp(p->id, R_simple_recipe_table[i * 3]) == 0) {
            s32 id = itemNameToID(R_simple_recipe_table[i * 3 + 1]);

            evtSetValue(event, args[1], id);
            id = itemNameToID(R_simple_recipe_table[i * 3 + 2]);
            evtSetValue(event, args[2], id);
            return EVT_RETURN_DONE;
        }
    }
    evtSetValue(event, args[1], 0);
    evtSetValue(event, args[2], 0);
    return EVT_RETURN_DONE;
}

#pragma opt_dead_assignments reset

#pragma opt_propagation off // needed: with propagation on MWCC hoists the args load before the inlined naniga_okorukana_cook
USER_FUNC(cook_chk2) {
    s32* args = event->args;
    s32 item1 = evtGetValue(event, args[0]);
    s32 item2 = evtGetValue(event, args[1]);
    ItemData* p1 = &itemDataTable[item1];
    ItemData* p2 = &itemDataTable[item2];
    u32 i;

    if (item1 == 0xA3 || item2 == 0xA3) {
        s32 result = naniga_okorukana_cook();

        evtSetValue(event, args[2], result);
        return EVT_RETURN_DONE;
    }
    for (i = 0; i < 0x121; i++) {
        if ((strcmp(p1->id, R_cookbook_recipe_table[i * 3]) == 0 &&
             strcmp(p2->id, R_cookbook_recipe_table[i * 3 + 1]) == 0) ||
            (strcmp(p2->id, R_cookbook_recipe_table[i * 3]) == 0 &&
             strcmp(p1->id, R_cookbook_recipe_table[i * 3 + 1]) == 0)) {
            s32 id = itemNameToID(R_cookbook_recipe_table[i * 3 + 2]);

            evtSetValue(event, args[2], id);
            return EVT_RETURN_DONE;
        }
    }
    evtSetValue(event, args[2], 0);
    return EVT_RETURN_DONE;
}

#pragma opt_propagation reset

s32 chkCookItem(s32 id) {
    return id != 0;
}

USER_FUNC(make_item_tbl) {
    s32* args = event->args;
    s32 item = evtGetValue(event, args[0]);
    s32 count = pouchGetHaveItemCnt();
    s32 j = 0;
    s32 i;

    for (i = 0; i < count; i++) {
        ItemType have;

        if (item == i) {
            continue;
        }
        have = pouchHaveItem(i);
        if (have == 0) {
            continue;
        }
        item_tbl[j] = have;
        j++;
    }
    item_tbl[j] = -1;
    evtSetValue(event, args[1], j);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_item_name) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], (s32)itemDataTable[id].id);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(cooking_evt)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(11), -1)
    SET_VALUE(LW(12), -1)
    SET_VALUE(LW(14), 0)
    SET_VALUE(LW(15), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ナンシー"), 32)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 1, -337, -80, 60)
        CALL(evt_party_set_dir_npc, 1, STRING("ナンシー"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -347, -100, 60)
        CALL(evt_party_set_dir_npc, 0, STRING("ナンシー"))
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_sub_get_dist, LW(0), LW(2), -377, -100, LW(3))
    IF_INT_LESS(LW(3), 10)
        CALL(evt_mario_dispflag_onoff, 1, 4)
    END_IF
    CALL(evt_mario_mov_pos2, -377, -100, 60)
    CALL(evt_mario_dispflag_onoff, 0, 4)
    CALL(evt_mario_set_dir_npc, STRING("ナンシー"))
    WAIT_MS(500)
    WAIT_WHILE(GF(0))
    CALL(evt_set_dir_to_target, STRING("ナンシー"), STRING("mario"))
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ナンシー"))
    CALL(evt_npc_flag_onoff, 0, STRING("ナンシー"), 32)
    IF_INT_EQUAL(GSW(1420), 18)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_18_001"), 0, STRING("ナンシー"))
            SET_VALUE(GSWF(5405), 1)
            CALL(evt_msg_print_add, 0, STRING("irai_18_008_1"))
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_18_002"), 0, STRING("ナンシー"))
            SET_VALUE(LF(0), 0)
            CALL(evt_pouch_check_item, 60, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 1)
                CALL(evt_msg_continue)
                CALL(evt_win_item_select, 0, PTR(iri_18_item_tbl), LW(0), 0)
            ELSE
                SET_VALUE(LW(0), -1)
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_NOT_EQUAL(LW(0), -1)
                CALL(evt_pouch_remove_item, 60, LW(0))
                CALL(evt_msg_print, 0, STRING("irai_18_003"), 0, STRING("ナンシー"))
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_npc_get_position, STRING("ナンシー"), LW(3), LW(4), LW(5))
                IF_INT_LESS(LW(0), LW(3))
                    ADD(LW(0), 20)
                ELSE
                    SUBTRACT(LW(0), 20)
                END_IF
                ADD(LW(1), 20)
                ADD(LW(2), 5)
                CALL(evt_item_entry, STRING("cb"), 60, LW(0), LW(1), LW(2), 17, -1, 0)
                CALL(evt_mario_set_pose, STRING("M_C_3"))
                WAIT_MS(800)
                CALL(evt_item_delete, STRING("cb"))
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                WAIT_MS(500)
                CALL(evt_msg_print, 0, STRING("irai_18_004"), 0, STRING("ナンシー"))
                CALL(evt_mario_set_pose, STRING("M_N_1"))
                SET_VALUE(LW(13), 183)
                SET_VALUE(LW(14), 148)
                SET_VALUE(LW(15), 154)
                CALL_CHILD(cooking_evt_sub)
                CALL(evt_msg_print, 0, STRING("irai_18_005"), 0, STRING("ナンシー"))
                SET_VALUE(LF(0), 0)
                CALL(evt_pouch_get_haveitemcnt, LW(0))
                CALL(evt_pouch_check_item, 1, LW(1))
                IF_INT_EQUAL(LW(1), 0)
                    IF_INT_GREATER_OR_EQUAL(LW(0), 10)
                        SET_VALUE(LF(0), 1)
                    END_IF
                ELSE
                    IF_INT_GREATER_OR_EQUAL(LW(0), 20)
                        SET_VALUE(LF(0), 1)
                    END_IF
                END_IF
                IF_INT_EQUAL(LF(0), 0)
                    CALL(evt_msg_continue)
                    CALL(evt_item_entry, STRING("hk"), 183, 0, -1000, 0, 17, -1, 0)
                    CALL(evt_item_get_item, STRING("hk"))
                    CALL(evt_item_get_item_end_wait, STRING("hk"))
                    SET_VALUE(GSWF(1202), 1)
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("irai_18_006_1"))
                    CALL(evt_npc_set_ry, STRING("ナンシー"), 270)
                    WAIT_FRAMES(1)
                    CALL(evt_npc_wait_pera, STRING("ナンシー"))
                    WAIT_MS(300)
                    CALL(evt_item_entry, STRING("iri_18"), 183, -440, 30, -80, 16, GSWF(1202), 0)
                    WAIT_MS(300)
                    CALL(evt_npc_set_ry, STRING("ナンシー"), 90)
                    WAIT_FRAMES(1)
                    CALL(evt_npc_wait_pera, STRING("ナンシー"))
                END_IF
                CALL(evt_msg_print, 0, STRING("irai_18_007"), 0, STRING("ナンシー"))
                SET_VALUE(GSW(1420), 0)
                SET_VALUE(GSWF(5392), 1)
                CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
                CALL(evt_mario_key_onoff, 1)
                RETURN
            ELSE
                IF_INT_EQUAL(LF(0), 0)
                    CALL(evt_msg_print, 0, STRING("irai_18_008"), 0, STRING("ナンシー"))
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("irai_18_008"))
                END_IF
                CALL(evt_msg_print_add, 0, STRING("irai_18_008_1"))
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(1197), 0)
        IF_INT_NOT_EQUAL(GSW(1420), 18)
            IF_INT_EQUAL(GSWF(5392), 0)
                CALL(evt_msg_print, 0, STRING("gor_01_cook_000"), 0, STRING("ナンシー"))
            END_IF
        END_IF
        CALL(evt_msg_print_add, 0, STRING("gor_01_cook_001"))
        SET_VALUE(GSWF(1197), 1)
    ELSE
        IF_INT_EQUAL(LSWF(1), 0)
            SET_VALUE(LF(0), 0)
            IF_INT_GREATER_OR_EQUAL(GSW(0), 215)
                IF_INT_NOT_EQUAL(GSW(1420), 18)
                    IF_INT_EQUAL(GSWF(5392), 0)
                        SET_VALUE(LF(0), 1)
                    END_IF
                END_IF
            END_IF
            IF_INT_EQUAL(LF(0), 0)
                IF_INT_NOT_EQUAL(GSW(1420), 18)
                    CALL(evt_msg_print, 0, STRING("gor_01_cook_002"), 0, STRING("ナンシー"))
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("gor_01_cook_002"))
                END_IF
            ELSE
                CALL(evt_msg_print, 0, STRING("gor_01_cook_003"), 0, STRING("ナンシー"))
            END_IF
        ELSE
            IF_INT_NOT_EQUAL(GSW(1420), 18)
                CALL(evt_msg_print, 0, STRING("gor_01_cook_004"), 0, STRING("ナンシー"))
            ELSE
                CALL(evt_msg_print_add, 0, STRING("gor_01_cook_004"))
            END_IF
        END_IF
    END_IF
    SET_VALUE(LSWF(1), 1)
    CALL(evt_msg_select, 0, STRING("gor_01_select"))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(0)
    END_IF
    CALL(make_item_tbl, LW(11), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("gor_01_cook_005"))
        GOTO(5)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("gor_01_cook_007"))
    LABEL(1)
    CALL(evt_win_item_select, 1, PTR(item_tbl), LW(0), LW(1))
    IF_INT_EQUAL(LW(0), -1)
        CALL(evt_msg_print, 0, STRING("gor_01_cook_008"), 0, STRING("ナンシー"))
        CALL(evt_msg_select, 0, STRING("gor_01_select"))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            GOTO(0)
            CASE_EQUAL(1)
            CALL(evt_msg_continue)
            GOTO(1)
        END_SWITCH
    END_IF
    SET_VALUE(LW(14), LW(0))
    SET_VALUE(LW(11), LW(1))
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSWF(5392), 1)
        CALL(make_item_tbl, LW(11), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("gor_01_cook_009"), 0, STRING("ナンシー"))
            CALL(evt_msg_select, 0, STRING("gor_01_select"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_continue)
                LABEL(2)
                CALL(evt_win_item_select, 1, PTR(item_tbl), LW(0), LW(1))
                IF_INT_EQUAL(LW(0), -1)
                    CALL(evt_msg_print, 0, STRING("gor_01_cook_010"), 0, STRING("ナンシー"))
                    CALL(evt_msg_select, 0, STRING("gor_01_cook_select"))
                    SWITCH(LW(0))
                        CASE_EQUAL(0)
                        SET_VALUE(LF(0), 1)
                        GOTO(3)
                        CASE_EQUAL(1)
                        CALL(evt_msg_continue)
                        GOTO(2)
                        CASE_EQUAL(2)
                        GOTO(0)
                    END_SWITCH
                END_IF
                IF_INT_EQUAL(LW(14), LW(0))
                    CALL(evt_msg_print, 0, STRING("gor_01_cook_011"), 0, STRING("ナンシー"))
                    GOTO(2)
                END_IF
                SET_VALUE(LW(15), LW(0))
                SET_VALUE(LW(12), LW(1))
            ELSE
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
    END_IF
    LABEL(3)
    IF_INT_EQUAL(LW(15), 0)
        CALL(get_item_name, LW(14), LW(0))
        IF_INT_EQUAL(LF(0), 0)
            CALL(evt_msg_print_insert, 0, STRING("gor_01_cook_012"), 0, STRING("ナンシー"), LW(0))
        ELSE
            CALL(evt_msg_print_add_insert, 0, STRING("gor_01_cook_012"), LW(0))
        END_IF
    ELSE
        CALL(get_item_name, LW(14), LW(0))
        CALL(get_item_name, LW(15), LW(1))
        CALL(evt_msg_print_insert, 0, STRING("gor_01_cook_013"), 0, STRING("ナンシー"), LW(0), LW(1))
    END_IF
    CALL(evt_msg_select, 0, STRING("gor_01_select"))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(4)
    END_IF
    IF_INT_EQUAL(LW(15), 0)
        CALL(cook_chk, LW(14), LW(0), LW(1))
        CALL(N_evt_pouch_remove_item_index, LW(14), LW(11), LW(3))
        IF_INT_EQUAL(GSWF(5392), 1)
            SET_VALUE(LW(2), LW(1))
        ELSE
            SET_VALUE(LW(2), LW(0))
        END_IF
    ELSE
        CALL(cook_chk2, LW(14), LW(15), LW(2))
        CALL(N_evt_pouch_remove_item_index, LW(14), LW(11), LW(3))
        CALL(N_evt_pouch_remove_item_index, LW(15), LW(12), LW(3))
    END_IF
    IF_INT_EQUAL(LW(2), 0)
        SET_VALUE(LW(2), 182)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("gor_01_cook_015"))
    SET_VALUE(LW(13), LW(2))
    CALL_CHILD(cooking_evt_sub)
    SET_VALUE(LW(2), LW(13))
    CALL(chk_cook_msg, LW(2), LW(3))
    SWITCH(LW(3))
        CASE_EQUAL(0)
        CALL(evt_msg_print, 0, STRING("gor_01_cook_018"), 0, STRING("ナンシー"))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("gor_01_cook_017"), 0, STRING("ナンシー"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("gor_01_cook_020"), 0, STRING("ナンシー"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("gor_01_cook_019"), 0, STRING("ナンシー"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("gor_01_cook_021"), 0, STRING("ナンシー"))
    END_SWITCH
    CALL(evt_msg_print_add, 0, STRING("gor_01_cook_022"))
    CALL(evt_item_entry, STRING("cook"), LW(2), 0, -1000, 0, 17, -1, 0)
    CALL(evt_item_get_item, STRING("cook"))
    SET_VALUE(GSWF(64), 1)
    CALL(cooking_flag, LW(2))
    CALL(evt_msg_print, 0, STRING("gor_01_cook_023"), 0, STRING("ナンシー"))
    GOTO(5)
    LABEL(0)
    CALL(evt_msg_print_add, 0, STRING("gor_01_cook_006"))
    GOTO(5)
    LABEL(4)
    CALL(evt_msg_print_add, 0, STRING("gor_01_cook_014"))
    GOTO(5)
    LABEL(5)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// item_tbl and iri_09_item_tbl are uninitialised: the original keeps both in .bss
// (item_tbl at 0x0, iri_09_item_tbl at 0x54). Their tentative definitions at the top
// of the file place them there.


// The original lays chk_cook_eff_type out as a branchy tree (two sparse switches) and inlines it
// into cook_effect / chk_cook_msg. Auto-inlining drops the branchy source (too big as a plain
// function), so it lives in a static inline helper that both the global wrapper and the callers
// inline through.
static inline s32 cook_eff_type_impl(s32 itemId) {
    if (itemId < 0x94) {
        return 1;
    }
    if (itemId < 0xA1) {
        return 0;
    }
    if (itemId < 0xB3) {
        switch (itemId) {
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xB1:
        case 0xB2:
            return 1;
        }
        return 0;
    }
    if (itemId < 0xEC) {
        switch (itemId) {
        case 0xC4:
        case 0xCA:
        case 0xCB:
        case 0xD1:
            return 1;
        }
        return 0;
    }
    return 0;
}

s32 chk_cook_eff_type(s32 itemId) {
    return cook_eff_type_impl(itemId);
}
USER_FUNC(cook_effect) {
    s32 item = evtGetValue(event, event->args[0]);

    switch (chk_cook_eff_type(item)) {
        case 0: {
            Vec pos = vec3_gor_000169f8;

            psndSFXOn_3D("SFX_EVT_COOK_FIRE1", &pos);
            effFireEntry(4, 0x3C, -380.0f, 25.0f, -170.0f, 1.5f);
            break;
        }
        case 1:
        default: {
            Vec pos = vec3_gor_00016a04;

            psndSFXOn_3D("SFX_EVT_COOK_EXPLOSION1", &pos);
            effKemuri12N64Entry(0, 0x3C, -380.0f, 60.0f, -170.0f, 2.0f);
            break;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(chk_cook_msg) {
    s32* args = event->args;
    s32 item = evtGetValue(event, args[0]);
    s32 result;

    if (item == 0xC3 || item == 0xC4) {
        result = 0;
    } else if ((item >= 0xC1 && item <= 0xC5) || item == 0xE8 || item == 0xC8) {
        result = 1;
    } else if (item == 0xB6) {
        result = 2;
    } else {
        result = chk_cook_eff_type(item) + 3;
    }
    evtSetValue(event, args[1], result);
    return EVT_RETURN_DONE;
}

USER_FUNC(cooking_flag) {
    swSet(evtGetValue(event, event->args[0]) - 0x72);
    return EVT_RETURN_DONE;
}

USER_FUNC(chk_special_cook) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    s32 found = 0;
    s32 i;
    s32 entry;

    for (i = 0; (entry = special_cook_list[i]) != -1; i++) {
        if (value == entry) {
            found = 1;
            break;
        }
    }
    evtSetValue(event, args[1], found);
    return EVT_RETURN_DONE;
}
// clang-format on

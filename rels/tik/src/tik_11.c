#include "tik_11.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_win.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 unk_802399f8[];
static USER_FUNC(bgm_start_wait);
static USER_FUNC(bgScrollControll);
static USER_FUNC(enzan_lw_load);
static USER_FUNC(enzan_lw_save);
static void enzan_set_f_i(void);  // TODO: real signature
static void enzan_set_i_f(void);  // TODO: real signature
static USER_FUNC(enzan_cam_turn_keisan_f);
static void enzan_cam_turn_keisan(void);  // TODO: real signature
static void wariai_enzan_0f(void);  // TODO: real signature
static void wariai_enzan_0(void);  // TODO: real signature
static USER_FUNC(wariai_enzan_1f);
static void wariai_enzan_1(void);  // TODO: real signature
static USER_FUNC(effect_stardust);
static USER_FUNC(setmertn);

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(unk_evt_tik_00021280)
    LOOP(0)
        CALL(evt_party_set_dir_pos, 0, -9, 0)
        CALL(unk_800e23e0, -9, 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: write bgm_start_wait (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write bgScrollControll (static, 0x40 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write enzan_lw_load (static, 0x30 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write enzan_lw_save (static, 0x30 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write enzan_cam_turn_keisan_f (static, 0x1e4 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write effect_stardust (static, 0xdc bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(powerdownya_mario_level_rtn)
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_LESS(LW(0), 39)
        CALL(evt_msg_print_add, 0, STRING("powerdown_04"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_win_coin_on, 0, LW(12))
    LOOP(0)
        CALL(evt_win_other_select, 9)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_msg_print, 0, STRING("powerdown_03_01"), 0, STRING("me"))
            RETURN
        END_IF
        WAIT_MS(240)
        SET_VALUE(GW(1), LW(1))
        CALL(evt_win_other_select, 10)
        IF_INT_EQUAL(LW(0), 0)
            WAIT_MS(240)
            CONTINUE_LOOP
        END_IF
        CALL(evt_msg_print_insert, 0, STRING("powerdown_06"), 0, STRING("me"), LW(2), LW(3), 39)
        CALL(evt_msg_select, 0, STRING("powerdown_09"))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_msg_continue)
    END_LOOP
    CALL(evt_pouch_get_coin, LW(0))
    SUBTRACT(LW(0), 39)
    CALL(evt_pouch_set_coin, LW(0))
    CALL(evt_win_coin_wait, LW(12))
    WAIT_MS(500)
    CALL(evt_win_coin_off, LW(12))
    CALL(enzan_lw_save)
    SPAWN_EVENT_GET_ID(PTR(unk_evt_tik_00021280), GW(1))
    CALL(evt_snd_bgmoff_f, 1024, 2000)
    CALL(evt_msg_print_add, 0, STRING("powerdown_11"))
    CALL(bgScrollControll, 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -20, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(5))
        CALL(evt_mapobj_trans, 1, STRING("down_pira"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(5), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(500)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_POWERDOWN1"))
    CALL(bgm_start_wait)
    CALL(evt_eff64, STRING("aaax"), STRING("lensflare_n64"), 0, -9, 49, 0, FLOAT(1.0f), -1, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("down_pira"), 1)
    WAIT_FRAMES(5)
    CALL(evt_msg_print, 0, STRING("powerdown_12"), 0, STRING("me"))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S"), 255, 255, 255, LW(0))
            CALL(evt_mapobj_color, 1, STRING("down_pira"), 255, 255, 255, 255)
            CALL(evt_mapobj_color, 1, STRING("ie_naka"), 255, 255, 255, 255)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 90, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("ie_l"), 0, 0, LW(0))
            MULTIPLY(LW(0), -1)
            CALL(evt_mapobj_rotate, 1, STRING("ie_o"), LW(0), 0, 0)
            CALL(evt_mapobj_rotate, 1, STRING("ie_r"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1600)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -9, 212, 886, -9, 56, 0, 1600, 11)
    WAIT_MS(1600)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 100, 7000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(enzan_cam_turn_keisan_f, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(6), 7000)
        DIVIDE(LW(6), 1000)
        ADD(LW(6), 1)
        LOOP(LW(6))
            CALL(evt_eff64, 0, STRING("crystal_n64"), 3, -9, 49, 0, FLOAT(1.0f), 60, 0, 0, 0, 0, 0, 0)
            WAIT_MS(1000)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        SET_FLOAT_VALUE(LW(7), LW(4))
        ADD_FLOAT(LW(7), FLOAT(18.0f))
        CALL(evt_eff64, STRING("cccx"), STRING("pokopi_pcharge_n64"), 2, LW(3), LW(7), LW(5), 1, -1, 0, 0, 0, 0, 0, 0)
        WAIT_MS(500)
        CALL(evt_mario_set_pose, STRING("M_I_Z"))
    END_INLINE_EVENT
    WAIT_MS(7000)
    CALL(evt_eff_softdelete, STRING("aaax"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("down_pira"), 1)
    WAIT_MS(500)
    CALL(evt_eff64, STRING("dddx"), STRING("release_n64"), 65283, -9, 49, 0, FLOAT(1.0f), 200, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_eff_softdelete, STRING("cccx"))
    CALL(evt_mario_set_pose, STRING("M_I_1"))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    SET_FLOAT_VALUE(LW(7), LW(4))
    ADD_FLOAT(LW(7), FLOAT(18.0f))
    CALL(effect_stardust, 0, LW(3), LW(7), LW(5), FLOAT(50.0f), FLOAT(50.0f), 20, 120)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 1200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S"), 255, 255, 255, LW(0))
            CALL(evt_mapobj_color, 1, STRING("down_pira"), 255, 255, 255, 255)
            CALL(evt_mapobj_color, 1, STRING("ie_naka"), 255, 255, 255, 255)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 90, 0, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("ie_l"), 0, 0, LW(0))
            MULTIPLY(LW(0), -1)
            CALL(evt_mapobj_rotate, 1, STRING("ie_o"), LW(0), 0, 0)
            CALL(evt_mapobj_rotate, 1, STRING("ie_r"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(bgScrollControll, 1)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_off, 1600, 11)
    WAIT_MS(1400)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_sub_intpl_msec_init, 11, -20, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(5))
        CALL(evt_mapobj_trans, 1, STRING("down_pira"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(5), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_set_pose, STRING("M_V_2"))
    WAIT_MS(1500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_dispflag_onoff, 0, 4)
    DELETE_EVENT(GW(1))
    CALL(enzan_lw_load)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(unitwin_setmariolevel, LW(5), LW(6), LW(7), LW(8))
    IF_INT_NOT_EQUAL(LW(8), 0)
        CALL(evt_msg_print, 0, STRING("powerdown_10"), 0, STRING("me"))
    ELSE
        CALL(evt_msg_print, 0, STRING("powerdown_13"), 0, STRING("me"))
    END_IF
    RETURN
EVT_END()

// TODO: write wariai_enzan_1f (static, 0xc0 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(powerdownya_party_lank_rtn)
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_LESS(LW(0), 39)
        CALL(evt_msg_print_add, 0, STRING("powerdown_04"))
        RETURN
    END_IF
    CALL(unk_802399f8, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("powerdown_07"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    CALL(evt_win_coin_on, 0, LW(12))
    LOOP(0)
        CALL(evt_win_other_select, 7)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_msg_print, 0, STRING("powerdown_03_01"), 0, STRING("me"))
            RETURN
        END_IF
        WAIT_MS(240)
        SET_VALUE(GW(1), LW(1))
        CALL(evt_win_other_select, 8)
        IF_INT_EQUAL(LW(0), 0)
            WAIT_MS(240)
            CONTINUE_LOOP
        END_IF
        CALL(evt_msg_print_insert, 0, STRING("powerdown_08"), 0, STRING("me"), LW(3), LW(4), 39)
        CALL(evt_msg_select, 0, STRING("powerdown_09"))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_msg_continue)
    END_LOOP
    CALL(evt_pouch_get_coin, LW(0))
    SUBTRACT(LW(0), 39)
    CALL(evt_pouch_set_coin, LW(0))
    CALL(evt_win_coin_wait, LW(12))
    WAIT_MS(500)
    CALL(evt_win_coin_off, LW(12))
    CALL(enzan_lw_save)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(1), LW(0))
        CALL(evt_mario_goodbye_party, 0)
        CALL(evt_mario_hello_party, 0, LW(1))
    END_IF
    CALL(evt_msg_print_add, 0, STRING("powerdown_11"))
    CALL(bgScrollControll, 0)
    CALL(evt_mario_goodbye_party, 0)
    CALL(evt_mario_hello_party, 0, LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 0, -20, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(5))
        CALL(evt_mapobj_trans, 1, STRING("down_pira"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(5), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(500)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_POWERDOWN1"))
    CALL(bgm_start_wait)
    CALL(evt_eff64, STRING("aaax"), STRING("lensflare_n64"), 0, -9, 49, 0, FLOAT(1.0f), -1, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("down_pira"), 1)
    SPAWN_EVENT_GET_ID(PTR(unk_evt_tik_00021280), GW(1))
    CALL(evt_msg_print, 0, STRING("powerdown_12"), 0, STRING("me"))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S"), 255, 255, 255, LW(0))
            CALL(evt_mapobj_color, 1, STRING("down_pira"), 255, 255, 255, 255)
            CALL(evt_mapobj_color, 1, STRING("ie_naka"), 255, 255, 255, 255)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 90, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("ie_l"), 0, 0, LW(0))
            MULTIPLY(LW(0), -1)
            CALL(evt_mapobj_rotate, 1, STRING("ie_o"), LW(0), 0, 0)
            CALL(evt_mapobj_rotate, 1, STRING("ie_r"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1600)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -9, 212, 886, -9, 56, 0, 1600, 11)
    WAIT_MS(1600)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 100, 7000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(enzan_cam_turn_keisan_f, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(6), 7000)
        DIVIDE(LW(6), 1000)
        ADD(LW(6), 1)
        LOOP(LW(6))
            CALL(evt_eff64, 0, STRING("crystal_n64"), 3, -9, 49, 0, FLOAT(1.0f), 60, 0, 0, 0, 0, 0, 0)
            WAIT_MS(1000)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_party, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(1)
            CALL(evt_party_set_pose, 0, STRING("PKR_I_1"))
            CASE_EQUAL(2)
            CALL(evt_party_set_pose, 0, STRING("PNK_I_1"))
            CASE_EQUAL(3)
            CALL(evt_party_set_pose, 0, STRING("I_1"))
            CASE_EQUAL(4)
            CALL(evt_party_set_pose, 0, STRING("PYS_I_1"))
            CASE_EQUAL(5)
            CALL(evt_party_set_pose, 0, STRING("PWD_I_1"))
            CASE_EQUAL(6)
            CALL(evt_party_set_pose, 0, STRING("PTR_I_1"))
            CASE_EQUAL(7)
            CALL(evt_party_set_pose, 0, STRING("PCH_I_1"))
        END_SWITCH
        WAIT_MS(500)
        DELETE_EVENT(GW(1))
        CALL(evt_party_dispflg_onoff, 0, 16, 1)
        CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(effect_stardust, 4, LW(3), LW(4), LW(5), FLOAT(50.0f), FLOAT(50.0f), 14, 60)
        CALL(evt_party_get_dispdir, 0, LW(9))
        SET_FLOAT_VALUE(LW(10), LW(9))
        ADD_FLOAT(LW(10), 1440)
        CALL(evt_party_flg_onoff, 0, 33554432, 1)
        SET_VALUE(LW(6), 60)
        LOOP(LW(6))
            CALL(wariai_enzan_1f, LW(6), 60, 0, 40, LW(7))
            CALL(wariai_enzan_1f, LW(6), 60, LW(9), LW(10), LW(8))
            ADD_FLOAT(LW(7), LW(4))
            CALL(evt_party_set_pos, 0, LW(3), LW(7), LW(5))
            CALL(evt_party_set_dispdir, 0, LW(8))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_party_dispflg_onoff, 0, 16, 0)
        CALL(evt_party_flg_onoff, 0, 33554432, 0)
        CALL(evt_mario_goodbye_party, 0)
        CALL(evt_mario_hello_party, 0, LW(1))
        WAIT_MS(500)
        CALL(evt_party_stop, 0)
        SPAWN_EVENT_GET_ID(PTR(unk_evt_tik_00021280), GW(1))
        CALL(evt_mario_get_party, LW(1))
        SWITCH(LW(1))
            CASE_EQUAL(1)
            CALL(evt_party_set_pose, 0, STRING("PKR_C_1"))
            CASE_EQUAL(2)
            CALL(evt_party_set_pose, 0, STRING("PNK_C_1"))
            CASE_EQUAL(3)
            CALL(evt_party_set_pose, 0, STRING("C_1"))
            CASE_EQUAL(4)
            CALL(evt_party_set_pose, 0, STRING("PYS_C_1"))
            CASE_EQUAL(5)
            CALL(evt_party_set_pose, 0, STRING("PWD_C_1"))
            CASE_EQUAL(6)
            CALL(evt_party_set_pose, 0, STRING("PTR_C_1"))
            CASE_EQUAL(7)
            CALL(evt_party_set_pose, 0, STRING("PCH_C_1"))
        END_SWITCH
        CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
        SET_FLOAT_VALUE(LW(7), LW(4))
        ADD_FLOAT(LW(7), FLOAT(18.0f))
        CALL(evt_eff64, STRING("cccx"), STRING("pokopi_pcharge_n64"), 2, LW(3), LW(7), LW(5), 1, -1, 0, 0, 0, 0, 0, 0)
        WAIT_MS(2000)
        CALL(evt_party_set_breed_pose, 0, 1)
    END_INLINE_EVENT
    WAIT_MS(7000)
    CALL(evt_eff_softdelete, STRING("aaax"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("down_pira"), 1)
    WAIT_MS(500)
    CALL(evt_eff64, STRING("dddx"), STRING("release_n64"), 65283, -9, 49, 0, FLOAT(1.0f), 200, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_eff_softdelete, STRING("cccx"))
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_I_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_I_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("I_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_I_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_I_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_I_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_I_1"))
    END_SWITCH
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    SET_FLOAT_VALUE(LW(7), LW(4))
    ADD_FLOAT(LW(7), FLOAT(18.0f))
    CALL(effect_stardust, 0, LW(3), LW(7), LW(5), FLOAT(50.0f), FLOAT(50.0f), 20, 120)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 1200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S"), 255, 255, 255, LW(0))
            CALL(evt_mapobj_color, 1, STRING("down_pira"), 255, 255, 255, 255)
            CALL(evt_mapobj_color, 1, STRING("ie_naka"), 255, 255, 255, 255)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 90, 0, 1600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("ie_l"), 0, 0, LW(0))
            MULTIPLY(LW(0), -1)
            CALL(evt_mapobj_rotate, 1, STRING("ie_o"), LW(0), 0, 0)
            CALL(evt_mapobj_rotate, 1, STRING("ie_r"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(bgScrollControll, 1)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_off, 1600, 11)
    WAIT_MS(1400)
    CALL(evt_party_set_breed_pose, 0, 1)
    CALL(evt_sub_intpl_msec_init, 11, -20, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(5))
        CALL(evt_mapobj_trans, 1, STRING("down_pira"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(5), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_Y_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_Y_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("Y_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_Y_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_Y_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_Y_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_Y_1"))
    END_SWITCH
    WAIT_MS(1500)
    CALL(evt_party_set_breed_pose, 0, 1)
    CALL(evt_mario_dispflag_onoff, 0, 4)
    DELETE_EVENT(GW(1))
    CALL(enzan_lw_load)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(unitwin_getpartylank, LW(1), LW(0))
    ADD(LW(0), 1)
    CALL(unitwin_setpartylank, LW(1), LW(0))
    CALL(unitwin_getpartylank, LW(2), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(unitwin_setpartylank, LW(2), LW(0))
    CALL(evt_msg_print, 0, STRING("powerdown_13"), 0, STRING("me"))
    RETURN
EVT_END()

// TODO: write setmertn (static, 0x40 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(powerdownya_table_talk)
    CALL(setmertn, STRING("パワーダウン屋"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    IF_INT_EQUAL(GSWF(1342), 0)
        CALL(evt_msg_print_insert, 0, STRING("powerdown_00"), 0, STRING("me"), 39)
        SET_VALUE(GSWF(1342), 1)
        SET_VALUE(GSWF(1371), 1)
    ELSE
        IF_INT_EQUAL(GSWF(1371), 0)
            CALL(evt_msg_print, 0, STRING("powerdown_00_01"), 0, STRING("me"))
            SET_VALUE(GSWF(1371), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("powerdown_01"), 0, STRING("me"))
        END_IF
    END_IF
    CALL(evt_msg_select, 0, STRING("powerdown_02"))
    IF_INT_EQUAL(LW(0), 2)
        CALL(evt_msg_print_add, 0, STRING("powerdown_03"))
        GOTO(0)
    END_IF
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(powerdownya_mario_level_rtn)
    ELSE
        CALL_CHILD(powerdownya_party_lank_rtn)
    END_IF
    LABEL(0)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(powerdownya_direct_talk)
    CALL(evt_msg_print, 0, STRING("powerdown_14"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(powerdownya_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_power_down"), 0, PTR(powerdownya_table_talk), 0)
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[46] = {
    (u32)"パワーダウン屋", 0x00000600, 0x00000000, (u32)&powerdownya_init,
    0x00000000, (u32)&powerdownya_direct_talk, 0x00000000, 0x00000000,
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

static BeroEntry bero_entry_data[2] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_01", "e_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(tik_11_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(GSWF(1371), 0)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_map_playanim, STRING("anm_kem"), 1, 0)
    RETURN
EVT_END()

// TODO: type
static u32 xlw[16] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: write enzan_set_f_i (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write enzan_set_i_f (static, 0x54 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write enzan_cam_turn_keisan (static, 0x1f0 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write wariai_enzan_0f (static, 0xcc bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write wariai_enzan_0 (static, 0xd8 bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it

// TODO: write wariai_enzan_1 (static, 0xcc bytes) here:
// its asm is in build/G8MJ01/tik/asm/tik_11.s; this is where the original has it, just before the first script that uses it
// clang-format on

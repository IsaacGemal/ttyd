#include "kpa.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_twoddisp.h"
#include "forcedata.h"
#include "kpa_act.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
s32 kpa_stage_clear[35];

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write kpa_mobj_on (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_init)
    CALL(evt_mario_set_mode, 2)
    CALL(evt_mario_kill_party, 0)
    CALL(kpa_mobj_on)
    CALL(kpa_score_disp_init)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("kpa_replay"))
        SET_VALUE(GSW(1222), 180)
    ELSE
        IF_STR_NOT_EQUAL(LW(0), STRING("kpa_next_map"))
            SET_VALUE(GSW(1221), 1)
            SET_VALUE(GSW(1222), 180)
        END_IF
    END_IF
    CALL(evt_cam_type_change, 1, 4)
    SET_VALUE(LSWF(2), 0)
    SET_VALUE(LSWF(3), 0)
    SET_VALUE(LSWF(4), 0)
    SET_VALUE(LSWF(5), 0)
    RETURN
EVT_END()

// TODO: write kpa_make_mobj_name (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_read_mobj)
    SET_INT_READ_ADDR(LW(0))
    LOOP(100)
        READ_INT(LW(1))
        IF_INT_EQUAL(LW(1), -1)
            BREAK_LOOP
        END_IF
        CALL(kpa_make_mobj_name, LW(2))
        SWITCH(LW(1))
            CASE_EQUAL(0)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_ojama_blk, LW(2), LW(3), LW(4), 0, 0, 0, STRING("MOBJ_Lv1BigBlock"))
            CASE_EQUAL(1)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_stone_blk, LW(2), LW(3), LW(4), 0, 0, 0, STRING("MOBJ_Lv2BigBlock"))
            CASE_EQUAL(2)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 0, 0, 0)
            CASE_EQUAL(4)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 2, 0, 0)
            CASE_EQUAL(3)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 1, 0, 0)
            CASE_EQUAL(5)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 3, 0, 0)
            CASE_EQUAL(6)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_koopa_badgeblk, LW(2), LW(3), LW(4), 0, LW(5), 0, 0, 0)
            CASE_EQUAL(7)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_koopa_hidden_blk, LW(2), LW(3), LW(4), 0, LW(5), 1, 0, 0)
            CASE_EQUAL(8)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_blk, LW(2), LW(3), LW(4), 0, 0, 0, 0)
            CASE_EQUAL(9)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_dokan, 0, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(10)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_dokan, 1, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(11)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_dokan, 2, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(12)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_sango, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(13)
            READ_INT4(LW(3), LW(4), LW(5), LW(6))
            CALL(evt_mobj_koopa_fireber_dodai, 0, LW(2), LW(3), LW(4), 0, LW(5), LW(6), 0, 0)
            CASE_EQUAL(14)
            READ_INT4(LW(3), LW(4), LW(5), LW(6))
            CALL(evt_mobj_koopa_fireber_dodai, 1, LW(2), LW(3), LW(4), 0, LW(5), LW(6), 0, 0)
            CASE_EQUAL(15)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_koopa_pole, STRING("pole"), LW(3), LW(4), 0, LW(5), PTR(kpa_stage_clear), LSWF(3))
            CASE_EQUAL(16)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_jumpstand_red, 0, LW(2), LW(3), LW(4), 0, LW(5), 0)
            CASE_EQUAL(17)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_jumpstand_blue, 0, LW(2), LW(5), LW(3), LW(4), 0, 0, 0)
        END_SWITCH
    END_LOOP
    RETURN
EVT_END()

// TODO: write kpa_add_coin (global, 0x90 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_coin_evt)
    CALL(kpa_add_coin, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

// TODO: write kpa_make_item_name (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_read_item)
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT(LW(1))
        IF_INT_EQUAL(LW(1), -1)
            BREAK_LOOP
        END_IF
        CALL(kpa_make_item_name, LW(2))
        READ_INT2(LW(3), LW(4))
        CALL(evt_item_entry, LW(2), 121, LW(3), LW(4), 0, 16, -1, PTR(kpa_coin_evt))
        CALL(evt_item_flag_on, LW(2), 4)
    END_LOOP
    RETURN
EVT_END()

// TODO: write unk_kpa_00000a08 (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_nohitenemy_ct_clear (global, 0x2c bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_status_get_stage (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_dead)
    IF_INT_EQUAL(LSWF(3), 1)
        RETURN
    END_IF
    SET_VALUE(LSWF(3), 1)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(unk_kpa_00000a08)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_adjust_dir)
    CALL(kpa_nohitenemy_ct_clear)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_set_motion, 0)
    CALL(kpa_chg_pose, STRING("KPA2_D_4"))
    SET_VALUE(LSWF(4), 1)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
    SET_FLOAT_VALUE(LW(3), LW(5))
    SET_FLOAT_VALUE(LW(0), LW(3))
    SET_FLOAT_VALUE(LW(1), FLOAT(8.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(-0.14f))
    CALL(evt_koopa_chk_dead, LW(7))
    IF_INT_EQUAL(LW(7), 1)
        MULTIPLY_FLOAT(LW(1), FLOAT(0.7f))
    END_IF
    SUBTRACT_FLOAT(LW(3), FLOAT(200.0f))
    CALL(evt_sub_get_mapname, LW(15))
    IF_STR_EQUAL(LW(15), STRING("kpa_03"))
        SET_VALUE(LW(5), 150)
    ELSE
        SET_VALUE(LW(5), 155)
    END_IF
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(1), 0)
            SET_FLOAT_VALUE(LF(0), 0)
        ELSE
            SET_FLOAT_VALUE(LF(0), 1)
        END_IF
        ADD_FLOAT(LW(0), LW(1))
        ADD_FLOAT(LW(1), LW(2))
        IF_INT_LESS(LW(1), 0)
            IF_INT_EQUAL(LF(0), 0)
            END_IF
        END_IF
        IF_INT_LESS(LW(0), LW(3))
            BREAK_LOOP
        END_IF
        CALL(evt_mario_set_pos, LW(4), LW(0), LW(5))
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_LESS(GSW(1221), 99)
        ADD(GSW(1221), 1)
    END_IF
    CALL(kpa_status_get_stage, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(1), STRING("kpa_02"))
        CASE_EQUAL(2)
        SET_VALUE(LW(1), STRING("kpa_03"))
        CASE_EQUAL(3)
        SET_VALUE(LW(1), STRING("kpa_05"))
    END_SWITCH
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_cam_letter_box_disable, 0)
    END_INLINE_EVENT
    CALL(evt_bero_mapchange, LW(1), STRING("kpa_replay"))
    RETURN
EVT_END()

EVT_BEGIN(kpa_fall_sub)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_get_pos, 0, LW(13), LW(14), LW(15))
    LOOP(0)
        CALL(evt_mario_get_name_hitobj_ride, LW(0))
        IF_STR_NOT_EQUAL(LW(0), STRING(""))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL_CHILD(kpa_dead)
    RETURN
EVT_END()

// TODO: write kpa_muteki_check (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_bgm_change)
    IF_INT_GREATER(GSW(1222), 30)
        SET_VALUE(LW(2), 0)
    ELSE
        SET_VALUE(LW(2), 1)
    END_IF
    SET_VALUE(LW(3), -1)
    SET_VALUE(LW(4), -1)
    LABEL(10)
    IF_INT_GREATER(GSW(1222), 30)
        SET_VALUE(LW(3), 0)
        ADD(LW(3), LSW(1))
    ELSE
        IF_INT_EQUAL(LW(2), 0)
            SET_VALUE(LW(3), 3)
            ADD(LW(3), LSW(1))
        ELSE
            SET_VALUE(LW(3), 6)
        END_IF
    END_IF
    CALL(kpa_muteki_check)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(3), 7)
        SET_VALUE(LW(2), 0)
    END_IF
    IF_INT_EQUAL(LSWF(4), 1)
        SET_VALUE(LW(3), 8)
    END_IF
    IF_INT_EQUAL(LSWF(5), 1)
        SET_VALUE(LW(3), 9)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), LW(4))
        SET_VALUE(LW(4), LW(3))
        SWITCH(LW(3))
            CASE_EQUAL(0)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD3"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(2)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD4"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(3)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD1_UP1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(4)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD3_UP1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(5)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD4_UP1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(6)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD1_UP2"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(7)
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_bgmon, 513, STRING("BGM_STGK_FIELD2"))
            CASE_EQUAL(8)
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_KUPPA_DOWN1"))
            CALL(evt_snd_bgmoff, 513)
            RETURN
            CASE_EQUAL(9)
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_KUPPA_GOAL1"))
            CALL(evt_snd_bgmoff, 513)
            RETURN
        END_SWITCH
        WAIT_MS(300)
    END_IF
    WAIT_FRAMES(1)
    GOTO(10)
    RETURN
EVT_END()

// TODO: write kpa_get_msg (global, 0x8c bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_set_scale (global, 0xb8 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_stage_start)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff, 512)
    CALL(evt_map_blend_set_flag, 1, STRING("mario"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("mario"), 4096)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 255)
    CALL(evt_map_set_blend, 1, 0, 0, 0, 255)
    CALL(kpa_set_scale, FLOAT(0.4f))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    ADD(LW(1), 25)
    ADD(LW(2), 400)
    CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(0), LW(1), 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_dispdir, 180)
    CALL(evt_seq_wait, 2)
    CALL(evt_twoddisp_init)
    CALL(evt_twoddisp_entry, STRING("batu"), 469)
    CALL(evt_twoddisp_set_base_position, STRING("batu"), 20, -50)
    CALL(evt_twoddisp_set_position, STRING("batu"), 10, 0)
    CALL(evt_twoddisp_entry, STRING("mugen"), 526)
    CALL(evt_twoddisp_set_base_position, STRING("mugen"), 20, -50)
    CALL(evt_twoddisp_set_position, STRING("mugen"), 54, 3)
    CALL(kpa_get_msg, LW(0))
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA2_S_1"), STRING("KPA2_T_1"))
    CALL(evt_msg_print, 0, LW(0), 0, STRING("mario"))
    CALL(evt_fade_out, 1000)
    SPAWN_EVENT(kpa_bgm_change)
    CALL(evt_snd_envoff, 512)
    CALL(evt_map_blend_off, 0)
    CALL(evt_map_blend_off, 1)
    CALL(evt_twoddisp_flag_onoff, 0, STRING("batu"), 1)
    CALL(evt_twoddisp_flag_onoff, 0, STRING("mugen"), 1)
    CALL(kpa_set_scale, FLOAT(1.0f))
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_fade_in, 1000)
    SET_VALUE(LSWF(2), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

// TODO: type
u32 numeric_id[5] = {
    0x01D901DB, 0x01DD01DF, 0x01E101E3, 0x01E501E7,
    0x01E901EB,
};

// TODO: write kpa_status_main (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_status_main_evt)
    LOOP(0)
        CALL(kpa_status_main)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: write kpa_pole_hitoff (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_set_time (global, 0x80 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_time)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 180)
    SUBTRACT(LW(1), GSW(1222))
    SET_VALUE(LW(3), LW(1))
    DIVIDE(LW(1), 10)
    MODULO(LW(3), 10)
    MULTIPLY(LW(3), 1000)
    SET_VALUE(LF(0), 0)
    LOOP(0)
        IF_INT_NOT_EQUAL(LSWF(2), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(0)
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            IF_INT_NOT_EQUAL(LSWF(3), 0)
                RETURN
            END_IF
            CALL(evt_sub_get_stopwatch, LW(0), LW(2))
            IF_INT_EQUAL(LF(0), 0)
                ADD(LW(2), LW(3))
            END_IF
            CALL(kpa_set_time, LW(2), LW(1), GSW(1222))
            IF_INT_GREATER_OR_EQUAL(LW(2), 10000)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LF(0), 1)
        ADD(LW(1), 1)
        IF_INT_GREATER_OR_EQUAL(LW(1), 18)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(kpa_pole_hitoff)
    CALL_CHILD(kpa_dead)
    RETURN
EVT_END()

// TODO: write kpa_time_countdown (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kpa_stage_clear)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_sfxon, STRING("SFX_KUPPA_COUNTER1"), LW(0))
    CALL(kpa_time_countdown)
    CALL(evt_snd_sfxoff, LW(0))
    CALL(evt_eff, 0, STRING("stageclear"), 1, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(3500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: label$603 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 label$603[4] = {
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
// };

// TODO: no$581 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 no$581[1] = {
//     0x00000000,
// };

// TODO: name$580 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 name$580[4] = {
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
// };

// TODO: no$573 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 no$573[1] = {
//     0x00000000,
// };

// TODO: name$572 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 name$572[4] = {
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
// };

// TODO: type
u32 kwp[1] = {
    0x00000000,
};

// TODO: write kpa_get_cam_pt (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_stageclear_disp_entry (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_stageclear_disp (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_status_init (global, 0x100 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write kpa_status_disp (global, 0x464 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0xec bytes) here:
// its asm is in build/G8MJ01/kpa/asm/kpa.s; this is where the original has it, just before the first script that uses it
// clang-format on

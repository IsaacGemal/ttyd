#include "yuu_02.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_movefloor.h"
#include "evt/evt_msg.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_yuugijou.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 eff_call_minigame[];
extern s32 evt_yuucounter_alloc[];
extern s32 evt_yuucounter_free[];
extern s32 evt_yuuminigame_alloc[];
extern s32 evt_yuunpc_minigame3[];
extern s32 evt_yuuranking_alloc_31_text_52F8[];
extern s32 evt_yuuranking_free_31_text_5274[];
extern s32 evt_yuutitle_alloc[];
extern s32 evt_yuutitle_free[];
extern s32 yuu_atai_init[];
extern s32 yuu_atai_pop[];
extern s32 yuu_atai_push[];
extern s32 yuuminigame_get_dataadr[];
extern s32 yuuminigame_set_dataadr[];
extern s32 yuuranking_get_monte_31_text_51C0[];
extern s32 yuuranking_wait_button_31_text_51F8[];
static USER_FUNC(re_fall_rtn);
static USER_FUNC(fall_rtn);
static void monte_disp(void);  // TODO: real signature
static void get_monte_check(void);  // TODO: real signature

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

// TODO: type
u32 fukkipoint[45] = {
    0x43960000, 0x00000000, 0x43960000, 0x443B8000,
    0x00000000, 0x43960000, 0x448FC000, 0x00000000,
    0x43960000, 0x44BB8000, 0x00000000, 0x43960000,
    0x44C1C000, 0x00000000, 0xC3960000, 0x44674000,
    0x43480000, 0xC3960000, 0x437A0000, 0x43480000,
    0xC3960000, 0xC37A0000, 0x43480000, 0xC3960000,
    0xC46D8000, 0x43480000, 0xC3960000, 0xC4B54000,
    0x00000000, 0xC3960000, 0xC4C1C000, 0x00000000,
    0x43960000, 0xC46D8000, 0x00000000, 0x43960000,
    0xC37A0000, 0x00000000, 0x43960000, 0x437A0000,
    0x00000000, 0x43960000, 0x437A0000, 0x00000000,
    0x43960000,
};

// TODO: write unk_yuu_00000f1c (global, 0x138 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(unk_evt_yuu_000152bc)
    CALL(unk_yuu_00000f1c)
    RETURN
EVT_END()

// TODO: type
static u32 montetable[66] = {
    0x44BB8000, 0x44778000, 0xC3480000, 0xC3960000,
    0x00000001, 0x00000001, 0x44BB8000, 0x44778000,
    0xC3960000, 0xC3C80000, 0x00000001, 0x00000002,
    0x43960000, 0x44610000, 0xC37A0000, 0xC3898000,
    0x00000001, 0x00000001, 0x43960000, 0x44610000,
    0xC3A28000, 0xC3AF0000, 0x00000001, 0x00000002,
    0x43160000, 0xC3160000, 0xC3820000, 0xC3AF0000,
    0x00000001, 0x00000002, 0xC485C000, 0xC4A64000,
    0xC3610000, 0xC3BB8000, 0x00000001, 0x00000002,
    0xC46D8000, 0xC46D8000, 0x43960000, 0x43960000,
    0x00000001, 0x00000001, 0xC49C4000, 0xC49C4000,
    0x43960000, 0x43960000, 0x00000001, 0x00000001,
    0xC4A8C000, 0xC4A8C000, 0x43960000, 0x43960000,
    0x00000001, 0x00000001, 0xC4B54000, 0xC4B54000,
    0x43960000, 0x43960000, 0x00000001, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: write fall_rtn (static, 0x5ac bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(minigame_view_monte)
    CALL(fall_rtn)
    RETURN
EVT_END()

// TODO: write yuu_roll_reset (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(yuu_02_jumpret)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_cam_get_pos, 4, LW(7), LW(8), LW(9))
    CALL(evt_cam_get_at, 4, LW(10), LW(11), LW(12))
    CALL(evt_cam3d_evt_set, LW(7), LW(8), LW(9), LW(10), LW(11), LW(12), 0, 0)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        IF_INT_LESS_OR_EQUAL(LW(5), -400)
            CALL(evt_mario_cont_onoff, 0)
            CALL(evt_mario_dispflag_onoff, 1, 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_set_pos, LW(0), LW(3), LW(2))
    CALL(yuu_roll_reset)
    CALL(evt_mario_cam_y_on)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 1000, 0)
    WAIT_MS(1000)
    CALL(evt_cam_get_pos, 4, LW(7), LW(8), LW(9))
    CALL(evt_cam_get_at, 4, LW(10), LW(11), LW(12))
    CALL(evt_cam3d_evt_set, LW(7), LW(8), LW(9), LW(10), LW(11), LW(12), 0, 0)
    CALL(evt_mario_cam_y_off)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(evt_mario_set_pos, LW(0), LW(4), LW(2))
        CALL(evt_mario_get_name_hitobj_ride, LW(6))
        IF_STR_NOT_EQUAL(LW(6), 0)
            CALL(evt_eff, STRING(""), STRING("kemuri"), 0, LW(0), LW(4), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_cam3d_evt_off, 500, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    RETURN
EVT_END()

EVT_BEGIN(yuu_02_kaijo_init)
    CALL(evt_mario_set_pos, 200, 0, 300)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(yuu_02_kaijo_init2)
    CALL(evt_mario_set_pos, 200, 0, 300)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

// TODO: write yuuminigame3_set_phase (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame03_main)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(yuu_02_kaijo_init)
    CALL(evt_yuuminigame_alloc, LW(0), 2)
    CALL(yuuminigame_set_dataadr, 0, LW(0))
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(yuuminigame_get_dataadr, 0, LW(0))
    CALL(yuuminigame3_set_phase, LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(minigame03_start_cam)
    CALL(evt_cam3d_evt_set, FLOAT(-1600.0f), FLOAT(573.0f), FLOAT(1333.0f), FLOAT(-1600.0f), FLOAT(12.0f), FLOAT(-89.0f), 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, FLOAT(1600.0f), FLOAT(573.0f), FLOAT(1333.0f), FLOAT(1600.0f), FLOAT(12.0f), FLOAT(-89.0f), 8000, 0)
    WAIT_MS(1000)
    CALL(evt_yuutitle_alloc, 0, 0, STRING("menu_title_minigame3"))
    WAIT_MS(5000)
    CALL(evt_yuutitle_free)
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(minigame03_start_cam_cancel)
    CALL(evt_seq_wait, 2)
    CALL(evt_yuutitle_free)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_wait_move_end)
    CALL_CHILD(yuu_02_kaijo_init2)
    RETURN
EVT_END()

// TODO: write yuuminigame3_opening_chk (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame3_opening_on_off (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame3_roll_fix_on (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame03_start)
    CALL(yuu_atai_init)
    CALL(yuu_atai_push, LW(0))
    SET_VALUE(LW(0), STRING("S_ugoku"))
    SET_VALUE(LW(1), STRING("ugo1"))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), -200)
    SET_VALUE(LW(4), 2400)
    SET_VALUE(LW(5), 600)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 1, LW(15))
    SET_VALUE(LW(0), STRING("S_ugoku1"))
    SET_VALUE(LW(1), STRING("ugo2"))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 200)
    SET_VALUE(LW(4), 2400)
    SET_VALUE(LW(5), 600)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 2, LW(15))
    CALL(yuu_atai_pop, LW(0))
    CALL(yuuminigame3_opening_chk, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT_GET_ID(PTR(minigame03_start_cam), LW(8))
        LOOP(0)
            IS_EVENT_RUNNING(LW(8), LW(9))
            IF_INT_EQUAL(LW(9), 0)
                CALL(evt_seq_wait, 2)
                BREAK_LOOP
            ELSE
                CALL(evt_key_get_buttontrg, 0, LW(9))
                IF_BITS_SET(LW(9), 768)
                    DELETE_EVENT(LW(8))
                    CALL(evt_seq_wait, 2)
                    CALL_CHILD(minigame03_start_cam_cancel)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(yuuminigame3_opening_on_off, LW(0), 0)
        CALL_CHILD(yuu_02_kaijo_init2)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    END_IF
    CALL(evt_mario_mov_pos2, 305, 300, 120)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_MINIGAME1"))
    CALL(eff_call_minigame, 0, 0, 0, 500)
    WAIT_MS(3000)
    CALL(yuuminigame3_roll_fix_on)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_yuucounter_alloc, 0, 70, 1, 180)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write yuuminigame3_roll_fix_off (global, 0x8 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write re_fall_rtn (static, 0xa0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_00001210 (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame03_end)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE2"), 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_LESS(LW(1), -50)
                CALL(evt_mario_jump_pos, 300, 0, 300, 600, 0, -1000)
            END_IF
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(yuuminigame_get_dataadr, 1, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame_get_dataadr, 2, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame3_roll_fix_off)
    IF_INT_LESS(LW(0), 0)
        CALL(eff_call_minigame, 2, 0, 0, 500)
        WAIT_MS(4000)
    ELSE
        CALL(eff_call_minigame, 1, 0, 0, 500)
        WAIT_MS(4000)
    END_IF
    CALL(evt_yuucounter_free)
    CALL(yuugijou_montemedal_disp_onoff, 1)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 2, 0, LW(0), LW(2), LW(1), 180)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(yuuranking_get_monte_31_text_51C0, LW(3))
    CALL(yuugijou_add_montemedal, 0, LW(3))
    CALL(yuugijou_montemedal_wait)
    CALL(evt_yuuranking_free_31_text_5274)
    WAIT_MS(200)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 2, 1, LW(0), LW(2), LW(1), 180)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(evt_yuuranking_free_31_text_5274)
    SET_VALUE(LW(1), 10)
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(0), 1)
    ELSE
        CALL(evt_msg_select, 0, STRING("sysyuu_02_00"))
    END_IF
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GSWF(1229), 1)
        CALL(evt_mario_set_prev_party_pos, 0, 0, -1000, 0)
        CALL(evt_mario_set_prev_party_pos, 1, 0, -1000, 0)
        CALL(evt_bero_mapchange, STRING("gor_03"), STRING("A_dokan_02"))
        CALL(evt_snd_bgmoff, 512)
        RETURN
    END_IF
    MULTIPLY(LW(1), -1)
    CALL(yuugijou_add_montemedal, 0, LW(1))
    CALL(yuugijou_montemedal_wait)
    WAIT_MS(200)
    CALL(yuugijou_montemedal_disp_onoff, 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_sfxoff, GW(10))
    CALL(evt_fade_out, 1000)
    CALL(re_fall_rtn)
    SET_VALUE(GF(0), 0)
    CALL_CHILD(yuu_02_kaijo_init2)
    CALL(unk_yuu_00001210)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(evt_fade_in, 1000)
    RETURN
EVT_END()

EVT_BEGIN(s_en1_turn)
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(0.75f))
    LOOP(0)
        ADD_FLOAT(LW(0), LW(1))
        IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
        END_IF
        CALL(evt_mapobj_rotate, 1, STRING("S_en1"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yuu_02_init_evt)
    CALL(evt_map_playanim, STRING("den1"), 1, 0)
    CALL(evt_map_playanim, STRING("den2"), 1, 0)
    CALL(evt_map_playanim, STRING("kumo"), 1, 0)
    CALL(evt_map_playanim, STRING("wakka"), 1, 0)
    SPAWN_EVENT(s_en1_turn)
    SPAWN_EVENT(minigame03_main)
    CALL_CHILD(evt_yuunpc_minigame3)
    RETURN
EVT_END()

// TODO: type
static u32 monte_p[1] = {
    0x00000000,
};

// TODO: type
u32 unk_yuu_bss00000004[1] = {
    0x00000000,
};

// TODO: type
static u32 yuunpc_wp[1] = {
    0x00000000,
};

// TODO: write yuuminigame3_gamerule (global, 0x39c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_disp_3roll (global, 0x20 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_main_3roll (global, 0x160 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_init_3roll (global, 0x8c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write monte_disp (static, 0x78 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write get_monte_check (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_00001054 (global, 0x10c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_00001160 (global, 0x80 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_000011e0 (global, 0x18 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it

// TODO: write fn_31_112C (global, 0x18 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_02.s; this is where the original has it, just before the first script that uses it
// clang-format on

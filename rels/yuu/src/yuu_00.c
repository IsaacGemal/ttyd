#include "yuu_00.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
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
extern s32 evt_yuunpc_minigame1[];
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
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 montetable[20] = {
    0xC4960000, 0xC4480000, 0x43E10000, 0x43480000,
    0x00000002, 0xC4480000, 0xC3960000, 0x43960000,
    0x42C80000, 0x00000002, 0xC3960000, 0x43480000,
    0x43480000, 0x42C80000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: write fall_rtn (static, 0x3ac bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(minigame_view_monte)
    CALL(fall_rtn)
    RETURN
EVT_END()

EVT_BEGIN(yuu_00_kaijo_init)
    CALL(evt_mario_set_pos, -1550, 400, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(yuu_00_kaijo_init2)
    CALL(evt_mario_set_pos, -1350, 400, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(light_sub_event)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 96)
        LOOP(0)
            WAIT_FRAMES(1)
            IF_INT_NOT_EQUAL(GF(0), 1)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(0), 30)
                SET_VALUE(LW(1), 1)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 35)
            END_IF
            IF_INT_EQUAL(LW(0), 60)
                SET_VALUE(LW(1), 2)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 35)
            END_IF
            IF_INT_EQUAL(LW(0), 78)
                SET_VALUE(LW(1), 3)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 0)
            END_IF
            IF_INT_EQUAL(LW(0), 96)
                SET_VALUE(LW(0), 0)
                SET_VALUE(LW(1), 0)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 0)
            END_IF
            SWITCH(LW(1))
                CASE_EQUAL(0)
                SET_VALUE(LW(3), 35)
                SUBTRACT(LW(3), LW(5))
                MULTIPLY(LW(3), LW(2))
                DIVIDE(LW(3), 30)
                ADD(LW(3), LW(5))
                CALL(evt_mapobj_rotate, 1, STRING("light01"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_b"), 0, 0, LW(3))
                CASE_EQUAL(1)
                SET_VALUE(LW(3), 35)
                CALL(evt_mapobj_rotate, 1, STRING("light01"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_b"), 0, 0, LW(3))
                CASE_EQUAL(2)
                SET_VALUE(LW(3), 0)
                SUBTRACT(LW(3), LW(5))
                MULTIPLY(LW(3), LW(2))
                DIVIDE(LW(3), 18)
                ADD(LW(3), LW(5))
                CALL(evt_mapobj_rotate, 1, STRING("light01"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_b"), 0, 0, LW(3))
                CASE_EQUAL(3)
                SET_VALUE(LW(3), 0)
                CALL(evt_mapobj_rotate, 1, STRING("light01"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_b"), 0, 0, LW(3))
            END_SWITCH
            ADD(LW(0), 1)
            ADD(LW(2), 1)
        END_LOOP
        WAIT_MS(300)
        SET_VALUE(LW(2), 0)
        SET_VALUE(LW(5), LW(3))
        LOOP(0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(2), 18)
                BREAK_LOOP
            END_IF
            SET_VALUE(LW(3), 0)
            SUBTRACT(LW(3), LW(5))
            MULTIPLY(LW(3), LW(2))
            DIVIDE(LW(3), 18)
            ADD(LW(3), LW(5))
            CALL(evt_mapobj_rotate, 1, STRING("light01"), 0, 0, LW(3))
            CALL(evt_mapobj_rotate, 1, STRING("light_b"), 0, 0, LW(3))
            ADD(LW(2), 1)
        END_LOOP
        SET_VALUE(LW(3), 0)
        CALL(evt_mapobj_rotate, 1, STRING("light01"), 0, 0, LW(3))
        CALL(evt_mapobj_rotate, 1, STRING("light_b"), 0, 0, LW(3))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 30)
        LOOP(0)
            WAIT_FRAMES(1)
            IF_INT_NOT_EQUAL(GF(0), 1)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(0), 30)
                SET_VALUE(LW(1), 1)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 35)
            END_IF
            IF_INT_EQUAL(LW(0), 60)
                SET_VALUE(LW(1), 2)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 35)
            END_IF
            IF_INT_EQUAL(LW(0), 78)
                SET_VALUE(LW(1), 3)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 0)
            END_IF
            IF_INT_EQUAL(LW(0), 96)
                SET_VALUE(LW(0), 0)
                SET_VALUE(LW(1), 0)
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(5), 0)
            END_IF
            SWITCH(LW(1))
                CASE_EQUAL(0)
                SET_VALUE(LW(3), 35)
                SUBTRACT(LW(3), LW(5))
                MULTIPLY(LW(3), LW(2))
                DIVIDE(LW(3), 30)
                ADD(LW(3), LW(5))
                CALL(evt_mapobj_rotate, 1, STRING("light02"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_c"), 0, 0, LW(3))
                CASE_EQUAL(1)
                SET_VALUE(LW(3), 35)
                CALL(evt_mapobj_rotate, 1, STRING("light02"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_c"), 0, 0, LW(3))
                CASE_EQUAL(2)
                SET_VALUE(LW(3), 0)
                SUBTRACT(LW(3), LW(5))
                MULTIPLY(LW(3), LW(2))
                DIVIDE(LW(3), 18)
                ADD(LW(3), LW(5))
                CALL(evt_mapobj_rotate, 1, STRING("light02"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_c"), 0, 0, LW(3))
                CASE_EQUAL(3)
                SET_VALUE(LW(3), 0)
                CALL(evt_mapobj_rotate, 1, STRING("light02"), 0, 0, LW(3))
                CALL(evt_mapobj_rotate, 1, STRING("light_c"), 0, 0, LW(3))
            END_SWITCH
            ADD(LW(0), 1)
            ADD(LW(2), 1)
        END_LOOP
        WAIT_MS(300)
        SET_VALUE(LW(2), 0)
        SET_VALUE(LW(5), LW(3))
        LOOP(0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(2), 18)
                BREAK_LOOP
            END_IF
            SET_VALUE(LW(3), 0)
            SUBTRACT(LW(3), LW(5))
            MULTIPLY(LW(3), LW(2))
            DIVIDE(LW(3), 18)
            ADD(LW(3), LW(5))
            CALL(evt_mapobj_rotate, 1, STRING("light02"), 0, 0, LW(3))
            CALL(evt_mapobj_rotate, 1, STRING("light_c"), 0, 0, LW(3))
            ADD(LW(2), 1)
        END_LOOP
        SET_VALUE(LW(3), 0)
        CALL(evt_mapobj_rotate, 1, STRING("light02"), 0, 0, LW(3))
        CALL(evt_mapobj_rotate, 1, STRING("light_c"), 0, 0, LW(3))
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: write yuuminigame1_set_phase (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame01_main)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(yuu_00_kaijo_init)
    CALL(evt_yuuminigame_alloc, LW(0), 0)
    CALL(yuuminigame_set_dataadr, 0, LW(0))
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL_CHILD(light_sub_event)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(yuuminigame_get_dataadr, 0, LW(0))
    CALL(yuuminigame1_set_phase, LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(minigame01_start_cam)
    CALL(evt_cam3d_evt_set, FLOAT(-1230.0f), FLOAT(360.0f), FLOAT(1140.0f), FLOAT(-1230.0f), FLOAT(160.0f), FLOAT(-10.0f), 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, FLOAT(700.0f), FLOAT(360.0f), FLOAT(1140.0f), FLOAT(700.0f), FLOAT(160.0f), FLOAT(-10.0f), 6000, 0)
    WAIT_MS(1000)
    CALL(evt_yuutitle_alloc, 0, 0, STRING("menu_title_minigame1"))
    WAIT_MS(3000)
    CALL(evt_yuutitle_free)
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(minigame01_start_cam_cancel)
    CALL(evt_seq_wait, 2)
    CALL(evt_yuutitle_free)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_wait_move_end)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(yuu_00_kaijo_init2)
    RETURN
EVT_END()

// TODO: write yuuminigame1_opening_chk (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame1_opening_on_off (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_change_plane (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame01_start)
    CALL(yuu_atai_init)
    CALL(yuu_atai_push, LW(0))
    CALL(evt_map_playanim, STRING("neon_01"), 1, 0)
    SET_VALUE(LW(0), STRING("S_dai_01"))
    SET_VALUE(LW(1), STRING("A_dai_01"))
    SET_VALUE(LW(2), -150)
    SET_VALUE(LW(3), 150)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), STRING("S_dai_01_k"))
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 1, LW(15))
    SET_VALUE(LW(0), STRING("S_dai_02"))
    SET_VALUE(LW(1), STRING("A_dai_02"))
    SET_VALUE(LW(2), -150)
    SET_VALUE(LW(3), 150)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), STRING("S_dai_02_k"))
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 2, LW(15))
    SET_VALUE(LW(0), STRING("S_dai_03"))
    SET_VALUE(LW(1), STRING("A_dai_03"))
    SET_VALUE(LW(2), -150)
    SET_VALUE(LW(3), 150)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 200)
    SET_VALUE(LW(6), 3)
    SET_VALUE(LW(7), STRING("S_dai_03_k"))
    SET_VALUE(LW(8), -150)
    SET_VALUE(LW(9), 150)
    SET_VALUE(LW(10), 2000)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 3, LW(15))
    SET_VALUE(LW(0), STRING("S_dai_04"))
    SET_VALUE(LW(1), STRING("A_dai_04"))
    SET_VALUE(LW(2), -150)
    SET_VALUE(LW(3), 150)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 200)
    SET_VALUE(LW(6), 4)
    SET_VALUE(LW(7), STRING("S_dai_04_k"))
    SET_VALUE(LW(8), -150)
    SET_VALUE(LW(9), 150)
    SET_VALUE(LW(10), 2000)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 4, LW(15))
    SET_VALUE(LW(0), STRING("S_dai_05"))
    SET_VALUE(LW(1), STRING("A_dai_05"))
    SET_VALUE(LW(2), -150)
    SET_VALUE(LW(3), 150)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 200)
    SET_VALUE(LW(6), 4)
    SET_VALUE(LW(7), STRING("S_dai_05_k"))
    SET_VALUE(LW(8), -150)
    SET_VALUE(LW(9), 150)
    SET_VALUE(LW(10), 2000)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 5, LW(15))
    CALL(yuu_atai_pop, LW(0))
    CALL(yuuminigame1_opening_chk, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT_GET_ID(PTR(minigame01_start_cam), LW(8))
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
                    CALL_CHILD(minigame01_start_cam_cancel)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(yuuminigame1_opening_on_off, LW(0), 0)
        CALL_CHILD(yuu_00_kaijo_init2)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    END_IF
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_MINIGAME1"))
    WAIT_MS(200)
    CALL(evt_mario_set_pose, STRING("M_I_1"))
    WAIT_MS(400)
    WAIT_MS(200)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(600)
    CALL(evt_map_playanim, STRING("neon_01"), 2, 0)
    SET_VALUE(GF(0), 0)
    CALL(eff_call_minigame, 0, 0, 0, 500)
    WAIT_MS(3000)
    CALL(evt_change_plane)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    WAIT_MS(1000)
    CALL(evt_yuucounter_alloc, 0, 70, 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write re_fall_rtn (static, 0xa0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_00004250 (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame01_end)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE2"), 0)
    CALL(yuuminigame_get_dataadr, 1, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame_get_dataadr, 2, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame_get_dataadr, 3, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame_get_dataadr, 4, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame_get_dataadr, 5, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(evt_mario_adjust_dir)
    IF_INT_LESS(LW(0), 0)
        CALL(eff_call_minigame, 2, 0, 0, 500)
        CALL(evt_mario_set_pose, STRING("M_I_S"))
        WAIT_MS(3800)
        CALL(evt_mario_set_pose, STRING("M_D_2"))
        WAIT_MS(200)
    ELSE
        CALL(eff_call_minigame, 1, 0, 0, 500)
        WAIT_MS(500)
        IF_INT_LESS(LW(1), 1)
            CALL(evt_mario_set_pose, STRING("M_X_1"))
        ELSE
            IF_INT_GREATER(LW(1), 1)
                CALL(evt_mario_set_pose, STRING("M_V_2"))
            ELSE
                CALL(evt_mario_set_pose, STRING("M_I_1"))
            END_IF
        END_IF
        WAIT_MS(3500)
    END_IF
    CALL(evt_yuucounter_free)
    CALL(yuugijou_montemedal_disp_onoff, 1)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 0, 0, LW(0), LW(2), LW(1), 0)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(yuuranking_get_monte_31_text_51C0, LW(3))
    CALL(yuugijou_add_montemedal, 0, LW(3))
    CALL(yuugijou_montemedal_wait)
    CALL(evt_yuuranking_free_31_text_5274)
    WAIT_MS(200)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 0, 1, LW(0), LW(2), LW(1), 0)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(evt_yuuranking_free_31_text_5274)
    SET_VALUE(LW(1), 10)
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(0), 1)
    ELSE
        CALL(evt_msg_select, 0, STRING("sysyuu_00_00"))
    END_IF
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GSWF(1229), 1)
        CALL(evt_mario_set_prev_party_pos, 0, 0, -1000, 0)
        CALL(evt_mario_set_prev_party_pos, 1, 0, -1000, 0)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_bero_mapchange, STRING("gor_03"), STRING("A_dokan_02"))
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
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(yuu_00_kaijo_init2)
    CALL(unk_yuu_00004250)
    WAIT_MS(500)
    CALL_CHILD(light_sub_event)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(evt_fade_in, 1000)
    RETURN
EVT_END()

EVT_BEGIN(yuu_00_init_evt)
    CALL(evt_map_playanim, STRING("neon_02"), 1, 0)
    SPAWN_EVENT(minigame01_main)
    CALL_CHILD(evt_yuunpc_minigame1)
    RETURN
EVT_END()

// TODO: type
static u32 monte_p[1] = {
    0x00000000,
};

// TODO: type
static u32 yuunpc_wp[1] = {
    0x00000000,
};

// TODO: write yuuminigame1_gamerule (global, 0x21c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_disp_1hikoki (global, 0x20 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_main_1hikoki (global, 0x108 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_init_1hikoki (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write monte_disp (static, 0x78 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it

// TODO: write get_monte_check (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_00.s; this is where the original has it, just before the first script that uses it
// clang-format on

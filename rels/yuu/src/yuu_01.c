#include "yuu_01.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
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
extern s32 evt_yuunpc_minigame2[];
extern s32 evt_yuuranking_alloc_31_text_52F8[];
extern s32 evt_yuuranking_free_31_text_5274[];
extern s32 evt_yuutitle_alloc[];
extern s32 evt_yuutitle_free[];
extern s32 yuuminigame_get_dataadr[];
extern s32 yuuminigame_set_dataadr[];
extern s32 yuuranking_get_monte_31_text_51C0[];
extern s32 yuuranking_wait_button_31_text_51F8[];
static USER_FUNC(re_fall_rtn);
static USER_FUNC(fall_rtn);
static void monte_disp(void);  // TODO: real signature
static void get_monte_check(void);  // TODO: real signature

// clang-format off
extend_data(16,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(17,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

char* s_nametbl[21] = {
    NULL, "S_dai_uzu1", "S_dai_uzu2", "S_dai_uzu3", "S_dai_uzu4", "S_uzu1", "S_uzu2", "S_uzu3",
    "S_uzu4", "S_uzu5", "S_uzu6", "S_uzu7", "S_uzu8", "S_taru01", "S_taru02", "S_taru03",
    "S_taru04", "S_taru05", "S_taru06", "S_taru07", "S_taru08",
};

char* a2_nametbl[21] = {
    NULL, "polySurface439_q00000", "polySurface439_q00001", "polySurface439_q00002",
    "polySurface439_q00003", "polySurface439_q00008", "polySurface439_q00010",
    "polySurface439_q00012", "polySurface439_q00014", "polySurface439_q00016",
    "polySurface439_q00018", "polySurface439_q00020", "polySurface439_q00022", "A_taru01",
    "A_taru02", "A_taru03", "A_taru04", "A_taru05", "A_taru06", "A_taru07", "A_taru08",
};

char* a_nametbl[21] = {
    NULL, "A_dai_uzu1", "A_dai_uzu2", "A_dai_uzu3", "A_dai_uzu4", "A_uzu1", "A_uzu2", "A_uzu3",
    "A_uzu4", "A_uzu5", "A_uzu6", "A_uzu7", "A_uzu8", "A_taru01", "A_taru02", "A_taru03",
    "A_taru04", "A_taru05", "A_taru06", "A_taru07", "A_taru08",
};

// TODO: type
u32 turn_tbl[22] = {
    0x00000000, 0x00000005, 0xFFFFFFFB, 0x00000005,
    0xFFFFFFFB, 0x0000000A, 0xFFFFFFF6, 0x0000000A,
    0xFFFFFFF6, 0x0000000A, 0xFFFFFFF6, 0x0000000A,
    0xFFFFFFF6, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 hoko_tbl[22] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000002D, 0x00000087, 0x0000002D,
    0x00000087, 0x0000002D, 0x0000002D, 0x00000087,
    0x000000B4, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 montetable[30] = {
    0x00000000, 0x00000000, 0x42480000, 0x43AA0000,
    0x00000001, 0x437A0000, 0x437A0000, 0x42480000,
    0x43AA0000, 0x00000001, 0x43E10000, 0x44368000,
    0xC3A50000, 0x42480000, 0x00000001, 0xC31B0000,
    0xC31B0000, 0xC3A50000, 0xC2700000, 0x00000001,
    0x43160000, 0xC31B0000, 0xC3A50000, 0xC2700000,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: write fall_rtn (static, 0x3ac bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(minigame_view_monte)
    CALL(fall_rtn)
    RETURN
EVT_END()

EVT_BEGIN(yuu_01_kaijo_init)
    CALL(evt_mario_set_pos, -800, 0, 200)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(yuu_01_kaijo_init2)
    CALL(evt_mario_set_pos, -800, 0, 200)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

// TODO: write yuuminigame2_set_phase (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame02_main)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(yuu_01_kaijo_init)
    CALL(evt_yuuminigame_alloc, LW(0), 1)
    CALL(yuuminigame_set_dataadr, 0, LW(0))
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_BOAT_WATER1"), GW(11))
    CALL(yuuminigame_get_dataadr, 0, LW(0))
    CALL(yuuminigame2_set_phase, LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(minigame02_start_cam)
    CALL(evt_cam3d_evt_set, FLOAT(-560.0f), FLOAT(565.0f), FLOAT(1185.0f), FLOAT(-560.0f), FLOAT(75.0f), FLOAT(155.0f), 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, FLOAT(550.0f), FLOAT(565.0f), FLOAT(1185.0f), FLOAT(550.0f), FLOAT(75.0f), FLOAT(155.0f), 4000, 0)
    WAIT_MS(500)
    CALL(evt_yuutitle_alloc, 0, 0, STRING("menu_title_minigame2"))
    WAIT_MS(3000)
    CALL(evt_yuutitle_free)
    WAIT_MS(500)
    RETURN
EVT_END()

EVT_BEGIN(minigame02_start_cam_cancel)
    CALL(evt_seq_wait, 2)
    CALL(evt_yuutitle_free)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_wait_move_end)
    CALL_CHILD(yuu_01_kaijo_init2)
    RETURN
EVT_END()

// TODO: write yuuminigame2_opening_chk (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame2_opening_on_off (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame02_start)
    CALL(yuuminigame2_opening_chk, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT_GET_ID(PTR(minigame02_start_cam), LW(8))
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
                    CALL_CHILD(minigame02_start_cam_cancel)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(yuuminigame2_opening_on_off, LW(0), 0)
        CALL_CHILD(yuu_01_kaijo_init2)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    END_IF
    CALL(evt_hitobj_onoff, STRING("taisaku"), 0, 0)
    CALL(evt_mario_mov_pos2, -660, 200, 120)
    CALL(evt_hitobj_onoff, STRING("taisaku"), 0, 1)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_MINIGAME1"))
    CALL(eff_call_minigame, 0, 0, 0, 500)
    WAIT_MS(3000)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_yuucounter_alloc, 0, 70, 1, 120)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write re_fall_rtn (static, 0xa0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_00005268 (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame02_end)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE2"), 0)
    IF_INT_LESS(LW(0), 0)
        CALL(eff_call_minigame, 2, 0, 0, 500)
        WAIT_MS(4000)
    ELSE
        CALL(eff_call_minigame, 1, 0, 0, 500)
        WAIT_MS(4000)
    END_IF
    CALL(evt_yuucounter_free)
    CALL(yuugijou_montemedal_disp_onoff, 1)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 3, 0, LW(0), LW(2), LW(1), 120)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(yuuranking_get_monte_31_text_51C0, LW(3))
    CALL(yuugijou_add_montemedal, 0, LW(3))
    CALL(yuugijou_montemedal_wait)
    CALL(evt_yuuranking_free_31_text_5274)
    WAIT_MS(200)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 3, 1, LW(0), LW(2), LW(1), 120)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(evt_yuuranking_free_31_text_5274)
    SET_VALUE(LW(1), 10)
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(0), 1)
    ELSE
        CALL(evt_msg_select, 0, STRING("sysyuu_01_00"))
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
    CALL(evt_snd_sfxoff, GW(11))
    CALL(evt_fade_out, 1000)
    CALL(re_fall_rtn)
    CALL_CHILD(yuu_01_kaijo_init2)
    CALL(unk_yuu_00005268)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_BOAT_WATER1"), GW(11))
    CALL(evt_fade_in, 1000)
    RETURN
EVT_END()

// TODO: write yuuminigame2_get_name (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(yuu_01_init_evt)
    CALL(evt_map_playanim, STRING("den1"), 1, 0)
    CALL(evt_map_playanim, STRING("den2"), 1, 0)
    CALL(evt_map_playanim, STRING("kumo"), 1, 0)
    CALL(evt_map_playanim, STRING("wakka"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("nagare"), 1, 0)
    CALL(yuuminigame2_get_name, 1, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -140)
    SET_VALUE(LW(3), 140)
    SET_VALUE(LW(4), 1500)
    SET_VALUE(LW(5), 100)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 1, LW(15))
    CALL(yuuminigame2_get_name, 2, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -220)
    SET_VALUE(LW(3), 50)
    SET_VALUE(LW(4), 3500)
    SET_VALUE(LW(5), 200)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 2, LW(15))
    CALL(yuuminigame2_get_name, 3, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), 20)
    SET_VALUE(LW(3), -60)
    SET_VALUE(LW(4), 800)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 4)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 115)
    SET_VALUE(LW(9), -40)
    SET_VALUE(LW(10), 1600)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 3, LW(15))
    CALL(yuuminigame2_get_name, 4, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -50)
    SET_VALUE(LW(3), 0)
    SET_VALUE(LW(4), 800)
    SET_VALUE(LW(5), 100)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 4, LW(15))
    CALL(yuuminigame2_get_name, 5, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -75)
    SET_VALUE(LW(3), 75)
    SET_VALUE(LW(4), 1500)
    SET_VALUE(LW(5), 400)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 5, LW(15))
    CALL(yuuminigame2_get_name, 6, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), 75)
    SET_VALUE(LW(3), -75)
    SET_VALUE(LW(4), 1500)
    SET_VALUE(LW(5), 400)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 6, LW(15))
    CALL(yuuminigame2_get_name, 7, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), 75)
    SET_VALUE(LW(3), 0)
    SET_VALUE(LW(4), 1500)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 7, LW(15))
    CALL(yuuminigame2_get_name, 8, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -75)
    SET_VALUE(LW(3), 40)
    SET_VALUE(LW(4), 1500)
    SET_VALUE(LW(5), 100)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 8, LW(15))
    CALL(yuuminigame2_get_name, 9, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), 120)
    SET_VALUE(LW(3), -40)
    SET_VALUE(LW(4), 2800)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 9, LW(15))
    CALL(yuuminigame2_get_name, 10, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -40)
    SET_VALUE(LW(3), 60)
    SET_VALUE(LW(4), 2800)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 10, LW(15))
    CALL(yuuminigame2_get_name, 11, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), 40)
    SET_VALUE(LW(3), -50)
    SET_VALUE(LW(4), 2800)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 11, LW(15))
    CALL(yuuminigame2_get_name, 12, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -40)
    SET_VALUE(LW(3), 40)
    SET_VALUE(LW(4), 2800)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 2)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 12, LW(15))
    CALL(yuuminigame2_get_name, 13, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -20)
    SET_VALUE(LW(3), 75)
    SET_VALUE(LW(4), 3600)
    SET_VALUE(LW(5), 400)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 13, LW(15))
    CALL(yuuminigame2_get_name, 14, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -75)
    SET_VALUE(LW(3), 75)
    SET_VALUE(LW(4), 4000)
    SET_VALUE(LW(5), 800)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 14, LW(15))
    CALL(yuuminigame2_get_name, 15, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -70)
    SET_VALUE(LW(3), 75)
    SET_VALUE(LW(4), 4000)
    SET_VALUE(LW(5), 600)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 15, LW(15))
    CALL(yuuminigame2_get_name, 16, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), 75)
    SET_VALUE(LW(3), -75)
    SET_VALUE(LW(4), 4000)
    SET_VALUE(LW(5), 600)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 16, LW(15))
    CALL(yuuminigame2_get_name, 17, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -40)
    SET_VALUE(LW(3), 100)
    SET_VALUE(LW(4), 3000)
    SET_VALUE(LW(5), 300)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 17, LW(15))
    CALL(yuuminigame2_get_name, 18, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 240)
    SET_VALUE(LW(4), 4000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 18, LW(15))
    CALL(yuuminigame2_get_name, 19, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -60)
    SET_VALUE(LW(3), 0)
    SET_VALUE(LW(4), 1000)
    SET_VALUE(LW(5), 100)
    SET_VALUE(LW(6), 2)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 19, LW(15))
    CALL(yuuminigame2_get_name, 20, LW(0), LW(1), LW(11))
    SET_VALUE(LW(0), LW(0))
    SET_VALUE(LW(1), LW(1))
    SET_VALUE(LW(2), -25)
    SET_VALUE(LW(3), 75)
    SET_VALUE(LW(4), 3000)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), LW(11))
    SET_VALUE(LW(12), 1)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 20, LW(15))
    SPAWN_EVENT(minigame02_main)
    CALL_CHILD(evt_yuunpc_minigame2)
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

// TODO: write yuuminigame2_gamerule (global, 0x750 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_disp_2fune (global, 0x20 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_main_2fune (global, 0x144 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_init_2fune (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

// TODO: write monte_disp (static, 0x78 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it

// TODO: write get_monte_check (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_01.s; this is where the original has it, just before the first script that uses it
// clang-format on

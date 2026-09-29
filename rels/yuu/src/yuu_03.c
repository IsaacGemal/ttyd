#include "yuu_03.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
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
extern s32 evt_yuunpc_minigame4[];
extern s32 evt_yuuranking_alloc_31_text_52F8[];
extern s32 evt_yuuranking_free_31_text_5274[];
extern s32 evt_yuutitle_alloc[];
extern s32 evt_yuutitle_free[];
extern s32 yuuminigame_get_dataadr[];
extern s32 yuuminigame_set_dataadr[];
extern s32 yuuranking_get_monte_31_text_51C0[];
extern s32 yuuranking_wait_button_31_text_51F8[];
static void randf(void);  // TODO: real signature
static void polygon(void);  // TODO: real signature
static void evt_nomal_cange(void);  // TODO: real signature

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

// TODO: write unk_yuu_000034bc (global, 0x1a0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(unk_evt_yuu_00015fe0)
    CALL(unk_yuu_000034bc)
    RETURN
EVT_END()

// TODO: write evt_ryushi (global, 0x73c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_ryushi_clr (global, 0x32c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(ryushi_event)
    SET_VALUE(GW(0), 0)
    CALL(evt_ryushi_clr, LW(1))
    LOOP(0)
        CALL(evt_ryushi)
        IF_INT_EQUAL(GW(0), 1)
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(GW(0), 2)
            RETURN
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yuu_03_kaijo_init)
    CALL(evt_mario_set_pos, -1650, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(yuu_03_kaijo_init2)
    CALL(evt_mario_set_pos, -1520, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

// TODO: write yuuminigame4_set_phase (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame04_main)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(yuu_03_kaijo_init)
    CALL(evt_yuuminigame_alloc, LW(0), 3)
    CALL(yuuminigame_set_dataadr, 0, LW(0))
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(yuuminigame_get_dataadr, 0, LW(0))
    CALL(yuuminigame4_set_phase, LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(minigame04_start_cam)
    CALL(evt_cam3d_evt_set, FLOAT(-1520.0f), FLOAT(335.0f), FLOAT(850.0f), FLOAT(-1520.0f), FLOAT(65.0f), FLOAT(-25.0f), 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, FLOAT(1285.0f), FLOAT(334.0f), FLOAT(850.0f), FLOAT(1285.0f), FLOAT(65.0f), FLOAT(-25.0f), 10000, 0)
    WAIT_MS(1000)
    CALL(evt_yuutitle_alloc, 0, 0, STRING("menu_title_minigame4"))
    WAIT_MS(7000)
    CALL(evt_yuutitle_free)
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(minigame04_start_cam_cancel)
    CALL(evt_seq_wait, 2)
    CALL(evt_yuutitle_free)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_wait_move_end)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(yuu_03_kaijo_init2)
    RETURN
EVT_END()

// TODO: write yuuminigame4_opening_chk (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame4_opening_on_off (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write yuunpc_go (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame04_start)
    CALL(yuuminigame4_opening_chk, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT_GET_ID(PTR(minigame04_start_cam), LW(8))
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
                    CALL_CHILD(minigame04_start_cam_cancel)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(yuuminigame4_opening_on_off, LW(0), 0)
        CALL_CHILD(yuu_03_kaijo_init2)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    END_IF
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_MINIGAME1"))
    CALL(eff_call_minigame, 0, 0, 0, 500)
    WAIT_MS(3000)
    CALL(evt_mario_set_enable_key, 352)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(yuunpc_go)
    CALL(evt_yuucounter_alloc, 0, 70, 2, 701)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write re_init_yuunpc (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_0000371c (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(minigame04_end)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_cam3d_evt_set_now)
    CALL(evt_mario_set_enable_key, 0)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE2"), 0)
    SET_VALUE(GW(0), 1)
    SET_VALUE(GF(0), 1)
    SET_VALUE(LW(3), 1)
    BEGIN_INLINE_EVENT
        IF_INT_LESS(LW(0), 701)
            LOOP(0)
                CALL(evt_mario_get_name_hitobj_ride, LW(0))
                IF_STR_NOT_EQUAL(LW(0), 0)
                    BREAK_LOOP
                END_IF
                IF_INT_EQUAL(GF(0), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER(LW(1), -100)
                IF_INT_NOT_EQUAL(GF(0), 0)
                    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                    IF_INT_LESS(LW(0), 1650)
                        CALL(evt_mario_normalize)
                        CALL(evt_mario_mov_pos2, 1650, LW(2), 120)
                    END_IF
                END_IF
            ELSE
                CALL(evt_mario_wait_rideon)
                CALL(evt_mario_normalize)
            END_IF
        ELSE
            CALL(evt_mario_wait_rideon)
            CALL(evt_mario_normalize)
        END_IF
    END_INLINE_EVENT
    IF_INT_GREATER_OR_EQUAL(LW(0), 701)
        CALL(eff_call_minigame, 2, 0, 0, 500)
        WAIT_MS(4000)
    ELSE
        CALL(eff_call_minigame, 1, 0, 0, 500)
        WAIT_MS(4000)
    END_IF
    CALL(evt_yuucounter_free)
    CALL(yuugijou_montemedal_disp_onoff, 1)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 1, 0, LW(0), 0, LW(1), 701)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(yuuranking_get_monte_31_text_51C0, LW(3))
    CALL(yuugijou_add_montemedal, 0, LW(3))
    CALL(yuugijou_montemedal_wait)
    CALL(evt_yuuranking_free_31_text_5274)
    WAIT_MS(200)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 1, 1, LW(0), 0, LW(1), 701)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(evt_yuuranking_free_31_text_5274)
    SET_VALUE(LW(1), 10)
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(0), 1)
    ELSE
        CALL(evt_msg_select, 0, STRING("sysyuu_03_00"))
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
    CALL(evt_cam3d_evt_off, 0, 11)
    SET_VALUE(GF(0), 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(yuu_03_kaijo_init2)
    CALL(evt_cam_road_reset)
    CALL(unk_yuu_0000371c)
    WAIT_MS(500)
    CALL(re_init_yuunpc)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(evt_fade_in, 1000)
    RETURN
EVT_END()

EVT_BEGIN(yuu_03_init_evt)
    CALL(evt_map_playanim, STRING("yajirushi"), 1, 0)
    CALL(evt_map_playanim, STRING("tama"), 1, 0)
    CALL(evt_map_playanim, STRING("gurade"), 1, 0)
    CALL(evt_map_playanim, STRING("kyaku"), 1, 0)
    SPAWN_EVENT(minigame04_main)
    CALL_CHILD(evt_yuunpc_minigame4)
    RETURN
EVT_END()

// TODO: write yuuminigame4_gamerule (global, 0x7a0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: type
static u32 jumptable_00001654[9] = {
    (u32)((u8*)&yuuminigame4_gamerule + 0x5e8), (u32)((u8*)&yuuminigame4_gamerule + 0x26c), (u32)((u8*)&yuuminigame4_gamerule + 0x26c), (u32)((u8*)&yuuminigame4_gamerule + 0x310),
    (u32)((u8*)&yuuminigame4_gamerule + 0x388), (u32)((u8*)&yuuminigame4_gamerule + 0x42c), (u32)((u8*)&yuuminigame4_gamerule + 0x42c), (u32)((u8*)&yuuminigame4_gamerule + 0x4d0),
    (u32)((u8*)&yuuminigame4_gamerule + 0x548),
};

// TODO: type
static u32 jumptable_00001678[9] = {
    (u32)((u8*)&yuuminigame4_gamerule + 0x54), (u32)((u8*)&yuuminigame4_gamerule + 0xd8), (u32)((u8*)&yuuminigame4_gamerule + 0xf4), (u32)((u8*)&yuuminigame4_gamerule + 0x128),
    (u32)((u8*)&yuuminigame4_gamerule + 0x148), (u32)((u8*)&yuuminigame4_gamerule + 0x190), (u32)((u8*)&yuuminigame4_gamerule + 0x1b0), (u32)((u8*)&yuuminigame4_gamerule + 0x1e4),
    (u32)((u8*)&yuuminigame4_gamerule + 0x204),
};

// TODO: type
static u32 jumptable_0000169C[9] = {
    (u32)((u8*)&evt_ryushi + 0x208), (u32)((u8*)&evt_ryushi + 0x160), (u32)((u8*)&evt_ryushi + 0x19c), (u32)((u8*)&evt_ryushi + 0x208),
    (u32)((u8*)&evt_ryushi + 0x1d8), (u32)((u8*)&evt_ryushi + 0x160), (u32)((u8*)&evt_ryushi + 0x19c), (u32)((u8*)&evt_ryushi + 0x208),
    (u32)((u8*)&evt_ryushi + 0x1d8),
};

// TODO: type
u32 unk_yuu_bss00000010[1] = {
    0x00000000,
};

// TODO: type
u32 xwp[1] = {
    0x00000000,
};

// TODO: type
u32 ryuwp[1] = {
    0x00000000,
};

// TODO: type
static u32 yuunpc_wp[1] = {
    0x00000000,
};

// TODO: write yuuminigame_disp_4slit (global, 0x20 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_main_4slit (global, 0x4e0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write yuuminigame_init_4slit (global, 0xf8 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_ryushi_init (global, 0xb4 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write randf (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write ryushi_disp (global, 0x574 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write polygon (static, 0x124 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_nomal_cange (static, 0x74 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_slit_cancel (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_0000341c (global, 0xa0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_yuu_0000365c (global, 0xc0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/yuu_03.s; this is where the original has it, just before the first script that uses it
// clang-format on

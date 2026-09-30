#include "yuu_01.h"
#include "cam_shift.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
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
#include "evt_yuuminigame.h"
#include "forcedata.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "memory.h"
#include "motion/mot_ship.h"
#include "party.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "rel/yuu/yuu.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <string.h>

typedef struct MonteEntry {
    u8 unk0;                    // 0x00
    u8 unk1;                    // 0x01
    s16 count;                  // 0x02
    u8 unk4;                    // 0x04
    u8 pad_05[0x08 - 0x05];     // 0x05
    f32 unk8;                   // 0x08
    Vec position;               // 0x0C
    f32 unk18;                  // 0x18
    u8 pad_1C[0x40 - 0x1C];     // 0x1C
    void* work;                 // 0x40
} MonteEntry;                   // 0x44

#pragma pool_data off

#pragma force_active on
extern s32 evt_yuucounter_alloc[];
extern s32 evt_yuucounter_free[];
extern s32 evt_yuunpc_minigame2[];
extern s32 evt_yuuranking_alloc_31_text_52F8[];
extern s32 evt_yuuranking_free_31_text_5274[];
extern s32 evt_yuutitle_alloc[];
extern s32 evt_yuutitle_free[];
extern s32 yuuranking_get_monte_31_text_51C0[];
extern s32 yuuranking_wait_button_31_text_51F8[];
static USER_FUNC(re_fall_rtn);
static USER_FUNC(fall_rtn);
static void monte_disp(CameraId cameraId, void* param);
static BOOL get_monte_check(Vec* a, Vec* b);
static MonteEntry* monte_p;
static u32 yuunpc_wp[1];

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

static MonteTable montetable[6] = {
    {0.0f, 0.0f, 50.0f, 340.0f, 1},
    {250.0f, 250.0f, 50.0f, 340.0f, 1},
    {450.0f, 730.0f, -330.0f, 50.0f, 1},
    {-155.0f, -155.0f, -330.0f, -60.0f, 1},
    {150.0f, -155.0f, -330.0f, -60.0f, 1},
    {0.0f, 0.0f, 0.0f, 0.0f, 0},
};

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
static u32 yuunpc_wp[1] = {
    0x00000000,
};

USER_FUNC(unk_yuu_00005268) {
    camShiftReset();
    return EVT_RETURN_DONE;
}

static BOOL get_monte_check(Vec* a, Vec* b) {
    if (PSVECDistance(a, b) < 20.0f && __fabs(a->y - b->y) < 20.0) {
        return TRUE;
    }
    return FALSE;
}
// clang-format on
static void monte_disp(CameraId cameraId, void* param) {
    MonteEntry* entries = param;
    s32 i;

    if (param != NULL) {
        for (i = 0; i < entries->count; i++) {
            if (entries[i].unk0 == 0) {
                iconDispGx(&entries[i].position, 0, 0x13f, 1.0f);
            }
        }
    }
}

static inline f32 monte_rand(f32 min, f32 max) {
    return min + (f32)irand((s32)(100.0f * (max - min))) / 100.0f;
}

// NONMATCHING (99.44%): base[2]=0.0f is set before the do-while and base[4]=1.0f after it, matching the
// target's store layout. Only register ties remain: the z min/max pointers land in r26/r27 reversed (z
// subtraction in f0, not f1) and the psndSFX string address gets an extra `mr` into r21 instead of r26.
static USER_FUNC(fall_rtn) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    s32 wasAllocated;
    Yuuminigame2Data* data;
    s32 total;
    s32 i;
    s32 tableIdx;
    s32 pointIdx;
    const char* sfx;

    wasAllocated = 0;
    data = work->data;
    if (isFirstCall) {
        if (monte_p != NULL) {
            wasAllocated = 1;
        }
        total = 0;
        i = 0;
        {
            s32 c;
            while ((c = montetable[i].count) != 0) {
                total += c;
                i++;
            }
        }
        monte_p = _mapAlloc(total * 0x44);
        memset(monte_p, 0, total * 0x44);
        monte_p->count = total;
        monte_p->work = work;
        data->unk34 = 0;

        tableIdx = 0;
        pointIdx = 0;
        for (i = 0; i < monte_p->count; i++) {
            s32 j;
            f32* base = (f32*)((u8*)&monte_p[i] + 8);
            f32 lo;
            f32 hi;
            f32 min;
            f32 max;

            base[2] = 0.0f;
            do {
                lo = montetable[tableIdx].xmin;
                hi = montetable[tableIdx].xmax;
                if (lo < hi) {
                    min = lo;
                    max = hi;
                } else {
                    min = hi;
                    max = lo;
                }
                base[1] = monte_rand(min, max);
                lo = montetable[tableIdx].zmin;
                hi = montetable[tableIdx].zmax;
                if (lo < hi) {
                    min = lo;
                    max = hi;
                } else {
                    min = hi;
                    max = lo;
                }
                base[3] = monte_rand(min, max);
                for (j = 0; j < pointIdx; j++) {
                    Vec tmp;
                    PSVECSubtract((Vec*)(base + 1), (Vec*)((u8*)base - (j + 1) * 0x38 + 4), &tmp);
                    if (PSVECMag(&tmp) < 20.0f) {
                        break;
                    }
                }
            } while (j != pointIdx);
            pointIdx++;
            if (pointIdx >= montetable[tableIdx].count) {
                pointIdx = 0;
                tableIdx++;
            }
            base[4] = 1.0f;
        }
    }

    sfx = "SFX_EVT_GAME_MONTE_COUNT1";
    {
        for (i = 0; i < monte_p->count; i++) {
            MonteEntry* p = &monte_p[i];
            switch (p->unk4) {
            case 0:
                if (get_monte_check(&p->position, &marioGetPtr()->position) == 1) {
                    data->unk34++;
                    psndSFXOn_3D(sfx, &marioGetPtr()->position);
                    p->unk0 = 1;
                    p->unk4 = 1;
                }
                break;
            case 1:
                break;
            }
        }
    }
    if (wasAllocated == 0) {
        dispEntry(4, 0.0f, 1, monte_disp, monte_p);
    }
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(re_fall_rtn) {
    if (monte_p != NULL) {
        Yuuminigame2Data* data = ((YuuminigameWork*)((MonteEntry*)monte_p)->work)->data;
        if (data->unk38 != NULL) {
            evtDeleteID(data->unk38->eventId);
            data->unk38 = NULL;
            _mapFree(monte_p);
            data->unk38 = evtEntryType(minigame_view_monte, 0, 0, 0);
            data->unk38->lwData[0] = (s32)((MonteEntry*)monte_p)->work;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame2_set_phase) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    Yuuminigame2Data* data = work->data;
    data->phase = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame2_get_name) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    evtSetValue(event, args[1], (s32)s_nametbl[index]);
    evtSetValue(event, args[2], (s32)a_nametbl[index]);
    evtSetValue(event, args[3], turn_tbl[index]);
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame2_opening_on_off) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    Yuuminigame2Data* data = work->data;
    if (evtGetValue(event, args[1]) != 0) {
        data->flags |= 1;
    } else {
        data->flags &= ~1;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame2_opening_chk) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    Yuuminigame2Data* data = work->data;
    if (data->flags & 1) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

void yuuminigame_init_2fune(EventEntry* event, BOOL isFirstCall) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    Yuuminigame2Data* data = work->data;

    data->unk38 = evtEntryType(minigame_view_monte, 0, 0, 0);
    data->unk38->lwData[0] = event->lwData[0];
    data->flags |= 1;
    marioChgShipMotion();
}

void yuuminigame_main_2fune(EventEntry* event, BOOL isFirstCall) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    Yuuminigame2Data* data = work->data;

    switch (data->phase) {
    case 5:
        data->phase = 1;
        /* fallthrough */
    case 1:
        work->childEvent = evtEntry(minigame02_start, 0, 0);
        work->childEvent->lwData[0] = event->lwData[0];
        data->phase++;
        break;
    case 2:
        data->unk10 = 0x15;
        data->phase = 3;
        data->unk0C = 0;
        /* fallthrough */
    case 3:
        if (yuuminigame2_gamerule(event, isFirstCall, work, data)) {
            data->phase++;
        }
        break;
    case 4:
        work->childEvent = evtEntry(minigame02_end, 0, 0);
        work->childEvent->lwData[0] = data->unk08;
        if (data->unk0C + 5 <= 0) {
            data->unk0C = 1;
        } else {
            data->unk0C += 0x105;
        }
        work->childEvent->lwData[1] = data->unk0C;
        work->childEvent->lwData[2] = data->unk34;
        data->phase++;
        break;
    }
}

void yuuminigame_disp_2fune(CameraId cameraId, void* param) {
    monteCountDisp();
}

// 100%: the dead `(0xD ^ i) >> 1` after the second search needs an explicit `if (i < 0xD) break;`,
// and the hoko_tbl swirl-power conversion inlines `irand` into the argument (so the 8.0f
// constant's address is materialised as in the original). Both were reproduced here.
s32 yuuminigame2_gamerule(EventEntry* event, BOOL isFirstCall, void* work, Yuuminigame2Data* data) {
    MarioWork* mario = marioGetPtr();
    Vec near[3];
    Vec pos;
    Vec tmp;
    Vec vec;
    Mtx mtx;
    s32 i;
    s32 j;
    f32 radius2;
    f32 angle;

    if (data->unk10 == 0x15) {
        if ((mario->position.x <= -700.0f && mario->position.z <= 0.0f) || yuucounter_get_data() < 0) {
            data->unk08 = yuucounter_get_data();
            marioKeyOff();
            partyKeyOff();
            yuucounter_flag_on(2);
            return 1;
        }
    }

    near[2] = mario->position;
    near[1] = mario->position;
    near[0] = mario->position;
    near[0].x -= 12.5f;
    near[2].x += 12.5f;

    while (data->unk10 < 1 || data->unk10 >= 5) {
        radius2 = 3306.25f;
        for (i = 1; i < 5; i++) {
            mapObjGetPos(s_nametbl[i], &pos);
            pos.y = mario->position.y;
            for (j = 0; j < 3; j++) {
                if (PSVECSquareDistance(&near[j], &pos) <= radius2) {
                    tmp = mario->position;
                    if (tmp.x == pos.x && tmp.z == pos.z) {
                        tmp.x += 1.0f;
                    }
                    data->unk10 = i;
                    marioKeyOff();
                    partyKeyOff();
                    mario->flags |= 0x200;
                    mario->flags |= 0x100;
                    data->unk14 = 0;
                    PSVECSubtract(&tmp, &pos, &data->unk1C);
                    data->unk1C.y = 0.0f;
                    data->unk18 = PSVECMag(&data->unk1C);
                    PSVECNormalize(&data->unk1C, &data->unk1C);
                    data->unk28 = 0x78;
                    data->unk2C = mario->position.y;
                    mario->dispFlags |= 0x100;
                    psndSFXOn_3D((const char*)0x8DB, &mario->position);
                    data->unk0C--;
                    break;
                }
            }
            if (j < 3) {
                break;
            }
        }

        if (i >= 5) {
            radius2 = 225.0f;
            for (i = 5; i < 0xD; i++) {
                mapObjGetPos(s_nametbl[i], &pos);
                pos.y = mario->position.y;
                for (j = 0; j < 3; j++) {
                    if (PSVECSquareDistance(&near[j], &pos) <= radius2) {
                        if (marioChkSwirlPower() == 0) {
                            psndSFXOn_3D((const char*)0x8DC, &mario->position);
                            data->unk0C--;
                        }
                        data->unk10 = i;
                        {
                            marioSetSwirlPower(1000, 8.0f, (f32)(s32)(hoko_tbl[data->unk10] + irand(0x5B) - 0x2D));
                        }
                        mario->dispFlags |= 0x100;
                        break;
                    }
                }
                if (j < 3) {
                    break;
                }
            }
            if (i < 0xD) {
                break;
            }
        }
        break;
    }

    if (data->unk10 == 0x15) {
        goto end;
    }

    angle = mario->unk1A8 + (f32)(s32)turn_tbl[data->unk10];
    if (data->unk10 >= 5 && data->unk10 < 0xD) {
        if (marioChkSwirlPower() != 0) {
            goto apply;
        }
        data->unk10 = 0x15;
        mario->dispFlags &= ~0x100;
        if (mario->unk1A8 >= -90.0f && mario->unk1A8 < 90.0f) {
            angle = 0.0f;
        } else {
            angle = 180.0f;
        }
        goto apply;
    }

    if (data->unk10 >= 1 && data->unk10 < 5) {
        switch (data->unk14) {
        case 0:
            data->unk28--;
            PSMTXRotRad(mtx, 'Y', (f32)(s32)turn_tbl[data->unk10] * 0.017453292f);
            PSMTXMultVec(mtx, &data->unk1C, &data->unk1C);
            data->unk1C.y = 0.0f;
            PSVECNormalize(&data->unk1C, &data->unk1C);
            {
                f32 scale;

                if (data->unk28 >= 0x1E) {
                    scale = data->unk18 * ((f32)(data->unk28 - 0x1E) / 90.0f);
                } else {
                    scale = 0.0f;
                }
                PSVECScale(&data->unk1C, &vec, scale);
            }
            mapObjGetPos(s_nametbl[data->unk10], &pos);
            PSVECAdd(&vec, &pos, &vec);
            mario->position.x = vec.x;
            mario->position.z = vec.z;
            mario->position.y = data->unk2C - 30.0f * ((f32)(0x78 - data->unk28) / 120.0f);
            if (data->unk28 <= 0) {
                data->unk14++;
                angle = 0.0f;
                data->unk28 = 0x1E;
                mario->dispFlags &= ~0x100;
                data->unk30 = mario->position.x;
            }
            break;

        case 1:
            data->unk28--;
            angle = 0.0f;
            mario->position.x = data->unk30 + (-100.0f) * ((f32)(0x1E - data->unk28) / 30.0f);
            mario->position.y = data->unk2C - 30.0f;
            if (data->unk28 <= 0) {
                data->unk14++;
                data->unk28 = 0x1E;
            }
            break;

        case 2:
            data->unk28--;
            angle = 0.0f;
            mario->position.y = data->unk2C - 30.0f * ((f32)data->unk28 / 30.0f);
            if (data->unk28 <= 0) {
                data->unk10 = 0x15;
                marioKeyOn();
                partyKeyOn();
                mario->flags &= ~0x200;
                mario->flags &= ~0x100;
            }
            break;
        }
    }

apply:
    *(f32*)mario->unk1AC = reviseAngle(angle);
    mario->unk1A8 = *(f32*)mario->unk1AC;
end:
    return 0;
}


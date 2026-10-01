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
#include "cam_shift.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "evt_yuuminigame.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "math.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "memory.h"
#include "party.h"
#include "pmario_sound.h"
#include "rel/yuu/yuu.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
extern s32 evt_yuucounter_alloc[];
extern s32 evt_yuucounter_free[];
extern s32 evt_yuunpc_minigame1[];
extern s32 evt_yuuranking_alloc_31_text_52F8[];
extern s32 evt_yuuranking_free_31_text_5274[];
extern s32 evt_yuutitle_alloc[];
extern s32 evt_yuutitle_free[];
extern s32 yuuranking_get_monte_31_text_51C0[];
extern s32 yuuranking_wait_button_31_text_51F8[];
static USER_FUNC(re_fall_rtn);
static USER_FUNC(fall_rtn);
static void monte_disp(CameraId cameraId, void* param);

// One monte (chip) entry, stride 0x44.
typedef struct YuuMonte {
    u8 flag;                  // 0x00
    u8 pad_01;
    s16 count;                // 0x02
    u8 state;                 // 0x04
    u8 pad_05[7];             // 0x05
    Vec pos;                  // 0x0C
    f32 unk18;                // 0x18
    u8 pad_1C[0x24];          // 0x1C
    YuuminigameWork* work;    // 0x40
} YuuMonte;

// Region a monte can spawn in (stride 0x14).
typedef struct MonteTable {
    f32 xMin;   // 0x00
    f32 xMax;   // 0x04
    f32 yMin;   // 0x08
    f32 yMax;   // 0x0C
    s32 count;  // 0x10
} MonteTable;

static s32 get_monte_check(Vec* a, Vec* b);

static u8* yuunpc_wp;
static YuuMonte* monte_p;

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

static MonteTable montetable[] = {
    { -1200.0f, -800.0f, 450.0f, 200.0f, 2 },
    { -800.0f, -300.0f, 300.0f, 100.0f, 2 },
    { -300.0f, 200.0f, 200.0f, 100.0f, 1 },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0 },
};


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




// clang-format on

static inline f32 monte_rand(f32 min, f32 max) {
    return min + (f32)irand((s32)(100.0f * (max - min))) / 100.0f;
}

USER_FUNC(unk_yuu_00004250) {
    camShiftReset();
    return EVT_RETURN_DONE;
}

static s32 get_monte_check(Vec* a, Vec* b) {
    if (PSVECDistance(a, b) < 20.0f && fabsf(a->y - b->y) < 20.0) {
        return 1;
    }
    return 0;
}

static void monte_disp(CameraId cameraId, void* param) {
    YuuMonte* monte = param;
    s32 i;

    if (monte != NULL) {
        for (i = 0; i < monte->count; i++) {
            if (monte[i].flag == 0) {
                iconDispGx(&monte[i].pos, 0, 0x13F, 1.0f);
            }
        }
    }
}

// Setting base[3]=0.0f before the do-while (not at its top) matched the target's store layout, and
// expanding the y-range monte_rand with `max -= min` (MUST_MATCH) made its subtraction reuse max's
// register (f1), as the target does. The only remaining difference is the sfx string address: the
// compiler materialises it as `addi r0,r4,str@l` and copies it to r21, where the target forms it
// directly in r26 (an extra `mr` plus a register difference at the psndSFXOn_3D call). Referencing the
// target's named .rodata globals instead of literals does not change this.
// NONMATCHING (99.53%): only the sfx register difference above remains.
static USER_FUNC(fall_rtn) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    s32 wasAllocated;
    YuuMinigame1Data* data;
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
        data->count = 0;

        tableIdx = 0;
        pointIdx = 0;
        for (i = 0; i < monte_p->count; i++) {
            s32 j;
            f32* base = (f32*)((u8*)&monte_p[i] + 8);
            f32 min;
            f32 max;

            base[3] = 0.0f;
            do {
                if (montetable[tableIdx].xMin < montetable[tableIdx].xMax) {
                    min = montetable[tableIdx].xMin;
                    max = montetable[tableIdx].xMax;
                } else {
                    min = montetable[tableIdx].xMax;
                    max = montetable[tableIdx].xMin;
                }
                base[1] = monte_rand(min, max);
                if (montetable[tableIdx].yMin < montetable[tableIdx].yMax) {
                    min = montetable[tableIdx].yMin;
                    max = montetable[tableIdx].yMax;
                } else {
                    min = montetable[tableIdx].yMax;
                    max = montetable[tableIdx].yMin;
                }
#ifdef MUST_MATCH
                // match-only: the in-place range makes MWCC keep it in max's register (f1)
                max -= min;
                base[2] = min + (f32)irand((s32)(100.0f * max)) / 100.0f;
#else
                base[2] = monte_rand(min, max);
#endif
                for (j = 0; j < pointIdx; j++) {
                    Vec diff;
                    PSVECSubtract((Vec*)(base + 1), (Vec*)((u8*)base - (j + 1) * 0x38 + 4), &diff);
                    if (PSVECMag(&diff) < 20.0f) {
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

    {
        sfx = "SFX_EVT_GAME_MONTE_COUNT1";
        for (i = 0; i < monte_p->count; i++) {
            YuuMonte* p = &monte_p[i];
            switch (p->state) {
                case 0:
                    if (get_monte_check(&p->pos, &marioGetPtr()->position) == 1) {
                        data->count++;
                        psndSFXOn_3D(sfx, &marioGetPtr()->position);
                        p->flag = 1;
                        p->state = 1;
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
        YuuMinigame1Data* data = ((YuuminigameWork*)monte_p->work)->data;
        if (data->event != NULL) {
            evtDeleteID(data->event->eventId);
            data->event = NULL;
            _mapFree(monte_p);
            data->event = evtEntryType(minigame_view_monte, 0, 0, 0);
            data->event->lwData[0] = (s32)monte_p->work;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_change_plane) {
    marioChgMot(0x18);
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame1_set_phase) {
    s32* args = event->args;
    YuuMinigame1Data* data = ((YuuminigameWork*)evtGetValue(event, args[0]))->data;
    data->phase = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame1_opening_on_off) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    YuuMinigame1Data* data = work->data;
    if (evtGetValue(event, args[1]) != 0) {
        data->flags |= 1;
    } else {
        data->flags &= ~1;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame1_opening_chk) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    if (((YuuMinigame1Data*)work->data)->flags & 1) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

void yuuminigame_init_1hikoki(EventEntry* event, BOOL isFirstCall) {
    YuuMinigame1Data* data = ((YuuminigameWork*)event->lwData[0])->data;

    data->event = evtEntryType(minigame_view_monte, 0, 0, 0);
    data->event->lwData[0] = event->lwData[0];
    data->flags |= 1;
}

void yuuminigame_main_1hikoki(EventEntry* event, BOOL isFirstCall) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    YuuMinigame1Data* data = work->data;

    switch (data->phase) {
        case 0:
            break;
        case 4:
            data->phase = 1;
            // fallthrough
        case 1:
            work->childEvent = evtEntry(minigame01_start, 0, 0);
            work->childEvent->lwData[0] = event->lwData[0];
            data->phase++;
            break;
        case 2:
            if (yuuminigame1_gamerule(event, isFirstCall, work, data) != 0) {
                data->phase++;
            }
            break;
        case 3:
            work->childEvent = evtEntry(minigame01_end, 0, 0);
            work->childEvent->lwData[0] = data->counterData;
            work->childEvent->lwData[1] = data->table;
            work->childEvent->lwData[2] = data->count;
            data->phase++;
            break;
        default:
            break;
    }
}

void yuuminigame_disp_1hikoki(CameraId cameraId, void* param) {
    monteCountDisp();
}

s32 yuuminigame1_gamerule(EventEntry* event, BOOL isFirstCall, YuuminigameWork* work, YuuMinigame1Data* data) {
    MarioWork* mario = marioGetPtr();
    u16 motion;

    if (mario->hitobjStandOn != NULL) {
        if (mario->trigFlags & 1) {
            motion = mario->prevMotionId;
        } else {
            motion = mario->currMotionId;
        }
        if (motion != 0x18) {
            data->table = 1;
            data->counterData = yuucounter_get_data();
            if (strcmp("A_dai_01", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = 2;
            }
            if (strcmp("A_dai_02", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = 2;
            }
            if (strcmp("A_dai_03", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = 2;
            }
            if (strcmp("A_dai_04", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = 3;
            }
            if (strcmp("A_dai_05", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = 5;
            }
            if (strcmp("A_ground_02", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = -5;
            }
            if (strcmp("A_ground_05", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = -5;
            }
            if (strcmp("A_ground_03", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = -10;
            }
            if (strcmp("A_ground_06", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = -15;
            }
            if (strcmp("A_ground_04", hitGetName(mario->hitobjStandOn)) == 0) {
                data->table = -20;
            }
            marioKeyOff();
            partyKeyOff();
            yuucounter_flag_on(2);
            return 1;
        }
    }
    return 0;
}

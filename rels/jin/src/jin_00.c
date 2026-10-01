#include "jin_00.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "eff/eff_teresa.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "motion/mot_hammer.h"
#include "party.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "system.h"
#include <stdlib.h>

#pragma pool_data off

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];
static void senteresa_ext_shadow_disp(void);
static void senteresa_ext_disp(void);
static void senteresa_ext_dispent(void);
static void senteresa_ext_init(void);
static void computePos(TeresaEntry* p, Vec* out);
static f32 randf(f32 min, f32 max);
s32 senteresa_evt[1248];
static TeresaEntry* pTeresa;
Vec g_vecRotCenter;
u32 g_bMove[1];
u32 g_nMarioMoveCnt[1];
s32 g_nNumHuttobasu[1];
u32 g_nNumHarituku[1];
static s32 g_nNumEndProc[1];
f32 g_fSpeed[1];

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

static EVT_BEGIN(w_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("door01L"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("door01R"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("door01L"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("door01R"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n3_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n3_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door07"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door07"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n2_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n2_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(e3_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door06"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(e3_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door06"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(e1_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door04"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(e1_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door04"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(e2_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door05"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(e2_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door05"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(jin_00_dai)
    DEBUG_PUT_MSG(STRING("出る"))
    CALL(evt_mario_normalize)
    SET_VALUE(GSWF(2079), 1)
    CALL(evt_bero_mapchange, STRING("jin_06"), 0)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(jin_00_dai_in_sub)
    CALL(evt_seq_wait, 2)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, 390, -60, 0)
    CALL(evt_party_set_pos, 0, 390, -60, -10)
    CALL(evt_party_set_pos, 1, 390, -60, -20)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_party_set_dir, 1, 270, -1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_jump_pos, 365, 40, 0, 700, 0, -1000)
        CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_party_jump_pos, 0, 365, 40, -10, 700, 0, -1000)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_party_jump_pos, 1, 365, 40, -20, 700, 0, -1000)
    END_INLINE_EVENT
    WAIT_MS(1500)
    RETURN
EVT_END()

EVT_BEGIN(jin_00_dai_in)
    DEBUG_PUT_MSG(STRING("入る"))
    SPAWN_EVENT(jin_00_dai_in_sub)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(check_roll)
    CALL(evt_mario_get_motion, LW(0))
    IF_INT_EQUAL(LW(0), 22)
        SET_VALUE(LW(0), 0)
    ELSE
        SET_VALUE(LW(0), 1)
    END_IF
    RETURN
EVT_END()


EVT_BEGIN(teresa_n_fadein)
    IS_EVENT_RUNNING(GW(0), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        DELETE_EVENT(GW(0))
    END_IF
    CALL(get_npc_color, STRING("奥扉テレサ"), LW(0), LW(1), LW(2), LW(3))
    LOOP(0)
        ADD(LW(3), 4)
        IF_INT_GREATER_OR_EQUAL(LW(3), 255)
            SET_VALUE(LW(3), 255)
            BREAK_LOOP
        END_IF
        CALL(evt_npc_set_color, STRING("奥扉テレサ"), 255, 255, 255, LW(3))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_color, STRING("奥扉テレサ"), 255, 255, 255, 255)
    RETURN
EVT_END()

EVT_BEGIN(teresa_n_fadeout)
    SET_VALUE(LW(0), 255)
    LOOP(0)
        SUBTRACT(LW(0), 4)
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            SET_VALUE(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_npc_set_color, STRING("奥扉テレサ"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_color, STRING("奥扉テレサ"), 255, 255, 255, 0)
    RETURN
EVT_END()

EVT_BEGIN(door_teresa_n)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 197)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_start_for_event)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("奥扉テレサ"), LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_npc_set_ry, STRING("奥扉テレサ"), 270)
        CALL(evt_mario_set_dir, 90, 0, 0)
        SET_VALUE(LW(6), -20)
        SET_VALUE(LW(7), 90)
    ELSE
        CALL(evt_npc_set_ry, STRING("奥扉テレサ"), 90)
        CALL(evt_mario_set_dir, 270, 0, 0)
        SET_VALUE(LW(6), 20)
        SET_VALUE(LW(7), 270)
    END_IF
    CALL_CHILD(teresa_n_fadein)
    IF_INT_LESS(GSW(0), 196)
        CALL(evt_msg_print, 0, STRING("stg4_jin_49"), 0, STRING("奥扉テレサ"))
    ELSE
        CALL(evt_msg_print, 0, STRING("stg4_jin_51"), 0, STRING("奥扉テレサ"))
    END_IF
    SPAWN_EVENT_GET_ID(PTR(teresa_n_fadeout), GW(0))
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(teresa_s_fadein)
    IS_EVENT_RUNNING(GW(1), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        DELETE_EVENT(GW(1))
    END_IF
    CALL(get_npc_color, STRING("手前扉テレサ"), LW(0), LW(1), LW(2), LW(3))
    LOOP(0)
        ADD(LW(3), 4)
        IF_INT_GREATER_OR_EQUAL(LW(3), 255)
            SET_VALUE(LW(3), 255)
            BREAK_LOOP
        END_IF
        CALL(evt_npc_set_color, STRING("手前扉テレサ"), 255, 255, 255, LW(3))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_color, STRING("手前扉テレサ"), 255, 255, 255, 255)
    RETURN
EVT_END()

EVT_BEGIN(teresa_s_fadeout)
    SET_VALUE(LW(0), 255)
    LOOP(0)
        SUBTRACT(LW(0), 4)
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            SET_VALUE(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_npc_set_color, STRING("手前扉テレサ"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_color, STRING("手前扉テレサ"), 255, 255, 255, 0)
    RETURN
EVT_END()

EVT_BEGIN(door_teresa_s)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 197)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_start_for_event)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("手前扉テレサ"), LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_npc_set_ry, STRING("手前扉テレサ"), 270)
        CALL(evt_mario_set_dir, 90, 0, 0)
        SET_VALUE(LW(6), -30)
        SET_VALUE(LW(7), 90)
    ELSE
        CALL(evt_npc_set_ry, STRING("手前扉テレサ"), 90)
        CALL(evt_mario_set_dir, 270, 0, 0)
        SET_VALUE(LW(6), 20)
        SET_VALUE(LW(7), 270)
    END_IF
    CALL_CHILD(teresa_s_fadein)
    IF_INT_LESS(GSW(0), 196)
        CALL(evt_msg_print, 0, STRING("stg4_jin_50"), 0, STRING("手前扉テレサ"))
    ELSE
        CALL(evt_msg_print, 0, STRING("stg4_jin_52"), 0, STRING("手前扉テレサ"))
    END_IF
    SPAWN_EVENT_GET_ID(PTR(teresa_s_fadeout), GW(1))
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

BeroEntry bero_stg4majo3nin[1] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "usu_00", "evt_majo3", 5, 5, w_close, w_open},
};

static BeroEntry bero_entry_data[10] = {
    {"n_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, check_roll, "jin_05", "s_bero", 1, 1},
    {"ana", 3840, 0, 0x4E20, 100000, 0, 0, -1, jin_00_dai_in, 0, jin_00_dai, "jin_06", NULL, 4, 4},
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, 120, NULL, -1, NULL, "gra_06", "n_bero", 5, 5, w_close, w_open},
    {"n_bero_3", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, door_teresa_n, "jin_01", "s_bero_1", 5, 5, n3_close, n3_open},
    {"s_bero", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, door_teresa_s, "jin_02", "n_bero_1", 5, 5, s_close, s_open},
    {"n_bero_2", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_04", "s_bero", 5, 5, n2_close, n2_open},
    {"e_bero_3", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_03", "w_bero3", 5, 5, e3_close, e3_open},
    {"e_bero_1", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_03", "w_bero1", 5, 5, e1_close, e1_open},
    {"e_bero_2", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_03", "w_bero2", 5, 5, e2_close, e2_open},
    {NULL},
};

EVT_BEGIN(first_jin00)
    CALL(evt_mario_set_pos, -640, 0, 0)
    CALL(evt_party_set_pos, 0, -640, 0, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 227, 365, 1120, 227, 128, -223, 0, 11)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_seq_wait, 2)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, -328, 365, 1120, -328, 128, -223, 5000, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL_CHILD(evt_bero_info_run)
    END_INLINE_EVENT
    WAIT_MS(5000)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(1000)
    SET_VALUE(GSW(0), 194)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(push_evt)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_get_pose, LW(0))
    IF_STR_NOT_EQUAL(LW(0), STRING("M_O_1"))
        CALL(evt_mario_set_pose, STRING("M_O_1"))
    END_IF
    SET_VALUE(LW(0), GW(10))
    MODULO(LW(0), 4)
    DIVIDE(LW(0), 2)
    IF_INT_LESS(GW(10), 40)
        MULTIPLY(LW(0), 1)
    ELSE
        MULTIPLY(LW(0), 2)
        BEGIN_INLINE_EVENT
            CALL(evt_sub_random, 100, LW(0))
            IF_INT_LESS(LW(0), 10)
                CALL(evt_mapobj_get_position, STRING("oki01"), LW(0), LW(1), LW(2))
                CALL(evt_eff, 0, STRING("kemuri"), 2, LW(0), 35, 40, 0, 0, 0, 0, 0, 0, 0, 0)
                ADD(LW(0), 20)
                CALL(evt_eff, 0, STRING("kemuri"), 2, LW(0), 35, 40, 0, 0, 0, 0, 0, 0, 0, 0)
            END_IF
        END_INLINE_EVENT
    END_IF
    CALL(evt_mapobj_trans, 1, STRING("oki01"), LW(0), 0, 0)
    ADD(GW(10), 1)
    IF_INT_GREATER_OR_EQUAL(GW(10), 80)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_mario_set_pose, STRING("M_O_1"))
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        BEGIN_INLINE_EVENT
            CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 1600)
        END_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_MOVE_PEDESTAL1"), LW(2), LW(3), LW(4), LW(8))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_MOVE_PEDESTAL2"), LW(2), LW(3), LW(4), LW(9))
        BEGIN_INLINE_EVENT
            LOOP(8)
                CALL(evt_mapobj_get_position, STRING("oki01"), LW(0), LW(1), LW(2))
                CALL(evt_eff, 0, STRING("kemuri"), 2, LW(0), 35, 40, 0, 0, 0, 0, 0, 0, 0, 0)
                ADD(LW(0), 20)
                CALL(evt_eff, 0, STRING("kemuri"), 2, LW(0), 35, 40, 0, 0, 0, 0, 0, 0, 0, 0)
                WAIT_MS(200)
            END_LOOP
        END_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 500, 1600)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_trans, 1, STRING("oki01"), LW(0), 0, 0)
        CALL(evt_hit_bind_update, STRING("a_oki01"))
        ADD(LW(0), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), 396)
            CALL(evt_mario_get_pose, LW(5))
            IF_STR_NOT_EQUAL(LW(5), STRING("M_N_7"))
                BEGIN_INLINE_EVENT
                    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
                END_INLINE_EVENT
                CALL(evt_mario_set_pose, STRING("M_N_7"))
            END_IF
        ELSE
            CALL(evt_mario_set_pos, LW(0), LW(3), LW(4))
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        CALL(evt_snd_sfxoff, LW(8))
        CALL(evt_snd_sfxoff, LW(9))
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        CALL(evt_mario_dispflag_onoff, 0, 2)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_hitobj_onoff, STRING("a_futa"), 0, 0)
        SET_VALUE(GSW(0), 195)
        CALL(evt_exit_case_evt)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(nopush_evt)
    SET_VALUE(GW(10), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_key)
    SET_VALUE(GSW(0), 198)
    RETURN
EVT_END()

// TODO: type
static u32 lock_tbl[2] = {
    0x00000016, 0xFFFFFFFF,
};

static EVT_BEGIN(lock_item_check)
    CALL(evt_pouch_check_item, 22, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(lock_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock)
    SET_VALUE(GSW(0), 199)
    SET_VALUE(LW(0), STRING("n_bero_2"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

EVT_BEGIN(tyuou_teresa_kusukusu)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("中央テレサ"), LW(3), LW(4), LW(5))
        CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(6))
        IF_INT_LESS(LW(6), 125)
            CALL(evt_msg_print, 0, STRING("stg4_jin_53"), 0, STRING("中央テレサ"))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(tyuou_teresa_look_mario)
    CALL(evt_seq_wait, 2)
    LOOP(0)
        CALL(evt_set_dir_to_target, STRING("中央テレサ"), STRING("mario"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(tyuou_teresa_init)
    SPAWN_EVENT(npc_init_evt)
    IF_INT_EQUAL(GSW(0), 196)
        CALL(evt_npc_set_position, STRING("me"), 70, 0, 0)
    END_IF
    IF_INT_EQUAL(GSW(0), 197)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
        CALL(evt_npc_set_color, STRING("me"), 255, 255, 255, 0)
        CALL(evt_npc_change_interrupt, STRING("me"), 6, 0)
    END_IF
    RETURN
EVT_END()


EVT_BEGIN(tyuou_teresa_talk)
    CALL(evt_msg_print, 0, STRING("stg4_jin_54"), 0, STRING("中央テレサ"))
    CALL(evt_msg_select, 0, STRING("stg4_jin_54_yn"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("stg4_jin_55"))
        RETURN
    ELSE
        CALL(evt_msg_continue)
    END_IF
    CALL(evt_cam3d_evt_set, 70, 165, 472, 70, 72, -45, 1000, 11)
    CALL(evt_npc_set_anim, STRING("中央テレサ"), STRING("A_1"))
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_pose, STRING("M_I_O"))
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_mario_dispflag_onoff, 1, 256)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("中央テレサ"), LW(3), LW(4), LW(5))
        IF_INT_LESS(LW(0), LW(3))
            CALL(evt_npc_set_ry, STRING("中央テレサ"), 270)
            SUBTRACT(LW(0), 40)
        ELSE
            CALL(evt_npc_set_ry, STRING("中央テレサ"), 90)
            ADD(LW(0), 40)
        END_IF
        CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 300, 0, -1000)
        WAIT_MS(300)
        CALL(evt_mario_dispflag_onoff, 0, 256)
        CALL(evt_mario_dispflag_onoff, 0, 2)
    END_INLINE_EVENT
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_party_dispflg_onoff, 0, 16, 1)
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("中央テレサ"), LW(3), LW(4), LW(5))
        IF_INT_LESS(LW(0), LW(3))
            SUBTRACT(LW(0), 40)
        ELSE
            ADD(LW(0), 40)
        END_IF
        CALL(evt_party_jump_pos, 0, LW(0), LW(1), LW(2), 300, 0, -1000)
        WAIT_MS(300)
        CALL(evt_party_dispflg_onoff, 0, 16, 0)
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_cam3d_evt_set, 70, 193, 689, 70, 72, 0, 60, 11)
            WAIT_MS(60)
            IF_INT_EQUAL(GF(0), 1)
                BREAK_LOOP
            END_IF
            CALL(evt_cam3d_evt_set, 70, 165, 472, 70, 72, -45, 60, 11)
            WAIT_MS(60)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 100, 400, 100)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        CALL(evt_npc_set_scale, STRING("中央テレサ"), LW(0), LW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("中央テレサ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_TELESA_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("中央テレサ"), STRING("S_1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg4_jin_56"), 0, STRING("中央テレサ"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    SET_VALUE(GF(0), 1)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, 70, 165, 472, 70, 72, -45, 500, 11)
    CALL(evt_sub_intpl_msec_init, 11, 400, 100, 500)
    LABEL(2)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
    CALL(evt_npc_set_scale, STRING("中央テレサ"), LW(0), LW(0), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg4_jin_57"), 0, STRING("中央テレサ"))
    CALL(evt_cam3d_evt_set, 70, 400, 1125, 70, 200, -13, 1000, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
        LABEL(3)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_color, STRING("中央テレサ"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(3)
        END_IF
        CALL(evt_npc_set_color, STRING("中央テレサ"), 255, 255, 255, 0)
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("中央テレサ"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("中央テレサ"), 0x80000000)
    CALL(evt_npc_change_interrupt, STRING("中央テレサ"), 6, 0)
    SPAWN_EVENT(senteresa_evt)
    CALL(senteresa_wait_end_seq)
    WAIT_MS(3500)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 197)
    RETURN
EVT_END()

EVT_BEGIN(oku_teresa_init)
    SPAWN_EVENT(npc_init_evt)
    IF_INT_LESS(GSW(0), 197)
        CALL(evt_npc_set_position, STRING("me"), -60, 45, -238)
        CALL(evt_npc_set_color, STRING("me"), 255, 255, 255, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(temae_teresa_init)
    SPAWN_EVENT(npc_init_evt)
    IF_INT_LESS(GSW(0), 197)
        CALL(evt_npc_set_position, STRING("me"), -60, 45, 238)
        CALL(evt_npc_set_color, STRING("me"), 255, 255, 255, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(atomic_teresa_init)
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[276] = {
    (u32)"テレサ", 0x40000600, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"中央テレサ",
    0x40000600, 0x00000000, (u32)&tyuou_teresa_init, 0x00000000,
    (u32)&tyuou_teresa_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"奥扉テレサ", 0x40000620,
    0x00000000, (u32)&oku_teresa_init, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"手前扉テレサ", 0x40000620, 0x00000000,
    (u32)&temae_teresa_init, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"アトミックテレサ", 0x000400A0, 0x00000000, (u32)&atomic_teresa_init,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"1001テレサ",
    0x000400A0, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"1002テレサ", 0x000400A0,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"1003テレサ", 0x000400A0, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"1004テレサ", 0x000400A0, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"1005テレサ",
    0x000400A0, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"1006テレサ", 0x000400A0,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 senteresa_ext_entry_data[141] = {
    (u32)"c_teresa", (u32)"S_1", 0x00000000, (u32)"c_teresa",
    (u32)"S_1", 0x40A00000, (u32)"c_teresa", (u32)"S_1",
    0x41200000, (u32)"c_teresa", (u32)"S_1", 0x41700000,
    (u32)"c_teresa", (u32)"S_1", 0x41A00000, (u32)"c_teresa",
    (u32)"S_1", 0x41C80000, (u32)"c_teresa", (u32)"S_1",
    0x41F00000, (u32)"c_teresa", (u32)"S_1", 0x420C0000,
    (u32)"c_teresa", (u32)"S_1", 0x42200000, (u32)"c_teresa",
    (u32)"S_1", 0x42340000, (u32)"c_teresa", (u32)"S_1",
    0x42480000, (u32)"c_teresa", (u32)"S_1", 0x425C0000,
    (u32)"c_teresa", (u32)"S_1", 0x42700000, (u32)"c_teresa",
    (u32)"S_1", 0x42820000, (u32)"c_teresa", (u32)"S_1",
    0x428C0000, (u32)"c_teresa", (u32)"S_1", 0x42960000,
    (u32)"c_teresa", (u32)"W_1", 0x00000000, (u32)"c_teresa",
    (u32)"W_1", 0x3F800000, (u32)"c_teresa", (u32)"W_1",
    0x40000000, (u32)"c_teresa", (u32)"W_1", 0x40400000,
    (u32)"c_teresa", (u32)"W_1", 0x40800000, (u32)"c_teresa",
    (u32)"W_1", 0x40A00000, (u32)"c_teresa", (u32)"W_1",
    0x40C00000, (u32)"c_teresa", (u32)"W_1", 0x40E00000,
    (u32)"c_teresa", (u32)"W_1", 0x41000000, (u32)"c_teresa",
    (u32)"W_1", 0x41100000, (u32)"c_teresa", (u32)"D_1",
    0x00000000, (u32)"c_teresa", (u32)"D_1", 0x3F800000,
    (u32)"c_teresa", (u32)"D_1", 0x40000000, (u32)"c_teresa",
    (u32)"D_1", 0x40400000, (u32)"c_teresa", (u32)"D_1",
    0x40800000, (u32)"c_teresa", (u32)"D_1", 0x40A00000,
    (u32)"c_teresa", (u32)"D_1", 0x40C00000, (u32)"c_teresa",
    (u32)"D_1", 0x40E00000, (u32)"c_teresa", (u32)"A_2",
    0x00000000, (u32)"c_teresa", (u32)"A_2", 0x3F800000,
    (u32)"c_teresa", (u32)"A_2", 0x40000000, (u32)"c_teresa",
    (u32)"A_2", 0x40400000, (u32)"c_teresa", (u32)"A_2",
    0x40800000, (u32)"c_teresa", (u32)"A_2", 0x40A00000,
    (u32)"c_teresa", (u32)"A_2", 0x40C00000, (u32)"c_teresa",
    (u32)"A_2", 0x40E00000, (u32)"c_teresa", (u32)"A_2",
    0x41000000, (u32)"c_teresa", (u32)"A_2", 0x41100000,
    (u32)"c_teresa", (u32)"A_2", 0x41200000, (u32)"c_teresa",
    (u32)"A_2", 0x41300000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: mt$859 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 mt$859[12] = {
//     0x41200000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x41200000, 0x00000000, 0x3DCCCCCD,
//     0x00000000, 0x00000000, 0x41200000, 0x00000000,
// };














EVT_BEGIN(senteresa_evt)
    CALL(evt_ext_entry, 200, PTR(senteresa_ext_entry_data), PTR(senteresa_ext_init), PTR(senteresa_ext_main), PTR(senteresa_ext_dispent))
    LOOP(0)
        CALL(senteresa_get_seq, 199, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 4)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GW(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(600)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(8))
            CALL(senteresa_get_rot_center, LW(0), LW(1), LW(2))
            SET_VALUE(LW(3), LW(0))
            ADD(LW(3), LW(8))
            ADD(LW(3), 65)
            CALL(evt_sub_random, 65, LW(4))
            ADD(LW(3), LW(4))
            SET_VALUE(LW(5), LW(1))
            ADD(LW(5), 5)
            CALL(evt_sub_random, 40, LW(6))
            ADD(LW(5), LW(6))
            SET_VALUE(LW(7), LW(2))
            SUBTRACT(LW(7), 90)
            CALL(evt_msg_toge, 3, LW(3), LW(5), LW(7))
            CALL(evt_msg_print, 0, STRING("stg4_jin_58"), 0, 0)
            CALL(evt_sub_random, 200, LW(0))
            ADD(LW(0), 100)
            WAIT_MS(LW(0))
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(600)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(8))
            CALL(senteresa_get_rot_center, LW(0), LW(1), LW(2))
            SET_VALUE(LW(3), LW(0))
            ADD(LW(3), LW(8))
            SUBTRACT(LW(3), 32)
            CALL(evt_sub_random, 65, LW(4))
            ADD(LW(3), LW(4))
            SET_VALUE(LW(5), LW(1))
            ADD(LW(5), 5)
            CALL(evt_sub_random, 70, LW(6))
            ADD(LW(5), LW(6))
            SET_VALUE(LW(7), LW(2))
            SUBTRACT(LW(7), 90)
            CALL(evt_msg_toge, 3, LW(3), LW(5), LW(7))
            CALL(evt_msg_print, 0, STRING("stg4_jin_59"), 0, 0)
            CALL(evt_sub_random, 200, LW(0))
            ADD(LW(0), 100)
            WAIT_MS(LW(0))
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(900)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(8))
            CALL(senteresa_get_rot_center, LW(0), LW(1), LW(2))
            SET_VALUE(LW(3), LW(0))
            ADD(LW(3), LW(8))
            SUBTRACT(LW(3), 110)
            CALL(evt_sub_random, 45, LW(4))
            ADD(LW(3), LW(4))
            SET_VALUE(LW(5), LW(1))
            ADD(LW(5), 45)
            CALL(evt_sub_random, 30, LW(6))
            ADD(LW(5), LW(6))
            SET_VALUE(LW(7), LW(2))
            SUBTRACT(LW(7), 90)
            CALL(evt_msg_toge, 3, LW(3), LW(5), LW(7))
            CALL(evt_msg_print, 0, STRING("stg4_jin_60"), 0, 0)
            CALL(evt_sub_random, 300, LW(0))
            ADD(LW(0), 100)
            WAIT_MS(LW(0))
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(750)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(8))
            CALL(senteresa_get_rot_center, LW(0), LW(1), LW(2))
            SET_VALUE(LW(3), LW(0))
            ADD(LW(3), LW(8))
            ADD(LW(3), 55)
            CALL(evt_sub_random, 75, LW(4))
            ADD(LW(3), LW(4))
            SET_VALUE(LW(5), LW(1))
            SUBTRACT(LW(5), 80)
            CALL(evt_sub_random, 30, LW(6))
            SUBTRACT(LW(5), LW(6))
            SET_VALUE(LW(7), LW(2))
            SUBTRACT(LW(7), 90)
            CALL(evt_msg_toge, 3, LW(3), LW(5), LW(7))
            CALL(evt_msg_print, 0, STRING("stg4_jin_59"), 0, 0)
            CALL(evt_sub_random, 200, LW(0))
            ADD(LW(0), 100)
            WAIT_MS(LW(0))
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(900)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(8))
            CALL(senteresa_get_rot_center, LW(0), LW(1), LW(2))
            SET_VALUE(LW(3), LW(0))
            ADD(LW(3), LW(8))
            SUBTRACT(LW(3), 110)
            CALL(evt_sub_random, 75, LW(4))
            ADD(LW(3), LW(4))
            SET_VALUE(LW(5), LW(1))
            SUBTRACT(LW(5), 80)
            CALL(evt_sub_random, 30, LW(6))
            SUBTRACT(LW(5), LW(6))
            SET_VALUE(LW(7), LW(2))
            SUBTRACT(LW(7), 90)
            CALL(evt_msg_toge, 3, LW(3), LW(5), LW(7))
            CALL(evt_msg_print, 0, STRING("stg4_jin_58"), 0, 0)
            CALL(evt_sub_random, 300, LW(0))
            ADD(LW(0), 100)
            WAIT_MS(LW(0))
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    LOOP(0)
        PEEK_INT(LW(0), PTR(g_nNumHuttobasu))
        IF_INT_GREATER_OR_EQUAL(LW(0), 20)
            SET_VALUE(GW(0), 1)
            BREAK_LOOP
        END_IF
        CALL(senteresa_mario_spd)
        WAIT_FRAMES(1)
        PEEK_INT(LW(0), PTR(g_nNumHarituku))
        IF_INT_GREATER_OR_EQUAL(LW(0), 20)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_mario_normalize)
            CALL(evt_mario_cont_onoff, 0)
            CALL(evt_party_cont_onoff, 0, 0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
            CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
            CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 0, 11)
            CALL(evt_mario_set_pose, STRING("M_I_O"))
            CALL(evt_mario_cam_y_off)
            CALL(senteresa_turiageru)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
            LOOP(150)
                ADD_FLOAT(LW(1), FLOAT(1.0f))
                CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
                CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            SET_VALUE(GSWF(2227), 1)
            CALL(evt_mario_normalize)
            IF_INT_EQUAL(GSW(0), 201)
                CALL(evt_bero_mapchange, STRING("usu_00"), STRING("evt_majo3"))
            ELSE
                CALL(evt_bero_mapchange, STRING("gra_06"), STRING("n_bero"))
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(senteresa_tikaduku_stop)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(2500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 60)
    CALL(evt_npc_set_position, STRING("中央テレサ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_scale, STRING("中央テレサ"), 1, 1, 1)
    CALL(evt_npc_flag_onoff, 1, STRING("中央テレサ"), 0x80000000)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_get_position, STRING("中央テレサ"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(1), 20)
        CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), 0, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
        CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 1000, 11)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 0)
    LOOP(25)
        ADD(LW(0), 10)
        CALL(evt_npc_set_color, STRING("中央テレサ"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_color, STRING("中央テレサ"), 255, 255, 255, 255)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg4_jin_61"), 0, STRING("中央テレサ"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("中央テレサ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 20)
    CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), 0, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    ADD(LW(5), 160)
    ADD(LW(8), 160)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 2000, 11)
        WAIT_MS(1000)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("中央テレサ"), LW(2), LW(3), LW(4))
    SET_VALUE(LW(5), LW(3))
    ADD(LW(5), 200)
    CALL(evt_sub_intpl_msec_init, 11, LW(3), LW(5), 2000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_npc_set_position, STRING("中央テレサ"), LW(2), LW(0), LW(4))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_npc_get_position, STRING("中央テレサ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_dir, STRING("中央テレサ"), LW(3))
    CALL(evt_npc_set_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("アトミックテレサ"), LW(3))
    CALL(senteresa_syugo)
    LOOP(0)
        PEEK_INT(LW(0), PTR(g_nNumEndProc))
        IF_INT_EQUAL(LW(0), 200)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_get_position, STRING("中央テレサ"), LW(0), LW(1), LW(2))
    CALL(teresa_eff, LW(0), LW(1), LW(2))
    WAIT_MS(500)
    CALL(senteresa_dispoff)
    CALL(evt_npc_get_position, STRING("中央テレサ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 25)
    CALL(evt_npc_set_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 1, STRING("中央テレサ"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("アトミックテレサ"), 128)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg4_jin_62"), 0, STRING("アトミックテレサ"))
    CALL(evt_cam3d_evt_off, 400, 11)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_O"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    ADD(LW(2), 3)
    CALL(evt_npc_glide_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2), 0, FLOAT(450.0f), 1, 11, 0)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    BEGIN_INLINE_EVENT
        LOOP(100)
            CALL(evt_npc_get_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 4)
            CALL(evt_npc_set_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            WAIT_MS(10)
            CALL(evt_npc_get_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            ADD(LW(1), 4)
            CALL(evt_npc_set_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            WAIT_MS(10)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(50)
            CALL(evt_npc_get_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 4)
            CALL(evt_npc_set_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            WAIT_MS(20)
            CALL(evt_npc_get_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            ADD(LW(0), 4)
            CALL(evt_npc_set_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
            WAIT_MS(20)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(300)
    SET_VALUE(GW(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 110)
        ADD(LW(1), 70)
        SUBTRACT(LW(2), 90)
        CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_jin_63"), 0, 0)
        WAIT_MS(750)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 70)
        SUBTRACT(LW(1), 90)
        SUBTRACT(LW(2), 90)
        CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_jin_64"), 0, 0)
        WAIT_MS(750)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(250)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 30)
        ADD(LW(1), 30)
        SUBTRACT(LW(2), 90)
        CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_jin_65"), 0, 0)
        WAIT_MS(750)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 40)
        SUBTRACT(LW(1), 20)
        SUBTRACT(LW(2), 90)
        CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_jin_64"), 0, 0)
        WAIT_MS(750)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 80)
        SUBTRACT(LW(1), 100)
        SUBTRACT(LW(2), 90)
        CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_jin_65"), 0, 0)
        WAIT_MS(750)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 105)
        ADD(LW(1), 75)
        SUBTRACT(LW(2), 90)
        CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_jin_63"), 0, 0)
        WAIT_MS(750)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("アトミックテレサ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_battle_info, STRING("アトミックテレサ"), 7)
    CALL(evt_npc_battle_start, STRING("アトミックテレサ"))
    CALL(evt_npc_wait_battle_end)
    ADD(LW(1), 50)
    CALL(evt_item_entry, STRING("item_99"), 326, LW(0), LW(1), LW(2), 4, GSWF(2242), 0)
    CALL(evt_item_flag_on, STRING("item_99"), 256)
    SET_VALUE(GSWF(2242), 1)
    CALL(senteresa_huttobu)
    CALL(evt_npc_flag_onoff, 1, STRING("アトミックテレサ"), 128)
    LOOP(0)
        PEEK_INT(LW(0), PTR(g_nNumEndProc))
        IF_INT_EQUAL(LW(0), 200)
            SET_VALUE(GW(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(300)
    CALL(senteresa_end)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSWF(2226), 1)
    RETURN
EVT_END()

EVT_BEGIN(jin_00_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -500, 260, -200, 0, 0)
    IF_INT_LESS(GSW(0), 194)
        SET_VALUE(LW(0), PTR(bero_entry_data))
        CALL(evt_bero_get_info)
        SPAWN_EVENT(first_jin00)
    ELSE
        SET_VALUE(LW(0), PTR(bero_entry_data))
        CALL(evt_bero_get_info)
        CALL_CHILD(evt_bero_info_run)
    END_IF
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_LESS(GSW(0), 198)
        CALL(evt_item_entry, STRING("key00"), 22, -555, 200, 190, 0, -1, PTR(evt_key))
    END_IF
    IF_INT_LESS(GSW(0), 199)
        CALL(evt_mobj_lock, STRING("mobj_lock00"), 22, -435, 210, -245, 0, PTR(lock_item_check), PTR(evt_unlock), GSWF(2228))
        SET_VALUE(LW(0), STRING("n_bero_2"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    SET_VALUE(GW(0), -1)
    SET_VALUE(GW(1), -1)
    IF_INT_EQUAL(GSW(0), 196)
        SPAWN_EVENT(tyuou_teresa_kusukusu)
        SPAWN_EVENT(tyuou_teresa_look_mario)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 197)
        IF_INT_EQUAL(GSWF(2226), 0)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                WAIT_MS(1000)
                SPAWN_EVENT(senteresa_evt)
            END_INLINE_EVENT
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(2226), 0)
        CALL(evt_npc_check_delete, STRING("テレサ１"))
        CALL(evt_npc_check_delete, STRING("テレサ２"))
        CALL(evt_npc_check_delete, STRING("テレサ３"))
        CALL(evt_npc_check_delete, STRING("テレサ４"))
        CALL(evt_npc_check_delete, STRING("テレサ５"))
        CALL(evt_npc_check_delete, STRING("テレサ８"))
    END_IF
    CALL(evt_hit_bind_mapobj, STRING("a_oki01"), STRING("oki01"))
    IF_INT_LESS(GSW(0), 195)
        SET_VALUE(GW(10), 0)
        CALL(evt_run_case_evt, 10, 1, STRING("A_dai"), 0, PTR(push_evt), 0)
        CALL(evt_run_case_evt, 11, 1, STRING("A_dai"), 0, PTR(nopush_evt), 0)
    ELSE
        CALL(evt_mapobj_trans, 1, STRING("oki01"), FLOAT(50.0f), 0, 0)
        CALL(evt_hit_bind_update, STRING("a_oki01"))
        CALL(evt_hitobj_onoff, STRING("a_futa"), 0, 0)
    END_IF
    CALL(evt_map_playanim, STRING("hikari"), 1, 0)
    IF_INT_EQUAL(GSW(0), 201)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("w_bero"), PTR(bero_stg4majo3nin))
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("e_bero_2"))
        CALL(evt_cam_road_reset)
    END_IF
    CALL(evt_hitobj_attr_onoff, 0, 0, STRING("a_futa"), 4)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("hahen"), 2)
    RETURN
EVT_END()













// clang-format on
USER_FUNC(get_npc_color) {
    s32* args = event->args;
    NpcEntry* npc = npcNameToPtr((const char*)evtGetValue(event, args[0]));

    evtSetValue(event, args[1], npc->color.r);
    evtSetValue(event, args[2], npc->color.g);
    evtSetValue(event, args[3], npc->color.b);
    evtSetValue(event, args[4], npc->color.a);
    return EVT_RETURN_DONE;
}

s32 randn(s32 min, s32 max) {
    return min + rand() % (max - min);
}

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;
    f32 d = max - min;

    return r * d + min;
}

f32 computeAngle(ExtPose* pose, NpcEntry* npc) {
    Mtx* m = pose->matrix;
    f32 angle = angleABf(npc->position.x, npc->position.z, (*m)[0][3], (*m)[2][3]);

    if (0.0f <= angle && angle < 90.0f) {
        return 90.0f - angle;
    }
    if (90.0f <= angle && angle < 180.0f) {
        return 360.0f - (angle - 90.0f);
    }
    if (180.0f <= angle && angle < 270.0f) {
        return 180.0f + (270.0f - angle);
    }
    return 90.0f + (360.0f - angle);
}

void computeMtx(ExtPose* pose, f32 angle) {
    Mtx m;

    PSMTXRotRad(m, 0x59, angle * 0.017453292f);
    (*pose->matrix)[0][0] = m[0][0];
    (*pose->matrix)[0][1] = m[0][1];
    (*pose->matrix)[0][2] = m[0][2];
    (*pose->matrix)[1][0] = m[1][0];
    (*pose->matrix)[1][1] = m[1][1];
    (*pose->matrix)[1][2] = m[1][2];
    (*pose->matrix)[2][0] = m[2][0];
    (*pose->matrix)[2][1] = m[2][1];
    (*pose->matrix)[2][2] = m[2][2];
}

s32 computeAlpha(TeresaEntry* p, NpcEntry* npc, f32 angle) {
    f32 a;

    if (angle > 180.0f) {
        angle = 360.0f - angle;
    }
    a = npc->moveDir - 180.0f;
    if (a < 0.0f) {
        a += 360.0f;
    }
    if (a < 90.0f) {
        if (angle < 90.0f) {
            p->field_0x40 = (u8)(s32)(128.0f * (angle / 90.0f) + 128.0f);
            return 0;
        } else {
            p->field_0x40 = 0xFF;
            return 1;
        }
    } else {
        if (angle > 90.0f) {
            p->field_0x40 = (u8)(s32)(128.0f * ((180.0f - angle) / 90.0f) + 128.0f);
            return 0;
        } else {
            p->field_0x40 = 0xFF;
            return 1;
        }
    }
}

static void computePos(TeresaEntry* p, Vec* out) {
    Mtx m;
    Vec v;

    v.x = p->field_0x10;
    v.y = 0.0f;
    v.z = p->field_0x18;
    PSMTXRotRad(m, 0x59, p->field_0x0C * 0.017453292f);
    PSMTXMultVec(m, &v, out);
}

static void senteresa_ext_init(void) {
    ExtPose* pose = extGetPosePtr();
    MarioWork* mario = marioGetPtr();
    s32 count = extGetPoseNum();
    s32 i;

    pTeresa = _mapAlloc(0x3520);
    for (i = 0; i < count; i++, pose++) {
        pose->flags = 3;
        PSMTXIdentity(*pose->matrix);
        pTeresa[i].field_0x0C = 0.0f;
        pTeresa[i].field_0x3C = (i % 100 + 1) * 3;
        pTeresa[i].field_0x40 = 0;
        pTeresa[i].field_0x41 = 0;
        pTeresa[i].field_0x20 = randf(0.0f, 0.1f);
        pTeresa[i].field_0x10 = randf(-300.0f, 300.0f);
        pTeresa[i].field_0x14 = 0.0f;
        pTeresa[i].field_0x18 = randf(-235.0f, 235.0f);
        (*pose->matrix)[0][3] = mario->position.x + pTeresa[i].field_0x10;
        (*pose->matrix)[1][3] = randf(10.0f, 400.0f);
        (*pose->matrix)[2][3] = mario->position.z + pTeresa[i].field_0x18;
        if (mario->position.x < (*pose->matrix)[0][3]) {
            pose->field_0x4 = 90.0f;
        } else {
            pose->field_0x4 = 270.0f;
        }
    }
    g_nNumEndProc[0] = 0;
    g_nNumHarituku[0] = 0;
    g_nNumHuttobasu[0] = 0;
    g_nMarioMoveCnt[0] = 0;
    g_fSpeed[0] = 0.0f;
    g_bMove[0] = 0;
    g_vecRotCenter.x = mario->position.x;
    g_vecRotCenter.y = 0.0f;
    g_vecRotCenter.z = mario->position.z;
}

static void senteresa_ext_main(void) {
    ExtPose* pose = extGetPosePtr();
    MarioWork* mario = marioGetPtr();
    s32 count = extGetPoseNum();
    s32 i;
    s32 hammer = 0;
    s32 bero;
    s32 party;
    Vec pos;
    Vec v;

    if (marioGetAngleKaitenHammer() > 0.0f) {
        hammer = 1;
    }
    PSVECSubtract(&mario->position, &g_vecRotCenter, &v);
    if (mario->baseSpeed > 0.0f) {
        g_bMove[0] = 1;
    }
    if ((s32)g_bMove[0] != 0) {
        g_nMarioMoveCnt[0]++;
    }
    if ((s32)g_nMarioMoveCnt[0] > 12) {
        f32 f1;

        g_fSpeed[0] += 0.01f;
        if (g_fSpeed[0] > 2.6f) {
            g_fSpeed[0] = 2.6f;
        }
        if (v.x == 0.0f && v.y == 0.0f && v.z == 0.0f) {
            g_vecRotCenter = mario->position;
            if (mario->baseSpeed == 0.0f) {
                g_fSpeed[0] = 0.0f;
                g_bMove[0] = 0;
                g_nMarioMoveCnt[0] = 0;
            }
        } else {
            f32 mag = PSVECMag(&v);

            if (mag > 50.0f) {
                f1 = 2.6f;
            } else {
                f1 = 2.6f * mag / 50.0f;
            }
            if (g_fSpeed[0] > f1) {
                g_fSpeed[0] = f1;
            }
            PSVECNormalize(&v, &v);
            PSVECScale(&v, &v, g_fSpeed[0]);
            PSVECAdd(&g_vecRotCenter, &v, &g_vecRotCenter);
            PSVECSubtract(&g_vecRotCenter, &mario->position, &pos);
            if (PSVECMag(&pos) < 0.3f) {
                g_vecRotCenter = mario->position;
                if (mario->baseSpeed == 0.0f) {
                    g_fSpeed[0] = 0.0f;
                    g_bMove[0] = 0;
                    g_nMarioMoveCnt[0] = 0;
                }
            }
        }
    }
    bero = (bero_get_BeroEXEC() & 3) != 0;
    party = 0;
    if (partyGetPtr(0) != NULL) {
        if (partyGetPtr(0)->flags & 0x100) {
            party = 1;
        }
    }
    for (i = 0; i < count; i++, pose++) {
        switch (pTeresa[i].field_0x41) {
            case 0:
                pose->field_0x10 = 0;
                pTeresa[i].field_0x3C--;
                if (pTeresa[i].field_0x3C == 0) {
                    if (i == 0) {
                        Vec pos = {0.0f, 200.0f, 0.0f};

                        psndSFXOn_3D((const char*)0x5D3, &pos);
                    }
                    pTeresa[i].field_0x41++;
                }
                break;
            case 1: {
                u8 old;
                f32 angle;

                pose->field_0x10 = (gp->retraceCount / 5 + i * i) & 0xF;
                if (mario->position.x < (*pose->matrix)[0][3]) {
                    pose->field_0x4 = 90.0f;
                } else {
                    pose->field_0x4 = 270.0f;
                }
                computePos(&pTeresa[i], &pos);
                (*pose->matrix)[0][3] = g_vecRotCenter.x + pos.x;
                (*pose->matrix)[2][3] = g_vecRotCenter.z + pos.z;
                pos.y = (*pose->matrix)[1][3];
                (*pose->matrix)[1][3] = 0.4f * (f32)sin(1.2f * pTeresa[i].field_0x1C) + pos.y;
                pTeresa[i].field_0x1C += 0.08f;
                if (pTeresa[i].field_0x1C > 3.1415927f) {
                    pTeresa[i].field_0x1C -= 3.1415927f;
                }
                angle = computeAngle(pose, (NpcEntry*)mario);
                pTeresa[i].field_0x40 += 3;
                old = pTeresa[i].field_0x40;
                computeAlpha(&pTeresa[i], (NpcEntry*)mario, angle);
                if (pTeresa[i].field_0x40 < old) {
                    pTeresa[i].field_0x41++;
                    g_nNumEndProc[0]++;
                    if (i == 0) {
                        Vec pos = {0.0f, 200.0f, 0.0f};

                        psndSFXOn_3D((const char*)0x5D3, &pos);
                    }
                } else {
                    pTeresa[i].field_0x40 = old;
                    if (pTeresa[i].field_0x40 >= 0xFA) {
                        pTeresa[i].field_0x40 = 0xFF;
                        pTeresa[i].field_0x41++;
                        g_nNumEndProc[0]++;
                        if (i == 0) {
                            Vec pos = {0.0f, 200.0f, 0.0f};

                            psndSFXOn_3D((const char*)0x5D3, &pos);
                        }
                    }
                }
                break;
            }
            case 2: {
                f32 angle;

                pose->field_0x10 = (gp->retraceCount / 5 + i * i) & 0xF;
                if (mario->position.x < (*pose->matrix)[0][3]) {
                    pose->field_0x4 = 90.0f;
                } else {
                    pose->field_0x4 = 270.0f;
                }
                computePos(&pTeresa[i], &pos);
                (*pose->matrix)[0][3] = g_vecRotCenter.x + pos.x;
                (*pose->matrix)[2][3] = g_vecRotCenter.z + pos.z;
                pos.y = (*pose->matrix)[1][3];
                (*pose->matrix)[1][3] = 0.4f * (f32)sin(1.2f * pTeresa[i].field_0x1C) + pos.y;
                pTeresa[i].field_0x1C += 0.08f;
                if (pTeresa[i].field_0x1C > 3.1415927f) {
                    pTeresa[i].field_0x1C -= 3.1415927f;
                }
                angle = computeAngle(pose, (NpcEntry*)mario);
                computeAlpha(&pTeresa[i], (NpcEntry*)mario, angle);
                if (g_nNumEndProc[0] == 200) {
                    pTeresa[i].field_0x3C = i + 1;
                    pTeresa[i].field_0x41++;
                }
                break;
            }
            case 3:
                pose->field_0x10 = (gp->retraceCount / 5 + i * i) & 0xF;
                if (mario->position.x < (*pose->matrix)[0][3]) {
                    pose->field_0x4 = 90.0f;
                } else {
                    pose->field_0x4 = 270.0f;
                }
                computePos(&pTeresa[i], &pos);
                (*pose->matrix)[0][3] = g_vecRotCenter.x + pos.x;
                (*pose->matrix)[2][3] = g_vecRotCenter.z + pos.z;
                pos.y = (*pose->matrix)[1][3];
                (*pose->matrix)[1][3] = 0.4f * (f32)sin(1.2f * pTeresa[i].field_0x1C) + pos.y;
                pTeresa[i].field_0x1C += 0.08f;
                pTeresa[i].field_0x3C--;
                if (pTeresa[i].field_0x3C == 0) {
                    pose->flags &= ~2;
                    pTeresa[i].field_0x3C = i * 0x1E + rand() % 10 + 0x127;
                    pTeresa[i].field_0x41++;
                }
                break;
            case 4: {
                f32 angle;
                s32 ret;

                pose->field_0x10 = (gp->retraceCount + i * i) % 10 + 0x10;
                computePos(&pTeresa[i], &pos);
                pos.y = (*pose->matrix)[1][3];
                (*pose->matrix)[0][3] = g_vecRotCenter.x + pos.x;
                (*pose->matrix)[1][3] = pos.y;
                (*pose->matrix)[2][3] = g_vecRotCenter.z + pos.z;
                pTeresa[i].field_0x1C += 0.08f;
                if (pTeresa[i].field_0x1C > 3.1415927f) {
                    pTeresa[i].field_0x1C -= 3.1415927f;
                }
                pTeresa[i].field_0x20 += 0.01f;
                if (pTeresa[i].field_0x20 > 1.2f) {
                    pTeresa[i].field_0x20 = 1.2f;
                }
                pTeresa[i].field_0x0C += pTeresa[i].field_0x20;
                if (pTeresa[i].field_0x0C > 360.0f) {
                    pTeresa[i].field_0x0C -= 360.0f;
                }
                angle = computeAngle(pose, (NpcEntry*)mario);
                computeMtx(pose, angle);
                ret = computeAlpha(&pTeresa[i], (NpcEntry*)mario, angle);
                if (g_nNumHuttobasu[0] >= 20) {
                    break;
                }
                if (ret != 0) {
                    if (rand() % 1000 == 0x138) {
                        pTeresa[i].field_0x3C = 100;
                        pTeresa[i].field_0x41 = 9;
                    }
                }
                if (party) {
                    break;
                }
                if (g_nNumHuttobasu[0] >= 20) {
                    break;
                }
                pTeresa[i].field_0x3C--;
                if (pTeresa[i].field_0x3C == 0) {
                    pTeresa[i].field_0x1C = 0.0f;
                    pTeresa[i].field_0x41++;
                }
                break;
            }
            case 5: {
                f32 angle;

                pose->field_0x10 = (gp->retraceCount + i * i) % 10 + 0x10;
                pos.x = (*pose->matrix)[0][3];
                pos.y = (*pose->matrix)[1][3];
                pos.z = (*pose->matrix)[2][3];
                PSVECSubtract(&mario->position, &pos, &v);
                PSVECNormalize(&v, &v);
                PSVECScale(&v, &v, 1.8f);
                PSVECAdd(&pos, &v, &pos);
                (*pose->matrix)[0][3] = pos.x;
                (*pose->matrix)[1][3] = 0.4f * (f32)sin(1.2f * pTeresa[i].field_0x1C) + pos.y;
                (*pose->matrix)[2][3] = pos.z;
                pTeresa[i].field_0x1C += 0.08f;
                if (pTeresa[i].field_0x1C > 3.1415927f) {
                    pTeresa[i].field_0x1C -= 3.1415927f;
                }
                if (!party) {
                    PSVECSubtract(&pos, &mario->position, &pos);
                    if (PSVECMag(&pos) <= 25.0f) {
                        f32 y = (*pose->matrix)[1][3] - mario->position.y;

                        if (y > -2.0f && y < 10.0f) {
                            pTeresa[i].field_0x10 = (*pose->matrix)[0][3] - mario->position.x;
                            pTeresa[i].field_0x14 = (*pose->matrix)[1][3] - mario->position.y;
                            pTeresa[i].field_0x18 = (*pose->matrix)[2][3] - mario->position.z;
                            pTeresa[i].field_0x41++;
                            if (!bero) {
                                g_nNumHarituku[0]++;
                            }
                        }
                    }
                }
                angle = computeAngle(pose, (NpcEntry*)mario);
                computeMtx(pose, angle);
                computeAlpha(&pTeresa[i], (NpcEntry*)mario, angle);
                break;
            }
            case 6: {
                f32 angle;
                s32 ret;

                pose->field_0x10 = (gp->retraceCount + i * i) % 10 + 0x10;
                (*pose->matrix)[0][3] = mario->position.x + pTeresa[i].field_0x10;
                (*pose->matrix)[1][3] = mario->position.y + pTeresa[i].field_0x14;
                (*pose->matrix)[2][3] = mario->position.z + pTeresa[i].field_0x18;
                angle = computeAngle(pose, (NpcEntry*)mario);
                ret = computeAlpha(&pTeresa[i], (NpcEntry*)mario, angle);
                if (!hammer) {
                    break;
                }
                pos.x = (*pose->matrix)[0][3];
                pos.y = (*pose->matrix)[1][3];
                pos.z = (*pose->matrix)[2][3];
                PSVECSubtract(&pos, &mario->position, &v);
                PSVECNormalize(&v, &v);
                pTeresa[i].field_0x00 = 4.2f * v.x;
                pTeresa[i].field_0x08 = v.z > 0.0f ? randf(5.2f, 12.7f) * v.z : randf(-5.0f, -2.0f) * v.z;
                pTeresa[i].field_0x04 = randf(6.0f, 10.0f);
                pTeresa[i].field_0x41 = 10;
                computeMtx(pose, 0.0f);
                g_nNumHarituku[0]--;
                g_nNumHuttobasu[0]++;
                {
                    Vec sfx = {0.0f, 0.0f, 0.0f};

                    sfx.x = (*pose->matrix)[0][3];
                    sfx.y = (*pose->matrix)[1][3];
                    sfx.z = (*pose->matrix)[2][3];
                    psndSFXOn_3D((const char*)0x5D7, &sfx);
                }
                break;
            }
            case 9: {
                f32 angle;
                s32 ret;

                pose->field_0x10 = (s32)((100.0f - (f32)pTeresa[i].field_0x3C) / 100.0f * 12.0f) + 0x22;
                computePos(&pTeresa[i], &pos);
                pos.y = (*pose->matrix)[1][3];
                (*pose->matrix)[0][3] = g_vecRotCenter.x + pos.x;
                (*pose->matrix)[1][3] = 0.4f * (f32)sin(1.2f * pTeresa[i].field_0x1C) + pos.y;
                (*pose->matrix)[2][3] = g_vecRotCenter.z + pos.z;
                pTeresa[i].field_0x1C += 0.08f;
                if (pTeresa[i].field_0x1C > 3.1415927f) {
                    pTeresa[i].field_0x1C -= 3.1415927f;
                }
                pTeresa[i].field_0x0C += 1.2f;
                if (pTeresa[i].field_0x0C > 360.0f) {
                    pTeresa[i].field_0x0C -= 360.0f;
                }
                angle = computeAngle(pose, (NpcEntry*)mario);
                computeMtx(pose, angle);
                ret = computeAlpha(&pTeresa[i], (NpcEntry*)mario, angle);
                pTeresa[i].field_0x3C--;
                if (ret == 0 || pTeresa[i].field_0x3C == 0) {
                    pTeresa[i].field_0x3C = rand() % 300 + 200;
                    pTeresa[i].field_0x41 = 4;
                }
                break;
            }
            case 0xA:
                pose->field_0x10 = (gp->retraceCount + i * i) % 8 + 0x1A;
                (*pose->matrix)[0][3] += pTeresa[i].field_0x00;
                (*pose->matrix)[1][3] += pTeresa[i].field_0x04;
                (*pose->matrix)[2][3] += pTeresa[i].field_0x08;
                pTeresa[i].field_0x04 -= 0.3f;
                if ((*pose->matrix)[1][3] < -20.0f) {
                    pose->flags = 0;
                    pTeresa[i].field_0x41 = 0x63;
                }
                break;
            case 0x14: {
                f32 angle;

                pose->field_0x10 = (gp->retraceCount + i * i) % 10 + 0x10;
                computePos(&pTeresa[i], &pos);
                pos.y = (*pose->matrix)[1][3];
                (*pose->matrix)[0][3] = g_vecRotCenter.x + pos.x;
                (*pose->matrix)[1][3] = 0.4f * (f32)sin(1.2f * pTeresa[i].field_0x1C) + pos.y;
                (*pose->matrix)[2][3] = g_vecRotCenter.z + pos.z;
                pTeresa[i].field_0x1C += 0.08f;
                if (pTeresa[i].field_0x1C > 3.1415927f) {
                    pTeresa[i].field_0x1C -= 3.1415927f;
                }
                pTeresa[i].field_0x0C += 1.2f;
                if (pTeresa[i].field_0x0C > 360.0f) {
                    pTeresa[i].field_0x0C -= 360.0f;
                }
                angle = computeAngle(pose, (NpcEntry*)mario);
                computeMtx(pose, angle);
                computeAlpha(&pTeresa[i], (NpcEntry*)mario, angle);
                pTeresa[i].field_0x3C--;
                if (pTeresa[i].field_0x3C == 0) {
                    pose->flags = 3;
                    pTeresa[i].field_0x24 = (*pose->matrix)[0][3];
                    pTeresa[i].field_0x28 = (*pose->matrix)[1][3];
                    pTeresa[i].field_0x2C = (*pose->matrix)[2][3];
                    pTeresa[i].field_0x1C = 0.0f;
                    pTeresa[i].field_0x41++;
                }
                break;
            }
            case 0x15: {
                f32 t;

                pose->field_0x10 = (gp->retraceCount + i * i) % 10 + 0x10;
                t = pTeresa[i].field_0x1C;
                if (t >= 1.0f) {
                    pTeresa[i].field_0x41 = 0x16;
                    {
                        Vec pos = {0.0f, 0.0f, 0.0f};

                        pos.x = (*pose->matrix)[0][3];
                        pos.y = (*pose->matrix)[1][3];
                        pos.z = (*pose->matrix)[2][3];
                        psndSFXOn_3D((const char*)0x5D4, &pos);
                    }
                    g_nNumEndProc[0]++;
                } else {
                    pos.x = (1.0f - t) * pTeresa[i].field_0x24 + t * pTeresa[i].field_0x30;
                    pos.y = (1.0f - t) * pTeresa[i].field_0x28 + t * pTeresa[i].field_0x34;
                    pos.z = (1.0f - t) * pTeresa[i].field_0x2C + t * pTeresa[i].field_0x38;
                    (*pose->matrix)[0][3] = pos.x;
                    (*pose->matrix)[1][3] = 20.0f * (f32)sin(6.28f * pTeresa[i].field_0x1C) + pos.y;
                    (*pose->matrix)[2][3] = pos.z;
                    pTeresa[i].field_0x1C += 0.01f;
                }
                break;
            }
            case 0x16: {
                s32 m;

                pose->field_0x10 = (gp->retraceCount + i * i) % 10 + 0x10;
                m = pTeresa[i].field_0x3C & 7;
                if (m == 0) {
                    (*pose->matrix)[0][3] += 2.0f;
                } else if (m == 3) {
                    (*pose->matrix)[0][3] -= 2.0f;
                }
                pTeresa[i].field_0x3C++;
                break;
            }
            case 0x1E:
                pose->field_0x10 = (gp->retraceCount + i * i) % 8 + 0x1A;
                (*pose->matrix)[0][3] += pTeresa[i].field_0x00;
                (*pose->matrix)[1][3] += pTeresa[i].field_0x04;
                (*pose->matrix)[2][3] += pTeresa[i].field_0x08;
                pTeresa[i].field_0x04 -= 0.3f;
                if ((*pose->matrix)[1][3] < -10.0f) {
                    pose->flags = 0;
                    pTeresa[i].field_0x41 = 0x63;
                    g_nNumEndProc[0]++;
                }
                break;
            case 0x50:
                pose->field_0x10 = (gp->retraceCount + i * i) % 10 + 0x10;
                (*pose->matrix)[1][3] = mario->position.y + pTeresa[i].field_0x14;
                break;
            case 0x63:
                break;
            default:
                break;
        }
    }
}

static void senteresa_ext_dispent(void) {
    dispEntry(CAMERA_3D, 0.0f, 1, (DispCallback)senteresa_ext_disp, NULL);
}

void senteresa_loadRenderMode(void) {
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(1, 3, 1);
}

void senteresa_loadTev(void) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};

    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, color);
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
}

// Inlined into senteresa_ext_disp so its GXColor fog local lands in retail's 0x8 slot.
static inline void senteresa_setupTev(void) {
    GXColor fog = {0xFF, 0xFF, 0xFF, 0xFF};

    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, fog);
}

static void senteresa_ext_disp(void) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
    s32 count = extGetPoseNum();
    s32 i;

    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(1, 3, 1);
    extLoadVertex();
    extLoadTexture();
    senteresa_setupTev();
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
    for (i = 0; i < count; i++) {
        color.a = pTeresa[i].field_0x40;
        GXSetTevColor(1, color);
        extPoseDraw(i);
    }
    extLoadTextureExit();
}

static void senteresa_ext_shadow_disp(void) {
    static Mtx mt_859 = {
        {10.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 10.0f, 0.0f, 0.1f},
        {0.0f, 0.0f, 10.0f, 0.0f},
    };

    extLoadShadowMtx(mt_859);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
}

USER_FUNC(senteresa_get_seq) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], pTeresa[idx].field_0x41);
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_get_random_position) {
    s32* args = event->args;
    ExtPose* poses = extGetPosePtr();
    s32 num = extGetPoseNum();
    ExtPose* pose = &poses[rand() % num];

    evtSetFloat(event, args[0], (*pose->matrix)[0][3]);
    evtSetFloat(event, args[1], (*pose->matrix)[1][3]);
    evtSetFloat(event, args[2], (*pose->matrix)[2][3]);
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_tikaduku_stop) {
    s32 count = extGetPoseNum();
    s32 i;

    for (i = 0; i < count; i++) {
        if (pTeresa[i].field_0x41 != 10) {
            pTeresa[i].field_0x41 = 4;
            pTeresa[i].field_0x3C = 0x1869F;
        }
    }
    return EVT_RETURN_DONE;
}

static inline f32 randf_syugo(f32 min, f32 max) {
#ifdef MUST_MATCH
    // The f64 round-trip is a no-op MWCC folds away, but it makes the compiler create the int->float magic
    // constant's value before the other pool constants, giving this caller's f27-f31 colouring.
    f32 r = (f32)rand();
    f64 d = (f64)r;

    r = (f32)d / 32767.0f;
#else
    f32 r = (f32)rand() / 32767.0f;
#endif
    return r * (max - min) + min;
}

USER_FUNC(senteresa_syugo) {
    ExtPose* pose = extGetPosePtr();
    NpcEntry* npc = npcNameToPtr("中央テレサ");
    s32 count = extGetPoseNum();
    s32 i;

    g_nNumEndProc[0] = 0;
    for (i = 0; i < count; i++, pose++) {
        f32 a;
        f32 b;

        pTeresa[i].field_0x24 = (*pose->matrix)[0][3];
        pTeresa[i].field_0x28 = (*pose->matrix)[1][3];
        pTeresa[i].field_0x2C = (*pose->matrix)[2][3];
        a = randf_syugo(0.0f, 6.2831f);
        b = randf_syugo(0.0f, 25.0f);
        pTeresa[i].field_0x30 = b * (f32)cos(a) + npc->position.x;
        pTeresa[i].field_0x34 = b * (f32)sin(a) + npc->position.y;
        pTeresa[i].field_0x38 = npc->position.z;
        pTeresa[i].field_0x1C = 0.0f;
        pTeresa[i].field_0x3C = (i & 0xF) + (rand() % 10) * 30 + 1;
        pTeresa[i].field_0x40 = 0xFF;
        pTeresa[i].field_0x41 = 0x14;
        if (pTeresa[i].field_0x24 < pTeresa[i].field_0x30) {
            pose->field_0x4 = 270.0f;
        } else {
            pose->field_0x4 = 90.0f;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_dispoff) {
    ExtPose* pose = extGetPosePtr();
    s32 count = extGetPoseNum();
    s32 i;

    for (i = 0; i < count; i++, pose++) {
        pose->flags = 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_huttobu) {
    ExtPose* pose = extGetPosePtr();
    NpcEntry* npc = npcNameToPtr("アトミックテレサ");
    s32 count = extGetPoseNum();
    s32 i;

    g_nNumEndProc[0] = 0;
    for (i = 0; i < count; i++, pose++) {
        f32 mul;

        pose->flags = 3;
        (*pose->matrix)[0][3] = npc->position.x;
        (*pose->matrix)[1][3] = npc->position.y;
        (*pose->matrix)[2][3] = npc->position.z;
        pTeresa[i].field_0x00 = 4.0f * (f32)sin(pTeresa[i].field_0x0C);
        pTeresa[i].field_0x04 = randf(6.0f, 12.0f);
        pTeresa[i].field_0x08 = (f32)cos(pTeresa[i].field_0x0C);
        if (pTeresa[i].field_0x08 > 0.0f) {
            mul = randf(5.0f, 12.5f);
        } else {
            mul = randf(-5.0f, -2.0f);
        }
        pTeresa[i].field_0x08 *= mul;
        pTeresa[i].field_0x40 = 0xFF;
        pTeresa[i].field_0x41 = 0x1E;
        if (i & 1) {
            Vec pos = {0.0f, 0.0f, 0.0f};

            pos.x = (*pose->matrix)[0][3];
            pos.y = (*pose->matrix)[1][3];
            pos.z = (*pose->matrix)[2][3];
            psndSFXOn_3D((const char*)0x5D7, &pos);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_turiageru) {
    s32 n = extGetPoseNum();
    s32 i;

    g_nNumEndProc[0] = 0;
    for (i = 0; i < n; i++) {
        if (pTeresa[i].field_0x41 == 6) {
            pTeresa[i].field_0x41 = 0x50;
        } else if (pTeresa[i].field_0x41 != 9) {
            pTeresa[i].field_0x3C = 0x1869F;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_end) {
    extReset();
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_wait_end_seq) {
    return g_nNumEndProc[0] == 200 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

USER_FUNC(senteresa_get_rot_center) {
    s32* args = event->args;

    evtSetValue(event, args[0], (s32)g_vecRotCenter.x);
    evtSetValue(event, args[1], (s32)g_vecRotCenter.y);
    evtSetValue(event, args[2], (s32)g_vecRotCenter.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_mario_spd) {
    MarioWork* mario = marioGetPtr();
    f32 f = (f32)(20 - (s32)g_nNumHarituku[0]) / 20.0f;

    if (mario->currMotionId == 1) {
        mario->baseSpeed = mario->mBaseWalkSpeed * f;
    } else if (mario->currMotionId == 2) {
        mario->baseSpeed = mario->mBaseDashSpeed * f;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(teresa_eff) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    effTeresaEntry(0, x, y + 20.0f, z + 20.0f);
    return EVT_RETURN_DONE;
}

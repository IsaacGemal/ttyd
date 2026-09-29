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
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];
static void senteresa_ext_shadow_disp(void);  // TODO: real signature
static void senteresa_ext_disp(void);  // TODO: real signature
static void senteresa_ext_dispent(void);  // TODO: real signature
static void senteresa_ext_main(void);  // TODO: real signature
static void senteresa_ext_init(void);  // TODO: real signature
static void computePos(void);  // TODO: real signature
static void randf(void);  // TODO: real signature
s32 senteresa_evt[1248];
static u32 g_nNumEndProc[1];
u32 g_nNumHarituku[1];
u32 g_nNumHuttobasu[1];

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

// TODO: write get_npc_color (global, 0x80 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

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

// TODO: write senteresa_wait_end_seq (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

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

// TODO: write teresa_eff (global, 0x98 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_mario_spd (global, 0x94 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_get_rot_center (global, 0x94 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_end (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_turiageru (global, 0x84 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_huttobu (global, 0x2bc bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_dispoff (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_syugo (global, 0x278 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_tikaduku_stop (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_get_seq (global, 0x5c bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_ext_dispent (static, 0x40 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_ext_main (static, 0x278c bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_ext_init (static, 0x340 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

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

// TODO: type
u32 g_fSpeed[1] = {
    0x00000000,
};

// TODO: type
static u32 g_nNumEndProc[1] = {
    0x00000000,
};

// TODO: type
u32 g_nNumHarituku[1] = {
    0x00000000,
};

// TODO: type
u32 g_nNumHuttobasu[1] = {
    0x00000000,
};

// TODO: type
u32 g_nMarioMoveCnt[1] = {
    0x00000000,
};

// TODO: type
u32 g_bMove[1] = {
    0x00000000,
};

// TODO: type
u32 g_vecRotCenter[3] = {
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pTeresa[1] = {
    0x00000000,
};

// TODO: write senteresa_get_random_position (global, 0x94 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_ext_shadow_disp (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_ext_disp (static, 0x1c4 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_loadTev (global, 0x108 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write senteresa_loadRenderMode (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write computePos (static, 0x78 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write computeAlpha (global, 0xec bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write computeMtx (global, 0xac bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write computeAngle (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write randf (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it

// TODO: write randn (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/jin/asm/jin_00.s; this is where the original has it, just before the first script that uses it
// clang-format on

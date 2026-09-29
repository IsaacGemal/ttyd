#include "evt_lect_tik.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_lecture.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_mario_paper_plane[];
static void bgm_start_wait(void);  // TODO: real signature
static void evt_nomal_cange(void);  // TODO: real signature
static void lect_push_r_check(void);  // TODO: real signature
static USER_FUNC(evtflash);
static void lect_slit_check(void);  // TODO: real signature
static USER_FUNC(dai_moveti_get);
static USER_FUNC(dai_moveti_set);
static void lect_mobj_hit_nasi(void);  // TODO: real signature
static void effitemget_busaiku(void);  // TODO: real signature
static void effitemget_open_ptr(void);  // TODO: real signature
static void effitemget_open(void);  // TODO: real signature
static void effitemgetchange_cam_ptr(void);  // TODO: real signature
static void effitemgetchange_cam(void);  // TODO: real signature
static USER_FUNC(lect_map_cam_chg);
static void lect_map_cam_chg_sub(void);  // TODO: real signature
static USER_FUNC(lecture_button_set_pos);
static USER_FUNC(lecture_button_seq_wait);
static void lecture_button_get_icon_id(void);  // TODO: real signature
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_flag_on_off);
static void lecture_button_set_alpha(void);  // TODO: real signature
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_rtn);
static void lecture_button_disp(void);  // TODO: real signature
static USER_FUNC(x_flag_wait);
static USER_FUNC(x_flag_get);
static void x_flag_add(void);  // TODO: real signature
static USER_FUNC(x_flag_set);
static void lect_status_on(void);  // TODO: real signature
static void lect_status_off(void);  // TODO: real signature
static USER_FUNC(lect_mario_ido_sum);
static void lect_mario_ido_sub(void);  // TODO: real signature

// clang-format off
extend_data(148,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(149,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write lect_map_cam_chg (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(plane_majin_lecture_init)
    CALL(lect_set_systemlevel, 0)
    CALL(lect_test_systemlevel)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, -300, -600, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 0, 11)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pos, -300, -600, 0)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 0, 11)
    CALL(lect_map_cam_chg, STRING("hikou"), 7)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

// TODO: write evtflash (static, 0x22c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write dai_moveti_get (static, 0x70 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write dai_moveti_set (static, 0xb4 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_pos (static, 0xcc bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_seq_wait (static, 0x6c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_seq (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_flag_on_off (static, 0x88 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_icon_id (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_rtn (static, 0x184 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_wait (static, 0x68 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_get (static, 0x54 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_set (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mario_ido_sum (static, 0x278 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(plane_majin_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("hikou"))
        SET_VALUE(LW(1), STRING("a_asiba"))
        SET_VALUE(LW(5), STRING("a_hikou"))
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
        CALL(evt_hit_bind_mapobj, LW(5), LW(0))
        CALL(dai_moveti_set, FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
        LOOP(0)
            CALL(dai_moveti_get, LW(2), LW(3), LW(4))
            CALL(evt_mapobj_trans, 1, LW(0), LW(2), LW(3), LW(4))
            CALL(evt_hit_bind_update, LW(1))
            CALL(evt_hit_bind_update, LW(5))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1073741856)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 7)
    WAIT_FRAMES(1)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_cam_get_at, 4, LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 25)
            CALL(evt_npc_set_position, STRING("ペラ魔人"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_1"))
    WAIT_MS(200)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_majin_13"), 0, STRING("ペラ魔人"))
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    LOOP(4)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_LIGHT1"), LW(0), LW(1), LW(2), 0)
        CALL(evtflash)
    END_LOOP
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_majin_14"), 0, STRING("ペラ魔人"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(80.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 11)
    SET_FLOAT_VALUE(LW(1), FLOAT(2.66602f))
    CALL(dai_moveti_get, LW(0), LW(2), LW(3))
    LOOP(0)
        SUBTRACT_FLOAT(LW(0), LW(1))
        IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(-220.0f))
            SET_FLOAT_VALUE(LW(0), FLOAT(-220.0f))
            CALL(dai_moveti_set, LW(0), LW(2), LW(3))
            BREAK_LOOP
        ELSE
            CALL(dai_moveti_set, LW(0), LW(2), LW(3))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_majin_15"), 0, STRING("ペラ魔人"))
    LOOP(0)
        CALL(x_flag_set, 0, 1)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(0), STRING("hikou"))
            SET_VALUE(LW(1), STRING("a_hikou_ue"))
            LOOP(0)
                SPAWN_EVENT_GET_ID(PTR(evt_ride_airport), LW(2))
                WAIT_FRAMES(2)
                IS_EVENT_RUNNING(LW(2), LW(3))
                IF_INT_EQUAL(LW(3), 1)
                    BREAK_LOOP
                END_IF
            END_LOOP
            CALL(x_flag_wait, 0, 2)
            IS_EVENT_RUNNING(LW(2), LW(3))
            IF_INT_EQUAL(LW(3), 1)
                DELETE_EVENT(LW(2))
            END_IF
            LOOP(0)
                SPAWN_EVENT_GET_ID(PTR(evt_nonride_airport), LW(2))
                WAIT_FRAMES(2)
                IS_EVENT_RUNNING(LW(2), LW(3))
                IF_INT_EQUAL(LW(3), 1)
                    BREAK_LOOP
                END_IF
            END_LOOP
            IS_EVENT_RUNNING(LW(2), LW(3))
            IF_INT_EQUAL(LW(3), 1)
                DELETE_EVENT(LW(2))
            END_IF
        END_INLINE_EVENT
        CALL(x_flag_set, 1, 1)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                IF_INT_GREATER_OR_EQUAL(LW(0), -220)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            LOOP(0)
                CALL(x_flag_get, 1, LW(0))
                IF_INT_NOT_EQUAL(LW(0), 3)
                    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 0, 11)
                END_IF
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(evt_mario_mov_pos2, FLOAT(-200.0f), FLOAT(0.0f), FLOAT(80.0f))
        CALL(evt_msg_print_add, 0, STRING("mac_majin_15_1"))
        CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_icon_id, 0, 106)
        CALL(lecture_button_set_seq, 0, 2)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(x_flag_set, 0, 1)
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(lecture_button_set_icon_id, 0, 107)
                WAIT_MS(300)
                CALL(x_flag_get, 0, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                CALL(lecture_button_set_icon_id, 0, 106)
                WAIT_MS(300)
                CALL(x_flag_get, 0, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_INLINE_EVENT
        LOOP(0)
            CALL(evt_key_get_buttontrg, 0, LW(0))
            IF_BITS_SET(LW(0), 2048)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_continue)
        CALL(evt_mario_paper_plane, 1)
        CALL(x_flag_set, 0, 2)
        CALL(lecture_button_set_seq, 0, 3)
        BEGIN_INLINE_EVENT
            CALL(x_flag_set, 0, 1)
            LOOP(0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                IF_INT_LESS_OR_EQUAL(LW(1), -750)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(x_flag_set, 1, 3)
            WAIT_FRAMES(1)
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            CALL(dai_moveti_set, FLOAT(3000.0f), FLOAT(0.0f), FLOAT(0.0f))
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            SET_VALUE(LW(0), -300)
            SET_VALUE(LW(1), -490)
            SET_VALUE(LW(2), 0)
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_evt_set, FLOAT(-297.0f), FLOAT(-351.0f), FLOAT(457.0f), FLOAT(-297.0f), FLOAT(-449.0f), FLOAT(-101.8f), 1000, 0)
            CALL(evt_mario_cont_onoff, 0)
            CALL(lect_mario_ido_sum, 1000)
            CALL(evt_mario_cont_onoff, 1)
            CALL(dai_moveti_set, FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
            CALL(x_flag_set, 1, 1)
            LOOP(0)
                CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
                CALL(x_flag_get, 0, LW(3))
                IF_INT_NOT_EQUAL(LW(3), 1)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_flag_on_off, 0, 2, 0)
        WAIT_MS(300)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("mac_majin_16"), 0, STRING("ペラ魔人"))
        CALL(evt_msg_select, 0, STRING("mac_majin_17"))
        SET_VALUE(LW(3), LW(0))
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(1), -550)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(LW(3), 0)
            CALL(x_flag_set, 1, 2)
            WAIT_FRAMES(2)
            CALL(evt_cam3d_evt_set, FLOAT(-300.0f), FLOAT(-460.0f), FLOAT(457.0f), FLOAT(-300.0f), FLOAT(-558.0f), FLOAT(-101.8f), 1200, 0)
            CALL(x_flag_set, 0, 2)
            CALL(evt_mario_paper_plane, 0)
            CALL(evt_mario_wait_landon)
            CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(80.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 11)
            SET_FLOAT_VALUE(LW(1), FLOAT(2.66602f))
            CALL(dai_moveti_get, LW(0), LW(2), LW(3))
            LOOP(0)
                SUBTRACT_FLOAT(LW(0), LW(1))
                IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(-220.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(-220.0f))
                    CALL(dai_moveti_set, LW(0), LW(2), LW(3))
                    BREAK_LOOP
                ELSE
                    CALL(dai_moveti_set, LW(0), LW(2), LW(3))
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_msg_print_add, 0, STRING("mac_majin_18"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("mac_majin_18_1"))
    CALL(x_flag_set, 1, 2)
    WAIT_FRAMES(2)
    CALL(evt_cam3d_evt_set, FLOAT(-300.0f), FLOAT(-460.0f), FLOAT(457.0f), FLOAT(-300.0f), FLOAT(-558.0f), FLOAT(-101.8f), 1200, 0)
    CALL(x_flag_set, 0, 2)
    CALL(evt_mario_paper_plane, 0)
    CALL(evt_mario_wait_landon)
    CALL(evt_msg_print_add, 0, STRING("mac_majin_19"))
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 4)
    CALL(evt_npc_set_position, STRING("ペラ魔人"), 0, -1000, 0)
    RETURN
EVT_END()

// TODO: type
static u32 daiv[3] = {
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 lbtn[34] = {
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

// TODO: type
static u32 x_flag_data[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 wwstart[2] = {
    0x00000000, 0x00000000,
};

// TODO: write bgm_start_wait (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_nomal_cange (static, 0x74 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_push_r_check (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_slit_check (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mobj_hit_nasi (static, 0xb0 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_busaiku (static, 0x12c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_open_ptr (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemget_open (static, 0x3c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemgetchange_cam_ptr (static, 0x50 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write effitemgetchange_cam (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_map_cam_chg_sub (static, 0x298 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_get_icon_id (static, 0x58 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_set_alpha (static, 0x5c bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lecture_button_disp (static, 0xd0 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write x_flag_add (static, 0x60 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_status_on (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_status_off (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it

// TODO: write lect_mario_ido_sub (static, 0x14 bytes) here:
// its asm is in build/G8MJ01/tik/asm/event/evt_lect_tik.s; this is where the original has it, just before the first script that uses it
// clang-format on

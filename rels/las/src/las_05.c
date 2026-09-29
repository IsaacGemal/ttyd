#include "las_05.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
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
static USER_FUNC(make_name);
static USER_FUNC(set_party_y_pos);
static USER_FUNC(senkaronGetMarioPushedFlag);
static USER_FUNC(senkaronAttackStart);
static USER_FUNC(senkaronFlagOffMesPushing);
static USER_FUNC(senkaronFlagOnMesPushing);
static USER_FUNC(senkaronToEscape);
static void senkaron_ext_shadow_disp(void);  // TODO: real signature
static void senkaron_ext_disp(void);  // TODO: real signature
static void senkaron_ext_dispent(void);  // TODO: real signature
static void senkaron_ext_main(void);  // TODO: real signature
static void senkaron_ext_init(void);  // TODO: real signature
static void randf(void);  // TODO: real signature
static s32 key_evt[5];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_04", "e_bero", 5, 5, w_door_close, w_door_open},
    {"e_bero_1", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_06", "w_bero", 5, 5, e_door_close, e_door_open},
    {NULL},
};

// TODO: type
u8 kurokaron_prev_goal[4] = {
    0x05, 0x00, 0x00, 0x00,
};

// TODO: type
u32 kurokaron_move_list[12] = {
    0x43FA0000, 0xC3020000, 0x00000000, 0xC3480000,
    0xC3FA0000, 0xC3020000, 0xC3FA0000, 0x43020000,
    0x00000000, 0x43480000, 0x43FA0000, 0x43020000,
};

// TODO: write kurokaron_move_position (global, 0x84c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write kurokaron_get_move_pos (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write kurokaron_flag_check (global, 0x58 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(kurokaron_move)
    SET_VALUE(LW(3), 0)
    SET_FLOAT_VALUE(GW(0), FLOAT(70.0f))
    SET_VALUE(GW(1), 200)
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(13), 0)
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(15))
        LOOP(0)
            CALL(evt_sub_get_stopwatch, LW(15), LW(14))
            IF_INT_GREATER_OR_EQUAL(LW(14), 10000)
                ADD(LW(13), 1)
                IF_INT_EQUAL(LW(13), 6)
                    BREAK_LOOP
                END_IF
                SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(15))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_FLOAT_VALUE(GW(0), FLOAT(50.0f))
        SET_VALUE(GW(1), 300)
        DEBUG_PUT_MSG(STRING("kurokaron 1 minute late"))
    END_SIBLING_EVENT
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(kurokaron_get_move_pos, LW(0), LW(1))
        CALL(kurokaron_move_position, STRING("黒カロン"), LW(0), LW(1), GW(0))
        CALL(kurokaron_flag_check, 2, LW(2))
        IF_INT_NOT_EQUAL(LW(2), 0)
            IF_INT_EQUAL(LW(3), 0)
                IF_INT_EQUAL(LF(0), 0)
                    WAIT_MS(GW(1))
                    SET_VALUE(LF(0), 1)
                ELSE
                    CALL(evt_sub_random, 300, LW(5))
                    ADD(LW(5), GW(1))
                    WAIT_MS(LW(5))
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(3), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 senkaron_ext_entry_data[78] = {
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x00000000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x3F800000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x40000000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x40400000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x40800000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x40A00000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x40C00000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x40E00000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41000000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41100000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41200000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41300000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41400000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41500000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41600000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41700000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41800000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41880000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41900000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41980000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41A00000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41A80000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41B00000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41B80000,
    (u32)"c_karon_h", (u32)"KRN_D_1", 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 move_check_list[20] = {
    0x00000019, 0x00000034, 0x0000004F, 0x0000006B,
    0x00000088, 0x000000A6, 0x000000C5, 0x000000E5,
    0x00000107, 0x0000012A, 0x0000014F, 0x00000176,
    0x000001A0, 0x000001CD, 0x000001FE, 0x00000235,
    0x00000274, 0x000002BF, 0x00000326, 0x000003E8,
};

// TODO: mt$975 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 mt$975[12] = {
//     0x41200000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x41200000, 0x00000000, 0x3DCCCCCD,
//     0x00000000, 0x00000000, 0x41200000, 0x00000000,
// };

// TODO: write party_hit (global, 0x9c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write chk_mario_ctrloff (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write party_land_on (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write mario_land_on (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write set_party_y_pos (static, 0x54 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write get_party_y_pos (global, 0x58 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write set_mario_y_pos (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write get_mario_y_pos (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaronGetMarioPushedFlag (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaronAttackStart (static, 0x1c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaronFlagOffMesPushing (static, 0x1c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaronFlagOnMesPushing (static, 0x1c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaronToEscape (static, 0x1c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaron_ext_dispent (static, 0x64 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaron_ext_main (static, 0x14bc bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaron_ext_init (static, 0x24c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(senkaron_event)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_mario_key_onoff, 0)
        BEGIN_INLINE_EVENT
            LOOP(10)
                CALL(evt_npc_get_position, STRING("黒カロン"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KARON_APPEAR1"), LW(0), LW(1), LW(2), 0)
                WAIT_MS(200)
            END_LOOP
        END_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("黒カロン"), STRING("KRN_D_3"))
        WAIT_MS(300)
        CALL(evt_cam3d_evt_set, 550, 80, 310, 550, 30, -90, 300, 0)
        WAIT_MS(300)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg8_las_02"), 0, STRING("黒カロン"))
        CALL(evt_ext_entry, 1000, PTR(senkaron_ext_entry_data), PTR(senkaron_ext_init), PTR(senkaron_ext_main), PTR(senkaron_ext_dispent))
        CALL(senkaronFlagOffMesPushing)
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_mario_adjust_dir)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, STRING("fukidashi"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        WAIT_MS(1000)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_set_pose, STRING("M_N_5B"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_I_Y"))
        END_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("黒カロン"), STRING("KRN_D_4"))
        SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(11), FLOAT(10.0f))
        SET_FLOAT_VALUE(LW(12), FLOAT(-1.0f))
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_npc_get_position, STRING("黒カロン"), LW(13), LW(10), LW(14))
            ADD_FLOAT(LW(10), LW(11))
            ADD_FLOAT(LW(11), LW(12))
            IF_INT_LESS_OR_EQUAL(LW(10), 0)
                SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
                CALL(evt_npc_set_position, STRING("黒カロン"), LW(13), LW(10), LW(14))
                BREAK_LOOP
            END_IF
            CALL(evt_npc_set_position, STRING("黒カロン"), LW(13), LW(10), LW(14))
        END_LOOP
        CALL(evt_npc_set_anim, STRING("黒カロン"), STRING("KRN_R_1"))
        CALL(evt_cam3d_evt_off, 300, 0)
        CALL(senkaronAttackStart)
        CALL(senkaronFlagOnMesPushing)
        BEGIN_INLINE_EVENT
            WAIT_MS(600)
            CALL(evt_snd_sfxon, STRING("SFX_STG8_KARON_DOWN1"), 0)
            CALL(evt_snd_sfxon, STRING("SFX_STG8_KARON_MOVE1"), GW(13))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(400)
            CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 3500)
        END_INLINE_EVENT
        SET_VALUE(GF(0), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            CALL(evt_npc_flag_onoff, 1, STRING("黒カロン"), 32)
            CALL(kurokaron_get_move_pos, LW(0), LW(1))
            CALL(evt_npc_move_position, STRING("黒カロン"), LW(0), LW(1), 0, FLOAT(80.0f), 0)
            WAIT_FRAMES(1)
            CALL(evt_npc_flag_onoff, 0, STRING("黒カロン"), 32)
            SET_VALUE(GF(0), 1)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            CALL(evt_mario_set_dir, 270, 300, 0)
        END_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_print_party, STRING("stg8_las_03"))
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_print, 0, STRING("stg8_las_04"), 0, STRING("黒カロン"))
        SET_FLOAT_VALUE(LW(3), FLOAT(-1.5f))
        SET_FLOAT_VALUE(LW(4), FLOAT(0.001f))
        SET_VALUE(LW(5), 270)
        CALL(evt_mario_set_pose, STRING("M_I_O"))
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(0), 370)
                BREAK_LOOP
            END_IF
            ADD_FLOAT(LW(0), LW(3))
            ADD_FLOAT(LW(3), LW(4))
            IF_INT_GREATER_OR_EQUAL(LW(3), 0)
                SET_FLOAT_VALUE(LW(3), FLOAT(-0.1f))
            END_IF
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            ADD(LW(5), 10)
            IF_INT_GREATER_OR_EQUAL(LW(5), 360)
                SUBTRACT(LW(5), 360)
            END_IF
            CALL(evt_mario_set_dispdir, LW(5))
        END_LOOP
        CALL(senkaronFlagOffMesPushing)
        CALL(evt_npc_flag_onoff, 1, STRING("黒カロン"), 32)
        CALL(evt_npc_change_interrupt, STRING("黒カロン"), 1, PTR(kurokaron_move))
        CALL(evt_npc_restart_regular_event, STRING("黒カロン"))
        CALL(evt_mario_dispflag_onoff, 0, 2)
        CALL(evt_mario_key_onoff, 1)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        SET_VALUE(GF(0), 0)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(7), 0)
            LOOP(0)
                IF_INT_NOT_EQUAL(GF(0), 0)
                    BREAK_LOOP
                END_IF
                CALL(senkaronGetMarioPushedFlag, LW(6))
                IF_INT_NOT_EQUAL(LW(6), 0)
                    SET_VALUE(LW(7), 18)
                END_IF
                CALL(evt_mario_party_use_check, LW(6))
                IF_INT_NOT_EQUAL(LW(6), 0)
                    SET_VALUE(LW(7), 0)
                END_IF
                IF_INT_GREATER(LW(7), 0)
                    CALL(evt_mario_get_motion, LW(8))
                    IF_INT_EQUAL(LW(8), 0)
                        CALL(evt_mario_get_pose, LW(9))
                        IF_STR_NOT_EQUAL(LW(9), STRING("M_I_O"))
                            CALL(evt_mario_set_pose, STRING("M_I_O"))
                        END_IF
                        CALL(evt_mario_get_dispdir, LW(5))
                        ADD(LW(5), 10)
                        IF_INT_GREATER_OR_EQUAL(LW(5), 360)
                            SUBTRACT(LW(5), 360)
                        END_IF
                        CALL(evt_mario_set_dispdir, LW(5))
                        CALL(evt_party_set_dispdir, 0, LW(5))
                        SUBTRACT(LW(7), 1)
                        IF_INT_EQUAL(LW(7), 0)
                            CALL(evt_mario_set_pose, STRING("M_S_1"))
                        END_IF
                    ELSE
                        SET_VALUE(LW(7), 0)
                    END_IF
                ELSE
                    CALL(evt_mario_get_motion, LW(8))
                    IF_INT_EQUAL(LW(8), 0)
                        CALL(evt_mario_get_dispdir, LW(5))
                        SET_VALUE(LF(1), 0)
                        IF_INT_GREATER_OR_EQUAL(LW(5), 90)
                            IF_INT_LESS(LW(5), 270)
                                SET_VALUE(LF(1), 1)
                            END_IF
                        END_IF
                        IF_INT_EQUAL(LF(1), 0)
                            CALL(evt_mario_set_dispdir, 0)
                        ELSE
                            CALL(evt_mario_set_dispdir, 180)
                        END_IF
                    END_IF
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            SET_VALUE(LF(0), 0)
            SET_VALUE(LW(15), 0)
            LOOP(0)
                IF_INT_NOT_EQUAL(GF(0), 0)
                    CALL(evt_cam_letter_box_disable, 0)
                    BREAK_LOOP
                END_IF
                SET_VALUE(LW(5), 0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_sub_area_check, 0, 0, 0, 5, LW(0), LW(2), LW(3))
                ADD(LW(5), LW(3))
                CALL(evt_key_get_dir, 0, LW(0))
                CALL(evt_key_get_button, 0, LW(1))
                ADD(LW(0), LW(1))
                IF_INT_EQUAL(LW(0), 0)
                    ADD(LW(5), 1)
                END_IF
                SET_VALUE(LW(2), 0)
                CALL(evt_mario_party_use_check, LW(2))
                SWITCH(LW(2))
                    CASE_EQUAL(0)
                    ADD(LW(5), 1)
                    CASE_EQUAL(4)
                    ADD(LW(5), 1)
                END_SWITCH
                CALL(evt_mario_get_motion, LW(4))
                SWITCH(LW(4))
                    CASE_EQUAL(0)
                    ADD(LW(5), 1)
                    CASE_EQUAL(22)
                    ADD(LW(5), 1)
                    CASE_EQUAL(26)
                    ADD(LW(5), 1)
                END_SWITCH
                IF_INT_EQUAL(LW(5), 4)
                    IF_INT_EQUAL(LF(0), 0)
                        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(14))
                        SET_VALUE(LF(0), 1)
                    ELSE
                        CALL(evt_sub_get_stopwatch, LW(14), LW(0))
                        SWITCH(LW(15))
                            CASE_EQUAL(0)
                            IF_INT_GREATER_OR_EQUAL(LW(0), 1000)
                                CALL(evt_cam_letter_box_onoff, 1, 0)
                                CALL(evt_cam_letter_box_disable, 1)
                                CALL(evt_mario_cont_onoff, 0)
                                CALL(evt_party_cont_onoff, 0, 0)
                                CALL(evt_mario_cam_off)
                                ADD(LW(15), 1)
                            END_IF
                            CASE_EQUAL(1)
                            IF_INT_GREATER_OR_EQUAL(LW(0), 2000)
                                CALL(evt_msg_print_party, STRING("stg8_las_05"))
                                SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(14))
                                ADD(LW(15), 1)
                            END_IF
                            CASE_EQUAL(2)
                            IF_INT_GREATER_OR_EQUAL(LW(0), 3000)
                                CALL(evt_msg_print_party, STRING("stg8_las_06"))
                                CALL(evt_seq_set_seq, 5, 0, 0)
                            END_IF
                        END_SWITCH
                        IF_INT_GREATER(LW(15), 0)
                            CALL(get_mario_y_pos, LW(0))
                            CALL(get_party_y_pos, LW(1))
                            ADD_FLOAT(LW(0), FLOAT(-0.16602f))
                            ADD_FLOAT(LW(1), FLOAT(-0.16602f))
                            CALL(set_mario_y_pos, LW(0))
                            CALL(set_party_y_pos, LW(1))
                        END_IF
                    END_IF
                ELSE
                    CALL(mario_land_on)
                    CALL(party_land_on)
                    CALL(chk_mario_ctrloff, LW(0))
                    IF_INT_NOT_EQUAL(LW(0), 0)
                        CALL(evt_mario_cont_onoff, 1)
                        CALL(evt_mario_cam_on)
                        CALL(evt_party_cont_onoff, 0, 1)
                        CALL(evt_cam_letter_box_disable, 0)
                    END_IF
                    SET_VALUE(LF(0), 0)
                    SET_VALUE(LW(15), 0)
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(party_hit, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                BREAK_LOOP
            END_IF
            SET_VALUE(LF(0), 0)
            CALL(evt_party_get_status, 6, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LW(0), 2)
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LW(0), 3)
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LF(0), 0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_npc_get_position, STRING("黒カロン"), LW(3), LW(4), LW(5))
                CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(6))
                IF_INT_LESS_OR_EQUAL(LW(6), 30)
                    BREAK_LOOP
                END_IF
            END_IF
        END_LOOP
        SET_VALUE(GF(0), 1)
        WAIT_FRAMES(1)
        CALL(mario_land_on)
        CALL(party_land_on)
        CALL(evt_mario_adjust_dir)
        CALL(evt_mario_set_motion, 0)
        CALL(chk_mario_ctrloff, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_mario_cont_onoff, 1)
            CALL(evt_mario_cam_on)
            CALL(evt_party_cont_onoff, 0, 1)
        END_IF
        CALL(evt_npc_get_reglid, STRING("黒カロン"), LW(0))
        DELETE_EVENT(LW(0))
        CALL(evt_npc_change_interrupt, STRING("黒カロン"), 1, 0)
        CALL(evt_npc_set_stay_anim, STRING("黒カロン"))
        CALL(senkaronFlagOnMesPushing)
        CALL(evt_snd_sfxoff, GW(13))
        WAIT_FRAMES(1)
        CALL(evt_npc_set_battle_info, STRING("黒カロン"), 7)
        CALL(evt_npc_battle_start, STRING("黒カロン"))
        CALL(evt_npc_wait_battle_end)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
        CALL(senkaronToEscape)
        SET_VALUE(GSW(0), 383)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_on)
        CALL(evt_npc_get_position, STRING("黒カロン"), LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(evt_sub_get_dir, LW(3), LW(5), LW(0), LW(2), LW(6))
        CALL(evt_item_entry, STRING("key"), 46, LW(0), LW(1), LW(2), 4, -1, PTR(key_evt))
        CALL(evt_item_set_move_dir_speed, STRING("key"), LW(6), FLOAT(45.0f))
        CALL(evt_item_set_jump_power, STRING("key"), FLOAT(500.0f))
        CALL(evt_item_set_gravity, STRING("key"), FLOAT(1.0f))
        CALL(evt_item_set_bound_rate, STRING("key"), FLOAT(1.0f))
        CALL(evt_item_set_bound_limit, STRING("key"), 0)
        CALL(evt_item_status_on, STRING("key"), 4)
        CALL(evt_item_status_on, STRING("key"), 16)
        CALL(evt_npc_set_position, STRING("黒カロン"), 0, -1000, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kurokaron_init)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_npc_set_position, STRING("me"), 570, 0, 0)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
        CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_Z_2"))
        CALL(evt_npc_set_balloontype, STRING("me"), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kurokaron_talk)
    SPAWN_EVENT(senkaron_event)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_npc_change_interrupt, STRING("me"), 6, 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[69] = {
    (u32)"黒カロン", 0x01000000, 0x00000000, (u32)&kurokaron_init,
    0x00000000, (u32)&kurokaron_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"カロン",
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
    0x00000000,
};

static EVT_BEGIN(key_evt)
    SET_VALUE(GSW(0), 384)
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x0000002E, 0xFFFFFFFF,
};

static EVT_BEGIN(key_check_evt)
    CALL(evt_pouch_check_item, 46, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(unlock_evt)
    SET_VALUE(GSW(0), 385)
    SET_VALUE(LW(0), STRING("e_bero_1"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

// TODO: write make_name (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(shan_init)
    SET_VALUE(LW(0), 1)
    LOOP(3)
        SWITCH(LW(0))
            CASE_EQUAL(1)
            SET_VALUE(LW(2), 8)
            CASE_DEFAULT
            SET_VALUE(LW(2), 4)
        END_SWITCH
        SET_VALUE(LW(1), 1)
        LOOP(LW(2))
            CALL(make_name, LW(0), LW(1), LW(3))
            CALL(evt_mapobj_flag_onoff, 1, 1, LW(3), 1)
            CALL(evt_mapobj_get_position, LW(3), LW(4), LW(5), LW(6))
            SUBTRACT_FLOAT(LW(5), FLOAT(7.0f))
            CALL(evt_eff, 0, STRING("fire"), 3, LW(4), LW(5), LW(6), FLOAT(0.8f), 1000, 0, 0, 0, 0, 0, 0)
            ADD(LW(1), 1)
        END_LOOP
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(las_05_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_npc_setup, PTR(npcEnt))
    END_IF
    IF_INT_EQUAL(GSW(0), 383)
        CALL(evt_item_entry, STRING("key"), 46, 0, 0, 0, 0, -1, PTR(key_evt))
    END_IF
    IF_INT_LESS(GSW(0), 385)
        CALL(evt_mobj_lock, STRING("lock"), 46, 590, 20, 0, -90, PTR(key_check_evt), PTR(unlock_evt), 0)
        SET_VALUE(LW(0), STRING("e_bero_1"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    CALL_CHILD(shan_init)
    RETURN
EVT_END()

// TODO: type
u8 kurokaron_move_index[1] = {
    0x00,
};

// TODO: type
u8 kurokaron_move_flag[1] = {
    0x00,
};

// TODO: type
u8 kurokaron_same_goal[1] = {
    0x00,
};

// TODO: type
u32 kurokaron_vivian_prev_sts[1] = {
    0x00000000,
};

// TODO: type
u8 kurokaron_stop_cnt[1] = {
    0x00,
};

// TODO: type
u8 kurokaron_run_cnt[2] = {
    0x00, 0x00,
};

// TODO: flag$596 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 flag$596[1] = {
//     0x00000000,
// };

// TODO: count$597 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 count$597[1] = {
//     0x00000000,
// };

// TODO: name$1054 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 name$1054[5] = {
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000,
// };

// TODO: type
u32 skwp[1] = {
    0x00000000,
};

// TODO: write senkaron_ext_shadow_disp (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write senkaron_ext_disp (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write party_hit_check (global, 0x208 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write calc_area (global, 0xb8 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write move_check (global, 0x144 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write randf (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write _kurokaron_next_move_index (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write kurokaron_move2 (global, 0x2a8 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it

// TODO: write _kurokaron_run_away (global, 0x238 bytes) here:
// its asm is in build/G8MJ01/las/asm/las_05.s; this is where the original has it, just before the first script that uses it
// clang-format on

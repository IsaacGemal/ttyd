#include "las_09.h"
#include "driver/effdrv.h"
#include "eff/n64/eff_guruguru_n64.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "driver/hitdrv.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(eff_guruguru);

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door3_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door3_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door3_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door3_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[8] = {
    {"w_bero", 1, 6, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "las_08", "e_bero_1", 5, 5, &w_door_close, &w_door_open},
    {"e_bero", 1, 6, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "las_19", "w_bero", 5, 5, &e_door_close, &e_door_open},
    {"n_bero", 1, 6, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "las_10", "s_bero", 5, 5, &n_door_close, &n_door_open},
    {"dokan_1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_5", 1, 1, 0, 0},
    {"dokan_2", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_4", 1, 1, 0, 0},
    {"dokan_5", 4098, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_1", 1, 1, 0, 0},
    {"dokan_4", 4098, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_2", 1, 1, 0, 0},
    {NULL},
};

static HitDamageReturn dmgRetPoint[58] = {
    {"mod_00", {0.0f, -1000.0f, 0.0f}},
    {"mod_01", {0.0f, -1000.0f, 0.0f}},
    {"mod_02", {0.0f, -1000.0f, 0.0f}},
    {"mod_03", {0.0f, -1000.0f, 0.0f}},
    {"mod_05", {0.0f, -1000.0f, 0.0f}},
    {"mod_06", {0.0f, -1000.0f, 0.0f}},
    {"mod_07", {0.0f, -1000.0f, 0.0f}},
    {"mod_08", {0.0f, -1000.0f, 0.0f}},
    {"mod_09", {0.0f, -1000.0f, 0.0f}},
    {"mod_10", {0.0f, -1000.0f, 0.0f}},
    {"mod_11", {0.0f, -1000.0f, 0.0f}},
    {"mod_12", {0.0f, -1000.0f, 0.0f}},
    {"mod_13", {0.0f, -1000.0f, 0.0f}},
    {"mod_14", {0.0f, -1000.0f, 0.0f}},
    {"mod_15", {0.0f, -1000.0f, 0.0f}},
    {"mod_16", {0.0f, -1000.0f, 0.0f}},
    {"mod_17", {0.0f, -1000.0f, 0.0f}},
    {"mod_18", {0.0f, -1000.0f, 0.0f}},
    {"mod_20", {0.0f, -1000.0f, 0.0f}},
    {"mod_21", {0.0f, -1000.0f, 0.0f}},
    {"mod_22", {0.0f, -1000.0f, 0.0f}},
    {"mod_23", {0.0f, -1000.0f, 0.0f}},
    {"mod_24", {0.0f, -1000.0f, 0.0f}},
    {"mod_25", {0.0f, -1000.0f, 0.0f}},
    {"mod_26", {0.0f, -1000.0f, 0.0f}},
    {"mod_27", {0.0f, -1000.0f, 0.0f}},
    {"mod_28", {0.0f, -1000.0f, 0.0f}},
    {"mod_29", {0.0f, -1000.0f, 0.0f}},
    {"mod_30", {0.0f, -1000.0f, 0.0f}},
    {"mod_31", {0.0f, -1000.0f, 0.0f}},
    {"mod_32", {0.0f, -1000.0f, 0.0f}},
    {"mod_33", {0.0f, -1000.0f, 0.0f}},
    {"mod_34", {0.0f, -1000.0f, 0.0f}},
    {"mod_35", {0.0f, -1000.0f, 0.0f}},
    {"mod_36", {0.0f, -1000.0f, 0.0f}},
    {"mod_37", {0.0f, -1000.0f, 0.0f}},
    {"mod_38", {0.0f, -1000.0f, 0.0f}},
    {"mod_39", {0.0f, -1000.0f, 0.0f}},
    {"mod_40", {0.0f, -1000.0f, 0.0f}},
    {"mod_41", {0.0f, -1000.0f, 0.0f}},
    {"mod_42", {0.0f, -1000.0f, 0.0f}},
    {"mod_43", {0.0f, -1000.0f, 0.0f}},
    {"mod_44", {0.0f, -1000.0f, 0.0f}},
    {"mod_45", {0.0f, -1000.0f, 0.0f}},
    {"mod_46", {0.0f, -1000.0f, 0.0f}},
    {"mod_47", {0.0f, -1000.0f, 0.0f}},
    {"mod_48", {0.0f, -1000.0f, 0.0f}},
    {"mod_49", {0.0f, -1000.0f, 0.0f}},
    {"mod_50", {0.0f, -1000.0f, 0.0f}},
    {"mod_51", {0.0f, -1000.0f, 0.0f}},
    {"mod_52", {0.0f, -1000.0f, 0.0f}},
    {"mod_53", {0.0f, -1000.0f, 0.0f}},
    {"mod_54", {0.0f, -1000.0f, 0.0f}},
    {"mod_55", {0.0f, -1000.0f, 0.0f}},
    {"mod_56", {0.0f, -1000.0f, 0.0f}},
    {"mod_57", {0.0f, -1000.0f, 0.0f}},
    {"mod_58", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

static EVT_BEGIN(first_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_set_party, 0, 1)
    END_IF
    CALL(evt_mario_set_pos, -1020, 0, 0)
    CALL(evt_party_set_pos, 0, -1070, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_cam3d_evt_set, 0, 466, 1414, 0, 362, 0, 0, 11)
    WAIT_FRAMES(1)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_cam3d_evt_set, 0, 87, 1414, 0, 110, 0, 3000, 0)
        WAIT_MS(3000)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    LOOP(0)
        IF_INT_NOT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 157, 87, 1126, -800, 110, 85, 2000, 11)
    WAIT_MS(2000)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(10))
        IF_INT_EQUAL(LW(10), 0)
            SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
            CALL_CHILD(w_door_open)
            BREAK_LOOP
        ELSE
            CALL_CHILD(w_door_open)
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -925, 0, FLOAT(150.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -875, 0, FLOAT(150.0f))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(10))
        IF_INT_EQUAL(LW(10), 0)
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            CALL_CHILD(w_door_close)
            BREAK_LOOP
        ELSE
            CALL_CHILD(w_door_close)
        END_IF
    END_LOOP
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -900, 80, 259, -900, 32, -12, 0, 11)
    CALL(evt_party_stop, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg8_las_11"))
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 387)
    CALL_CHILD(bero_case_entry)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(bomb_wanwan)
    SET_VALUE(LW(14), LW(0))
    SET_VALUE(LW(15), LW(1))
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, LW(14), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_entry, STRING("ofs"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_mapobj_set_offscreen, 1, LW(14), STRING("ofs"))
    CALL(evt_offscreen_get_boundingbox2, STRING("ofs"), LW(0), LW(1), LW(2), LW(3))
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(2), 0)
        IF_INT_EQUAL(LW(3), 0)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("ofs"), 1, LW(0), LW(1), LW(2), LW(3))
        WAIT_FRAMES(1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(14), 1)
        CALL(evt_img_clear_virtual_point, STRING("img"))
        CALL(evt_img_onoff, STRING("img"), 1)
        CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
        CALL(evt_img_wait_animend, STRING("img"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(14), 1)
    END_IF
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_mapobj_clear_offscreen, 1, LW(14))
    CALL(evt_paper_delete, STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_delete, STRING("ofs"))
    CALL(evt_hitobj_onoff, LW(15), 0, 0)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    RETURN
EVT_END()

EVT_BEGIN(bomb_wanwan1_evt)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(0), STRING("wan3"))
    SET_VALUE(LW(1), STRING("A_wan1"))
    CALL_CHILD(bomb_wanwan)
    SET_VALUE(GSWF(4326), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(bomb_wanwan2_evt)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(0), STRING("wan1"))
    SET_VALUE(LW(1), STRING("A_wan2"))
    CALL_CHILD(bomb_wanwan)
    SET_VALUE(GSWF(4327), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(switch_1_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 0, 400, 1368, 0, -40, 700, 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_QUAKE2"), LW(10))
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.005f), 1500)
    CALL(evt_snd_sfxoff, LW(10))
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_daizadai1_3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_PORT_APPEAR1"), LW(0), LW(1), LW(2), 0)
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_msec_init, 1, 255, 50, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_mapobj_color, 1, STRING("S_daizadai1_3"), 255, 255, 255, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_SIBLING_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 180, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_rotate, 1, STRING("S_daizadai1_3"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mapobj_get_position, STRING("S_daizadai1_2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_PORT_APPEAR1"), LW(0), LW(1), LW(2), 0)
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_msec_init, 1, 255, 50, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_mapobj_color, 1, STRING("S_daizadai1_2"), 255, 255, 255, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_SIBLING_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, -180, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_rotate, 1, STRING("S_daizadai1_2"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_mapobj_get_position, STRING("S_daizadai1_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_PORT_APPEAR2"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, 255, 50, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S_daizadai1_1"), 255, 255, 255, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_daizadai1_1"), LW(0), 0, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_hitobj_onoff, STRING("A_daizadai1"), 0, 0)
    IF_INT_EQUAL(GSWF(4329), 1)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change"), STRING("Ship_Change2"))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(switch_2_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 0, 450, 45, 0, -75, -558, 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_QUAKE2"), LW(10))
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.005f), 1500)
    CALL(evt_snd_sfxoff, LW(10))
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_daizadai2_3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_PORT_APPEAR1"), LW(0), LW(1), LW(2), 0)
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_msec_init, 1, 255, 50, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_mapobj_color, 1, STRING("S_daizadai2_3"), 255, 255, 255, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_SIBLING_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 180, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_rotate, 1, STRING("S_daizadai2_3"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mapobj_get_position, STRING("S_daizadai2_2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_PORT_APPEAR1"), LW(0), LW(1), LW(2), 0)
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_msec_init, 1, 255, 50, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_mapobj_color, 1, STRING("S_daizadai2_2"), 255, 255, 255, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_SIBLING_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, -180, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_rotate, 1, STRING("S_daizadai2_2"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_mapobj_get_position, STRING("S_daizadai2_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_PORT_APPEAR2"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, 255, 50, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S_daizadai2_1"), 255, 255, 255, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_daizadai2_1"), LW(0), 0, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    IF_INT_EQUAL(GSWF(4328), 1)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change1"), STRING("Ship_Change1"))
    CALL(evt_hitobj_attr_onoff, 0, 1, STRING("Ship_Change1"), 32768)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(goikkou_talk)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print, 0, STRING("stg8_las_27"), 0, STRING("マジョリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_28"), 0, STRING("マリリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_29"), 0, STRING("ランペル"))
    WAIT_MS(100)
    CALL(evt_msg_print_party, STRING("stg8_las_30"))
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

EVT_BEGIN(majyorin_init)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 268435456)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

static EVT_BEGIN(maririn_init)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 268435456)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(ranpel_init)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 268435456)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[4] = {
    {"マジョリン", 1536, 0, &majyorin_init},
    {"マリリン", 1536, 0, &maririn_init},
    {"ランペル", 1536, 0, &ranpel_init},
    {NULL},
};

EVT_BEGIN(majyorin_evt_npc_entry)
    CALL(evt_npc_entry, STRING("にせ"), STRING("c_kuri_h"))
    CALL(evt_npc_set_tribe, STRING("にせ"), STRING("クリハカセ"))
    CALL(evt_npc_flag_onoff, 1, STRING("にせ"), 1536)
    CALL(evt_npc_status_onoff, 1, STRING("にせ"), 2)
    CALL(evt_npc_set_position, STRING("にせ"), 0, -1000, 0)
    RETURN
EVT_END()

static USER_FUNC(eff_guruguru) {
    s32* args = event->args;
    s32 name;
    f32 x, y, z, scale;
    EffectEntry* effect;
    EffGuruguruData* data;

    name = evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    scale = evtGetFloat(event, args[4]);
    effect = effGuruguruN64Entry(0, 3, x, y, z, scale);
    data = effect->userdata;
    data->drawScale = 0.8f;
    effSetName(effect, (const char*)name);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(majyorin_evt_main)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_wait_landon)
    CALL(evt_npc_stop_for_event)
    CALL(evt_npc_change_fbat_mode, 7)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(300)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir, 0, 90, 200)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_WITCH1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg8_las_12"), 0, STRING("マジョリン"))
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_cam3d_evt_set, 765, 102, 340, 765, 43, 9, 500, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_majo_disp_on, STRING("マジョリン"), 750, 0, 20, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_majo_disp_on, STRING("マリリン"), 780, 0, -10, 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_position, STRING("にせ"), 950, 0, 30)
    CALL(evt_npc_set_ry, STRING("にせ"), 270)
    CALL(evt_npc_move_position, STRING("にせ"), 800, 50, 0, FLOAT(70.0f), 1)
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print_party, STRING("stg8_las_12_01"))
    CALL(evt_party_set_pos, 0, 650, 0, 90)
    CALL(evt_mario_set_pos, 670, 0, 65)
    CALL(evt_mario_adjust_dir)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_13"), 0, STRING("マジョリン"))
    CALL(evt_cam3d_evt_set, 660, 82, 338, 660, 30, 43, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg8_las_14"))
    CALL(evt_cam3d_evt_set, 765, 102, 340, 765, 43, 9, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_15"), 0, STRING("マジョリン"))
    CALL(evt_npc_get_position, STRING("にせ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_dir, STRING("にせ"), LW(3))
    SET_VALUE(LW(4), LW(2))
    ADD(LW(4), 15)
    CALL(evt_eff, 0, STRING("kemuri_test"), 7, LW(0), LW(1), LW(4), FLOAT(1.4f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_RANPEL_TRANSFORM1"), LW(0), LW(1), LW(4), 0)
    WAIT_MS(300)
    CALL(evt_npc_set_position, STRING("ランペル"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("ランペル"), LW(3))
    CALL(evt_npc_set_position, STRING("にせ"), 0, -1000, 0)
    WAIT_MS(700)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg8_las_15_01"), 0, STRING("ランペル"))
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg8_las_15_02"), 0, STRING("マジョリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_16"), 0, STRING("マリリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_17"), 0, STRING("ランペル"))
    CALL(evt_npc_set_ry, STRING("マジョリン"), 90)
    WAIT_MS(500)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_18"), 0, STRING("マジョリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_19"), 0, STRING("ランペル"))
    CALL(evt_cam3d_evt_set, 660, 82, 338, 660, 30, 43, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg8_las_20"))
    CALL(evt_npc_set_ry, STRING("マジョリン"), 270)
    CALL(evt_cam3d_evt_set, 779, 104, 340, 779, 45, 9, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg8_las_21"), 0, STRING("マジョリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_22"), 0, STRING("マリリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_23"), 0, STRING("ランペル"))
    CALL(evt_cam3d_evt_set, 700, 193, 760, 700, 57, -12, 1000, 1)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 30)
        SUBTRACT(LW(2), 30)
        CALL(evt_npc_move_position, STRING("マジョリン"), LW(0), LW(2), 0, FLOAT(90.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 60)
        SUBTRACT(LW(2), 60)
        CALL(evt_npc_move_position, STRING("マリリン"), LW(0), LW(2), 0, FLOAT(90.0f), 0)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 80)
    SUBTRACT(LW(2), 10)
    CALL(evt_npc_move_position, STRING("ランペル"), LW(0), LW(2), 0, FLOAT(90.0f), 0)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_party_run, 0)
    CALL(evt_npc_set_battle_info, STRING("マジョリン"), 8)
    CALL(evt_npc_battle_start, STRING("マジョリン"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    SUBTRACT(LW(2), 10)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("マジョリン"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("マリリン"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("ランペル"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("マジョリン"), 16777216)
    CALL(evt_npc_flag_onoff, 1, STRING("マリリン"), 16777216)
    CALL(evt_npc_flag_onoff, 1, STRING("ランペル"), 16777216)
    CALL(evt_npc_set_force_regl_anim, STRING("マジョリン"), STRING("MJR_D_4"))
    CALL(evt_npc_set_force_regl_anim, STRING("マリリン"), STRING("MRR_D_4"))
    CALL(evt_npc_set_force_regl_anim, STRING("ランペル"), STRING("D_2"))
    CALL(evt_npc_set_autotalkpose, STRING("マジョリン"), STRING("MJR_D_4"), STRING("MJR_T_3"))
    CALL(evt_npc_set_autotalkpose, STRING("マリリン"), STRING("MRR_D_4"), STRING("MRR_T_2"))
    CALL(evt_npc_set_autotalkpose, STRING("ランペル"), STRING("D_2"), STRING("T_4"))
    CALL(evt_npc_set_anim, STRING("マジョリン"), STRING("MJR_D_4"))
    CALL(evt_npc_set_anim, STRING("マリリン"), STRING("MRR_D_4"))
    CALL(evt_npc_set_anim, STRING("ランペル"), STRING("D_2"))
    CALL(evt_npc_set_height, STRING("マジョリン"), 22)
    CALL(evt_npc_set_height, STRING("マリリン"), 29)
    CALL(evt_npc_set_height, STRING("ランペル"), 32)
    CALL(evt_npc_get_position, STRING("マジョリン"), LW(0), LW(1), LW(2))
    ADD(LW(1), 32)
    CALL(eff_guruguru, STRING("eff0"), LW(0), LW(1), LW(2), 20)
    CALL(evt_npc_get_position, STRING("マリリン"), LW(0), LW(1), LW(2))
    ADD(LW(1), 39)
    CALL(eff_guruguru, STRING("eff1"), LW(0), LW(1), LW(2), 25)
    CALL(evt_npc_get_position, STRING("ランペル"), LW(0), LW(1), LW(2))
    ADD(LW(1), 42)
    CALL(eff_guruguru, STRING("eff2"), LW(0), LW(1), LW(2), 25)
    CALL(evt_npc_get_position, STRING("マジョリン"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_WITCH_PIYO1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_24"), 0, STRING("マジョリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_25"), 0, STRING("マリリン"))
    CALL(evt_msg_print, 0, STRING("stg8_las_26"), 0, STRING("ランペル"))
    WAIT_MS(800)
    CALL(evt_npc_change_interrupt, STRING("マジョリン"), 6, PTR(goikkou_talk))
    CALL(evt_npc_change_interrupt, STRING("マリリン"), 6, PTR(goikkou_talk))
    CALL(evt_npc_change_interrupt, STRING("ランペル"), 6, PTR(goikkou_talk))
    SET_VALUE(GSW(0), 391)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_start_for_event)
    RETURN
EVT_END()

EVT_BEGIN(majyorin_evt)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_area_check, 1, 620, 90, 100, LW(0), LW(2), LW(3))
        IF_INT_EQUAL(LW(3), 1)
            CALL(evt_mario_get_name_hitobj_ride, LW(4))
            IF_STR_NOT_EQUAL(LW(4), STRING("mod_38"))
                IF_INT_LESS_OR_EQUAL(LW(1), 0)
                    IF_INT_GREATER_OR_EQUAL(LW(1), -5)
                        BREAK_LOOP
                    END_IF
                END_IF
            END_IF
            IF_INT_GREATER_OR_EQUAL(LW(0), 640)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SPAWN_EVENT(majyorin_evt_main)
    RETURN
EVT_END()

EVT_BEGIN(las_09_init_evt)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS3"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    IF_INT_NOT_EQUAL(GSW(0), 386)
        CALL_CHILD(evt_bero_info_run)
    END_IF
    CALL(evt_hit_damage_return_set, PTR(dmgRetPoint))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_mobj_recovery_blk, STRING("r_blk"), 10, 190, 50, 25, 0, 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -190, 50, 25, 0, 0)
    IF_INT_EQUAL(GSW(0), 386)
        SPAWN_EVENT(first_evt)
    END_IF
    IF_INT_EQUAL(GSWF(4326), 0)
        CALL(evt_run_case_evt, 15, 1, STRING("A_wan1"), 0, PTR(bomb_wanwan1_evt), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("wan3"), 1)
        CALL(evt_hitobj_onoff, STRING("A_wan1"), 0, 0)
    END_IF
    IF_INT_EQUAL(GSWF(4327), 0)
        CALL(evt_run_case_evt, 15, 1, STRING("A_wan2"), 0, PTR(bomb_wanwan2_evt), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("wan1"), 1)
        CALL(evt_hitobj_onoff, STRING("A_wan2"), 0, 0)
    END_IF
    CALL(evt_mobj_switch_blue, 2, STRING("switch1"), -540, 10, -1300, PTR(switch_1_evt), GSWF(4328))
    CALL(evt_mobj_switch_blue, 2, STRING("switch2"), 540, 10, -1300, PTR(switch_2_evt), GSWF(4329))
    IF_INT_EQUAL(GSWF(4328), 1)
        CALL(evt_mapobj_rotate, 0, STRING("S_daizadai1_1"), 180, 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("S_daizadai1_2"), 0, 0, -180)
        CALL(evt_mapobj_rotate, 0, STRING("S_daizadai1_3"), 0, 0, 180)
        CALL(evt_hitobj_onoff, STRING("A_daizadai1"), 0, 0)
        CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change"), STRING("Ship_Change2"))
    END_IF
    IF_INT_EQUAL(GSWF(4329), 1)
        CALL(evt_mapobj_rotate, 0, STRING("S_daizadai2_1"), 180, 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("S_daizadai2_2"), 0, 0, -180)
        CALL(evt_mapobj_rotate, 0, STRING("S_daizadai2_3"), 0, 0, 180)
        CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change1"), STRING("Ship_Change1"))
    ELSE
        CALL(evt_hitobj_attr_onoff, 0, 0, STRING("Ship_Change1"), 32768)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_den"), 2)
    IF_INT_EQUAL(GSW(0), 390)
        CALL_CHILD(majyorin_evt_npc_entry)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("majyorin_evt"))
            CALL(evt_mario_set_pos, 620, 0, 82)
            CALL(evt_party_set_pos, 0, 600, 0, 86)
            CALL(evt_mario_set_dir, 90, 0, 0)
            CALL(evt_party_set_dir, 0, 90, 0)
            BEGIN_INLINE_EVENT
                CALL(evt_seq_wait, 2)
                CALL(evt_bero_exec_wait, 65536)
                SPAWN_EVENT(majyorin_evt_main)
            END_INLINE_EVENT
        ELSE
            SPAWN_EVENT(majyorin_evt)
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 388)
        SET_VALUE(GSWF(4380), 1)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

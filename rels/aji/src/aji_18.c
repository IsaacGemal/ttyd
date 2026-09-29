#include "aji_18.h"
#include "driver/fadedrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "mario/mariost.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_emergency[];
static USER_FUNC(sysflag);
static USER_FUNC(make_list);
s32 key_list[4];
extern GXColor zero_aji_00016d18;

static USER_FUNC(make_list) {
    s32 i = 0;

    if ((s32)pouchCheckItem(29) > 0) {
        key_list[i++] = 29;
    }
    if ((s32)pouchCheckItem(30) > 0) {
        key_list[i++] = 30;
    }
    if ((s32)pouchCheckItem(31) > 0) {
        key_list[i++] = 31;
    }
    key_list[i] = -1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(sysflag) {
    if (gp->flags & 1) {
        gp->flags &= ~1;
        fadeEntry(0x1c, 0, zero_aji_00016d18);
    }
    return EVT_RETURN_DONE;
}

// clang-format off
extend_data(84,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(85,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_01", "e_bero_4", 5, 5,
     (s32)&w_door_close, (s32)&w_door_open},
    {"e_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_13", "w_bero", 5, 5,
     (s32)&e_door_close, (s32)&e_door_open},
    {NULL},
};

EVT_BEGIN(evt_machine)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_67"), 0, 0)
    SET_VALUE(LW(10), 0)
    CALL(evt_pouch_check_item, 29, LW(11))
    ADD(LW(10), LW(11))
    CALL(evt_pouch_check_item, 30, LW(11))
    ADD(LW(10), LW(11))
    CALL(evt_pouch_check_item, 31, LW(11))
    ADD(LW(10), LW(11))
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(make_list)
    CALL(evt_win_item_select, 0, PTR(key_list), LW(10), 0)
    SWITCH(LW(10))
        CASE_EQUAL(29)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi1_aka"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi1_mid"), 1)
            SET_VALUE(GSWF(4191), 1)
            CALL(evt_pouch_remove_item, LW(10), LW(1))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DOOR_UNLOCK1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_exit_case_evt)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg7_aji_67_01"), 0, 0)
        END_IF
        CASE_EQUAL(30)
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi2_aka"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi2_mid"), 1)
            SET_VALUE(GSWF(4192), 1)
            CALL(evt_pouch_remove_item, LW(10), LW(1))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DOOR_UNLOCK1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_exit_case_evt)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg7_aji_67_01"), 0, 0)
        END_IF
        CASE_EQUAL(31)
        IF_INT_EQUAL(LW(0), 2)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi3_aka"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi3_mid"), 1)
            SET_VALUE(GSWF(4193), 1)
            CALL(evt_pouch_remove_item, LW(10), LW(1))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DOOR_UNLOCK1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_exit_case_evt)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg7_aji_67_01"), 0, 0)
        END_IF
        CASE_DEFAULT
    END_SWITCH
    IF_INT_EQUAL(GSWF(4191), 1)
        IF_INT_EQUAL(GSWF(4192), 1)
            IF_INT_EQUAL(GSWF(4193), 1)
                SET_VALUE(GSW(0), 369)
                CALL(evt_del_case_evt, 0, GW(10))
                SET_VALUE(LW(0), STRING("e_bero"))
                CALL_CHILD(bero_case_switch_on)
                CALL(evt_cam3d_evt_set, -140, 90, 369, 150, 60, -98, 500, 11)
                WAIT_MS(1200)
                CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_lights1_m"), 1)
                CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_lights1_r"), 1)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DOOR_LAMP1"), LW(0), LW(1), LW(2), 0)
                WAIT_MS(700)
                CALL(evt_cam3d_evt_off, 11, 0)
            END_IF
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(key_lock_koujyou)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_67_02"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_koopa_after)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 75, 0, FLOAT(80.0f))
    END_INLINE_EVENT
    CALL(evt_party_move_pos2, 0, 115, 0, FLOAT(80.0f))
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(500)
    CALL(evt_fade_end_wait)
    CALL(evt_party_stop, 0)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print_party, STRING("stg7_aji_43"))
    CALL(evt_cam3d_evt_set, 95, 55, 292, 95, 41, -8, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg7_aji_44"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_THINK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_Z"))
    WAIT_MS(1500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg7_aji_45"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

// typed non-const so sysflag's by-value copy takes its address; the original places it in .rodata
__declspec(section ".rodata") GXColor zero_aji_00016d18 = {0, 0, 0, 0};

EVT_BEGIN(aji_18_init_evt)
    CALL(sysflag)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 369)
        SET_VALUE(GSWF(4191), 1)
        SET_VALUE(GSWF(4192), 1)
        SET_VALUE(GSWF(4193), 1)
    END_IF
    IF_INT_EQUAL(GSWF(4191), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi1_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi1_mid"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi1_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi1_mid"), 1)
    END_IF
    IF_INT_EQUAL(GSWF(4192), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi2_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi2_mid"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi2_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi2_mid"), 1)
    END_IF
    IF_INT_EQUAL(GSWF(4193), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi3_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi3_mid"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi3_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi3_mid"), 1)
    END_IF
    SET_VALUE(LW(0), 0)
    CALL(evt_pouch_check_item, 29, LW(1))
    ADD(LW(0), LW(1))
    CALL(evt_pouch_check_item, 30, LW(1))
    ADD(LW(0), LW(1))
    CALL(evt_pouch_check_item, 31, LW(1))
    ADD(LW(0), LW(1))
    IF_INT_EQUAL(GSWF(4191), 0)
        SET_VALUE(LW(0), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi1"), 0, PTR(evt_machine), 0)
    END_IF
    IF_INT_EQUAL(GSWF(4192), 0)
        SET_VALUE(LW(0), 1)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi2"), 0, PTR(evt_machine), 0)
    END_IF
    IF_INT_EQUAL(GSWF(4193), 0)
        SET_VALUE(LW(0), 2)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi3"), 0, PTR(evt_machine), 0)
    END_IF
    IF_INT_LESS(GSW(0), 369)
        CALL(evt_run_case_evt, 9, 1, STRING("e_bero"), 0, PTR(key_lock_koujyou), GW(10))
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_lights1_m"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_lights1_r"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_lights1_m"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_lights1_r"), 1)
    END_IF
    SWITCH(GSW(0))
        CASE_BETWEEN(373, 375)
        CALL(evt_run_case_evt, 9, 1, STRING("e_bero"), 0, PTR(key_lock_koujyou), 0)
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_lights1_m"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_lights1_r"), 1)
    END_SWITCH
    SWITCH(GSW(0))
        CASE_BETWEEN(373, 375)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("e_bero"))
            CALL(evt_mario_get_party, LW(15))
            IF_INT_EQUAL(LW(15), 0)
                CALL(evt_mario_set_party, 0, 1)
                CALL(evt_party_run, 0)
                CALL(evt_pouch_mario_recovery)
                CALL(evt_pouch_all_party_recovery)
            END_IF
            SPAWN_EVENT(evt_koopa_after)
        END_IF
    END_SWITCH
    SPAWN_EVENT(evt_emergency)
    RETURN
EVT_END()

// clang-format on
#pragma force_active off

#include "driver/hitdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "mario/mario_pouch.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_dou_normal_puka[];

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

EVT_BEGIN(dou_03_custom)
    CALL(evt_hitobj_onoff, STRING("on_off"), 1, 0)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "dou_12", "e_bero_1", 1, 1},
    {"w_bero_2", 0, 0, 0x4E20, 100000, 0, 0, -1, dou_03_custom, -1, NULL, "dou_12", "e_bero_2", 1, 1},
    {"w_bero_3", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "dou_04", "e_bero", 1, 1},
    {"e_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, dou_03_custom, -1, NULL, "dou_05", "w_bero_1", 1, 1},
    {"e_bero_2", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "dou_05", "w_bero_2", 1, 1},
    {"e_bero_3", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "dou_05", "w_bero_3", 1, 1},
    {"s_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "dou_09", "n_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(watergate_handle_init)
    IF_INT_LESS(GSW(0), 248)
        CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_reba"), 1)
        SET_VALUE(LW(0), STRING("s_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("s_bero"), 128)
        CALL(evt_hitobj_onoff, STRING("A_reba"), 1, 0)
    ELSE
        CALL(evt_map_playanim, STRING("mon_up"), 0, 0)
    END_IF
    RETURN
EVT_END()

static s32 itemsel_table[] = {
    ITEM_GATE_HANDLE, -1,
};

EVT_BEGIN(watergate_lever)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_mapobj_rotate, 1, STRING("S_reba"), LW(0), 0, 0)
        ADD(LW(0), 2)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(watergate_geer_small)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_mapobj_rotate, 1, STRING("S_zenmai1"), LW(0), 0, 0)
        ADD(LW(0), 2)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(watergate_geer_large)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_mapobj_rotate, 1, STRING("S_zenmai2"), LW(0), 0, 0)
        ADD(LW(0), -4)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(nagare_taru)
    CALL(evt_sub_intpl_msec_init, 1, 0, 100, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, STRING("S_taru"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru"), 1)
    CALL(evt_hitobj_onoff, STRING("A_taru"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(nagare_taru_se)
    CALL(evt_mapobj_get_position, STRING("S_taru"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_BARREL_MOVE1"), LW(0), LW(1), LW(2), LW(9))
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, STRING("S_taru"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_BARREL_MOVE1"), LW(0), LW(1), LW(2), LW(9))
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, STRING("S_taru"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_BARREL_MOVE1"), LW(0), LW(1), LW(2), LW(9))
    RETURN
EVT_END()

EVT_BEGIN(nagare_bin)
    CALL(evt_sub_intpl_msec_init, 1, 0, 100, 1200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, STRING("S_bin"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bin"), 1)
    RETURN
EVT_END()

EVT_BEGIN(watergate_handle)
    IF_INT_LESS(GSW(0), 248)
        CALL(evt_pouch_check_item, 72, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_msg_toge, 1, 0, 0, 0)
            CALL(evt_msg_print, 0, STRING("stg5_dou_03_before"), 0, 0)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        CALL(evt_win_item_select, 0, PTR(itemsel_table), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            RETURN
        END_IF
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_reba"), 1)
        CALL(evt_hitobj_onoff, STRING("A_reba"), 1, 1)
        CALL(evt_pouch_remove_item, 72, LW(0))
        CALL(evt_snd_sfxon, STRING("SFX_STG5_FLOODGATE_SHAKE1"), LW(10))
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 500)
        WAIT_MS(500)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_cam3d_evt_set, 98, 151, 563, 98, 56, 21, 500, 11)
        WAIT_MS(1000)
        CALL(evt_mapobj_get_position, STRING("s03_mon"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_FLOODGATE_UP1"), LW(0), LW(1), LW(2), LW(9))
        CALL(evt_map_playanim, STRING("mae"), 1, 0)
        CALL(evt_map_playanim, STRING("mon"), 0, 0)
        CALL(evt_map_checkanim, STRING("mon"), LW(0), LW(1))
        SPAWN_EVENT_GET_ID(PTR(watergate_lever), LW(10))
        SPAWN_EVENT_GET_ID(PTR(watergate_geer_small), LW(11))
        SPAWN_EVENT_GET_ID(PTR(watergate_geer_large), LW(12))
        BEGIN_INLINE_EVENT
            CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), LW(1))
        END_INLINE_EVENT
        WAIT_MS(LW(1))
        BEGIN_INLINE_EVENT
            CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 250)
        END_INLINE_EVENT
        CALL(evt_map_pauseanim, 0, STRING("mae"))
        DELETE_EVENT(LW(10))
        DELETE_EVENT(LW(11))
        DELETE_EVENT(LW(12))
        CALL(evt_snd_sfxoff, LW(9))
        CALL(evt_mapobj_get_position, STRING("s03_mon"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_FLOODGATE_UP2"), LW(0), LW(1), LW(2), LW(9))
        DELETE_EVENT(GW(10))
        SPAWN_EVENT(nagare_taru)
        SPAWN_EVENT(nagare_taru_se)
        DELETE_EVENT(GW(11))
        SPAWN_EVENT(nagare_bin)
        WAIT_MS(2000)
        CALL(evt_cam3d_evt_off, 300, 11)
        WAIT_MS(500)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_VOICE1"), 0)
        CALL(evt_msg_print, 0, STRING("stg5_dou_02"), 0, 0)
        WAIT_MS(500)
        CALL(evt_party_stop, 0)
        CALL(evt_msg_print_party, STRING("stg5_dou_03"))
        CALL(evt_party_run, 0)
        SET_VALUE(LW(0), STRING("s_bero"))
        CALL_CHILD(bero_case_switch_on)
        CALL(evt_hitobj_attr_onoff, 1, 1, STRING("s_bero"), 128)
        SET_VALUE(GSW(0), 248)
        CALL(evt_mario_key_onoff, 1)
    ELSE
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg5_dou_03_after"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main0)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main1)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main2)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

static HitDamageReturn hitDmgRetPoints[] = {
    {"mod_01", {0.0f, -1000.0f, 0.0f}},
    {"mod_02", {0.0f, -1000.0f, 0.0f}},
    {"mod_03", {0.0f, -1000.0f, 0.0f}},
    {"mod_04", {0.0f, -1000.0f, 0.0f}},
    {"mod_05", {0.0f, -1000.0f, 0.0f}},
    {"mod_06", {230.0f, 50.0f, 50.0f}},
    {"mod_07", {0.0f, -1000.0f, 0.0f}},
    {"mod_09", {0.0f, -1000.0f, 0.0f}},
    {"mod_10", {0.0f, -1000.0f, 0.0f}},
    {"mod_11", {0.0f, -1000.0f, 0.0f}},
    {"mod_12", {0.0f, -1000.0f, 0.0f}},
    {"mod_13", {0.0f, -1000.0f, 0.0f}},
    {"mod_14", {0.0f, -1000.0f, 0.0f}},
    {"mod_15", {0.0f, -1000.0f, 0.0f}},
    {"mod_16", {0.0f, -1000.0f, 0.0f}},
    {"mod_17", {435.0f, 25.0f, 10.0f}},
    {"Ship_Change", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

EVT_BEGIN(dou_03_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU4"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("moku"), 1, 0)
    CALL(evt_map_playanim, STRING("tubo"), 1, 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 450, 275, -550, STRING("s03_iwadan2"), 125, GSWF(5645))
    CALL(evt_run_case_evt, 9, 1, STRING("A_makimaki"), 0, PTR(watergate_handle), 0)
    SPAWN_EVENT(watergate_handle_init)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 245)
        CALL(evt_mapobj_trans, 0, STRING("tetu"), 1, 100, 0)
    END_IF
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    IF_INT_LESS(GSW(0), 248)
        SET_VALUE(LW(0), STRING("S_taru"))
        SET_VALUE(LW(1), STRING("A_taru"))
        SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main0), LW(10))
        SET_VALUE(GW(10), LW(10))
        SET_VALUE(LW(0), STRING("S_bin"))
        SET_VALUE(LW(1), 0)
        SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main1), LW(10))
        SET_VALUE(GW(11), LW(10))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru"), 1)
        CALL(evt_hitobj_onoff, STRING("A_taru"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bin"), 1)
    END_IF
    SET_VALUE(LW(0), STRING("s03_fune"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main2), LW(10))
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change"), STRING("Ship_Change"))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

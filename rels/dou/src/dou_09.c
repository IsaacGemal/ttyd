#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "driver/hitdrv.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 dou_normal_puka[];
extern s32 dou_puka_event[];

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

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_13", "e_bero_1", 1, 1},
    {"e_bero_1", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_10", "w_bero_1", 1, 1},
    {"e_bero_2", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_10", "w_bero_2", 1, 1},
    {"n_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_03", "s_bero", 1, 1},
    {NULL},
};

static EVT_BEGIN(dou_puka_event_init0)
    SET_VALUE(GW(0), 0)
    SET_VALUE(GW(1), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_0)
    IF_INT_NOT_EQUAL(GW(0), 0)
        RETURN
    END_IF
    SET_VALUE(GW(0), 1)
    SET_RAW_VALUE(LW(6), GW(1))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(0), 2)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_rev_0)
    IF_INT_NOT_EQUAL(GW(0), 2)
        RETURN
    END_IF
    SET_VALUE(GW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main0)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(1), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_sub0)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(1))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(0), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_init1)
    SET_VALUE(GW(2), 0)
    SET_VALUE(GW(3), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_1)
    IF_INT_NOT_EQUAL(GW(2), 0)
        RETURN
    END_IF
    SET_VALUE(GW(2), 1)
    SET_RAW_VALUE(LW(6), GW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(2), 2)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_rev_1)
    IF_INT_NOT_EQUAL(GW(2), 2)
        RETURN
    END_IF
    SET_VALUE(GW(2), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main1)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(3), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_sub1)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(3))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(2), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(2), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_init2)
    SET_VALUE(GW(4), 0)
    SET_VALUE(GW(5), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_2)
    IF_INT_NOT_EQUAL(GW(4), 0)
        RETURN
    END_IF
    SET_VALUE(GW(4), 1)
    SET_RAW_VALUE(LW(6), GW(5))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(4), 2)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_rev_2)
    IF_INT_NOT_EQUAL(GW(4), 2)
        RETURN
    END_IF
    SET_VALUE(GW(4), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main2)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(5), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_sub2)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(5))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(4), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(4), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_init3)
    SET_VALUE(GW(6), 0)
    SET_VALUE(GW(7), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_3)
    IF_INT_NOT_EQUAL(GW(6), 0)
        RETURN
    END_IF
    SET_VALUE(GW(6), 1)
    SET_RAW_VALUE(LW(6), GW(7))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(6), 2)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_rev_3)
    IF_INT_NOT_EQUAL(GW(6), 2)
        RETURN
    END_IF
    SET_VALUE(GW(6), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main3)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(7), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_sub3)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(7))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(6), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(6), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

static HitDamageReturn hitDmgRetPoints[] = {
    {"mod_01", {0.0f, -1000.0f, 0.0f}},
    {"mod_02", {0.0f, -1000.0f, 0.0f}},
    {"mod_03", {0.0f, -1000.0f, 0.0f}},
    {"mod_04", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

static EVT_BEGIN(evt_ride_taru)
    CALL(evt_mapobj_get_position, LW(0), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_BARREL_ON1"), LW(10), LW(11), LW(12), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_ride_ship)
    CALL(evt_mapobj_get_position, LW(0), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_ON1"), LW(10), LW(11), LW(12), 0)
    RETURN
EVT_END()

static EVT_BEGIN(bombwall_bomb)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, STRING("S_ki"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_hitobj_attr_onoff, 1, 1, STRING("w_bero"), 4)
    SET_VALUE(GSWF(2981), 1)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_ki"), STRING("offscreen"))
    CALL(evt_offscreen_get_boundingbox2, STRING("offscreen"), LW(10), LW(11), LW(12), LW(13))
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(12), 0)
        IF_INT_EQUAL(LW(13), 0)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 1, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_img_clear_virtual_point, STRING("capture_img"))
        CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ki"), 1)
        CALL(evt_hitobj_onoff, STRING("A_ki1"), 1, 0)
        CALL(evt_img_wait_animend, STRING("capture_img"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ki"), 1)
        CALL(evt_hitobj_onoff, STRING("A_ki1"), 1, 0)
    END_IF
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_ki"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    RETURN
EVT_END()

static EVT_BEGIN(bombwall_init)
    IF_INT_EQUAL(GSWF(2981), 0)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("w_bero"), 4)
        CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
        CALL(evt_run_case_evt, 15, 1, STRING("A_ki1"), 0, PTR(bombwall_bomb), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ki"), 1)
        CALL(evt_hitobj_onoff, STRING("A_ki1"), 1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(dou_09_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU11"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    WAIT_FRAMES(1)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("moku"), 1, 0)
    IF_INT_LESS(GSW(0), 248)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("mon_b"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("mon_a"), 1)
    END_IF
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    SET_VALUE(LW(0), STRING("S_taru01"))
    SET_VALUE(LW(1), STRING("A_taru01"))
    SET_VALUE(LW(2), STRING("A_taru01"))
    SET_VALUE(LW(3), PTR(evt_ride_taru))
    CALL_CHILD(dou_puka_event_init0)
    SPAWN_EVENT(dou_puka_event_main0)
    SPAWN_EVENT(dou_puka_event_sub0)
    CALL(evt_run_case_evt, 0, 1, STRING("A_taru01"), 0, PTR(dou_puka_event_0), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("A_taru01"), 0, PTR(dou_puka_event_rev_0), 0)
    SET_VALUE(LW(0), STRING("S_taru02"))
    SET_VALUE(LW(1), STRING("A_taru02"))
    SET_VALUE(LW(2), STRING("A_taru02"))
    SET_VALUE(LW(3), PTR(evt_ride_taru))
    CALL_CHILD(dou_puka_event_init1)
    SPAWN_EVENT(dou_puka_event_main1)
    SPAWN_EVENT(dou_puka_event_sub1)
    CALL(evt_run_case_evt, 0, 1, STRING("A_taru02"), 0, PTR(dou_puka_event_1), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("A_taru02"), 0, PTR(dou_puka_event_rev_1), 0)
    SET_VALUE(LW(0), STRING("S_taru03"))
    SET_VALUE(LW(1), STRING("A_taru03"))
    SET_VALUE(LW(2), STRING("A_taru03"))
    SET_VALUE(LW(3), PTR(evt_ride_taru))
    CALL_CHILD(dou_puka_event_init2)
    SPAWN_EVENT(dou_puka_event_main2)
    SPAWN_EVENT(dou_puka_event_sub2)
    CALL(evt_run_case_evt, 0, 1, STRING("A_taru03"), 0, PTR(dou_puka_event_2), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("A_taru03"), 0, PTR(dou_puka_event_rev_2), 0)
    SET_VALUE(LW(0), STRING("s09_fune01"))
    SET_VALUE(LW(1), STRING("a09_fune01"))
    SET_VALUE(LW(2), STRING("a09_fune_ata1"))
    SET_VALUE(LW(3), PTR(evt_ride_ship))
    CALL_CHILD(dou_puka_event_init3)
    SPAWN_EVENT(dou_puka_event_main3)
    SPAWN_EVENT(dou_puka_event_sub3)
    CALL(evt_run_case_evt, 0, 1, STRING("a09_fune_ata1"), 0, PTR(dou_puka_event_3), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("a09_fune_ata1"), 0, PTR(dou_puka_event_rev_3), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk1"), 500, 140, -100, 0, GSWF(5557), 0)
    CALL(evt_mobj_brick, STRING("hblk1"), 500, 50, -100, 0, 13, 0, GSWF(2994))
    SPAWN_EVENT(bombwall_init)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

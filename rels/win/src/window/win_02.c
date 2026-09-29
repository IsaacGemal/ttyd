#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "win_01", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "win_03", "w_bero", 1, 1},
    {"dokan2", 2, 0, 6, 100000, 0, 0, -1, NULL, 10, NULL, NULL, "dokan1", 1, 1},
    {"dokan1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan2", 1, 1},
    {NULL},
};

EVT_BEGIN(evt_hikou_nobiru_init)
    CALL(evt_hit_bind_mapobj, STRING("a_dai"), STRING("hikou"))
    CALL(evt_hit_bind_mapobj, STRING("a_hi"), STRING("hikou"))
    CALL(evt_hit_bind_mapobj, STRING("A_dokan1"), STRING("S_dokan2"))
    CALL(evt_hit_bind_mapobj, STRING("A_hi_kabe"), STRING("S_nob_tree"))
    CALL(evt_hit_bind_mapobj, STRING("A_dokan_nobi"), STRING("S_dokan2"))
    SET_VALUE(LW(0), 175)
    CALL(evt_mapobj_trans, 1, STRING("S_dokan_ha"), 0, LW(0), 0)
    SET_FLOAT_VALUE(LW(2), LW(0))
    ADD_FLOAT(LW(2), 90)
    DIVIDE_FLOAT(LW(2), 90)
    CALL(evt_mapobj_scale, 1, STRING("S_dokan2"), 1, LW(2), 1)
    SET_VALUE(LW(0), 175)
    CALL(evt_mapobj_trans, 1, STRING("hikou"), 0, LW(0), 0)
    SET_FLOAT_VALUE(LW(2), LW(0))
    ADD_FLOAT(LW(2), 116)
    DIVIDE_FLOAT(LW(2), 116)
    CALL(evt_mapobj_scale, 1, STRING("S_nob_tree"), 1, LW(2), 1)
    CALL(evt_hit_bind_update, STRING("a_dai"))
    CALL(evt_hit_bind_update, STRING("a_hi"))
    CALL(evt_hit_bind_update, STRING("A_dokan1"))
    CALL(evt_hit_bind_update, STRING("A_dokan_nobi"))
    CALL(evt_hit_bind_update, STRING("A_hi_kabe"))
    RETURN
EVT_END()

EVT_BEGIN(evt_hikou_nobiru)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tree_ha04"), 1)
    CALL(evt_cam3d_evt_set, -307, 220, 533, -307, 124, -7, 500, 11)
    WAIT_MS(1000)
    CALL(evt_snd_sfxon, STRING("SFX_STG2_SWITCH_QUAKE1"), LW(10))
    CALL(evt_cam_shake, 4, FLOAT(0.015f), FLOAT(0.0f), 1000)
    CALL(evt_snd_sfxoff, LW(10))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -307, 393, 533, -307, 297, -7, 3000, 0)
    CALL(evt_hit_bind_mapobj, STRING("a_dai"), STRING("hikou"))
    CALL(evt_hit_bind_mapobj, STRING("a_hi"), STRING("hikou"))
    CALL(evt_hit_bind_mapobj, STRING("A_dokan1"), STRING("S_dokan2"))
    CALL(evt_hit_bind_mapobj, STRING("A_hi_kabe"), STRING("S_nob_tree"))
    CALL(evt_hit_bind_mapobj, STRING("A_dokan_nobi"), STRING("S_dokan2"))
    CALL(evt_hit_get_position, STRING("A_dokan1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_STEP_EXTEND1"), LW(0), LW(1), LW(2), LW(9))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 175, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("S_dokan_ha"), 0, LW(0), 0)
            SET_FLOAT_VALUE(LW(2), LW(0))
            ADD_FLOAT(LW(2), 90)
            DIVIDE_FLOAT(LW(2), 90)
            CALL(evt_mapobj_scale, 1, STRING("S_dokan2"), 1, LW(2), 1)
            CALL(evt_hit_bind_update, STRING("A_dokan1"))
            CALL(evt_hit_bind_update, STRING("A_dokan_nobi"))
            CALL(evt_hit_bind_update, STRING("A_hi_kabe"))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 0, 175, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, STRING("hikou"), 0, LW(0), 0)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), 116)
        DIVIDE_FLOAT(LW(2), 116)
        CALL(evt_mapobj_scale, 1, STRING("S_nob_tree"), 1, LW(2), 1)
        CALL(evt_hit_bind_update, STRING("a_dai"))
        CALL(evt_hit_bind_update, STRING("a_hi"))
        CALL(evt_hit_bind_update, STRING("A_dokan1"))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(9))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_tree_ha04"), 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static TreeAccessEntry tree_access_data[] = {
    {4, "A_tree_01", "S_tree_mki_01", "S_tree_ha01", NULL, NULL},
    {4, "A_tree_02", "S_tree_mki_02", "S_tree_ha02", NULL, NULL},
    {7},
};

EVT_BEGIN(win_02_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_WIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG2_WIN1"))
    SET_VALUE(LW(0), PTR(tree_access_data))
    CALL_CHILD(evt_sub_tree_access_entry)
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou"), STRING("a_hi"))
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -470, 300, 50, 0, GSWF(5541), 0)
    CALL(evt_mobj_brick, STRING("mobj_box01"), 67, 110, -130, 271, 12, 0, GSWF(2683))
    CALL(evt_mobj_badgeblk, STRING("mobj_badge_gat"), 600, 185, -75, 248, 0, GSWF(2677), 1)
    CALL(evt_item_entry, STRING("item_00"), 125, -472, 50, -134, 16, GSWF(5608), 0)
    IF_INT_EQUAL(GSWF(2678), 1)
        SPAWN_EVENT(evt_hikou_nobiru_init)
    END_IF
    CALL(evt_mobj_switch_blue, 0, STRING("mobj_hashi0"), -415, 50, -125, PTR(evt_hikou_nobiru), GSWF(2678))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hikou"), 2)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

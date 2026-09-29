#include "evt/evt_bero.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

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

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "nok_01", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_03", "w_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(evt_ki_6)
    CALL(evt_item_delete_check, STRING("item_ki_6"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_ki_6"), 121, 192, 92, -120, 14, GSWF(1786), 0)
    END_IF
    RETURN
EVT_END()

static TreeAccessEntry ki_data[] = {
    {0, "a_ki_1", "miki_1", "ha_temae_1", "ha_usiro_1", NULL},
    {0, "a_ki_2", "miki_2", "ha_temae_2", "ha_usiro_2", NULL},
    {0, "a_ki_3", "miki_3", "ha_temae_3", "ha_usiro_3", NULL},
    {0, "a_ki_4", "miki_4", "ha_temae_4", "ha_usiro_4", NULL},
    {0, "a_ki_5", "miki_5", "ha_temae_5", "ha_usiro_5", evt_ki_6},
    {0, "a_ki_6", "miki_6", "ha_temae_6", "ha_usiro_6", NULL},
    {0, "a_ki_7", "miki_7", "ha_temae_7", "ha_usiro_7", NULL},
    {7},
};

static EVT_BEGIN(evt_kusa_1)
    CALL(evt_item_delete_check, STRING("item_kusa_1"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_1"), 125, -645, 50, -70, 4, GSWF(5600), 0)
    END_IF
    RETURN
EVT_END()

static KusaAccessEntry kusa_data[] = {
    {0, "a_kusa_1", "kusa_1", evt_kusa_1},
    {0, "a_kusa_2", "kusa_2", NULL},
    {0, "a_kusa_3", "kusa_3", NULL},
    {0, "a_kusa_4", "kusa_4", NULL},
    {0, "a_kusa_5", "kusa_5", NULL},
    {0, "a_kusa_6", "kusa_6", NULL},
    {0, "a_kusa_7", "kusa_7", NULL},
    {0, "a_kusa_8", "kusa_8", NULL},
    {0, "a_kusa_9", "kusa_9", NULL},
    {0, "a_kusa_10", "kusa_10", NULL},
    {0, "a_kusa_11", "kusa_11", NULL},
    {0, "a_kusa_12", "kusa_12", NULL},
    {0, "a_kusa_13", "kusa_13", NULL},
    {0, "a_kusa_14", "kusa_14", NULL},
    {0, "a_ku_15", "kusa_15", NULL},
    {4},
};

EVT_BEGIN(hei_02_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_HEI1"))
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_HEI1"))
    CALL(evt_npc_change_fbat_mode, 1)
    SET_VALUE(LW(0), PTR(ki_data))
    CALL_CHILD(evt_sub_tree_access_entry)
    SET_VALUE(LW(0), PTR(kusa_data))
    CALL_CHILD(evt_sub_kusa_access_entry)
    CALL(evt_map_playanim, STRING("anm_mugi_1"), 1, 0)
    CALL(evt_mobj_badgeblk, STRING("m"), 425, 25, -50, 139, 0, GSWF(1787), 0)
    CALL(evt_cloud_init)
    CALL(evt_cloud_ent, 0, STRING("kusa_1"), STRING("a_kusa_1"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_2"), STRING("a_kusa_2"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_3"), STRING("a_kusa_3"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_4"), STRING("a_kusa_4"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_5"), STRING("a_kusa_5"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_6"), STRING("a_kusa_6"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_7"), STRING("a_kusa_7"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_8"), STRING("a_kusa_8"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_9"), STRING("a_kusa_9"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_10"), STRING("a_kusa_10"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_11"), STRING("a_kusa_11"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_12"), STRING("a_kusa_12"), 0)
    CALL(evt_cloud_ent, 0, STRING("kusa_13"), STRING("a_kusa_13"), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

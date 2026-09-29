#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_05", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_07", "w_bero", 1, 1},
    {NULL},
};

static TreeAccessEntry ki_data[] = {
    {0, "a_ki_1", "miki__1", "ha_temae_1", "ha_usiro_1", NULL},
    {0, "a_ki_2", "miki__2", "ha_temae_2", "ha_usiro_2", NULL},
    {0, "a_ki_3", "miki__3", "ha_temae_3", "ha_usiro_3", NULL},
    {0, "a_ki_4", "miki__4", "ha_temae_4", "ha_usiro_4", NULL},
    {7},
};

EVT_BEGIN(evt_kusa_8)
    CALL(evt_item_delete_check, STRING("item_kusa_8"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_8"), 121, 200, 75, 60, 4, GSWF(1794), 0)
    END_IF
    RETURN
EVT_END()

static KusaAccessEntry kusa_data[] = {
    {0, "a_kusa_2", "kusa_2", NULL},
    {0, "a_kusa_3", "kusa_3", NULL},
    {0, "a_kusa_4", "kusa_4", NULL},
    {0, "a_kusa_5", "kusa_5", NULL},
    {0, "a_kusa_6", "kusa_6", NULL},
    {0, "a_kusa_8", "kusa_8", evt_kusa_8},
    {0, "a_kusa_12", "kusa_12", NULL},
    {0, "a_kusa_13", "kusa_13", NULL},
    {0, "a_kusa_14", "kusa_14", NULL},
    {4},
};

EVT_BEGIN(hei_06_init_evt)
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
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 5, 375, 160, -75, 0, 0)
    CALL(evt_mobj_badgeblk, STRING("mobj_hatenablk"), -25, 110, -75, 132, 0, GSWF(1795), 0)
    CALL(evt_item_entry, STRING("item_00"), 164, 50, 50, 105, 16, GSWF(1806), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

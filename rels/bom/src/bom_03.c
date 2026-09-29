#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_bom_snowfall[];

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static TreeAccessEntry tree_tbl[] = {
    {3, "A_ki1", "S_ki_1", "S_ha_1", NULL, NULL},
    {3, "A_ki2", "S_ki_2", "S_ha_2", NULL, NULL},
    {3, "A_ki3", "S_ki_3", "S_ha_3", NULL, NULL},
    {7},
};

static EVT_BEGIN(evt_tree)
    SET_VALUE(LW(0), PTR(tree_tbl))
    CALL_CHILD(evt_sub_tree_access_entry)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "bom_00", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "bom_04", "w_bero4", 1, 1},
    {NULL},
};

EVT_BEGIN(bom_03_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_bom_snowfall)
    CALL_CHILD(evt_tree)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_BOM1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_BOM1"))
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -405, 60, 25, 0, GSWF(5566), 0)
    CALL(evt_item_entry, STRING("item00"), 125, 45, 5, 120, 16, GSWF(5662), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

#include "mri_puni.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 mri_countdown[];
extern s32 mri_puni_init[];
extern s32 puni_disp_info[];

// clang-format off
extend_data(120,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(121,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BeroEntry bero_black_shadow[] = {
    {"dokan1", 2, 0, 2, 100000, 0, 0, -1, NULL, 10, NULL, "mri_04", "", 1, 1, 0, 0},
};

static BeroEntry bero_entry_data[] = {
    {"dokan1", 2, 0, 2, 100000, 0, 0, -1, NULL, 10, NULL, "mri_03", "dokan", 1, 1, 0, 0},
    {"dokan2", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "mri_02", "dokan3", 1, 1, 0, 0},
    {"dokan3", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan4", 1, 1, 0, 0},
    {"dokan4", 2, 0, 6, 100000, 0, 0, -1, NULL, 10, NULL, NULL, "dokan3", 1, 1, 0, 0},
    {NULL, 0, 0, 0, 0, 0, 0, 0, NULL, 0, NULL, NULL, NULL, 0, 0, 0, 0},
};

static EVT_BEGIN(evt_ueki_08)
    CALL(evt_item_delete_check, STRING("item_ueki_08"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_ueki_08"), 142, 250, -320, 130, 4, GSWF(2875), 0)
    END_IF
    RETURN
EVT_END()

static KusaAccessEntry ueki_access_data[] = {
    {1, "A_ueki01", "S_ueki01", NULL},
    {1, "A_ueki02", "S_ueki02", NULL},
    {1, "A_ueki03", "S_ueki03", NULL},
    {1, "A_ueki04", "S_ueki04", NULL},
    {1, "A_ueki05", "S_ueki05", NULL},
    {1, "A_ueki08", "S_ueki08", evt_ueki_08},
    {4},
};

EVT_BEGIN(mri_20_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 20, -89, -319, -1000, -1000, 0, 0)
    CALL(evt_puni_set_hamari, 45, -319, 92, -87, -91, -317, -321)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    CALL_CHILD(mri_puni_init)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT(puni_disp_info)
    SPAWN_EVENT(mri_countdown)
    SWITCH(GSW(0))
        CASE_BETWEEN(109, 110)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI9"))
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI4"))
    END_SWITCH
    IF_INT_GREATER(GSW(0), 113)
        CALL(evt_npc_check_delete, STRING("軍団ザコ２"))
        CALL(evt_npc_check_delete, STRING("バリアーン"))
    END_IF
    SET_VALUE(LW(0), PTR(ueki_access_data))
    CALL_CHILD(evt_sub_kusa_access_entry)
    CALL(evt_map_playanim, STRING("mizu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu_02"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_02"), 1, 0)
    IF_INT_EQUAL(GSW(0), 90)
        SET_VALUE(LW(0), STRING("dokan1"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan1"), PTR(bero_black_shadow))
        SET_VALUE(LW(0), STRING("dokan1"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

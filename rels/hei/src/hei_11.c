#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 hei_test_water_tev[];

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_entry_data[45] = {
    (u32)"w_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"hei_10", (u32)"e_bero",
    0x00010001, 0x00000000, 0x00000000, (u32)"e_bero",
    0x00000000, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"hei_12", (u32)"w_bero", 0x00010001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

EVT_BEGIN(hei_11_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_TORIDE1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_HEI3"))
    CALL(evt_snd_set_rev_mode, 1)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_set_tevcallback, 1, PTR(hei_test_water_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("mizu"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("mizu"), 256)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

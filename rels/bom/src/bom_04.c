#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_bom_snowfall[];

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

// TODO: type
static u32 tree_tbl[24] = {
    0x00000003, (u32)"A_ki1", (u32)"S_ki_1", (u32)"S_ha_1",
    0x00000000, 0x00000000, 0x00000003, (u32)"A_ki2",
    (u32)"S_ki_2", (u32)"S_ha_2", 0x00000000, 0x00000000,
    0x00000003, (u32)"A_ki3", (u32)"S_ki_3", (u32)"S_ha_3",
    0x00000000, 0x00000000, 0x00000007, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(evt_tree)
    SET_VALUE(LW(0), PTR(tree_tbl))
    CALL_CHILD(evt_sub_tree_access_entry)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[45] = {
    (u32)"w_bero4", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"bom_03", (u32)"e_bero",
    0x00010001, 0x00000000, 0x00000000, (u32)"e_bero4",
    0x00000000, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"bom_01", (u32)"w_bero", 0x00010001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

EVT_BEGIN(bom_04_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_bom_snowfall)
    CALL_CHILD(evt_tree)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_BOM1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_BOM1"))
    CALL(evt_item_entry, STRING("item00"), 125, 450, 5, 110, 16, GSWF(5663), 0)
    CALL(evt_mobj_badgeblk, STRING("mobj_badge_gat"), 150, 0, -75, 279, 0, GSWF(3887), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bom04_hei2"), 2)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

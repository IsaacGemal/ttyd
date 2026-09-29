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

// TODO: type
static u32 bero_entry_data[45] = {
    (u32)"w_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"hei_03", (u32)"e_bero",
    0x00010001, 0x00000000, 0x00000000, (u32)"e_bero",
    0x00000000, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"hei_05", (u32)"w_bero", 0x00010001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(evt_ki_3)
    CALL(evt_item_delete_check, STRING("item_ki_3"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_ki_3"), 121, 17, 90, -122, 14, GSWF(1791), 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 ki_data[24] = {
    0x00000000, (u32)"a_ki_1", (u32)"miki_1", (u32)"ha_temae",
    (u32)"ha_usiro", 0x00000000, 0x00000000, (u32)"a_ki_2",
    (u32)"miki_2", (u32)"ha_temae_2", (u32)"ha_usiro_2", 0x00000000,
    0x00000000, (u32)"a_ki_3", (u32)"miki_3", (u32)"ha_temae_3",
    (u32)"ha_usiro_3", (u32)&evt_ki_3, 0x00000007, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(evt_kusa_11)
    CALL(evt_item_delete_check, STRING("item_kusa_11"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_11"), 121, 364, 75, -95, 4, GSWF(1790), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_kusa_12)
    CALL(evt_item_delete_check, STRING("item_kusa_12"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_12"), 139, 320, 75, -55, 4, GSWF(1801), 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 kusa_data[48] = {
    0x00000000, (u32)"a_kusa_1", (u32)"kusa_1", 0x00000000,
    0x00000000, (u32)"a_kusa_2", (u32)"kusa_2", 0x00000000,
    0x00000000, (u32)"a_kusa_3", (u32)"kusa_3", 0x00000000,
    0x00000000, (u32)"a_kusa_4", (u32)"kusa_4", 0x00000000,
    0x00000000, (u32)"a_kisa_5", (u32)"kusa_5", 0x00000000,
    0x00000000, (u32)"a_kusa_6", (u32)"kusa_6", 0x00000000,
    0x00000000, (u32)"a_kusa_7", (u32)"kusa_7", 0x00000000,
    0x00000000, (u32)"a_kusa_9", (u32)"kusa_9", 0x00000000,
    0x00000000, (u32)"a_kusa_10", (u32)"kusa_10", 0x00000000,
    0x00000000, (u32)"a_kusa_11", (u32)"kusa_11", (u32)&evt_kusa_11,
    0x00000000, (u32)"a_kusa_12", (u32)"kusa_12", (u32)&evt_kusa_12,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(hei_04_init_evt)
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
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group1"), 2)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

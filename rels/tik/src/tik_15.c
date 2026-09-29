#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "driver/hitdrv.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(88,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(89,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"dokan_1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "tik_04", "dokan", 1, 1},
    {"dokan_2", 2, 0, 6, 100000, 0, 0, -1, NULL, 10, NULL, "tik_01", "dokan_1", 1, 1},
    {"w_bero_1", 0, 0, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "tik_16", "e_bero_1", 1, 1},
    {"w_bero_2", 0, 0, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "tik_16", "e_bero_2", 1, 1},
    {"e_bero", 0, 0, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "tik_20", "w_bero", 1, 1},
    {"n_bero", 0, 0, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "tik_21", "s_bero", 1, 1},
    {NULL},
};

static HitDamageReturn hitDmgRetPoints[] = {
    {"mod_1", {0.0f, -1000.0f, 0.0f}},
    {"mod_2", {0.0f, -1000.0f, 0.0f}},
    {"mod_3", {0.0f, -1000.0f, 0.0f}},
    {"mod_4", {0.0f, -1000.0f, 0.0f}},
    {"mod_5", {0.0f, -1000.0f, 0.0f}},
    {"mod_6", {0.0f, -1000.0f, 0.0f}},
    {"mod_7", {0.0f, -1000.0f, 0.0f}},
    {"mod_8", {0.0f, -1000.0f, 0.0f}},
    {"mod_9", {0.0f, -1000.0f, 0.0f}},
    {"mod_10", {0.0f, -1000.0f, 0.0f}},
    {"mod_11", {0.0f, -1000.0f, 0.0f}},
    {"mod_12", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

EVT_BEGIN(tik_15_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK7"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_map_playanim, STRING("anm_mizu"), 1, 0)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 56)
        CALL(evt_npc_check_delete, STRING("クリボー"))
        CALL(evt_npc_check_delete, STRING("パタクリボー"))
        CALL(evt_npc_check_delete, STRING("トゲクリボー"))
    END_IF
    IF_INT_LESS(GSW(0), 261)
        CALL(evt_npc_check_delete, STRING("ハンマーブロス"))
        CALL(evt_npc_check_delete, STRING("カメック"))
    END_IF
    CALL(evt_mobj_lv_blk, STRING("block00"), -370, 0, -25, 0, GSWF(1343), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_lv_blk, STRING("block01"), -420, 0, -25, 0, GSWF(1344), STRING("MOBJ_Lv1BigBlock"))
    CALL(evt_mobj_badgeblk, STRING("mobj_01"), 50, 0, -38, 148, 0, GSWF(1362), 0)
    CALL(evt_mobj_badgeblk, STRING("mobj_02"), -140, 0, -38, 132, 0, GSWF(1363), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

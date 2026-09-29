#include "evt/evt_bero.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(100,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(101,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_entry_data[75] = {
    (u32)"dokan_1", 0x00020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000006, 0x00000000, (u32)"tik_06", (u32)"dokan_2",
    0x00010001, 0x00000000, 0x00000000, (u32)"dokan_2",
    0x00020000, 0x00000002, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0x0000000A,
    0x00000000, (u32)"tik_07", (u32)"dokan_2", 0x00010001,
    0x00000000, 0x00000000, (u32)"e_bero_1", 0x00000000,
    0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)"tik_17", (u32)"w_bero_1", 0x00010001, 0x00000000,
    0x00000000, (u32)"e_bero_2", 0x00000000, 0x00004E20,
    0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)"tik_17",
    (u32)"w_bero_2", 0x00010001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(tik_18_cloud_evt)
    SET_VALUE(GSWF(1351), 1)
    RETURN
EVT_END()

// TODO: type
static u32 hitDmgRetPoints[52] = {
    (u32)"mod_1", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_2", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_3", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_4", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_5", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_6", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_7", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_8", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_9", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_10", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_11", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_12", 0x00000000, 0xC47A0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(tik_18_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK7"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_map_playanim, STRING("anm_mizu"), 1, 0)
    IF_INT_EQUAL(GSWF(1351), 0)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 0, STRING("s_win_1"), STRING("a_win_1"), PTR(tik_18_cloud_evt))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_win_1"), 1)
        CALL(evt_hitobj_onoff, STRING("a_win_1"), 1, 0)
    END_IF
    IF_INT_LESS(GSW(0), 261)
        CALL(evt_npc_check_delete, STRING("カメック"))
        CALL(evt_npc_check_delete, STRING("トゲノコ"))
    ELSE
        CALL(evt_npc_check_delete, STRING("ハンニャー１"))
        CALL(evt_npc_check_delete, STRING("ハンニャー２"))
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change_01"), STRING("ue"))
    CALL(evt_mobj_badgeblk, STRING("mobj_01"), 110, 0, -38, 157, 0, GSWF(1365), 0)
    CALL(evt_mobj_badgeblk, STRING("mobj_02"), 310, 0, -38, 158, 0, GSWF(1366), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

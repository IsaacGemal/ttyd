#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "driver/hitdrv.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(84,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(85,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"w_bero_1", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_02", "e_bero_1", 1, 1},
    {"w_bero_2", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_02", "e_bero_2", 1, 1},
    {"e_bero_1", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_03", "w_bero_1", 1, 1},
    {"e_bero_2", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_03", "w_bero_2", 1, 1},
    {NULL},
};

static HitDamageReturn hitDmgRetPoints[] = {
    {"mod_01", {0.0f, -1000.0f, 0.0f}},
    {"mod_02", {0.0f, -1000.0f, 0.0f}},
    {"mod_03", {0.0f, -1000.0f, 0.0f}},
    {"mod_04", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

EVT_BEGIN(dou_12_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU3"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("moku"), 1, 0)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_npc_check, STRING("キラー大砲１"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_link, STRING("キラー大砲１"), STRING("キラー大砲２"))
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

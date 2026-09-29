#include "driver/hitdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_movefloor.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(108,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(109,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_15", "e_bero", 1, 1},
    {NULL},
};

static HitDamageReturn hitDmgRetPoints[] = {
    {"mod_1", {0.0f, -1000.0f, 0.0f}},
    {"mod_2", {0.0f, -1000.0f, 0.0f}},
    {"mod_3", {0.0f, -1000.0f, 0.0f}},
    {"mod_4", {0.0f, -1000.0f, 0.0f}},
    {"mod_5", {0.0f, -1000.0f, 0.0f}},
    {"ue", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

EVT_BEGIN(tik_20_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK7"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_map_playanim, STRING("anm_mizu"), 1, 0)
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change_01"), STRING("ue"))
    SET_VALUE(LW(0), STRING("dai_3"))
    SET_VALUE(LW(1), STRING("a_dai_3"))
    SET_VALUE(LW(2), -10)
    SET_VALUE(LW(3), 30)
    SET_VALUE(LW(4), 900)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT(moving_floor)
    CALL(evt_item_entry, STRING("item_01"), 274, 180, 100, -125, 16, GSWF(1367), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk01"), -50, 60, -75, 0, GSWF(5535), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk02"), 0, 100, -25, 0, GSWF(5536), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk03"), 50, 60, -75, 0, GSWF(5537), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

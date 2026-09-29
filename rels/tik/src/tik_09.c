#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"dokan_1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "tik_10", "dokan_2", 1, 1, 0, 0},
    {"dokan_2", 2, 0, 8, 100000, 0, 0, -1, NULL, 6, NULL, "tik_06", "dokan_1", 1, 1, 0, 0},
    {NULL},
};

EVT_BEGIN(evt_tik09_dokan)
    CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("dokan_1_s"))
    CALL(evt_mapobj_trans, 0, STRING("dokan_1_s"), 0, 0, 0)
    CALL(evt_hit_bind_update, STRING("a_dokan_1"))
    WAIT_MS(1500)
    CALL(evt_sub_intpl_init, 13, 0, 30, 100)
    LOOP(100)
        CALL(evt_sub_intpl_get_value)
        CALL(evt_mapobj_trans, 0, STRING("dokan_1_s"), 0, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_dokan_1"))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_trans, 0, STRING("dokan_1_s"), 0, 30, 0)
    CALL(evt_hit_bind_update, STRING("a_dokan_1"))
    RETURN
EVT_END()

EVT_BEGIN(tik_09_init_evt)
    CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("dokan_1_s"))
    CALL(evt_mapobj_trans, 1, STRING("dokan_1_s"), 0, 30, 0)
    CALL(evt_hit_bind_update, STRING("a_dokan_1"))
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT(evt_tik09_dokan)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

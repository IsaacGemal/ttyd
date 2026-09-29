#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_00", "a_usiro_bero", 1, 1},
    {"e_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gon_01", "w_bero_1", 1, 1},
    {"e_bero_3", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gon_01", "w_bero_2", 1, 1},
    {NULL},
};

static BeroEntry e3_bero[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gon_00", "w_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(gon_00_init_evt)
    SET_VALUE(GSWF(255), 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    WAIT_FRAMES(1)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 160, 60, -90, 0, 0)
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 6, -150, 260, 0, 0, 0)
    CALL(evt_mobj_jumpstand_blue, 0, STRING("mobj_jump"), 230, 0, 0, -90, 0, 0)
    CALL(evt_map_playanim, STRING("hokori"), 1, 0)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 4096)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("w_bero"), PTR(e3_bero))
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

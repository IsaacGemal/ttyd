#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_00", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_01", "w_bero", 1, 1},
    {"a_dokan_1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "a_dokan_2", 1, 1},
    {"a_dokan_2", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "a_dokan_1", 1, 1},
    {NULL},
};

static TreeAccessEntry ki_data[] = {
    {0, "a_ki_1", "miki_", "ha_temae", "ha_usiro", NULL},
    {7},
};

EVT_BEGIN(evt_anm_kan)
    IF_INT_EQUAL(GF(0), 1)
        GOTO(99)
    END_IF
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(GSW(301), 10)
            SET_VALUE(GF(0), 1)
            WAIT_MS(200)
            CALL(evt_item_entry, STRING("item_tukushi"), 175, 565, 15, -65, 5, -1, 0)
        ELSE
            ADD(GSW(301), 1)
            IF_INT_EQUAL(GSW(301), 10)
                SET_VALUE(GF(0), 1)
                WAIT_MS(200)
                CALL(evt_item_entry, STRING("item_tukushi"), 175, 565, 15, -65, 5, -1, 0)
            END_IF
        END_IF
    END_INLINE_EVENT
    LABEL(99)
    CALL(evt_map_playanim, STRING("anm_kan_1"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(hei_13_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GSW(301), 0)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_mobj_badgeblk, STRING("m"), -320, 60, -100, 299, 0, GSWF(1797), 1)
    SET_VALUE(LSWF(3), 0)
    CALL(evt_mobj_brick, STRING("mm"), 325, 175, -100, 0, 0, 0, LSWF(3))
    IF_INT_EQUAL(LSWF(3), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("mm"), 2)
    END_IF
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_CLEAR(LW(0), 4096)
        CALL(evt_item_entry, STRING("i"), 292, 325, 200, -100, 16, GSWF(1798), 0)
    END_IF
    CALL(evt_mobj_brick, STRING("10m"), 325, 60, -100, 0, 3, 0, GSWF(1799))
    SET_VALUE(LW(0), PTR(ki_data))
    CALL_CHILD(evt_sub_tree_access_entry)
    CALL(evt_map_playanim, STRING("anm_mugi"), 1, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_HEI1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_HEI1"))
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_CLEAR(LW(0), 4096)
        CALL(evt_run_case_evt, 13, 1, STRING("a_kan_5"), 0, PTR(evt_anm_kan), 0)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

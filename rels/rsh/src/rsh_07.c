#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_rsh_window[];

// clang-format off
extend_data(44,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(45,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// animeArg3/animeArg4 hold animation names; BeroEntry types them s32, so they are cast here.
static BeroEntry bero_data_a[] = {
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_04_a", "w_bero", 1, 1, "w_door_close", "w_door_open"},
    {NULL},
};

static BeroEntry bero_data_b[] = {
    {"w_bero", 1, 0, 2, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_04_b", "w_bero", 1, 1, "w_door_close", "w_door_open"},
    {NULL},
};

static BeroEntry bero_data_c[] = {
    {"w_bero", 1, 0, 2, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_04_c", "w_bero", 1, 1, "w_door_close", "w_door_open"},
    {NULL},
};

static EVT_BEGIN(init_unten)
    CALL(evt_npc_set_ry, STRING("me"), 270)
    RETURN
EVT_END()

static EVT_BEGIN(talk_unten)
    IF_INT_EQUAL(GSWF(3581), 0)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_08"), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_08_1"), 0, 0)
    END_IF
    RETURN
EVT_END()

static NpcSetupInfo npc_data[] = {
    {"運転手", 0x40020610, 0, init_unten, NULL, talk_unten},
    {NULL},
};

static char* access_tbl[] = {
    "nittei", "stg6_rsh_c_00_1", "soujyuu", "stg6_rsh_260", NULL,
};

static EVT_BEGIN(evt_access)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, LW(1), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_access_entry)
    SET_INT_READ_ADDR(PTR(access_tbl))
    LOOP(0)
        READ_INT2(LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 9, 1, LW(0), 0, PTR(evt_access), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(rsh_07_init_evt)
    IF_INT_EQUAL(GSWF(3579), 0)
        IF_INT_EQUAL(GSWF(3580), 0)
            IF_INT_EQUAL(GSWF(3581), 0)
                CALL(evt_map_playanim, STRING("yure"), 1, 0)
                CALL(evt_map_playanim, STRING("hai_sora"), 1, 0)
                CALL(evt_map_playanim, STRING("hai_yama"), 1, 0)
                CALL(evt_map_playanim, STRING("hai_iwayama"), 1, 0)
                CALL(evt_map_playanim, STRING("hai_saboten"), 1, 0)
            END_IF
        END_IF
    END_IF
    CALL(evt_npc_setup, PTR(npc_data))
    CALL_CHILD(evt_access_entry)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 210, 60, -145, 0, 0)
    IF_INT_EQUAL(GSWF(3460), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
    ELSE
        IF_INT_EQUAL(GSWF(3461), 1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
            CALL(evt_snd_envoff, 512)
            CALL(evt_snd_bgmoff, 26624)
        ELSE
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), STRING("rsh_07_a"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH1"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
            END_IF
            IF_STR_EQUAL(LW(0), STRING("rsh_07_b"))
            END_IF
            IF_STR_EQUAL(LW(0), STRING("rsh_07_c"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_07_a_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_a))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_07_init_evt)
    RETURN
EVT_END()

EVT_BEGIN(rsh_07_b_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_b))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_07_init_evt)
    RETURN
EVT_END()

static char* window_tbl[] = {
    "mado_uturi_1", "mado_uturi_2", "mado_uturi_3", "mado_uturi_4", NULL,
};

EVT_BEGIN(rsh_07_c_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_c))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_07_init_evt)
    SET_VALUE(LW(0), PTR(window_tbl))
    CALL_CHILD(evt_rsh_window)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

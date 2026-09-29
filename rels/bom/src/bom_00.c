#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
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

EVT_BEGIN(evt_bom_snowfall)
    CALL(evt_eff64, 0, STRING("snowfall_n64"), 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

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

static EVT_BEGIN(first_evt_dokan)
    IF_INT_EQUAL(GSWF(3884), 1)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    SET_VALUE(GF(0), 1)
    WAIT_WHILE(GF(0))
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 0)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[45] = {
    (u32)"dokan", 0x00020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, (u32)&first_evt_dokan,
    0x00000006, 0x00000000, (u32)"tik_07", (u32)"dokan__1",
    0x00010001, 0x00000000, 0x00000000, (u32)"e_bero",
    0x00000000, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"bom_03", (u32)"w_bero", 0x00010001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(first_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam3d_evt_set, -176, 567, 533, -176, 471, -9, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, -176, 183, 533, -176, 87, -9, 6000, 11)
    WAIT_MS(6000)
    SET_VALUE(GF(0), 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_mov_pos2, -155, 31, FLOAT(60.0f))
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_party_move_pos2, 0, -188, 31, FLOAT(60.0f))
    CALL(evt_party_set_dir, 0, 90, 300)
    CALL(evt_cam3d_evt_set, -175, 90, 295, -175, 35, 0, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("stg7_bom_01"))
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 352)
    SET_VALUE(GSWF(3884), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_snd_bgm_scope, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(first_evt2)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_cam3d_evt_set, -176, 567, 533, -176, 471, -9, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, -176, 183, 533, -176, 87, -9, 6000, 11)
    WAIT_MS(6000)
    SET_VALUE(GF(0), 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_mov_pos2, -155, 31, FLOAT(60.0f))
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_party_move_pos2, 0, -188, 31, FLOAT(60.0f))
    CALL(evt_party_set_dir, 0, 90, 300)
    CALL(evt_cam3d_evt_set, -175, 90, 295, -175, 35, 0, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("stg7_bom_01_00"))
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSWF(3884), 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

EVT_BEGIN(bom_00_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_BOM1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_BOM1"))
    CALL_CHILD(evt_bom_snowfall)
    CALL_CHILD(evt_tree)
    CALL(evt_mobj_brick, STRING("mobj_badge_gat2"), 215, 65, -75, 255, 12, 0, GSWF(3891))
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -300, 5, 0, STRING("S_gra02_dai"), 125, GSWF(5658))
    IF_INT_EQUAL(GSWF(3884), 0)
        SWITCH(GSW(0))
            CASE_LESS(349)
            SPAWN_EVENT(first_evt2)
            CASE_DEFAULT
            SPAWN_EVENT(first_evt)
        END_SWITCH
    ELSE
        IF_INT_GREATER_OR_EQUAL(GSW(0), 349)
            IF_INT_LESS(GSW(0), 352)
                SET_VALUE(GSW(0), 352)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

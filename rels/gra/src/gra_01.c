#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_gra_vivian_pose_set[];
extern s32 evt_kagemario_init_11_text_804[];
extern s32 evt_usu_kagemario_party_kill_11_data_4F98[];

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
    {"w_bero1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gra_00", "e_bero", 1, 1},
    {"e_bero1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gra_02", "w_bero_1", 1, 1},
    {NULL},
};

EVT_BEGIN(gra_01_after_ranpel2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_run, 0)
    CALL(evt_party_set_pos, 0, -640, 0, 0)
    CALL(evt_mario_set_pos, -610, 0, 0)
    CALL(evt_gra_vivian_pose_set, 1, STRING("PTR_S_4"))
    CALL(evt_gra_vivian_pose_set, 2, STRING("PTR_S_4"))
    CALL(evt_gra_vivian_pose_set, 3, STRING("PTR_S_4"))
    CALL(evt_gra_vivian_pose_set, 7, STRING("PTR_T_5"))
    CALL(evt_party_set_homing_dist, 0, FLOAT(20.0f))
    CALL(evt_seq_wait, 2)
    CALL(evt_mario_mov_pos2, -540, 0, FLOAT(150.0f))
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_party_stop, 0)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg4_gra_16_01"))
    CALL(evt_gra_vivian_pose_set, 1, STRING("PTR_S_1"))
    CALL(evt_gra_vivian_pose_set, 2, STRING("PTR_W_1"))
    CALL(evt_gra_vivian_pose_set, 3, STRING("PTR_R_1"))
    CALL(evt_gra_vivian_pose_set, 7, STRING("PTR_T_1"))
    SET_VALUE(GSWF(2075), 1)
    CALL_CHILD(bero_case_entry)
    CALL(evt_party_set_homing_dist, 0, FLOAT(50.0f))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_npc_change_fbat_mode, 1)
    RETURN
EVT_END()

static TreeAccessEntry tree_tbl[] = {
    {2, "A_ki_1", "S_gra01_ki01", NULL, NULL, NULL},
    {2, "A_ki_2", "S_gra01_ki02", NULL, NULL, NULL},
    {2, "A_ki_3", "S_gra01_ki03", NULL, NULL, NULL},
    {7},
};

EVT_BEGIN(gra_01_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_GRA1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_USU1"))
    CALL(evt_kagemario_init_11_text_804)
    SET_VALUE(LW(0), PTR(tree_tbl))
    CALL_CHILD(evt_sub_tree_access_entry)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL(evt_mobj_badgeblk, STRING("blk00"), 0, 0, -100, 121, 0, GSWF(2082), 0)
    CALL(evt_mobj_brick, STRING("blk02"), -350, 0, -100, 0, 0, 0, LSWF(2))
    CALL(evt_mobj_brick, STRING("blk03"), -200, 60, 100, 0, 0, 0, LSWF(3))
    CALL(evt_mobj_brick, STRING("blk04"), 200, 60, 100, 0, 0, 0, LSWF(4))
    CALL(evt_mobj_brick, STRING("blk05"), 0, 85, -100, 149, 11, 0, GSWF(2087))
    CALL(evt_mobj_brick, STRING("blk06"), 475, 60, -100, 0, 3, 0, GSWF(2088))
    SET_VALUE(LF(0), 0)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 206)
        IF_INT_EQUAL(GSWF(2075), 0)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL_CHILD(evt_bero_info_run)
    ELSE
        SPAWN_EVENT(gra_01_after_ranpel2)
    END_IF
    CALL_CHILD(evt_usu_kagemario_party_kill_11_data_4F98)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

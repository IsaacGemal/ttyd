#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 hikari_evt[];

// clang-format off
extend_data(80,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(81,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_09", "e_bero", 5, 5, (s32)w_door_close, (s32)w_door_open},
    {"e_bero_1", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_21", "w_bero", 5, 5, (s32)e1_door_close, (s32)e1_door_open},
    {"e_bero_2", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_21", "w_bero", 5, 5, (s32)e2_door_close, (s32)e2_door_open},
    {"n_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_20", "s_bero_1", 1, 1},
    {NULL},
};

char* mapgrp_name_1[18] = {
    "S_kai_02", "S_kai_03", "S_kai_04", "S_kai_05",
    "S_kai_06", "S_kai_07", "S_kai_08", "S_kai_09",
    "S_kai_10", "S_kai_11", "S_kai_12", "S_kai_13",
    "S_kai_14", "S_kai_15", "S_kai_16", "S_kai_17",
    "S_kai_18", "S_kai_25",
};

char* mapgrp_name_2[5] = {
    "S_kai_20", "S_kai_21", "S_kai_22", "S_kai_23",
    "S_kai_24",
};

static EVT_BEGIN(cam_shake)
    CALL(evt_cam_shake, 4, FLOAT(0.03f), FLOAT(0.03f), 200)
    RETURN
EVT_END()

EVT_BEGIN(change_map_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 575, 143, 531, 575, 67, -13, 0, 11)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yuka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_under"), 1)
    CALL(evt_hitobj_onoff, STRING("A_under"), 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kai_26"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_02"), 0, 30, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_03"), 0, 60, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_04"), 0, 90, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_05"), 0, 120, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_06"), 0, 150, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_07"), 0, 180, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_08"), 0, 210, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_09"), 0, 240, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_10"), 0, 270, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_11"), 0, 300, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_12"), 0, 330, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_13"), 0, 360, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_14"), 0, 390, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_15"), 0, 420, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_16"), 0, 450, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_17"), 0, 480, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_18"), 0, 510, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_20"), 0, 30, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_21"), 0, 60, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_22"), 0, 90, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_23"), 0, 120, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_24"), 0, 150, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_19"), 0, 150, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_kai_25"), 0, 540, 0)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_cam3d_evt_set, -575, 143, 531, -575, 67, -13, 4000, 0)
        WAIT_MS(4500)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    LOOP(0)
        IF_INT_NOT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka"), 1)
    CALL(evt_cam3d_evt_set, -5, -547, 531, -5, -623, -13, 9200, 0)
    WAIT_MS(300)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        SET_FLOAT_VALUE(LW(15), FLOAT(1.25f))
        LOOP(0)
            SET_FLOAT_VALUE(LW(14), FLOAT(30.0f))
            LOOP(24)
                SET_INT_READ_ADDR(PTR(mapgrp_name_1))
                SET_FLOAT_VALUE(LW(1), LW(14))
                SET_VALUE(LW(2), 0)
                LOOP(18)
                    READ_INT(LW(4))
                    IF_INT_GREATER_OR_EQUAL(LW(2), LW(0))
                        CALL(evt_mapobj_trans, 1, LW(4), 0, LW(1), 0)
                        ADD_FLOAT(LW(1), FLOAT(30.0f))
                    END_IF
                    ADD(LW(2), 1)
                END_LOOP
                SUBTRACT_FLOAT(LW(14), LW(15))
                WAIT_FRAMES(1)
            END_LOOP
            IF_INT_EQUAL(GF(0), 0)
                CALL(evt_snd_sfxon, STRING("SFX_STG8_MAP_MOVE1"), 0)
                SPAWN_EVENT(cam_shake)
            END_IF
            ADD(LW(0), 1)
            IF_INT_EQUAL(LW(0), 18)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(7600)
    SET_VALUE(GF(0), 1)
    CALL(evt_bero_mapchange, STRING("las_10"), STRING("ret_chgmap_evt"))
    RETURN
EVT_END()

static EVT_BEGIN(switch1_evt)
    IF_INT_EQUAL(GSWF(4353), 0)
        SET_VALUE(GSWF(4353), 1)
    ELSE
        SET_VALUE(GSWF(4353), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(switch2_evt)
    IF_INT_EQUAL(GSWF(4354), 0)
        SET_VALUE(GSWF(4354), 1)
    ELSE
        SET_VALUE(GSWF(4354), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(cloud_evt)
    SET_VALUE(GSW(0), 392)
    RETURN
EVT_END()

EVT_BEGIN(las_19_init_evt)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("chgmap_evt"))
        CALL(evt_snd_bgmoff, 512)
    ELSE
        IF_INT_LESS(GSW(0), 390)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
        ELSE
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST3"))
        END_IF
    END_IF
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    IF_INT_LESS(GSW(0), 392)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 0, STRING("S_futa"), STRING("A_futa"), PTR(cloud_evt))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_futa"), 1)
        CALL(evt_hitobj_onoff, STRING("A_futa"), 0, 0)
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("chgmap_evt"))
        CALL(evt_npc_check_delete, STRING("マホーン１"))
        SPAWN_EVENT(change_map_evt)
    ELSE
        CALL_CHILD(evt_bero_info_run)
    END_IF
    IF_INT_LESS(GSW(0), 390)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_under"), 1)
        CALL(evt_hitobj_onoff, STRING("A_under"), 1, 0)
        CALL(evt_npc_check_delete, STRING("マホーン２"))
        CALL(evt_npc_check_delete, STRING("マホーン３"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka"), 1)
        CALL(evt_hitobj_onoff, STRING("A_be"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kai_20"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kai_21"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kai_22"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kai_23"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kai_24"), 1)
        CALL(evt_npc_check_delete, STRING("マホーン１"))
        SPAWN_EVENT(hikari_evt)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 390)
        CALL(evt_mobj_switch_float_blk, STRING("switch1"), 375, -320, -45, STRING(""), 2, PTR(switch1_evt), 0)
        CALL(evt_mobj_switch_float_blk, STRING("switch2"), 450, -320, -45, STRING(""), 0, PTR(switch2_evt), 0)
        IF_INT_EQUAL(GSWF(4353), 0)
            CALL(evt_mobj_set_anim, STRING("switch1"), STRING("A_3"))
        ELSE
            CALL(evt_mobj_set_anim, STRING("switch1"), STRING("S_1"))
        END_IF
        IF_INT_EQUAL(GSWF(4354), 0)
            CALL(evt_mobj_set_anim, STRING("switch2"), STRING("A_3"))
        ELSE
            CALL(evt_mobj_set_anim, STRING("switch2"), STRING("S_1"))
        END_IF
    END_IF
    CALL(evt_mobj_badgeblk, STRING("mobj_item00"), 30, -440, -40, 129, 0, GSWF(4387), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

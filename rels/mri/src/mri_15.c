#include "mri_puni.h"
#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_mri_door_close[];
extern s32 evt_mri_door_open[];
extern s32 mri_countdown[];
extern s32 mri_puni_init[];
extern s32 mri_waterfall_effect[];
extern s32 puni_disp_info[];

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(bero_s_doa_close)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), STRING("S_doa"))
    SPAWN_EVENT(evt_mri_door_close)
    RETURN
EVT_END()

static EVT_BEGIN(bero_s_doa_open)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), STRING("S_doa"))
    SPAWN_EVENT(evt_mri_door_open)
    RETURN
EVT_END()

EVT_BEGIN(evt_dokan)
    IF_INT_EQUAL(GSWF(2866), 0)
        SET_VALUE(GSWF(2866), 1)
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 1, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "mri_06", "e_bero", 4, 4, bero_s_doa_close, bero_s_doa_open},
    {"dokan", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, evt_dokan, "mri_14", "dokan", 1, 1},
    {NULL},
};

static EVT_BEGIN(door_switch_init)
    IF_INT_EQUAL(GSWF(2865), 0)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
    ELSE
        CALL(evt_map_playanim, STRING("light"), 2, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(door_switch)
    CALL(evt_mapobj_get_position, STRING("S_lights"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOOR_UNLOCK"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOOR_LAMP"), LW(0), LW(1), LW(2), 0)
    IF_INT_EQUAL(GSWF(2865), 0)
        SET_VALUE(GSWF(2865), 1)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_on)
        CALL(evt_map_playanim, STRING("light"), 0, 0)
    ELSE
        SET_VALUE(GSWF(2865), 0)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_map_playanim, STRING("light"), 0, 0)
        CALL(evt_map_pauseanim, 0, STRING("light"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(dai3_escape)
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        SET_VALUE(LW(9), 0)
        SET_VALUE(LW(10), STRING("S_doa"))
        SPAWN_EVENT(evt_mri_door_close)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("第三勢力幹部"), -193, 29, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 0, -1000, 0)
    WAIT_WHILE(1)
    RETURN
EVT_END()

EVT_BEGIN(dai3_battle_init)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), -119, 0, 29)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ａ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1073741856)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), -129, 0, 66)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ｂ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1073741856)
    IF_INT_EQUAL(GSWF(2866), 0)
        CALL(evt_npc_set_position, STRING("第三勢力幹部"), -5, 0, 29)
        CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 270)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1536)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1073741856)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(dai3_battle)
    CALL(evt_seq_wait, 2)
    IF_INT_EQUAL(GSWF(2866), 0)
        SPAWN_EVENT_GET_ID(PTR(dai3_escape), LW(10))
    END_IF
    CALL(evt_bero_exec_wait, 65536)
    LOOP(0)
        CALL(evt_mario_get_name_hitobj_ride, LW(0))
        CALL(evt_bero_exec_get, LW(1))
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        IF_INT_EQUAL(LW(1), 65536)
            IF_STR_EQUAL(LW(0), STRING("A_yuka"))
                IF_INT_LESS_OR_EQUAL(LW(10), 20)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        ADD(LW(0), -10)
        ADD(LW(2), -20)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ａ"), LW(0), LW(2), 400, 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        ADD(LW(0), -20)
        ADD(LW(2), 20)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ｂ"), LW(0), LW(2), 400, 0, 0)
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_npc_set_battle_info, STRING("第三勢力部下Ａ"), 11)
    CALL(evt_npc_battle_start, STRING("第三勢力部下Ａ"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    IF_INT_EQUAL(GSWF(2866), 0)
        DELETE_EVENT(LW(10))
        CALL(evt_npc_set_position, STRING("第三勢力幹部"), 0, -1000, 0)
        SET_VALUE(GSWF(2866), 1)
    END_IF
    SET_VALUE(GSW(0), 110)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 0, -1000, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(ueki_evt_01)
    CALL(evt_item_delete_check, STRING("item_ueki01"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_ueki01"), 125, -37, 0, 96, 0, GSWF(5616), 0)
    END_IF
    RETURN
EVT_END()

static KusaAccessEntry ueki_access_data[] = {
    {1, "A_ueki01", "S_ueki01", ueki_evt_01},
    {1, "A_ueki02", "S_ueki02", NULL},
    {1, "A_ueki03", "S_ueki03", NULL},
    {1, "A_ueki04", "S_ueki04", NULL},
    {1, "A_ueki05", "S_ueki05", NULL},
    {1, "A_ueki06", "S_ueki06", NULL},
    {4},
};

EVT_BEGIN(mri_15_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 15, 0, -310, -1000, -1000, 0, 0)
    CALL(evt_puni_set_hamari, 134, 0, 63, 2, -2, -1000, -1000)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    CALL_CHILD(mri_puni_init)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT(puni_disp_info)
    SPAWN_EVENT(mri_countdown)
    SET_VALUE(LW(0), PTR(ueki_access_data))
    CALL_CHILD(evt_sub_kusa_access_entry)
    SWITCH(GSW(0))
        CASE_BETWEEN(109, 110)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI9"))
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI1"))
    END_SWITCH
    CALL(evt_map_playanim, STRING("mizu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu_02"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_02"), 1, 0)
    CALL(evt_map_playanim, STRING("kawa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("kawa_02"), 1, 0)
    CALL(evt_map_playanim, STRING("kawa_sibu_01"), 1, 0)
    SET_VALUE(LW(0), -100)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), -75)
    SET_FLOAT_VALUE(LW(3), FLOAT(1.0f))
    SPAWN_EVENT(mri_waterfall_effect)
    SET_VALUE(LW(0), 60)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), -40)
    SET_FLOAT_VALUE(LW(3), FLOAT(1.0f))
    SPAWN_EVENT(mri_waterfall_effect)
    IF_INT_EQUAL(GSW(0), 109)
        CALL_CHILD(dai3_battle_init)
        SPAWN_EVENT(dai3_battle)
    END_IF
    SPAWN_EVENT(door_switch_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_sui"), 0, PTR(door_switch), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_puni_all_fall_check[];
extern s32 evt_puni_mem_init[];
extern s32 evt_puni_set_hamari[];
extern s32 evt_puni_set_yuka[];
extern s32 evt_target_flag_onoff[];
extern s32 evt_target_set_position[];
extern s32 mri_countdown[];
extern s32 mri_puni_init[];
extern s32 mri_punipuni_itemremove[];
extern s32 mri_punipuni_itemselect[];
extern s32 mri_punipuni_itemset[];
extern s32 mri_punipuni_setup[];
extern s32 puni_disp_info[];

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_entry_data[90] = {
    (u32)"e_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"mri_11", (u32)"w_bero",
    0x00010001, 0x00000000, 0x00000000, (u32)"e_bero_2",
    0x00000000, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"mri_17", (u32)"w_bero", 0x00010001,
    0x00000000, 0x00000000, (u32)"dokan1", 0x00020000,
    0x00000009, 0x000186A0, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x00000006, 0x00000000,
    0x00000000, (u32)"dokan2", 0x00010001, 0x00000000,
    0x00000000, (u32)"dokan2", 0x00020000, 0x00000006,
    0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0x0000000A, 0x00000000, 0x00000000,
    (u32)"dokan1", 0x00010001, 0x00000000, 0x00000000,
    (u32)"dokan3", 0x00020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000006, 0x00000000, (u32)"mri_18", (u32)"dokan1",
    0x00010001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static EVT_BEGIN(punipuni_ball00_init)
    CALL_CHILD(mri_punipuni_setup)
    IF_INT_EQUAL(GSWF(2859), 1)
        CALL(evt_item_set_position, LW(10), 5, 30, -95)
        CALL(evt_target_flag_onoff, LW(11), 0, 3)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_hikari"), 1)
    END_IF
    CALL(evt_target_set_position, LW(11), 5, 0, -65)
    RETURN
EVT_END()

static EVT_BEGIN(punipuni_ball00)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(GSWF(2859), 0)
        CALL_CHILD(mri_punipuni_itemselect)
        IF_INT_EQUAL(LW(1), 0)
            GOTO(100)
        END_IF
        SET_VALUE(GSWF(2859), 1)
        CALL(evt_item_set_position, LW(10), 5, 30, -95)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_sui_hikari"), 1)
        CALL_CHILD(mri_punipuni_itemset)
    ELSE
        SET_VALUE(GSWF(2859), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_hikari"), 1)
        CALL_CHILD(mri_punipuni_itemremove)
    END_IF
    LABEL(100)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_f1_ueki_01)
    CALL(evt_item_delete_check, STRING("item_ueki_01"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_ueki_01"), 121, -190, -309, 65, 4, GSWF(2860), 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 ueki_access_data[32] = {
    0x00000001, (u32)"A_f1_ueki01", (u32)"S_f1_ueki01", (u32)&evt_f1_ueki_01,
    0x00000001, (u32)"A_f1_ueki02", (u32)"S_f1_ueki02", 0x00000000,
    0x00000001, (u32)"A_f2_ueki01", (u32)"S_f2_ueki01", 0x00000000,
    0x00000001, (u32)"A_f2_ueki02", (u32)"S_f2_ueki02", 0x00000000,
    0x00000001, (u32)"A_f2_ueki03", (u32)"S_f2_ueki03", 0x00000000,
    0x00000001, (u32)"A_f2_ueki04", (u32)"S_f2_ueki04", 0x00000000,
    0x00000001, (u32)"A_f2_ueki05", (u32)"S_f2_ueki05", 0x00000000,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(breakyuka_init)
    IF_INT_EQUAL(GSWF(2858), 0)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak_01"), 1, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(breakyuka_main)
    CALL(evt_hitobj_onoff, STRING("A_f2_rak_01"), 1, 1)
    CALL(evt_puni_all_fall_check)
    RETURN
EVT_END()

EVT_BEGIN(mri_12_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 12, 0, -250, -310, -1000, 3, 0)
    CALL(evt_puni_set_hamari, 0, 0, 0, 6, -6, -248, -312)
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
    CALL(evt_map_playanim, STRING("sui_hikari"), 1, 0)
    SPAWN_EVENT(breakyuka_init)
    CALL(evt_mobj_breaking_floor, STRING("mobj_bf00"), -45, 0, -64, 1, PTR(breakyuka_main), GSWF(2858))
    SET_VALUE(LW(10), STRING("punipuni"))
    SET_VALUE(LW(11), 0)
    SPAWN_EVENT(punipuni_ball00_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_dai_mae"), 0, PTR(punipuni_ball00), 0)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_item_entry, STRING("item_00"), 125, -175, -310, -85, 16, GSWF(5615), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

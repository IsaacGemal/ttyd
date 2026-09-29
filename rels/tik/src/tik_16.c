#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_movefloor.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(92,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(93,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_entry_data[105] = {
    (u32)"w_bero_2", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"tik_17", (u32)"e_bero_2",
    0x00010001, 0x00000000, 0x00000000, (u32)"w_bero_1",
    0x00000000, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"tik_17", (u32)"e_bero_1", 0x00010001,
    0x00000000, 0x00000000, (u32)"e_bero_2", 0x00000000,
    0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)"tik_15", (u32)"w_bero_2", 0x00010001, 0x00000000,
    0x00000000, (u32)"e_bero_1", 0x00000000, 0x00004E20,
    0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)"tik_15",
    (u32)"w_bero_1", 0x00010001, 0x00000000, 0x00000000,
    (u32)"dokan_1", 0x00020000, 0x00000009, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000006, 0x00000000, (u32)"mri_00", (u32)"dokan_1",
    0x00010001, 0x00000000, 0x00000000, (u32)"dokan_2",
    0x00020000, 0x00000009, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000006,
    0x00000000, (u32)"nok_01", (u32)"dokan_1", 0x00010001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
static u32 hitDmgRetPoints[36] = {
    (u32)"mod_1", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_2", 0xC37A0000, 0x00000000, 0x00000000,
    (u32)"mod_3", 0xC37A0000, 0x00000000, 0x00000000,
    (u32)"mod_4", 0x437A0000, 0x00000000, 0x00000000,
    (u32)"mod_5", 0x437A0000, 0x00000000, 0x00000000,
    (u32)"mod_6", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"a_dai_1", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"a_dai_2", 0x00000000, 0xC47A0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(evt_switch_00_init)
    CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("S_dakan_1"))
    IF_INT_EQUAL(GSWF(1345), 1)
        CALL(evt_mapobj_trans, 1, STRING("S_dakan_1"), 0, 30, 0)
        CALL(evt_hit_bind_update, STRING("a_dokan_1"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_1_k"), 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_switch_00_after)
    IF_INT_EQUAL(GSWF(1345), 0)
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_switch_00)
    CALL(evt_mapobj_get_position, STRING("S_dakan_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOKAN_APPEAR1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 30, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_dakan_1"), 0, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_dokan_1"))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_1_k"), 1)
    SET_VALUE(LW(0), STRING("dokan_1"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

static EVT_BEGIN(evt_switch_01_init)
    CALL(evt_hit_bind_mapobj, STRING("a_dokan_2"), STRING("S_dakan_2"))
    IF_INT_EQUAL(GSWF(1346), 1)
        CALL(evt_mapobj_trans, 1, STRING("S_dakan_2"), 0, 30, 0)
        CALL(evt_hit_bind_update, STRING("a_dokan_2"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_2_k"), 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_switch_01_after)
    IF_INT_EQUAL(GSWF(1346), 0)
        SET_VALUE(LW(0), STRING("dokan_2"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_switch_01)
    CALL(evt_mapobj_get_position, STRING("S_dakan_2"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOKAN_APPEAR1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 30, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_dakan_2"), 0, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("a_dokan_2"))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_2_k"), 1)
    SET_VALUE(LW(0), STRING("dokan_2"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

EVT_BEGIN(tik_16_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK7"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL_CHILD(evt_switch_00_init)
    CALL_CHILD(evt_switch_01_init)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_switch_00_after)
    CALL_CHILD(evt_switch_01_after)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_map_playanim, STRING("anm_mizu"), 1, 0)
    CALL(evt_mobj_lv_blk, STRING("block"), 0, 0, 0, 0, GSWF(1347), STRING("MOBJ_Lv1BigBigBlock"))
    IF_INT_EQUAL(GSWF(1347), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("block"), 128)
    END_IF
    SET_VALUE(LW(0), STRING("dai_3"))
    SET_VALUE(LW(1), STRING("a_dai_3"))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), -75)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 1)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT(moving_floor)
    CALL(evt_mobj_tornadoswitch_blue, 0, STRING("switch00"), -125, 0, 0, PTR(evt_switch_00), GSWF(1345))
    CALL(evt_mobj_tornadoswitch_blue, 0, STRING("switch01"), 125, 0, 0, PTR(evt_switch_01), GSWF(1346))
    CALL(evt_mobj_powerupblk, STRING("pblk1"), 125, 160, -100, 0, GSWF(5534), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

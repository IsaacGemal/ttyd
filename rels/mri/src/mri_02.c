#include "mri_02.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "rel/mri/mri.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_puni_dokan_onoff[];

// clang-format off
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[4] = {
    {"dokan1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, "mri_01", "dokan", 1, 1},
    {"dokan2", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, "mri_05", "dokan3", 1, 1},
    {"dokan3", 2, 0, 0x02, 100000, 0, 0, -1, NULL, 10, NULL, "mri_20", "dokan2", 1, 1},
    {NULL},
};

static EVT_BEGIN(evt_ueki_04)
    CALL(evt_item_delete_check, STRING("item_ueki_04"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_ueki_04"), 142, 100, 0, -20, 4, GSWF(2831), 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 ueki_access_data[24] = {
    0x00000001, (u32)"A_ueki01", (u32)"S_ueki01", 0x00000000,
    0x00000001, (u32)"A_ueki02", (u32)"S_ueki02", 0x00000000,
    0x00000001, (u32)"A_ueki03", (u32)"s_ueki03", 0x00000000,
    0x00000001, (u32)"A_ueki04", (u32)"S_ueki04", (u32)&evt_ueki_04,
    0x00000001, (u32)"A_ueki05", (u32)"S_ueki05", 0x00000000,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: write moveSwitch (global, 0x1d4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(punipuni_switch)
    CALL(moveSwitch)
    RETURN
EVT_END()

EVT_BEGIN(dokan_event)
    WAIT_WHILE(GF(15))
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 4500)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_dokan2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOKAN_QUAKE1"), LW(0), LW(1), LW(2), LW(10))
        WAIT_MS(4500)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 60, 126, 363, 60, 62, -3, 500, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        CALL(evt_mapobj_get_position, STRING("S_dokan2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOKAN_APPEAR1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, -50, 0, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, STRING("S_dokan2"), 0, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("A_dokan2"))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print_party, STRING("stg2_mri_e16_00"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 96)
    CALL(evt_puni_dokan_onoff, STRING("dokan2"), 0)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write getSwitch (global, 0x54 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_02.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(dokan_init)
    CALL(evt_hit_bind_mapobj, STRING("A_dokan2"), STRING("S_dokan2"))
    CALL(evt_mapobj_trans, 1, STRING("S_dokan2"), 0, -50, 0)
    CALL(evt_hit_bind_update, STRING("A_dokan2"))
    LOOP(0)
        CALL(getSwitch, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            IF_INT_EQUAL(GSWF(2830), 1)
                CALL_CHILD(dokan_event)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(yuka_main)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_isk_01_03_C"), 1)
    LOOP(0)
        SET_VALUE(LW(1), GW(0))
        DIVIDE(LW(1), 10)
        SET_VALUE(LW(0), GW(0))
        MODULO(LW(0), 10)
        SET_VALUE(LW(11), LW(1))
        MULTIPLY(LW(11), 36)
        SET_VALUE(LW(10), LW(0))
        MULTIPLY(LW(10), 36)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_01_02"), LW(11), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_01_03"), LW(10), 0, 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: write setPuniWeight (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_02.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(yuka_move)
    SET_VALUE(LW(12), GW(0))
    LOOP(0)
        SET_VALUE(LW(10), 1)
        SET_VALUE(LW(11), 0)
        LOOP(0)
            CALL(evt_puni_get_flag, LW(10), LW(0))
            CALL(evt_puni_get_flag, LW(10), LW(1))
            AND(LW(0), 1024)
            AND(LW(1), 128)
            IF_INT_NOT_EQUAL(LW(0), 0)
                IF_INT_EQUAL(LW(1), 0)
                    CALL(evt_puni_get_position, LW(10), LW(0), LW(1), LW(2))
                    IF_INT_GREATER_OR_EQUAL(LW(1), -20)
                        IF_INT_LESS_OR_EQUAL(LW(1), 20)
                            CALL(evt_sub_area_check, 0, 0, -20, 50, LW(0), LW(2), LW(15))
                            IF_INT_EQUAL(LW(15), 1)
                                ADD(LW(11), 1)
                            END_IF
                        END_IF
                    END_IF
                END_IF
            END_IF
            ADD(LW(10), 1)
            IF_INT_EQUAL(LW(10), 103)
                BREAK_LOOP
            END_IF
        END_LOOP
        SET_VALUE(GW(0), 10)
        SUBTRACT(GW(0), LW(11))
        IF_INT_GREATER_OR_EQUAL(GW(0), 10)
            SET_VALUE(GW(0), 10)
        END_IF
        IF_INT_LESS_OR_EQUAL(GW(0), 0)
            SET_VALUE(GW(0), 0)
        END_IF
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        SUBTRACT_FLOAT(LW(0), LW(11))
        IF_INT_EQUAL(GSWF(2830), 1)
            CALL(setPuniWeight, LW(0))
        ELSE
            CALL(setPuniWeight, 0)
        END_IF
        IF_INT_NOT_EQUAL(GW(0), LW(12))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_SWITCH_COUNTER1"), 0, 0, -305, 0)
        END_IF
        SET_VALUE(LW(12), GW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(punipuni_ball00_init)
    CALL_CHILD(mri_punipuni_setup)
    IF_INT_EQUAL(GSWF(2830), 1)
        CALL(evt_item_set_position, LW(10), 0, 30, -68)
        CALL(evt_target_flag_onoff, LW(11), 0, 3)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_hikari"), 1)
    END_IF
    CALL(evt_target_set_position, LW(11), 0, 0, -8)
    SET_VALUE(GF(15), 0)
    RETURN
EVT_END()

static EVT_BEGIN(punipuni_ball00)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(GSWF(2830), 0)
        CALL_CHILD(mri_punipuni_itemselect)
        IF_INT_EQUAL(LW(1), 0)
            GOTO(100)
        END_IF
        CALL(evt_item_set_position, LW(10), 0, 30, -68)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_sui_hikari"), 1)
        SET_VALUE(GF(15), 1)
        CALL_CHILD(mri_punipuni_itemset)
        SET_VALUE(GSWF(2830), 1)
        WAIT_MS(500)
        SET_VALUE(GF(15), 0)
    ELSE
        SET_VALUE(GSWF(2830), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_hikari"), 1)
        CALL_CHILD(mri_punipuni_itemremove)
    END_IF
    LABEL(100)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(mri_02_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 2, 0, -310, -1000, -1000, 1, 0)
    CALL(evt_puni_set_hamari, 0, 0, 0, 2, -2, -1000, -1000)
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
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 155, 0, 50, STRING("S_yuka"), 125, GSWF(5612))
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
    CALL(evt_hit_bind_mapobj, STRING("A_sui_l"), STRING("S_sui_l"))
    SPAWN_EVENT(punipuni_switch)
    SET_VALUE(LW(10), STRING("punipuni"))
    SET_VALUE(LW(11), 0)
    SPAWN_EVENT(punipuni_ball00_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_dai_mae"), 0, PTR(punipuni_ball00), 0)
    SET_VALUE(GW(0), 10)
    SPAWN_EVENT(yuka_main)
    SPAWN_EVENT(yuka_move)
    IF_INT_LESS(GSW(0), 96)
        SPAWN_EVENT(dokan_init)
        BEGIN_INLINE_EVENT
            CALL(evt_bero_exec_wait, 65536)
            CALL(evt_puni_dokan_onoff, STRING("dokan2"), 1)
        END_INLINE_EVENT
    END_IF
    CALL(evt_npc_change_fbat_mode, 1)
    SET_VALUE(LW(0), STRING("S_sui_l_02"))
    SET_RAW_VALUE(LW(1), GW(0))
    SET_VALUE(LW(2), 9)
    SPAWN_EVENT(mri_hikari_color)
    RETURN
EVT_END()

// TODO: type
u32 mri02switch[4] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: write getMri02Yuka (global, 0x10 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_02.s; this is where the original has it, just before the first script that uses it
// clang-format on

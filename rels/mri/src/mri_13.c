#include "mri_13.h"
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

#pragma pool_data off

#pragma force_active on
extern s32 evt_puni_flag_onoff[];
extern s32 evt_puni_get_flag[];
extern s32 evt_puni_get_mode[];
extern s32 evt_puni_get_position[];
extern s32 evt_puni_mem_init[];
extern s32 evt_puni_set_anim[];
extern s32 evt_puni_set_hamari[];
extern s32 evt_puni_set_mode[];
extern s32 evt_puni_set_yuka[];
extern s32 evt_puni_winddisable_onoff[];
extern s32 evt_target_flag_onoff[];
extern s32 evt_target_set_position[];
extern s32 mri_countdown[];
extern s32 mri_hikari_color[];
extern s32 mri_puni_init[];
extern s32 mri_puni_save[];
extern s32 mri_punipuni_itemremove[];
extern s32 mri_punipuni_itemselect[];
extern s32 mri_punipuni_itemset[];
extern s32 mri_punipuni_setup[];
extern s32 puni_disp_info[];

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

BeroEntry bero_dai3_kanbu[1] = {
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "mri_14", "", 1, 1},
};

static BeroEntry bero_entry_data[3] = {
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "mri_14", "w_bero", 1, 1},
    {"dokan", 2, 0, 0x06, 100000, 0, 0, -1, NULL, 10, NULL, "mri_16", "dokan7", 1, 1},
    {NULL},
};

static EVT_BEGIN(punipuni_mario_halt)
    SET_VALUE(GF(11), 1)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        SET_VALUE(GF(11), 0)
    END_INLINE_EVENT
    LOOP(0)
        IF_INT_EQUAL(GF(10), 0)
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(GF(11), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(punipuni_ball00_init)
    CALL_CHILD(mri_punipuni_setup)
    IF_INT_EQUAL(GSWF(2861), 1)
        IF_INT_LESS(GSW(0), 107)
            CALL(evt_item_set_position, LW(10), -55, 30, 0)
        ELSE
            CALL(evt_item_set_position, LW(10), -55, -279, 0)
        END_IF
        CALL(evt_target_flag_onoff, LW(11), 0, 3)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_hikari"), 1)
    END_IF
    IF_INT_LESS(GSW(0), 107)
        CALL(evt_target_set_position, LW(11), -55, 0, 50)
    ELSE
        CALL(evt_target_set_position, LW(11), -55, -309, 50)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(punipuni_ball00)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(GSWF(2861), 0)
        CALL_CHILD(mri_punipuni_itemselect)
        IF_INT_EQUAL(LW(1), 0)
            GOTO(100)
        END_IF
        SET_VALUE(GSWF(2861), 1)
        IF_INT_LESS(GSW(0), 107)
            CALL(evt_item_set_position, LW(10), -55, 30, 0)
        ELSE
            CALL(evt_item_set_position, LW(10), -55, -279, 0)
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_sui_hikari"), 1)
        SET_VALUE(GF(0), 1)
        CALL_CHILD(mri_punipuni_itemset)
        SET_VALUE(GF(0), 0)
    ELSE
        SET_VALUE(GSWF(2861), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_hikari"), 1)
        CALL_CHILD(mri_punipuni_itemremove)
    END_IF
    LABEL(100)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(yuka_init)
    CALL(evt_hit_bind_mapobj, STRING("A_dai"), STRING("S_dai"))
    CALL(evt_hit_bind_mapobj, STRING("A_dai_02"), STRING("S_dai"))
    CALL(evt_mapobj_trans, 1, STRING("S_dai"), 0, FLOAT(-309.0f), 0)
    CALL(evt_hit_bind_update, STRING("A_dai"))
    CALL(evt_hit_bind_update, STRING("A_dai_02"))
    CALL(evt_mapobj_trans, 1, STRING("S_sui_l"), 0, -309, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_sui_hikari"), 0, -309, 0)
    CALL(evt_hitobj_onoff, STRING("A_sui_l1"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_kage"), 1)
    RETURN
EVT_END()

EVT_BEGIN(yuka_slot)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_isk_02_c"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_isk_03_c"), 1)
    SET_VALUE(LW(13), GW(0))
    LOOP(0)
        SET_VALUE(LW(2), GW(0))
        DIVIDE(LW(2), 100)
        SET_VALUE(LW(1), GW(0))
        MODULO(LW(1), 100)
        DIVIDE(LW(1), 10)
        SET_VALUE(LW(0), GW(0))
        MODULO(LW(0), 10)
        SET_VALUE(LW(12), LW(2))
        MULTIPLY(LW(12), 36)
        SET_VALUE(LW(11), LW(1))
        MULTIPLY(LW(11), 36)
        SET_VALUE(LW(10), LW(0))
        MULTIPLY(LW(10), 36)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_02"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_03"), LW(11), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_04"), LW(12), 0, 0)
        IF_INT_GREATER_OR_EQUAL(GW(0), 10)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_isk_03_c"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_isk_03_c"), 1)
        END_IF
        IF_INT_NOT_EQUAL(GW(0), LW(13))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_SWITCH_COUNTER1"), -150, 0, -20, 0)
        END_IF
        SET_VALUE(LW(13), GW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yuka_shake)
    LOOP(5)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 1000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yuka_shake2)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 500)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.009f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.008f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.007f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.006f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.004f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.003f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.002f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.001f), 1000)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.015f), 1000)
    RETURN
EVT_END()

static EVT_BEGIN(yuka_check)
    LOOP(0)
        SET_VALUE(LW(10), 1)
        SET_VALUE(LW(11), 0)
        LOOP(0)
            CALL(evt_puni_get_flag, LW(10), LW(0))
            CALL(evt_puni_get_flag, LW(10), LW(1))
            AND(LW(0), 1024)
            AND(LW(1), 128)
            CALL(evt_puni_get_mode, LW(10), LW(2))
            IF_INT_NOT_EQUAL(LW(0), 0)
                IF_INT_EQUAL(LW(1), 0)
                    IF_INT_NOT_EQUAL(LW(2), 13)
                        CALL(evt_puni_get_position, LW(10), LW(0), LW(1), LW(2))
                        IF_INT_LESS(GSW(0), 107)
                            IF_INT_GREATER_OR_EQUAL(LW(1), -20)
                                IF_INT_LESS_OR_EQUAL(LW(1), 20)
                                    CALL(evt_sub_area_check, 0, -50, 60, 50, LW(0), LW(2), LW(15))
                                    IF_INT_EQUAL(LW(15), 1)
                                        ADD(LW(11), 1)
                                    END_IF
                                END_IF
                            END_IF
                        ELSE
                            IF_INT_GREATER_OR_EQUAL(LW(1), -329)
                                IF_INT_LESS_OR_EQUAL(LW(1), -289)
                                    CALL(evt_sub_area_check, 0, -50, 60, 50, LW(0), LW(2), LW(15))
                                    IF_INT_EQUAL(LW(15), 1)
                                        ADD(LW(11), 1)
                                    END_IF
                                END_IF
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
        SET_VALUE(GW(0), 101)
        SUBTRACT(GW(0), LW(11))
        IF_INT_GREATER_OR_EQUAL(GW(0), 101)
            SET_VALUE(GW(0), 101)
        END_IF
        IF_INT_LESS_OR_EQUAL(GW(0), 0)
            SET_VALUE(GW(0), 0)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: write yuka_fall_check (global, 0x134 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_13.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_puni_force_pose (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/mri/asm/mri_13.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(yuka_event)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_area_check, 1, -50, 60, 60, LW(0), LW(2), LW(14))
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_area_check, 1, -50, 60, 60, LW(0), LW(2), LW(15))
        IF_INT_EQUAL(GSWF(2861), 1)
            IF_INT_EQUAL(GW(0), 0)
                IF_INT_EQUAL(GF(0), 0)
                    IF_INT_EQUAL(LW(14), 1)
                        IF_INT_EQUAL(LW(15), 1)
                            BREAK_LOOP
                        END_IF
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_puni_winddisable_onoff, 1)
    CALL(evt_target_flag_onoff, 0, 1, 1)
    SET_VALUE(LW(10), 1)
    LOOP(0)
        CALL(evt_puni_set_mode, LW(10), 30)
        CALL(evt_puni_set_anim, LW(10), 0)
        CALL(evt_puni_force_pose, LW(10))
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_FRAMES(2)
    CALL(evt_mario_mov_pos2, -50, 60, FLOAT(120.0f))
    CALL(evt_mapobj_scale, 1, STRING("S_sui_kage"), FLOAT(0.5f), FLOAT(0.5f), FLOAT(0.5f))
    DELETE_EVENT(GW(1))
    DELETE_EVENT(GW(2))
    WAIT_MS(1000)
    CALL(evt_snd_sfxon, STRING("SFX_STG2_QUAKE1"), GW(4))
    SPAWN_EVENT_GET_ID(PTR(yuka_shake), LW(10))
    CALL(evt_puni_flag_onoff, 1, 0, 262144)
    CALL(evt_puni_set_mode, 1, 30)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイドsp"), 1536)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e21_00"), 0, STRING("ガイドsp"))
    CALL(evt_puni_flag_onoff, 1, 1, 262144)
    CALL(evt_puni_set_mode, 1, 30)
    CALL(evt_cam3d_evt_set, 330, -240, 404, -50, -60, 50, 0, 11)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -225, -80, 355, -50, -270, 50, 9000, 11)
    CALL(evt_snd_sfxoff, GW(4))
    DELETE_EVENT(LW(10))
    CALL(evt_snd_sfxon, STRING("SFX_STG2_QUAKE1"), GW(3))
    SPAWN_EVENT(yuka_shake2)
    CALL(evt_hit_bind_mapobj, STRING("A_sui_l1"), STRING("S_sui_l"))
    CALL(evt_hit_bind_mapobj, STRING("A_dai"), STRING("S_dai"))
    CALL(evt_hit_bind_mapobj, STRING("A_dai_02"), STRING("S_dai"))
    CALL(evt_snd_sfxon, STRING("SFX_STG2_FLOOR_MOVE1"), GW(2))
    CALL(evt_item_get_position, STRING("punipuni"), LW(3), LW(4), LW(5))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, FLOAT(0.5f), FLOAT(1.0f), 10000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_scale, 1, STRING("S_sui_kage"), LW(0), LW(0), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_kage"), 1)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 0, -30950, 10000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(LW(6), LW(4))
        ADD_FLOAT(LW(6), LW(0))
        CALL(evt_item_set_position, STRING("punipuni"), LW(3), LW(6), LW(5))
        CALL(evt_mapobj_trans, 1, STRING("S_sui_l"), 0, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("A_sui_l1"))
        CALL(evt_mapobj_trans, 1, STRING("S_dai"), 0, LW(0), 0)
        CALL(evt_hit_bind_update, STRING("A_dai"))
        CALL(evt_hit_bind_update, STRING("A_dai_02"))
        CALL(evt_mapobj_trans, 1, STRING("S_sui_hikari"), 0, LW(0), 0)
        CALL(yuka_fall_check)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxoff, GW(2))
    CALL(evt_snd_sfxoff, GW(3))
    CALL(evt_snd_sfxon, STRING("SFX_STG2_FLOOR_STOP1"), 0)
    WAIT_MS(1000)
    CALL(evt_puni_flag_onoff, 1, 0, 262144)
    CALL(evt_puni_set_mode, 1, 30)
    CALL(evt_npc_get_position, STRING("ガイドsp"), LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e21_00_01"), 0, STRING("ガイドsp"))
    CALL(evt_puni_flag_onoff, 1, 1, 262144)
    SET_VALUE(LW(10), 1)
    LOOP(0)
        CALL(evt_puni_set_mode, LW(10), 1)
        CALL(evt_puni_flag_onoff, LW(10), 0, 32768)
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_hitobj_onoff, STRING("A_sui_l1"), 1, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SPAWN_EVENT(yuka_check)
    CALL(evt_target_flag_onoff, 0, 0, 1)
    CALL(evt_target_set_position, 0, -55, -309, 50)
    SET_VALUE(GSW(0), 107)
    CALL(evt_puni_set_hamari, -180, -310, 60, 2, -2, -306, -312)
    CALL(evt_puni_winddisable_onoff, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_f1_ueki_01)
    CALL(evt_item_delete_check, STRING("item_ueki_01"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_ueki_01"), 148, -190, -309, 115, 4, GSWF(2862), 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 ueki_access_data[28] = {
    0x00000001, (u32)"A_f2_ueki02", (u32)"S_f2_ueki02", 0x00000000,
    0x00000001, (u32)"A_f2_ueki03", (u32)"S_f2_ueki03", 0x00000000,
    0x00000001, (u32)"A_f2_ueki04", (u32)"S_f2_ueki04", 0x00000000,
    0x00000001, (u32)"A_f1_ueki01", (u32)"S_f1_ueki01", (u32)&evt_f1_ueki_01,
    0x00000001, (u32)"A_f1_ueki02", (u32)"S_f1_ueki02", 0x00000000,
    0x00000001, (u32)"A_f1_ueki03", (u32)"S_f1_ueki03", 0x00000000,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(mri_13_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 13, 0, -310, -1000, -1000, 0, 0)
    CALL(evt_puni_set_hamari, -250, 0, 60, 2, -2, -306, -312)
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
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -150, -249, 0, PTR(mri_puni_save), 0)
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 8, -200, -249, 0, 0, 0)
    SET_VALUE(GW(0), 101)
    IF_INT_LESS(GSW(0), 107)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak"), 1, 0)
        SPAWN_EVENT_GET_ID(PTR(yuka_slot), GW(1))
        SPAWN_EVENT_GET_ID(PTR(yuka_check), GW(2))
        SPAWN_EVENT(yuka_event)
        CALL(evt_puni_set_hamari, -250, 0, 60, 2, -2, -306, -312)
    ELSE
        SPAWN_EVENT(yuka_init)
        SPAWN_EVENT(yuka_check)
        CALL(evt_puni_set_hamari, -180, -310, 60, 2, -2, -306, -312)
    END_IF
    SET_VALUE(LW(0), STRING("S_sui_l"))
    SET_RAW_VALUE(LW(1), GW(0))
    SET_VALUE(LW(2), 100)
    SPAWN_EVENT(mri_hikari_color)
    SET_VALUE(LW(10), STRING("punipuni"))
    SET_VALUE(LW(11), 0)
    SPAWN_EVENT(punipuni_ball00_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_dai"), 0, PTR(punipuni_ball00), 0)
    IF_INT_LESS(GSW(0), 108)
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("e_bero"), PTR(bero_dai3_kanbu))
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    RETURN
EVT_END()
// clang-format on

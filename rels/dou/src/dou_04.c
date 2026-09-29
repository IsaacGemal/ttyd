#include "battle/battle_sub.h"
#include "dou_04.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "mario/mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"

#include <string.h>

#pragma pool_data off

#pragma fp_contract on // REL: target has fused fmsubs/fmadds; the REL flag is missing it (report)

#pragma force_active on

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_entry_data[30] = {
    (u32)"e_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"dou_03", (u32)"w_bero_3",
    0x00010001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(box_trans)
    SET_FLOAT_VALUE(LW(3), 0)
    LOOP(0)
        CALL(evt_mapobj_trans, 1, STRING("s04_m_hako_a"), GW(0), GW(1), GW(2))
        CALL(evt_mapobj_trans, 1, STRING("mizuhashi"), GW(0), 0, GW(2))
        CALL(evt_hit_bind_update, STRING("a04_m_hako_03"))
        SET_FLOAT_VALUE(LW(4), GW(0))
        SUBTRACT_FLOAT(LW(4), LW(3))
        CALL(evt_mario_get_name_hitobj_ride, LW(2))
        IF_STR_EQUAL(LW(2), STRING("a04_m_hako_03"))
            CALL(evt_mario_set_hosei_xyz, LW(4), 0, 0)
        END_IF
        CALL(evt_party_get_name_hitobj_ride, 0, LW(2))
        IF_STR_EQUAL(LW(2), STRING("a04_m_hako_03"))
            CALL(evt_party_set_hosei_xyz, 0, LW(4), 0, 0)
        END_IF
        CALL(evt_party_get_name_hitobj_ride, 1, LW(2))
        IF_STR_EQUAL(LW(2), STRING("a04_m_hako_03"))
            CALL(evt_party_set_hosei_xyz, 1, LW(4), 0, 0)
        END_IF
        SET_FLOAT_VALUE(LW(3), GW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(box_pukapuka)
    CALL(evt_box_pukapuka, GW(1))
    RETURN
EVT_END()

EVT_BEGIN(box_event)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(2), FLOAT(0.0f))
    SPAWN_EVENT(box_trans)
    SPAWN_EVENT(box_pukapuka)
    CALL(evt_hit_bind_mapobj, STRING("a04_m_hako_03"), STRING("s04_m_hako_a"))
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 1, 0, 3, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 0, 3, 50, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mapobj_get_position, STRING("s04_m_hako_a"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_STEP_FLOAT1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_STEP_FLOAT2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, 50, 49, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, 49, 50, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 1, 50, 47, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 0, 47, 0, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mapobj_get_position, STRING("s04_m_hako_a"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_STEP_FLOAT1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_STEP_FLOAT2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, 0, 1, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, 1, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, GW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(lift_event_00)
    IF_INT_EQUAL(LW(0), 0)
        SET_FLOAT_VALUE(LW(0), LW(3))
        LOOP(0)
            CALL(evt_mapobj_trans, 1, STRING("s04_m_hako_b"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("a04_m_hako_02"))
            CALL(evt_mario_get_name_hitobj_ride, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_02"))
                CALL(evt_mario_set_hosei_xyz, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 0, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_02"))
                CALL(evt_party_set_hosei_xyz, 0, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 1, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_02"))
                CALL(evt_party_set_hosei_xyz, 1, 0, LW(0), 0)
            END_IF
            SET_VALUE(LF(10), 0)
            CALL(evt_mario_get_name_hitobj_head, LW(2))
            IF_STR_EQUAL(LW(2), STRING("soko2"))
                SET_VALUE(LF(10), 1)
            END_IF
            CALL(evt_mario_check_menu, LF(11))
            IF_INT_EQUAL(LF(10), 1)
                BREAK_LOOP
            END_IF
            ADD_FLOAT(LW(0), FLOAT(-0.75f))
            WAIT_FRAMES(1)
            IF_FLOAT_LESS_OR_EQUAL(LW(0), FLOAT(-180.0f))
                SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
                CALL(evt_mapobj_trans, 1, STRING("s04_m_hako_b"), 0, LW(0), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_mapobj_get_position, STRING("s04_m_hako_b"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_STEP_DOWN1"), LW(10), LW(11), LW(12), 0)
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.001f), 150)
    ELSE
        SET_FLOAT_VALUE(LW(0), LW(3))
        LOOP(0)
            CALL(evt_mapobj_trans, 1, STRING("s04_m_hako_b"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("a04_m_hako_02"))
            CALL(evt_mario_get_name_hitobj_ride, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_02"))
                CALL(evt_mario_set_hosei_xyz, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 0, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_02"))
                CALL(evt_party_set_hosei_xyz, 0, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 1, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_02"))
                CALL(evt_party_set_hosei_xyz, 1, 0, LW(0), 0)
            END_IF
            SET_VALUE(LF(10), 0)
            CALL(evt_mario_get_name_hitobj_head, LW(2))
            IF_STR_EQUAL(LW(2), STRING("soko1"))
                SET_VALUE(LF(10), 1)
            END_IF
            CALL(evt_mario_check_menu, LF(11))
            IF_INT_EQUAL(LF(10), 1)
                BREAK_LOOP
            END_IF
            ADD_FLOAT(LW(0), FLOAT(0.75f))
            WAIT_FRAMES(1)
            IF_FLOAT_GREATER_OR_EQUAL(LW(0), FLOAT(0.0f))
                SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                CALL(evt_mapobj_trans, 1, STRING("s04_m_hako_b"), 0, LW(0), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_IF
    SET_FLOAT_VALUE(LW(4), LW(0))
    RETURN
EVT_END()

EVT_BEGIN(lift_event_01)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_get_position, STRING("s04_m_ita_b"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_STEP_UP1"), LW(0), LW(1), LW(2), LW(9))
        SET_FLOAT_VALUE(LW(0), LW(3))
        LOOP(0)
            CALL(evt_mapobj_trans, 1, STRING("s04_m_ita_b"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("a04_m_hako_01"))
            CALL(evt_mario_get_name_hitobj_ride, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_01"))
                CALL(evt_mario_set_hosei_xyz, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 0, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_01"))
                CALL(evt_party_set_hosei_xyz, 0, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 1, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_01"))
                CALL(evt_party_set_hosei_xyz, 1, 0, LW(0), 0)
            END_IF
            SET_VALUE(LF(10), 0)
            CALL(evt_mario_get_name_hitobj_head, LW(2))
            IF_STR_EQUAL(LW(2), STRING("soko2"))
                SET_VALUE(LF(10), 1)
            END_IF
            CALL(evt_mario_check_menu, LF(11))
            IF_INT_EQUAL(LF(10), 1)
                BREAK_LOOP
            END_IF
            ADD_FLOAT(LW(0), FLOAT(0.5332031f))
            WAIT_FRAMES(1)
            IF_FLOAT_GREATER_OR_EQUAL(LW(0), FLOAT(128.0f))
                SET_FLOAT_VALUE(LW(0), FLOAT(128.0f))
                CALL(evt_mapobj_trans, 1, STRING("s04_m_ita_b"), 0, LW(0), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_snd_sfxoff, LW(9))
    ELSE
        CALL(evt_mapobj_get_position, STRING("s04_m_ita_b"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_STEP_FALL1"), LW(0), LW(1), LW(2), LW(9))
        SET_FLOAT_VALUE(LW(0), LW(3))
        LOOP(0)
            CALL(evt_mapobj_trans, 1, STRING("s04_m_ita_b"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("a04_m_hako_01"))
            CALL(evt_mario_get_name_hitobj_ride, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_01"))
                CALL(evt_mario_set_hosei_xyz, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 0, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_01"))
                CALL(evt_party_set_hosei_xyz, 0, 0, LW(0), 0)
            END_IF
            CALL(evt_party_get_name_hitobj_ride, 1, LW(2))
            IF_STR_EQUAL(LW(2), STRING("a04_m_hako_01"))
                CALL(evt_party_set_hosei_xyz, 1, 0, LW(0), 0)
            END_IF
            SET_VALUE(LF(10), 0)
            CALL(evt_mario_get_name_hitobj_head, LW(2))
            IF_STR_EQUAL(LW(2), STRING("soko1"))
                SET_VALUE(LF(10), 1)
            END_IF
            CALL(evt_mario_check_menu, LF(11))
            IF_INT_EQUAL(LF(10), 1)
                BREAK_LOOP
            END_IF
            ADD_FLOAT(LW(0), FLOAT(-0.5332031f))
            WAIT_FRAMES(1)
            IF_FLOAT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                CALL(evt_mapobj_trans, 1, STRING("s04_m_ita_b"), 0, LW(0), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_snd_sfxoff, LW(9))
    END_IF
    SET_FLOAT_VALUE(LW(4), LW(0))
    RETURN
EVT_END()

EVT_BEGIN(lift_main_00)
    SET_VALUE(LF(10), 0)
    LOOP(0)
        WAIT_WHILE(GF(10))
        IF_INT_EQUAL(LF(10), 0)
            SET_VALUE(LW(0), 0)
            SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
        ELSE
            SET_VALUE(LW(0), 0)
            SET_FLOAT_VALUE(LW(3), LW(4))
        END_IF
        CALL_CHILD(lift_event_00)
        WAIT_MS(4000)
        IF_INT_EQUAL(LF(10), 0)
            SET_VALUE(LW(0), 1)
            SET_FLOAT_VALUE(LW(3), FLOAT(-180.0f))
        ELSE
            SET_VALUE(LW(0), 1)
            SET_FLOAT_VALUE(LW(3), LW(4))
        END_IF
        CALL_CHILD(lift_event_00)
        SET_VALUE(GF(10), 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(lift_main_01)
    SET_VALUE(LF(10), 0)
    LOOP(0)
        WAIT_WHILE(GF(10))
        IF_INT_EQUAL(LF(10), 0)
            SET_VALUE(LW(0), 0)
            SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
        ELSE
            SET_VALUE(LW(0), 0)
            SET_FLOAT_VALUE(LW(3), LW(4))
        END_IF
        CALL_CHILD(lift_event_01)
        WAIT_MS(4000)
        IF_INT_EQUAL(LF(10), 0)
            SET_VALUE(LW(0), 1)
            SET_FLOAT_VALUE(LW(3), FLOAT(128.0f))
        ELSE
            SET_VALUE(LW(0), 1)
            SET_FLOAT_VALUE(LW(3), LW(4))
        END_IF
        CALL_CHILD(lift_event_01)
        SET_VALUE(GF(10), 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(lift_event_init)
    CALL(evt_hit_bind_mapobj, STRING("a04_m_hako_01"), STRING("s04_m_ita_b"))
    CALL(evt_hit_bind_mapobj, STRING("a04_m_hako_02"), STRING("s04_m_hako_b"))
    SET_VALUE(GF(10), 1)
    SPAWN_EVENT(lift_main_00)
    SPAWN_EVENT(lift_main_01)
    RETURN
EVT_END()

EVT_BEGIN(lift_event)
    SET_VALUE(GF(10), 0)
    RETURN
EVT_END()

// TODO: type
static u32 hitDmgRetPoints[16] = {
    (u32)"mod_01", 0x42F00000, 0x41200000, 0x00000000,
    (u32)"mod_02", 0xC2B40000, 0x41200000, 0xC2DC0000,
    (u32)"mod_03", 0xC3160000, 0x41200000, 0x42480000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(kuribo_init)
    IF_INT_EQUAL(GSW(1420), 21)
        CALL(evt_npc_set_position, STRING("me"), -183, 10, 135)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kuribo_talk)
    IF_INT_EQUAL(GSW(1420), 21)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_21_001"), 0, 0)
            SET_VALUE(GSWF(5405), 1)
        ELSE
            CALL(evt_mario_get_party, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                CALL(evt_party_stop, 0)
                CALL(evt_npc_set_ry, STRING("me"), 90)
                CALL(evt_party_set_dir_npc, 0, STRING("クリチェロ"))
                WAIT_MS(300)
                CALL(evt_msg_print, 0, STRING("irai_21_002"), 0, STRING("me"))
                SET_VALUE(GF(0), 1)
                BEGIN_INLINE_EVENT
                    CALL(evt_mario_mov_pos2, -120, 126, 50)
                    WAIT_FRAMES(1)
                    CALL(evt_mario_set_dir, 270, 300, 0)
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    CALL(evt_party_dispflg_onoff, 0, 16, 1)
                    CALL(evt_party_move_pos2, 0, -144, 86, 40)
                    WAIT_FRAMES(1)
                    CALL(evt_party_dispflg_onoff, 0, 16, 0)
                    SET_VALUE(GF(0), 0)
                END_INLINE_EVENT
                CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
                CALL(evt_npc_move_position, STRING("me"), -175, 86, 0, 40, 0)
                WAIT_WHILE(GF(0))
                WAIT_MS(500)
                CALL(evt_msg_print, 0, STRING("irai_21_003"), 0, STRING("me"))
                CALL(evt_msg_print, 0, STRING("irai_21_004"), 0, STRING("party"))
                CALL(evt_eff_fukidashi, 1, STRING(""), STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
                WAIT_MS(600)
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                ADD(LW(0), 5)
                CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, 30, 0)
                BEGIN_INLINE_EVENT
                    WAIT_MS(250)
                    CALL(evt_mario_dispflag_onoff, 1, 2)
                    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 200, 1, 5)
                    CALL(evt_mario_dispflag_onoff, 0, 2)
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    WAIT_MS(250)
                    CALL(evt_party_dispflg_onoff, 0, 16, 1)
                    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
                    CALL(evt_party_jump_pos, 0, LW(0), LW(1), LW(2), 200, 1, 5)
                    CALL(evt_party_dispflg_onoff, 0, 16, 0)
                END_INLINE_EVENT
                CALL(evt_msg_print, 0, STRING("irai_21_005"), 0, STRING("me"))
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                ADD(LW(0), 10)
                ADD(LW(1), 15)
                ADD(LW(2), 5)
                CALL(evt_item_entry, STRING("ff"), 219, LW(0), LW(1), LW(2), 17, GSWF(2991), 0)
                WAIT_MS(800)
                CALL(evt_msg_print, 0, STRING("irai_21_006"), 0, STRING("party"))
                WAIT_MS(500)
                CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 30)
                WAIT_MS(600)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                SUBTRACT(LW(0), 15)
                SUBTRACT(LW(2), 20)
                CALL(evt_party_move_pos2, 0, LW(0), LW(2), 60)
                CALL(evt_party_set_dir, 0, 270, 300)
                WAIT_MS(300)
                CALL(evt_msg_print, 0, STRING("irai_21_007"), 0, STRING("party"))
                BEGIN_INLINE_EVENT
                    CALL(evt_mario_set_pose, STRING("M_N_5B"))
                    WAIT_MS(500)
                    CALL(evt_mario_set_pose, STRING("M_I_G"))
                    WAIT_MS(600)
                    CALL(evt_mario_set_pose, STRING("M_S_1"))
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
                    WAIT_MS(600)
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    WAIT_MS(100)
                    CALL(evt_item_get_position, STRING("ff"), LW(2), LW(3), LW(4))
                    CALL(evt_sub_intpl_msec_init, 1, LW(3), 10, 100)
                    LOOP(0)
                        CALL(evt_sub_intpl_msec_get_value)
                        CALL(evt_item_set_position, STRING("ff"), LW(2), LW(0), LW(4))
                        WAIT_FRAMES(1)
                        IF_INT_EQUAL(LW(1), 0)
                            BREAK_LOOP
                        END_IF
                    END_LOOP
                END_INLINE_EVENT
                WAIT_MS(800)
                CALL(evt_msg_print, 0, STRING("irai_21_008"), 0, STRING("me"))
                CALL(evt_npc_move_position, STRING("me"), -140, -76, 0, 60, 0)
                WAIT_MS(1000)
                BEGIN_INLINE_EVENT
                    WAIT_MS(200)
                    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 30)
                    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 30)
                END_INLINE_EVENT
                CALL(evt_msg_print, 0, STRING("irai_21_009"), 0, STRING("me"))
                BEGIN_INLINE_EVENT
                    WAIT_MS(400)
                    CALL(evt_mario_set_dir, 90, 300, 0)
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    WAIT_MS(400)
                    CALL(evt_party_set_dir, 0, 90, 300)
                END_INLINE_EVENT
                CALL(evt_npc_move_position, STRING("me"), 140, -76, 0, 150, 0)
                CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
                CALL(evt_msg_print, 0, STRING("irai_21_010"), 0, STRING("party"))
                CALL(evt_item_get_position, STRING("ff"), LW(0), LW(1), LW(2))
                SUBTRACT(LW(2), 10)
                CALL(evt_party_move_pos2, 0, LW(0), LW(2), 40)
                CALL(evt_item_set_position, STRING("ff"), 0, -1000, 0)
                WAIT_MS(300)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                SUBTRACT(LW(0), 20)
                SUBTRACT(LW(2), 20)
                CALL(evt_party_move_pos2, 0, LW(0), LW(2), 40)
                WAIT_FRAMES(1)
                CALL(evt_party_set_dir, 0, 90, 300)
                CALL(evt_msg_print, 0, STRING("irai_21_011"), 0, STRING("party"))
                CALL(evt_mario_set_dir, 270, 300, 0)
                WAIT_MS(300)
                CALL(evt_pouch_get_haveitemcnt, LW(0))
                CALL(evt_pouch_check_item, 1, LW(1))
                IF_INT_EQUAL(LW(1), 0)
                    SET_VALUE(LW(1), 10)
                ELSE
                    SET_VALUE(LW(1), 20)
                END_IF
                IF_INT_LESS(LW(0), LW(1))
                    CALL(evt_msg_continue)
                    CALL(evt_item_get_item, STRING("ff"))
                    CALL(evt_item_get_item_end_wait, STRING("ff"))
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("irai_21_011_1"))
                    CALL(evt_party_set_dir, 0, 270, 300)
                    WAIT_MS(300)
                    CALL(evt_item_set_position, STRING("ff"), -165, 10, 106)
                    CALL(evt_item_flag_off, STRING("ff"), 2)
                    WAIT_MS(300)
                    CALL(evt_party_set_dir, 0, 90, 300)
                END_IF
                CALL(evt_msg_print, 0, STRING("irai_21_013"), 0, STRING("party"))
                CALL(evt_party_run, 0)
                SET_VALUE(GSW(1420), 0)
                SET_VALUE(GSWF(5395), 1)
                CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
            ELSE
                LOOP(0)
                    CALL(evt_mario_party_use_check, LW(1))
                    IF_INT_EQUAL(LW(1), 0)
                        BREAK_LOOP
                    END_IF
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(evt_party_stop, 0)
                SET_VALUE(GF(0), 1)
                BEGIN_INLINE_EVENT
                    CALL(evt_mario_mov_pos2, -150, 100, 60)
                    CALL(evt_mario_set_dir_npc, STRING("クリチェロ"))
                    SET_VALUE(GF(0), 0)
                END_INLINE_EVENT
                CALL(evt_party_move_pos2, 0, -130, 130, 60)
                CALL(evt_party_set_dir_npc, 0, STRING("クリチェロ"))
                WAIT_WHILE(GF(0))
                CALL(evt_npc_set_ry, STRING("me"), 90)
                WAIT_FRAMES(1)
                CALL(evt_npc_wait_pera, STRING("me"))
                SWITCH(LW(0))
                    CASE_EQUAL(2)
                    SET_VALUE(LW(0), STRING("irai_21_014"))
                    SET_VALUE(LW(1), STRING("irai_21_015"))
                    CASE_EQUAL(5)
                    SET_VALUE(LW(0), STRING("irai_21_016"))
                    SET_VALUE(LW(1), STRING("irai_21_017"))
                    CASE_EQUAL(4)
                    SET_VALUE(LW(0), STRING("irai_21_018"))
                    SET_VALUE(LW(1), STRING("irai_21_019"))
                    CASE_EQUAL(6)
                    SET_VALUE(LW(0), STRING("irai_21_020"))
                    SET_VALUE(LW(1), STRING("irai_21_021"))
                    CASE_EQUAL(3)
                    SET_VALUE(LW(0), STRING("irai_21_022"))
                    SET_VALUE(LW(1), STRING("irai_21_023"))
                    CASE_EQUAL(7)
                    SET_VALUE(LW(0), STRING("irai_21_024"))
                    SET_VALUE(LW(1), STRING("irai_21_025"))
                END_SWITCH
                CALL(evt_msg_print, 0, LW(0), 0, 0)
                BEGIN_INLINE_EVENT
                    WAIT_MS(200)
                    CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
                    CALL(evt_party_get_pos, 0, LW(5), LW(6), LW(7))
                    IF_INT_GREATER_OR_EQUAL(LW(2), LW(5))
                        CALL(evt_mario_set_dir, 270, 200, 0)
                    ELSE
                        CALL(evt_mario_set_dir, 90, 200, 0)
                    END_IF
                END_INLINE_EVENT
                CALL(evt_msg_print, 0, LW(1), 0, STRING("party"))
                CALL(evt_party_run, 0)
            END_IF
        END_IF
        RETURN
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[46] = {
    (u32)"クリチェロ", 0x00000600, 0x00000000, (u32)&kuribo_init,
    0x00000000, (u32)&kuribo_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(dou_04_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU6"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    WAIT_FRAMES(1)
    CALL(evt_npc_setup, PTR(npcEnt))
    SPAWN_EVENT(box_event)
    CALL(evt_mobj_switch_red, 0, STRING("mobj00"), 25, 32, 125, PTR(lift_event), 0)
    SPAWN_EVENT(lift_event_init)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("moku"), 1, 0)
    CALL(evt_item_entry, STRING("kagitsume"), 20, 122, 110, -118, 0, GSWF(2977), 0)
    CALL(evt_item_entry, STRING("starpice"), 125, -230, 200, 110, 0, GSWF(5646), 0)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -230, 290, -105, 0, GSWF(5555), 0)
    IF_INT_EQUAL(GSWF(5395), 1)
        CALL(evt_item_entry, STRING("iri_21"), 219, -165, 10, 106, 16, GSWF(2991), 0)
    END_IF
    RETURN
EVT_END()

USER_FUNC(evt_mapgrp_translate2) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32* work = (f32*)&event->lwData[6];
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 ridingName = evtGetValue(event, args[4]);
    HitEntry* hit;

    mapObjTranslate((const char*)name, x, y, z);
    x = 10.0f * (x / 10.0f);
    y = 10.0f * (y / 10.0f);
    z = 10.0f * (z / 10.0f);
    hit = marioGetPtr()->hitobjStandOn;
    if (hit != NULL && strcmp((const char*)ridingName, hitGetName(hit)) == 0) {
        *(f32*)((u8*)mario + 0x174) = x - work[0];
        *(f32*)((u8*)mario + 0x178) = y - work[1];
        *(f32*)((u8*)mario + 0x17C) = z - work[2];
    }
    work[0] = 10.0f * (x / 10.0f);
    work[1] = 10.0f * (y / 10.0f);
    work[2] = 10.0f * (z / 10.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_box_pukapuka) {
    s32* args = event->args;
    f32 value;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
    }
    value = (1.5f * sinfd((f32)event->userdata[0]) - 1.5f) + (0.7f * sinfd((f32)event->userdata[1]) - 0.7f);
    event->userdata[0] = (s32)reviseAngle((f32)(event->userdata[0] + 2));
    event->userdata[1] = (s32)reviseAngle((f32)(event->userdata[1] + 1));
    value -= 25.0f;
    evtSetFloat(event, args[0], value);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_mario_check_menu) {
    s32* args = event->args;
    s32 enabled = marioCheckMenuDisable();

    evtSetValue(event, args[0], enabled);
    return EVT_RETURN_DONE;
}

// clang-format on

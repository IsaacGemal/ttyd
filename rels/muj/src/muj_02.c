#include "muj_02.h"
#include "driver/dispdrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "driver/hitdrv.h"
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
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include <stdio.h>

#pragma pool_data off

#pragma force_active on

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
    {"w_bero", 0, 0, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "muj_01", "e_bero1", 1, 1},
    {"e_bero", 0, 0, 20000, 100000, 0, 0, -1, NULL, -1, NULL, "muj_03", "w_bero", 1, 1},
    {"dokan_1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, "tik_17", "dokan_2", 1, 1},
    {NULL},
};

EVT_BEGIN(mony_peton_setup)
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
        BEGIN_INLINE_EVENT
            ADD(LW(0), 40)
            ADD(LW(2), 30)
            CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(120.0f))
            WAIT_FRAMES(1)
            CALL(evt_mario_set_dir, 270, 200, 0)
            CALL(evt_mario_dispflag_onoff, 1, 2)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            ADD(LW(0), 75)
            ADD(LW(2), 30)
            CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(120.0f))
            WAIT_FRAMES(1)
            CALL(evt_party_set_dir, 0, 270, -1)
        END_INLINE_EVENT
        SUBTRACT(LW(0), 55)
        CALL(evt_npc_set_ry, STRING("ピートン"), 90)
        CALL(evt_npc_move_position, STRING("ピートン"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    ELSE
        CALL(evt_npc_get_position, STRING("ピートン"), LW(0), LW(1), LW(2))
        BEGIN_INLINE_EVENT
            ADD(LW(0), -40)
            ADD(LW(2), 30)
            CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(120.0f))
            WAIT_FRAMES(1)
            CALL(evt_mario_set_dir, 90, 200, 0)
            CALL(evt_mario_dispflag_onoff, 1, 2)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            ADD(LW(0), -75)
            ADD(LW(2), 30)
            CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(120.0f))
            WAIT_FRAMES(1)
            CALL(evt_party_set_dir, 0, 90, -1)
        END_INLINE_EVENT
        ADD(LW(0), 55)
        CALL(evt_npc_set_ry, STRING("モニー"), 270)
        CALL(evt_npc_move_position, STRING("モニー"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_IF
    RETURN
EVT_END()

void printCount(CameraId cameraId, void* param) {
    char buf[40];

    s32 count = evtGetValue(NULL, GW(0));
    sprintf(buf, "%3d回", count);
    FontDrawStart();
    FontDrawStringPitch(buf, 90.0f, 120.0f, 20.0f);
    count = evtGetValue(NULL, GW(0));
    sprintf(buf, "かい", count);
    FontDrawScale(0.55f);
    FontDrawStringPitch(buf, 148.0f, 131.0f, 20.0f);
}

EVT_BEGIN(lost_wedding_ring_sub)
    CALL(countDisp)
    RETURN
EVT_END()

EVT_BEGIN(lost_wedding_ring)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 32)
    CALL(evt_npc_get_reglid, STRING("モニー"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_get_reglid, STRING("ピートン"), LW(0))
    DELETE_EVENT(LW(0))
    CALL_CHILD(mony_peton_setup)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("mario"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_171"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_172"), 0, STRING("モニー"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_QUESTION1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 1, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("ピートン"), LW(10), LW(11), LW(12))
    ADD(LW(0), LW(10))
    DIVIDE(LW(0), 2)
    CALL(evt_cam3d_evt_set, LW(0), 118, 343, LW(0), 54, -18, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_173"), 0, STRING("モニー"))
    CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("モニー"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_174"), 0, STRING("ピートン"))
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("ピートン"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_175"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_176"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_177"), 0, STRING("モニー"))
    CALL(evt_npc_get_position, STRING("ピートン"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), LW(0), LW(1), LW(2), 225, 30)
    CALL(evt_msg_print, 0, STRING("stg5_muj_178"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_179"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_180"), 0, STRING("ピートン"))
    CALL(evt_get_target_dir, STRING("モニー"), STRING("ピートン"), LW(0))
    ADD(LW(0), 180)
    CALL(evt_npc_set_ry, STRING("モニー"), LW(0))
    CALL(evt_msg_print, 0, STRING("stg5_muj_181"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_182"), 0, STRING("ピートン"))
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("ピートン"))
    WAIT_MS(500)
    SET_VALUE(GW(0), 10)
    SPAWN_EVENT_GET_ID(PTR(lost_wedding_ring_sub), LW(0))
    LOOP(90)
        CALL(evt_msg_print_add, 0, STRING("stg5_muj_182_01"))
        ADD(GW(0), 1)
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("stg5_muj_182_02"))
    DELETE_EVENT(LW(0))
    CALL(evt_msg_print_add, 0, STRING("stg5_muj_182_03"))
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("ピートン"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_183"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_184"), 0, STRING("ピートン"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_get_pos, 0, LW(10), LW(11), LW(12))
    ADD(LW(0), LW(10))
    DIVIDE(LW(0), 2)
    CALL(evt_cam3d_evt_set, LW(0), 118, 343, LW(0), 54, -18, 500, 11)
    WAIT_MS(500)
    CALL(evt_pouch_check_item, 73, LW(10))
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_msg_print_party, STRING("stg5_muj_185"))
    ELSE
        CALL(evt_msg_print_party, STRING("stg5_muj_185_01"))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    CALL(evt_cam3d_evt_off, 500, 11)
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_npc_set_ry, STRING("ピートン"), 270)
        CALL(evt_npc_move_position, STRING("ピートン"), 400, -40, 0, FLOAT(120.0f), 0)
    ELSE
        CALL(evt_npc_set_ry, STRING("モニー"), 90)
        CALL(evt_npc_move_position, STRING("モニー"), 500, -40, 0, FLOAT(120.0f), 0)
    END_IF
    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 32)
    CALL(evt_npc_flag_onoff, 0, STRING("モニー"), 32)
    CALL(evt_npc_restart_regular_event, STRING("モニー"))
    CALL(evt_npc_restart_regular_event, STRING("ピートン"))
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

// TODO: type
static u32 itemsel_table00[2] = {
    0x00000049, 0xFFFFFFFF,
};

EVT_BEGIN(wedding_ring_consign)
    SET_VALUE(LW(0), 0)
    CALL(evt_pouch_check_item, 73, LW(10))
    IF_INT_NOT_EQUAL(LW(10), 0)
        CALL(evt_win_item_select, 0, PTR(itemsel_table00), LW(10), 0)
        IF_INT_NOT_EQUAL(LW(10), -1)
            CALL(evt_pouch_remove_item, 73, LW(10))
            SET_VALUE(LW(0), 1)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(find_wedding_ring)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 1073741856)
    CALL(evt_npc_get_reglid, STRING("モニー"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_get_reglid, STRING("ピートン"), LW(0))
    DELETE_EVENT(LW(0))
    CALL_CHILD(mony_peton_setup)
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("ピートン"))
    CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("モニー"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_196"), 0, STRING("モニー"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_197"), 0, STRING("ピートン"))
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("mario"))
    CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("mario"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_198"), 0, STRING("モニー"))
    SET_VALUE(GF(0), 1)
    SET_VALUE(GF(1), 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
        ADD(LW(0), -10)
        ADD(LW(2), 110)
        CALL(evt_npc_set_ry, STRING("モニー"), 270)
        CALL(evt_npc_move_position, STRING("モニー"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
        ADD(LW(0), -200)
        CALL(evt_npc_move_position, STRING("モニー"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
        CALL(evt_npc_set_position, STRING("モニー"), 0, -1000, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ピートン"), LW(0), LW(1), LW(2))
        ADD(LW(0), -10)
        ADD(LW(2), 110)
        CALL(evt_npc_set_ry, STRING("ピートン"), 270)
        CALL(evt_npc_move_position, STRING("ピートン"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
        ADD(LW(0), -200)
        CALL(evt_npc_move_position, STRING("ピートン"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
        CALL(evt_npc_set_position, STRING("ピートン"), 0, -1000, 0)
        SET_VALUE(GF(1), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    WAIT_WHILE(GF(1))
    CALL(evt_party_run, 0)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    RETURN
EVT_END()

static EVT_BEGIN(mony_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    SWITCH(GSW(0))
        CASE_BETWEEN(274, 275)
        CALL(evt_npc_set_position, STRING("me"), 500, 0, -40)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(mony_peton_regl)
    LOOP(0)
        SET_VALUE(LW(3), LW(0))
        CALL(evt_sub_random, 15, LW(10))
        ADD(LW(10), 10)
        SUBTRACT(LW(3), LW(10))
        SET_VALUE(LW(4), LW(0))
        CALL(evt_sub_random, 15, LW(10))
        ADD(LW(10), 10)
        ADD(LW(4), LW(10))
        CALL(evt_sub_random, 500, LW(5))
        ADD(LW(5), 1000)
        CALL(evt_sub_random, 500, LW(6))
        ADD(LW(6), 1000)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        WAIT_MS(600)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        WAIT_MS(600)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_move_position, STRING("me"), LW(3), LW(2), 0, FLOAT(45.0f), 0)
        WAIT_MS(LW(5))
        CALL(evt_npc_set_ry, STRING("me"), 90)
        WAIT_MS(600)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        WAIT_MS(600)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_move_position, STRING("me"), LW(4), LW(2), 0, FLOAT(45.0f), 0)
        WAIT_MS(LW(6))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mony_regl)
    SET_VALUE(LW(0), 500)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), -40)
    CALL_CHILD(mony_peton_regl)
    RETURN
EVT_END()

static EVT_BEGIN(mony_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(274)
        CALL(evt_msg_print, 0, STRING("stg5_muj_169"), 0, STRING("me"))
        SET_VALUE(LW(10), 0)
        CALL_CHILD(lost_wedding_ring)
        SET_VALUE(GSW(0), 275)
        CASE_EQUAL(275)
        CALL_CHILD(wedding_ring_consign)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("stg5_muj_192"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_194"), 0, STRING("me"))
            SET_VALUE(LW(10), 0)
            CALL_CHILD(find_wedding_ring)
            SET_VALUE(GSW(0), 276)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    SWITCH(GSW(0))
        CASE_BETWEEN(274, 275)
        CALL(evt_npc_set_position, STRING("me"), 400, 0, -40)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(peton_regl)
    SET_VALUE(LW(0), 400)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), -40)
    CALL_CHILD(mony_peton_regl)
    RETURN
EVT_END()

static EVT_BEGIN(peton_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(274)
        CALL(evt_msg_print, 0, STRING("stg5_muj_170"), 0, STRING("me"))
        SET_VALUE(LW(10), 1)
        CALL_CHILD(lost_wedding_ring)
        SET_VALUE(GSW(0), 275)
        CASE_EQUAL(275)
        CALL_CHILD(wedding_ring_consign)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("stg5_muj_193"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("stg5_muj_195"), 0, STRING("me"))
            SET_VALUE(LW(10), 1)
            CALL_CHILD(find_wedding_ring)
            SET_VALUE(GSW(0), 276)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[3] = {
    {"モニー", 0x600, 0, mony_init, mony_regl, mony_talk},
    {"ピートン", 0x600, 0, peton_init, peton_regl, peton_talk},
    {NULL},
};

static HitDamageReturn hitDmgRetPoints[2] = {
    {"A_yuka_01", {0.0f, 0.0f, 0.0f}},
    {NULL},
};

static EVT_BEGIN(evt_switch_init)
    IF_INT_EQUAL(GSWF(1349), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_dakan"), 1)
        CALL(evt_hitobj_onoff, STRING("A_dokan"), 1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_treeb_01)
    CALL(evt_item_delete_check, STRING("item_treeb_01"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_treeb_01"), 170, -350, 90, -70, 14, GSWF(3152), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_kusa_2)
    CALL(evt_item_delete_check, STRING("item_kusa_2"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_2"), 121, 323, 0, 50, 4, GSWF(3154), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_kusa_6)
    CALL(evt_item_delete_check, STRING("item_kusa_6"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_6"), 125, -500, 0, 110, 4, GSWF(5641), 0)
    END_IF
    RETURN
EVT_END()

static TreeAccessEntry tree_tbl[7] = {
    {5, "A_treeA_01", "treeA_01_m", "treeA_01_ha", NULL, NULL},
    {5, "A_treeA_02", "treeA_02_m", "treeA_02_ha", NULL, NULL},
    {5, "A_treeB_01", "treeB_01_m", "treeB_01_ha", NULL, evt_treeb_01},
    {5, "A_treeB_02", "treeB_02_m", "treeB_02_ha", NULL, NULL},
    {5, "A_treeB_03", "treeB_03_m", "treeB_03_ha", NULL, NULL},
    {5, "A_treeB_04", "treeB_04_m", "treeB_04_ha", NULL, NULL},
    {7},
};

static KusaAccessEntry kusa_tbl[7] = {
    {1, "A_kuwa_move_01", "S_kuwa_move_01", NULL},
    {1, "A_kuwa_move_02", "S_kuwa_move_02", evt_kusa_2},
    {1, "A_kuwa_move_03", "S_kuwa_move_03", NULL},
    {1, "A_kuwa_move_04", "S_kuwa_move_04", NULL},
    {1, "A_kuwa_move_05", "S_kuwa_move_05", NULL},
    {1, "A_kuwa_move_06", "S_kuwa_move_06", evt_kusa_6},
    {4},
};

EVT_BEGIN(muj_02_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ4"))
    SET_VALUE(GSWF(3152), 0)
    CALL_CHILD(evt_switch_init)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 269)
        CALL(evt_npc_check_delete, STRING("チョロボン１"))
        CALL(evt_npc_check_delete, STRING("チョロボン２"))
        CALL(evt_npc_check_delete, STRING("チョロボン７"))
    END_IF
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_eff64, STRING("sunlight"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mobj_badgeblk, STRING("mobj_badge_gat"), -200, 60, -90, 251, 0, GSWF(3146), 1)
    CALL(evt_mobj_badgeblk, STRING("mobj_hatena"), -135, 0, 135, 143, 0, GSWF(3150), 0)
    SET_VALUE(LW(0), PTR(tree_tbl))
    CALL_CHILD(evt_sub_tree_access_entry)
    SET_VALUE(LW(0), PTR(kusa_tbl))
    CALL_CHILD(evt_sub_kusa_access_entry)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 2)
    RETURN
EVT_END()

USER_FUNC(countDisp) {
    dispEntry(CAMERA_2D, 399.0f, 0, printCount, NULL);
    return 0;
}
// clang-format on
#pragma force_active off

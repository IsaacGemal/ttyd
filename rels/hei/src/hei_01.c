#include "hei_01.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 all_party_lecture_12_data_1CF80[];
static USER_FUNC(set_dir);

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(bero_out_event)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[5] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_13", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, bero_out_event, "nok_00", "w_bero", 1, 1},
    {"dokan_h", 4098, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan", 1, 1},
    {"dokan", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_h", 1, 1},
    {NULL},
};

static EVT_BEGIN(evt_kusa_2)
    CALL(evt_item_delete_check, STRING("item_kusa_2"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_2"), 121, -200, 0, 120, 4, GSWF(1802), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_kusa_4)
    CALL(evt_item_delete_check, STRING("item_kusa_4"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_4"), 163, 190, 0, -70, 4, GSWF(1784), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_kusa_7)
    CALL(evt_item_delete_check, STRING("item_kusa_7"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_7"), 121, -285, 0, 85, 4, GSWF(1800), 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 kusa_data[28] = {
    0x00000000, (u32)"a_kusa_2", (u32)"kusa_2", (u32)&evt_kusa_2,
    0x00000000, (u32)"a_kusa_3", (u32)"kusa_7", 0x00000000,
    0x00000000, (u32)"a_kusa_4", (u32)"kusa_4", (u32)&evt_kusa_4,
    0x00000000, (u32)"a_kusa_6", (u32)"kusa_6", 0x00000000,
    0x00000000, (u32)"a_kusa_7", (u32)"kusa_9", (u32)&evt_kusa_7,
    0x00000000, (u32)"a_kusa_8", (u32)"kusa_pPlane371_q00274", 0x00000000,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(kusa_dokan)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_get_position, STRING("anm_kusa_1"), LW(5), LW(6), LW(7))
    CALL(evt_snd_sfxon_3d, STRING("SFX_GRASS_SHAKE1"), LW(5), LW(6), LW(7), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(600)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOKAN_APPEAR1"), LW(5), LW(6), LW(7), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_kusa_1"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_kusa_1"), LW(0), LW(1))
    WAIT_MS(LW(1))
    SET_VALUE(GSWF(1783), 1)
    CALL(evt_hitobj_onoff, STRING("a_kusa_1"), 0, 0)
    SET_VALUE(LW(0), STRING("dokan"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static char* name_tbl[25] = {
    "hasi_1", "hasi_2", "hasi_3", "hasi_4", "hasi_5", "hasi_6", "hasi_7", "hasi_8", "hasi_9",
    "hasi_10", "hasi_11", "hasi_12", "hasi_13", "hasi_14", "hasi_15", "hasi_16", "hasi_17",
    "hasi_18", "hasi_19", "hasi_20", "hasi_21", "hasi_22", "hasi_23", "hasi_24", "hasi_25",
};

EVT_BEGIN(bridge_evt_02)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -15, 150, 500, -15, 45, 85, 500, 11)
    WAIT_MS(1000)
    CALL(evt_paper_entry, STRING("OFF_hei_01c"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_hei_01c"))
    SET_INT_READ_ADDR(PTR(name_tbl))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, 0, 1, 54, 140, 500, 250)
    CALL(evt_mapobj_set_offscreen, 1, STRING("S"), STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("hasi"))
    LOOP(0)
        CALL(evt_img_free_capture, STRING("capture_img"), 0)
        CALL(evt_img_alloc_capture, STRING("capture_img"), 0, 0, 1, 54, 140, 500, 250)
        WAIT_FRAMES(1)
        SET_VALUE(LW(10), GW(0))
        READ_INT_INDEXED(LW(11), LW(10))
        ADD(GW(0), 1)
        CALL(evt_mapobj_set_offscreen, 1, LW(11), STRING("offscreen"))
        CALL(evt_img_free_capture, STRING("capture_img"), 1)
        CALL(evt_img_alloc_capture, STRING("capture_img"), 1, STRING("offscreen"), 1, 54, 140, 500, 250)
        CALL(evt_snd_sfxon, STRING("SFX_STG1_PERA1"), 0)
        SWITCH(GW(0))
            CASE_LESS_OR_EQUAL(2)
            CALL(evt_img_set_paper_timerate, STRING("capture_img"), FLOAT(1.2f))
            CASE_LESS_OR_EQUAL(3)
            CALL(evt_img_set_paper_timerate, STRING("capture_img"), FLOAT(2.4f))
            CASE_DEFAULT
            CALL(evt_img_set_paper_timerate, STRING("capture_img"), FLOAT(4.0f))
        END_SWITCH
        CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
        CALL(evt_img_wait_animend, STRING("capture_img"))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(11), 1)
        IF_INT_GREATER_OR_EQUAL(GW(0), 25)
            CALL(evt_img_free_capture, STRING("capture_img"), 0)
            CALL(evt_img_free_capture, STRING("capture_img"), 1)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S"))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(bridge_evt)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    BEGIN_INLINE_EVENT
        CALL(evt_mobj_wait_animation_end, STRING("bg_switch"))
        CALL(evt_mobj_delete, STRING("bg_switch"))
    END_INLINE_EVENT
    CALL_CHILD(bridge_evt_02)
    CALL(evt_hitobj_onoff, STRING("a_hasi"), 1, 1)
    SET_VALUE(GSW(0), 25)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_start_for_event)
    RETURN
EVT_END()

EVT_BEGIN(nokotaro_get)
    CALL(evt_npc_flag_onoff, 0, STRING("ノコタロウ"), 512)
    CALL(evt_npc_set_anim, STRING("ノコタロウ"), STRING("PNK_Y_2"))
    CALL(evt_msg_print_add, 0, STRING("stg1_hei_07"))
    CALL(evt_npc_set_anim, STRING("ノコタロウ"), STRING("PNK_S_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("ノコタロウ"), 512)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 400, 85, 255, 400, 40, -10, 800, 11)
    WAIT_MS(800)
    SET_VALUE(LW(8), STRING("ノコタロウ"))
    SET_VALUE(LW(9), 2)
    CALL_CHILD(all_party_lecture_12_data_1CF80)
    CALL(evt_msg_print, 0, STRING("stg1_hei_09"), 0, STRING("party"))
    SET_VALUE(GSW(0), 33)
    RETURN
EVT_END()

EVT_BEGIN(nokotaro_kanyu)
    CALL(evt_msg_select, 0, STRING("stg1_hei_12_yesno1"))
    CALL(evt_msg_print_add, 1, STRING("<scrl_auto><once_stop>"))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(nokotaro_get)
    ELSE
        CALL(evt_msg_print_add, 0, STRING("stg1_hei_10"))
        CALL(evt_msg_select, 0, STRING("stg1_hei_12_yesno2"))
        CALL(evt_msg_print_add, 1, STRING("<scrl_auto><once_stop>"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("stg1_hei_11"))
        ELSE
            CALL_CHILD(nokotaro_get)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(nokotaro_nakama)
    IF_INT_EQUAL(GSWF(1774), 0)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(1775), 0)
        RETURN
    END_IF
    IF_INT_NOT_EQUAL(GSW(0), 32)
        RETURN
    END_IF
    IF_INT_NOT_EQUAL(GSWF(1781), 0)
        CALL(evt_npc_set_position, STRING("ノコタロウ"), 400, 0, 0)
        CALL(evt_npc_set_ry, STRING("ノコタロウ"), 90)
        RETURN
    END_IF
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(0), 500)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff_f, 512, 3000)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    CALL(evt_npc_set_position, STRING("ノコタロウ"), 300, 0, 0)
    CALL(evt_npc_set_ry, STRING("ノコタロウ"), 90)
    CALL(evt_msg_print, 0, STRING("stg1_hei_03"), 0, STRING("ノコタロウ"))
    CALL(evt_cam3d_evt_set, 300, 134, 524, 300, 38, -17, 1000, 11)
    WAIT_MS(1500)
    CALL(evt_msg_print, 0, STRING("stg1_hei_04"), 0, STRING("ノコタロウ"))
    CALL(evt_cam3d_evt_set, 425, 85, 253, 425, 38, -12, 1500, 11)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("ノコタロウ"), 400, 0, 0, FLOAT(130.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 450, 0, FLOAT(80.0f))
        CALL(evt_mario_set_dir_npc, STRING("ノコタロウ"))
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("stg1_hei_05"), 0, STRING("ノコタロウ"))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_jump_pos, LW(0), 0, LW(2), 200, 1, -1000)
    WAIT_MS(700)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_msg_print, 0, STRING("stg1_hei_05_1"), 0, STRING("ノコタロウ"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_THINK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_1"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg1_hei_05_2"), 0, STRING("ノコタロウ"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_hei_06"), 0, STRING("ノコタロウ"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_THINK1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_1"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg1_hei_06_1"), 0, STRING("ノコタロウ"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(nokotaro_kanyu)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(500)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_start_for_event)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG1_HEI1"), 2000)
    SET_VALUE(GSWF(1781), 1)
    RETURN
EVT_END()

static EVT_BEGIN(nokotarou_init)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(nokotarou_reg)
    RETURN
EVT_END()

EVT_BEGIN(nokotarou_talk_2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_stop_for_event)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12"), 0, STRING("ノコタロウ"))
    CALL_CHILD(nokotaro_kanyu)
    CALL(evt_npc_start_for_event)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 33)
        CALL(evt_npc_change_fbat_mode, 1)
    ELSE
        CALL(evt_party_run, 0)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(nokotarou_talk)
    SPAWN_EVENT(nokotarou_talk_2)
    RETURN
EVT_END()

EVT_BEGIN(nokorin_pose)
    CALL(evt_npc_get_position, STRING("ノコリン"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKORIN_MOVE1"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_npc_flag_onoff, 0, STRING("ノコリン"), 1024)
    CALL(evt_npc_pera_onoff, STRING("ノコリン"), 0)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(0)
            CALL(evt_npc_set_ry, STRING("ノコリン"), LW(0))
            ADD(LW(0), -30)
            IF_INT_LESS(LW(0), 0)
                ADD(LW(0), 360)
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_npc_set_anim, STRING("ノコリン"), STRING("A_1"))
    CALL(evt_snd_sfxoff, LW(3))
    RETURN
EVT_END()

EVT_BEGIN(evt_nokorin)
    IF_INT_EQUAL(GSWF(1782), 1)
        RETURN
    END_IF
    LABEL(0)
    WAIT_FRAMES(1)
    IF_INT_LESS(GSW(0), 33)
        GOTO(0)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), -20)
        CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(40.0f))
    END_INLINE_EVENT
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -20)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(40.0f))
    CALL(evt_npc_set_position, STRING("ノコリン"), 700, 0, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_01"), 0, STRING("ノコリン"))
    CALL(evt_party_set_dir_npc, 0, STRING("ノコリン"))
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 3, STRING("a"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_mario_set_dir_npc, STRING("ノコリン"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_N"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("b"), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1200)
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 550, 89, 279, 550, 38, -8, 1000, 11)
    CALL(evt_npc_move_position, STRING("ノコリン"), 550, 0, 0, FLOAT(80.0f), 1)
    WAIT_MS(2000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_mario_set_pos, 370, 0, 0)
        CALL(evt_party_set_pos, 0, 400, 0, 0)
    ELSE
        CALL(evt_mario_set_pos, 400, 0, 0)
        CALL(evt_party_set_pos, 0, 370, 0, 0)
    END_IF
    CALL(evt_cam3d_evt_set, 400, 89, 279, 400, 38, -8, 0, 11)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_02"), 0, STRING("party"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 460, 126, 485, 460, 40, -8, 0, 11)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_03"), 0, STRING("ノコリン"))
    WAIT_MS(1200)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_04"), 0, STRING("party"))
    CALL(evt_cam3d_evt_set, 550, 89, 279, 550, 38, -8, 0, 11)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_05"), 0, STRING("ノコリン"))
    CALL(evt_cam3d_evt_set, 380, 89, 279, 380, 38, -8, 0, 11)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_06"), 0, STRING("party"))
    CALL(evt_cam3d_evt_set, 460, 126, 485, 460, 40, -8, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GF(10), 0)
    SET_VALUE(GF(11), 0)
    SET_VALUE(GF(12), 0)
    BEGIN_INLINE_EVENT
        LABEL(10)
        CALL(evt_npc_get_position, STRING("ノコリン"), LW(10), LW(11), LW(12))
        CALL(evt_mario_get_pos, 0, LW(13), LW(14), LW(15))
        IF_INT_GREATER(LW(10), LW(13))
            WAIT_FRAMES(1)
            GOTO(10)
        END_IF
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_DAMAGE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_D_1"))
        CALL(evt_sub_intpl_msec_init, 5, 0, 3600, 2000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mario_set_dispdir, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        SET_VALUE(GF(10), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LABEL(11)
        CALL(evt_npc_get_position, STRING("ノコリン"), LW(10), LW(11), LW(12))
        CALL(evt_party_get_pos, 0, LW(13), LW(14), LW(15))
        IF_INT_GREATER(LW(10), LW(13))
            WAIT_FRAMES(1)
            GOTO(11)
        END_IF
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PARTY_DAMAGE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_party_set_pose, 0, STRING("PNK_D_1"))
        CALL(evt_sub_intpl_msec_init, 5, 0, 3600, 2000)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_party_set_dispdir, 0, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
        CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
        SET_VALUE(GF(11), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg1_hei_12_07"), 0, 0)
        SET_VALUE(GF(12), 1)
    END_INLINE_EVENT
    CALL_CHILD(nokorin_pose)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ノコリン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKORIN_MOVE2"), LW(0), LW(1), LW(2), LW(3))
        LOOP(0)
            CALL(evt_snd_sfxchk, LW(3), LW(0))
            IF_INT_EQUAL(LW(0), -1)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ノコリン"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(3), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 385, 126, 485, 385, 40, -8, 500, 11)
    CALL(evt_npc_move_position, STRING("ノコリン"), 200, 0, 0, FLOAT(450.0f), 1)
    LOOP(0)
        IF_INT_EQUAL(GF(10), 1)
            IF_INT_EQUAL(GF(11), 1)
                IF_INT_EQUAL(GF(12), 1)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    SET_VALUE(GF(10), 0)
    SET_VALUE(GF(11), 0)
    SET_VALUE(GF(12), 0)
    BEGIN_INLINE_EVENT
        LABEL(12)
        CALL(evt_npc_get_position, STRING("ノコリン"), LW(10), LW(11), LW(12))
        CALL(evt_mario_get_pos, 0, LW(13), LW(14), LW(15))
        IF_INT_GREATER(LW(13), LW(10))
            WAIT_FRAMES(1)
            GOTO(12)
        END_IF
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1_2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_DAMAGE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_D_1"))
        CALL(evt_sub_intpl_msec_init, 5, 0, -3780, 2000)
        LABEL(3)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mario_set_dispdir, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(3)
        END_IF
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        SET_VALUE(GF(10), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LABEL(13)
        CALL(evt_npc_get_position, STRING("ノコリン"), LW(10), LW(11), LW(12))
        CALL(evt_party_get_pos, 0, LW(13), LW(14), LW(15))
        IF_INT_GREATER(LW(13), LW(10))
            WAIT_FRAMES(1)
            GOTO(13)
        END_IF
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_PARTY_DAMAGE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_party_set_pose, 0, STRING("PNK_D_1"))
        CALL(evt_sub_intpl_msec_init, 5, 0, -3780, 2000)
        LABEL(4)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_party_set_dispdir, 0, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(4)
        END_IF
        CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
        SET_VALUE(GF(11), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg1_hei_12_08"), 0, 0)
        SET_VALUE(GF(12), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ノコリン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_NOKORIN_MOVE2"), LW(0), LW(1), LW(2), LW(3))
        LOOP(0)
            CALL(evt_snd_sfxchk, LW(3), LW(0))
            IF_INT_EQUAL(LW(0), -1)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ノコリン"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(3), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("ノコリン"), 600, 0, 0, FLOAT(450.0f), 1)
    CALL(evt_npc_set_position, STRING("ノコリン"), 0, -1000, 0)
    LOOP(0)
        IF_INT_EQUAL(GF(10), 1)
            IF_INT_EQUAL(GF(11), 1)
                IF_INT_EQUAL(GF(12), 1)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_09"), 0, STRING("party"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    IF_INT_LESS_OR_EQUAL(LW(3), LW(0))
        CALL(evt_mario_set_dir, 90, 300, 0)
        CALL(evt_party_set_dir, 0, 270, 300)
    ELSE
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_party_set_dir, 0, 90, 300)
    END_IF
    CALL(evt_msg_print, 0, STRING("stg1_hei_12_10"), 0, STRING("party"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSWF(1782), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[69] = {
    (u32)"ノコタロウ", 0x00000000, 0x00000000, (u32)&nokotarou_init,
    (u32)&nokotarou_reg, (u32)&nokotarou_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0xFFFFFFFF, (u32)"ノコリン",
    0x40000620, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
static u32 hitDmgRetPoints[12] = {
    (u32)"mod_00", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_01", 0x00000000, 0xC47A0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(peach_mail)
    LABEL(0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_GREATER(LW(0), 200)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(10))
    IF_INT_GREATER(LW(10), 100)
        ADD(LW(0), 40)
        CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_party_set_dir, 0, 270, 0)
    END_IF
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION1"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("eff0"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD_FLOAT(LW(0), FLOAT(40.0f))
        CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
        IF_FLOAT_LESS(LW(0), LW(3))
            CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
        END_IF
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg1_gon_44"))
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_cam3d_evt_set_rel, 0, 48, 267, 20, 38, 5, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_mario_adjust_dir)
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_MS(600)
    CALL(evt_msg_print_party, STRING("stg1_gon_44_01"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_gon_45"), 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg1_gon_46"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg1_gon_47"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 63)
    CALL(evt_pouch_receive_mail, 0)
    CALL(evt_pouch_open_mail, 0)
    RETURN
EVT_END()

EVT_BEGIN(christine_hei_01)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_stop_for_event)
    CALL(evt_mario_normalize)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg1_hei_02_02"), 0, STRING("party"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    SET_VALUE(GSWF(1805), 1)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_npc_start_for_event)
    RETURN
EVT_END()

EVT_BEGIN(hei_01_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(LW(0), STRING("dokan"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_mobj_badgeblk, STRING("mobj_hatenablk"), 315, 60, -75, 132, 0, GSWF(1785), 0)
    CALL(evt_item_entry, STRING("item_01"), 125, 450, 40, -447, 16, GSWF(5599), 0)
    CALL(evt_item_set_scale, STRING("item_01"), FLOAT(0.6f))
    SET_VALUE(LW(0), PTR(kusa_data))
    CALL_CHILD(evt_sub_kusa_access_entry)
    IF_INT_EQUAL(GSWF(1783), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("a_kusa_1"), 0, PTR(kusa_dokan), 0)
    ELSE
        CALL(evt_hitobj_onoff, STRING("a_kusa_1"), 0, 0)
        CALL(evt_map_playanim, STRING("anm_kusa_1"), 2, 0)
        SET_VALUE(LW(0), STRING("dokan"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    CALL(evt_map_playanim, STRING("anm_mizu"), 1, 0)
    IF_INT_LESS(GSW(0), 25)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hasi"), 1)
        CALL(evt_hitobj_onoff, STRING("a_hasi"), 1, 0)
        SET_VALUE(GW(0), 0)
        CALL(evt_mobj_switch_blue, 2, STRING("bg_switch"), -8, 40, -450, PTR(bridge_evt), 0)
    END_IF
    SPAWN_EVENT(nokotaro_nakama)
    SPAWN_EVENT(evt_nokorin)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    IF_INT_EQUAL(GSW(0), 62)
        SPAWN_EVENT(peach_mail)
    END_IF
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_HEI1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_HEI2"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("USIROKUSA"), 2)
    IF_INT_LESS(GSW(0), 25)
        IF_INT_EQUAL(GSWF(1805), 0)
            SPAWN_EVENT(christine_hei_01)
        END_IF
    END_IF
    RETURN
EVT_END()

static USER_FUNC(set_dir) {
    s32* args;
    s32 dir;
    s32 base;
    PartyEntry* party;

    args = event->args;
    dir = evtGetValue(event, args[0]);
    party = partyGetPtr(0);
    if (party) {
        base = 90;
        if (dir > 180) {
            base = 270;
        }
        party->field_110 = party->field_10C = (f32)(270 - base);
    }
    return EVT_RETURN_DONE;
}


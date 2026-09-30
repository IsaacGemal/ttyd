#include "mri_puni.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
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
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 mri_guide_goryu[];
extern s32 mri_puni_init[];
extern s32 puni_disp_info[];
extern s32 puni_piders_check[];

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BeroEntry bero_bluejail[] = {
    "w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1,
    NULL, "mri_03", "", 1, 1, 0, 0,
};

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "mri_08", "e_bero", 1, 1, 0, 0},
    {"n_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "mri_10", "s_bero", 1, 1, 0, 0},
    {"dokan2", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan3", 1, 1, 0, 0},
    {"dokan3", 2, 0, 6, 100000, 0, 0, -1, NULL, 10, NULL, NULL, "dokan2", 1, 1, 0, 0},
    {NULL},
};

EVT_BEGIN(wall_roll_sub)
    CALL(evt_img_entry, LW(2))
    CALL(evt_img_set_paper, LW(2), LW(0))
    CALL(evt_offscreen_entry, LW(3))
    CALL(evt_mapobj_set_offscreen, 1, LW(1), LW(3))
    CALL(evt_offscreen_get_boundingbox, LW(3), LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_alloc_capture, LW(2), 0, 0, 1, LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_clear_virtual_point, LW(2))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mapobj_get_position, LW(1), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF2_SLOT_GOOUT1"), LW(10), LW(11), LW(12), LW(15))
        WAIT_MS(1800)
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, LW(2), STRING("A_1"))
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
    CALL(evt_img_wait_animend, LW(2))
    CALL(evt_img_free_capture, LW(2), 0)
    CALL(evt_img_free_capture, LW(2), 1)
    CALL(evt_img_onoff, LW(2), 0)
    CALL(evt_img_release, LW(2))
    CALL(evt_mapobj_clear_offscreen, 1, LW(1))
    CALL(evt_offscreen_delete, LW(3))
    RETURN
EVT_END()

EVT_BEGIN(wall_roll)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG2_QUAKE2"), LW(10))
        CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 1000)
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 500)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 159, 399, 0, 86, -18, 500, 11)
    WAIT_MS(1000)
    SET_VALUE(LW(0), STRING("OFF_d_roll_down"))
    SET_VALUE(LW(1), STRING("S_isk_01A01"))
    SET_VALUE(LW(2), STRING("img00"))
    SET_VALUE(LW(3), STRING("ofs00"))
    SPAWN_EVENT(wall_roll_sub)
    WAIT_MS(2000)
    SET_VALUE(LW(0), STRING("OFF_d_roll_left"))
    SET_VALUE(LW(1), STRING("S_isk_01A02"))
    SET_VALUE(LW(2), STRING("img01"))
    SET_VALUE(LW(3), STRING("ofs01"))
    SPAWN_EVENT(wall_roll_sub)
    WAIT_MS(2000)
    SET_VALUE(LW(0), STRING("OFF_d_roll_up"))
    SET_VALUE(LW(1), STRING("S_isk_01A03"))
    SET_VALUE(LW(2), STRING("img02"))
    SET_VALUE(LW(3), STRING("ofs02"))
    SPAWN_EVENT(wall_roll_sub)
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("OFF_d_roll_right"))
    SET_VALUE(LW(1), STRING("S_isk_01A04"))
    SET_VALUE(LW(2), STRING("img03"))
    SET_VALUE(LW(3), STRING("ofs03"))
    CALL_CHILD(wall_roll_sub)
    CALL(evt_hitobj_onoff, STRING("A_isk_01A"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_f2_rak03"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_f2_rak04"), 1, 0)
    SET_VALUE(GSWF(2850), 1)
    WAIT_FRAMES(1)
    CALL(evt_puni_all_fall_check)
    CALL(evt_puni_set_maze)
    CALL(evt_cam3d_evt_off, 300, 11)
    WAIT_MS(500)
    CALL(evt_mobj_get_position, STRING("slot00"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("small_star"), 3, LW(0), LW(1), LW(2), 0, -20, 0, 3, 0, 0, 0, 0)
    CALL(evt_mobj_get_position, STRING("slot01"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("small_star"), 3, LW(0), LW(1), LW(2), 0, -20, 0, 3, 0, 0, 0, 0)
    CALL(evt_mobj_get_position, STRING("slot02"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("small_star"), 3, LW(0), LW(1), LW(2), 0, -20, 0, 3, 0, 0, 0, 0)
    CALL(evt_mobj_get_position, STRING("slot03"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("small_star"), 3, LW(0), LW(1), LW(2), 0, -20, 0, 3, 0, 0, 0, 0)
    CALL(evt_mobj_delete, STRING("slot00"))
    CALL(evt_mobj_delete, STRING("slot01"))
    CALL(evt_mobj_delete, STRING("slot02"))
    CALL(evt_mobj_delete, STRING("slot03"))
    WAIT_MS(500)
    SET_VALUE(GSW(0), 104)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(breakyuka_init)
    IF_INT_EQUAL(GSWF(2851), 0)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak_02"), 1, 0)
    ELSE
        CALL(evt_hitobj_onoff, STRING("A_f2_rak_02"), 1, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(breakyuka_main)
    CALL(evt_hitobj_onoff, STRING("A_f2_rak_02"), 1, 1)
    CALL(evt_puni_all_fall_check)
    RETURN
EVT_END()

static KusaAccessEntry ueki_access_data[] = {
    {1, "A_ueki_01", "S_ueki_01", NULL},
    {1, "A_ueki_02", "S_ueki_02", NULL},
    {1, "A_ueki_03", "S_ueki_03", NULL},
    {1, "A_ueki_04", "S_ueki_04", NULL},
    {1, "A_ueki_05", "S_ueki_05", NULL},
    {4},
};

EVT_BEGIN(slot00_event)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 104)
        RETURN
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    SET_VALUE(LW(10), GW(0))
    SET_VALUE(LW(11), GW(0))
    ADD(LW(11), 90)
    CALL(evt_sub_intpl_msec_init, 0, LW(10), LW(11), 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(0), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_GREATER_OR_EQUAL(GW(0), 360)
        SUBTRACT(GW(0), 360)
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

EVT_BEGIN(slot01_event)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 104)
        RETURN
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    SET_VALUE(LW(10), GW(1))
    SET_VALUE(LW(11), GW(1))
    ADD(LW(11), 90)
    CALL(evt_sub_intpl_msec_init, 0, LW(10), LW(11), 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(1), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_GREATER_OR_EQUAL(GW(1), 360)
        SUBTRACT(GW(1), 360)
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

EVT_BEGIN(slot02_event)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 104)
        RETURN
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    SET_VALUE(LW(10), GW(2))
    SET_VALUE(LW(11), GW(2))
    ADD(LW(11), 90)
    CALL(evt_sub_intpl_msec_init, 0, LW(10), LW(11), 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(2), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_GREATER_OR_EQUAL(GW(2), 360)
        SUBTRACT(GW(2), 360)
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

EVT_BEGIN(slot03_event)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 104)
        RETURN
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 0)
    SET_VALUE(LW(10), GW(3))
    SET_VALUE(LW(11), GW(3))
    ADD(LW(11), 90)
    CALL(evt_sub_intpl_msec_init, 0, LW(10), LW(11), 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(3), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_GREATER_OR_EQUAL(GW(3), 360)
        SUBTRACT(GW(3), 360)
    END_IF
    CALL(evt_mobj_hitevt_onoff, STRING("me"), 1)
    RETURN
EVT_END()

EVT_BEGIN(slot_event)
    SET_VALUE(GW(0), 0)
    SET_VALUE(GW(1), 0)
    SET_VALUE(GW(2), 0)
    SET_VALUE(GW(3), 0)
    LOOP(0)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_01A01a"), GW(0), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_01A02a"), GW(1), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_01A03a"), GW(2), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_isk_01A04a"), GW(3), 0, 0)
        IF_INT_EQUAL(GW(0), 270)
            IF_INT_EQUAL(GW(1), 180)
                IF_INT_EQUAL(GW(2), 0)
                    IF_INT_EQUAL(GW(3), 90)
                        BREAK_LOOP
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SPAWN_EVENT(wall_roll)
    RETURN
EVT_END()

EVT_BEGIN(get_blue_jail_key)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 17, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL_CHILD(mri_guide_goryu)
    CALL(evt_puni_flag_onoff, 1, 0, 262144)
    CALL(evt_puni_set_mode, 1, 30)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイドsp"), 1536)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("ガイドsp"), LW(10), LW(11), LW(12))
        IF_INT_GREATER_OR_EQUAL(LW(10), LW(0))
            CALL(evt_mario_set_dir, 90, 200, 0)
        ELSE
            CALL(evt_mario_set_dir, 270, 200, 0)
        END_IF
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("ガイドsp"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    IF_INT_GREATER_OR_EQUAL(LW(10), LW(0))
        CALL(evt_npc_set_ry, STRING("ガイドsp"), 90)
    ELSE
        CALL(evt_npc_set_ry, STRING("ガイドsp"), 270)
    END_IF
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00"), 0, STRING("ガイドsp"))
    CALL(evt_puni_flag_onoff, 1, 1, 262144)
    CALL(evt_puni_set_mode, 1, 1)
    SET_VALUE(GSW(0), 98)
    CALL(evt_fade_set_mapchange_type, 0, 2, 500, 1, 500)
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_bero_overwrite, STRING("w_bero"), PTR(bero_bluejail))
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(mri_09_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 9, 100, 0, -310, -1000, 2, 2)
    CALL(evt_puni_set_hamari, -250, 100, 78, 102, -2, -230, -311)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    CALL_CHILD(mri_puni_init)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT(puni_disp_info)
    SPAWN_EVENT(puni_piders_check)
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
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_map_playanim, STRING("mizu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu_02"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_02"), 1, 0)
    SWITCH(GSW(0))
        CASE_LESS(103)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak03"), 1, 0)
        CASE_EQUAL(103)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak03"), 1, 0)
        SPAWN_EVENT(slot_event)
        CALL(evt_mobj_switch_red, 1, STRING("slot00"), -80, 0, 135, PTR(slot00_event), 0)
        CALL(evt_mobj_switch_red, 1, STRING("slot01"), -30, 0, 135, PTR(slot01_event), 0)
        CALL(evt_mobj_switch_red, 1, STRING("slot02"), 30, 0, 135, PTR(slot02_event), 0)
        CALL(evt_mobj_switch_red, 1, STRING("slot03"), 80, 0, 135, PTR(slot03_event), 0)
        CALL(evt_paper_entry, STRING("OFF_d_roll_down"))
        CALL(evt_paper_entry, STRING("OFF_d_roll_up"))
        CALL(evt_paper_entry, STRING("OFF_d_roll_left"))
        CALL(evt_paper_entry, STRING("OFF_d_roll_right"))
        CASE_GREATER_OR_EQUAL(104)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_isk_01A"), 1)
        CALL(evt_hitobj_onoff, STRING("A_isk_01A"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak03"), 1, 1)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak04"), 1, 0)
    END_SWITCH
    CALL(evt_mobj_itembox, STRING("mobj_box00"), 125, 0, 25, 0, 0, PTR(get_blue_jail_key), GSWF(2852))
    CALL(evt_item_entry, STRING("item100"), 256, -250, -234, 75, 16, GSWF(2853), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk1"), 0, -174, -125, 0, GSWF(5543), 0)
    SPAWN_EVENT(breakyuka_init)
    CALL(evt_mobj_breaking_floor, STRING("mobj_bf00"), 0, 0, 75, 1, PTR(breakyuka_main), GSWF(2851))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

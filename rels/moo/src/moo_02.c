#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

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

EVT_BEGIN(n_bero_in)
    CALL(evt_mario_bgmode_on)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_cam_y_off)
    CALL(evt_hit_get_position, STRING("n_bero"), LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL_CHILD(bero_case_entry)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(n_bero_out)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_mapchange, STRING("aji_19"), STRING("w_bero"))
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[90] = {
    (u32)"w_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"moo_05", (u32)"e_bero",
    0x00010001, 0x00000000, 0x00000000, (u32)"dokan_1",
    0x00020000, 0x00000009, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000006,
    0x00000000, 0x00000000, (u32)"dokan_2", 0x00010001,
    0x00000000, 0x00000000, (u32)"dokan_2", 0x10020000,
    0x00000009, 0x000186A0, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x00000006, 0x00000000,
    0x00000000, (u32)"dokan_1", 0x00010001, 0x00000000,
    0x00000000, (u32)"n_bero", 0x10030000, 0x00004E20,
    0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
    (u32)&n_bero_in, 0x00000005, (u32)&n_bero_out, (u32)"aji_00",
    (u32)"w_bero", 0x00010001, 0x00000000, 0x00000000,
    (u32)"e_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"moo_06", (u32)"w_bero",
    0x00010001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static EVT_BEGIN(dokan_evt)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, STRING("S_iwa_04"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 0, STRING("S_iwa_04"), STRING("offscreen"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("offscreen"), 0, 0, 0, 0, 0)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_shadow, STRING("img"), 2)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_iwa_04"), 1)
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_free_capture, STRING("img"), 0)
    CALL(evt_img_set_shadow, STRING("img"), 0)
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 0, STRING("S_iwa_04"))
    SET_VALUE(GSW(0), 359)
    CALL(evt_hitobj_onoff, STRING("A_iwa_04"), 1, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(rock_main)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 0, LW(10), STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 0, 1, LW(10), 1)
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("offscreen"), 0, 0, 0, 0, 0)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_shadow, STRING("img"), 2)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_release, STRING("img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 0, LW(10))
    CALL(evt_hitobj_onoff, LW(11), 1, 0)
    RETURN
EVT_END()

static EVT_BEGIN(rock_evt0)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(10), STRING("S_iwa_01"))
    SET_VALUE(LW(11), STRING("A_iwa_01"))
    CALL_CHILD(rock_main)
    SET_VALUE(GSWF(4029), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(rock_evt1)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(10), STRING("S_iwa_07"))
    SET_VALUE(LW(11), STRING("A_iwa_07"))
    CALL_CHILD(rock_main)
    SET_VALUE(GSWF(4030), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(moo_02_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_MON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_MOO2"))
    CALL(evt_map_playanim, STRING("hosi_01"), 1, 0)
    CALL(evt_map_playanim, STRING("hosi_02"), 1, 0)
    CALL(evt_map_playanim, STRING("hosi_03"), 1, 0)
    CALL(evt_map_playanim, STRING("light"), 1, 0)
    CALL(evt_item_entry, STRING("item00"), 125, -55, 0, -135, 16, GSWF(5664), 0)
    CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
    IF_INT_LESS(GSW(0), 359)
        CALL(evt_run_case_evt, 15, 1, STRING("A_iwa_04"), 0, PTR(dokan_evt), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_iwa_04"), 1)
        CALL(evt_hitobj_onoff, STRING("A_iwa_04"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(4029), 0)
        CALL(evt_run_case_evt, 15, 1, STRING("A_iwa_01"), 0, PTR(rock_evt0), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_iwa_01"), 1)
        CALL(evt_hitobj_onoff, STRING("A_iwa_01"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSWF(4030), 0)
        CALL(evt_run_case_evt, 15, 1, STRING("A_iwa_07"), 0, PTR(rock_evt1), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_iwa_07"), 1)
        CALL(evt_hitobj_onoff, STRING("A_iwa_07"), 1, 0)
    END_IF
    CALL(evt_item_entry, STRING("item_01"), 142, 330, 0, 403, 16, GSWF(4037), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

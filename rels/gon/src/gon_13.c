#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_paper.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(96,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(97,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gon_07", "e_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(item_get03)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box04"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 108, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box04"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(switch_evt)
    CALL(evt_mario_wait_landon)
    CALL(evt_paper_entry, STRING("OFF_d_biri"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_biri"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 604, 480)
    WAIT_FRAMES(1)
    CALL(evt_img_onoff, STRING("img"), 1)
    CALL(evt_mobj_flag_onoff, 0, STRING("mobj_box04"), 16384)
    CALL(evt_mobj_hit_onoff, STRING("mobj_box04"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_OFF1_PERA_BREAK1"), 0)
        WAIT_MS(1350)
        CALL(evt_snd_sfxon, STRING("SFX_OFF1_PERA_FALL1"), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_biri"))
    RETURN
EVT_END()

EVT_BEGIN(gon_13_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_snd_bgmon, 160, 0)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_floatswitch_blue, 0, STRING("mobj_switch00"), 50, 60, 40, PTR(switch_evt), GSWF(1499))
    CALL(evt_mobj_itembox, STRING("mobj_box04"), 0, 0, -50, 0, 0, PTR(item_get03), GSWF(1510))
    IF_INT_EQUAL(GSWF(1499), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("mobj_box04"), 16384)
        CALL(evt_mobj_hit_onoff, STRING("mobj_box04"), 0)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

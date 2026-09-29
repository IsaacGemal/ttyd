#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_lecture.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 all_juyoitem_lecture_rel_7_data_150F8[];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(n_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[] = {
    {"n_bero", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_04", "s_bero", 5, 5, (s32)&n_close, (s32)&n_open},
    {"w_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_06", "e_bero_2", 1, 1, 0, 0},
    {"w_bero_2", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_06", "e_bero_1", 5, 5, (s32)&w_close, (s32)&w_open},
    {NULL},
};

EVT_BEGIN(grass_open_1)
    IF_INT_EQUAL(GF(0), 1)
        CALL(evt_map_playanim, STRING("anm_nata"), 0, 0)
        SET_VALUE(GF(0), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(grass_open_2)
    IF_INT_EQUAL(GF(0), 0)
        CALL(evt_map_playanim, STRING("anm_tana"), 0, 0)
        SET_VALUE(GF(0), 1)
    ELSE
        CALL(evt_map_playanim, STRING("anm_nata"), 0, 0)
        SET_VALUE(GF(0), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_boots)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("tbox"), LW(0), LW(1), LW(2))
    CALL(lect_juyoitem_rel_set, PTR(all_juyoitem_lecture_rel_7_data_150F8))
    CALL(evt_item_entry, STRING("boots"), 8, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("tbox"))
    CALL(evt_item_get_important_item, STRING("boots"))
    CALL(evt_item_get_item, STRING("boots"))
    CALL(evt_item_get_item_end_wait, STRING("boots"))
    SET_VALUE(GSW(0), 319)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_elvkey)
    SET_VALUE(GSW(0), 320)
    RETURN
EVT_END()

static EVT_BEGIN(evt_head)
    CALL(evt_item_delete, STRING("elvkey"))
    CALL(evt_item_entry, STRING("elvkey"), 26, -135, 166, -70, 4, -1, PTR(evt_elvkey))
    CALL(evt_item_set_move_dir_speed, STRING("elvkey"), FLOAT(135.0f), FLOAT(90.0f))
    CALL(evt_item_set_gravity, STRING("elvkey"), FLOAT(1.0f))
    CALL(evt_item_set_bound_rate, STRING("elvkey"), FLOAT(0.6f))
    CALL(evt_item_set_jump_power, STRING("elvkey"), FLOAT(500.0f))
    CALL(evt_item_status_on, STRING("elvkey"), 16)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(eki_05_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI2"))
    SET_VALUE(GF(0), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("a_garasu_1"), 0, PTR(grass_open_1), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("a_garasu_2"), 0, PTR(grass_open_2), 0)
    IF_INT_LESS(GSW(0), 319)
        CALL(evt_mobj_itembox, STRING("tbox"), -85, 295, 0, 1, 0, PTR(evt_boots), GSWF(3728))
    END_IF
    IF_INT_LESS(GSW(0), 320)
        CALL(evt_item_entry, STRING("elvkey"), 26, -135, 136, -70, 17, -1, PTR(evt_elvkey))
        CALL(evt_run_case_evt, 16, 1, STRING("a_t_2"), 0, PTR(evt_head), 0)
    END_IF
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_map_playanim, STRING("anm_mizu"), 0, 0)
            CALL(evt_map_checkanim, STRING("anm_mizu"), LW(0), LW(1))
            WAIT_MS(700)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_AMB_WATERDROP1"), -129, 295, -17, 0)
            WAIT_MS(100)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_AMB_WATERDROP1"), 179, 295, -17, 0)
            WAIT_MS(100)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

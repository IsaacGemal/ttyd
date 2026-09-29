#include "driver/hitdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 hikari_evt[];

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

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_21", "e_bero_1", 5, 5, &w_door_close, &w_door_open},
    {"e_bero_1", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_25", "w_bero", 5, 5, &e1_door_close, &e1_door_open},
    {"e_bero_2", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_25", "w_bero", 5, 5, &e2_door_close, &e2_door_open},
    {"n_bero_1", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_22", "s_bero", 1, 1},
    {"n_bero_2", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_24", "s_bero_1", 1, 1},
    {NULL},
};

static BeroEntry w_bero_mapchg[] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_21", "e_bero_2", 5, 5, &w_door_close, &w_door_open},
};

static HitDamageReturn dmgRetPoint[] = {
    {"A_oti_01", {0.0f, -1000.0f, 0.0f}},
    {"A_oti_02", {0.0f, -1000.0f, 0.0f}},
    {"A_oti_03", {0.0f, -1000.0f, 0.0f}},
    {"A_oti_04", {0.0f, -1000.0f, 0.0f}},
    {"A_oti_05", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

EVT_BEGIN(las_23_init_evt)
    IF_INT_LESS(GSW(0), 390)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST3"))
    END_IF
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_LESS(GSW(0), 390)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_under"), 1)
        CALL(evt_hitobj_onoff, STRING("A_under"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_paipu"), 1)
        CALL(evt_hitobj_onoff, STRING("jav_01"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("jav_02"), 1, 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka"), 1)
        CALL(evt_hitobj_onoff, STRING("A_be"), 1, 0)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("w_bero"), PTR(w_bero_mapchg))
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_on)
        SPAWN_EVENT(hikari_evt)
    END_IF
    CALL(evt_hit_damage_return_set, PTR(dmgRetPoint))
    CALL_CHILD(evt_gazigazi_entry)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

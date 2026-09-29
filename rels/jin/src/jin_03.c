#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];

// clang-format off
extend_data(16,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(17,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(n_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door01"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w1_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00A"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w1_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00A"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00B"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00B"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(w3_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00C"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(w3_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00C"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"n_bero", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_01", "s_bero_2", 5, 5, &n_close, &n_open},
    {"s_bero", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_02", "n_bero_2", 5, 5, &s_close, &s_open},
    {"w_bero3", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_00", "e_bero_3", 5, 5, &w1_close, &w1_open},
    {"w_bero1", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_00", "e_bero_1", 5, 5, &w3_close, &w3_open},
    {"w_bero2", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_00", "e_bero_2", 5, 5, &w2_close, &w2_open},
    {NULL},
};

EVT_BEGIN(jin_03_kaidan_temae_kesu)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kaidan01"), 1)
    CALL(evt_hitobj_onoff, STRING("a_kaidan01"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(jin_03_kaidan_naka_kesu)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kaidan02"), 1)
    CALL(evt_hitobj_onoff, STRING("a_kaidan02"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(jin_03_kaidan_oku_kesu)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kaidan03"), 1)
    CALL(evt_hitobj_onoff, STRING("a_kaidan03"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(jin_03_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    IF_INT_EQUAL(LSW(1), 0)
        SPAWN_EVENT(jin_03_kaidan_temae_kesu)
        SPAWN_EVENT(jin_03_kaidan_oku_kesu)
    ELSE
        IF_INT_EQUAL(LSW(1), 1)
            SPAWN_EVENT(jin_03_kaidan_naka_kesu)
            SPAWN_EVENT(jin_03_kaidan_oku_kesu)
        ELSE
            SPAWN_EVENT(jin_03_kaidan_temae_kesu)
            SPAWN_EVENT(jin_03_kaidan_naka_kesu)
        END_IF
    END_IF
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 125, 0, 0, STRING("S_yuka"), 125, GSWF(5634))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

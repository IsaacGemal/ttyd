#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(jin_11_ana)
    SET_VALUE(GSWF(2079), 1)
    CALL(evt_bero_mapchange, STRING("jin_07"), 0)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static EVT_BEGIN(s_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door10"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door10"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"s_bero", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_10", "n_bero", 5, 5,
     s_close, s_open},
    {"ana", 0, 0, 4, 0x34D, 0x73, 0x4D, -1, NULL, 0, jin_11_ana, "jin_07", NULL, 4, 4, 0, 0},
    {NULL},
};

EVT_BEGIN(evt_yuka)
    SET_VALUE(GSW(0), 208)
    RETURN
EVT_END()

EVT_BEGIN(jin_11_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -50, 60, -40, 0, 0)
    CALL(evt_mobj_powerupblk, STRING("pblk00"), 40, 60, -40, 0, GSWF(5551), 0)
    IF_INT_LESS(GSW(0), 208)
        CALL(evt_mobj_breaking_floor, STRING("mobj_bf00"), 845, 115, 5, 0, PTR(evt_yuka), GSWF(2233))
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];

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

static EVT_BEGIN(check_roll)
    CALL(evt_mario_get_motion, LW(0))
    IF_INT_EQUAL(LW(0), 22)
        SET_VALUE(LW(0), 0)
    ELSE
        SET_VALUE(LW(0), 1)
    END_IF
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"s_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, check_roll, "jin_00", "n_bero_1", 1, 1},
    {NULL},
};

EVT_BEGIN(item_box_00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("_item00"), 60, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("_item00"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("_item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box01)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box01"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("_item01"), 253, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("_item01"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box01"))
    CALL(evt_item_get_item, STRING("_item01"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(jin_05_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_mobj_powerupblk, STRING("pblk00"), 65, 60, 35, 0, GSWF(5549), 0)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), 125, 0, 75, 0, 0, PTR(item_box_00), GSWF(2235))
    CALL(evt_mobj_itembox, STRING("mobj_box01"), -55, 0, 55, 0, 0, PTR(mobj_box01), GSWF(2237))
    CALL(evt_item_entry, STRING("item_00"), 125, -10, 0, -40, 16, GSWF(5635), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

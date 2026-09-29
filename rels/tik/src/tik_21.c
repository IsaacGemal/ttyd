#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

// clang-format off
extend_data(84,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(85,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_entry_data[30] = {
    (u32)"s_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"tik_15", (u32)"n_bero",
    0x00010001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(hari_trap)
    CALL(evt_hit_bind_mapobj, STRING("a_toge"), STRING("s_toge"))
    LOOP(0)
        CALL(evt_hitobj_attr_onoff, 1, 1, STRING("a_yuka"), 2048)
        CALL(evt_mapobj_get_position, STRING("s_toge"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_FLOOR_TRAP1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 1, -5, 30, 150)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("s_toge"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("a_toge"))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(1500)
        CALL(evt_mapobj_get_position, STRING("s_toge"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_FLOOR_TRAP2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 4, 30, -5, 300)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("s_toge"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("a_toge"))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("a_yuka"), 2048)
        WAIT_MS(2200)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 hitDmgRetPoints[12] = {
    (u32)"mod_1", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"mod_2", 0x43020000, 0x00000000, 0xC2800000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 284, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tik_21_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    SPAWN_EVENT(hari_trap)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), 150, 0, -100, 0, 0, PTR(mobj_box00), GSWF(1368))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

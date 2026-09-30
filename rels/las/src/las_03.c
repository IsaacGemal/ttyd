#include "las_03.h"
#include "driver/hitdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on

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

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_02", "e_bero", 5, 5, w_door_close, w_door_open},
    {"e_bero_1", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_04", "w_bero", 5, 5, e_door_close, e_door_open},
    {NULL},
};

static HitDamageReturn dmgRetPoint[3] = {
    {"A_yuka_02", {0.0f, -1000.0f, 0.0f}},
    {"A_yuka_03", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

char* toge_data[18] = {
    "S_toge_01", "A_toge_01", "S_toge_02", "A_toge_02", "S_toge_03", "A_toge_03", "S_toge_04",
    "A_toge_04", "S_toge_05", "A_toge_05", "S_toge_06", "A_toge_06", "S_toge_07", "A_toge_07",
    "S_toge_08", "A_toge_08", "S_toge_09", "A_toge_09",
};

u16 toge_flag;

USER_FUNC(get_toge_name) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], (s32)toge_data[index * 2]);
    evtSetValue(event, args[2], (s32)toge_data[index * 2 + 1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(toge_check_flag) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], toge_flag & (1 << index));
    return EVT_RETURN_DONE;
}

USER_FUNC(toge_flag_onoff) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 onoff = evtGetValue(event, args[1]);

    if (onoff != 0) {
        toge_flag |= (1 << index);
    } else {
        toge_flag &= ~(1 << index);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(toge_evt_sub)
    SET_VALUE(LW(2), LW(0))
    CALL(get_toge_name, LW(2), LW(3), LW(4))
    CALL(toge_check_flag, LW(2), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(toge_flag_onoff, LW(2), 1)
    CALL(evt_mapobj_get_position, LW(3), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_FLOOR_TRAP1"), LW(7), LW(8), LW(9), 0)
    CALL(evt_sub_intpl_msec_init, 0, 0, 30, 150)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 0, LW(3), 0, LW(0), 0)
    CALL(evt_hit_bind_update, LW(4))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, LW(3), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_FLOOR_TRAP2"), LW(7), LW(8), LW(9), 0)
    CALL(evt_sub_intpl_msec_init, 0, 30, 0, 400)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 0, LW(3), 0, LW(0), 0)
    CALL(evt_hit_bind_update, LW(4))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_mapobj_trans, 0, LW(3), 0, 0, 0)
    CALL(evt_hit_bind_update, LW(4))
    CALL(toge_flag_onoff, LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(toge_evt)
    CALL(evt_hit_damage_return_set, PTR(dmgRetPoint))
    CALL_CHILD(evt_gazigazi_entry)
    SET_INT_READ_ADDR(PTR(toge_data))
    LOOP(9)
        READ_INT2(LW(0), LW(1))
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_LOOP
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    LOOP(0)
        CALL(evt_mario_get_motion, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 32)
            CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
            SET_VALUE(LW(4), 0)
            LOOP(0)
                CALL(get_toge_name, LW(4), LW(5), LW(6))
                CALL(evt_mapobj_get_position, LW(5), LW(6), LW(7), LW(8))
                CALL(evt_sub_area_check, 1, LW(6), LW(8), 90, LW(1), LW(3), LW(9))
                IF_INT_EQUAL(LW(9), 1)
                    SET_VALUE(LW(0), LW(4))
                    SPAWN_EVENT(toge_evt_sub)
                END_IF
                ADD(LW(4), 1)
                IF_INT_GREATER_OR_EQUAL(LW(4), 9)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(las_03_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_badgeblk, STRING("mobj_badge_gat"), -350, 0, -100, 264, 0, GSWF(4375), 1)
    SPAWN_EVENT(toge_evt)
    RETURN
EVT_END()

// clang-format on

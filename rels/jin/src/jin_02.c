#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];

// clang-format off
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(n2_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n2_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n1_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(n1_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door00"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"n_bero_2", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_03", "s_bero", 5, 5,
     n2_close, n2_open},
    {"n_bero_1", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_00", "s_bero", 5, 5,
     n1_close, n1_open},
    {NULL},
};

static EVT_BEGIN(cam_shake)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_KAIDAN_MOVE1"), 300, 100, -200, LW(9))
    CALL(evt_sub_intpl_init, 11, 0, 10, 30)
    LABEL(0)
    CALL(evt_sub_intpl_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    CALL(evt_cam_shake, 4, LW(0), FLOAT(0.0f), 100)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_sub_intpl_init, 11, 10, 0, 30)
    LABEL(1)
    CALL(evt_sub_intpl_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    CALL(evt_cam_shake, 4, LW(0), FLOAT(0.0f), 100)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    RETURN
EVT_END()

EVT_BEGIN(evt_kaidan_temae)
    IF_INT_NOT_EQUAL(LSW(1), 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_normalize)
        CALL(evt_party_stop, 0)
        IF_INT_EQUAL(GSWF(2234), 0)
            BEGIN_INLINE_EVENT_GET_ID(LW(3))
                WAIT_MS(500)
                CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
                WAIT_MS(1000)
                LOOP(0)
                    CALL(evt_sub_random, 200, LW(0))
                    ADD(LW(0), 300)
                    CALL(evt_mario_set_dir, 90, LW(0), 0)
                    WAIT_MS(LW(0))
                    CALL(evt_sub_random, 300, LW(0))
                    ADD(LW(0), 400)
                    CALL(evt_mario_set_dir, 270, LW(0), 0)
                    WAIT_MS(LW(0))
                END_LOOP
            END_INLINE_EVENT
            BEGIN_INLINE_EVENT_GET_ID(LW(4))
                WAIT_MS(500)
                CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
                WAIT_MS(1000)
                LOOP(0)
                    CALL(evt_sub_random, 200, LW(0))
                    ADD(LW(0), 400)
                    CALL(evt_party_set_dir, 0, 90, LW(0))
                    WAIT_MS(LW(0))
                    CALL(evt_sub_random, 200, LW(0))
                    ADD(LW(0), 200)
                    CALL(evt_party_set_dir, 0, 270, LW(0))
                    WAIT_MS(LW(0))
                END_LOOP
            END_INLINE_EVENT
        END_IF
        CALL_CHILD(cam_shake)
        IF_INT_EQUAL(GSWF(2234), 0)
            WAIT_MS(500)
            DELETE_EVENT(LW(3))
            DELETE_EVENT(LW(4))
            SET_VALUE(GSWF(2234), 1)
        END_IF
        ADD(LSW(1), 1)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_party_run, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(jin_02_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN2"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_mobj_switch_red, 0, STRING("mobj_switch1"), 485, 30, -10, PTR(evt_kaidan_temae), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

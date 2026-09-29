#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[] = {
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_10", "w_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(jin_09_jumpstand)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_jump_jumpstand, 0, 0, 0, -10, 700, 0, 50, 0, 0, 1800, 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(jin_09_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_mobj_powerupblk, STRING("blk00"), -120, 60, 90, 0, GSWF(5550), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_random, 3000, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
            CALL(evt_map_playanim, STRING("sizuku_01"), 0, 0)
            CALL(evt_map_checkanim, STRING("sizuku_01"), LW(0), LW(1))
            WAIT_MS(LW(1))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_AMB_WATERDROP1"), -76, 0, 13, 0)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_random, 3000, LW(0))
            ADD(LW(0), 1000)
            WAIT_MS(LW(0))
            CALL(evt_map_playanim, STRING("sizuku_02"), 0, 0)
            CALL(evt_map_checkanim, STRING("sizuku_02"), LW(0), LW(1))
            WAIT_MS(LW(1))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_AMB_WATERDROP1"), 32, 0, -64, 0)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mobj_jumpstand_red, 0, STRING("jumpstand"), 0, 0, 0, PTR(jin_09_jumpstand), 0)
    IF_INT_NOT_EQUAL(GSWF(2079), 0)
        SET_VALUE(GSWF(2079), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_mario_cont_onoff, 0)
            CALL(evt_party_cont_onoff, 0, 0)
            CALL(evt_mario_set_pos, 0, 365, 0)
            CALL(evt_mario_get_party, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(evt_party_set_pos, 0, 0, 395, 0)
            END_IF
            CALL(evt_cam_road_reset)
            CALL(evt_seq_wait, 2)
            CALL(evt_mario_cont_onoff, 1)
            WAIT_MS(180)
            CALL(evt_party_cont_onoff, 0, 1)
            CALL(evt_mario_key_onoff, 1)
        END_INLINE_EVENT
        SET_VALUE(LW(0), 1)
    ELSE
        SET_VALUE(LW(0), 0)
    END_IF
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_seq_wait, 2)
            LOOP(0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                IF_INT_LESS(LW(1), 365)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_IF
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(1), 365)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_bero_mapchange, STRING("gra_06"), STRING("ana"))
    END_INLINE_EVENT
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

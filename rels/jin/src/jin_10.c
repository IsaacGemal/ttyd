#include "jin_10.h"
#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "party.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(44,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(45,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door11"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door11"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {
        "w_bero",
        0x0000, 0x0000, 0x00004E20,
        0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
        NULL, 0xFFFFFFFF, NULL,
        "jin_09", "e_bero",
        0x0001, 0x0001, 0x00000000, 0x00000000,
    },
    {
        "n_bero",
        0x0001, 0x0002, 0x00004E20,
        0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
        NULL, 0xFFFFFFFF, NULL,
        "jin_11", "s_bero",
        0x0005, 0x0005, door_close, door_open,
    },
    {
        NULL,
        0x0000, 0x0000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        NULL, 0x00000000, NULL,
        NULL, NULL,
        0x0000, 0x0000, 0x00000000, 0x00000000,
    },
};

static USER_FUNC(get_pos)
{
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    evtSetFloat(event, args[0], mario->position.x);
    evtSetFloat(event, args[1], mario->position.y);
    evtSetFloat(event, args[2], mario->position.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(set_pos)
{
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    MarioWork* mario = marioGetPtr();
    Vec pos = { 0.0f, 0.0f, 0.0f };
    pos.x = x;
    pos.y = y;
    pos.z = z;
    mario->position = pos;
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(push_evt)
    IF_INT_EQUAL(GF(0), 0)
        CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_GRID_MOVE1"), LW(0), LW(1), LW(2), GW(3))
        SET_VALUE(GF(0), 1)
    ELSE
        CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfx_pos, GW(3), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_mario_push)
    CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(3), LW(0))
        SET_FLOAT_VALUE(LW(10), FLOAT(0.5f))
    ELSE
        SET_FLOAT_VALUE(LW(10), FLOAT(-1.0f))
    END_IF
    ADD_FLOAT(GW(10), LW(10))
    IF_FLOAT_GREATER_OR_EQUAL(GW(10), FLOAT(265.0f))
        SET_FLOAT_VALUE(GW(10), FLOAT(265.0f))
        SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
        IF_INT_EQUAL(GF(0), 1)
            CALL(evt_snd_sfxoff, GW(3))
            SET_VALUE(GF(0), 0)
        END_IF
    END_IF
    IF_FLOAT_LESS_OR_EQUAL(GW(10), FLOAT(0.0f))
        SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
        IF_INT_EQUAL(GF(0), 1)
            CALL(evt_snd_sfxoff, GW(3))
            SET_VALUE(GF(0), 0)
        END_IF
    END_IF
    SET_FLOAT_VALUE(GW(11), GW(10))
    DIVIDE_FLOAT(GW(11), FLOAT(4.0f))
    CALL(evt_mapobj_trans, 1, STRING("saku"), GW(10), GW(11), 0)
    CALL(evt_hit_bind_update, STRING("a_saku"))
    CALL(get_pos, LW(0), LW(1), LW(2))
    ADD_FLOAT(LW(0), LW(10))
    CALL(set_pos, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

USER_FUNC(get_width)
{
    s32* args = event->args;
    PartyEntry* party = partyGetPtr(evtGetValue(event, args[0]));
    evtSetFloat(event, args[1], 0.8f * *(f32*)((u8*)party + 0xF4));
    return EVT_RETURN_DONE;
}

EVT_BEGIN(unpush_evt)
    LOOP(0)
        LOOP(0)
            CALL(evt_mario_get_name_hitobj_push, LW(0))
            IF_STR_NOT_EQUAL(LW(0), STRING("a_saku"))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LF(0), 1)
        CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        IF_INT_LESS(LW(3), LW(0))
            LOOP(0)
                CALL(evt_party_get_status, 6, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
                CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
                SUBTRACT_FLOAT(LW(0), LW(3))
                IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
                END_IF
                IF_FLOAT_LESS(LW(0), FLOAT(20.0f))
                    SET_VALUE(LF(0), 0)
                    BREAK_LOOP
                END_IF
                CALL(evt_mario_get_party, LW(0))
                IF_INT_NOT_EQUAL(LW(0), 0)
                    CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
                    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
                    CALL(get_width, 0, LW(6))
                    SUBTRACT_FLOAT(LW(0), LW(3))
                    IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                        MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
                    END_IF
                    IF_FLOAT_LESS(LW(0), LW(6))
                        SET_VALUE(LF(0), 0)
                        BREAK_LOOP
                    END_IF
                END_IF
                CALL(evt_mario_get_exparty, LW(0))
                IF_INT_NOT_EQUAL(LW(0), 0)
                    CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
                    CALL(evt_party_get_pos, 1, LW(3), LW(4), LW(5))
                    CALL(get_width, 1, LW(6))
                    SUBTRACT_FLOAT(LW(0), LW(3))
                    IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                        MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
                    END_IF
                    IF_FLOAT_LESS(LW(0), LW(6))
                        SET_VALUE(LF(0), 0)
                        BREAK_LOOP
                    END_IF
                END_IF
                BREAK_LOOP
            END_LOOP
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            IF_INT_EQUAL(GF(0), 1)
                CALL(evt_snd_sfxoff, GW(3))
                SET_VALUE(GF(0), 0)
            END_IF
        ELSE
            SUBTRACT_FLOAT(GW(10), FLOAT(0.5f))
            IF_FLOAT_LESS(GW(10), FLOAT(0.0f))
                SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
                IF_INT_EQUAL(GF(0), 1)
                    CALL(evt_snd_sfxoff, GW(3))
                    SET_VALUE(GF(0), 0)
                END_IF
            ELSE
                IF_INT_EQUAL(GF(0), 0)
                    CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
                    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_GRID_MOVE1"), LW(0), LW(1), LW(2), GW(3))
                    SET_VALUE(GF(0), 1)
                END_IF
            END_IF
            SET_FLOAT_VALUE(GW(11), GW(10))
            DIVIDE_FLOAT(GW(11), FLOAT(4.0f))
            CALL(evt_mapobj_trans, 1, STRING("saku"), GW(10), GW(11), 0)
            CALL(evt_hit_bind_update, STRING("a_saku"))
            IF_INT_EQUAL(GF(0), 1)
                CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfx_pos, GW(3), LW(0), LW(1), LW(2))
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(vivi_chk)
    LOOP(0)
        CALL(evt_party_get_status, 6, LW(0))
        IF_INT_EQUAL(LW(0), 2)
            CALL(evt_mapobj_get_position, STRING("saku"), LW(0), LW(1), LW(2))
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            SUBTRACT_FLOAT(LW(0), LW(3))
            IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
            END_IF
            IF_FLOAT_LESS(LW(0), FLOAT(25.0f))
            ELSE
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(jin_10_init_evt)
    SET_VALUE(GF(0), 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_mobj_badgeblk, STRING("m"), 60, 60, -25, 243, 0, GSWF(2241), 1)
    SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(11), FLOAT(0.0f))
    CALL(evt_hit_bind_mapobj, STRING("a_saku"), STRING("saku"))
    CALL(evt_run_case_evt, 10, 1, STRING("a_saku"), 0, PTR(push_evt), 0)
    SPAWN_EVENT(unpush_evt)
    SPAWN_EVENT(vivi_chk)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

#include "moo_04.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "math_ext.h"
#include "party.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(mario_kirimomi);

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

// TODO: type
static u32 spline_dt[9] = {
    0xC14EABBA, 0x42F7B327, 0x44003E79, 0x418F97F1,
    0x42383335, 0x40A0614E, 0x42824EF5, 0x424199A0,
    0xC296A3F5,
};

static EVT_BEGIN(party_damage_pose)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_D_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_D_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("D_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_D_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_D_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_D_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_D_1"))
    END_SWITCH
    RETURN
EVT_END()

static USER_FUNC(mario_kirimomi) {
    s32* args;
    MarioWork* mario;
    PartyEntry* party;
    f32 scale;

    args = event->args;
    scale = evtGetFloat(event, args[0]);
    mario = marioGetPtr();
    party = partyGetPtr(0);
    *(f32*)((u8*)mario + 0xCC) = scale;
    *(f32*)((u8*)mario + 0xC8) = scale;
    *(f32*)((u8*)mario + 0xB4) = mario->wPlayerCollisionBox.y / 2.0f;
    *(f32*)((u8*)mario + 0xB4) = *(f32*)((u8*)mario + 0xB4) * *(f32*)((u8*)mario + 0xCC);
    *(f32*)((u8*)mario + 0xC4) -= 10.0f;
    *(f32*)((u8*)mario + 0xC4) = (f32)fmod(*(f32*)((u8*)mario + 0xC4), 360.0);

    if (party != NULL) {
        *(f32*)((u8*)party + 0x74) = scale;
        *(f32*)((u8*)party + 0x70) = scale;
        if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            *(f32*)((u8*)party + 0x8C) = *(f32*)((u8*)party + 0xF0) / 2.0f;
        } else {
            *(f32*)((u8*)party + 0x8C) = (*(f32*)((u8*)party + 0xF0) - 10.0f) / 2.0f;
        }
        *(f32*)((u8*)party + 0x8C) *= *(f32*)((u8*)party + 0x74);
        *(f32*)((u8*)party + 0x6C) -= 10.0f;
        *(f32*)((u8*)party + 0x6C) = (f32)fmod(*(f32*)((u8*)party + 0x6C), 360.0);
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(mario_fly)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_MARIO_FLY1"), LW(10))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    CALL_CHILD(party_damage_pose)
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_mario_set_pos, -300, -300, 0)
    CALL(evt_party_set_pos, 0, -300, -300, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 4, 1000, 10, 5000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
        CALL(mario_kirimomi, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_spline_init, 1, 6, PTR(spline_dt), 3, 0, 5000)
        LABEL(0)
        CALL(evt_sub_spline_get_value, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(3), 1)
            GOTO(0)
        END_IF
        CALL(evt_mario_set_pos, 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_spline_init, 1, 6, PTR(spline_dt), 3, 0, 5000)
        LABEL(1)
        CALL(evt_sub_spline_get_value, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(3), 1)
            GOTO(1)
        END_IF
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(7000)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_bero_mapchange, STRING("moo_00"), 0)
    RETURN
EVT_END()

EVT_BEGIN(kanbu_fly)
    WAIT_MS(2000)
    CALL(evt_npc_set_position, STRING("幹部"), 25, 75, -70)
    CALL(evt_npc_set_scale, STRING("幹部"), FLOAT(0.3f), FLOAT(0.3f), FLOAT(0.3f))
    BEGIN_INLINE_EVENT
        LOOP(0)
            SET_FLOAT_VALUE(LW(0), FLOAT(25.0f))
            CALL(evt_npc_get_scale, STRING("幹部"), LW(1), LW(2), LW(3))
            MULTIPLY_FLOAT(LW(0), LW(2))
            CALL(evt_npc_set_rotate_offset, STRING("幹部"), 0, LW(0), 0)
            CALL(evt_npc_add_rotate, STRING("幹部"), 0, 0, 30)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(-1.25f))
        SET_FLOAT_VALUE(LW(1), FLOAT(0.25f))
        SET_FLOAT_VALUE(LW(2), FLOAT(0.0f))
        CALL(evt_npc_get_position, STRING("幹部"), LW(3), LW(4), LW(5))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_3RD_FLY1"), LW(3), LW(4), LW(5), LW(9))
        CALL(evt_sub_intpl_msec_init, 4, 0, 100, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(LW(2), FLOAT(-125.0f))
            SET_FLOAT_VALUE(LW(3), FLOAT(25.0f))
            SET_FLOAT_VALUE(LW(4), FLOAT(0.0f))
            MULTIPLY_FLOAT(LW(2), LW(0))
            MULTIPLY_FLOAT(LW(3), LW(0))
            MULTIPLY_FLOAT(LW(4), LW(0))
            ADD_FLOAT(LW(2), FLOAT(25.0f))
            ADD_FLOAT(LW(3), FLOAT(75.0f))
            ADD_FLOAT(LW(4), FLOAT(-70.0f))
            CALL(evt_npc_set_position, STRING("幹部"), LW(2), LW(3), LW(4))
            CALL(evt_snd_sfx_pos, LW(9), LW(2), LW(3), LW(4))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_npc_set_position, STRING("幹部"), 0, -1000, 0)
        CALL(evt_snd_sfxoff, LW(9))
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 30, 1, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        CALL(evt_npc_set_scale, STRING("幹部"), LW(0), LW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_3RD_LIGHT1"), -100, 100, -70, 0)
    CALL(evt_eff64, 0, STRING("wangflush_n64"), 0, -100, 100, -70, 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(3000)
    CALL(evt_bero_mapchange, STRING("aji_14"), STRING("tuzuki"))
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[46] = {
    (u32)"幹部", 0x40000600, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_cam3d_evt_set, 75, 165, 533, 75, 70, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_fade_end_wait)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_45"), 0, 0)
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff_f, 512, 3000)
    CALL(evt_snd_envoff_f, 512, 3000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_bero_mapchange, STRING("gor_00"), STRING("epilogue2"))
    RETURN
EVT_END()

EVT_BEGIN(moo_04_init_evt)
    IF_INT_NOT_EQUAL(GSW(0), 402)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_MON1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_MOO4"))
    END_IF
    CALL(evt_npc_setup, PTR(npcEnt))
    SWITCH(GSW(0))
        CASE_LESS(358)
        CALL(evt_cam3d_evt_set, 0, 134, 533, 0, 38, -8, 0, 11)
        SPAWN_EVENT(mario_fly)
        WAIT_FRAMES(1)
        CASE_LESS(373)
        CALL(evt_cam3d_evt_set, 20, 200, 533, 20, 104, -8, 0, 11)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_set_pos, 0, -1000, 0)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        WAIT_FRAMES(1)
        SPAWN_EVENT(kanbu_fly)
        CASE_EQUAL(402)
        CALL(evt_mario_kill_party, 0)
        SPAWN_EVENT(epilogue_evt)
        CASE_DEFAULT
        CALL(evt_cam3d_evt_set, 75, 160, 533, 75, 65, -8, 0, 11)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_cont_onoff, 0)
            CALL(evt_party_cont_onoff, 0, 0)
            CALL(evt_mario_set_pos, 0, -1000, 0)
            CALL(evt_party_set_pos, 0, 0, -1000, 0)
            WAIT_FRAMES(1)
            CALL(evt_seq_wait, 2)
            WAIT_MS(3000)
            CALL(evt_msg_print, 0, STRING("stg7_aji_63"), 0, 0)
            WAIT_MS(2000)
            CALL(evt_eff64, 0, STRING("crystal_n64"), 0, 30, 90, -75, FLOAT(1.0f), 50, 0, 0, 0, 0, 0, 0)
            CALL(evt_eff64, 0, STRING("crystal_n64"), 6, 30, 90, -75, FLOAT(1.0f), 50, 0, 0, 0, 0, 0, 0)
            WAIT_MS(1000)
            CALL(evt_eff64, STRING("hikari"), STRING("radiation_n64"), 2, 30, 90, -75, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
            SET_VALUE(GSW(0), 377)
            CALL(evt_fade_entry, 17, 2000, 255, 255, 255)
            CALL(evt_fade_end_wait)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 1)
            CALL(evt_bg_disp_onoff, 0)
            CALL(evt_eff_delete, STRING("hikari"))
            CALL(evt_fade_entry, 16, 2000, 255, 255, 255)
            CALL(evt_fade_end_wait)
            CALL(evt_fade_set_mapchange_type, 1, 10, 1000, 9, 1000)
            SET_VALUE(GSWF(4175), 1)
            CALL(evt_bero_mapchange, STRING("tik_00"), 0)
        END_INLINE_EVENT
    END_SWITCH
    RETURN
EVT_END()
// clang-format on

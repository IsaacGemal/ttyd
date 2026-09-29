#include "moo_03.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "math_ext.h"
#include "party.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(z);
static USER_FUNC(mario_kirimomi);

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

static EVT_BEGIN(mario_fly)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_MARIO_FLY1"), LW(10))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_F_1"))
    CALL_CHILD(party_damage_pose)
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_mario_set_pos, -300, -300, 0)
    CALL(evt_party_set_pos, 0, -300, -300, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, -304, 304, 3000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(2), LW(0))
        MULTIPLY_FLOAT(LW(2), FLOAT(0.79f))
        ADD_FLOAT(LW(2), FLOAT(30.0f))
        CALL(evt_mario_set_pos, LW(0), LW(2), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, -304, 304, 3000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(2), LW(0))
        MULTIPLY_FLOAT(LW(2), FLOAT(0.79f))
        ADD_FLOAT(LW(2), FLOAT(-50.0f))
        CALL(evt_party_set_pos, 0, LW(0), LW(2), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg7_moo_01_kur"), 0, 0)
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg7_moo_01_nok"), 0, 0)
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg7_moo_01_bom"), 0, 0)
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg7_moo_01_yos"), 0, 0)
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg7_moo_01_win"), 0, 0)
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg7_moo_01_viv"), 0, 0)
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg7_moo_01_chu"), 0, 0)
    END_SWITCH
    WAIT_MS(3000)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_bero_mapchange, STRING("moo_04"), 0)
    RETURN
EVT_END()

static USER_FUNC(mario_kirimomi) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(0);

    *(f32*)((u8*)mario + 0xB4) = *(f32*)((u8*)mario + 0x1B8) * 0.5f;
    *(f32*)((u8*)mario + 0xC4) -= 10.0f;
    *(f32*)((u8*)mario + 0xC4) = (f32)fmod(*(f32*)((u8*)mario + 0xC4), 360.0);

    if (party != NULL) {
        if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            *(f32*)((u8*)party + 0x8C) = *(f32*)((u8*)party + 0xF0) * 0.5f;
        } else {
            *(f32*)((u8*)party + 0x8C) = (*(f32*)((u8*)party + 0xF0) - 10.0f) * 0.5f;
        }
        *(f32*)((u8*)party + 0x8C) *= party->field_70.y;
        *(f32*)((u8*)party + 0x6C) -= 10.0f;
        *(f32*)((u8*)party + 0x6C) = (f32)fmod(*(f32*)((u8*)party + 0x6C), 360.0);
    }
    return EVT_RETURN_BLOCK;
}
static USER_FUNC(z) {
    event->lwData[0] = (s32)(1024.0f * ((240.0f / (f32)tan(0.21816612780094147)) * 0.5f)) - EVTDAT_FLOAT_BASE;
    return EVT_RETURN_DONE;
}


EVT_BEGIN(moo_03_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_MON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_MOO3"))
    CALL(evt_map_playanim, STRING("hosi_01"), 1, 0)
    CALL(z)
    CALL(evt_cam3d_evt_set, 0, 0, LW(0), 0, 0, 0, 0, 11)
    SPAWN_EVENT(mario_fly)
    WAIT_FRAMES(10)
    BEGIN_INLINE_EVENT
        CALL(mario_kirimomi)
    END_INLINE_EVENT
    RETURN
EVT_END()
// clang-format on

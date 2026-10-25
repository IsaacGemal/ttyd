#include "pik_02.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_memcard.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "kpa_act.h"
#include "party/party_motion.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
#include "eff/eff_starstone.h"
#include "mario/mario_cam.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(stone_bg);
static USER_FUNC(starstone_entry);
static USER_FUNC(get_starstone_pos);
static USER_FUNC(set_starstone_pos);
static USER_FUNC(starstone_fall);
static USER_FUNC(npc_set_pos);
BeroEntry s_bero_peach[1];
s32 evt_kaiga[14];

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

EVT_BEGIN(haikei_on)
    CALL(evt_hitobj_onoff, STRING("a_E_yuka"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("h_bero"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("h_dokan"), 1, 1)
    CALL(evt_del_case_evt, 0, GW(10))
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(haikei_off)
    CALL(evt_hitobj_onoff, STRING("a_E_yuka"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("h_bero"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("h_dokan"), 1, 0)
    CALL(evt_run_case_evt, 9, 1, STRING("a_kaiga"), 0, PTR(evt_kaiga), GW(10))
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_return_sub)
    CALL(evt_mario_wait_movable)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -170, -960, FLOAT(80.0f))
        CALL(evt_party_set_dir_npc, 0, STRING("ポワン探偵"))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -160, -935, FLOAT(80.0f))
    CALL(evt_mario_set_dir_npc, STRING("ポワン探偵"))
    CALL(evt_cam3d_evt_set, -130, 64, -629, -130, 50, -929, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg6_pik_21"), 0, STRING("ポワン探偵"))
    SET_VALUE(GSW(0), 339)
    CALL(evt_fade_set_mapchange_type, 1, 55, 0, 54, 0)
    SET_VALUE(LW(0), STRING("s_bero"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_bero_overwrite, STRING("s_bero"), PTR(s_bero_peach))
    SET_VALUE(LW(0), STRING("s_bero"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER(LW(0), -100)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_reverse_ry, STRING("ポワン探偵"))
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(evt_return)
    IF_INT_EQUAL(GSW(0), 338)
        SPAWN_EVENT(evt_return_sub)
    END_IF
    CALL_CHILD(haikei_off)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_go)
    CALL_CHILD(haikei_on)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[5] = {
    {"s_bero", 1, 6, 20000, 100000, 0, 0, -1, haikei_off, -1, NULL, "pik_01", "n_bero", 5, 5, s_door_close, s_door_open},
    {"dokan", 2, 0, 9, 100000, 0, 0, -1, evt_return, 6, evt_go, NULL, "h_dokan", 1, 1, 0, 0},
    {"h_dokan", 0x1002, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan", 1, 1, 0, 0},
    {"h_bero", 0x1000, 0, 2, 100000, 0, 0, -1, NULL, 0, NULL, "pik_03", "s_bero", 1, 1, 0, 0},
    {NULL},
};


EVT_BEGIN(out_room)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_envoff_f, 512, 2000)
    CALL(evt_snd_sfx_all_stop)
    RETURN
EVT_END()

BeroEntry s_bero_peach[1] = {
    {"s_bero", 1, 6, 20000, 100000, 0, 0, 0x50, haikei_off, -1, out_room, "aji_10", NULL, 5, 5, s_door_close, s_door_open},
};


static EVT_BEGIN(init_powan)
    SWITCH(GSW(0))
        CASE_BETWEEN(336, 337)
        CALL(evt_npc_set_position, STRING("me"), -120, 0, 490)
        CASE_EQUAL(338)
        CALL(evt_npc_set_position, STRING("me"), -100, 10, -930)
        CASE_GREATER(338)
        CALL(evt_npc_set_position, STRING("me"), -120, 0, 490)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_powan)
    SWITCH(GSW(0))
        CASE_LESS(338)
        CALL(evt_msg_print, 0, STRING("stg6_pik_19"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(339)
        IF_INT_EQUAL(LSWF(0), 0)
            SET_VALUE(LSWF(0), 1)
            CALL(evt_msg_print, 0, STRING("stg6_pik_22"), 0, STRING("me"))
        ELSE
            SET_VALUE(LSWF(0), 0)
            CALL(evt_msg_print, 0, STRING("stg6_pik_23"), 0, STRING("me"))
        END_IF
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_09_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_09_1_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_majo)
    SWITCH(GSW(0))
        CASE_LESS(336)
        CALL(evt_npc_set_position, STRING("me"), -35, 10, -950)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_mari)
    SWITCH(GSW(0))
        CASE_LESS(336)
        CALL(evt_npc_set_position, STRING("me"), 30, 10, -950)
        CALL(evt_npc_set_ry, STRING("me"), 270)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_ramp)
    SWITCH(GSW(0))
        CASE_LESS(336)
        CALL(evt_npc_set_position, STRING("me"), 20, 10, -910)
        CALL(evt_npc_set_ry, STRING("me"), 270)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(patakuri_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

static EVT_BEGIN(baba_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

static NpcSetupInfo npc_data[7] = {
    {"ポワン探偵", 0x40000600, 0, init_powan, NULL, talk_powan},
    {"マジョリン", 0x40000600, 0, init_majo},
    {"マリリン", 0x40000600, 0, init_mari},
    {"ランペル", 0x40000600, 0, init_ramp},
    {"パタクリ", 0x40000600, 0, patakuri_init},
    {"ババ", 0x40000600, 0, baba_init},
    {NULL},
};

static USER_FUNC(npc_set_pos) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);
    Vec pos = { 0.0f, 0.0f, 0.0f };

    pos.x = x;
    pos.y = y;
    pos.z = z;
    npc->prevPosition = pos;
    npc->position = npc->prevPosition;
    return EVT_RETURN_DONE;
}

USER_FUNC(hoshi) {
    event->lwData[10] = (s32)effStarStoneEntry(5, 0.0f, 60.0f, -950.0f, 1.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(hoshi_delete) {
    effSoftDelete((EffectEntry*)event->lwData[10]);
    return EVT_RETURN_DONE;
}

USER_FUNC(hoshi_r) {
    EffectEntry* entry = (EffectEntry*)event->lwData[10];

    ((effStarStoneData*)entry->userdata)->rotY = (f32)evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

// Keeping each polynomial in one helper (rather than inlining it twice) is what puts the angle^2
// scratch in f0 and the wrapped-angle pivot in f1; with the coefficients as helper locals the seed
// lands in f4/f3/f2 exactly as retail. See evt_sub_get_coin for the same helper structure.
static inline f32 hoshi_sin_approx(f32 angle) {
    f32 v = 0.00761f;
    f32 b = 0.16605f;
    f32 one = 1.0f;
    f32 t, u;

    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) {
            u = angle * angle;
            v = v * u - b;
            v = v * u + one;
            v = v * angle;
        } else {
            t = 1.5707964f - (angle - 1.5707964f);
            u = t * t;
            v = v * u - b;
            v = v * u + one;
            v = v * t;
        }
    } else if (angle < 4.712389f) {
        t = angle - 3.1415927f;
        u = t * t;
        v = v * u - b;
        v = v * u + one;
        v = v * t;
        v = -v;
    } else {
        t = 1.5707964f - (angle - 4.712389f);
        u = t * t;
        v = v * u - b;
        v = v * u + one;
        v = v * t;
        v = -v;
    }
    return v;
}

static inline f32 hoshi_cos_approx(f32 angle) {
    f32 v = 0.03705f;
    f32 b = 0.4967f;
    f32 one = 1.0f;
    f32 u;

    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) {
            u = angle * angle;
            v = v * u - b;
            v = v * u + one;
        } else {
            u = 1.5707964f - (angle - 1.5707964f);
            u = u * u;
            v = v * u - b;
            v = v * u + one;
            v = -v;
        }
    } else if (angle < 4.712389f) {
        u = angle - 3.1415927f;
        u = u * u;
        v = v * u - b;
        v = v * u + one;
        v = -v;
    } else {
        u = 1.5707964f - (angle - 4.712389f);
        u = u * u;
        v = v * u - b;
        v = v * u + one;
    }
    return v;
}

// hoshi_jump: retail spills the first-call d/t/scale through 4-byte stack slots and copies them out
// with lwz/stw, and reads the 50.0 and 0.5 from the pool.
USER_FUNC(hoshi_jump) {
    EffectEntry* entry = (EffectEntry*)event->lwData[10];
    effStarStoneData* data = entry->userdata;
    NpcEntry* npc = npcNameToPtr("マジョリン");
    f32 nx = npc->position.x;
    f32 ny = npc->position.y + 50.0f;
    f32 nz = npc->position.z;
    f32 f2, f4, f5;
    f32 t;
    f32 scale;
    f32 d;

    scale = 0.5f;
    if (isFirstCall) {
        d = distABf(nx, nz, data->x, data->z) / scale;
        f4 = ny - data->y;
        f2 = 0.1f * d;
        t = f2 / 2.0f + f4 / d;
#ifdef MUST_MATCH
        // MUST_MATCH: retail spills the first-call values through 4-byte stack slots and copies them to
        // event->userdata with lwz/stw; the natural float stores are under #else.
        event->userdata[0] = *(s32*)&d;
        event->userdata[1] = *(s32*)&t;
        event->userdata[2] = *(s32*)&scale;
#else
        event->userdataf[0] = d;
        event->userdataf[1] = t;
        event->userdataf[2] = scale;
#endif
        event->field_0x84 = 0;
    }

    d = event->userdataf[0];
    t = event->userdataf[1];
    scale = event->userdataf[2];

    f2 = 6.2831855f * angleABf(data->x, data->z, nx, nz);
    f5 = f2 / 360.0f;
    data->x = scale * hoshi_sin_approx(f5) + data->x;
    data->z = data->z - scale * hoshi_cos_approx(f5);
    data->y = data->y + t;

    t = t - 0.1f;
    d = d - 1.0f;
    if (d < 0.0f) {
        data->x = nx;
        data->y = ny;
        data->z = nz;
        return EVT_RETURN_DONE;
    }
#ifdef MUST_MATCH
    event->userdata[0] = *(s32*)&d;
    event->userdata[1] = *(s32*)&t;
#else
    event->userdataf[0] = d;
    event->userdataf[1] = t;
#endif
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(evt_garnet)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_rotate, 1, STRING("door_1_a"), 0, 70, 0)
    CALL(evt_mapobj_rotate, 1, STRING("door_1_b"), 0, -70, 0)
    CALL(evt_mario_set_pos, 0, 0, 650)
    CALL(evt_party_set_pos, 0, 30, 0, 680)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), -30, 0, 710)
    CALL(evt_npc_set_ry, STRING("ポワン探偵"), 90)
    CALL(hoshi)
    CALL(evt_seq_wait, 2)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos, 0, 570, 1000)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos, 0, 30, 580, 1000)
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("ポワン探偵"), 2)
    CALL(evt_npc_move_position, STRING("ポワン探偵"), -30, 600, 1000, 0, 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 70, 0, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("door_1_a"), 0, LW(0), 0)
    MULTIPLY(LW(0), -1)
    CALL(evt_mapobj_rotate, 1, STRING("door_1_b"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("eff0"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("eff1"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 0, 125, -590, 0, 65, -930, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED1"))
    WAIT_MS(1000)
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("マジョリン"), STRING("MJR_A2_1"))
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT_GET_ID(LW(15))
        LOOP(0)
            CALL(hoshi_r, LW(0))
            ADD(LW(0), 20)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(hoshi_jump)
    WAIT_MS(500)
    DELETE_EVENT(LW(15))
    CALL(hoshi_delete)
    CALL(evt_npc_get_position, STRING("マジョリン"), LW(0), LW(1), LW(2))
    ADD(LW(1), 50)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_STARATONE_LIGHT1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff64, 0, STRING("stardust_n64"), 0, LW(0), LW(1), LW(2), FLOAT(20.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_WITCH1"))
    CALL(evt_snd_envoff, 1024)
    CALL(evt_msg_print, 0, STRING("stg6_pik_13"), 0, STRING("マジョリン"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_majo_disp_off, STRING("マジョリン"))
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_npc_majo_disp_off, STRING("マリリン"))
    CALL(evt_npc_set_position, STRING("マジョリン"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("マリリン"), 0, -1000, 0)
    LOOP(2)
        CALL(evt_npc_reverse_ry, STRING("ランペル"))
        WAIT_MS(400)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg6_pik_14"), 0, STRING("ランペル"))
    CALL(evt_cam3d_evt_set, 0, 54, -568, -230, 30, -930, 2900, 11)
    CALL(evt_npc_status_onoff, 1, STRING("ランペル"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("ランペル"), 131120)
    CALL(evt_npc_move_position, STRING("ランペル"), -105, -845, 0, FLOAT(100.0f), 1)
    CALL(evt_npc_flag_onoff, 0, STRING("ランペル"), 131088)
    CALL(evt_npc_move_position, STRING("ランペル"), -185, -795, 0, FLOAT(100.0f), 1)
    WAIT_MS(500)
    CALL(evt_npc_jump_position_nohit, STRING("ランペル"), -250, 0, -795, FLOAT(300.0f), 40)
    CALL(evt_npc_set_position, STRING("ランペル"), 0, -1000, 0)
    WAIT_MS(300)
    CALL(evt_snd_bgmoff_f, 512, 3000)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ポワン探偵"), 32768)
    CALL(evt_party_set_pos, 0, -90, 0, 450)
    CALL(evt_mario_set_pos, -70, 0, 490)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), -120, 0, 490)
    CALL(evt_set_dir_to_target, STRING("ポワン探偵"), STRING("mario"))
    CALL(evt_party_stop, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_cam3d_evt_set, -100, 77, 748, -100, 35, 450, 0, 11)
    WAIT_MS(500)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_pik_15_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_pik_15_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_pik_15_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_pik_15_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_pik_15_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_pik_15_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_pik_15_6"), 0, STRING("party"))
    END_SWITCH
    WAIT_MS(500)
    CALL(evt_party_run, 0)
    CALL(evt_msg_print, 0, STRING("stg6_pik_16"), 0, STRING("ポワン探偵"))
    WAIT_MS(500)
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("stg6_pik_17"), 0, STRING("ポワン探偵"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_AWAKE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("stg6_pik_18"), 0, STRING("ポワン探偵"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 336)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_sw)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -65, 450, -480, -165, 115, -930, 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, STRING("ka_ten"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_CURTAIN_UP1"), LW(0), LW(1), LW(2), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 0, 150, 1500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("ka_ten"), 0, LW(0), 0)
    CALL(evt_hit_bind_update, STRING("a_ka_ten"))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    SET_VALUE(GSW(0), 337)
    SET_VALUE(LW(0), 1)
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    WAIT_MS(1000)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kaiga)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg6_pik_24"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_daiza)
    CALL(evt_mario_key_onoff, 0)
    SWITCH(GSW(0))
        CASE_BETWEEN(347, 374)
        CALL(evt_msg_print, 0, STRING("stg7_pik_clear_09_2"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg6_pik_25"), 0, 0)
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

USER_FUNC(get_starstone_seq) {
    s32* args = event->args;
    effStarStoneData* data = effNameToPtr((const char*)evtGetValue(event, args[0]))->userdata;

    evtSetValue(event, args[1], data->state);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_fall) {
    effStarStoneData* data = effNameToPtr((const char*)evtGetValue(event, event->args[0]))->userdata;

    data->state = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(set_starstone_pos) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* entry = effNameToPtr((const char*)name);

    ((effStarStoneData*)entry->userdata)->x = x;
    ((effStarStoneData*)entry->userdata)->y = y;
    ((effStarStoneData*)entry->userdata)->z = z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(get_starstone_pos) {
    s32* args = event->args;
    EffectEntry* entry = effNameToPtr((const char*)evtGetValue(event, args[0]));

    evtSetFloat(event, args[1], ((effStarStoneData*)entry->userdata)->x);
    evtSetFloat(event, args[2], ((effStarStoneData*)entry->userdata)->y);
    evtSetFloat(event, args[3], ((effStarStoneData*)entry->userdata)->z);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_entry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 idx = evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    f32 scale = evtGetFloat(event, args[5]);
    EffectEntry* entry = effStarStoneEntry(idx, x, y, z, scale);

    effSetName(entry, (const char*)name);
    ((effStarStoneData*)entry->userdata)->flags |= 0x10;
    return EVT_RETURN_DONE;
}

static USER_FUNC(stone_bg) {
    effStarStoneData* data = effNameToPtr("sub_bg")->userdata;

    data->camId = 5;
    data = effNameToPtr("nise")->userdata;
    data->camId = 5;
    marioSetCamId(5);
    return EVT_RETURN_DONE;
}

USER_FUNC(reset_stone_cam) {
    effStarStoneData* data = effNameToPtr("nise")->userdata;

    data->camId = 4;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(pik_02_koopa_evt)
    CALL(evt_snd_envoff, 512)
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_mario_key_onoff, 0)
    CALL(starstone_entry, STRING("nise"), 5, 0, 57, -955, 1)
    CALL(evt_mario_set_pos, 60, 0, -500)
    CALL(evt_npc_set_position, STRING("ババ"), 140, 0, -500)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_npc_set_ry, STRING("ババ"), 270)
    CALL(evt_cam3d_evt_set, 0, 360, -338, 0, 250, -1130, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_KUPPA1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_cam3d_evt_set, 0, 150, -338, 0, 40, -1130, 7000, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ババ"), 140, -880, 0, 60, 0)
        CALL(evt_npc_set_position, STRING("ババ"), 140, 9, -885)
        CALL(evt_npc_move_position, STRING("ババ"), 140, -925, 0, 60, 0)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 60, -880, 60)
    CALL(evt_mario_set_pos, 60, 9, -885)
    CALL(evt_mario_mov_pos2, 60, -925, 60)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_013_01"), 0, STRING("mario"))
    CALL(evt_cam3d_evt_set, 60, 127, -467, 60, 80, -914, 500, 11)
    WAIT_MS(600)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_HAPPY2_1"), LW(0), LW(1), LW(2), 0)
    CALL(set_starstone_pos, STRING("nise"), 80, 85, -915)
    CALL(kpa_chg_pose, STRING("KPA_Y_1"))
    CALL(evt_eff64, STRING("sub_hikari"), STRING("radiation_n64"), 2, 60, 56, -925, 1, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("sub_bg"), STRING("itemget"), 0, 60, 56, -935, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(stone_bg)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GET_IMPORTANT_ITEM1"))
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_eff_softdelete, STRING("sub_hikari"))
    CALL(evt_eff_softdelete, STRING("sub_bg"))
    WAIT_MS(300)
    CALL(reset_stone_cam)
    CALL(evt_mario_init_camid)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_6"), STRING("KPA_T_6"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_014"), 0, STRING("mario"))
    WAIT_MS(500)
    CALL(evt_npc_set_autotalkpose, STRING("ババ"), STRING("U_1"), STRING("T_5"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_015"), 0, STRING("ババ"))
    CALL(evt_cam3d_evt_set, 19, 143, -268, 19, 74, -914, 800, 11)
    WAIT_MS(800)
    CALL(evt_npc_status_onoff, 1, STRING("ポワン探偵"), 2)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), -80, 0, -600)
    CALL(evt_npc_set_ry, STRING("ポワン探偵"), 90)
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("ポワン探偵"), -70, -800, 0, FLOAT(80.0f), 0)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ポワン探偵"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(700)
        CALL(kpa_chg_pose, STRING("KPA_S_1"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT_FLOAT(LW(0), 35)
        ADD_FLOAT(LW(1), 20)
        CALL(set_starstone_pos, STRING("nise"), LW(0), LW(1), LW(2))
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -65, 60, -563, -65, 40, -762, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_016"), 0, STRING("ポワン探偵"))
    CALL(evt_npc_set_autotalkpose, STRING("ババ"), STRING("S_1"), STRING("T_1"))
    CALL(evt_npc_set_stay_anim, STRING("ババ"))
    CALL(evt_cam3d_evt_set, 10, 113, -367, 10, 55, -914, 1000, 11)
    WAIT_MS(1300)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SUBTRACT_FLOAT(LW(0), 35)
            ADD_FLOAT(LW(1), 20)
            CALL(set_starstone_pos, STRING("nise"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT_FLOAT(LW(0), 35)
        ADD_FLOAT(LW(1), 20)
        CALL(set_starstone_pos, STRING("nise"), LW(0), LW(1), LW(2))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, STRING("ババ"), 131088)
        CALL(evt_npc_move_position, STRING("ババ"), 111, -781, 1750, 0, 0)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos, 60, -800, 1750)
    SET_VALUE(GF(0), 1)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_017"), 0, STRING("mario"))
    CALL(evt_msg_select, 0, STRING("kpa_stg7_018"))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(1), STRING("kpa_stg7_019"))
        SET_VALUE(LW(2), STRING("kpa_stg7_020"))
        CASE_EQUAL(1)
        SET_VALUE(LW(1), STRING("kpa_stg7_021"))
        SET_VALUE(LW(2), STRING("kpa_stg7_022"))
        CASE_EQUAL(2)
        SET_VALUE(LW(1), STRING("kpa_stg7_023"))
        SET_VALUE(LW(2), STRING("kpa_stg7_024"))
    END_SWITCH
    CALL(evt_msg_print_add, 0, LW(1))
    CALL(evt_cam3d_evt_set, -65, 60, -563, -65, 40, -762, 800, 11)
    WAIT_MS(800)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_npc_get_position, STRING("ポワン探偵"), LW(3), LW(4), LW(5))
        SET_VALUE(LW(6), LW(3))
        SUBTRACT(LW(6), 1)
        LOOP(10)
            CALL(evt_npc_set_position, STRING("ポワン探偵"), LW(6), LW(4), LW(5))
            WAIT_FRAMES(2)
            ADD(LW(6), 2)
            CALL(evt_npc_set_position, STRING("ポワン探偵"), LW(6), LW(4), LW(5))
            WAIT_FRAMES(2)
            SUBTRACT(LW(6), 2)
        END_LOOP
        CALL(evt_npc_set_position, STRING("ポワン探偵"), LW(3), LW(4), LW(5))
    END_IF
    CALL(evt_msg_print, 0, LW(2), 0, STRING("ポワン探偵"))
    CALL(evt_cam3d_evt_set, 10, 113, -367, 10, 55, -914, 800, 11)
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED3"))
        WAIT_MS(1000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_SURPRISED2_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("ババ"), STRING("P_1"))
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_5"), STRING("KPA_T_5"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_025"), 0, STRING("mario"))
    CALL(get_starstone_pos, STRING("nise"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_STONE_DOWN1"), LW(0), LW(1), LW(2), LW(15))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(get_starstone_seq, STRING("nise"), LW(0))
            IF_INT_EQUAL(LW(0), 3)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(15))
        CALL(get_starstone_pos, STRING("nise"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_STONE_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(starstone_fall, STRING("nise"))
    WAIT_MS(1000)
    CALL(get_starstone_pos, STRING("nise"), LW(0), LW(1), LW(2))
    LOOP(10)
        CALL(set_starstone_pos, STRING("nise"), 0, -1000, 0)
        WAIT_FRAMES(2)
        CALL(set_starstone_pos, STRING("nise"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(2)
    END_LOOP
    CALL(evt_eff_softdelete, STRING("nise"))
    CALL(evt_npc_flag_onoff, 1, STRING("ババ"), 32)
    CALL(evt_npc_move_position, STRING("ババ"), -13, -792, 0, FLOAT(90.0f), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ババ"), 32)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_026"), 0, STRING("ババ"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_027"), 0, STRING("ポワン探偵"))
    CALL(evt_npc_set_ry, STRING("ババ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_1"), STRING("KPA_T_1"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_028"), 0, STRING("mario"))
    CALL(kpa_chg_pose, STRING("KPA_I_2"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            CALL(evt_sub_get_stopwatch, LW(0), LW(1))
            IF_INT_GREATER_OR_EQUAL(LW(1), 30000)
                BREAK_LOOP
            END_IF
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01f), 150)
            WAIT_MS(150)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_ANGRY1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_ANGRY2"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("kpa_stg7_029"), 0, STRING("ババ"))
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("パタクリ"), -250, 130, -800)
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_PKURI_GLASS_BREAK1"), -250, 130, -800, LW(0))
    LOOP(0)
        CALL(evt_snd_sfxchk, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(kpa_chg_pose, STRING("KPA_S_1"))
    SET_VALUE(GF(0), 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_SURPRISED1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ババ"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_npc_set_ry, STRING("ババ"), 270)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_030"), 0, STRING("パタクリ"))
    CALL(evt_npc_set_position, STRING("パタクリ"), -200, 80, -800)
    CALL(evt_npc_set_ry, STRING("パタクリ"), 90)
    WAIT_MS(1000)
    SET_VALUE(GW(0), 80)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 180, 330, 1300)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_sub_get_sincos, LW(0), LW(2), LW(3))
        MULTIPLY_FLOAT(LW(2), FLOAT(50.0f))
        ADD_FLOAT(LW(2), FLOAT(80.0f))
        SET_VALUE(GW(0), LW(2))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("ポワン探偵"), LW(2), LW(3), LW(4))
    CALL(evt_sub_intpl_msec_init, 0, -200, LW(2), 1300)
    LABEL(2)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_npc_set_position, STRING("パタクリ"), LW(0), GW(0), -800)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    CALL(evt_npc_flag_onoff, 0, STRING("パタクリ"), 1024)
    CALL(evt_npc_set_anim, STRING("パタクリ"), STRING("PTK_A_1"))
    CALL(evt_npc_get_position, STRING("パタクリ"), LW(2), LW(3), LW(4))
    CALL(evt_sub_intpl_msec_init, 4, LW(3), 30, 1200)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_npc_set_position, STRING("パタクリ"), LW(2), LW(0), LW(4))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_npc_set_anim, STRING("パタクリ"), STRING("PTK_S_2"))
    SET_VALUE(LW(5), LW(0))
    SUBTRACT(LW(5), 5)
    CALL(evt_eff, STRING(""), STRING("kemuri"), 0, LW(2), LW(5), LW(4), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_PKURI_LANDING1"), LW(2), LW(0), LW(4), 0)
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("パタクリ"), STRING("PTK_S_3"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_031"), 0, STRING("ババ"))
    CALL(evt_cam3d_evt_set, -65, 83, -504, -65, 47, -852, 800, 11)
    WAIT_MS(800)
    CALL(evt_npc_set_autotalkpose, STRING("パタクリ"), STRING("PTK_S_3"), STRING("PTK_T_2"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_032"), 0, STRING("パタクリ"))
    CALL(evt_npc_set_autotalkpose, STRING("ババ"), STRING("S_1"), STRING("T_1"))
    CALL(evt_npc_set_stay_anim, STRING("ババ"))
    CALL(evt_cam3d_evt_set, 20, 114, -369, 20, 58, -914, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_033"), 0, STRING("mario"))
    CALL(evt_npc_set_ry, STRING("ババ"), 90)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_034"), 0, STRING("ババ"))
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_4"), STRING("KPA_T_3"))
    CALL(evt_msg_print, 0, STRING("kpa_stg7_035"), 0, STRING("mario"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mario_mov_pos2, LW(0), -400, FLOAT(90.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
        CALL(evt_npc_move_position, STRING("ババ"), LW(0), -400, 0, FLOAT(90.0f), 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    CALL(evt_npc_flag_onoff, 1, STRING("パタクリ"), 1024)
    CALL(evt_npc_get_position, STRING("パタクリ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_glide_position, STRING("パタクリ"), LW(0), LW(1), -400, 0, FLOAT(90.0f), FLOAT(30.0f), 1, 0)
    CALL(evt_cam3d_evt_set, -65, 60, -563, -65, 40, -762, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("kpa_stg7_036"), 0, STRING("ポワン探偵"))
    SET_VALUE(GSW(0), 375)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_fade_entry, 6, 300, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(2000)
    SET_VALUE(LF(10), 0)
    SET_VALUE(LW(0), 20)
    CALL_CHILD(evt_memcard_save)
    CALL(evt_mario_set_mode, 0)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_envoff_f, 512, 2000)
    CALL(evt_fade_set_mapchange_type, 1, 59, 0, 58, 0)
    CALL(evt_bero_mapchange, STRING("aji_18"), STRING("e_bero"))
    RETURN
EVT_END()

EVT_BEGIN(nise_starstone_init)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 347)
        IF_INT_LESS(GSW(0), 374)
            CALL(starstone_entry, STRING("nisemono"), 5, 0, 57, -955, 1)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(pik_02_init_evt)
    IF_INT_LESS(GSW(0), 336)
        SET_VALUE(LW(0), PTR(bero_data))
        CALL(evt_bero_get_info)
        CALL_CHILD(bero_case_entry)
        CALL_CHILD(haikei_off)
    ELSE
        SET_VALUE(LW(0), PTR(bero_data))
        CALL(evt_bero_get_info)
        CALL_CHILD(evt_bero_info_run)
    END_IF
    IF_INT_NOT_EQUAL(GSW(0), 374)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_PIK2"))
        CALL(evt_snd_set_rev_mode, 2)
    END_IF
    CALL(evt_npc_setup, PTR(npc_data))
    CALL(evt_hit_bind_mapobj, STRING("a_ka_ten"), STRING("ka_ten"))
    IF_INT_LESS(GSW(0), 337)
        CALL(evt_mobj_switch_blue, 0, STRING("sw"), -300, 330, -975, PTR(evt_sw), 0)
        SET_VALUE(LW(0), 1)
        CALL_CHILD(bero_case_switch_off)
    ELSE
        CALL(evt_mapobj_trans, 1, STRING("ka_ten"), 0, 150, 0)
        CALL(evt_hit_bind_update, STRING("a_ka_ten"))
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("s_bero"))
        CALL(evt_run_case_evt, 9, 1, STRING("a_kaiga"), 0, PTR(evt_kaiga), GW(10))
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("dai_a"), 0, PTR(evt_daiza), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("door_1"), 2)
    IF_INT_LESS(GSW(0), 336)
        SPAWN_EVENT(evt_garnet)
    END_IF
    IF_INT_EQUAL(GSW(0), 339)
        BEGIN_INLINE_EVENT
            CALL(evt_bero_exec_wait, 65536)
            CALL(evt_fade_set_mapchange_type, 1, 55, 0, 54, 0)
            SET_VALUE(LW(0), STRING("s_bero"))
            CALL_CHILD(bero_case_switch_off)
            CALL(evt_bero_overwrite, STRING("s_bero"), PTR(s_bero_peach))
            SET_VALUE(LW(0), STRING("s_bero"))
            CALL_CHILD(bero_case_switch_on)
        END_INLINE_EVENT
    END_IF
    CALL_CHILD(nise_starstone_init)
    IF_INT_EQUAL(GSW(0), 374)
        CALL(evt_mario_set_mode, 2)
        CALL(evt_mario_kill_party, 0)
        SPAWN_EVENT(pik_02_koopa_evt)
    END_IF
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou"), STRING("a_hi1"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("saku_2"), 2)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()


// clang-format on

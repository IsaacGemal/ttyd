#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on

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

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[] = {
    {"w_bero", 1, 4, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_06", "w_bero", 5, 5,
     (s32)&w_door_close, (s32)&w_door_open},
    {NULL},
};

static EVT_BEGIN(evt_kinoshikowa)
    CALL(evt_npc_set_position, STRING("キノシコワ"), -200, 0, 10)
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, 30, 0)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(0), -55)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_eff_fukidashi, 1, 0, STRING("キノシコワ"), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
        CALL(evt_npc_move_position, STRING("キノシコワ"), -250, 10, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("door_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), 30)
    LOOP(10)
        SUBTRACT(LW(0), 3)
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_307_13_00"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_block_broken)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_move_pos2, 0, 95, 10, FLOAT(80.0f))
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED2"))
    CALL(evt_party_set_dir, 0, 90, 200)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_307_10"))
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_307_11"), 0, STRING("ブロッツダミー"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    SPAWN_EVENT(evt_kinoshikowa)
    SET_VALUE(GSW(0), 156)
    RETURN
EVT_END()

static EVT_BEGIN(init_michel)
    IF_INT_LESS(GSW(0), 164)
        CALL(evt_npc_set_anim, STRING("マイケル"), STRING("NKT_Z_1"))
        CALL(evt_npc_set_position, STRING("マイケル"), 175, 0, -20)
        CALL(evt_npc_set_rotate, STRING("マイケル"), -80, 0, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(init_michel_dummy)
    IF_INT_LESS(GSW(0), 164)
        CALL(evt_npc_set_position, STRING("マイケルダミー"), 175, 0, -20)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(talk_michel)
    CALL(evt_msg_print, 0, STRING("stg3_tou_307_14_01"), 0, STRING("マイケル"))
    RETURN
EVT_END()

static EVT_BEGIN(init_brots)
    IF_INT_LESS(GSW(0), 164)
        CALL(evt_npc_set_position, STRING("ブロッツ"), 110, 3, -60)
        CALL(evt_npc_set_rotate, STRING("ブロッツ"), -70, 0, -80)
        CALL(evt_npc_flag_onoff, 0, STRING("ブロッツ"), 1536)
        CALL(evt_npc_set_anim, STRING("ブロッツ"), STRING("S_1"))
        IF_INT_EQUAL(GSWF(2518), 1)
            CALL(evt_npc_set_anim, STRING("ブロッツ"), STRING("X_1"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(init_brots_dummy)
    IF_INT_LESS(GSW(0), 164)
        CALL(evt_npc_set_position, STRING("ブロッツダミー"), 125, 4, -60)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(talk_brots)
    IF_INT_EQUAL(GSWF(2518), 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_party_set_breed_pose, 0, 1)
        CALL(evt_npc_get_position, STRING("ブロッツ"), LW(0), LW(1), LW(2))
        CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
        IF_INT_LESS(LW(0), LW(3))
            CALL(evt_party_set_dir, 0, 270, 200)
        ELSE
            CALL(evt_party_set_dir, 0, 90, 200)
        END_IF
        CALL(evt_npc_flag_onoff, 1, STRING("ブロッツ"), 1536)
        CALL(evt_msg_print, 0, STRING("stg3_tou_307_12"), 0, 0)
        CALL(evt_npc_flag_onoff, 0, STRING("ブロッツ"), 1536)
        CALL(evt_npc_set_anim, STRING("ブロッツ"), STRING("X_1"))
        CALL(evt_msg_print_party, STRING("stg3_tou_307_13"))
        SET_VALUE(GSWF(2518), 1)
        CALL(evt_party_cont_onoff, 0, 1)
    ELSE
        CALL(evt_npc_flag_onoff, 0, STRING("ブロッツ"), 1536)
        CALL(evt_npc_set_anim, STRING("ブロッツ"), STRING("S_1"))
        CALL(evt_msg_print, 0, STRING("stg3_tou_307_14"), 0, 0)
        CALL(evt_npc_set_anim, STRING("ブロッツ"), STRING("X_1"))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(init_kinoshikowa)
    RETURN
EVT_END()

static EVT_BEGIN(talk_kinoshikowa)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"マイケル", 0x41000020, 0, &init_michel},
    {"ブロッツ", 0x41000620, 0, &init_brots},
    {"マイケルダミー", 0x41000080, 0, &init_michel_dummy, NULL, &talk_michel},
    {"ブロッツダミー", 0x41000080, 0, &init_brots_dummy, NULL, &talk_brots},
    {"キノシコワ", 0x40000600, 0, &init_kinoshikowa, NULL, &talk_kinoshikowa},
    {NULL},
};

EVT_BEGIN(tou_12_init_evt)
    IF_INT_EQUAL(GSW(0), 163)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
    ELSE
        SWITCH(GSW(0))
            CASE_BETWEEN(127, 137)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
            CALL(evt_snd_bgmoff, 26624)
            CASE_GREATER_OR_EQUAL(165)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
            CALL(evt_snd_bgmoff, 26624)
            CASE_DEFAULT
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
        END_SWITCH
    END_IF
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_item_entry, STRING("item01"), 125, 190, 0, 30, 16, GSWF(5627), 0)
    IF_INT_LESS(GSW(0), 156)
        CALL(evt_mobj_lv_blk, STRING("aa"), 125, 0, 0, PTR(evt_block_broken), GSWF(2441), STRING("MOBJ_Lv1BigBigBlock"))
        CALL(evt_mobj_flag_onoff, 0, STRING("aa"), 64)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 164)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gor_hae"), 1)
    ELSE
        CALL(evt_map_playanim, STRING("anm_hae"), 1, 0)
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(evt_sub_random, 1000, LW(0))
                ADD(LW(0), 3000)
                WAIT_MS(LW(0))
                CALL(evt_mapobj_get_position, STRING("gor_hae"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FLY1"), LW(0), LW(1), LW(2), 0)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

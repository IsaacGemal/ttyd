#include "gra_02.h"
#include "driver/animdrv.h"
#include "driver/npcdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_urouro.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_kagemario_init_11_text_804[];
extern s32 evt_usu_kagemario_party_kill_11_data_4F98[];
static USER_FUNC(booboo_main);

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(shopmaster_pig_init)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("ブタお店の主人"), 0, -2000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(shopmaster_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_gra_17"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_gra_18"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(urouro_move)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741824)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(50.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"ブタお店の主人", 0x600, 0, shopmaster_pig_init, urouro_move, shopmaster_talk},
    {NULL},
};

EVT_BEGIN(w_bero_2_in)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero_1", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "gra_01", "e_bero1", 1, 1, 0, 0},
    {"w_bero_2", 0x1000, 0, 6, 0xF2, 0x15, -450, -1, w_bero_2_in, 0, NULL, "gra_03", "e_bero", 1, 1, 0, 0},
    {"dokan_1", 2, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_2", 1, 1, 0, 0},
    {"dokan_2", 0x1002, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_1", 1, 1, 0, 0},
    {NULL},
};

EVT_BEGIN(gra02_get_key)
    SET_VALUE(GSW(0), 184)
    RETURN
EVT_END()

EVT_BEGIN(gra02_straw_blow_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(1000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    SET_VALUE(GSW(0), 188)
    RETURN
EVT_END()

static const char* booboo_name[] = {
    "ブタお店の主人",
};

static USER_FUNC(booboo_main) {
    animPoseSetMaterialLightFlagOn(npcNameToPtr(booboo_name[0])->poseId, 0x10);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(gra_02_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_GRA1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_USU1"))
    CALL(evt_kagemario_init_11_text_804)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_item_entry, STRING("item00"), 125, -125, 0, -110, 16, GSWF(5631), 0)
    CALL(evt_item_entry, STRING("item01"), 125, 305, 0, -35, 16, GSWF(5632), 0)
    CALL_CHILD(evt_usu_kagemario_party_kill_11_data_4F98)
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_LESS(GSW(0), 184)
        CALL(evt_item_entry, STRING("key00"), 21, -30, 0, 0, 0, -1, PTR(gra02_get_key))
    END_IF
    IF_INT_LESS(GSW(0), 188)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 0, STRING("S_wara"), STRING("polySurface981_q023"), PTR(gra02_straw_blow_evt))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_wara"), 1)
        CALL(evt_hitobj_onoff, STRING("polySurface981_q023"), 0, 0)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(booboo_main)
    END_INLINE_EVENT
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

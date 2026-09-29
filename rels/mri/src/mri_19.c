#include "evt/evt_bero.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_puni_mem_init[];
extern s32 evt_puni_set_hamari[];
extern s32 evt_puni_set_yuka[];
extern s32 mri_countdown[];
extern s32 mri_puni_init[];
extern s32 puni_disp_info[];
extern s32 puni_piders_check[];

// clang-format off
extend_data(128,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(129,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 bero_entry_data[75] = {
    (u32)"dokan1", 0x00020000, 0x00000002, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x0000000A, 0x00000000, (u32)"mri_05", (u32)"dokan4",
    0x00010001, 0x00000000, 0x00000000, (u32)"dokan2",
    0x00020000, 0x00000009, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000006,
    0x00000000, (u32)"mri_06", (u32)"dokan1", 0x00010001,
    0x00000000, 0x00000000, (u32)"dokan3", 0x00020000,
    0x00000009, 0x000186A0, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x00000006, 0x00000000,
    0x00000000, (u32)"dokan4", 0x00010001, 0x00000000,
    0x00000000, (u32)"dokan4", 0x00020000, 0x00000006,
    0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, 0x0000000A, 0x00000000, 0x00000000,
    (u32)"dokan3", 0x00010001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 ueki_access_data[28] = {
    0x00000001, (u32)"A_ueki01", (u32)"S_ueki01", 0x00000000,
    0x00000001, (u32)"A_ueki02", (u32)"S_ueki02", 0x00000000,
    0x00000001, (u32)"A_ueki03", (u32)"S_ueki03", 0x00000000,
    0x00000001, (u32)"A_ueki04", (u32)"S_ueki04", 0x00000000,
    0x00000001, (u32)"A_ueki05", (u32)"S_ueki05", 0x00000000,
    0x00000001, (u32)"A_ueki08", (u32)"S_ueki08", 0x00000000,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(cloud_wind_event)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou"), STRING("a_hi"))
    CALL(evt_map_entry_airport_harbor, 0, STRING("hikou1"), STRING("a_hi1"))
    CALL(evt_hitobj_attr_onoff, 1, 1, STRING("a_hi"), 16)
    SET_VALUE(GSWF(2874), 1)
    RETURN
EVT_END()

EVT_BEGIN(mri_19_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 19, 110, -319, -1000, -1000, 0, 0)
    CALL(evt_puni_set_hamari, 25, -319, 113, 112, 108, -317, -321)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    CALL_CHILD(mri_puni_init)
    SPAWN_EVENT(puni_disp_info)
    SPAWN_EVENT(mri_countdown)
    SWITCH(GSW(0))
        CASE_BETWEEN(109, 110)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI9"))
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI1"))
    END_SWITCH
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -360, -175, 25, 0, GSWF(5545), 0)
    SET_VALUE(LW(0), PTR(ueki_access_data))
    CALL_CHILD(evt_sub_kusa_access_entry)
    CALL(evt_map_playanim, STRING("mizu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu_02"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_02"), 1, 0)
    IF_INT_EQUAL(GSWF(2874), 0)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 0, STRING("S_f2_isk"), STRING("A_f2_isk"), PTR(cloud_wind_event))
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("a_hi"), 16)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_f2_isk"), 1)
        CALL(evt_hitobj_onoff, STRING("A_f2_isk"), 1, 0)
        CALL(evt_map_entry_airport_harbor, 0, STRING("hikou"), STRING("a_hi"))
        CALL(evt_map_entry_airport_harbor, 0, STRING("hikou1"), STRING("a_hi1"))
    END_IF
    SPAWN_EVENT(puni_piders_check)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

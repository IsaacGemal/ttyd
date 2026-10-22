#include "mri_puni.h"
#include "mri_00.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin/mtx.h>
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/envdrv.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/imgdrv.h"
#include "driver/mapdrv.h"
#include "driver/offscreendrv.h"
#include "eff/eff_starstone.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "party/party_cloud.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "rel/mri/mri.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_mri_door_naname_close[];
extern s32 evt_mri_door_naname_close_pure[];
extern s32 evt_mri_door_naname_open[];
extern s32 evt_mri_door_naname_open_pure[];
extern s32 evt_mri_get16bitdata[];
extern s32 mri_puni_jump_se[];
extern s32 mri_puni_walk_se[];
extern s32 mri_waterfall_effect[];

// Work entry for the "poof" cloud effect (local layout, not in a header yet).
static CloudEntry* cloud;
static s8 cloud_once_flag;

static void _img_alloc_capture(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);
static void _mapsetofs_all(CloudEntry* cp);
static void _mapclsofs_all(CloudEntry* cp);
static USER_FUNC(evt_cloud_main);
static USER_FUNC(yami_view);
static USER_FUNC(anim_tevcallback);
static USER_FUNC(starstone_entry);
static USER_FUNC(starstone_set_pos);
static USER_FUNC(starstone_set_ry);
static USER_FUNC(sysflag);
static s32 guide_talk[29];
BeroEntry bero_lostitem[1];

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 puni_rand_position[98] = {
    0x0053000A, 0x0021001F, 0x00B00076, 0xFF9E0031,
    0x00940093, 0x00A0003B, 0x00C3008E, 0x00DF002D,
    0x000AFFF4, 0xFFBB0003, 0xFFCEFFF3, 0x0072001A,
    0x00AE004A, 0xFFC3003E, 0x00C3005A, 0x00D90042,
    0xFFF7FFF5, 0x00D9006B, 0x008C003A, 0xFFF9002E,
    0x009D0080, 0xFFAF003C, 0x009D0014, 0xFFC30018,
    0x00B50037, 0xFFD10030, 0x00740054, 0x00CD00B5,
    0x001A0033, 0x006FFFF0, 0x0038003F, 0xFF88000F,
    0xFFBD002B, 0x0078003F, 0x0046001A, 0x004DFFDF,
    0xFFFF0016, 0xFF8B0027, 0x00AC0096, 0x00C4000F,
    0xFFEC001D, 0xFF990019, 0xFFAB0022, 0x0057002D,
    0x00B00062, 0xFFA9FFFA, 0x00D70056, 0x0080002A,
    0x00CC0033, 0x005CFFED, 0xFFCF0007, 0xFFBBFFF0,
    0xFFAA000E, 0xFF8D003B, 0x006C002F, 0x00670005,
    0x00920003, 0x000E0007, 0x009B0050, 0x00B700AB,
    0x00A300B8, 0xFFE50032, 0x009D00A6, 0x0082FFF6,
    0x00610040, 0x0087004D, 0x005E001A, 0xFF78002F,
    0x00D2001E, 0x004C0040, 0x00B50001, 0x009C0064,
    0x002E002E, 0xFF9E0047, 0x00840061, 0x0043002E,
    0x00D5007E, 0x00320016, 0x000D0024, 0x00A80028,
    0x00C100C5, 0x007A0008, 0x00890016, 0x00940027,
    0x00C30046, 0x0021000A, 0xFFD7001B, 0x0007003B,
    0x00A3FFF9, 0x00AF00CF, 0xFFE3FFF6, 0x008D0073,
    0x00B00015, 0x00BE0024, 0xFF980004, 0x00250045,
    0x00C5006E, 0x00D400A2,
};

EVT_BEGIN(mario_see_you_init)
    SET_VALUE(LW(10), 5)
    SET_VALUE(LW(11), 0)
    SET_INT_READ_ADDR(PTR(puni_rand_position))
    LOOP(0)
        CALL(evt_mri_get16bitdata, PTR(puni_rand_position), LW(11), LW(0))
        ADD(LW(11), 1)
        CALL(evt_mri_get16bitdata, PTR(puni_rand_position), LW(11), LW(2))
        ADD(LW(11), 1)
        CALL(evt_puni_init_puni, LW(10), 30, LW(0), 0, LW(2), 0)
        CALL(evt_puni_flag_onoff, LW(10), 0, 8388608)
        CALL(evt_puni_pera_onoff, LW(10), 1)
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 spline_dt[24] = {
    0x430C0000, 0x00000000, 0xC30C0000, 0xC184EDFA,
    0x00000000, 0x4168AF19, 0xC32AFC4E, 0x00000000,
    0x414BF954, 0xC38094B0, 0x00000000, 0xC2232515,
    0xC340C10A, 0x421D3332, 0xC3014B8C, 0xC300EAB8,
    0x42953333, 0xC3273092, 0xC245C0DC, 0x430FF331,
    0xC34D0000, 0x41200000, 0x431B0000, 0xC35C0000,
};

EVT_BEGIN(seal_bigtree)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_move_beside_mario, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e03_00"), 0, STRING("me"))
    CALL(evt_cam3d_evt_set, FLOAT(142.2f), FLOAT(160.1f), FLOAT(440.1f), FLOAT(142.2f), FLOAT(64.6f), FLOAT(-50.8f), 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg2_mri_e03_01"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 140, 90, 193, 140, 45, -154, 750, 11)
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイド"), 140, -140, 1000, 0, 0)
    DELETE_EVENT(LW(9))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e03_02"), 0, STRING("ガイド"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, FLOAT(113.3f), FLOAT(83.6f), FLOAT(143.5f), FLOAT(113.3f), FLOAT(45.0f), FLOAT(-154.0f), 750, 11)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_move_pos, 0, 110, -120, 1000)
    CALL(evt_party_set_dir, 0, 90, -1)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg2_mri_e03_03"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, FLOAT(140.9f), FLOAT(83.6f), FLOAT(143.5f), FLOAT(140.9f), FLOAT(45.0f), FLOAT(-154.0f), 500, 11)
    WAIT_MS(700)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e03_04"), 0, STRING("ガイド"))
    WAIT_MS(300)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 131088)
    CALL(evt_npc_status_onoff, 1, STRING("ガイド"), 2)
    CALL(evt_npc_change_interrupt, STRING("ガイド"), 6, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_spline_init, 0, 0, PTR(spline_dt), 8, 50, 0)
        LABEL(0)
        CALL(evt_sub_spline_get_value, LW(3), LW(4), LW(5), LW(6))
        SET_VALUE(LW(0), STRING("ガイド"))
        SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
        CALL(evt_npc_move_position, STRING("ガイド"), LW(3), LW(5), 160, 0, 0)
        DELETE_EVENT(LW(9))
        IF_INT_EQUAL(LW(6), 1)
            GOTO(0)
        END_IF
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741856)
        CALL(evt_npc_change_interrupt, STRING("ガイド"), 6, PTR(guide_talk))
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_party_run, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 75)
    SET_VALUE(GSWF(2881), 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(seal_bigtree2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e03_06"), 0, STRING("me"))
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg2_mri_e03_07"))
    WAIT_MS(300)
    SET_VALUE(LW(0), STRING("me"))
    SPAWN_EVENT(mri_puni_jump_se)
    CALL(evt_npc_jump_position_nohit, STRING("me"), 10, 155, -230, 200, 20)
    SET_VALUE(LW(0), STRING("me"))
    SPAWN_EVENT(mri_puni_jump_se)
    CALL(evt_npc_jump_position_nohit, STRING("me"), 10, 155, -230, 200, 20)
    CALL(evt_cam3d_evt_set, 5, 277, 176, 5, 200, -216, 500, 11)
    WAIT_MS(700)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e03_08"), 0, STRING("me"))
    WAIT_MS(200)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e03_09"), 0, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_party_pos, 1, 10, LW(0), LW(1), LW(2))
    CALL(evt_party_run, 1)
    CALL(evt_party_run, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 76)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_bero_overwrite, STRING("w_bero"), PTR(bero_lostitem))
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

static EVT_BEGIN(guide_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(73, 74)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 16777216)
        CALL(evt_npc_set_position, STRING("me"), 135, 0, -70)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CASE_EQUAL(75)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 16777216)
        CALL(evt_npc_set_position, STRING("me"), 10, 155, -220)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CASE_EQUAL(88)
        IF_INT_EQUAL(GSWF(2826), 0)
            CALL(evt_npc_set_position, STRING("me"), 140, 0, -140)
            CALL(evt_npc_set_ry, STRING("me"), 90)
        END_IF
        CASE_GREATER_OR_EQUAL(117)
        IF_INT_EQUAL(GSWF(2884), 0)
            CALL(evt_npc_set_position, STRING("me"), -34, 0, 177)
            CALL(evt_npc_set_ry, STRING("me"), 270)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(guide_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(74)
        SPAWN_EVENT(seal_bigtree)
        CASE_EQUAL(75)
        SPAWN_EVENT(seal_bigtree2)
        CASE_EQUAL(88)
        CALL(evt_msg_print, 0, STRING("stg2_mri_e10_05"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(118)
        CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_07"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(rival_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(117)
        IF_INT_EQUAL(GSWF(2884), 0)
            CALL(evt_npc_set_position, STRING("me"), 38, 0, 143)
            CALL(evt_npc_set_ry, STRING("me"), 270)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(rival_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_09"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(chibitoge_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(117)
        IF_INT_EQUAL(GSWF(2884), 0)
            IF_INT_NOT_EQUAL(GSWF(2878), 0)
                CALL(evt_npc_set_position, STRING("me"), 50, 0, 191)
                CALL(evt_npc_set_ry, STRING("me"), 270)
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(chibitoge_talk)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 200, 18)
    RETURN
EVT_END()

static EVT_BEGIN(sister_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(117)
        IF_INT_EQUAL(GSWF(2884), 0)
            CALL(evt_npc_set_position, STRING("me"), -10, 0, 121)
            CALL(evt_npc_set_ry, STRING("me"), 270)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(sister_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_08"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(elder_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(117)
        IF_INT_EQUAL(GSWF(2884), 0)
            CALL(evt_npc_set_position, STRING("me"), 10, 0, 214)
            CALL(evt_npc_set_ry, STRING("me"), 270)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(elder_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_10"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(fabio_init)
    IF_INT_NOT_EQUAL(GSW(0), 400)
        IF_INT_EQUAL(GSWF(5402), 1)
            CALL(evt_npc_set_position, STRING("me"), -375, FLOAT(0.56f), -134)
            CALL(evt_npc_set_ry, STRING("me"), 90)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(fabio_talk)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("irai_28_006"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("irai_28_006_1"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(odoriko_1_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_07"), 0, STRING("踊り子１"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_08"), 0, STRING("踊り子２"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_09"), 0, STRING("踊り子３"))
    RETURN
EVT_END()

static EVT_BEGIN(odoriko_1_init)
    SET_VALUE(LW(0), STRING("踊り子１"))
    CALL(evt_npc_entry, LW(0), STRING("c_dance_r"))
    CALL(evt_npc_set_tribe, LW(0), STRING("踊り子"))
    CALL(evt_npc_change_interrupt, LW(0), 6, PTR(odoriko_1_talk))
    CALL(evt_npc_flag_onoff, 1, LW(0), 525824)
    CALL(evt_npc_set_ry, LW(0), 270)
    CALL(evt_npc_set_position, LW(0), 90, 0, 400)
    RETURN
EVT_END()

static EVT_BEGIN(odoriko_2_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_07"), 0, STRING("踊り子１"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_08"), 0, STRING("踊り子２"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_09"), 0, STRING("踊り子３"))
    RETURN
EVT_END()

static EVT_BEGIN(odoriko_2_init)
    SET_VALUE(LW(0), STRING("踊り子２"))
    CALL(evt_npc_entry, LW(0), STRING("c_dance_p"))
    CALL(evt_npc_set_tribe, LW(0), STRING("踊り子ピンク"))
    CALL(evt_npc_change_interrupt, LW(0), 6, PTR(odoriko_2_talk))
    CALL(evt_npc_flag_onoff, 1, LW(0), 525824)
    CALL(evt_npc_set_ry, LW(0), 270)
    CALL(evt_npc_set_position, LW(0), 120, 0, 370)
    RETURN
EVT_END()

static EVT_BEGIN(odoriko_3_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_07"), 0, STRING("踊り子１"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_08"), 0, STRING("踊り子２"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_00_09"), 0, STRING("踊り子３"))
    RETURN
EVT_END()

static EVT_BEGIN(odoriko_3_init)
    SET_VALUE(LW(0), STRING("踊り子３"))
    CALL(evt_npc_entry, LW(0), STRING("c_dance_k"))
    CALL(evt_npc_set_tribe, LW(0), STRING("踊り子黄緑"))
    CALL(evt_npc_change_interrupt, LW(0), 6, PTR(odoriko_3_talk))
    CALL(evt_npc_flag_onoff, 1, LW(0), 525824)
    CALL(evt_npc_set_ry, LW(0), 270)
    CALL(evt_npc_set_position, LW(0), 150, 0, 420)
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[161] = {
    (u32)"ガイド", 0x00080600, 0x00000000, (u32)&guide_init,
    0x00000000, (u32)&guide_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ライバル",
    0x00080600, 0x00000000, (u32)&rival_init, 0x00000000,
    (u32)&rival_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ちびトゲ", 0x00080600,
    0x00000000, (u32)&chibitoge_init, 0x00000000, (u32)&chibitoge_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ガイド妹", 0x00080600, 0x00000000,
    (u32)&sister_init, 0x00000000, (u32)&sister_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"プニ族長老", 0x00080600, 0x00000000, (u32)&elder_init,
    0x00000000, (u32)&elder_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ファビオ",
    0x00080600, 0x00000000, (u32)&fabio_init, 0x00000000,
    (u32)&fabio_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(bero_01_doa_close)
    SET_VALUE(LW(10), STRING("S_doa"))
    SPAWN_EVENT(evt_mri_door_naname_close)
    RETURN
EVT_END()

static EVT_BEGIN(bero_01_doa_open)
    SET_VALUE(LW(10), STRING("S_doa"))
    SPAWN_EVENT(evt_mri_door_naname_open)
    RETURN
EVT_END()

static EVT_BEGIN(first_time_w_bero)
    IF_INT_EQUAL(GSW(0), 73)
        SET_VALUE(GF(0), 1)
        WAIT_WHILE(GF(0))
        SET_VALUE(LW(0), 1)
        RETURN
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(out_ne_bero)
    IF_INT_EQUAL(GSWF(2826), 0)
        WAIT_MS(50)
        CALL(evt_npc_set_ry, STRING("ガイド"), 90)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
            CALL(evt_npc_set_position, STRING("ガイド"), 0, -1000, 0)
            CALL(evt_mario_set_party_pos, 1, 10, LW(0), LW(1), LW(2))
            CALL(evt_party_set_dir, 1, 90, 0)
        END_INLINE_EVENT
        SET_VALUE(GSWF(2826), 1)
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(out_w_bero)
    CALL(evt_fade_set_mapchange_type, 1, 2, 500, 1, 500)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(out_dokan_1)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 112)
        IF_INT_EQUAL(GSWF(2884), 0)
            SET_VALUE(GSWF(2884), 1)
        END_IF
        IF_INT_EQUAL(GSWF(2840), 1)
            SET_VALUE(GSWF(2840), 0)
        END_IF
        IF_INT_EQUAL(GSWF(2827), 1)
            SET_VALUE(GSWF(2827), 0)
        END_IF
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

BeroEntry bero_lostitem[1] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, out_w_bero, "win_03", "", 1, 1},
};

static BeroEntry bero_entry_data[4] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, first_time_w_bero, -1, NULL, "win_01", "n_bero", 1, 1},
    {"ne_bero", 1, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, out_ne_bero, "mri_01", "w_bero", 4, 4, bero_01_doa_close, bero_01_doa_open},
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, out_dokan_1, "tik_16", "dokan_1", 1, 1},
    {NULL},
};

EVT_BEGIN(first_bigtree)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_cam3d_evt_set, -700, 500, 916, 275, 500, -195, 0, 0)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_seq_wait, 2)
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_telop_entry, 5)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 275, 250, 1795, 275, 280, -195, 5000, 11)
    WAIT_MS(6000)
    CALL(evt_cam3d_evt_set, FLOAT(-601.1f), FLOAT(208.3f), FLOAT(720.9f), FLOAT(-601.1f), FLOAT(65.1f), FLOAT(-15.7f), 1000, 11)
    WAIT_MS(550)
    CALL(evt_party_run, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_mov_pos2, -618, LW(2), FLOAT(120.0f))
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GF(0), 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_snd_bgm_scope, 0, 0)
    SET_VALUE(GSW(0), 74)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static const f32 zero_capturemap = 0.0f;
static char* capturemap[4] = {
    "S_non", "S_ha_10_07", "S_kesu", (char*)&zero_capturemap,
};

static void _img_alloc_capture(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7) {
    EventEntry event;
    s32 args[10];

    event.args = args;
    args[0] = a0;
    args[1] = a1;
    args[2] = a2;
    args[3] = a3;
    args[4] = a4;
    args[5] = a5;
    args[6] = a6;
    args[7] = a7;
    evt_img_alloc_capture(&event, 1);
}

s32 _ofs_get_boundingbox(const char* name, s32* a, s32* b, s32* c, s32* d) {
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;
    s32 id = offscreenNameToId(name);

    if (offscreenGetBoundingBox_BeforeFrame(id, &left, &top, &right, &bottom) == 0) {
        return 0;
    }
    *a = left;
    *b = top;
    *c = right - left + 50;
    *d = bottom - top;
    if (*a + *c >= 0x260) {
        *c = 0x260 - *a;
    }
    return 2;
}

static void _mapsetofs_all(CloudEntry* cp) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (cp->mapGrpName[i][0] == 0) {
            break;
        }
        mapGrpSetOffScreen((const char*)cp->mapGrpName[i], cp->offName);
    }
}

static void _mapclsofs_all(CloudEntry* cp) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (cp->mapGrpName[i][0] == 0) {
            break;
        }
        mapGrpClearOffScreen((const char*)cp->mapGrpName[i]);
    }
}

static USER_FUNC(evt_cloud_main) {
    CloudEntry* cp = (CloudEntry*)event->lwData[0];
    s32 inBattle = (gp->inBattle != 0);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(0);
    ImageEntry* img;
    HitEntry* hit;
    HitEntry* h0;
    HitEntry* h1;
    HitEntry* h2;
    HitEntry* h3;

    if (isFirstCall) {
        return EVT_RETURN_BLOCK;
    }
    if (party == NULL) {
        return EVT_RETURN_BLOCK;
    }

    h0 = (HitEntry*)cloudGetHitBreathout(0);
    h1 = (HitEntry*)cloudGetHitBreathout(1);
    h2 = (HitEntry*)cloudGetHitBreathout(2);
    h3 = (HitEntry*)cloudGetHitBreathout(3);

    switch (cp->mode) {
    case 0:
        if (cloud_once_flag == 1) {
            break;
        }
        hit = hitNameToPtr(cp->hitName);
        if (h0 == hit || h1 == hit || h2 == hit || h3 == hit) {
            cp->mode++;
        }
        if (cp->breathCount > 0) {
            cp->breathCount--;
        }
        break;
    case 1: {
        f32 angle = mario->dispReqDir;

        if (angle > 90.0f && angle <= 270.0f) {
            strcpy(cp->animName, "P_oof_cloud_2");
        } else {
            strcpy(cp->animName, "P_oof_cloud_1");
        }
        if (animGroupBaseAsync(cp->animName, inBattle, NULL)) {
            cloud_once_flag = 1;
            animPaperPoseEntry(cp->animName, inBattle);
            offscreenEntry(cp->offName);
            _mapsetofs_all(cp);
            cp->mode++;
        }
        break;
    }
    case 2:
        imgEntry(cp->imgName, inBattle);
        img = imgNameToPtr(cp->imgName, inBattle);
        img->poseId = animPaperPoseGetId(cp->animName, inBattle);
        (img->startTime) = animTimeGetTime(inBattle);
        imgClearVirtualPoint(img);
        imgSetShadow(img, 2);
        img->flags |= 2;
        cp->mode = 0xDE;
        break;
    case 0xDE:
        if (_ofs_get_boundingbox(cp->offName, &cp->left, &cp->top, &cp->width, &cp->height) != 0) {
            _img_alloc_capture((s32)cp->imgName, 0, 0, 1, cp->left, cp->top, cp->width, cp->height);
            cp->mode = 0xDF;
        }
        break;
    case 0xDF:
        img = imgNameToPtr(cp->imgName, inBattle);
        img->flags &= ~2;
        cp->mode = 3;
        // fallthrough
    case 3:
        img = imgNameToPtr(cp->imgName, inBattle);
        img->animName = "A_1";
        (img->startTime) = animTimeGetTime(inBattle);
        img->flags &= ~2;
        cp->waitTimer = 0x1E;
        cp->lastLoopTimes = 0.0f;
        cp->mode++;
        break;
    case 4:
        img = imgNameToPtr(cp->imgName, inBattle);
        if (cp->lastLoopTimes == 0.0f || 1.0f + cp->lastLoopTimes < img->loopTimes) {
            psndSFXOn_3D("SFX_OFF_CLAUD_WIND2", &mario->position);
            cp->lastLoopTimes = img->loopTimes;
        }
        if (mario->currMotionId != 0x1B) {
            cp->mode = 0xA;
            break;
        }
        hit = hitNameToPtr(cp->hitName);
        if (h0 != hit && h1 != hit && h2 != hit && h3 != hit) {
            if (cp->waitTimer == 0) {
                cp->mode = 0xA;
            } else {
                cp->waitTimer--;
                cp->breathCount += 2;
            }
        } else {
            cp->waitTimer = 0x1E;
            cp->breathCount += 2;
            if (cp->breathCount > cp->breathLimit) {
                cp->mode = 0x14;
            }
        }
        break;
    case 0xA:
        cp->mode = 0x63;
        break;
    case 0x14:
        psndSFXOn_3D("SFX_OFF_CLAUD_WIND3", &mario->position);
        if (cp->endScript != NULL) {
            evtEntry(cp->endScript, 0, 0);
        }
        img = imgNameToPtr(cp->imgName, inBattle);
        img->animName = "A_2";
        (img->startTime) = animTimeGetTime(inBattle);
        cp->waitTimer = 0x3C;
        cp->mode++;
        break;
    case 0x15:
        if (cp->waitTimer != 0) {
            cp->waitTimer--;
        } else {
            cp->waitTimer = 0x1E;
            cp->breathCount += 2;
            cp->mode = 0x64;
        }
        break;
    case 0x63:
    case 0x64:
        img = imgNameToPtr(cp->imgName, inBattle);
        _mapclsofs_all(cp);
        animPaperPoseRelease(img->poseId);
        imgFreeCapture(img, 0);
        imgSetShadow(img, 0);
        imgRelease(img, inBattle);
        offscreenDelete(cp->offName);
        cloud_once_flag = 0;
        if (cp->mode == 0x63) {
            cp->mode = 0;
            break;
        }
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(evt_cloud_main_evt)
    CALL(evt_cloud_main)
    RETURN
EVT_END()

USER_FUNC(evt_mri_cloud_init) {
    s32 i;

    cloud = _mapAlloc(0x1B00);
    for (i = 0; i < 16; i++) {
        cloud[i].flags = 0;
        sprintf(cloud[i].imgName, "i_name_%1d", i);
        sprintf(cloud[i].offName, "o_name_%1d", i);
    }
    cloud_once_flag = 0;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mri_cloud_ent) {
    s32* args = event->args;
    s8** a;
    s8* b;
    void* c;
    CloudEntry* cp;
    s32 i, j;
    a = (s8**)evtGetValue(event, args[0]);
    b = (s8*)evtGetValue(event, args[1]);
    c = (void*)evtGetValue(event, args[2]);
    for (j = 0; j < 16; j++) {
        if (cloud[j].flags == 0) {
            break;
        }
    }
    if (j >= 16) {
        return EVT_RETURN_DONE;
    }

    cp = &cloud[j];
    cp->flags = 1;
    strcpy(cp->animName, "P_oof_cloud_1");
    for (i = 0; i < 8; i++) {
        if (a[i][0] == 0) {
            break;
        }
        strcpy((char*)cp->mapGrpName[i], (const char*)a[i]);
    }
    strcpy(cp->hitName, (const char*)b);
    cp->endScript = c;
    cp->breathLimit = 0xB4;
    cp->breathCount = 0;
    cp->mode = 0;
    cp->waitTimer = 0;
    cp->event = evtEntry(evt_cloud_main_evt, 0, 0);
    cp->event->lwData[0] = (s32)cp;
    return EVT_RETURN_DONE;
}

// TODO: type
u32 guide_move_data[15] = {
    0x4238364F, 0x4311800F, 0xC311B235, 0x42A84F42,
    0x4311800F, 0xC326B4D1, 0x42C75095, 0x430F0000,
    0xC33C55DD, 0x4302BB65, 0x431BA668, 0xC339B593,
    0x432029B9, 0x43257345, 0xC3386BE5,
};

EVT_BEGIN(open_bigtree)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_non"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tree_futa"), 1)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_mov_pos2, -12, -206, FLOAT(120.0f))
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos2, 0, -44, -206, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_party_jump_pos, 1, 20, 154, -220, 500, 0, -1000)
    CALL(evt_eff_fukidashi, 4, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_cam3d_evt_set, 21, 288, 213, 21, 205, -216, 500, 11)
    WAIT_MS(1200)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e10_00"), 0, STRING("extparty"))
    CALL(evt_party_get_pos, 1, LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_mario_clear_party, 1)
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT(mri_puni_jump_se)
    CALL(evt_npc_jump_position_nohit, STRING("ガイド"), 54, 143, -202, 200, 20)
    CALL(evt_cam3d_evt_set, 187, 288, 213, 187, 205, -216, 2000, 11)
    CALL(evt_npc_set_anim, STRING("ガイド"), STRING("W_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073741856)
    CALL(evt_sub_spline_init, 1, 11, PTR(guide_move_data), 5, 0, 2000)
    LOOP(0)
        CALL(evt_sub_spline_get_value, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_npc_set_position, STRING("ガイド"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(3), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(200)
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT(mri_puni_jump_se)
    CALL(evt_npc_jump_position_nohit, STRING("ガイド"), 191, 192, -184, 200, 20)
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイド"), 216, -214, 0, FLOAT(120.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_npc_flag_onoff, 0, STRING("ガイド"), 1073741856)
    CALL(evt_cam3d_evt_set, -26, 275, 213, -26, 192, -216, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e10_01"), 0, STRING("party"))
    CALL(evt_mario_set_dir, 270, 200, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e10_01_01"), 0, STRING("party"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_cam3d_evt_set, LW(0), 226, 30, LW(0), 178, -216, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e10_01_02"), 0, STRING("party"))
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("ガイド"), 226, 0, -225)
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073741856)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e10_02"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, 140, 90, 193, 140, 45, -154, 500, 11)
    WAIT_MS(1000)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_hit_get_position, STRING("ne_bero"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOOR_UNLOCK"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOOR_LAMP"), LW(0), LW(1), LW(2), 0)
    CALL(evt_map_playanim, STRING("light"), 0, 0)
    WAIT_MS(1000)
    SET_VALUE(LW(10), STRING("S_doa"))
    CALL_CHILD(evt_mri_door_naname_open_pure)
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        SET_VALUE(LW(10), STRING("S_doa"))
        SPAWN_EVENT(evt_mri_door_naname_close_pure)
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイド"), 140, -140, 0, FLOAT(120.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e10_03"), 0, STRING("ガイド"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -26, 275, 213, -26, 192, -216, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e10_04"), 0, STRING("party"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_flag_onoff, 0, STRING("ガイド"), 1073741856)
    SET_VALUE(GSW(0), 88)
    SET_VALUE(LW(0), STRING("ne_bero"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(mario_see_you)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, -85, 0, 185)
    CALL(evt_party_set_pos, 0, -125, 0, 185)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, -1)
    CALL(evt_cam3d_evt_set, 180, 130, 567, 180, 44, 127, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_puni_wait_systemmode, 2)
    CALL_CHILD(mario_see_you_init)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_seq_wait, 2)
    CALL(evt_fade_end_wait)
    CALL(evt_cam3d_evt_set, -29, 130, 567, -29, 44, 127, 3000, 0)
    WAIT_MS(3000)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_01"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, 9, 68, 429, 9, 12, 134, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_02"), 0, STRING("プニ族長老"))
    CALL(evt_cam3d_evt_set, -11, 68, 333, -11, 12, 38, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_03"), 0, STRING("ガイド妹"))
    CALL(evt_cam3d_evt_set, -35, 73, 400, -35, 17, 105, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_04"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, -65, 90, 575, -65, 30, 28, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg2_mri_e26_00_05"))
    CALL(evt_cam3d_evt_set, -29, 130, 567, -29, 44, 127, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e26_00_06"), 0, STRING("ガイド"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_off, 1000, 11)
    SET_VALUE(GSW(0), 118)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

const Vec vec3_mri_00020a58 = {0.0f, 100.0f, 0.0f};
const Vec vec3_mri_00020a64 = {0.0f, 1.0f, 0.0f};

static USER_FUNC(yami_view) {
    Vec camPos;
    Vec up;
    Vec target;
    Mtx mtx;
    f32 value = evtGetFloat(event, event->args[0]);

    target = (Vec){0.0f, 0.0f, 0.0f};
    target.x = value;
    up = vec3_mri_00020a64;
    camPos = vec3_mri_00020a58;
    camPos.x = value + -100.0f;
    C_MTXLookAt(mtx, &camPos, &up, &target);
    envSetYamiView(mtx);
    return EVT_RETURN_DONE;
}

static USER_FUNC(anim_tevcallback) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    NpcEntry* npc = npcNameToPtr_NoAssert((const char*)name);

    if (npc == NULL) {
        return EVT_RETURN_DONE;
    }
    animPoseSetMaterialFlagOn(npc->poseId, (flag & 0xF) << 24);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_puni_pos) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 x = (index % 15) * 20 + rand() % 10;
    s32 y = (x -= 280);
    s32 z = 180 - ((index / 15) * 20 + rand() % 30 - 15);

    evtSetValue(event, args[1], y);
    evtSetValue(event, args[2], z);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_entry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);

    effSetName(effStarStoneEntry(1, x, y, z, 1.0f), (const char*)name);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_pos) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    ((effStarStoneData*)eff->userdata)->x = x;
    ((effStarStoneData*)eff->userdata)->y = y;
    ((effStarStoneData*)eff->userdata)->z = z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_ry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 ry = evtGetFloat(event, args[1]);
    effStarStoneData* data = effNameToPtr((const char*)name)->userdata;

    data->rotY = ry;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(puni_set)
    SET_VALUE(LW(0), 5)
    SET_VALUE(LW(1), 0)
    LOOP(0)
        CALL(get_puni_pos, LW(1), LW(2), LW(3))
        CALL(evt_sub_random, 1, LW(5))
        IF_INT_EQUAL(LW(5), 0)
            SET_VALUE(LW(5), 90)
        ELSE
            SET_VALUE(LW(5), 270)
        END_IF
        CALL(evt_puni_init_puni, LW(0), 30, LW(2), 0, LW(3), 0)
        CALL(evt_puni_set_dir, LW(0), LW(5))
        CALL(evt_puni_flag_onoff, LW(0), 0, 8388608)
        ADD(LW(0), 1)
        ADD(LW(1), 1)
        IF_INT_EQUAL(LW(0), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(sekai_yami_npc)
    LOOP(0)
        CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
        IF_INT_GREATER_OR_EQUAL(GW(0), LW(1))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_get_reglid, LW(0), LW(1))
    DELETE_EVENT(LW(1))
    CALL(evt_eff_fukidashi, 1, STRING(""), LW(0), 0, 0, 0, 0, 0, 0, 0, 48)
    WAIT_MS(1000)
    LOOP(0)
        CALL(evt_sub_random, 400, LW(1))
        ADD(LW(1), 100)
        WAIT_MS(LW(1))
        CALL(evt_npc_reverse_ry, LW(0))
        WAIT_FRAMES(1)
        CALL(evt_npc_wait_pera, LW(0))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(sekai_yami_puni)
    LOOP(0)
        CALL(evt_puni_get_position, LW(0), LW(1), LW(2), LW(3))
        IF_INT_GREATER_OR_EQUAL(GW(0), LW(1))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_puni_get_position, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_eff_fukidashi, 2, STRING(""), 0, 0, 0, LW(1), LW(2), LW(3), 0, 0, 48)
    WAIT_MS(1000)
    LOOP(0)
        CALL(evt_sub_random, 400, LW(1))
        ADD(LW(1), 100)
        WAIT_MS(LW(1))
        CALL(evt_puni_get_dir, LW(0), LW(1))
        IF_INT_LESS(LW(1), 180)
            CALL(evt_puni_set_dir, LW(0), 270)
        ELSE
            CALL(evt_puni_set_dir, LW(0), 90)
        END_IF
        WAIT_MS(500)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(sekai_yami_evt)
    CALL(evt_puni_wait_systemmode, 2)
    SPAWN_EVENT(puni_set)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ライバル"), -100, 0, 225)
    CALL(evt_npc_set_ry, STRING("ライバル"), 270)
    CALL(evt_npc_set_position, STRING("ガイド"), -150, 0, 250)
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    CALL(evt_npc_set_position, STRING("ガイド妹"), -175, 0, 275)
    CALL(evt_npc_set_ry, STRING("ガイド妹"), 270)
    CALL(evt_npc_set_position, STRING("プニ族長老"), -120, 0, 300)
    CALL(evt_npc_set_ry, STRING("プニ族長老"), 270)
    CALL(evt_cam3d_evt_set, -150, 258, 1648, -150, 180, 150, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_sub_intpl_msec_init, 11, 255, 64, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_bg_set_color, LW(0), LW(0), LW(0), 255)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(2000)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback, STRING("ライバル"), 1)
    CALL(anim_tevcallback, STRING("ガイド"), 1)
    CALL(anim_tevcallback, STRING("ガイド妹"), 1)
    CALL(anim_tevcallback, STRING("プニ族長老"), 1)
    SET_VALUE(LSWF(3), 1)
    SET_VALUE(GW(0), -800)
    SET_VALUE(LW(0), STRING("ライバル"))
    SPAWN_EVENT(sekai_yami_npc)
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT_GET_ID(PTR(sekai_yami_npc), GW(1))
    SET_VALUE(LW(0), STRING("ガイド妹"))
    SPAWN_EVENT_GET_ID(PTR(sekai_yami_npc), GW(2))
    SET_VALUE(LW(0), STRING("プニ族長老"))
    SPAWN_EVENT(sekai_yami_npc)
    SET_VALUE(LW(0), 6)
    SPAWN_EVENT(sekai_yami_puni)
    SET_VALUE(LW(0), 39)
    SPAWN_EVENT(sekai_yami_puni)
    SET_VALUE(LW(0), 88)
    SPAWN_EVENT(sekai_yami_puni)
    SET_VALUE(LW(0), 46)
    SPAWN_EVENT(sekai_yami_puni)
    SET_VALUE(LW(0), 64)
    SPAWN_EVENT(sekai_yami_puni)
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S_yuka_hika_02"), 255, 255, 255, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka_hika_02"), 1)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_STG8_WORLD_DARK1"), 0)
    CALL(evt_sub_intpl_msec_init, 11, -800, 800, 5000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(yami_view, LW(0))
        SET_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -150, 74, 650, -150, 40, 150, 1000, 11)
    WAIT_MS(1000)
    DELETE_EVENT(GW(1))
    DELETE_EVENT(GW(2))
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    CALL(evt_npc_set_ry, STRING("ガイド妹"), 270)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ガイド"))
    CALL(evt_npc_wait_pera, STRING("ガイド妹"))
    CALL(evt_msg_print, 0, STRING("stg8_dark_02"), 0, STRING("ガイド妹"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_dark_02_01"), 0, STRING("ガイド"))
    WAIT_MS(1000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("tou_01"), STRING("sekai_yami1"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(minnnanokoe_evt)
    CALL(evt_puni_wait_systemmode, 2)
    SPAWN_EVENT(puni_set)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガイド"), -150, 0, 250)
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    CALL(evt_npc_set_position, STRING("ガイド妹"), -175, 0, 275)
    CALL(evt_npc_set_ry, STRING("ガイド妹"), 90)
    CALL(evt_npc_set_position, STRING("プニ族長老"), -120, 0, 300)
    CALL(evt_npc_set_ry, STRING("プニ族長老"), 270)
    CALL(evt_npc_set_position, STRING("ライバル"), -100, 0, 225)
    CALL(evt_npc_set_ry, STRING("ライバル"), 270)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka_hika_02"), 1)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 256)
    CALL(anim_tevcallback, STRING("ライバル"), 1)
    CALL(anim_tevcallback, STRING("ガイド"), 1)
    CALL(anim_tevcallback, STRING("ガイド妹"), 1)
    CALL(anim_tevcallback, STRING("プニ族長老"), 1)
    SET_VALUE(LSWF(3), 1)
    CALL(yami_view, 800)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(evt_cam3d_evt_set, -150, 258, 1648, -150, 180, 150, 0, 11)
    CALL(starstone_entry, STRING("starstone"), -150, 660, 200)
    CALL(evt_seq_wait, 2)
    WAIT_MS(500)
    SET_VALUE(GW(0), 660)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(starstone_set_pos, STRING("starstone"), -150, GW(0), 200)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(starstone_set_ry, STRING("starstone"), LW(0))
            SUBTRACT_FLOAT(LW(0), FLOAT(3.0f))
            IF_INT_LESS(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 660, 60, 4500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    SET_VALUE(GW(0), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 6000, 6500, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
            CALL(evt_sub_intpl_msec_init, 11, 6500, 6000, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(2)
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -150, 74, 650, -150, 40, 150, 800, 11)
    WAIT_MS(800)
    CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド"), LW(0), LW(1), LW(2), 300, 10)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド"), LW(0), LW(1), LW(2), 300, 10)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_10"), 0, STRING("ガイド"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_11"), 0, STRING("プニ族長老"))
    CALL(evt_npc_set_ry, STRING("ガイド"), 90)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ガイド"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_12"), 0, STRING("ガイド"))
    CALL(evt_npc_get_position, STRING("ガイド妹"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド妹"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド妹"), LW(0), LW(1), LW(2), 300, 10)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド妹"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド妹"), LW(0), LW(1), LW(2), 300, 10)
    CALL(evt_cam3d_evt_set, -150, 62, 550, -150, 35, 150, 800, 11)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_13"), 0, STRING("ガイド妹"))
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ガイド"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_14"), 0, STRING("ガイド"))
    CALL(evt_npc_get_position, STRING("ガイド妹"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド妹"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド妹"), LW(0), LW(1), LW(2), 300, 10)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド妹"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド妹"), LW(0), LW(1), LW(2), 300, 10)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_15"), 0, STRING("ガイド妹"))
    CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド"), LW(0), LW(1), LW(2), 300, 10)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガイド"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ガイド"), LW(0), LW(1), LW(2), 300, 10)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_16"), 0, STRING("ガイド"))
    CALL(evt_cam3d_evt_set, -150, 74, 650, -150, 40, 150, 800, 11)
    WAIT_MS(800)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 30)
            CALL(evt_puni_get_position, LW(0), LW(1), LW(2), LW(3))
            CALL(evt_sub_random, 10, LW(4))
            ADD(LW(4), 25)
            ADD(LW(2), LW(4))
            CALL(evt_msg_toge, 3, LW(1), LW(2), LW(3))
            BEGIN_INLINE_EVENT
                CALL(evt_msg_print, 0, STRING("stg8_las_148_17"), 0, 0)
            END_INLINE_EVENT
            WAIT_MS(300)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(4000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("tou_01"), STRING("minnnanokoe"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_switch_init)
    IF_INT_EQUAL(GSWF(1345), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_dakan"), 1)
        CALL(evt_hitobj_onoff, STRING("A_dokan"), 1, 0)
    END_IF
    RETURN
EVT_END()

static USER_FUNC(sysflag) {
    if (gp->flags & 1) {
        gp->flags &= ~1;
        fadeEntry(0x1C, 0, (GXColor){0, 0, 0, 0});
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(mri_00_init_evt)
    CALL(sysflag)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 0, 0, -310, -1000, -1000, 0, 0)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    CALL_CHILD(mri_puni_init)
    CALL_CHILD(evt_switch_init)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_cam_road_reset)
    IF_INT_EQUAL(GSW(0), 117)
        CALL(evt_mario_get_party, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_mario_set_party, 0, 1)
            CALL(evt_party_run, 0)
            CALL(evt_pouch_mario_recovery)
            CALL(evt_pouch_all_party_recovery)
        END_IF
        SPAWN_EVENT(mario_see_you)
    END_IF
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(117)
        IF_INT_EQUAL(GSWF(2884), 0)
            BEGIN_INLINE_EVENT
                CALL(evt_puni_wait_systemmode, 2)
                SPAWN_EVENT(mario_see_you_init)
            END_INLINE_EVENT
        END_IF
    END_SWITCH
    CALL(evt_map_playanim, STRING("hikari_01"), 1, 0)
    CALL(evt_map_playanim, STRING("kawa_sibu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("kawa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("kawa_02"), 1, 0)
    CALL(evt_map_playanim, STRING("neon"), 1, 0)
    SET_VALUE(LW(0), -220)
    SET_VALUE(LW(1), 1)
    SET_VALUE(LW(2), -295)
    SET_FLOAT_VALUE(LW(3), FLOAT(1.0f))
    SPAWN_EVENT(mri_waterfall_effect)
    SET_VALUE(LW(0), 531)
    SET_VALUE(LW(1), 1)
    SET_VALUE(LW(2), 118)
    SET_FLOAT_VALUE(LW(3), FLOAT(1.0f))
    SPAWN_EVENT(mri_waterfall_effect)
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami1"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_WITCH"))
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_VOICE1"))
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        IF_INT_NOT_EQUAL(GSW(0), 73)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI2"))
        ELSE
            BEGIN_INLINE_EVENT
                WAIT_MS(2000)
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
            END_INLINE_EVENT
            CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI2"))
        END_IF
    END_IF
    CALL(evt_item_entry, STRING("i"), 280, 528, 0, 110, 0, GSWF(2825), 0)
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 88)
        IF_INT_LESS_OR_EQUAL(GSW(0), 112)
            IF_INT_EQUAL(GSWF(2826), 1)
                CALL_CHILD(odoriko_1_init)
                CALL_CHILD(odoriko_2_init)
                CALL_CHILD(odoriko_3_init)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 73)
        SPAWN_EVENT(first_bigtree)
    END_IF
    IF_INT_LESS_OR_EQUAL(GSW(0), 87)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_non"), 1)
        CALL(evt_mri_cloud_init)
        CALL(evt_mri_cloud_ent, PTR(capturemap), STRING("A_kaze"), PTR(open_bigtree))
        SET_VALUE(LW(0), STRING("ne_bero"))
        CALL_CHILD(bero_case_switch_off)
    ELSE
        CALL(evt_map_playanim, STRING("light"), 2, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tree_futa"), 1)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kesu"), 1)
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami1"))
            SPAWN_EVENT(sekai_yami_evt)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SPAWN_EVENT(minnnanokoe_evt)
        END_IF
    END_IF
    RETURN
EVT_END()
// clang-format on

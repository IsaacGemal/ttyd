#include "mri_puni.h"
#include "mri_17.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/imgdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_mri_get16bitdata[];
extern s32 mri_countdown[];
extern s32 mri_guide_goryu[];
extern s32 mri_puni_init[];
extern s32 mri_puni_walk_se[];
extern s32 mri_punipuni_itemremove[];
extern s32 mri_punipuni_itemselect[];
extern s32 mri_punipuni_itemset[];
extern s32 mri_punipuni_setup[];
extern s32 puni_disp_info[];
static USER_FUNC(set_depthPlaneOffset);
s32 strange_statue[56];

// clang-format off
extend_data(116,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(117,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(bero_02_into_event)
    IF_INT_EQUAL(GSWF(2872), 0)
        SPAWN_EVENT(strange_statue)
        SET_VALUE(GSWF(2872), 1)
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {
        "w_bero",
        0, 0, 0x4E20, 0x186A0,
        0, 0, -1,
        NULL, -1, NULL,
        "mri_12", "e_bero_2",
        1, 1, 0, 0,
    },
    {
        "dokan1",
        2, 0, 9, 0x186A0,
        0, 0, -1,
        NULL, 6, NULL,
        NULL, "dokan2",
        1, 1, 0, 0,
    },
    {
        "dokan2",
        2, 0, 2, 0x186A0,
        0, 0, -1,
        bero_02_into_event, 0xA, NULL,
        NULL, "dokan1",
        1, 1, 0, 0,
    },
    {
        NULL,
        0, 0, 0, 0,
        0, 0, 0,
        NULL, 0, NULL,
        NULL, NULL,
        0, 0, 0, 0,
    },
};

static EVT_BEGIN(guide_init)
    RETURN
EVT_END()

static EVT_BEGIN(guide_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_06"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(sister_init)
    RETURN
EVT_END()

static EVT_BEGIN(sister_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_07"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(rival_init)
    RETURN
EVT_END()

static EVT_BEGIN(rival_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_08"), 0, STRING("me"))
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {
        "ガイド", 0x600, 0,
        guide_init, NULL, guide_talk,
        NULL, NULL, NULL,
        NULL, NULL,
        0, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f, 0.0f,
        0,
    },
    {
        "ガイド妹", 0x600, 0,
        sister_init, NULL, sister_talk,
        NULL, NULL, NULL,
        NULL, NULL,
        0, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f, 0.0f,
        0,
    },
    {
        "ライバル", 0x600, 0,
        rival_init, NULL, rival_talk,
        NULL, NULL, NULL,
        NULL, NULL,
        0, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f, 0.0f,
        0,
    },
    {
        NULL, 0, 0,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL,
        0, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f, 0.0f,
        0,
    },
};

static KusaAccessEntry ueki_access_data[] = {
    { 1, "A_f1_ueki01", "S_f1_ueki01", NULL },
    { 1, "A_f1_ueki02", "S_f1_ueki02", NULL },
    { 1, "A_f1_ueki03", "S_f1_ueki03", NULL },
    { 4, NULL, NULL, NULL },
};

// TODO: type
u32 trap_position[102] = {
    0x0045FFEA, 0x0046FFF7, 0x0040FFF5, 0x002EFFDE,
    0x0027FFD3, 0x0038FFE1, 0x0040FFF1, 0x0027FFFA,
    0x0044FFE7, 0x003EFFE4, 0x001FFFED, 0x0023FFF7,
    0x002DFFCF, 0x003CFFF6, 0x001EFFE9, 0x002BFFF4,
    0x0046FFE4, 0x001FFFE1, 0x0019FFED, 0x0015FFE6,
    0x003DFFF3, 0x0037FFED, 0x0039FFD3, 0x002DFFF6,
    0x00300006, 0x0034FFE9, 0x003EFFD2, 0x0038FFDA,
    0x001EFFDA, 0x0025FFE2, 0x0024FFFE, 0x001CFFE3,
    0x001FFFD0, 0x003BFFEB, 0x0023FFF0, 0x004DFFDF,
    0x0039FFD1, 0x002EFFDA, 0x0029FFE9, 0x001FFFDB,
    0x0027FFD2, 0x0034FFF1, 0x0022FFD9, 0x0044FFEA,
    0x003FFFFB, 0x0043FFDC, 0x002DFFE3, 0x0031FFD0,
    0x004CFFDB, 0x0024FFF2, 0x0033FFC7, 0x0011FFE3,
    0x0037FFF7, 0x0049FFD8, 0x003AFFFD, 0x003CFFC3,
    0x0022FFD9, 0x0045FFEE, 0x0045FFDF, 0x0031FFEB,
    0x0035FFF9, 0x0037FFCC, 0x0021FFEF, 0x0033FFE1,
    0x0051FFF5, 0x0030FFF7, 0x0036FFCC, 0x002FFFFF,
    0x003AFFE6, 0x0042FFD3, 0x001EFFE7, 0x002DFFEE,
    0x002EFFE7, 0x002BFFD2, 0x0029FFF6, 0x003DFFD4,
    0x0029FFE4, 0x002BFFCD, 0x0058FFE5, 0x0032FFD8,
    0x0033FFDC, 0x0032FFF7, 0x0021FFCB, 0x0026FFEB,
    0x001EFFD3, 0x0029FFCE, 0x0040FFD8, 0x001FFFDF,
    0x0044FFF5, 0x0043FFD0, 0x003BFFDE, 0x0041FFD8,
    0x0038FFF9, 0x0045FFE1, 0x002EFFD3, 0x004CFFE6,
    0x0035FFE5, 0x0041FFEE, 0x001AFFF5, 0x0044FFCB,
    0x0029FFDD, 0x0031FFC6,
};

EVT_BEGIN(trap_main)
    CALL(evt_puni_wait_systemmode, 2)
    SET_VALUE(LW(10), 1)
    SET_VALUE(LW(11), 0)
    LOOP(0)
        CALL(evt_mri_get16bitdata, PTR(trap_position), LW(11), LW(0))
        ADD(LW(11), 1)
        CALL(evt_mri_get16bitdata, PTR(trap_position), LW(11), LW(2))
        ADD(LW(11), 1)
        CALL(evt_puni_get_trap_index, LW(10), LW(12))
        IF_INT_EQUAL(LW(12), 1)
            CALL(evt_puni_init_puni, LW(10), 24, LW(0), 0, LW(2), 0)
            CALL(evt_puni_flag_onoff, LW(10), 0, 10485760)
            IF_INT_EQUAL(LW(10), 1)
                CALL(evt_puni_flag_onoff, LW(10), 0, 2048)
                CALL(evt_puni_set_position, LW(10), 39, 0, 14)
                CALL(evt_npc_set_position, STRING("ガイド"), 39, 0, 14)
            END_IF
            IF_INT_EQUAL(LW(10), 2)
                CALL(evt_puni_flag_onoff, LW(10), 0, 2048)
                CALL(evt_puni_set_position, LW(10), 113, 0, 17)
                CALL(evt_npc_set_position, STRING("ガイド妹"), 113, 0, 17)
            END_IF
            IF_INT_EQUAL(LW(10), 4)
                CALL(evt_puni_flag_onoff, LW(10), 0, 2048)
                CALL(evt_puni_set_position, LW(10), 120, 0, -50)
                CALL(evt_npc_set_position, STRING("ライバル"), 120, 0, -50)
            END_IF
        END_IF
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_mri_0005be38)
    CALL(evt_puni_wait_systemmode, 2)
    SET_VALUE(LW(10), 1)
    SET_VALUE(LW(11), 0)
    LOOP(0)
        CALL(evt_mri_get16bitdata, PTR(trap_position), LW(11), LW(0))
        ADD(LW(11), 1)
        CALL(evt_mri_get16bitdata, PTR(trap_position), LW(11), LW(2))
        ADD(LW(11), 1)
        CALL(evt_puni_get_trap_index, LW(10), LW(12))
        IF_INT_EQUAL(LW(12), 1)
            CALL(evt_puni_init_puni, LW(10), 16, LW(0), 0, LW(2), 0)
            CALL(evt_puni_flag_onoff, LW(10), 0, 8388608)
        END_IF
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(wana_kemuri)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, -18, 25, 76, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, 5, 10, 70, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, 23, 23, 77, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, 41, 0, 70, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, 66, 9, 72, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, 85, 14, 70, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, 103, 2, 78, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, 0, STRING("kemuri_test"), 0, 124, 15, 70, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

static USER_FUNC(set_depthPlaneOffset) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);
    f32 offset = evtGetFloat(event, args[1]);

    entry->depthPlaneOffset = offset;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(wana_offscreen)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_batan"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_hako"), STRING("offscreen"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 1, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF2_DOOR_GRID_SHUT1"), 50, 0, 0, 0)
        WAIT_MS(1750)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOOR_GRID_DOWN1"), 50, 0, 0, 0)
        SPAWN_EVENT(wana_kemuri)
        CALL(evt_cam_shake, 4, FLOAT(0.03f), FLOAT(0.0f), 500)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_hako"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako"), 1)
    CALL(evt_hitobj_onoff, STRING("A_hako"), 1, 1)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_saku"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_dokan1"), STRING("offscreen"))
    CALL(evt_offscreen_get_boundingbox, STRING("offscreen"), LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, 0, 1, LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_clear_virtual_point, STRING("capture_img"))
    CALL(set_depthPlaneOffset, STRING("capture_img"), FLOAT(-4.0f))
    CALL(evt_img_set_shadow, STRING("capture_img"), 2)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_hit_get_position, STRING("A_dokan1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF2_DOKAN_APPEAR1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF2_DOKAN_APPEAR2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_2"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_dokan1"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_dokan1"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_dokan1"), 1)
    CALL(evt_hitobj_onoff, STRING("A_dokan1"), 1, 1)
    RETURN
EVT_END()

EVT_BEGIN(trap_event)
    WAIT_WHILE(GF(15))
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1073742368)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1073742368)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1073742368)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 80, 151, 555, 80, 56, 13, 500, 11)
    WAIT_MS(500)
    CALL_CHILD(wana_offscreen)
    CALL_CHILD(mri_guide_goryu)
    CALL(evt_target_flag_onoff, 0, 1, 1)
    SET_VALUE(LW(10), 1)
    LOOP(0)
        CALL(evt_puni_set_mode, LW(10), 24)
        CALL(evt_puni_flag_onoff, LW(10), 0, 2097152)
        CALL(evt_puni_set_anim, LW(10), 0)
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_puni_flag_onoff, 1, 0, 262144)
    CALL(evt_puni_set_mode, 1, 30)
    CALL(evt_puni_get_position, 1, LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073743392)
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_glide_position, STRING("ガイド"), 12, 6, 8, 0, FLOAT(120.0f), 0, 0, 0)
    DELETE_EVENT(LW(9))
    CALL(evt_cam3d_evt_set, 13, 110, 326, 13, 56, 20, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_01"), 0, STRING("ガイド"))
    CALL(evt_msg_toge, 2, 5, 230, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_3RD_ENEMY"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_02"), 0, 0)
    CALL(evt_cam3d_evt_set, 50, 164, 624, 50, 56, 7, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 10, 180, 90)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 90, 180, 90)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 50, 180, 90)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ａ"), 90)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ｂ"), 270)
    CALL(evt_npc_sound_data_set, STRING("第三勢力部下Ａ"), 0, 0, 0, 0, 0)
    CALL(evt_npc_sound_data_set, STRING("第三勢力部下Ｂ"), 0, 0, 0, 0, 0)
    CALL(evt_npc_sound_data_set, STRING("第三勢力幹部"), 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_npc_set_anim, STRING("第三勢力部下Ａ"), STRING("R_1"))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_3RD_FALL1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_set_anim, STRING("第三勢力部下Ａ"), STRING("S_1"))
        CALL(evt_npc_glide_position, STRING("第三勢力部下Ａ"), 10, 0, 90, 500, 0, 0, 2, 0)
        CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 10, 0, 90)
        CALL(evt_npc_get_position, STRING("第三勢力部下Ａ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_3RD_LANDING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_set_anim, STRING("第三勢力部下Ａ"), STRING("S_1"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_npc_set_anim, STRING("第三勢力部下Ｂ"), STRING("R_1"))
        CALL(evt_npc_get_position, STRING("第三勢力部下Ｂ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_3RD_FALL1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_glide_position, STRING("第三勢力部下Ｂ"), 90, 0, 90, 500, 0, 0, 2, 0)
        CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 90, 0, 90)
        CALL(evt_npc_get_position, STRING("第三勢力部下Ｂ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_3RD_LANDING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_set_anim, STRING("第三勢力部下Ｂ"), STRING("S_1"))
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("第三勢力幹部"), STRING("R_1"))
    CALL(evt_npc_get_position, STRING("第三勢力幹部"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_KANBU_FALL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_glide_position, STRING("第三勢力幹部"), 50, 0, 90, 500, 0, 0, 2, 0)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 50, 0, 90)
    CALL(evt_npc_get_position, STRING("第三勢力幹部"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_KANBU_LANDING1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("第三勢力幹部"), STRING("S_1"))
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.007f), 500)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 50, 8, FLOAT(120.0f))
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1024)
    CALL(evt_npc_sound_data_reset, STRING("第三勢力部下Ａ"))
    CALL(evt_npc_sound_data_reset, STRING("第三勢力部下Ｂ"))
    CALL(evt_npc_sound_data_reset, STRING("第三勢力幹部"))
    CALL(evt_cam3d_evt_set, 50, 121, 451, 50, 43, 7, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_03"), 0, STRING("第三勢力幹部"))
    CALL(evt_cam3d_evt_set, 50, 151, 548, 50, 56, 7, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 270)
    WAIT_MS(200)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_set_ry, STRING("第三勢力部下Ａ"), 90)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_04"), 0, STRING("第三勢力幹部"))
    WAIT_MS(500)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, -58, 151, 548, -58, 56, 7, 1300, 11)
        WAIT_MS(1300)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(10), STRING("第三勢力部下Ａ"))
        CALL(evt_npc_set_ry, LW(10), 270)
        CALL(evt_npc_move_position, LW(10), -84, 66, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_move_position, LW(10), -200, -5, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_set_position, LW(10), 0, -1000, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(10), STRING("第三勢力部下Ｂ"))
        CALL(evt_npc_set_ry, LW(10), 270)
        CALL(evt_npc_move_position, LW(10), -84, 66, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_move_position, LW(10), -200, -5, 0, FLOAT(120.0f), 0)
        CALL(evt_npc_set_position, LW(10), 0, -1000, 0)
    END_INLINE_EVENT
    SET_VALUE(LW(10), STRING("第三勢力幹部"))
    CALL(evt_npc_set_ry, LW(10), 270)
    CALL(evt_npc_move_position, LW(10), -84, 66, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_move_position, LW(10), -200, -5, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_set_position, LW(10), 0, -1000, 0)
    WAIT_WHILE(GF(0))
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_cam3d_evt_set, 34, 122, 390, 34, 56, 14, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("ガイド"), 90)
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_05"), 0, STRING("ガイド"))
    CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_puni_flag_onoff, 1, 1, 262144)
    CALL(evt_puni_set_position, 1, LW(0), LW(1), LW(2))
    CALL(evt_puni_set_mode, 1, 24)
    SET_VALUE(GSW(0), 102)
    SET_VALUE(GF(1), 0)
    CALL(evt_cam3d_evt_off, 300, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(2), 73)
            BREAK_LOOP
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(2), -105)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print_party, STRING("stg2_mri_e18_00_09"))
    CALL(evt_puni_get_position, 1, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_12"), 0, STRING("ガイド"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
    RETURN
EVT_END()

EVT_BEGIN(trap_check)
    SET_VALUE(LW(10), 50)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), -25)
    SET_VALUE(LW(13), 70)
    CALL(evt_puni_wait_systemmode, 2)
    LOOP(0)
        CALL(evt_puni_count_area, LW(10), LW(11), LW(12), LW(13), LW(8))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_area_check, 1, LW(10), LW(12), LW(13), LW(0), LW(2), LW(9))
        IF_INT_GREATER_OR_EQUAL(LW(11), 5)
            SET_VALUE(LW(9), 0)
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(11), -5)
            SET_VALUE(LW(9), 0)
        END_IF
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_area_check, 1, LW(10), LW(12), LW(13), LW(0), LW(2), LW(14))
        IF_INT_GREATER_OR_EQUAL(LW(11), 5)
            SET_VALUE(LW(14), 0)
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(11), -5)
            SET_VALUE(LW(14), 0)
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(8), 10)
            IF_INT_EQUAL(GSWF(2870), 1)
                IF_INT_EQUAL(LW(9), 1)
                    IF_INT_EQUAL(LW(14), 1)
                        BREAK_LOOP
                    END_IF
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SPAWN_EVENT(trap_event)
    SET_VALUE(GF(1), 1)
    WAIT_WHILE(GF(1))
    SET_VALUE(LW(8), 1)
    LOOP(0)
        CALL(evt_puni_get_flag, LW(8), LW(9))
        AND(LW(9), 1024)
        IF_INT_NOT_EQUAL(LW(9), 0)
            CALL(evt_puni_get_position, LW(8), LW(0), LW(1), LW(2))
            SET_VALUE(LW(9), 0)
            IF_INT_GREATER(LW(0), -50)
                IF_INT_LESS(LW(0), 160)
                    IF_INT_GREATER(LW(2), -100)
                        IF_INT_LESS(LW(2), 70)
                            IF_INT_LESS(LW(1), 10)
                                IF_INT_GREATER(LW(1), -5)
                                    SET_VALUE(LW(9), 1)
                                END_IF
                            END_IF
                        END_IF
                    END_IF
                END_IF
            END_IF
            IF_INT_EQUAL(LW(9), 1)
                CALL(evt_puni_flag_onoff, LW(8), 0, 8388608)
                CALL(evt_puni_set_trap_index, LW(8), 1)
            END_IF
        END_IF
        ADD(LW(8), 1)
        IF_INT_EQUAL(LW(8), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(punipuni_ball00_init)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sui_hikari"), 1)
    CALL_CHILD(mri_punipuni_setup)
    IF_INT_EQUAL(GSWF(2870), 1)
        CALL(evt_item_set_position, LW(10), 50, 30, -58)
        CALL(evt_target_flag_onoff, LW(11), 0, 3)
    END_IF
    CALL(evt_target_set_position, LW(11), 50, 0, -15)
    SET_VALUE(GF(15), 0)
    RETURN
EVT_END()

static EVT_BEGIN(punipuni_ball00)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(GSWF(2870), 0)
        CALL_CHILD(mri_punipuni_itemselect)
        IF_INT_EQUAL(LW(1), 0)
            GOTO(100)
        END_IF
        SET_VALUE(GSWF(2870), 1)
        CALL(evt_item_set_position, LW(10), 50, 30, -58)
        SET_VALUE(GF(15), 1)
        CALL_CHILD(mri_punipuni_itemset)
        SET_VALUE(GF(15), 0)
    ELSE
        SET_VALUE(GSWF(2870), 0)
        CALL_CHILD(mri_punipuni_itemremove)
    END_IF
    LABEL(100)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(breakyuka_init)
    IF_INT_EQUAL(GSWF(2871), 0)
        CALL(evt_hitobj_onoff, STRING("A_f2_rak_01"), 1, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(breakyuka_main)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_hitobj_onoff, STRING("A_f2_rak_01"), 1, 1)
    CALL(evt_puni_all_fall_check)
    CALL(evt_mario_wait_landon)
    CALL(evt_puni_get_position, 1, LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e18_00_13"), 0, STRING("ガイド"))
    SET_VALUE(LW(10), 1)
    SET_VALUE(LW(11), 2)
    SET_VALUE(LW(12), 4)
    CALL(evt_puni_flag_onoff, LW(10), 1, 2048)
    CALL(evt_npc_set_position, STRING("ガイド"), 0, -1000, 0)
    CALL(evt_puni_get_trap_index, LW(11), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_puni_flag_onoff, LW(11), 1, 2048)
        CALL(evt_npc_set_position, STRING("ガイド妹"), 0, -1000, 0)
    END_IF
    CALL(evt_puni_get_trap_index, LW(12), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_puni_flag_onoff, LW(12), 1, 2048)
        CALL(evt_npc_set_position, STRING("ライバル"), 0, -1000, 0)
    END_IF
    SET_VALUE(LW(10), 1)
    LOOP(0)
        CALL(evt_puni_get_trap_index, LW(10), LW(12))
        IF_INT_EQUAL(LW(12), 1)
            CALL(evt_puni_set_mode, LW(10), 16)
            CALL(evt_puni_flag_onoff, LW(10), 1, 2097152)
        END_IF
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GSW(0), 106)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
    RETURN
EVT_END()

EVT_BEGIN(strange_statue)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_party_stop, 0)
    WAIT_MS(500)
    CALL(evt_party_move_pos2, 0, 50, -50, FLOAT(120.0f))
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(300)
    CALL(evt_party_set_dir, 0, 90, 200)
    WAIT_MS(400)
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(350)
    CALL(evt_party_set_dir, 0, 90, 200)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("stg2_mri_e18_00_11"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(switch_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_wait_landon)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_STG2_SWITCH_QUAKE1"), LW(10))
    CALL(evt_cam_shake, 4, FLOAT(0.015f), FLOAT(0.0f), 1000)
    CALL(evt_snd_sfxoff, LW(10))
    WAIT_MS(500)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print_party, STRING("stg2_mri_e18_00_10"))
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 103)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(mri_17_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 17, 0, -309, -1000, -1000, 6, 0)
    CALL(evt_puni_set_hamari, -128, 0, 94, 7, -3, -307, -311)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    CALL_CHILD(mri_puni_init)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT(puni_disp_info)
    SPAWN_EVENT(mri_countdown)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_map_playanim, STRING("mizu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu_02"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_02"), 1, 0)
    CALL(evt_map_playanim, STRING("sui_hikari"), 1, 0)
    SPAWN_EVENT(mri_countdown)
    SET_VALUE(LW(0), PTR(ueki_access_data))
    CALL_CHILD(evt_sub_kusa_access_entry)
    SWITCH(GSW(0))
        CASE_BETWEEN(109, 110)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI9"))
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI4"))
    END_SWITCH
    SET_VALUE(LW(10), STRING("punipuni"))
    SET_VALUE(LW(11), 0)
    SPAWN_EVENT(punipuni_ball00_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_dai_mae"), 0, PTR(punipuni_ball00), 0)
    SPAWN_EVENT(breakyuka_init)
    CALL(evt_mobj_breaking_floor, STRING("mobj_bf00"), 0, 0, -35, 1, PTR(breakyuka_main), GSWF(2871))
    IF_INT_LESS(GSW(0), 103)
        CALL(evt_mobj_switch_blue, 1, STRING("switch"), 60, -309, 53, PTR(switch_event), 0)
    END_IF
    CALL(evt_item_entry, STRING("item_01"), 125, 168, -310, -44, 16, GSWF(5617), 0)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(101)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hako"), 1)
        CALL(evt_hitobj_onoff, STRING("A_hako"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_dokan1"), 1)
        CALL(evt_hitobj_onoff, STRING("A_dokan1"), 1, 0)
        CALL(evt_paper_entry, STRING("OFF_d_batan"))
        CALL(evt_paper_entry, STRING("OFF_d_saku"))
        SPAWN_EVENT(trap_check)
        CASE_BETWEEN(102, 105)
        SPAWN_EVENT(trap_main)
        CASE_BETWEEN(106, 107)
        SPAWN_EVENT(unk_evt_mri_0005be38)
        CASE_GREATER_OR_EQUAL(106)
    END_SWITCH
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_saku_mae"), 2)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
